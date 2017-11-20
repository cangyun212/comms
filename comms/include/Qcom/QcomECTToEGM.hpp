#ifndef __SG_QCOM_ECT_TO_EGM_HPP__
#define __SG_QCOM_ECT_TO_EGM_HPP__

#include "Core.hpp"

#include "CommsPredeclare.hpp"
#include "Qcom/Qcom.hpp"


namespace sg 
{

    class QcomECTToEGM: public CommsPacketHandler
    {
    public:
        QcomECTToEGM(CommsQcomPtr q)
            : CommsPacketHandler(false)
            , m_qcom(q)
        {

        }

    public:
        uint8_t     Id() const override;
        uint8_t     RespId() const override;
        bool        Parse(uint8_t buf[], int length) override;

    public:
        bool        BuildECTToEGMPoll(QcomJobDataPtr &job, uint8_t poll_address, QcomECTToEGMPollData const& data);

    private:
        QcomPollPtr MakeECTToEGMPoll(uint8_t poll_address, uint8_t last_control, uint8_t psn, QcomECTToEGMPollData const& data);

    private:
        std::weak_ptr<CommsQcom>     m_qcom;
    };

    typedef std::shared_ptr<QcomECTToEGM> QcomECTToEGMPtr;

}

#endif // 

