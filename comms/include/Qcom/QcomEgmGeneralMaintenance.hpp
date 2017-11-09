#ifndef __QCOM_EGM_GENERAL_MAINTENANCE_HPP__
#define __QCOM_EGM_GENERAL_MAINTENANCE_HPP__

#include "Core.hpp"

#include "CommsPredeclare.hpp"
#include "Qcom/Qcom.hpp"

namespace sg
{
    class QcomEgmGeneralMaintenance: public CommsPacketHandler
    {
    public:
        QcomEgmGeneralMaintenance(CommsQcomPtr q)
            : CommsPacketHandler(false)
            , m_qcom(q)
        {

        }

    public:
        uint8_t     Id() const override;

    public:
        bool    BuildEgmGeneralMaintenancePoll(QcomJobDataPtr &job, uint8_t poll_address, uint16_t gvn, QcomEGMGeneralMaintenancePollData const& data);

    private:
        QcomPollPtr MakeEgmGeneralMaintenancePoll(uint8_t poll_address, uint16_t gvn,  uint8_t last_control, QcomEGMGeneralMaintenancePollData const& data);

    private:
        std::weak_ptr<CommsQcom>    m_qcom;
    };

    typedef std::shared_ptr<QcomEgmGeneralMaintenance>    QcomEgmGeneralMaintenancePtr;
}


#endif
