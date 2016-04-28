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
        bool        Parse(uint8_t buf[], int length) override;

    public:
        void        BuildGameConfigPoll(std::vector<QcomGameConfigCustomData> const& data);
        void        BuildGameConfigPoll(uint8_t poll_address, uint8_t var, uint8_t var_lock, uint8_t game_enable, uint8_t pnum,
                                        std::vector<uint8_t> const& lp, std::vector<uint32_t> const& camt);

    private:
        QcomPollPtr MakeGameConfigPoll(uint8_t poll_address, uint8_t last_control, uint16_t last_gvn, uint8_t var, uint8_t var_lock,
                                           uint8_t game_enable, uint8_t pnum, std::vector<uint8_t> const&lp, std::vector<uint32_t> const&camt);


    private:
        std::weak_ptr<CommsQcom>     m_qcom;
    };

    typedef std::shared_ptr<QcomGameConfiguration> QcomGameConfigurationPtr;

}

#endif // QCOM_EGM_CONFIG_HPP

