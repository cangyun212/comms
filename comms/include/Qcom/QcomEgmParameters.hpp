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
        bool        Parse(uint8_t buf[], int length) override;

    public:
        void        BuildEgmParametersPoll(std::vector<QcomEgmParametersCustomData> const& data);
        void		BuildEgmParametersPoll(uint8_t poll_address, uint8_t reserve, uint8_t autoplay, uint8_t crlimitmode, uint8_t opr,
                                           uint32_t lwin, uint32_t crlimit, uint8_t dumax, uint32_t dulimit, uint16_t tzadj, uint32_t pwrtime,
                                           uint8_t pid, uint16_t eodt, uint32_t npwinp, uint32_t sapwinp);

    private:
        QcomPollPtr		MakeEgmParametersPoll(uint8_t poll_address, uint8_t last_control, uint8_t reserve, uint8_t autoplay, uint8_t crlimitmode, uint8_t opr,
                                              uint32_t lwin, uint32_t crlimit, uint8_t dumax, uint32_t dulimit, uint16_t tzadj, uint32_t pwrtime,
                                              uint8_t pid, uint16_t eodt, uint32_t npwinp, uint32_t sapwinp);


    private:
		std::weak_ptr<CommsQcom>     m_qcom;
    };

    typedef std::shared_ptr<QcomEgmParameters> QcomEgmParametersPtr;

}

#endif // QCOM_EGM_CONFIG_HPP

