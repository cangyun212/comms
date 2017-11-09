#ifndef __SG_QCOM_GENERAL_RESET_HPP__
#define __SG_QCOM_GENERAL_RESET_HPP__

#include "Core.hpp"

#include "CommsPredeclare.hpp"
#include "Qcom/Qcom.hpp"


namespace sg {

    class QcomGeneralReset: public CommsPacketHandler
    {
    public:
        QcomGeneralReset(CommsQcomPtr q)
            : CommsPacketHandler(false)
            , m_qcom(q)
        {

        }

    public:
        uint8_t     Id() const override;

    public:
        bool        BuildGeneralResetPoll(QcomJobDataPtr &job, uint8_t poll_address, QcomGeneralResetPollData const& data);

    private:
        QcomPollPtr MakeGeneralResetPoll(uint8_t poll_address, uint8_t last_control, QcomGeneralResetPollData const& data);

    private:
        std::weak_ptr<CommsQcom>     m_qcom;
    };

    typedef std::shared_ptr<QcomGeneralReset> QcomGeneralResetPtr;

}

#endif // QCOM_EGM_CONFIG_HPP

