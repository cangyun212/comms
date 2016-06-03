#ifndef __SG_QCOM_BROADCAST_POLL_ADDRESS_HPP__
#define __SG_QCOM_BROADCAST_POLL_ADDRESS_HPP__

#include "Core.hpp"
#include "Utils.hpp"

#include "CommsPredeclare.hpp"
#include "Qcom/Qcom.hpp"
#include "Qcom/QcomInline.h"


namespace sg 
{
    enum QcomBroadcastType
    {
        BROADCAST_TYPE_SEEK_EGM = 1,
        BROADCAST_TYPE_TIME_DATA,
        BROADCAST_TYPE_LINK_JP_CUR_AMOUNT,
        BROADCAST_TYPE_GPM,
        BROADCAST_TYPE_POLL_ADDRESS,
        BROADCAST_TYPE_SITE_DETAILS,
    };

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
        void    BuildPollAddressPoll();
        void    BuildPollAddressPoll(uint8_t poll_address);
        void    BuildTimeDataBroadcast();
        void    BuildLinkProgressiveCurrentAmountBroadcast(QcomLinkedProgressiveData const& data);
        void    BuildGeneralPromotionalMessageBroadcast(std::string const& text);
        void    BuildSiteDetailsBroadcast(std::string const& stext, std::string const& ltext);

    private:
        QcomPollPtr     MakePollAddressPoll(size_t size);
        QcomPollPtr     MakePollAddressPoll(uint32_t ser, uint8_t poll_address);
        QcomJobDataPtr  MakePollAddressJob();
        QcomPollPtr     MakeTimeDataBroadcast();
        QcomPollPtr     MakeLinkProgressiveCurrentAmountBroadcast(QcomLinkedProgressiveData const& data);
        QcomPollPtr     MakeGeneralPromotionalMessageBroadcast(std::string const& text);
        QcomPollPtr     MakeSiteDetailsBroadcast(std::string const& stext, std::string const& ltext);

    private:
        std::weak_ptr<CommsQcom>     m_qcom;

    };

    typedef std::shared_ptr<QcomBroadcast>   QcomBroadcastPtr;

}


#endif


