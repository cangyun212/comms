#include "Core.hpp"
#include "Utils.hpp"

#include "Qcom/QcomECTToEGM.hpp"
#include "Qcom/qogr/qogr_crc.h"


namespace sg 
{

    uint8_t QcomECTToEGM::Id() const
    {
        return QCOM_ECTEGMP_FC;
    }

    uint8_t QcomECTToEGM::RespId() const
    {
        return QCOM_ECTEGMACKR_FC;
    }

    bool QcomECTToEGM::Parse(uint8_t buf[], int length)
    {
        SG_UNREF_PARAM(length);

        if (auto it = m_qcom.lock())
        {
            QCOM_RespMsgType *p = (QCOM_RespMsgType*)buf;
            if (p->DLL.Length >= QCOM_GET_PACKET_LENGTH(sizeof(qc_ectacktype)))
            {
                QcomDataPtr pd = it->GetEgmData(p->DLL.PollAddress);

                uint8_t psn = 0;

                if (pd)
                {
                    std::unique_lock<std::mutex> lock(pd->locker);

                    pd->data.control.last_control ^= (QCOM_ACK_MASK);

                    psn = QcomNextPSN(pd->data.control.psn[Qcom_PSN_ECT]);

                    //pd->data.mgc.groups[1].met[5] = p->Data.ectack.TCCIM;

                    //if (p->Data.ectack.ECTPSN == psn)
                    //    return true;
                }

                if (p->Data.ectack.ECTPSN == psn)
                {
                    COMMS_LOG("ECT To EGM Ack\n", CLL_Info);
                }
                else
                {
                    COMMS_LOG(boost::format("ECT To EGM Ack response received, received psn %|| is not match expected next psn %||\n") %
                        static_cast<uint32_t>(p->Data.ectack.ECTPSN) % static_cast<uint32_t>(psn) , CLL_Error);
                }
            }
            else
            {
                COMMS_LOG(boost::format("ECT To EGM Ack response received, data length %|| is not valid.\n") % 
                    static_cast<uint32_t>(p->DLL.Length), CLL_Error);
            }
        }

        return false;
    }

    QcomPollPtr QcomECTToEGM::MakeECTToEGMPoll(uint8_t poll_address, uint8_t last_control, uint8_t psn, QcomECTToEGMPollData const& data)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

        std::memset(poll.get(), 0, sizeof(QcomPoll));


        // ref Qcom1.6-15.4.2
        poll->poll.DLL.PollAddress = poll_address;
        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + sizeof(qc_ectegmptype2);
        poll->poll.DLL.ControlByte.CNTL = last_control;
        poll->poll.DLL.FunctionCode = QCOM_ECTEGMP_FC;

        poll->poll.Data.ectegmp2.FLG.bits.sourceid = data.id;
        poll->poll.Data.ectegmp2.FLG.bits.cashlessmode = data.cashless;
        poll->poll.Data.ectegmp2.PSN = psn;
        poll->poll.Data.ectegmp2.CAMT = data.eamt;
        //PutCRC_LSBfirst(poll->data, poll->poll.DLL.Length);
        //poll->length = poll->poll.DLL.Length + QCOM_CRC_SIZE;

        return poll;

    }


    bool QcomECTToEGM::BuildECTToEGMPoll(QcomJobDataPtr &job, uint8_t poll_address, QcomECTToEGMPollData const& data)
    {
        if (auto it = m_qcom.lock())
        {
            QcomDataPtr p = it->GetEgmData(poll_address);
            if (p)
            {
                std::unique_lock<std::mutex> lock(p->locker);

                p->data.control.psn[Qcom_PSN_ECT] = QcomNextPSN(p->data.control.psn[Qcom_PSN_ECT]);

                job->AddPoll(this->MakeECTToEGMPoll(poll_address, p->data.control.last_control, p->data.control.psn[Qcom_PSN_ECT], data));

                return true;
            }
        }

        return false;
    }


}


