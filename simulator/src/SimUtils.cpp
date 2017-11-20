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

            ActionCenter::ActionEventPtr evt = center.Install(Action::AT_QUIT);
            SG_ASSERT(evt);
            evt->connect(std::bind(on_quit, std::placeholders::_1, std::placeholders::_2));

            Simulator::Connection *connection = sim->Install(Action::AT_QCOM_SEEKEGM);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_QCOM_SEEKEGM);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::SeekEGM, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_QCOM_EGM_POLL_ADD_CONF);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_QCOM_EGM_POLL_ADD_CONF);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::EGMPollAddConf, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_QCOM_TIME_DATE);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_QCOM_TIME_DATE);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::TimeDate, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_QCOM_LP_CURRENT_AMOUNT);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_QCOM_LP_CURRENT_AMOUNT);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::LPCurrentAmount, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_QCOM_GENERAL_PROMOTIONAL);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_QCOM_GENERAL_PROMOTIONAL);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::GeneralPromotional, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_QCOM_SITE_DETAIL);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_QCOM_SITE_DETAIL);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::SiteDetail, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_LIST_EGM);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_LIST_EGM);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::ListEGM, sim.get(), std::placeholders::_1, std::placeholders::_2));

            // add a command test log to test log
            connection = sim->Install(Action::AT_PICK_EGM);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_PICK_EGM);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::PickEGM, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_QCOM_EGM_CONF_REQ);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_QCOM_EGM_CONF_REQ);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::EGMConfRequest, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_QCOM_EGM_CONF);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_QCOM_EGM_CONF);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::EGMConfiguration, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_QCOM_GAME_CONF);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_QCOM_GAME_CONF);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::GameConfiguration, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_QCOM_GAME_CONF_CHANGE);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_QCOM_GAME_CONF_CHANGE);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::GameConfigurationChange, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_QCOM_EGM_PARAMS);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_QCOM_EGM_PARAMS);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::EGMParameters, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_QCOM_PURGE_EVENTS);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_QCOM_PURGE_EVENTS);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::PurgeEvents, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_QCOM_GENERAL_STATUS);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_QCOM_GENERAL_STATUS);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::GeneralStatus, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_QCOM_PENDING);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_QCOM_PENDING);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::PendingPoll, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_QCOM_SEND);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_QCOM_SEND);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::SendPoll, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_RESET_DEV);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_RESET_DEV);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::ChangeDev, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_QCOM_PROGRESSIVE_CONFIG);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_QCOM_PROGRESSIVE_CONFIG);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::ProgressiveChange, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_QCOM_EXTJP_INFO);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_QCOM_EXTJP_INFO);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::ExtJPInfo, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_QCOM_PROGHASH_REQUEST);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_QCOM_PROGHASH_REQUEST);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::ProgHashRequest, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_QCOM_SYSLOCKUP_REQUEST);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_QCOM_SYSLOCKUP_REQUEST);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::SysLockupRequest, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_QCOM_CASH_TICKET_OUT_ACK);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_QCOM_CASH_TICKET_OUT_ACK);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::CashTicketOutAck, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_QCOM_CASH_TICKET_IN_ACK);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_QCOM_CASH_TICKET_IN_ACK);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::CashTicketInAck, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_QCOM_CASH_TICKET_OUT_REQ);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_QCOM_CASH_TICKET_OUT_REQ);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::CashTicketOutRequest, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_QCOM_EGM_GENERAL_MAINTENANCE);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_QCOM_EGM_GENERAL_MAINTENANCE);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::EGMGeneralMaintenance, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_QCOM_REQ_ALL_LOGGED_EVENTS);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_QCOM_REQ_ALL_LOGGED_EVENTS);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::RequestAllLoggedEvents, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_QCOM_NOTE_ACCEPTOR_MAINTENANCE);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_QCOM_NOTE_ACCEPTOR_MAINTENANCE);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::NoteAcceptorMaintenance, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_QCOM_HOPPPER_TICKET_PRINTER);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_QCOM_HOPPPER_TICKET_PRINTER);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::HopperTicketPrinterMaintenance, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_QCOM_LP_AWARD_ACK);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_QCOM_LP_AWARD_ACK);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::LPAwardAck, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_QCOM_GENERAL_RESET);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_QCOM_GENERAL_RESET);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::GeneralReset, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_QCOM_SPAM);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_QCOM_SPAM);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::SPAM, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_QCOM_TOWER_LIGHT_MAINTENANCE);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_QCOM_TOWER_LIGHT_MAINTENANCE);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::TowerLightMaintenance, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_QCOM_ECT_TO_EGM);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_QCOM_ECT_TO_EGM);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::ECTToEGM, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_QCOM_ECT_FROM_EGM_REQ);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_QCOM_ECT_FROM_EGM_REQ);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::ECTFromEGMLockupRequest, sim.get(), std::placeholders::_1, std::placeholders::_2));

            connection = sim->Install(Action::AT_QCOM_ECT_LOCKUP_RESET);
            SG_ASSERT(connection);
            evt = center.Install(Action::AT_QCOM_ECT_LOCKUP_RESET);
            SG_ASSERT(evt);

            *connection = evt->connect(std::bind(&QcomSim::ECTLockupReset, sim.get(), std::placeholders::_1, std::placeholders::_2));

            return sim;
        }

        ActionFactoryPtr setup_qcom_factory()
        {
            ActionFactoryPtr curr = MakeSharedPtr<QcomActionFactory>();
            ActionCenter& center = ActionCenter::Instance();
            center.Install(Action::AT_HELP)->connect(std::bind(&ActionFactory::Help, curr.get(), std::placeholders::_1, std::placeholders::_2));
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



