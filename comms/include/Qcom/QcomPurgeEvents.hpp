#ifndef __SG_QCOM_PURGE_EVENTS_HPP__
#define __SG_QCOM_PURGE_EVENTS_HPP__

#include "Core.hpp"

#include "CommsPredeclare.hpp"
#include "Qcom/Qcom.hpp"


namespace sg {

    class QcomPurgeEvents : public CommsPacketHandler
    {
    public:
        QcomPurgeEvents(CommsQcomPtr q)
            : CommsPacketHandler(false)
            , m_qcom(q)
        {

        }

    public:
        uint8_t     Id() const override;
        uint8_t     RespId() const override;
        bool        Parse(uint8_t buf[], int length) override;

    public:
        bool        BuildPurgeEventsPoll(QcomJobDataPtr &job, uint8_t poll_address, uint8_t evtno);

    private:
        QcomPollPtr MakePurgeEventsPoll(uint8_t poll_address, uint8_t last_control, uint8_t psn, uint8_t evtno);

    private:
        std::weak_ptr<CommsQcom>     m_qcom;
    };

    typedef std::shared_ptr<QcomPurgeEvents> QcomPurgeEventsPtr;

}
#endif // QCOM_PURGE_EVENTS_HPP

