#ifndef __SG_QCOM_BROADCAST_SEEK_HPP__
#define __SG_QCOM_BROADCAST_SEEK_HPP__

#include "Core.hpp"

#include "CommsPredeclare.hpp"
#include "Qcom/Qcom.hpp"

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
        uint8_t     Id() const override;
        uint8_t     RespId() const override { return QCOM_SEEKEGM_FC; }
        bool        Parse(uint8_t buf[], int length) override;

    public:
        bool        BuildSeekEGMPoll(QcomJobDataPtr job);


    private:
        std::weak_ptr<CommsQcom>     m_qcom;

    };

    typedef std::shared_ptr<QcomBroadcastSeek>   QcomBroadcastSeekPtr;

}


#endif


