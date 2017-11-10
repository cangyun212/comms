#include "Core.hpp"
#include "Utils.hpp"

#include "Qcom/QcomTowerLightMaintenance.hpp"
#include "Qcom/qogr/qogr_crc.h"

namespace sg 
{
    uint8_t QcomTowerLightMaintenance::Id() const
    {
        return QCOM_EGMTLMP_FC;
    }

    bool QcomTowerLightMaintenance::BuildTowerLightMaintenancePoll(QcomJobDataPtr &job, uint8_t poll_address, QcomTowerLightMaintenancePollData const& data)
    {
        if (auto it = m_qcom.lock())
        {
            {
                job->AddPoll(this->MakeTowerLightMaintenancePoll(poll_address, 0, data));

                return true;
            }
        }

        return false;
    }

    QcomPollPtr QcomTowerLightMaintenance::MakeTowerLightMaintenancePoll(uint8_t poll_address, uint8_t last_control, QcomTowerLightMaintenancePollData const& data)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

        std::memset(poll.get(), 0, sizeof(QcomPoll));

        // ref Qcom1.6-15.4.2
        poll->poll.DLL.PollAddress = poll_address;
        poll->poll.DLL.ControlByte.CNTL = last_control;
        poll->poll.DLL.FunctionCode = QCOM_EGMTLMP_FC;

        poll->poll.Data.egmtlmp.FLG.bits.on.lowest = data.yellow_on;
        poll->poll.Data.egmtlmp.FLG.bits.on.middle = data.blue_on;
        poll->poll.Data.egmtlmp.FLG.bits.on.top = data.red_on;
        poll->poll.Data.egmtlmp.FLG.bits.flash.lowest = data.yellow_flash;
        poll->poll.Data.egmtlmp.FLG.bits.flash.middle = data.blue_flash;
        poll->poll.Data.egmtlmp.FLG.bits.flash.top = data.red_flash;

        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + sizeof(qc_egmtlmptype);
        return poll;
    }

}
