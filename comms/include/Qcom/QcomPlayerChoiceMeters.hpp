#ifndef __SG_QCOM_PLAYER_CHOICE_METERS_HPP__
#define __SG_QCOM_PLAYER_CHOICE_METERS_HPP__

#include "Core.hpp"

#include "CommsPredeclare.hpp"
#include "Qcom/Qcom.hpp"
#include "Qcom/QcomInline.h"

namespace sg
{
    class QcomPlayerChoiceMeters : public CommsPacketHandler
    {
    public:
        QcomPlayerChoiceMeters(CommsQcomPtr q)
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

    typedef std::shared_ptr<QcomPlayerChoiceMeters>  QcomPlayerChoiceMetersPtr;
}

#endif


