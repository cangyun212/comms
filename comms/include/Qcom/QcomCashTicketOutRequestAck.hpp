#ifndef __QCOM_CASH_TO_REQ_ACK_HPP__
#define __QCOM_CASH_TO_REQ_ACK_HPP__

#include "Core.hpp"

#include "CommsPredeclare.hpp"
#include "Qcom/Qcom.hpp"

namespace sg
{
    class QcomCashTicketOutRequestAck : public CommsPacketHandler
    {
    public:
        QcomCashTicketOutRequestAck(CommsQcomPtr q)
            : CommsPacketHandler(false)
            , m_qcom(q)
        {

        }

    public:
        uint8_t     Id() const override;

    public:
        bool    BuildCashTicketOutRequestAckPoll(QcomJobDataPtr &job, uint8_t poll_address, QcomCashTicketOutRequestAckPollData const& data);

    private:
        QcomPollPtr MakeCashTicketOutRequestAckPoll(uint8_t poll_address, uint8_t last_control, QcomCashTicketOutRequestAckPollData const& data);

    private:
        std::weak_ptr<CommsQcom>    m_qcom;
    };

    typedef std::shared_ptr<QcomCashTicketOutRequestAck>    QcomCashTicketOutRequestAckPtr;
}


#endif
