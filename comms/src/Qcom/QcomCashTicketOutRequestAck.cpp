#include "Core.hpp"
#include "Utils.hpp"

#include <chrono>

#include "Qcom/QcomCashTicketOutRequestAck.hpp"
#include "Qcom/QcomInline.h"

namespace sg
{
    uint8_t QcomCashTicketOutRequestAck::Id() const
    {
        return QCOM_TORACKP_FC;
    }

    bool QcomCashTicketOutRequestAck::BuildCashTicketOutRequestAckPoll(QcomJobDataPtr &job, uint8_t poll_address, QcomCashTicketOutRequestAckPollData const& data)
    {
        if (auto it = m_qcom.lock())
        {
            job->AddPoll(this->MakeCashTicketOutRequestAckPoll(poll_address, 0, data));

            return true;
        }

        return false;
    }

    QcomPollPtr QcomCashTicketOutRequestAck::MakeCashTicketOutRequestAckPoll(uint8_t poll_address, uint8_t last_control, QcomCashTicketOutRequestAckPollData const& data)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

        std::memset(poll.get(), 0, sizeof(QcomPoll));

        poll->poll.DLL.PollAddress = poll_address;
        poll->poll.DLL.ControlByte.CNTL = last_control;
        poll->poll.DLL.FunctionCode = QCOM_TORACKP_FC;

        poll->poll.Data.torackp.flg.bits.onfail = (data.flag & QCOM_CTO_CANCEL) ? 1 : 0;
        poll->poll.Data.torackp.flg.bits.success = (data.flag & QCOM_CTO_APPROVE) ? 1 : 0;
        poll->poll.Data.torackp.TSER = data.serial;
        poll->poll.Data.torackp.TAMT = data.amount;

        auto now = std::chrono::system_clock::now();
        time_t now_time = std::chrono::system_clock::to_time_t(now);

        struct tm *cTime = gmtime(&now_time);
        if (cTime)
        {
            _QComPutBCD(cTime->tm_year, &poll->poll.Data.torackp.timedate.year, sizeof(poll->poll.Data.torackp.timedate.year));
            _QComPutBCD(cTime->tm_mon + 1, &poll->poll.Data.torackp.timedate.month, sizeof(poll->poll.Data.torackp.timedate.month));
            _QComPutBCD(cTime->tm_mday, &poll->poll.Data.torackp.timedate.day, sizeof(poll->poll.Data.torackp.timedate.day));
            _QComPutBCD(cTime->tm_hour, &poll->poll.Data.torackp.timedate.hours, sizeof(poll->poll.Data.torackp.timedate.hours));
            _QComPutBCD(cTime->tm_min, &poll->poll.Data.torackp.timedate.minutes, sizeof(poll->poll.Data.torackp.timedate.minutes));
            _QComPutBCD(cTime->tm_sec, &poll->poll.Data.torackp.timedate.seconds, sizeof(poll->poll.Data.torackp.timedate.seconds));
        }
        
        std::memcpy(poll->poll.Data.torackp.authno.AUTHNO, data.authno, sizeof(data.authno));

        poll->poll.Data.torackp.CLEN = data.clen;
        if (data.clen)
        {
            std::memcpy(poll->poll.Data.torackp.CTEXT, data.certification, sizeof(char) * data.clen);
        }

        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + sizeof(qc_torackptype) - sizeof(poll->poll.Data.torackp.CTEXT) + sizeof(char) * data.clen;

        return poll;
    }
}


