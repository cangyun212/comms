#ifndef QCOM_GENERAL_STATUS_HPP
#define QCOM_GENERAL_STATUS_HPP

#include "core/core.hpp"

#include "comms/comms_predeclare.hpp"
#include "comms/qcom/qcom.hpp"


namespace sg {

    class QcomGeneralStatus : public CommsPacketHandler
    {
    public:
        QcomGeneralStatus(CommsQcomPtr q)
            : CommsPacketHandler(false)
            , m_qcom(q)
        {

        }

    public:
        uint8_t     Id() const CORE_OVERRIDE;
        bool        Parse(uint8_t buf[], int length) CORE_OVERRIDE;

    public:
//        void        BuildGeneralStatusPoll(std::vector<QcomEGMConfigCustomData> const& data);
        void        BuildGeneralStatusPoll(uint8_t poll_address);
         QcomJobDataPtr  MakeGeneralStatusJob();
    private:
        QcomPollPtr MakeGeneralStatusPoll(uint8_t poll_address, uint8_t last_control);

    private:
        weak_ptr<CommsQcom>     m_qcom;
    };

    typedef shared_ptr<QcomGeneralStatus> QcomGenerlStatusPtr;

}

#endif // QCOM_GENERAL_STATUS_HPP

