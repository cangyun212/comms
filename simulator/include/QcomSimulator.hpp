#ifndef __SG_QCOM_SIMULATOR_HPP__
#define __SG_QCOM_SIMULATOR_HPP__

#include "Core.hpp"

#include "CommsPredeclare.hpp"
#include "Predeclare.hpp"
#include "ActionCenter.hpp"
#include "Simulator.hpp"

namespace sg {

    class QcomSim : public Simulator
    {
    public:
        QcomSim();
        ~QcomSim();

    public:
        void    DoInit() override;

    public:
        void    SeekEGM(const ActionCenter &sender, const ActionPtr &action);
        void    ListEGM(const ActionCenter &sender, const ActionPtr &action);
        void    PickEGM(const ActionCenter &sender, const ActionPtr &action);
        void    EGMPollAddConf(const ActionCenter &sender, const ActionPtr &action);
        void    TimeDate(const ActionCenter &sender, const ActionPtr &action);
        void    LPCurrentAmount(const ActionCenter &sender, const ActionPtr &action);
        void    GeneralPromotional(const ActionCenter &sender, const ActionPtr &action);
        void    SiteDetail(const ActionCenter &sender, const ActionPtr &action);
        void    EGMConfRequest(const ActionCenter &sender, const ActionPtr &action);
        void    EGMConfiguration(const ActionCenter &sender, const ActionPtr &action);
        void    GameConfiguration(const ActionCenter &sender, const ActionPtr &action);
        void    GameConfigurationChange(const ActionCenter &sender, const ActionPtr &action);
        void    EGMParameters(const ActionCenter &sender, const ActionPtr &action);
        void    PurgeEvents(const ActionCenter &sender, const ActionPtr &action);
        void    GeneralStatus(const ActionCenter &sender, const ActionPtr &action);
        void    ProgressiveChange(const ActionCenter &sender, const ActionPtr &action);
        void    ExtJPInfo(const ActionCenter &sender, const ActionPtr &action);
        void    ProgHashRequest(const ActionCenter &sender, const ActionPtr &action);
        void    PendingPoll(const ActionCenter &sender, const ActionPtr &action);
        void    SendPoll(const ActionCenter &sender, const ActionPtr &action);
        void    ChangeDev(const ActionCenter &sender, const ActionPtr &action);

    private:
        void    ListEGMInfo(bool show_all = true);
        bool    Pick(uint8_t target);

    private:
        CommsQcomPtr    m_qcom;
        uint8_t         m_curr_egm;
    };

}



#endif


