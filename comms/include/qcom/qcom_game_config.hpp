#ifndef QCOM_GAME_CONFIG_HPP
#define QCOM_GAME_CONFIG_HPP

#include "core/core.hpp"

#include "comms/comms_predeclare.hpp"
#include "comms/qcom/qcom.hpp"


namespace sg {

    class QcomGameConfiguration : public CommsPacketHandler
    {
    public:
        QcomGameConfiguration(CommsQcomPtr q)
            : CommsPacketHandler(false)
            , m_qcom(q)
        {

        }

    public:
        uint8_t     Id() const CORE_OVERRIDE;
        bool        Parse(uint8_t buf[], int length) CORE_OVERRIDE;

    public:
        void        BuildGameConfigPoll(std::vector<QcomGameConfigCustomData> const& data);
        void		BuildGameConfigPoll(uint8_t poll_address, uint8_t var, uint8_t var_lock, uint8_t game_enable, uint8_t pnum,
                                        std::vector<uint8_t> const& lp, std::vector<uint32_t> const& camt);

    private:
        QcomPollPtr		MakeGameConfigPoll(uint8_t poll_address, uint8_t last_control, uint16_t last_gvn, uint8_t var, uint8_t var_lock,
                                           uint8_t game_enable, uint8_t pnum, std::vector<uint8_t> const&lp, std::vector<uint32_t> const&camt);


    private:
        weak_ptr<CommsQcom>     m_qcom;
    };

    typedef shared_ptr<QcomGameConfiguration> QcomGameConfigurationPtr;

}

#endif // QCOM_EGM_CONFIG_HPP

