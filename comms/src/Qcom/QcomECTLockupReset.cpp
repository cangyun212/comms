#include "Core.hpp"
#include "Utils.hpp"

#include "Qcom/QcomECTLockupReset.hpp"
#include "Qcom/qogr/qogr_crc.h"

namespace sg 
{
    uint8_t QcomECTLockupReset::Id() const
    {
        return QCOM_ECTLRP_FC;
    }

    bool QcomECTLockupReset::BuildECTLockupResetPoll(QcomJobDataPtr &job, uint8_t poll_address, uint8_t denied)
    {
        if (auto it = m_qcom.lock())
        {
            {
                job->AddPoll(this->MakeECTLockupResetPoll(poll_address, 0, denied));

                return true;
            }
        }

        return false;
    }

    QcomPollPtr QcomECTLockupReset::MakeECTLockupResetPoll(uint8_t poll_address, uint8_t last_control, uint8_t denied)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

        std::memset(poll.get(), 0, sizeof(QcomPoll));

        // ref Qcom1.6-15.4.2
        poll->poll.DLL.PollAddress = poll_address;
        poll->poll.DLL.ControlByte.CNTL = last_control;
        poll->poll.DLL.FunctionCode = QCOM_ECTLRP_FC;

        poll->poll.Data.ectlrp.CFUNC.bits.success = denied ? 0 : 1;

        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + sizeof(qc_ectlrptype);
        return poll;
    }

}
