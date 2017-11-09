
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

                uint8_t iflag = 0xFF;
                if (pd)
                {
                    std::unique_lock<std::mutex> _lock(pd->locker);

                    pd->data.control.last_control ^= (QCOM_ACK_MASK);

                    // verify this field
                    uint8_t flag = (pd->data.nasr.nam.five << 0) |
                        (pd->data.nasr.nam.ten << 1) |
                        (pd->data.nasr.nam.twenty << 2) |
                        (pd->data.nasr.nam.fifty << 3) |
                        (pd->data.nasr.nam.hundred << 4);

                    if (p->Data.nasr.NAFLG.bytes.MSB != flag)
                    {
                        iflag = flag;
                    }

                    pd->data.nasr.full = p->Data.nasr.FLGA.bits.full;

                    std::memcpy(pd->data.nasr.nads, p->Data.nasr.NADS, sizeof(pd->data.nasr.nads));
                }

                if (iflag != 0xFF)
                {
                    COMMS_LOG(
                        boost::format("Invalid Note Acceptor denomination setting 0x%|02X| received, setting 0x%|02X| is expected\n") %
                        p->Data.nasr.NAFLG.bytes.MSB % iflag, CLL_Error);
                    return false;
                }

                COMMS_LOG("Note Acceptor Status Response received\n", CLL_Info);
                return true;
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


