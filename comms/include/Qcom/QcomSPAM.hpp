#ifndef __SG_QCOM_SPAM_HPP__
#define __SG_QCOM_SPAM_HPP__

#include "Core.hpp"

#include "CommsPredeclare.hpp"
#include "Qcom/Qcom.hpp"


namespace sg {

    class QcomSPAMA: public CommsPacketHandler
    {
    public:
        QcomSPAMA(CommsQcomPtr q)
            : CommsPacketHandler(false)
            , m_qcom(q)
        {

        }

    public:
        uint8_t     Id() const override;

    public:
        bool        BuildSPAMAPoll(QcomJobDataPtr &job, uint8_t poll_address, QcomSPAMPollData const& data);

    private:
        QcomPollPtr MakeSPAMAPoll(uint8_t poll_address, uint8_t last_control, QcomSPAMPollData const& data);

    private:
        std::weak_ptr<CommsQcom>     m_qcom;
    };

    class QcomSPAMB: public CommsPacketHandler
    {
    public:
        QcomSPAMB(CommsQcomPtr q)
            : CommsPacketHandler(false)
            , m_qcom(q)
        {

        }

    public:
        uint8_t     Id() const override;

    public:
        bool        BuildSPAMBPoll(QcomJobDataPtr &job, uint8_t poll_address, QcomSPAMPollData const& data);

    private:
        QcomPollPtr MakeSPAMBPoll(uint8_t poll_address, uint8_t last_control, QcomSPAMPollData const& data);

    private:
        std::weak_ptr<CommsQcom>     m_qcom;
    };

    typedef std::shared_ptr<QcomSPAMA> QcomSPAMAPtr;
    typedef std::shared_ptr<QcomSPAMB> QcomSPAMBPtr;

}

#endif // QCOM_EGM_CONFIG_HPP

