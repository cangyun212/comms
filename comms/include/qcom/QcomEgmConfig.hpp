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
        void        BuildEGMConfigPoll(std::vector<QcomEGMConfigCustomData> const& data);
        void        BuildEGMConfigPoll(uint8_t poll_address, uint8_t jur, uint32_t den, uint32_t tok, uint32_t maxden, uint16_t minrtp, uint16_t maxrtp, uint16_t maxsd,
                                       uint16_t maxlines, uint32_t maxbet, uint32_t maxnpwin, uint32_t maxpwin, uint32_t maxect);
    private:
        QcomPollPtr MakeEGMConfigPoll(uint8_t poll_address, uint8_t last_control, uint32_t ser, uint8_t jur, uint32_t den, uint32_t tok, uint32_t maxden, uint16_t minrtp, uint16_t maxrtp, uint16_t maxsd,
                                      uint16_t maxlines, uint32_t maxbet, uint32_t maxnpwin, uint32_t maxpwin, uint32_t maxect);

    private:
        std::weak_ptr<CommsQcom>     m_qcom;
    };

    typedef std::shared_ptr<QcomEgmConfiguration> QcomEgmConfigurationPtr;

}

#endif // QCOM_EGM_CONFIG_HPP

