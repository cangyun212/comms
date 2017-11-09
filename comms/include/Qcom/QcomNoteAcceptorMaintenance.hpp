#ifndef __SG_QCOM_NOTE_ACCEPTOR_MAINTENANCE_HPP__
#define __SG_QCOM_NOTE_ACCEPTOR_MAINTENANCE_HPP__

#include "Core.hpp"

#include "CommsPredeclare.hpp"
#include "Qcom/Qcom.hpp"


namespace sg {

    class QcomNoteAcceptorMaintenance: public CommsPacketHandler
    {
    public:
        QcomNoteAcceptorMaintenance(CommsQcomPtr q)
            : CommsPacketHandler(false)
            , m_qcom(q)
        {

        }

    public:
        uint8_t     Id() const override;
        uint8_t     RespId() const override;
        bool        Parse(uint8_t buf[], int length) override;

    public:
        bool        BuildNoteAcceptorMaintenancePoll(QcomJobDataPtr &job, uint8_t poll_address, QcomNoteAcceptorMaintenanceData const& data);

    private:
        QcomPollPtr MakeNoteAcceptorMaintenancePoll(uint8_t poll_address, uint8_t last_control, QcomNoteAcceptorMaintenanceData const& data);

    private:
        std::weak_ptr<CommsQcom>     m_qcom;
    };

    typedef std::shared_ptr<QcomNoteAcceptorMaintenance> QcomNoteAcceptorMaintenancePtr;

}


#endif



