#ifndef __SG_QCOM_BROADCAST_POLL_ADDRESS_HPP__
#define __SG_QCOM_BROADCAST_POLL_ADDRESS_HPP__

#include "Core.hpp"
#include "Utils.hpp"

#include "CommsPredeclare.hpp"
#include "Qcom/Qcom.hpp"
#include "Qcom/QcomInline.h"


namespace sg 
{
    class QcomBroadcast : public CommsPacketHandler
    {
    public:
        QcomBroadcast(CommsQcomPtr q)
            : CommsPacketHandler(false)
            , m_qcom(q)
        {

        }

    public:
        uint8_t     Id() const override;
        bool        Parse(uint8_t buf[], int length) override 
        {
            SG_UNREF_PARAM(buf);
            SG_UNREF_PARAM(length);

            return false;
        }

    public:
        bool    BuildPollAddressPoll(QcomJobDataPtr job);
        bool    BuildPollAddressPoll(QcomJobDataPtr job, uint8_t poll_address);
        bool    BuildTimeDateBroadcast(QcomJobDataPtr job);
        bool    BuildLinkProgressiveCurrentAmountBroadcast(QcomJobDataPtr job, QcomLinkedProgressiveData const& data);
        bool    BuildGeneralPromotionalMessageBroadcast(QcomJobDataPtr job, std::string const& text);
        bool    BuildSiteDetailsBroadcast(QcomJobDataPtr job, std::string const& stext, std::string const& ltext);

    private:
        QcomPollPtr     MakePollAddressPoll(size_t size);
        QcomPollPtr     MakePollAddressPoll(uint32_t ser, uint8_t poll_address);
        bool            MakePollAddressJob(QcomJobDataPtr job);
        QcomPollPtr     MakeTimeDateBroadcast();
        QcomPollPtr     MakeLinkProgressiveCurrentAmountBroadcast(QcomLinkedProgressiveData const& data);
        QcomPollPtr     MakeGeneralPromotionalMessageBroadcast(std::string const& text);
        QcomPollPtr     MakeSiteDetailsBroadcast(std::string const& stext, std::string const& ltext);

    private:
        std::weak_ptr<CommsQcom>     m_qcom;

    };

    typedef std::shared_ptr<QcomBroadcast>   QcomBroadcastPtr;

}


#endif


