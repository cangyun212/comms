#ifndef __SG_QCOM_EGM_CONFIG_REQ_HPP__
#define __SG_QCOM_EGM_CONFIG_REQ_HPP__

#include "Core.hpp"

#include "CommsPredeclare.hpp"
#include "Qcom/Qcom.hpp"


namespace sg {

    class QcomEgmConfigurationRequest : public CommsPacketHandler
    {
    public:
        QcomEgmConfigurationRequest(CommsQcomPtr q)
            : CommsPacketHandler(false)
            , m_qcom(q)
        {

        }

    public:
        uint8_t     Id() const override;
        bool        Parse(uint8_t buf[], int length) override;

    public:
        void        BuildEGMConfigReqPoll(std::vector<QcomEGMConifgReqCustomData> const& data);
        void        BuildEGMConfigReqPoll(uint8_t poll_address, uint8_t mef, uint8_t gcr, uint8_t psn);

    private:
        QcomPollPtr MakeEGMConfigReqPoll(uint8_t poll_address, uint8_t last_control, uint16_t mef, uint16_t gcr, uint16_t psn);

    private:
        std::weak_ptr<CommsQcom>     m_qcom;
    };

    typedef std::shared_ptr<QcomEgmConfigurationRequest> QcomEgmConfigurationRequestPtr;

}


#endif



