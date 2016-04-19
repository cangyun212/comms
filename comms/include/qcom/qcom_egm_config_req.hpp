#ifndef __SG_QCOM_EGM_CONFIG_REQ_HPP__
#define __SG_QCOM_EGM_CONFIG_REQ_HPP__

#include "core/core.hpp"

#include "comms/comms_predeclare.hpp"
#include "comms/qcom/qcom.hpp"


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
        uint8_t     Id() const CORE_OVERRIDE;
        bool        Parse(uint8_t buf[], int length) CORE_OVERRIDE;

    public:
        void        BuildEGMConfigReqPoll(std::vector<QcomEGMConifgReqCustomData> const& data);
        void        BuildEGMConfigReqPoll(uint8_t poll_address, uint8_t mef, uint8_t gcr, uint8_t psn);

    private:
        QcomPollPtr MakeEGMConfigReqPoll(uint8_t poll_address, uint8_t last_control, uint16_t mef, uint16_t gcr, uint16_t psn);

    private:
        weak_ptr<CommsQcom>     m_qcom;
    };

    typedef shared_ptr<QcomEgmConfigurationRequest> QcomEgmConfigurationRequestPtr;

}


#endif



