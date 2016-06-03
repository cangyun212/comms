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
        uint8_t     RespId() const override;
        bool        Parse(uint8_t buf[], int length) override;

    public:
        void        BuildEGMConfigReqPoll(uint8_t poll_address, QcomEGMControlPollData const& data);

    private:
        QcomPollPtr MakeEGMConfigReqPoll(uint8_t poll_address, uint8_t last_control, QcomEGMControlPollData const& data);

    private:
        std::weak_ptr<CommsQcom>     m_qcom;
    };

    typedef std::shared_ptr<QcomEgmConfigurationRequest> QcomEgmConfigurationRequestPtr;

}


#endif



