#ifndef __QCOM_HTP_MAINTENANCE_HPP__
#define __QCOM_HTP_MAINTENANCE_HPP__

#include "Core.hpp"

#include "CommsPredeclare.hpp"
#include "Qcom/Qcom.hpp"

namespace sg
{
    class QcomHTPMaintenance: public CommsPacketHandler
    {
    public:
        QcomHTPMaintenance(CommsQcomPtr q)
            : CommsPacketHandler(false)
            , m_qcom(q)
        {

        }

    public:
        uint8_t     Id() const override;

    public:
        bool    BuildHTPMaintenancePoll(QcomJobDataPtr &job, uint8_t poll_address, uint8_t test, QcomHopperTicketPrinterData const& data);

    private:
        QcomPollPtr MakeHTPMaintenancePoll(uint8_t poll_address, uint8_t last_control, uint8_t test, QcomHopperTicketPrinterData const& data);

    private:
        std::weak_ptr<CommsQcom>    m_qcom;
    };

    typedef std::shared_ptr<QcomHTPMaintenance> QcomHTPMaintenancePtr;
}


#endif
