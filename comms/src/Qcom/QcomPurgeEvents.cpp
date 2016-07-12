#include "Core.hpp"
#include "Utils.hpp"

#include "Qcom/QcomBroadcast.hpp"
#include "Qcom/QcomPurgeEvents.hpp"
#include "Qcom/qogr/qogr_crc.h"

namespace sg 
{
    uint8_t QcomPurgeEvents::Id() const
    {
        return QCOM_PEP_FC;
    }


    uint8_t QcomPurgeEvents::RespId() const 
    {
        return QCOM_PEPAR_FC; 
    }

    bool QcomPurgeEvents::Parse(uint8_t buf[], int length)
    {
        SG_UNREF_PARAM(length);
        if (auto it = m_qcom.lock())
        {
            QCOM_RespMsgType *p = (QCOM_RespMsgType*)buf;
            if (p->DLL.Length >= QCOM_GET_PACKET_LENGTH(sizeof(qc_peptype)))
            {
                QcomDataPtr pd = it->GetEgmData(p->DLL.PollAddress);

                uint8_t psn = 0;

                if (pd)
                {
                    std::unique_lock<std::mutex> lock(pd->locker);

                    pd->data.control.last_control ^= (QCOM_ACK_MASK);

                    psn = QcomNextPSN(pd->data.control.psn[Qcom_PSN_Events]);
                    if (psn == p->Data.pepar.PPSN)
                    {
                        return true;
                    }
                }

                COMMS_LOG(boost::format("Received invalid PSN %1%, PSN %2% is expected.\n") %
                    p->Data.pepar.PPSN % psn, CLL_Error);
            }
        }

        return false;

    }

    bool QcomPurgeEvents::BuildPurgeEventsPoll(QcomJobDataPtr &job, uint8_t poll_address, uint8_t evtno)
    {
        if (auto it = m_qcom.lock())
        {
            QcomDataPtr p = it->GetEgmData(poll_address);
            if (p)
            {
                std::unique_lock<std::mutex> lock(p->locker);

                p->data.control.psn[Qcom_PSN_Events] = QcomNextPSN(p->data.control.psn[Qcom_PSN_Events]);

                job->AddPoll(
                    this->MakePurgeEventsPoll(
                        poll_address, 
                        p->data.control.last_control, 
                        p->data.control.psn[Qcom_PSN_Events], 
                        evtno));

                return true;
            }
        }

        return false;
    }

    QcomPollPtr QcomPurgeEvents::MakePurgeEventsPoll(uint8_t poll_address, uint8_t last_control, uint8_t psn, uint8_t evtno)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

        std::memset(poll.get(), 0, sizeof(QcomPoll));

        poll->poll.DLL.PollAddress = poll_address;
        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + sizeof(qc_peptype);
        poll->poll.DLL.ControlByte.CNTL = last_control;
        poll->poll.DLL.FunctionCode = QCOM_PEP_FC;

        poll->poll.Data.pep.PSN = psn;
        poll->poll.Data.pep.FLG = (uint8_t)0;
        poll->poll.Data.pep.EVTNO = evtno;

        PutCRC_LSBfirst(poll->data, poll->poll.DLL.Length);
        poll->length = poll->poll.DLL.Length + QCOM_CRC_SIZE;

        return poll;
    }
}
