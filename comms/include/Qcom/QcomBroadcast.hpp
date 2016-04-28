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
        void            BuildPollAddressPoll();
        void            BuildPollAddressPoll(uint8_t poll_address);
        QcomJobDataPtr  MakePollAddressJob();

        void            BuildTimeDataBroadcast();
        void            BuildLinkProgressiveCurrentAmountBroadcast();
        void            BuildGeneralPromotionalMessageBroadcast(u8 gpm_text_length, const char* gpm_text);
        void            BuildSiteDetailsBroadcast(u8 sd_stext_length, u8 sd_ltext_length, const char* sds_text, const char* sdl_text);

    private:
        QcomPollPtr MakePollAddressPoll(size_t size);

        QcomPollPtr MakeTimeDataBroadcast();
        QcomPollPtr MakeLinkProgressiveCurrentAmountBroadcast();
        QcomPollPtr MakeGeneralPromotionalMessageBroadcast(u8 gpm_text_length, const char* gpm_text);
        QcomPollPtr MakeSiteDetailsBroadcast(u8 sd_stext_length, u8 sd_ltext_length, const char* sds_text, const char*sdl_text);

    private:
        std::weak_ptr<CommsQcom>     m_qcom;

    };

    typedef std::shared_ptr<QcomBroadcast>   QcomBroadcastPtr;

}


#endif


