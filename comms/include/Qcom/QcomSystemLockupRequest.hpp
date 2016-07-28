#ifndef __QCOM_SYSTEM_LOCKUP_REQUEST_HPP__
#define __QCOM_SYSTEM_LOCKUP_REQUEST_HPP__

#include "Core.hpp"

#include "CommsPredeclare.hpp"
#include "Qcom/Qcom.hpp"


namespace sg
{
    class QcomSysLockupReq : public CommsPacketHandler
    {
    public:
        QcomSysLockupReq(CommsQcomPtr q)
            : CommsPacketHandler(false)
            , m_qcom(q)
        {

        }

    public:
        uint8_t     Id() const override;

    public:
        bool        BuildSysLockupReqPoll(QcomJobDataPtr &job, uint8_t poll_address, QcomSysLockupRequestData const& data);

    private:
        QcomPollPtr MakeSysLockupReqPoll(uint8_t poll_address, uint8_t last_control, QcomSysLockupRequestData const& data);

    private:
        std::weak_ptr<CommsQcom>    m_qcom;
    };

    typedef std::shared_ptr<QcomSysLockupReq> QcomSysLockupReqPtr;
}



#endif // !__QCOM_SYSTEM_LOCKUP_REQUEST_HPP__


