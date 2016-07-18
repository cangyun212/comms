#include "Core.hpp"

#include "SimUtils.hpp"

#include "QcomAction.hpp"
#include "QcomActionFactory.hpp"
#include "QcomSimulator.hpp"

namespace sg 
{
    SimulatorPtr g_sim = nullptr;
    ActionFactoryPtr g_fac = nullptr;

    namespace
    {

        bool quit = false;

        void on_quit(ActionCenter const&, ActionPtr const&)
        {
            quit = true;
        }

        QcomSimPtr setup_qcom()
        {
            QcomSimPtr sim = MakeSharedPtr<QcomSim>();
            ActionCenter& center = ActionCenter::Instance();

            center.Install(Action::AT_QUIT);
            center.GetEvent(Action::AT_QUIT)->connect(
                std::bind(on_quit, std::placeholders::_1, std::placeholders::_2));

            sim->Install(Action::AT_QCOM_SEEKEGM);
            center.Install(Action::AT_QCOM_SEEKEGM);

            sim->GetConnection(Action::AT_QCOM_SEEKEGM) =
                center.GetEvent(Action::AT_QCOM_SEEKEGM)->connect(
                    std::bind(&QcomSim::SeekEGM, sim.get(), std::placeholders::_1, std::placeholders::_2));

            sim->Install(Action::AT_QCOM_EGM_POLL_ADD_CONF);
            center.Install(Action::AT_QCOM_EGM_POLL_ADD_CONF);

            sim->GetConnection(Action::AT_QCOM_EGM_POLL_ADD_CONF) =
                center.GetEvent(Action::AT_QCOM_EGM_POLL_ADD_CONF)->connect(
                    std::bind(&QcomSim::EGMPollAddConf, sim.get(), std::placeholders::_1, std::placeholders::_2));

            sim->Install(Action::AT_QCOM_TIME_DATE);
            center.Install(Action::AT_QCOM_TIME_DATE);

            sim->GetConnection(Action::AT_QCOM_TIME_DATE) =
                center.GetEvent(Action::AT_QCOM_TIME_DATE)->connect(
                    std::bind(&QcomSim::TimeDate, sim.get(), std::placeholders::_1, std::placeholders::_2));

            sim->Install(Action::AT_QCOM_LP_CURRENT_AMOUNT);
            center.Install(Action::AT_QCOM_LP_CURRENT_AMOUNT);

            sim->GetConnection(Action::AT_QCOM_LP_CURRENT_AMOUNT) =
                center.GetEvent(Action::AT_QCOM_LP_CURRENT_AMOUNT)->connect(
                    std::bind(&QcomSim::LPCurrentAmount, sim.get(), std::placeholders::_1, std::placeholders::_2));

            sim->Install(Action::AT_QCOM_GENERAL_PROMOTIONAL);
            center.Install(Action::AT_QCOM_GENERAL_PROMOTIONAL);

            sim->GetConnection(Action::AT_QCOM_GENERAL_PROMOTIONAL) =
                center.GetEvent(Action::AT_QCOM_GENERAL_PROMOTIONAL)->connect(
                    std::bind(&QcomSim::GeneralPromotional, sim.get(), std::placeholders::_1, std::placeholders::_2));

            sim->Install(Action::AT_QCOM_SITE_DETAIL);
            center.Install(Action::AT_QCOM_SITE_DETAIL);

            sim->GetConnection(Action::AT_QCOM_SITE_DETAIL) =
                center.GetEvent(Action::AT_QCOM_SITE_DETAIL)->connect(
                    std::bind(&QcomSim::SiteDetail, sim.get(), std::placeholders::_1, std::placeholders::_2));

            sim->Install(Action::AT_LIST_EGM);
            center.Install(Action::AT_LIST_EGM);

            sim->GetConnection(Action::AT_LIST_EGM) =
                center.GetEvent(Action::AT_LIST_EGM)->connect(
                    std::bind(&QcomSim::ListEGM, sim.get(), std::placeholders::_1, std::placeholders::_2));

            // add a command test log to test log
            sim->Install(Action::AT_PICK_EGM);
            center.Install(Action::AT_PICK_EGM);

            sim->GetConnection(Action::AT_PICK_EGM) =
                center.GetEvent(Action::AT_PICK_EGM)->connect(
                    std::bind(&QcomSim::PickEGM, sim.get(), std::placeholders::_1, std::placeholders::_2));

            sim->Install(Action::AT_QCOM_EGM_CONF_REQ);
            center.Install(Action::AT_QCOM_EGM_CONF_REQ);

            sim->GetConnection(Action::AT_QCOM_EGM_CONF_REQ) =
                center.GetEvent(Action::AT_QCOM_EGM_CONF_REQ)->connect(
                    std::bind(&QcomSim::EGMConfRequest, sim.get(), std::placeholders::_1, std::placeholders::_2));

            sim->Install(Action::AT_QCOM_EGM_CONF);
            center.Install(Action::AT_QCOM_EGM_CONF);

            sim->GetConnection(Action::AT_QCOM_EGM_CONF) =
                center.GetEvent(Action::AT_QCOM_EGM_CONF)->connect(
                    std::bind(&QcomSim::EGMConfiguration, sim.get(), std::placeholders::_1, std::placeholders::_2));

            sim->Install(Action::AT_QCOM_GAME_CONF);
            center.Install(Action::AT_QCOM_GAME_CONF);

            sim->GetConnection(Action::AT_QCOM_GAME_CONF) =
                center.GetEvent(Action::AT_QCOM_GAME_CONF)->connect(
                    std::bind(&QcomSim::GameConfiguration, sim.get(), std::placeholders::_1, std::placeholders::_2));

            sim->Install(Action::AT_QCOM_GAME_CONF_CHANGE);
            center.Install(Action::AT_QCOM_GAME_CONF_CHANGE);

            sim->GetConnection(Action::AT_QCOM_GAME_CONF_CHANGE) =
                center.GetEvent(Action::AT_QCOM_GAME_CONF_CHANGE)->connect(
                    std::bind(&QcomSim::GameConfigurationChange, sim.get(), std::placeholders::_1, std::placeholders::_2));

            sim->Install(Action::AT_QCOM_EGM_PARAMS);
            center.Install(Action::AT_QCOM_EGM_PARAMS);

            sim->GetConnection(Action::AT_QCOM_EGM_PARAMS) =
                center.GetEvent(Action::AT_QCOM_EGM_PARAMS)->connect(
                    std::bind(&QcomSim::EGMParameters, sim.get(), std::placeholders::_1, std::placeholders::_2));

            sim->Install(Action::AT_QCOM_PURGE_EVENTS);
            center.Install(Action::AT_QCOM_PURGE_EVENTS);

            sim->GetConnection(Action::AT_QCOM_PURGE_EVENTS) =
                center.GetEvent(Action::AT_QCOM_PURGE_EVENTS)->connect(
                    std::bind(&QcomSim::PurgeEvents, sim.get(), std::placeholders::_1, std::placeholders::_2));

            sim->Install(Action::AT_QCOM_GENERAL_STATUS);
            center.Install(Action::AT_QCOM_GENERAL_STATUS);

            sim->GetConnection(Action::AT_QCOM_GENERAL_STATUS) =
                center.GetEvent(Action::AT_QCOM_GENERAL_STATUS)->connect(
                    std::bind(&QcomSim::GeneralStatus, sim.get(), std::placeholders::_1, std::placeholders::_2));

            sim->Install(Action::AT_QCOM_PENDING);
            center.Install(Action::AT_QCOM_PENDING);

            sim->GetConnection(Action::AT_QCOM_PENDING) =
                center.GetEvent(Action::AT_QCOM_PENDING)->connect(
                    std::bind(&QcomSim::PendingPoll, sim.get(), std::placeholders::_1, std::placeholders::_2));

            sim->Install(Action::AT_QCOM_SEND);
            center.Install(Action::AT_QCOM_SEND);

            sim->GetConnection(Action::AT_QCOM_SEND) =
                center.GetEvent(Action::AT_QCOM_SEND)->connect(
                    std::bind(&QcomSim::SendPoll, sim.get(), std::placeholders::_1, std::placeholders::_2));

            sim->Install(Action::AT_RESET_DEV);
            center.Install(Action::AT_RESET_DEV);

            sim->GetConnection(Action::AT_RESET_DEV) =
            center.GetEvent(Action::AT_RESET_DEV)->connect(
                std::bind(&QcomSim::ChangeDev, sim.get(), std::placeholders::_1, std::placeholders::_2));

            sim->Install(Action::AT_QCOM_PROGRESSIVE_CONFIG);
            center.Install(Action::AT_QCOM_PROGRESSIVE_CONFIG);

            sim->GetConnection(Action::AT_QCOM_PROGRESSIVE_CONFIG) =
                center.GetEvent(Action::AT_QCOM_PROGRESSIVE_CONFIG)->connect(
                    std::bind(&QcomSim::ProgressiveChange, sim.get(), std::placeholders::_1, std::placeholders::_2));

            sim->Install(Action::AT_QCOM_EXTJP_INFO);
            center.Install(Action::AT_QCOM_EXTJP_INFO);

            sim->GetConnection(Action::AT_QCOM_EXTJP_INFO) =
                center.GetEvent(Action::AT_QCOM_EXTJP_INFO)->connect(
                    std::bind(&QcomSim::ExtJPInfo, sim.get(), std::placeholders::_1, std::placeholders::_2));

            return sim;
        }

        ActionFactoryPtr setup_qcom_factory()
        {
            ActionFactoryPtr curr = MakeSharedPtr<QcomActionFactory>();
            ActionCenter& center = ActionCenter::Instance();
            center.Install(Action::AT_HELP);
            center.GetEvent(Action::AT_HELP)->connect(std::bind(&ActionFactory::Help, curr.get(), std::placeholders::_1, std::placeholders::_2));
            return curr;
        }

    }

    void setup_sim()
    {
        g_sim = setup_qcom();
    }

    void setup_action_factory()
    {
        g_fac = setup_qcom_factory();
    }

    bool  quit_sim()
    {
        return quit;
    }
}



