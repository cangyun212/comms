#ifndef __SG_QCOM_BROADCAST_POLL_ADDRESS_HPP__
#define __SG_QCOM_BROADCAST_POLL_ADDRESS_HPP__

#include "core/core.hpp"
#include "core/core_utils.hpp"


#include "comms/comms_predeclare.hpp"
#include "comms/qcom/qcom.hpp"
#include "comms/qcom/qcom_inline.h"

typedef enum
{
    BROADCAST_TYPE_SEEK_EGM = 1,
    BROADCAST_TYPE_TIME_DATA,
    BROADCAST_TYPE_LINK_JP_CUR_AMOUNT,
    BROADCAST_TYPE_GPM,
    BROADCAST_TYPE_POLL_ADDRESS,
    BROADCAST_TYPE_SITE_DETAILS,
}BROADCAST_TYPE;
namespace sg {

    class QcomBroadcast : public CommsPacketHandler
    {
    public:
        QcomBroadcast(CommsQcomPtr q)
            : CommsPacketHandler(false)
            , m_qcom(q)
        {

        }

    public:
        uint8_t     Id() const CORE_OVERRIDE;
        bool        Parse(uint8_t buf[], int length) CORE_OVERRIDE
        {
            CORE_UNREF_PARAM(buf);
            CORE_UNREF_PARAM(length);

            return false;
        }
    public:
        void            BuildPollAddressPoll();
        void            BuildPollAddressPoll(uint8_t poll_address);
        QcomJobDataPtr  MakePollAddressJob();

        void            BuildTimeDataBroadcast();
        void            BuildLinkProgressiveCurrentAmountBroadcast();
        void            BuildGeneralPromotionalMessageBroadcast(uint16_t gpm_text_length, const char* gpm_text);
        void            BuildSiteDetailsBroadcast(uint16_t sd_stext_length, uint16_t sd_ltext_length, const char* sds_text, const char* sdl_text);

    private:
        QcomPollPtr MakePollAddressPoll(size_t size);

        QcomPollPtr MakeTimeDataBroadcast();
        QcomPollPtr MakeLinkProgressiveCurrentAmountBroadcast();
        QcomPollPtr MakeGeneralPromotionalMessageBroadcast(uint16_t gpm_text_length, const char* gpm_text);
        QcomPollPtr MakeSiteDetailsBroadcast(uint16_t sd_stext_length, uint16_t sd_ltext_length, const char* sds_text, const char*sdl_text);

    private:
        weak_ptr<CommsQcom>     m_qcom;

    };

    typedef shared_ptr<QcomBroadcast>   QcomBroadcastPtr;

}


#endif


