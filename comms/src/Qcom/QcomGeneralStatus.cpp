#include "Core.hpp"
#include "Utils.hpp"

#include "Qcom/QcomBroadcast.hpp"
#include "Qcom/QcomGeneralStatus.hpp"
#include "Qcom/qogr/qogr_crc.h"


namespace sg 
{

    uint8_t QcomGeneralStatus::Id() const
    {
        return QCOM_GSP_FC;
    }

    uint8_t QcomGeneralStatus::RespId() const
    {
        return QCOM_GSR_FC;
    }

    bool QcomGeneralStatus::Parse(uint8_t buf[], int length)
    {
        SG_UNREF_PARAM(length);

        if (auto it = m_qcom.lock())
        {
            QCOM_RespMsgType *p = (QCOM_RespMsgType*)buf;
            if (p->DLL.Length >= QCOM_GET_PACKET_LENGTH(sizeof(qc_gsrtype)))
            {
                QcomDataPtr pd = it->GetEgmData(p->DLL.PollAddress);

                if (pd)
                {
                    std::unique_lock<std::mutex> lock(pd->locker);

                    pd->data.control.last_control ^= (QCOM_ACK_MASK);

                    pd->data.status.flag_a = p->Data.gsr.FLGA.FLGA;
                    pd->data.status.flag_b = p->Data.gsr.FLGB.FLGB;
                    pd->data.status.state = p->Data.gsr.STATE;

                    return true;
                }
            }
        }

        return false;
    }

    QcomPollPtr QcomGeneralStatus::MakeGeneralStatusPoll(uint8_t poll_address, uint8_t last_control)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

        std::memset(poll.get(), 0, sizeof(QcomPoll));


        // ref Qcom1.6-15.4.2
        poll->poll.DLL.PollAddress = poll_address;
        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE;
        poll->poll.DLL.ControlByte.CNTL = last_control;
        poll->poll.DLL.FunctionCode = QCOM_GSP_FC;
        PutCRC_LSBfirst(poll->data, poll->poll.DLL.Length);
        poll->length = poll->poll.DLL.Length + QCOM_CRC_SIZE;

        return poll;

    }


    bool QcomGeneralStatus::BuildGeneralStatusPoll(QcomJobDataPtr &job, uint8_t poll_address)
    {
        if (auto it = m_qcom.lock())
        {
            QcomDataPtr p = it->GetEgmData(poll_address);
            if (p)
            {
                std::unique_lock<std::mutex> lock(p->locker);

                job->AddPoll(this->MakeGeneralStatusPoll(poll_address, p->data.control.last_control));

                return true;
            }
        }

        return false;
    }

    bool QcomGeneralStatus::BuildGeneralStatusPoll(QcomJobDataPtr &job)
    {
        if (auto it = m_qcom.lock())
        {
            std::vector<QcomDataPtr> egmDatas;
            it->CaptureEGMData(egmDatas);

            size_t size = egmDatas.size();
            if (!size)
                return false;

            for (uint8_t i = 0; i < size; ++i)
            {
                QcomDataPtr p = egmDatas[i];

                if (p)
                {
                    std::unique_lock<std::mutex> lock(p->locker);

                    if (p->data.control.poll_address)
                        job->AddPoll(this->MakeGeneralStatusPoll(p->data.control.poll_address, p->data.control.last_control));
                }
            }

            if (job->GetPollNum())
                return true;
        }

        return false;
    }

}


