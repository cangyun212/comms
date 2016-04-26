#ifndef __SG_QCOM_BROADCAST_SEEK_HPP__
#define __SG_QCOM_BROADCAST_SEEK_HPP__

#include "core/core.hpp"


#include "comms/comms_predeclare.hpp"
#include "comms/qcom/qcom.hpp"

namespace sg {

    class QcomBroadcastSeek : public CommsPacketHandler
    {
    public:
        QcomBroadcastSeek(CommsQcomPtr q)
            : CommsPacketHandler(false)
            , m_qcom(q)
        {

        }

    public:
        uint8_t     Id() const CORE_OVERRIDE;
        bool        Parse(uint8_t buf[], int length) CORE_OVERRIDE;

    public:
        void        BuildSeekEGMPoll();


    private:
        weak_ptr<CommsQcom>     m_qcom;

    };

    typedef shared_ptr<QcomBroadcastSeek>   QcomBroadcastSeekPtr;

}


#endif


