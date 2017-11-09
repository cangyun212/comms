#include "Core.hpp"
#include "Utils.hpp"

#include "Qcom/qogr/qogr_crc.h"
#include "Qcom/QcomGeneralReset.hpp"

namespace sg 
{
    uint8_t QcomGeneralReset::Id() const
    {
        return QCOM_EGMGRP_FC;
    }

    bool QcomGeneralReset::BuildGeneralResetPoll(QcomJobDataPtr &job, uint8_t poll_address, QcomGeneralResetPollData const& data)
    {
        if (auto it = m_qcom.lock())
        {
            {
                job->AddPoll(this->MakeGeneralResetPoll(poll_address, 0, data));

                return true;
            }
        }

        return false;
    }

    QcomPollPtr QcomGeneralReset::MakeGeneralResetPoll(uint8_t poll_address, uint8_t last_control, QcomGeneralResetPollData const& data)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

        std::memset(poll.get(), 0, sizeof(QcomPoll));

        // ref Qcom1.6-15.4.2
        poll->poll.DLL.PollAddress = poll_address;
        poll->poll.DLL.ControlByte.CNTL = last_control;
        poll->poll.DLL.FunctionCode = QCOM_EGMGRP_FC;

        poll->poll.Data.egmgrp2.FLG.bits.fault = data.fault;
        poll->poll.Data.egmgrp2.FLG.bits.lockup = data.lockup;
        poll->poll.Data.egmgrp2.STATE = data.state;

        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + sizeof(qc_egmgrptype2);
        return poll;
    }

}
