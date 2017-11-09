#ifndef __SG_QCOM_LP_AWARD_ACK_HPP__
#define __SG_QCOM_LP_AWARD_ACK_HPP__

#include "Core.hpp"

#include "CommsPredeclare.hpp"
#include "Qcom/Qcom.hpp"


namespace sg {

    class QcomLPAwardAck : public CommsPacketHandler
    {
    public:
        QcomLPAwardAck(CommsQcomPtr q)
            : CommsPacketHandler(false)
            , m_qcom(q)
        {

        }

    public:
        uint8_t     Id() const override;

    public:
        bool        BuildLPAwardAckPoll(QcomJobDataPtr &job, uint8_t poll_address);

    private:
        QcomPollPtr MakeLPAwardAckPoll(uint8_t poll_address, uint8_t last_control);

    private:
        std::weak_ptr<CommsQcom>     m_qcom;
    };

    typedef std::shared_ptr<QcomLPAwardAck> QcomLPAwardAckPtr;

}

#endif // QCOM_EGM_CONFIG_HPP

