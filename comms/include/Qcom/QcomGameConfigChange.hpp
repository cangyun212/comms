#ifndef __SG_QCOM_GAME_CONFIG_CHANGE_HPP__
#define __SG_QCOM_GAME_CONFIG_CHANGE_HPP__
#include "Core.hpp"

#include "CommsPredeclare.hpp"
#include "Qcom/Qcom.hpp"


namespace sg {

    class QcomGameConfigurationChange : public CommsPacketHandler
    {
    public:
        QcomGameConfigurationChange(CommsQcomPtr q)
            : CommsPacketHandler(false)
            , m_qcom(q)
        {

        }

    public:
        uint8_t     Id() const override;

    public:
        bool        BuildGameConfigChangePoll(QcomJobDataPtr &job, uint8_t poll_address, uint16_t gvn, QcomGameSettingData const& data, uint16_t &old_pgid, uint8_t &shared);

    private:
        QcomPollPtr MakeGameConfigChangePoll(uint8_t poll_address, uint8_t last_control, uint16_t gvn, QcomGameSettingData const& data);


    private:
        std::weak_ptr<CommsQcom>     m_qcom;
    };

    typedef std::shared_ptr<QcomGameConfigurationChange> QcomGameConfigurationChangePtr;

}
#endif // QCOM_GAME_CONFIG_CHANGE_HPP

