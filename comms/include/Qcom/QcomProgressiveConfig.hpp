#ifndef __SG_QCOM_PROGRESSIVE_CONFIG_HPP__
#define __SG_QCOM_PROGRESSIVE_CONFIG_HPP__

#include "Core.hpp"

#include "CommsPredeclare.hpp"
#include "Qcom/Qcom.hpp"

namespace sg
{
    class QcomProgressiveConfig : public CommsPacketHandler
    {
    public:
        QcomProgressiveConfig(CommsQcomPtr q)
            : CommsPacketHandler(false)
            , m_qcom(q)
        {

        }

    public:
        uint8_t     Id() const override;
        uint8_t     RespId() const override;
        bool        Parse(uint8_t buf[], int length) override;

    public:
        bool        BuildProgConfigPoll(QcomJobDataPtr &job, uint8_t poll_address, uint16_t gvn, uint8_t pnum, QcomProgressiveConfigData const& data);

    private:
        QcomPollPtr MakeProgConfigPoll(uint8_t poll_address, uint8_t last_control, uint16_t gvn, uint8_t pnum, QcomProgressiveConfigData const& data);

    private:
        std::weak_ptr<CommsQcom>    m_qcom;
    };

    typedef std::shared_ptr<QcomProgressiveConfig> QcomProgressiveConfigPtr;
}


#endif
