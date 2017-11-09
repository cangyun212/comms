#ifndef __QCOM_CASH_TO_REQ_HPP__
#define __QCOM_CASH_TO_REQ_HPP__

#include "Core.hpp"

#include "CommsPredeclare.hpp"
#include "Qcom/Qcom.hpp"

namespace sg
{
    class QcomCashTicketOutRequest : public CommsPacketHandler
    {
    public:
        QcomCashTicketOutRequest(CommsQcomPtr q)
            : CommsPacketHandler(false)
            , m_qcom(q)
        {

        }

    public:
        uint8_t     Id() const override;

    public:
        bool    BuildCashTicketOutRequestPoll(QcomJobDataPtr &job, uint8_t poll_address);

    private:
        QcomPollPtr MakeCashTicketOutRequestPoll(uint8_t poll_address, uint8_t last_control);

    private:
        std::weak_ptr<CommsQcom>    m_qcom;
    };

    typedef std::shared_ptr<QcomCashTicketOutRequest>    QcomCashTicketOutRequestPtr;
}


#endif
