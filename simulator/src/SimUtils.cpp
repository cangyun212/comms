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
                bind(on_quit, std::placeholders::_1, std::placeholders::_2));

            sim->Install(Action::AT_QCOM_SEEKEGM);
            center.Install(Action::AT_QCOM_SEEKEGM);

            sim->GetConnection(Action::AT_QCOM_SEEKEGM) =
                center.GetEvent(Action::AT_QCOM_SEEKEGM)->connect(
                    bind(&QcomSim::SeekEGM, sim.get(), std::placeholders::_1, std::placeholders::_2));

            sim->Install(Action::AT_QCOM_EGM_POLL_ADD_CONF);
            center.Install(Action::AT_QCOM_EGM_POLL_ADD_CONF);

            sim->GetConnection(Action::AT_QCOM_EGM_POLL_ADD_CONF) =
                center.GetEvent(Action::AT_QCOM_EGM_POLL_ADD_CONF)->connect(
                    bind(&QcomSim::EGMPollAddConf, sim.get(), std::placeholders::_1, std::placeholders::_2));

            sim->Install(Action::AT_LIST_EGM);
            center.Install(Action::AT_LIST_EGM);

            sim->GetConnection(Action::AT_LIST_EGM) =
                center.GetEvent(Action::AT_LIST_EGM)->connect(
                    bind(&QcomSim::ListEGM, sim.get(), std::placeholders::_1, std::placeholders::_2));
            // add a command test log to test log
            sim->Install(Action::AT_PICK_EGM);
            center.Install(Action::AT_PICK_EGM);

            sim->GetConnection(Action::AT_PICK_EGM) =
                center.GetEvent(Action::AT_PICK_EGM)->connect(
                    bind(&QcomSim::PickEGM, sim.get(), std::placeholders::_1, std::placeholders::_2));

            sim->Install(Action::AT_QCOM_EGM_CONF_REQ);
            center.Install(Action::AT_QCOM_EGM_CONF_REQ);

            sim->GetConnection(Action::AT_QCOM_EGM_CONF_REQ) =
                center.GetEvent(Action::AT_QCOM_EGM_CONF_REQ)->connect(
                    bind(&QcomSim::EGMConfRequest, sim.get(), std::placeholders::_1, std::placeholders::_2));

            sim->Install(Action::AT_QCOM_EGM_CONF);
            center.Install(Action::AT_QCOM_EGM_CONF);

            sim->GetConnection(Action::AT_QCOM_EGM_CONF) =
                center.GetEvent(Action::AT_QCOM_EGM_CONF)->connect(
                    bind(&QcomSim::EGMConfiguration, sim.get(), std::placeholders::_1, std::placeholders::_2));

        sim->Install(Action::AT_QCOM_GAME_CONF);
        center.Install(Action::AT_QCOM_GAME_CONF);

        sim->GetConnection(Action::AT_QCOM_GAME_CONF) =
            center.GetEvent(Action::AT_QCOM_GAME_CONF)->connect(
                    bind(&QcomSim::GameConfiguration, sim.get(), std::placeholders::_1, std::placeholders::_2));

        sim->Install(Action::AT_QCOM_GAME_CONF_CHANGE);
        center.Install(Action::AT_QCOM_GAME_CONF_CHANGE);

        sim->GetConnection(Action::AT_QCOM_GAME_CONF_CHANGE) =
                center.GetEvent(Action::AT_QCOM_GAME_CONF_CHANGE)->connect(
                    bind(&QcomSim::GameConfigurationChange, sim.get(), std::placeholders::_1, std::placeholders::_2));

        sim->Install(Action::AT_QCOM_EGM_PARAMS);
        center.Install(Action::AT_QCOM_EGM_PARAMS);

        sim->GetConnection(Action::AT_QCOM_EGM_PARAMS) =
                center.GetEvent(Action::AT_QCOM_EGM_PARAMS)->connect(
                    bind(&QcomSim::EGMParameters, sim.get(), std::placeholders::_1, std::placeholders::_2));

        sim->Install(Action::AT_QCOM_PURGE_EVENTS);
        center.Install(Action::AT_QCOM_PURGE_EVENTS);

        sim->GetConnection(Action::AT_QCOM_PURGE_EVENTS) =
                center.GetEvent(Action::AT_QCOM_PURGE_EVENTS)->connect(
                    bind(&QcomSim::PurgeEvents, sim.get(), std::placeholders::_1, std::placeholders::_2));

        //For broadcast Action
        sim->Install(Action::AT_QCOM_BROADCAST);
        center.Install(Action::AT_QCOM_BROADCAST);

            sim->GetConnection(Action::AT_QCOM_BROADCAST) =
                center.GetEvent(Action::AT_QCOM_BROADCAST)->connect(
                    bind(&QcomSim::SendBroadcast, sim.get(), std::placeholders::_1, std::placeholders::_2));

            center.Install(Action::AT_RESET_DEV);
            center.GetEvent(Action::AT_RESET_DEV)->connect(
                bind(&QcomSim::ChangeDev, sim.get(), std::placeholders::_1, std::placeholders::_2));

            return sim;
        }

        ActionFactoryPtr setup_qcom_factory()
        {
            ActionFactoryPtr curr = MakeSharedPtr<QcomActionFactory>();
            ActionCenter& center = ActionCenter::Instance();
            center.Install(Action::AT_HELP);
            center.GetEvent(Action::AT_HELP)->connect(bind(&ActionFactory::Help, curr.get(), std::placeholders::_1, std::placeholders::_2));
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



