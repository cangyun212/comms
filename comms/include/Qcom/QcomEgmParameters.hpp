#ifndef __SG_QCOM_EGM_PARAMETERS_HPP__
#define __SG_QCOM_EGM_PARAMETERS_HPP__

#include "Core.hpp"

#include "CommsPredeclare.hpp"
#include "Qcom/Qcom.hpp"


namespace sg {

    class QcomEgmParameters : public CommsPacketHandler
    {
    public:
        QcomEgmParameters(CommsQcomPtr q)
            : CommsPacketHandler(false)
            , m_qcom(q)
        {

        }

    public:
        uint8_t     Id() const override;

    public:
        bool        BuildEgmParametersPoll(QcomJobDataPtr &job, uint8_t poll_address, QcomEGMParametersData const& data);

    private:
        QcomPollPtr MakeEgmParametersPoll(uint8_t poll_address, uint8_t last_control, QcomEGMParametersData const& data);


    private:
        std::weak_ptr<CommsQcom>     m_qcom;
    };

    typedef std::shared_ptr<QcomEgmParameters> QcomEgmParametersPtr;

}

#endif // QCOM_EGM_CONFIG_HPP

