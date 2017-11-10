#ifndef __SG_QCOM_TOWER_LIGHT_MAINTENANCE_HPP__
#define __SG_QCOM_TOWER_LIGHT_MAINTENANCE_HPP__

#include "Core.hpp"

#include "CommsPredeclare.hpp"
#include "Qcom/Qcom.hpp"


namespace sg {

    class QcomTowerLightMaintenance: public CommsPacketHandler
    {
    public:
        QcomTowerLightMaintenance(CommsQcomPtr q)
            : CommsPacketHandler(false)
            , m_qcom(q)
        {

        }

    public:
        uint8_t     Id() const override;

    public:
        bool        BuildTowerLightMaintenancePoll(QcomJobDataPtr &job, uint8_t poll_address, QcomTowerLightMaintenancePollData const& data);

    private:
        QcomPollPtr MakeTowerLightMaintenancePoll(uint8_t poll_address, uint8_t last_control, QcomTowerLightMaintenancePollData const& data);

    private:
        std::weak_ptr<CommsQcom>     m_qcom;
    };

    typedef std::shared_ptr<QcomTowerLightMaintenance> QcomTowerLightMaintenancePtr;

}

#endif // QCOM_EGM_CONFIG_HPP

