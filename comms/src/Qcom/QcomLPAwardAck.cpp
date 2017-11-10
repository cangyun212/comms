#include "Core.hpp"
#include "Utils.hpp"

#include "Qcom/QcomLPAwardAck.hpp"
#include "Qcom/qogr/qogr_crc.h"

namespace sg 
{
    uint8_t QcomLPAwardAck::Id() const
    {
        return QCOM_LPAAP_FC;
    }

    bool QcomLPAwardAck::BuildLPAwardAckPoll(QcomJobDataPtr &job, uint8_t poll_address)
    {
        if (auto it = m_qcom.lock())
        {
            {
                job->AddPoll(this->MakeLPAwardAckPoll(poll_address, 0));

                return true;
            }
        }

        return false;
    }

    QcomPollPtr QcomLPAwardAck::MakeLPAwardAckPoll(uint8_t poll_address, uint8_t last_control)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

        std::memset(poll.get(), 0, sizeof(QcomPoll));

        // ref Qcom1.6-15.4.2
        poll->poll.DLL.PollAddress = poll_address;
        poll->poll.DLL.ControlByte.CNTL = last_control;
        poll->poll.DLL.FunctionCode = QCOM_LPAAP_FC;

        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE; //+ sizeof(qc_egmcptype2);
        return poll;
    }

}
