#ifndef __QCOM_CASH_TI_REQ_ACK_HPP__
#define __QCOM_CASH_TI_REQ_ACK_HPP__

#include "Core.hpp"

#include "CommsPredeclare.hpp"
#include "Qcom/Qcom.hpp"

namespace sg
{
    class QcomCashTicketInRequestAck : public CommsPacketHandler
    {
    public:
        QcomCashTicketInRequestAck(CommsQcomPtr q)
            : CommsPacketHandler(false)
            , m_qcom(q)
        {

        }

    public:
        uint8_t     Id() const override;

    public:
        bool    BuildCashTicketInRequestAckPoll(QcomJobDataPtr &job, uint8_t poll_address, QcomCashTicketInRequestAckPollData const& data);

    private:
        QcomPollPtr MakeCashTicketInRequestAckPoll(uint8_t poll_address, uint8_t last_control, QcomCashTicketInRequestAckPollData const& data);

    private:
        std::weak_ptr<CommsQcom>    m_qcom;
    };

    typedef std::shared_ptr<QcomCashTicketInRequestAck>    QcomCashTicketInRequestAckPtr;
}


#endif
