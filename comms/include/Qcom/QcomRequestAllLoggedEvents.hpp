#ifndef __SG_QCOM_REQUEST_ALL_LOGGED_EVENTS_HPP__
#define __SG_QCOM_REQUEST_ALL_LOGGED_EVENTS_HPP__

#include "Core.hpp"

#include "CommsPredeclare.hpp"
#include "Qcom/Qcom.hpp"


namespace sg {

    class QcomRequestAllLoggedEvents : public CommsPacketHandler
    {
    public:
        QcomRequestAllLoggedEvents(CommsQcomPtr q)
            : CommsPacketHandler(false)
            , m_qcom(q)
        {

        }

    public:
        uint8_t     Id() const override;

    public:
        bool        BuildRequestAllLoggedEventsPoll(QcomJobDataPtr &job, uint8_t poll_address);

    private:
        QcomPollPtr MakeRequestAllLoggedEventsPoll(uint8_t poll_address, uint8_t last_control);

    private:
        std::weak_ptr<CommsQcom>     m_qcom;
    };

    typedef std::shared_ptr<QcomRequestAllLoggedEvents> QcomRequestAllLoggedEventsPtr;

}

#endif // 

