#ifndef __SG_QCOM_EGM_CONFIG_HPP__
#define __SG_QCOM_EGM_CONFIG_HPP__

#include "Core.hpp"

#include "CommsPredeclare.hpp"
#include "Qcom/Qcom.hpp"


namespace sg {

    class QcomEgmConfiguration : public CommsPacketHandler
    {
    public:
        QcomEgmConfiguration(CommsQcomPtr q)
            : CommsPacketHandler(false)
            , m_qcom(q)
        {

        }

    public:
        uint8_t     Id() const override;
        bool        Parse(uint8_t buf[], int length) override;

    public:
        bool        BuildEGMConfigPoll(QcomJobDataPtr &job, uint8_t poll_address, QcomEGMConfigPollData const& data);
    private:
        QcomPollPtr MakeEGMConfigPoll(uint8_t poll_address, uint8_t last_control, uint32_t ser, QcomEGMConfigPollData const& data);

    private:
        std::weak_ptr<CommsQcom>     m_qcom;
    };

    typedef std::shared_ptr<QcomEgmConfiguration> QcomEgmConfigurationPtr;

}

#endif // QCOM_EGM_CONFIG_HPP

