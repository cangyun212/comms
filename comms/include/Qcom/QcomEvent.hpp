#ifndef __SG_QCOM_EVENT_HPP__
#define __SG_QCOM_EVENT_HPP__

#include "Core.hpp"

#include "CommsPredeclare.hpp"
#include "Qcom/Qcom.hpp"
#include "Qcom/QcomInline.h"

namespace sg
{
    class QcomEvent : public CommsPacketHandler
    {
    public:
        QcomEvent(CommsQcomPtr q)
            : CommsPacketHandler(false)
            , m_qcom(q)
        {

        }

    public:
        uint8_t     RespId() const override;
        bool        Parse(uint8_t buf[], int length) override;

    private:
        std::weak_ptr<CommsQcom>    m_qcom;
    };

    typedef std::shared_ptr<QcomEvent>      QcomEventPtr;
}

#endif // !__SG_QCOM_EVENT_HPP__


