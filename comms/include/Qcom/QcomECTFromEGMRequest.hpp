#ifndef __QCOM_ECT_FROM_EGM_REQUEST_HPP__
#define __QCOM_ECT_FROM_EGM_REQUEST_HPP__

#include "Core.hpp"

#include "CommsPredeclare.hpp"
#include "Qcom/Qcom.hpp"

namespace sg
{
    class QcomECTFromEGMRequest : public CommsPacketHandler
    {
    public:
        QcomECTFromEGMRequest(CommsQcomPtr q)
            : CommsPacketHandler(false)
            , m_qcom(q)
        {

        }

    public:
        uint8_t     Id() const override;

    public:
        bool    BuildECTFromEGMRequestPoll(QcomJobDataPtr &job, uint8_t poll_address);

    private:
        QcomPollPtr MakeECTFromEGMRequestPoll(uint8_t poll_address, uint8_t last_control);

    private:
        std::weak_ptr<CommsQcom>    m_qcom;
    };

    typedef std::shared_ptr<QcomECTFromEGMRequest>      QcomECTFromEGMRequestPtr;
}


#endif
