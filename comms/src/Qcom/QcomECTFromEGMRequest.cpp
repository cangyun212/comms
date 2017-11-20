#include "Core.hpp"
#include "Utils.hpp"

#include <chrono>

#include "Qcom/QcomECTFromEGMRequest.hpp"

namespace sg
{
    uint8_t QcomECTFromEGMRequest::Id() const
    {
        return QCOM_ECTEGMLRP_FC;
    }

    bool QcomECTFromEGMRequest::BuildECTFromEGMRequestPoll(QcomJobDataPtr &job, uint8_t poll_address)
    {
        if (auto it = m_qcom.lock())
        {
            job->AddPoll(this->MakeECTFromEGMRequestPoll(poll_address, 0));

            return true;
        }

        return false;
    }

    QcomPollPtr QcomECTFromEGMRequest::MakeECTFromEGMRequestPoll(uint8_t poll_address, uint8_t last_control)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

        std::memset(poll.get(), 0, sizeof(QcomPoll));

        poll->poll.DLL.PollAddress = poll_address;
        poll->poll.DLL.ControlByte.CNTL = last_control;
        poll->poll.DLL.FunctionCode = QCOM_ECTEGMLRP_FC;

        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE;

        return poll;
    }
}


