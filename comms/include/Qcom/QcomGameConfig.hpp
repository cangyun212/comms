#ifndef __SG_QCOM_GAME_CONFIG_HPP__
#define __SG_QCOM_GAME_CONFIG_HPP__

#include "Core.hpp"

#include "CommsPredeclare.hpp"
#include "Qcom/Qcom.hpp"


namespace sg 
{

    class QcomGameConfiguration : public CommsPacketHandler
    {
    public:
        QcomGameConfiguration(CommsQcomPtr q)
            : CommsPacketHandler(false)
            , m_qcom(q)
        {

        }

    public:
        uint8_t     Id() const override;
        uint8_t     RespId() const override;
        bool        Parse(uint8_t buf[], int length) override;

    public:
        bool        BuildGameConfigPoll(QcomJobDataPtr &job, uint8_t poll_address, uint16_t gvn, QcomGameConfigData const& data);

    private:
        QcomPollPtr MakeGameConfigPoll(uint8_t poll_address, uint8_t last_control, uint16_t gvn, QcomGameConfigData const& data);


    private:
        std::weak_ptr<CommsQcom>     m_qcom;
    };

    typedef std::shared_ptr<QcomGameConfiguration> QcomGameConfigurationPtr;

}

#endif // QCOM_EGM_CONFIG_HPP

