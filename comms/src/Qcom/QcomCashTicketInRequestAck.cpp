#include "Core.hpp"
#include "Utils.hpp"

#include <chrono>

#include "Qcom/QcomCashTicketInRequestAck.hpp"
#include "Qcom/QcomInline.h"

namespace sg
{
    uint8_t QcomCashTicketInRequestAck::Id() const
    {
        return QCOM_TIRACKP_FC;
    }

    bool QcomCashTicketInRequestAck::BuildCashTicketInRequestAckPoll(QcomJobDataPtr &job, uint8_t poll_address, QcomCashTicketInRequestAckPollData const& data)
    {
        if (auto it = m_qcom.lock())
        {
            job->AddPoll(this->MakeCashTicketInRequestAckPoll(poll_address, 0, data));

            return true;
        }

        return false;
    }

    QcomPollPtr QcomCashTicketInRequestAck::MakeCashTicketInRequestAckPoll(uint8_t poll_address, uint8_t last_control, QcomCashTicketInRequestAckPollData const& data)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

        std::memset(poll.get(), 0, sizeof(QcomPoll));

        poll->poll.DLL.PollAddress = poll_address;
        poll->poll.DLL.ControlByte.CNTL = last_control;
        poll->poll.DLL.FunctionCode = QCOM_TIRACKP_FC;

        poll->poll.Data.tirackp.FCODE = data.fcode;
        poll->poll.Data.tirackp.TAMT = data.amount;

        std::memcpy(poll->poll.Data.tirackp.authno.AUTHNO, data.authno, sizeof(data.authno));

        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + sizeof(qc_tirackptype);

        return poll;
    }
}


