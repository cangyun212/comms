#include "Core.hpp"
#include "Utils.hpp"

#include <chrono>

#include "Qcom/QcomHTPMaintenance.hpp"
#include "Qcom/QcomInline.h"

namespace sg
{
    uint8_t QcomHTPMaintenance::Id() const
    {
        return QCOM_HTPMP_FC;
    }

    bool QcomHTPMaintenance::BuildHTPMaintenancePoll(QcomJobDataPtr &job, uint8_t poll_address, uint8_t test, QcomHopperTicketPrinterData const& data)
    {
        if (auto it = m_qcom.lock())
        {
            QcomDataPtr p = it->GetEgmData(poll_address);

            if (p)
            {
                std::unique_lock<std::mutex> lock(p->locker);

                job->AddPoll(this->MakeHTPMaintenancePoll(poll_address, p->data.control.last_control, test, data));

                p->data.htp = data;
            }

            return true;
        }

        return false;
    }

    QcomPollPtr QcomHTPMaintenance::MakeHTPMaintenancePoll(uint8_t poll_address, uint8_t last_control, uint8_t test, QcomHopperTicketPrinterData const& data)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

        std::memset(poll.get(), 0, sizeof(QcomPoll));

        poll->poll.DLL.PollAddress = poll_address;
        poll->poll.DLL.ControlByte.CNTL = last_control;
        poll->poll.DLL.FunctionCode = QCOM_HTPMP_FC;

        poll->poll.Data.htpmp2.FLG.bits.PrintTestTicket = test;
        poll->poll.Data.htpmp2.REFILL = data.refill;
        poll->poll.Data.htpmp2.COLLIM = data.collim;
        poll->poll.Data.htpmp2.TICKET = data.ticket;
        poll->poll.Data.htpmp2.DOREFILL = data.dorefill;

        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + sizeof(qc_htpmptype2);

        return poll;
    }
}


