
#include "Core.hpp"
#include "Utils.hpp"

#include "Qcom/QcomNoteAcceptorMaintenance.hpp"
#include "Qcom/qogr/qogr_crc.h"



namespace sg 
{

    uint8_t QcomNoteAcceptorMaintenance::Id() const
    {
        return QCOM_NAMP_FC;
    }

    uint8_t QcomNoteAcceptorMaintenance::RespId() const
    {
        return QCOM_NASR_FC;
    }

    bool QcomNoteAcceptorMaintenance::Parse(uint8_t buf[], int length)
    {
        SG_UNREF_PARAM(length);

        if (auto it = m_qcom.lock())
        {
            QCOM_RespMsgType *p = (QCOM_RespMsgType*)buf;
            if (p->DLL.Length >= QCOM_GET_PACKET_LENGTH(sizeof(qc_nasrtype)))
            {
                QcomDataPtr pd = it->GetEgmData(p->DLL.PollAddress);

                uint8_t iflag = 0;
                if (pd)
                {
                    std::unique_lock<std::mutex> _lock(pd->locker);

                    pd->data.control.last_control ^= (QCOM_ACK_MASK);

                    // verify this field
                    if (pd->data.control.egm_config_state & QCOM_EGM_NAM_SET)
                    {
                        if (pd->data.nasr.nam.five != p->Data.nasr.NAFLG.bits.five)
                            iflag &= 0x1 << 0;
                        if (pd->data.nasr.nam.ten != p->Data.nasr.NAFLG.bits.ten)
                            iflag &= 0x1 << 1;
                        if (pd->data.nasr.nam.twenty != p->Data.nasr.NAFLG.bits.twenty)
                            iflag &= 0x1 << 2;
                        if (pd->data.nasr.nam.fifty != p->Data.nasr.NAFLG.bits.fifty)
                            iflag &= 0x1 << 3;
                        if (pd->data.nasr.nam.hundred != p->Data.nasr.NAFLG.bits.hundred)
                            iflag &= 0x1 << 4;
                        pd->data.control.egm_config_state &= ~QCOM_EGM_NAM_SET;
                    }

                    pd->data.nasr.nam.five = p->Data.nasr.NAFLG.bits.five;
                    pd->data.nasr.nam.ten = p->Data.nasr.NAFLG.bits.ten;
                    pd->data.nasr.nam.twenty = p->Data.nasr.NAFLG.bits.twenty;
                    pd->data.nasr.nam.fifty = p->Data.nasr.NAFLG.bits.fifty;
                    pd->data.nasr.nam.hundred = p->Data.nasr.NAFLG.bits.hundred;

                    pd->data.nasr.full = p->Data.nasr.FLGA.bits.full;

                    std::memcpy(pd->data.nasr.nads, p->Data.nasr.NADS, sizeof(pd->data.nasr.nads));
                }

                if (iflag != 0)
                {
                    COMMS_START_LOG_BLOCK();
                    const char* strs[] = { "$5", "$10", "$20", "$50", "$100" };
                    for (size_t i = 0; i < SG_ARRAY_SIZE(strs); ++i)
                    {
                        if (iflag & (0x1 << i))
                        {
                            if (p->Data.nasr.NAFLG.bytes.MSB & (0x1 << i))
                                COMMS_LOG_BLOCK(
                                    boost::format("The EGM banknote acceptor still could accept %|| denom banknote\n") % strs[i], CLL_Warning);
                            else
                                COMMS_LOG_BLOCK(
                                    boost::format("The EGM banknote acceptor couldn't accept %|| denom banknote\n") % strs[i] , CLL_Warning);
                        }
                    }
                    COMMS_END_LOG_BLOCK();
                    return false;
                }

                COMMS_LOG("Note Acceptor Status Response received\n", CLL_Info);
                return true;
            }
            else
            {
                COMMS_LOG(boost::format("Note Acceptor Status Response received, data length %|| is not valid.\n") % 
                    static_cast<uint32_t>(p->DLL.Length), CLL_Error);
            }
        }

        return false;
    }

    bool QcomNoteAcceptorMaintenance::BuildNoteAcceptorMaintenancePoll(QcomJobDataPtr &job, uint8_t poll_address, QcomNoteAcceptorMaintenanceData const & data)
    {
        if (auto it = m_qcom.lock())
        {
            QcomDataPtr p = it->GetEgmData(poll_address);
            if (p)
            {
                std::unique_lock<std::mutex> lock(p->locker);

                job->AddPoll(this->MakeNoteAcceptorMaintenancePoll(poll_address, p->data.control.last_control, data));

                p->data.nasr.nam = data;
                p->data.control.egm_config_state |= QCOM_EGM_NAM_SET;

                return true;
            }
        }

        return false;
    }

    QcomPollPtr QcomNoteAcceptorMaintenance::MakeNoteAcceptorMaintenancePoll(uint8_t poll_address, uint8_t last_control, QcomNoteAcceptorMaintenanceData const& data)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

        std::memset(poll.get(), 0, sizeof(QcomPoll));

        // ref Qcom1.6-15.4.1
        poll->poll.DLL.PollAddress = poll_address;
        poll->poll.DLL.ControlByte.CNTL = last_control;
        poll->poll.DLL.FunctionCode = QCOM_NAMP_FC;

        poll->poll.Data.namp.NAFLG.bits.five = data.five;
        poll->poll.Data.namp.NAFLG.bits.ten = data.ten;
        poll->poll.Data.namp.NAFLG.bits.twenty = data.twenty;
        poll->poll.Data.namp.NAFLG.bits.fifty = data.fifty;
        poll->poll.Data.namp.NAFLG.bits.hundred = data.hundred;

        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + sizeof(qc_namptype);

        return poll;
    }

}


