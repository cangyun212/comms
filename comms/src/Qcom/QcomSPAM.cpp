#include "Core.hpp"
#include "Utils.hpp"

#include "Qcom/QcomSPAM.hpp"
#include "Qcom/qogr/qogr_crc.h"

namespace sg 
{
    uint8_t QcomSPAMA::Id() const
    {
        return QCOM_SPAMA_FC;
    }

    uint8_t QcomSPAMB::Id() const
    {
        return QCOM_SPAMB_FC;
    }


    bool QcomSPAMA::BuildSPAMAPoll(QcomJobDataPtr &job, uint8_t poll_address, QcomSPAMPollData const& data)
    {
        if (auto it = m_qcom.lock())
        {
            {
                job->AddPoll(this->MakeSPAMAPoll(poll_address, 0, data));

                return true;
            }
        }

        return false;
    }

    bool QcomSPAMB::BuildSPAMBPoll(QcomJobDataPtr &job, uint8_t poll_address, QcomSPAMPollData const& data)
    {
        if (auto it = m_qcom.lock())
        {
            {
                job->AddPoll(this->MakeSPAMBPoll(poll_address, 0, data));

                return true;
            }
        }

        return false;
    }

    QcomPollPtr QcomSPAMA::MakeSPAMAPoll(uint8_t poll_address, uint8_t last_control, QcomSPAMPollData const& data)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

        std::memset(poll.get(), 0, sizeof(QcomPoll));

        // ref Qcom1.6-15.4.2
        poll->poll.DLL.PollAddress = poll_address;
        poll->poll.DLL.ControlByte.CNTL = last_control;
        poll->poll.DLL.FunctionCode = QCOM_SPAMA_FC;

        poll->poll.Data.spamp.FMT.bits.prom = data.prominence;
        poll->poll.Data.spamp.FMT.bits.fanfare = data.fanfare;
        poll->poll.Data.spamp.LEN = data.len;
        if (data.len > 0)
        {
            std::memcpy(poll->poll.Data.spamp.TEXT, data.text, sizeof(char) * data.len);
        }

        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + sizeof(qc_spamptype) - sizeof(poll->poll.Data.spamp.TEXT) + sizeof(char) * data.len;
        return poll;
    }

    QcomPollPtr QcomSPAMB::MakeSPAMBPoll(uint8_t poll_address, uint8_t last_control, QcomSPAMPollData const& data)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

        std::memset(poll.get(), 0, sizeof(QcomPoll));

        // ref Qcom1.6-15.4.2
        poll->poll.DLL.PollAddress = poll_address;
        poll->poll.DLL.ControlByte.CNTL = last_control;
        poll->poll.DLL.FunctionCode = QCOM_SPAMB_FC;

        poll->poll.Data.spamp.FMT.bits.prom = data.prominence;
        poll->poll.Data.spamp.FMT.bits.fanfare = data.fanfare;
        poll->poll.Data.spamp.LEN = data.len;
        if (data.len > 0)
        {
            std::memcpy(poll->poll.Data.spamp.TEXT, data.text, sizeof(char) * data.len);
        }

        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + sizeof(qc_spamptype) - sizeof(poll->poll.Data.spamp.TEXT) + sizeof(char) * data.len;
        return poll;
    }
}
