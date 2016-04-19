#include "core/core.hpp"
#include "core/core_utils.hpp"

#include "comms/qcom/qcom_broadcast.hpp"
#include "comms/qcom/qcom_general_status.hpp"
#include "comms/qcom/qogr/qogr_crc.h"


namespace sg {

uint8_t QcomGeneralStatus::Id() const
{
    return QCOM_GSP_FC;
}

bool QcomGeneralStatus::Parse(uint8_t buf[], int length)
{
    CORE_UNREF_PARAM(buf);
    CORE_UNREF_PARAM(length);
    return true;
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


void QcomGeneralStatus::BuildGeneralStatusPoll(uint8_t poll_address)
{
    if (CORE_AUTO(it, m_qcom.lock()))
    {
        QcomDataPtr p = it->GetEgmData(poll_address);
        if (p)
        {
            QcomJobDataPtr job = MakeSharedPtr<QcomJobData>(QcomJobData::JT_POLL);

            unique_lock<shared_mutex> lock(p->locker);

            if (p->data.poll_address == 0)
            {
                QcomBroadcastPtr pb = static_pointer_cast<QcomBroadcast>(it->GetHandler(QCOM_BROADCAST_ADDRESS));
                if (pb)
                {
                    pb->BuildPollAddressPoll();
                }
            }

            if (p->data.resp_funcode == QCOM_NO_RESPONSE)
                p->data.last_control ^= (QCOM_ACK_MASK);

            job->AddPoll(MakeGeneralStatusPoll(poll_address, p->data.last_control));

            p->data.resp_funcode = QCOM_NO_RESPONSE;

            it->AddJob(job);
        }
    }

}

QcomJobDataPtr QcomGeneralStatus::MakeGeneralStatusJob()
{
    if (CORE_AUTO(it, m_qcom.lock()))
    {
        std::vector<QcomDataPtr> egmDatas;
        it->GetEgmData(egmDatas);
        size_t size = egmDatas.size();

        if(!size)
        {
            return nullptr;
        }

        QcomJobDataPtr job = MakeSharedPtr<QcomJobData>(QcomJobData::JT_POLL);

        for ( uint8_t i = 0; i < size; ++i )
        {
            QcomDataPtr p = it->GetEgmData(i + 1);

            if (p)
            {
                if (p->data.resp_funcode == QCOM_NO_RESPONSE)
                    p->data.last_control ^= (QCOM_ACK_MASK);

                job->AddPoll(MakeGeneralStatusPoll(i + 1, p->data.last_control));

                p->data.resp_funcode = QCOM_NO_RESPONSE;
            }
        }

        return job;
    }

    return nullptr;
}

}


