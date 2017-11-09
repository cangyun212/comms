#include "Core.hpp"

#include "Qcom/QcomRequestAllLoggedEvents.hpp"
#include "Qcom/qogr/qogr_crc.h"

namespace sg 
{
    uint8_t QcomRequestAllLoggedEvents::Id() const
    {
        return QCOM_RALEP_FC;
    }

    bool QcomRequestAllLoggedEvents::BuildRequestAllLoggedEventsPoll(QcomJobDataPtr &job, uint8_t poll_address)
    {
        if (auto it = m_qcom.lock())
        {
            //QcomDataPtr p = it->GetEgmData(poll_address);
            //if (p)
            {
                //std::unique_lock<std::mutex> lock(p->locker);

                job->AddPoll(this->MakeRequestAllLoggedEventsPoll(poll_address, 0));

                return true;
            }
        }

        return false;
    }

    QcomPollPtr QcomRequestAllLoggedEvents::MakeRequestAllLoggedEventsPoll(uint8_t poll_address, uint8_t last_control)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

        std::memset(poll.get(), 0, sizeof(QcomPoll));

        poll->poll.DLL.PollAddress = poll_address;
        poll->poll.DLL.ControlByte.CNTL = last_control;
        poll->poll.DLL.FunctionCode = QCOM_RALEP_FC;

        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE;

        return poll;
    }

}
