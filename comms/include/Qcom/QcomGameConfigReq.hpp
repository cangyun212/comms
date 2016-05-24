#ifndef __SG_QCOM_GAME_CONFIG_REQ_HPP__
#define __SG_QCOM_GAME_CONFIG_REQ_HPP__
#include "Core.hpp"

#include "CommsPredeclare.hpp"
#include "Qcom/Qcom.hpp"


namespace sg {

    class QcomGameConfigurationRequest : public CommsPacketHandler
    {
    public:
        QcomGameConfigurationRequest(CommsQcomPtr q)
            : CommsPacketHandler(false)
            , m_qcom(q)
        {

        }

    public:
        uint8_t     Id() const override;
		uint8_t 	RespId() const override { return QCOM_EGMGCR_FC; }
        bool        Parse(uint8_t buf[], int length) override;

    private:
		std::weak_ptr<CommsQcom>     m_qcom;
    };

    typedef std::shared_ptr<QcomGameConfigurationRequest> QcomGameConfigurationRequestPtr;

}

#endif // QCOM_GAME_CONFIG_REQ

