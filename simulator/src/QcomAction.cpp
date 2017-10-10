#include "Core.hpp"

#include <algorithm>

#include "boost/program_options.hpp"

#include "Utils.hpp"
#include "Comms.hpp"
#include "Qcom/qogr/qogr_qcom.h"
#include "QcomAction.hpp"
#include "ActionOptions.hpp"

#define SG_QCOMAC_MAX_PROG_LEVEL    QCOM_MAX_PROGR_LEV_PER_GAME

namespace po = boost::program_options;
namespace nm = boost::numeric;

namespace sg 
{

    QcomSeekEGMAction::QcomSeekEGMAction()
        : Action(Action::AT_QCOM_SEEKEGM)
    {

    }

    QcomSeekEGMAction::~QcomSeekEGMAction()
    {

    }

    ActionPtr QcomSeekEGMAction::Clone()
    {
        return Action::DoClone<QcomSeekEGMAction>();
    }

    const char* QcomSeekEGMAction::Description() const
    {
        static const char* des = "\tSeek all EGM :\n\t\tsk,seek\n";
        return des;
    }


    QcomEGMPollAddConfAction::QcomEGMPollAddConfAction()
        : Action(Action::AT_QCOM_EGM_POLL_ADD_CONF)
    {

    }

    QcomEGMPollAddConfAction::~QcomEGMPollAddConfAction()
    {

    }

    ActionPtr QcomEGMPollAddConfAction::Clone()
    {
        return Action::DoClone<QcomEGMPollAddConfAction>();
    }

    const char* QcomEGMPollAddConfAction::Description() const
    {
        static const char* des = "\tConfig EGM poll address :\n\t\tpa,polladdress\n";
        return des;
    }

    uint8 QcomEGMConfRequestAction::s_mef = 1;
    uint8 QcomEGMConfRequestAction::s_gcr = 1;
    uint8 QcomEGMConfRequestAction::s_psn = 1;

    QcomEGMConfRequestAction::QcomEGMConfRequestAction()
        : Action(Action::AT_QCOM_EGM_CONF_REQ)
    {
    }

    QcomEGMConfRequestAction::~QcomEGMConfRequestAction()
    {

    }

    bool QcomEGMConfRequestAction::Parse(const ActionArgs &args)
    {
        bool res = false;

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: configreq [options]\n");
            COMMS_PRINT_BLOCK(vis_desc);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();

            res = false;
        }
        else
        {
            res = true;
        }



        return res;
    }

    void QcomEGMConfRequestAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();

            m_options->AddOption(ActionOption("mef,m", "enable egm machine", Value<uint8>(&s_mef)));
            m_options->AddOption(ActionOption("gcr,g", "commands egm queue the EGM Game configuration Response", Value<uint8>(&s_gcr)));
            m_options->AddOption(ActionOption("psn,p", "reset all Poll Sequence Numbers", Value<uint8>(&s_psn)));
            m_options->AddOption(ActionOption("help,h", "help message"));
        }
    }

    ActionPtr QcomEGMConfRequestAction::Clone()
    {
        return Action::DoClone<QcomEGMConfRequestAction>();
    }

    const char* QcomEGMConfRequestAction::Description() const
    {
        const char* des = "\tEGM configuration request :\n\t\tcfrq,configreq\n";
        return des;
    }

    uint8     QcomEGMConfAction::s_jur = 0;
    uint32    QcomEGMConfAction::s_den = 1;
    uint32    QcomEGMConfAction::s_tok = 100;
    uint32    QcomEGMConfAction::s_maxden = 100;
    uint16    QcomEGMConfAction::s_minrtp = 8500;
    uint16    QcomEGMConfAction::s_maxrtp = 10000;
    uint16    QcomEGMConfAction::s_maxsd = 15;
    uint16    QcomEGMConfAction::s_maxlines = 1024;
    uint32    QcomEGMConfAction::s_maxbet = 5000;
    uint32    QcomEGMConfAction::s_maxnpwin = 100000;
    uint32    QcomEGMConfAction::s_maxpwin = 2500000;
    uint32    QcomEGMConfAction::s_maxect = 1000000;

    QcomEGMConfAction::QcomEGMConfAction()
                    : Action(Action::AT_QCOM_EGM_CONF)
    {
    }

    QcomEGMConfAction::~QcomEGMConfAction()
    {

    }

    bool QcomEGMConfAction::Parse(const Action::ActionArgs &args)
    {
        bool res = false;

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: egmconfig [options]\n");
            COMMS_PRINT_BLOCK(vis_desc);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();

            res = false;
        }
        else
        {
            res = true;
        }


        return res;
    }

    void QcomEGMConfAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();

            m_options->AddOption(ActionOption("jur","jurisdictions", Value<uint8>(&s_jur)));
            m_options->AddOption(ActionOption("den", "credit denomination", Value<uint32>(&s_den)));
            m_options->AddOption(ActionOption("tok", "coin/token denomination", Value<uint32>(&s_tok)));
            m_options->AddOption(ActionOption("maxden", "EGM credit meter max denomination", Value<uint32>(&s_maxden)));
            m_options->AddOption(ActionOption("minrtp", "minimum RTP", Value<uint16>(&s_minrtp)));
            m_options->AddOption(ActionOption("maxrtp", "maximum RTP", Value<uint16>(&s_maxrtp)));

            m_options->AddOption(ActionOption(
                "maxsd",
                "regulatory maximum EGM game theoretical standard deviation", 
                Value<uint16>(&s_maxsd)));

            m_options->AddOption(ActionOption(
                "maxlines", 
                "regulatory maximum number of playlines in any game in the EGM", 
                Value<uint16>(&s_maxlines)));

            m_options->AddOption(ActionOption("maxbet", "maximum bet per play", Value<uint32>(&s_maxbet)));

            m_options->AddOption(ActionOption(
                "maxnpwin", 
                "regulatory maximum non-progressive EGM win permitted uint32 any game element", 
                Value<uint32>(&s_maxnpwin)));

            m_options->AddOption(ActionOption(
                "maxpwin", 
                "regulatory maximum SAP progressive EGM win permitted per SAP hit", 
                Value<uint32>(&s_maxpwin)));

            m_options->AddOption(ActionOption("maxect", "regulatory maximum allowable ECT to/from the EGM", Value<uint32>(&s_maxect)));
            m_options->AddOption(ActionOption("help,h", "help message"));
        }
    }

    ActionPtr QcomEGMConfAction::Clone()
    {
        return Action::DoClone<QcomEGMConfAction>();
    }

    const char* QcomEGMConfAction::Description() const
    {
        static const char* des = "\tEGM configuration :\n\t\tegmcf,egmconfig\n";
        return des;
    }

    uint16 QcomGameConfigurationAction::s_gvn = 0;
    uint16 QcomGameConfigurationAction::s_pgid = 0xFFFF;
    uint8  QcomGameConfigurationAction::s_var = 0;
    uint8  QcomGameConfigurationAction::s_var_lock = 0;
    uint8  QcomGameConfigurationAction::s_game_enable = 0;
    std::vector<uint8> QcomGameConfigurationAction::s_lp;
    std::vector<uint32> QcomGameConfigurationAction::s_camt;

    QcomGameConfigurationAction::QcomGameConfigurationAction()
        : Action(Action::AT_QCOM_GAME_CONF)
    {
    }

    QcomGameConfigurationAction::~QcomGameConfigurationAction()
    {

    }

    bool QcomGameConfigurationAction::Parse(const Action::ActionArgs &args)
    {
        bool res = false;

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: gameconfig [options]\n");
            COMMS_PRINT_BLOCK(vis_desc);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();
        }
        else
        {
            if (s_lp.size() == s_camt.size() && s_lp.size() < SG_QCOMAC_MAX_PROG_LEVEL)
                res = true;
            else
            {
                COMMS_LOG(
                    boost::format("Entry number of 'jptype' and 'amount' option must be equal and less then %||\n") %
                    SG_QCOMAC_MAX_PROG_LEVEL, CLL_Error);
            }
        }

        return res;
    }

    void QcomGameConfigurationAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();

            m_options->AddOption(
                ActionOption("gvn", "game version number, indentifies the game being configured", Value<uint16>(&s_gvn)));
            m_options->AddOption(
                ActionOption("pgid", "linked progressive group ID", Value<uint16>(&s_pgid)));
            m_options->AddOption(
                ActionOption("var", "game variation number, indicates to the EGM what variation to use", Value<uint8>(&s_var)));
            m_options->AddOption(
                ActionOption("varlock", "variation lock, if set, the EGM must lock the variation and \
                             not allow the variation to be changed", Value<uint8>(&s_var_lock)));
            m_options->AddOption(
                ActionOption("gameenable", "game enable flag, if set, enable game denoted by GVN", Value<uint8>(&s_game_enable)));
            m_options->AddOption(
                ActionOption("jptype", "progressive level type, if set, denotes the level is to be set as LP", 
                    Value< std::vector<uint8> >(&s_lp), true));
            m_options->AddOption(
                ActionOption("amount", "initial jackpot contribution/current amount for progressive level", 
                    Value< std::vector<uint32> >(&s_camt), true));
            m_options->AddOption(ActionOption("help,h", "help message"));
        }
    }

    ActionPtr QcomGameConfigurationAction::Clone()
    {
        return Action::DoClone<QcomGameConfigurationAction>();
    }

    const char* QcomGameConfigurationAction::Description() const
    {
        static const char* des = "\tGame configuration :\n\t\tgconf,gameconfig\n";
        return des;
    }

    uint8_t QcomGameConfigurationAction::ProgressiveConfig(uint8_t * lp, uint32_t * camt)
    {
        size_t pnum = s_lp.size(); //< s_camt.size() ? s_lp.size() : s_camt.size();

        SG_ASSERT(pnum == s_camt.size() && pnum < SG_QCOMAC_MAX_PROG_LEVEL); // check pnum dynamically instead of using debug assert

        for (size_t i = 0; i < pnum; ++i)
        {
            lp[i] = s_lp[i];
            camt[i] = s_camt[i];
        }

        return static_cast<uint8_t>(pnum);
    }

    uint16 QcomGameConfigurationChangeAction::s_gvn = 0;
    uint16 QcomGameConfigurationChangeAction::s_pgid = 0xFFFF;
    uint8  QcomGameConfigurationChangeAction::s_var = 0;
    uint8  QcomGameConfigurationChangeAction::s_game_enable = 1;

    QcomGameConfigurationChangeAction::QcomGameConfigurationChangeAction()
        : Action(Action::AT_QCOM_GAME_CONF_CHANGE)
    {
        m_options = MakeSharedPtr<ActionOptions>();


    }

    QcomGameConfigurationChangeAction::~QcomGameConfigurationChangeAction()
    {

    }
    bool QcomGameConfigurationChangeAction::Parse(const Action::ActionArgs &args)
    {
        bool res = false;

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: gconfchg [options]\n");
            COMMS_PRINT_BLOCK(vis_desc);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();

            res = false;
        }
        else
        {
            res = true;
        }

        return res;
    }

    void QcomGameConfigurationChangeAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();

            m_options->AddOption(ActionOption("gvn", "TODO", Value<uint16>(&s_gvn)));
            m_options->AddOption(ActionOption("pgid", "TODO", Value<uint16>(&s_pgid)));
            m_options->AddOption(ActionOption("var", "TODO", Value<uint8>(&s_var)));
            m_options->AddOption(ActionOption("gameenable", "TODO", Value<uint8>(&s_game_enable)));
            m_options->AddOption(ActionOption("help,h", "help message"));
        }
    }

    ActionPtr QcomGameConfigurationChangeAction::Clone()
    {
        return Action::DoClone<QcomGameConfigurationChangeAction>();
    }

    const char *QcomGameConfigurationChangeAction::Description() const
    {
        static const char* des = "\tGame configuration change:\n\t\tgconfchg,gameconfigchange\n";
        return des;
    }


     uint8   QcomEGMParametersAction::s_reserve = 1;
     uint8   QcomEGMParametersAction::s_autoplay = 0;
     uint8   QcomEGMParametersAction::s_crlimitmode = 0;
     uint8   QcomEGMParametersAction::s_opr = 0;
     uint32  QcomEGMParametersAction::s_lwin = 1000000;
     uint32  QcomEGMParametersAction::s_crlimit = 1000000;
     uint8   QcomEGMParametersAction::s_dumax = 5;
     uint32  QcomEGMParametersAction::s_dulimit = 1000000;
     int16   QcomEGMParametersAction::s_tzadj = 0;
     uint32  QcomEGMParametersAction::s_pwrtime = 900;
     uint8   QcomEGMParametersAction::s_pid = 0;
     uint16  QcomEGMParametersAction::s_eodt = 180;
     uint32  QcomEGMParametersAction::s_npwinp = 1000000;
     uint32  QcomEGMParametersAction::s_sapwinp = 1000000;

    QcomEGMParametersAction::QcomEGMParametersAction()
        :Action(Action::AT_QCOM_EGM_PARAMS)
    {


    }

    QcomEGMParametersAction::~QcomEGMParametersAction()
    {

    }

    bool QcomEGMParametersAction::Parse(const Action::ActionArgs &args)
    {
        bool res = false;

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: egmparams [options]\n");
            COMMS_PRINT_BLOCK(vis_desc);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();

            res = false;
        }
        else
        {
            res = true;
        }

        return res;
    }

    void QcomEGMParametersAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();

            m_options->AddOption(ActionOption("reserve", "Enable or Disable RESERV", Value<uint8>(&s_reserve)));
            m_options->AddOption(ActionOption("autoplay", "Enable or Disable autoplay", Value<uint8>(&s_autoplay)));
            m_options->AddOption(ActionOption("crlimitmode", "credit limit", Value<uint8>(&s_crlimitmode)));
            m_options->AddOption(ActionOption("opr", "Monitoring system operator ID", Value<uint8>(&s_opr)));
            m_options->AddOption(ActionOption("lwin", "Large win lockup threshold", Value<uint32>(&s_lwin)));
            m_options->AddOption(ActionOption("crlimit", "Credit-in lockout value", Value<uint32>(&s_crlimit)));
            m_options->AddOption(ActionOption("dumax", "Maximum allowable number of Gambles", Value<uint8>(&s_dumax)));
            m_options->AddOption(ActionOption("dulimit", "Double-Up/Gamble Limit", Value<uint32>(&s_dulimit)));
            m_options->AddOption(ActionOption("tzadj", "Time zone adjust", Value<int16>(&s_tzadj)));
            m_options->AddOption(ActionOption("pwrtime", "Power-save Time-out value", Value<uint32>(&s_pwrtime)));
            m_options->AddOption(ActionOption("pid", "Player Information Display", Value<uint8>(&s_pid)));
            m_options->AddOption(ActionOption("eodt", "End of the day time", Value<uint16>(&s_eodt)));
            m_options->AddOption(ActionOption("npwinp", "Non-Progressive Win Payout Threshold", Value<uint32>(&s_npwinp)));
            m_options->AddOption(ActionOption("sapwinp", "Stand alone progressive win payout threshold", Value<uint32>(&s_sapwinp)));
            m_options->AddOption(ActionOption("help,h", "help message"));
        }
    }

    ActionPtr QcomEGMParametersAction::Clone()
    {
        return Action::DoClone<QcomEGMParametersAction>();
    }

    const char *QcomEGMParametersAction::Description() const
    {
        static const char* des = "\tEGM Parameters:\n\t\tegmparams,egmparameters\n";
        return des;
    }


    uint8  QcomPurgeEventsAction::m_evtno = 255;

    QcomPurgeEventsAction::QcomPurgeEventsAction()
        :Action(Action::AT_QCOM_PURGE_EVENTS)
    {


    }

    QcomPurgeEventsAction::~QcomPurgeEventsAction()
    {

    }

    bool QcomPurgeEventsAction::Parse(const ActionArgs &args)
    {
        bool res = false;

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: gameconfig [options]\n");
            COMMS_PRINT_BLOCK(vis_desc);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();

            res = false;
        }
        else
        {
            res = true;
        }

        return res;
    }

    void QcomPurgeEventsAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();

            m_options->AddOption(ActionOption("evnto", "Event sequence number", Value<uint8>(&m_evtno)));

            m_options->AddOption(ActionOption("help,h", "help message"));
        }
    }

    ActionPtr QcomPurgeEventsAction::Clone()
    {
        return Action::DoClone<QcomPurgeEventsAction>();
    }

    const char *QcomPurgeEventsAction::Description() const
    {
        static const char* des = "\tPurge Events:\n\t\tpevents,purgeevents\n";
        return des;
    }

    QcomTimeDateAction::QcomTimeDateAction()
        : Action(Action::AT_QCOM_TIME_DATE)
    {
    }

    QcomTimeDateAction::~QcomTimeDateAction()
    {
    }

    ActionPtr QcomTimeDateAction::Clone()
    {
        return Action::DoClone<QcomTimeDateAction>();
    }

    const char * QcomTimeDateAction::Description() const
    {
        static const char* des = "\tTime Date Broadcast: \n\t\tsend time and date of SC\n";
        return des;
    }

    std::vector<uint32> QcomLPCurrentAmountAction::s_lpamt;
    std::vector<uint16> QcomLPCurrentAmountAction::s_pgid;

    QcomLPCurrentAmountAction::QcomLPCurrentAmountAction()
        : Action(Action::AT_QCOM_LP_CURRENT_AMOUNT)
    {
    }

    QcomLPCurrentAmountAction::~QcomLPCurrentAmountAction()
    {
    }

    bool QcomLPCurrentAmountAction::Parse(const ActionArgs & args)
    {
        bool res = false;

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: lp [options]\n");
            COMMS_PRINT_BLOCK(vis_desc);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();

            res = false;
        }
        else
        {
            res = true;
        }

        return res;
    }

    void QcomLPCurrentAmountAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();
            m_options->AddOption(
                ActionOption("lpamt", "Linked Progressive jackpot current amount", Value<std::vector<uint32> >(&s_lpamt), true));
            m_options->AddOption(
                ActionOption("pgid", "Linked Progressive Group ID", Value<std::vector<uint16> >(&s_pgid), true));

            m_options->AddOption(ActionOption("help,h", "help message"));
        }
    }

    ActionPtr QcomLPCurrentAmountAction::Clone()
    {
        return Action::DoClone<QcomLPCurrentAmountAction>();
    }

    const char * QcomLPCurrentAmountAction::Description() const
    {
        static const char* des = "\tLP Current Amount Broadcast: \n\t\tsend out the current progressive amount to all LP games\n";
        return des;
    }

    uint8_t QcomLPCurrentAmountAction::LPData(uint32_t * lpamt, uint16_t * pgid, uint8_t * plvl)
    {
        size_t pnum = s_lpamt.size() < s_pgid.size() ? s_lpamt.size() : s_pgid.size();

        for (size_t i = 0; i < pnum; ++i)
        {
            lpamt[i] = s_lpamt[i];
            pgid[i] = s_pgid[i];
            plvl[i] = static_cast<uint8_t>(i);
        }

        return static_cast<uint8_t>(pnum);
    }

    std::string QcomGeneralPromotionalAction::s_text;

    QcomGeneralPromotionalAction::QcomGeneralPromotionalAction()
        : Action(Action::AT_QCOM_GENERAL_PROMOTIONAL)
    {
    }

    QcomGeneralPromotionalAction::~QcomGeneralPromotionalAction()
    {
    }

    bool QcomGeneralPromotionalAction::Parse(const ActionArgs & args)
    {
        bool res = false;

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: gp [options]\n");
            COMMS_PRINT_BLOCK(vis_desc);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();

            res = false;
        }
        else
        {
            res = true;
        }

        return res;
    }

    void QcomGeneralPromotionalAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();
            m_options->AddOption(ActionOption("text", "Text displayed to player", Value<std::string>(&s_text)));
            m_options->AddOption(ActionOption("help,h", "help message"));
        }
    }

    ActionPtr QcomGeneralPromotionalAction::Clone()
    {
        return Action::DoClone<QcomGeneralPromotionalAction>();
    }

    const char * QcomGeneralPromotionalAction::Description() const
    {
        static const char* des = "\tGeneral Promotional Broadcast: \n\t\tsend out to request EGM to display an arbitrary text message\
                                    to the player while it is in idle mode\n";

        return des;
    }

    std::string QcomSiteDetailAction::s_stext;
    std::string QcomSiteDetailAction::s_ltext;

    QcomSiteDetailAction::QcomSiteDetailAction()
        : Action(Action::AT_QCOM_SITE_DETAIL)
    {
    }

    QcomSiteDetailAction::~QcomSiteDetailAction()
    {
    }

    bool QcomSiteDetailAction::Parse(const ActionArgs & args)
    {
        bool res = false;

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: sd [options]\n");
            COMMS_PRINT_BLOCK(vis_desc);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();

            res = false;
        }
        else
        {
            res = true;
        }

        return res;
    }

    void QcomSiteDetailAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();
            m_options->AddOption(ActionOption("stext", "Name of licensed venue", Value<std::string>(&s_stext)));
            m_options->AddOption(ActionOption("ltext", "Address/contact details of licensed venue", Value<std::string>(&s_ltext)));
            m_options->AddOption(ActionOption("help,h", "help message"));
        }
    }

    ActionPtr QcomSiteDetailAction::Clone()
    {
        return Action::DoClone<QcomSiteDetailAction>();
    }

    const char * QcomSiteDetailAction::Description() const
    {
        static const char* des = "\tSite Detail Broadcast: \n\t\tThis message is in support of cash out ticket printing\n";
        return des;
    }

    QcomEGMGeneralStatusAction::QcomEGMGeneralStatusAction()
        : Action(Action::AT_QCOM_GENERAL_STATUS)
    {
    }

    QcomEGMGeneralStatusAction::~QcomEGMGeneralStatusAction()
    {
    }

    ActionPtr QcomEGMGeneralStatusAction::Clone()
    {
        return Action::DoClone<QcomEGMGeneralStatusAction>();
    }

    const char * QcomEGMGeneralStatusAction::Description() const
    {
        static const char* des = "\tGeneral Status: \n\t\tSolicit a response from the EGM to check if the status of the EGM has changed\n";
        return des;
    }

    size_t QcomPendingAction::s_pollnum = 2;

    QcomPendingAction::QcomPendingAction()
        : Action(Action::AT_QCOM_PENDING)
    {
    }

    QcomPendingAction::~QcomPendingAction()
    {
    }

    bool QcomPendingAction::Parse(const ActionArgs & args)
    {
        bool res = false;

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: sd [options]\n");
            COMMS_PRINT_BLOCK(vis_desc);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();

            res = false;
        }
        else
        {
            res = true;
        }

        return res;
    }

    void QcomPendingAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();
            m_options->AddOption(ActionOption("pollnum", "number of pending polls", Value<size_t>(&s_pollnum)));
            m_options->AddOption(ActionOption("help,h", "help message"));
        }
    }

    ActionPtr QcomPendingAction::Clone()
    {
        return Action::DoClone<QcomPendingAction>();
    }

    const char * QcomPendingAction::Description() const
    {
        static const char* des = "\tPending Poll:\n\t\tPending all polls following this command\n";
        return des;
    }

    QcomSendAction::QcomSendAction()
        : Action(Action::AT_QCOM_SEND)
    {
    }

    QcomSendAction::~QcomSendAction()
    {
    }

    ActionPtr QcomSendAction::Clone()
    {
        return Action::DoClone<QcomSendAction>();
    }

    const char * QcomSendAction::Description() const
    {
        static const char* des = "\tSend Poll:\n\t\tSend all pending polls\n";
        return des;
    }

    std::vector<uint32> QcomProgressiveConfigAction::s_sup;
    std::vector<uint32> QcomProgressiveConfigAction::s_pinc;
    std::vector<uint32> QcomProgressiveConfigAction::s_ceil;
    std::vector<uint32> QcomProgressiveConfigAction::s_auxrtp;
    uint16 QcomProgressiveConfigAction::s_gvn = 0;

    QcomProgressiveConfigAction::QcomProgressiveConfigAction()
        : Action(AT_QCOM_PROGRESSIVE_CONFIG)
    {
    }

    QcomProgressiveConfigAction::~QcomProgressiveConfigAction()
    {
    }

    bool QcomProgressiveConfigAction::Parse(const ActionArgs & args)
    {
        bool res = false;

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: progressive [options]\n");
            COMMS_PRINT_BLOCK(vis_desc);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();
        }
        else
        {
            if (s_sup.size() == s_pinc.size() &&
                s_sup.size() == s_ceil.size() &&
                s_sup.size() == s_auxrtp.size() &&
                s_sup.size() <= SG_QCOMAC_MAX_PROG_LEVEL)
                res = true;
            else
            {
                COMMS_LOG(
                    boost::format("Entry number of 'sup', 'pinc', 'ceil' and 'auxrtp' must be equal and less than %||\n") %
                    SG_QCOMAC_MAX_PROG_LEVEL, CLL_Error);
            }
        }

        return res;
    }

    void QcomProgressiveConfigAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();
            m_options->AddOption(
                ActionOption("sup", "new jackpot level start-up amount", Value<std::vector<uint32> >(&s_sup), true));
            m_options->AddOption(
                ActionOption("pinc", "new jackpot level percentage increment x 10000", Value<std::vector<uint32> >(&s_pinc), true));
            m_options->AddOption(
                ActionOption("ceil", "new jackpot level ceiling", Value<std::vector<uint32> >(&s_ceil), true));
            m_options->AddOption(
                ActionOption("auxrtp", "new auxiliary RTP for the level x 10000", Value<std::vector<uint32> >(&s_auxrtp), true));
            m_options->AddOption(
                ActionOption("gvn", "game version number, identifies the game being re-configured", Value<uint16>(&s_gvn)));
            m_options->AddOption(ActionOption("help,h", "help message"));
        }
    }

    ActionPtr QcomProgressiveConfigAction::Clone()
    {
        return Action::DoClone<QcomProgressiveConfigAction>();
    }

    const char * QcomProgressiveConfigAction::Description() const
    {
        static const char* des = "\tProgressive Config Change Poll:\n\t\tChange a game's current SAP configuration and inform the\
                                EGM of the current parameters being used for its LP level (if any)\n";
        return des;
    }

    uint8_t QcomProgressiveConfigAction::ProgChangeData(uint32_t * sup, uint32_t * pinc, uint32_t * ceil, uint32_t *auxrtp) const
    {
        size_t num = s_sup.size();

        SG_ASSERT(num == s_pinc.size() && num == s_ceil.size() && num <= SG_QCOMAC_MAX_PROG_LEVEL);

        for (size_t i = 0; i < num; ++i)
        {
            sup[i] = s_sup[i];
            pinc[i] = s_pinc[i];
            ceil[i] = s_ceil[i];
            auxrtp[i] = s_auxrtp[i];
        }

        return static_cast<uint8_t>(num);
    }

    std::vector<uint16> QcomExtJPInfoAction::s_epgid;
    std::vector<uint8> QcomExtJPInfoAction::s_umf;
    std::vector<std::string> QcomExtJPInfoAction::s_name;
    uint16 QcomExtJPInfoAction::s_rtp = 0;
    uint8 QcomExtJPInfoAction::s_display = 1;
    uint8 QcomExtJPInfoAction::s_icon = 0;

    QcomExtJPInfoAction::QcomExtJPInfoAction()
        : Action(AT_QCOM_EXTJP_INFO)
    {
    }

    QcomExtJPInfoAction::~QcomExtJPInfoAction()
    {
    }

    bool QcomExtJPInfoAction::Parse(const ActionArgs & args)
    {
        bool res = false;

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: extjp [options]\n");
            COMMS_PRINT_BLOCK(vis_desc);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();
        }
        else
        {
            if (s_epgid.size() == s_umf.size() &&
                s_epgid.size() == s_name.size() &&
                s_epgid.size() <= SG_QCOMAC_MAX_PROG_LEVEL)
            {
                res = true;
            }
            else
            {
                COMMS_LOG(boost::format("Entry number of 'epgid', 'umf' and 'name' must be equal and less than %||\n") %
                    SG_QCOMAC_MAX_PROG_LEVEL, CLL_Error);
            }
        }
        
        return res;
    }

    void QcomExtJPInfoAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();
            m_options->AddOption(
                ActionOption("epgid", "external jackpot progressive group ID", Value<std::vector<uint16> >(&s_epgid), true));
            m_options->AddOption(
                ActionOption("umf", "unit modifier flag", Value<std::vector<uint8> >(&s_umf), true));
            m_options->AddOption(
                ActionOption("name", "name of the external jackpot group level", Value<std::vector<std::string> >(&s_name), true));
            m_options->AddOption(
                ActionOption("rtp", "total percentage RTP of the external jackpots on this EGM", Value<uint16>(&s_rtp)));
            m_options->AddOption(
                ActionOption("display", "display flag", Value<uint8>(&s_display)));
            m_options->AddOption(
                ActionOption("icon", "icon display ID", Value<uint8>(&s_icon)));
        }
    }

    ActionPtr QcomExtJPInfoAction::Clone()
    {
        return Action::DoClone<QcomExtJPInfoAction>();
    }

    const char * QcomExtJPInfoAction::Description() const
    {
        static const char* des = "\tExternal Jackpot Information Poll:\n\t\tInforms the EGM regarding details \
                                of any external jackpot system it has been placed in.\n";
        return des;
    }

    uint8_t QcomExtJPInfoAction::ExtJPData(uint16_t * epgid, uint8_t * lumf, char (*lname)[16]) const
    {
        size_t num = s_epgid.size();

        SG_ASSERT(num == s_umf.size() && num == s_name.size() && num <= SG_QCOMAC_MAX_PROG_LEVEL);

        for (size_t i = 0; i < num; ++i)
        {
            epgid[i] = s_epgid[i];
            lumf[i] = s_umf[i];
            strncpy(lname[i], s_name[i].c_str(), s_name[i].size());

            lname[i][s_name[i].size()] = 0;
        }

        return static_cast<uint8_t>(num);
    }

    std::vector<uint8> QcomProgHashRequestAction::s_seed;
    uint8 QcomProgHashRequestAction::s_new_seed = 1;
    uint8 QcomProgHashRequestAction::s_mef = 1;

    QcomProgHashRequestAction::QcomProgHashRequestAction()
        : Action(AT_QCOM_PROGHASH_REQUEST)
    {
    }

    QcomProgHashRequestAction::~QcomProgHashRequestAction()
    {
    }

    bool QcomProgHashRequestAction::Parse(const ActionArgs & args)
    {
        bool res = false;

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: hash [options]\n");
            COMMS_PRINT_BLOCK(vis_desc);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();
        }
        else
        {
            if (vm.count("seed"))
            {
                s_new_seed = 1;
            }
            else
            {
                s_new_seed = 0; // TODO : change s_new_seed from static to non-static
            }

            if (vm.count("mef"))
            {
                s_mef = 1;
            }

            res = true;
        }

        return res;
    }

    void QcomProgHashRequestAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();
            m_options->AddOption(ActionOption("seed", "hash alogrithm seed", Value<std::vector<uint8> >(&s_seed), true));
            m_options->AddOption(ActionOption("mef", "machine enable flag"));
        }
    }

    ActionPtr QcomProgHashRequestAction::Clone()
    {
        return Action::DoClone<QcomProgHashRequestAction>();
    }

    const char * QcomProgHashRequestAction::Description() const
    {
        static const char* des = "\tProgram Hash Request Poll:\n\t\tCommands the EGM to initiate a program hash calculation \
                                using the given seed\n";
        return des;
    }

    uint8_t QcomProgHashRequestAction::Seed(uint8_t * seed, size_t len)
    {
        std::memset(seed, 0, len);

        if (s_new_seed)
        {
            for (size_t i = 0; i < s_seed.size(); ++i)
            {
                seed[i] = s_seed[i];
            }
        }

        return s_new_seed;
    }

    std::string QcomSysLockupRequestAction::s_text;
    uint8 QcomSysLockupRequestAction::s_noreset_key = 0;
    uint8 QcomSysLockupRequestAction::s_continue = 0;
    uint8 QcomSysLockupRequestAction::s_question = 0;
    uint8 QcomSysLockupRequestAction::s_lamp_test = 0;
    uint8 QcomSysLockupRequestAction::s_fanfare = 0;

    QcomSysLockupRequestAction::QcomSysLockupRequestAction()
        : Action(AT_QCOM_SYSLOCKUP_REQUEST)
    {
    }

    QcomSysLockupRequestAction::~QcomSysLockupRequestAction()
    {
    }

    bool QcomSysLockupRequestAction::Parse(const ActionArgs & args)
    {
        bool res = false;

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: lockup [options]\n");
            COMMS_PRINT_BLOCK(vis_desc);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();
        }
        else
        {
            if (!vm.count("text"))
            {
                s_text = "";
            }

            if (vm.count("noreset"))
                s_noreset_key = 1;
            else
                s_noreset_key = 0;

            if (vm.count("continue"))
                s_continue = 1;
            else
                s_continue = 0;

            if (vm.count("question"))
                s_question = 1;
            else
                s_question = 0;

            if (vm.count("lamptest"))
                s_lamp_test = 1;
            else
                s_lamp_test = 0;

            if (vm.count("fanfare"))
                s_fanfare = 1;
            else
                s_fanfare = 0;


            res = true;
        }

        return res;
    }

    void QcomSysLockupRequestAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();
            m_options->AddOption(ActionOption("text", "system lockup messaage", Value<std::string>(&s_text)));
            m_options->AddOption(ActionOption("noreset,k", "EGM disable the lockup reset key-switch's effect on the system lockup\
                                               on the duration of the lockup if set"));
            m_options->AddOption(ActionOption("continue", "use continue style message during lockup if set"));
            m_options->AddOption(ActionOption("question", "use question style message during lockup if set"));
            m_options->AddOption(ActionOption("lamptest", "EGM turn on all its lamps during lockup if set"));
            m_options->AddOption(ActionOption("fanfare", "EGM trigger a short jackpot fanfare during lockup if set"));
        }
    }

    ActionPtr QcomSysLockupRequestAction::Clone()
    {
        return Action::DoClone<QcomSysLockupRequestAction>();
    }

    const char * QcomSysLockupRequestAction::Description() const
    {
        static const char *des = "\tSystem Lockup Request Poll:\n\t\tThis poll is used by the SC when the system wishes award\
                                  a system triggered prize through the EGM or lockup the EGM for some other reason.\n";
        return des;
    }

}




