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

    uint8 QcomEGMConfRequestAction::s_mef = 0;
    uint8 QcomEGMConfRequestAction::s_gcr = 0;
    uint8 QcomEGMConfRequestAction::s_psn = 0;

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
            COMMS_PRINT_BLOCK("\nUsage: configrequest/cr [options]\n");
            COMMS_PRINT_BLOCK(vis_desc);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();
        }
        else
        {
            SG_SET_FLAG_OPTION("mef", s_mef);
            SG_SET_FLAG_OPTION("gcr", s_gcr);
            SG_SET_FLAG_OPTION("psn", s_psn);

            res = true;
        }

        return res;
    }

    void QcomEGMConfRequestAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();

            m_options->AddOption(ActionOption("mef,m", "enable egm machine if set, otherwise disable it"));
            m_options->AddOption(ActionOption("gcr,g", "commands egm queue the EGM Game configuration Response if set"));
            m_options->AddOption(ActionOption("psn,p", "reset all Poll Sequence Numbers if set"));
            m_options->AddOption(ActionOption("help,h", "help message"));
        }
    }

    ActionPtr QcomEGMConfRequestAction::Clone()
    {
        return Action::DoClone<QcomEGMConfRequestAction>();
    }

    const char* QcomEGMConfRequestAction::Description() const
    {
        const char* des = "\tEGM configuration request :\n\t\tcr,configrequest\n";
        return des;
    }

    uint8     QcomEGMConfAction::s_jur = 0;
    uint32    QcomEGMConfAction::s_den = 1;
    uint32    QcomEGMConfAction::s_tok = 100;
    uint32    QcomEGMConfAction::s_maxden = 1000;
    uint16    QcomEGMConfAction::s_minrtp = 5000;
    uint16    QcomEGMConfAction::s_maxrtp = 9999;
    uint16    QcomEGMConfAction::s_maxsd = 65535;
    uint16    QcomEGMConfAction::s_maxlines = 65535;
    uint32    QcomEGMConfAction::s_maxbet = 4294967295;
    uint32    QcomEGMConfAction::s_maxnpwin = 4294967295;
    uint32    QcomEGMConfAction::s_maxpwin = 4294967295;
    uint32    QcomEGMConfAction::s_maxect = 1000000;

    QcomEGMConfAction::QcomEGMConfAction()
        : Action(Action::AT_QCOM_EGM_CONF)
    {
    }

    QcomEGMConfAction::~QcomEGMConfAction()
    {

    }

    void QcomEGMConfAction::ResetArgOptions()
    {
        s_jur = 0;
        s_den = 1;
        s_tok = 100;
        s_maxden = 1000;
        s_minrtp = 5000;
        s_maxrtp = 9999;
        s_maxsd = 65535;
        s_maxlines = 65535;
        s_maxbet = 4294967295;
        s_maxnpwin = 4294967295;
        s_maxpwin = 4294967295;
        s_maxect = 1000000;
    }

    bool QcomEGMConfAction::Parse(const Action::ActionArgs &args)
    {
        bool res = false;

        this->ResetArgOptions();

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: egmconfig/cf [options]\n");
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

            m_options->AddOption(ActionOption("jur", "jurisdictions", Value<uint8>(&s_jur)));
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
        static const char* des = "\tEGM configuration :\n\t\tcf,egmconfig\n";
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

    void QcomGameConfigurationAction::ResetArgOptions()
    {
        s_gvn = 0;
        s_pgid = 0xFFFF;
        s_var = 0;
        s_lp.clear();
        s_camt.clear();
    }

    bool QcomGameConfigurationAction::Parse(const Action::ActionArgs &args)
    {
        bool res = false;

        this->ResetArgOptions();

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: gameconfig/gc [options]\n");
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
                return res;
            }

            SG_SET_FLAG_OPTION("varlock", s_var_lock);
            SG_SET_FLAG_OPTION("gef", s_game_enable);
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
                ActionOption("varlock",
                    "variation lock, if set, the EGM must lock the variation and not allow the variation to be changed, otherwise variation hot-switch is enabled"));
            m_options->AddOption(
                ActionOption("gef", "game enable flag, if set, enable game denoted by GVN, otherwise disable the game"));
            m_options->AddOption(
                ActionOption("jptype", "progressive level type, if set to 0, denotes the level is to be set as SAP, otherwise level is LP",
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
        static const char* des = "\tGame configuration :\n\t\tgc,gameconfig\n";
        return des;
    }

    uint8_t QcomGameConfigurationAction::ProgressiveConfig(uint8_t * lp, uint32_t * camt)
    {
        size_t pnum = s_lp.size(); //< s_camt.size() ? s_lp.size() : s_camt.size();

        SG_ASSERT(pnum == s_camt.size() && pnum < SG_QCOMAC_MAX_PROG_LEVEL); // check pnum dynamically instead of using debug assert

        for (size_t i = 0; i < pnum; ++i)
        {
            lp[i] = s_lp[i] ? 1 : 0;
            camt[i] = s_camt[i];
        }

        return static_cast<uint8_t>(pnum);
    }

    uint16 QcomGameConfigurationChangeAction::s_gvn = 0;
    uint16 QcomGameConfigurationChangeAction::s_pgid = 0xFFFF;
    uint8  QcomGameConfigurationChangeAction::s_var = 0;
    uint8  QcomGameConfigurationChangeAction::s_gef = 1;

    QcomGameConfigurationChangeAction::QcomGameConfigurationChangeAction()
        : Action(Action::AT_QCOM_GAME_CONF_CHANGE)
    {
    }

    QcomGameConfigurationChangeAction::~QcomGameConfigurationChangeAction()
    {

    }

    void QcomGameConfigurationChangeAction::ResetArgOptions()
    {
        s_gvn = 0;
        s_pgid = 0xFFFF;
        s_var = 0;
    }

    bool QcomGameConfigurationChangeAction::Parse(const Action::ActionArgs &args)
    {
        bool res = false;

        this->ResetArgOptions();

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: changegconf/cc [options]\n");
            COMMS_PRINT_BLOCK(vis_desc);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();

            res = false;
        }
        else
        {
            SG_SET_FLAG_OPTION("gef", s_gef);

            res = true;
        }

        return res;
    }

    void QcomGameConfigurationChangeAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();

            m_options->AddOption(ActionOption("gvn", "game version number, identifies the game being re-configured", Value<uint16>(&s_gvn)));
            m_options->AddOption(ActionOption("pgid", "new linked progressive group ID", Value<uint16>(&s_pgid)));
            m_options->AddOption(ActionOption("var", "game version number, indicates the EGM what new variation to use", Value<uint8>(&s_var)));
            m_options->AddOption(ActionOption("gef", "enable game if set, otherwise disable it"));
            m_options->AddOption(ActionOption("help,h", "help message"));
        }
    }

    ActionPtr QcomGameConfigurationChangeAction::Clone()
    {
        return Action::DoClone<QcomGameConfigurationChangeAction>();
    }

    const char *QcomGameConfigurationChangeAction::Description() const
    {
        static const char* des = "\tChange Game configuration:\n\t\tcc,changegconf\n";
        return des;
    }


    uint8   QcomEGMParametersAction::s_reserve = 0;
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

    void QcomEGMParametersAction::ResetArgOptions()
    {
        s_opr = 0;
        s_lwin = 10000000;
        s_crlimit = 10000;
        s_dumax = 5;
        s_dulimit = 1000000;
        s_tzadj = 0;
        s_pwrtime = 900;
        s_pid = 0x00;
        s_eodt = 180;
        s_npwinp = 1000000;
        s_sapwinp = 1000000;
    }

    bool QcomEGMParametersAction::Parse(const Action::ActionArgs &args)
    {
        bool res = false;

        this->ResetArgOptions();

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: egmparams/pp [options]\n");
            COMMS_PRINT_BLOCK(vis_desc);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();

            res = false;
        }
        else
        {
            SG_SET_FLAG_OPTION("reserve", s_reserve);
            SG_SET_FLAG_OPTION("autoplay", s_autoplay);
            SG_SET_FLAG_OPTION("crlmode", s_crlimitmode);

            res = true;
        }

        return res;
    }

    void QcomEGMParametersAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();

            m_options->AddOption(ActionOption("reserve",
                "EGM may offer a player instigated machine reserve feature at its discretion if set, otherwise no reserver feature offered"));
            m_options->AddOption(ActionOption("autoplay", "autoplay is enabled if set, otherwise autoplay is disabled"));
            m_options->AddOption(ActionOption("crlmode", "both CRLIMIT mode are supported if set, otherwise only coin/token is rejected"));
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
        static const char* des = "\tSet EGM Parameters:\n\t\tpp,egmparams\n";
        return des;
    }


    uint8  QcomPurgeEventsAction::s_evtno = 255;

    QcomPurgeEventsAction::QcomPurgeEventsAction()
        :Action(Action::AT_QCOM_PURGE_EVENTS)
    {


    }

    QcomPurgeEventsAction::~QcomPurgeEventsAction()
    {

    }

    void QcomPurgeEventsAction::ResetArgOptions()
    {
        s_evtno = 255;
    }

    bool QcomPurgeEventsAction::Parse(const ActionArgs &args)
    {
        bool res = false;

        this->ResetArgOptions();

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: purgeevents/pe <event number>\n");
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
            m_options->AddOption(ActionOption("evnto", "", Value<uint8>(&s_evtno), false, 1));
            m_options->AddOption(ActionOption("help,h", "help message"));
        }
    }

    ActionPtr QcomPurgeEventsAction::Clone()
    {
        return Action::DoClone<QcomPurgeEventsAction>();
    }

    const char *QcomPurgeEventsAction::Description() const
    {
        static const char* des = "\tPurge Events:\n\t\tpe,purgeevents\n";
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
        static const char* des = "\tSend Time Date Broadcast: \n\t\ttd,timedate\n";
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

    void QcomLPCurrentAmountAction::ResetArgOptions()
    {
        s_lpamt.clear();
        s_pgid.clear();
    }

    bool QcomLPCurrentAmountAction::Parse(const ActionArgs & args)
    {
        bool res = false;

        this->ResetArgOptions();

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: lpcamt/lp [options]\n");
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
        static const char* des = "\tSend LP Current Amount Broadcast: \n\t\tlp,lpcamt\n";
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

    void QcomGeneralPromotionalAction::ResetArgOptions()
    {
        s_text.clear();
    }

    bool QcomGeneralPromotionalAction::Parse(const ActionArgs & args)
    {
        bool res = false;

        this->ResetArgOptions();

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: generalpromt/gp [options]\n");
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
        static const char* des = "\tSend General Promotional Broadcast: \n\t\tgp,generalpromt\n";

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

    void QcomSiteDetailAction::ResetArgOptions()
    {
        s_stext.clear();
        s_ltext.clear();
    }

    bool QcomSiteDetailAction::Parse(const ActionArgs & args)
    {
        bool res = false;

        this->ResetArgOptions();

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: sitedetail/sd [options]\n");
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
        static const char* des = "\tSend Site Detail Broadcast: \n\t\tsd,sitedetail\n";
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
        static const char* des = "\tRequest general Status of EGM: \n\t\tgs,generalstatus\n";
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

    void QcomPendingAction::ResetArgOptions()
    {
        s_pollnum = 2;
    }

    bool QcomPendingAction::Parse(const ActionArgs & args)
    {
        bool res = false;

        this->ResetArgOptions();

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: pending [options]\n");
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
        static const char* des = "\tPending Polls:\n\t\tpending\n";
        return des;
    }

    QcomSendAction::QcomSendAction()
        : Action(Action::AT_QCOM_SEND)
    {
    }

    QcomSendAction::~QcomSendAction()
    {
    }

    //bool QcomSendAction::Parse(const ActionArgs & args)
    //{
    //    bool res = true;

    //    SG_PARSE_OPTION(args, m_options);

    //    if (vm.count("help"))
    //    {
    //        COMMS_START_PRINT_BLOCK();
    //        COMMS_PRINT_BLOCK("\nUsage: send [options]\n");
    //        COMMS_PRINT_BLOCK(vis_desc);
    //        COMMS_PRINT_BLOCK("\n");
    //        COMMS_END_PRINT_BLOCK();

    //        res = false;
    //    }

    //    return res;
    //}

    //void QcomSendAction::BuildOptions()
    //{
    //    if (!m_options)
    //    {
    //        m_options = MakeSharedPtr<ActionOptions>();
    //        m_options->AddOption(ActionOption("help,h", "help message"));
    //    }
    //}

    ActionPtr QcomSendAction::Clone()
    {
        return Action::DoClone<QcomSendAction>();
    }

    const char * QcomSendAction::Description() const
    {
        static const char* des = "\tSend Pending Poll:\n\t\tsend\n";
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

    void QcomProgressiveConfigAction::ResetArgOptions()
    {
        s_sup.clear();
        s_pinc.clear();
        s_ceil.clear();
        s_auxrtp.clear();
        s_gvn = 0;
    }

    bool QcomProgressiveConfigAction::Parse(const ActionArgs & args)
    {
        bool res = false;

        this->ResetArgOptions();

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: progressive/pc [options]\n");
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
        static const char* des = "\tChange Progressive Config:\n\t\tpc,progressive\n";
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
    uint8 QcomExtJPInfoAction::s_display = 0;
    uint8 QcomExtJPInfoAction::s_icon = 0;

    QcomExtJPInfoAction::QcomExtJPInfoAction()
        : Action(AT_QCOM_EXTJP_INFO)
    {
    }

    QcomExtJPInfoAction::~QcomExtJPInfoAction()
    {
    }

    void QcomExtJPInfoAction::ResetArgOptions()
    {
        s_epgid.clear();
        s_umf.clear();
        s_name.clear();
        s_rtp = 0;
    }

    bool QcomExtJPInfoAction::Parse(const ActionArgs & args)
    {
        bool res = false;

        this->ResetArgOptions();

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: extjackpotinfo/extjpi [options]\n");
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

                return res;
            }

            SG_SET_FLAG_OPTION("display", s_display);

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
                ActionOption("umf", "unit modifier flag, display of EXTJIP data in unit-less if set", Value<std::vector<uint8> >(&s_umf), true));
            m_options->AddOption(
                ActionOption("name", "name of the external jackpot group level", Value<std::vector<std::string> >(&s_name), true));
            m_options->AddOption(
                ActionOption("rtp", "total percentage RTP of the external jackpots on this EGM", Value<uint16>(&s_rtp)));
            m_options->AddOption(
                ActionOption("display", "commands EGM display the last received current amounts via LP broadcasts if set"));
            m_options->AddOption(
                ActionOption("icon", "icon display ID, disable icon display set to 0", Value<uint8>(&s_icon)));
            m_options->AddOption(ActionOption("help,h", "help message"));
        }
    }

    ActionPtr QcomExtJPInfoAction::Clone()
    {
        return Action::DoClone<QcomExtJPInfoAction>();
    }

    const char * QcomExtJPInfoAction::Description() const
    {
        static const char* des = "\tExternal Jackpot Information:\n\t\textjpi,extjackpotinfo\n";
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
    uint8 QcomProgHashRequestAction::s_new_seed = 0;
    uint8 QcomProgHashRequestAction::s_mef = 0;

    QcomProgHashRequestAction::QcomProgHashRequestAction()
        : Action(AT_QCOM_PROGHASH_REQUEST)
    {
    }

    QcomProgHashRequestAction::~QcomProgHashRequestAction()
    {
    }

    void QcomProgHashRequestAction::ResetArgOptions()
    {
        s_seed.clear();
    }

    bool QcomProgHashRequestAction::Parse(const ActionArgs & args)
    {
        bool res = false;

        this->ResetArgOptions();

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
            SG_SET_FLAG_OPTION("seed", s_new_seed);
            SG_SET_FLAG_OPTION("mef", s_mef);

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
            m_options->AddOption(ActionOption("mef", "enable machine if set, otherwise disable it"));
            m_options->AddOption(ActionOption("help,h", "help message"));
        }
    }

    ActionPtr QcomProgHashRequestAction::Clone()
    {
        return Action::DoClone<QcomProgHashRequestAction>();
    }

    const char * QcomProgHashRequestAction::Description() const
    {
        static const char* des = "\tProgram Hash Request:\n\t\thash\n";
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

    void QcomSysLockupRequestAction::ResetArgOptions()
    {
        s_text.clear();
    }

    bool QcomSysLockupRequestAction::Parse(const ActionArgs & args)
    {
        bool res = false;

        this->ResetArgOptions();

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
            SG_SET_FLAG_OPTION("noreset", s_noreset_key);
            SG_SET_FLAG_OPTION("continue", s_continue);
            SG_SET_FLAG_OPTION("question", s_question);
            SG_SET_FLAG_OPTION("lamptest", s_lamp_test);
            SG_SET_FLAG_OPTION("fanfare", s_fanfare);

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
            m_options->AddOption(ActionOption("noreset,k", 
                "EGM disable the lockup reset key-switch's effect on the system lockup on the duration of the lockup if set"));
            m_options->AddOption(ActionOption("continue", "use continue style message during lockup if set"));
            m_options->AddOption(ActionOption("question", "use question style message during lockup if set"));
            m_options->AddOption(ActionOption("lamptest", "EGM turn on all its lamps during lockup if set"));
            m_options->AddOption(ActionOption("fanfare", "EGM trigger a short jackpot fanfare during lockup if set"));
            m_options->AddOption(ActionOption("help,h", "help message"));
        }
    }

    ActionPtr QcomSysLockupRequestAction::Clone()
    {
        return Action::DoClone<QcomSysLockupRequestAction>();
    }

    const char * QcomSysLockupRequestAction::Description() const
    {
        static const char *des = "\tSystem Lockup Request:\n\t\tlockup\n";
        return des;
    }

    std::string QcomCashTicketOutAckAction::s_certification;
    std::string QcomCashTicketOutAckAction::s_authno("999999999999999999");
    uint32 QcomCashTicketOutAckAction::s_amount = 0;
    uint16 QcomCashTicketOutAckAction::s_serial = 0;
    uint8 QcomCashTicketOutAckAction::s_approved = 0;
    uint8 QcomCashTicketOutAckAction::s_canceled = 0;

    QcomCashTicketOutAckAction::QcomCashTicketOutAckAction()
        : Action(AT_QCOM_CASH_TICKET_OUT_ACK)
    {

    }

    QcomCashTicketOutAckAction::~QcomCashTicketOutAckAction()
    {

    }

    void QcomCashTicketOutAckAction::ResetArgOptions()
    {
        s_certification.clear();
        s_authno = std::string("999999999999999999");
        s_amount = 0;
        s_serial = 0;
    }

    bool QcomCashTicketOutAckAction::Parse(ActionArgs const& args)
    {
        bool res = false;

        this->ResetArgOptions();

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: torack [options]\n");
            COMMS_PRINT_BLOCK(vis_desc);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();
        }
        else
        {

            SG_SET_FLAG_OPTION("approve", s_approved);
            SG_SET_FLAG_OPTION("cancel", s_canceled);

            if (s_approved && s_canceled)
            {
                COMMS_LOG("torack can't use approve and cancel option at the same time.\n", CLL_Error);
                return false;
            }

            res = true;
        }

        return res;
    }

    void QcomCashTicketOutAckAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();
            m_options->AddOption(ActionOption("msg,m", "certification message", Value<std::string>(&s_certification)));
            m_options->AddOption(ActionOption("authno", "ticket authorisation number displayed on the ticket, barcode number only", Value<std::string>(&s_authno)));
            m_options->AddOption(ActionOption("amount", "ticket out amount in cents displayed as decimal currency of up to 11 characters", Value<uint32>(&s_amount)));
            m_options->AddOption(ActionOption("serial", "ticket serial number", Value<uint16>(&s_serial)));
            m_options->AddOption(ActionOption("approve", "approve cash ticket request if set, otherwise deny it. can't be used with cancel option together"));
            m_options->AddOption(ActionOption("cancel", "cancel the cash request if set, egm will enter Cancel Credit Lockup. can't be used with approve option together"));
            m_options->AddOption(ActionOption("help,h", "help message"));
        }
    }

    ActionPtr QcomCashTicketOutAckAction::Clone()
    {
        return Action::DoClone<QcomCashTicketOutAckAction>();
    }

    const char* QcomCashTicketOutAckAction::Description() const
    {
        static const char * des = "\tCash Ticket Out Acknowledgement:\n\t\ttorack\n";

        return des;
    }

    std::string QcomCashTicketInAckAction::s_authno("999999999999999999");
    uint32 QcomCashTicketInAckAction::s_amount = 0;
    uint8 QcomCashTicketInAckAction::s_fcode = 0;

    QcomCashTicketInAckAction::QcomCashTicketInAckAction()
        : Action(AT_QCOM_CASH_TICKET_IN_ACK)
    {

    }

    QcomCashTicketInAckAction::~QcomCashTicketInAckAction()
    {

    }

    void QcomCashTicketInAckAction::ResetArgOptions()
    {
        s_authno = std::string("999999999999999999");
        s_amount = 0;
        s_fcode = 0;
    }

    bool QcomCashTicketInAckAction::Parse(const ActionArgs & args)
    {
        bool res = false;

        this->ResetArgOptions();

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: tirack [options]\n");
            COMMS_PRINT_BLOCK(vis_desc);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();
        }
        else
        {
            res = true;
        }

        return res;
    }

    void QcomCashTicketInAckAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();
            m_options->AddOption(ActionOption("authno", "ticket authorisation number from the last Ticket-In Request Event echoed back", Value<std::string>(&s_authno)));
            m_options->AddOption(ActionOption("amount", "ticket in amount in cents, display as decimal currency of up to 11 characters", Value<uint32>(&s_amount)));
            m_options->AddOption(ActionOption("fcode", "accept last cash ticket in request if not set, otherwise the value indicate reason for the failure", Value<uint8>(&s_fcode)));
            m_options->AddOption(ActionOption("help,h", "help message"));
        }
    }

    ActionPtr QcomCashTicketInAckAction::Clone()
    {
        return Action::DoClone<QcomCashTicketInAckAction>();
    }

    const char* QcomCashTicketInAckAction::Description() const
    {
        static const char * des = "\tCash Ticket In Acknowledgement Poll:\n\t\ttirack\n";

        return des;
    }

    QcomCashTicketOutRequestAction::QcomCashTicketOutRequestAction()
        : Action(AT_QCOM_CASH_TICKET_OUT_REQ)
    {

    }

    QcomCashTicketOutRequestAction::~QcomCashTicketOutRequestAction()
    {

    }

    ActionPtr QcomCashTicketOutRequestAction::Clone()
    {
        return Action::DoClone<QcomCashTicketOutRequestAction>();
    }

    const char* QcomCashTicketOutRequestAction::Description() const
    {
        static const char * des = "\tCash Ticket Out Request:\n\t\treqto\n";
        return des;
    }
    
    std::vector<uint8>   QcomEGMGeneralMaintenanceAction::s_meters;
    uint16  QcomEGMGeneralMaintenanceAction::s_gvn = 0;
    uint8   QcomEGMGeneralMaintenanceAction::s_nasr = 0;
    uint8   QcomEGMGeneralMaintenanceAction::s_mef = 0;
    uint8   QcomEGMGeneralMaintenanceAction::s_var = 0;
    uint8   QcomEGMGeneralMaintenanceAction::s_pcmr = 0;
    uint8   QcomEGMGeneralMaintenanceAction::s_bmr = 0;
    uint8   QcomEGMGeneralMaintenanceAction::s_gmecfg = 0;
    uint8   QcomEGMGeneralMaintenanceAction::s_progcfg = 0;
    uint8   QcomEGMGeneralMaintenanceAction::s_progmeters = 0;
    uint8   QcomEGMGeneralMaintenanceAction::s_multigame = 0;
    uint8   QcomEGMGeneralMaintenanceAction::s_gef = 0;

    QcomEGMGeneralMaintenanceAction::QcomEGMGeneralMaintenanceAction()
        : Action(AT_QCOM_EGM_GENERAL_MAINTENANCE)
    {

    }

    QcomEGMGeneralMaintenanceAction::~QcomEGMGeneralMaintenanceAction()
    {

    }

    void QcomEGMGeneralMaintenanceAction::ResetArgOptions()
    {
        s_meters.clear();
        s_var = 0;
        s_gvn = 0;
    }

    bool QcomEGMGeneralMaintenanceAction::Parse(const ActionArgs &args)
    {
        bool res = false;

        this->ResetArgOptions();

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: generalmainten/gm [options]\n");
            COMMS_PRINT_BLOCK(vis_desc);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();
        }
        else
        {
            if (s_meters.size() > 3)
            {
                COMMS_LOG("Too many arguments for meters option\n", CLL_Error);
                return false;
            }

            SG_SET_FLAG_OPTION("qnasr", s_nasr);
            SG_SET_FLAG_OPTION("mef", s_mef);
            SG_SET_FLAG_OPTION("pcmr", s_pcmr);
            SG_SET_FLAG_OPTION("bmr", s_bmr);
            SG_SET_FLAG_OPTION("gamecfg", s_gmecfg);
            SG_SET_FLAG_OPTION("progcfg", s_progcfg);
            SG_SET_FLAG_OPTION("progmeters", s_progmeters);
            SG_SET_FLAG_OPTION("multigame", s_multigame);
            SG_SET_FLAG_OPTION("gef", s_gef);

            res = true;
        }

        return res;
    }

    void QcomEGMGeneralMaintenanceAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();
            m_options->AddOption(
                ActionOption(
                    "meters", 
                    "set which meter group(s) to report via the EGM Meter Group/Contribution Response", 
                    Value<std::vector<uint8> >(&s_meters),
                    true));
            m_options->AddOption(
                ActionOption(
                    "var",
                    "game variation number",
                    Value<uint8>(&s_var)));
            m_options->AddOption(
                ActionOption(
                    "gvn", 
                    "game version number", 
                    Value<uint16>(&s_gvn)));
            m_options->AddOption(
                ActionOption(
                    "qnasr",
                    "commands the EGM to queue the Note Acceptor Status Response if set"));
            m_options->AddOption(
                ActionOption(
                    "mef",
                    "enable machine if set, otherwise disable it"));
            m_options->AddOption(
                ActionOption(
                    "pcmr",
                    "commands the EGM to queue a Player Choice Meter Response"));
            m_options->AddOption(
                ActionOption(
                    "bmr",
                    "commands the EGM to queue a Bet Meters Response"));
            m_options->AddOption(
                ActionOption(
                    "gamecfg",
                    "commads the EGM to queue a Game Configuration Response"));
            m_options->AddOption(
                ActionOption(
                    "progcfg",
                    "commands the EGM to queue a Progressive Configuration Response"));
            m_options->AddOption(
                ActionOption(
                    "progmeters",
                    "commands the EGM to queue a Progressive Meters Response"));
            m_options->AddOption(
                ActionOption(
                    "multigame",
                    "commands the EGM to queue a Multi-Game/Variation Meters Response"));
            m_options->AddOption(
                ActionOption(
                    "gef",
                    "set to enable game, otherwise disable it"));
            m_options->AddOption(ActionOption("help,h", "help message"));
        }
    }

    uint8_t QcomEGMGeneralMaintenanceAction::MetersGroupFlag(uint8_t index) const
    {
        SG_ASSERT(index < 3);

        for (uint8_t i = 0; i < s_meters.size(); ++i)
        {
            if (s_meters[i] == index)
                return 1;
        }

        return 0;
    }

    ActionPtr QcomEGMGeneralMaintenanceAction::Clone()
    {
        return Action::DoClone<QcomEGMGeneralMaintenanceAction>();
    }

    const char* QcomEGMGeneralMaintenanceAction::Description() const
    {
        static const char* des = "\tEGM General Maintenance:\n\t\tgm,generalmainten\n";
        return des;
    }

    QcomRequestAllLoggedEventsAction::QcomRequestAllLoggedEventsAction()
        : Action(AT_QCOM_REQ_ALL_LOGGED_EVENTS)
    {

    }

    QcomRequestAllLoggedEventsAction::~QcomRequestAllLoggedEventsAction()
    {

    }

    ActionPtr QcomRequestAllLoggedEventsAction::Clone()
    {
        return Action::DoClone<QcomRequestAllLoggedEventsAction>();
    }

    const char* QcomRequestAllLoggedEventsAction::Description() const
    {
        static const char * des = "\tRequest All Logged Events:\n\t\t\reqevents\n";

        return des;
    }

    std::vector<uint8> QcomNoteAcceptorMaintenanceAction::s_denoms;

    QcomNoteAcceptorMaintenanceAction::QcomNoteAcceptorMaintenanceAction()
        : Action(AT_QCOM_NOTE_ACCEPTOR_MAINTENANCE)
    {

    }

    QcomNoteAcceptorMaintenanceAction::~QcomNoteAcceptorMaintenanceAction()
    {

    }

    void QcomNoteAcceptorMaintenanceAction::ResetArgOptions()
    {
        s_denoms.clear();
    }

    bool QcomNoteAcceptorMaintenanceAction::Parse(const ActionArgs & args)
    {
        bool res = false;

        this->ResetArgOptions();

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: nam [options]\n");
            COMMS_PRINT_BLOCK(vis_desc);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();
        }
        else
        {
            if (s_denoms.size() > 5)
            {
                COMMS_LOG("too many arguments for denoms option\n", CLL_Error);
                return false;
            }

            res = true;
        }

        return res;
    }

    uint8_t QcomNoteAcceptorMaintenanceAction::GetDenomFlag(uint8_t denom) const
    {
        SG_ASSERT(denom == 5 || denom == 10 || denom == 20 || denom == 50 || denom == 100);

        for (size_t i = 0; i < s_denoms.size(); ++i)
        {
            if (s_denoms[i] == denom)
                return 1;
        }

        return 0;
    }

    void QcomNoteAcceptorMaintenanceAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();
            m_options->AddOption(
                ActionOption(
                    "denoms",
                    "set to enable the specified denomination banknot for acceptance by the banknote acceptor, otherwise disable acceptance",
                    Value<std::vector<uint8> >(&s_denoms), 
                    true));

            m_options->AddOption(ActionOption("help,h", "help message"));
        }
    }

    ActionPtr QcomNoteAcceptorMaintenanceAction::Clone()
    {
        return Action::DoClone<QcomNoteAcceptorMaintenanceAction>();
    }

    const char* QcomNoteAcceptorMaintenanceAction::Description() const
    {
        static const char * des = "\tNote Acceptor Maintenance:\n\t\tnam\n";

        return des;
    }

    uint8 QcomHopperTicketPrinterMaintenanceAction::s_test = 0;
    uint32 QcomHopperTicketPrinterMaintenanceAction::s_refill = 0;
    uint32 QcomHopperTicketPrinterMaintenanceAction::s_collim = 0;
    uint32 QcomHopperTicketPrinterMaintenanceAction::s_ticket = 0;
    uint32 QcomHopperTicketPrinterMaintenanceAction::s_dorefill = 0;

    QcomHopperTicketPrinterMaintenanceAction::QcomHopperTicketPrinterMaintenanceAction()
        : Action(AT_QCOM_HOPPPER_TICKET_PRINTER)
    {

    }

    QcomHopperTicketPrinterMaintenanceAction::~QcomHopperTicketPrinterMaintenanceAction()
    {

    }

    void QcomHopperTicketPrinterMaintenanceAction::ResetArgOptions()
    {
        s_refill = 16000;
        s_collim = 5000;
        s_ticket = 0;
        s_dorefill = 0;
    }

    bool QcomHopperTicketPrinterMaintenanceAction::Parse(const ActionArgs & args)
    {
        bool res = false;

        this->ResetArgOptions();

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: htpm [options]\n");
            COMMS_PRINT_BLOCK(vis_desc);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();
        }
        else
        {
            SG_SET_FLAG_OPTION("test", s_test);

            res = true;
        }

        return res;
    }

    void QcomHopperTicketPrinterMaintenanceAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();
            m_options->AddOption(ActionOption("test", "if set, commands the EGM to print out a test ticket"));
            m_options->AddOption(ActionOption("refille", "set default hopper refill amount", Value<uint32>(&s_refill)));
            m_options->AddOption(ActionOption("collim", "set hopper collect limit", Value<uint32>(&s_collim)));
            m_options->AddOption(ActionOption("ticket", "set ticket out limit", Value<uint32>(&s_ticket)));
            m_options->AddOption(ActionOption("dorefill", "extra refill amount", Value<uint32>(&s_dorefill)));
            m_options->AddOption(ActionOption("help,h", "help message"));
        }
    }

    ActionPtr QcomHopperTicketPrinterMaintenanceAction::Clone()
    {
        return Action::DoClone<QcomHopperTicketPrinterMaintenanceAction>();
    }

    const char* QcomHopperTicketPrinterMaintenanceAction::Description() const
    {
        static const char * des = "\tHopper Ticket Printer Maintenance:\n\t\thtpm\n";
        return des;
    }

    QcomLPAwardAckAction::QcomLPAwardAckAction()
        : Action(AT_QCOM_LP_AWARD_ACK)
    {

    }

    QcomLPAwardAckAction::~QcomLPAwardAckAction()
    {

    }

    ActionPtr QcomLPAwardAckAction::Clone()
    {
        return Action::DoClone<QcomLPAwardAckAction>();
    }

    const char* QcomLPAwardAckAction::Description() const
    {
        static const char * des = "\tLink Progressive Award Acknowledged:\n\t\tla,lpack\n";
        return des;
    }

    uint8 QcomGeneralResetAction::s_fault = 0;
    uint8 QcomGeneralResetAction::s_lockup = 0;
    uint8 QcomGeneralResetAction::s_state = 0;

    QcomGeneralResetAction::QcomGeneralResetAction()
        : Action(AT_QCOM_GENERAL_RESET)
    {

    }

    QcomGeneralResetAction::~QcomGeneralResetAction()
    {

    }

    void QcomGeneralResetAction::ResetArgOptions()
    {
        s_state = 0;
    }

    bool QcomGeneralResetAction::Parse(const ActionArgs &args)
    {
        bool res = false;

        this->ResetArgOptions();

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: reset [options]\n");
            COMMS_PRINT_BLOCK(vis_desc);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();
        }
        else
        {
            SG_SET_FLAG_OPTION("fault", s_fault);
            SG_SET_FLAG_OPTION("lockup", s_lockup);

            res = true;
        }

        return res;
    }

    void QcomGeneralResetAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();
            m_options->AddOption(ActionOption("fault", "reset fault condition if set"));
            m_options->AddOption(ActionOption("lockup", "reset fault condition if set"));
            m_options->AddOption(ActionOption("state", "lockup condition state code", Value<uint8>(&s_state)));
            m_options->AddOption(ActionOption("help,h", "help message"));
        }
    }

    ActionPtr QcomGeneralResetAction::Clone()
    {
        return Action::DoClone<QcomGeneralResetAction>();
    }

    const char* QcomGeneralResetAction::Description() const
    {
        static const char* des = "\tGeneral Reset:\n\t\treset\n";

        return des;
    }

    std::string QcomSPAMAction::s_text;
    uint8 QcomSPAMAction::s_prominence = 0;
    uint8 QcomSPAMAction::s_fanfare = 0;
    uint8 QcomSPAMAction::s_type = 1;

    QcomSPAMAction::QcomSPAMAction()
        : Action(AT_QCOM_SPAM)
    {

    }

    QcomSPAMAction::~QcomSPAMAction()
    {

    }

    void QcomSPAMAction::ResetArgOptions()
    {
        s_text.clear();
        s_type = 1;
    }

    bool QcomSPAMAction::Parse(const ActionArgs & args)
    {
        bool res = false;

        this->ResetArgOptions();

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: spam [options] <type>\n");
            COMMS_PRINT_BLOCK(vis_desc);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();
        }
        else
        {
            if (s_type != 1 && s_type != 2)
            {
                COMMS_LOG(boost::format("Invalid type %1% for spam") % s_type, CLL_Error);
                return false;
            }

            SG_SET_FLAG_OPTION("prom", s_prominence);
            SG_SET_FLAG_OPTION("fanfare", s_fanfare);

            res = true;
        }

        return res;
    }

    void QcomSPAMAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();
            m_options->AddOption(ActionOption("prom", "if set, the EGM must display any SPAM A message while in idle mode, applies to SPAM A only"));
            m_options->AddOption(ActionOption("fanfare", "if set, the EGM must make an short attention sound"));
            m_options->AddOption(ActionOption("type", "", Value<uint8>(&s_type), false, 1));
            m_options->AddOption(ActionOption("text", "message to display", Value<std::string>(&s_text)));
            m_options->AddOption(ActionOption("help,h", "help message"));
        }
    }

    ActionPtr QcomSPAMAction::Clone()
    {
        return Action::DoClone<QcomSPAMAction>();
    }

    const char* QcomSPAMAction::Description() const
    {
        static const char * des = "\tSpecific Promotional/Advisory Message:\n\t\tspam\n";

        return des;
    }

    uint8 QcomTowerLightMaintenanceAction::s_yellow_on = 0;
    uint8 QcomTowerLightMaintenanceAction::s_blue_on = 0;
    uint8 QcomTowerLightMaintenanceAction::s_red_on = 0;
    uint8 QcomTowerLightMaintenanceAction::s_yellow_flash = 0;
    uint8 QcomTowerLightMaintenanceAction::s_blue_flash = 0;
    uint8 QcomTowerLightMaintenanceAction::s_red_flash = 0;

    QcomTowerLightMaintenanceAction::QcomTowerLightMaintenanceAction()
        : Action(AT_QCOM_TOWER_LIGHT_MAINTENANCE)
    {

    }

    QcomTowerLightMaintenanceAction::~QcomTowerLightMaintenanceAction()
    {

    }

    bool QcomTowerLightMaintenanceAction::Parse(const ActionArgs & args)
    {
        bool res = false;

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: towerlight/tl [options]\n");
            COMMS_PRINT_BLOCK(vis_desc);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();
        }
        else
        {
            SG_SET_FLAG_OPTION("y", s_yellow_on);
            SG_SET_FLAG_OPTION("b", s_blue_on);
            SG_SET_FLAG_OPTION("r", s_red_on);
            SG_SET_FLAG_OPTION("flashy", s_yellow_flash);
            SG_SET_FLAG_OPTION("falshb", s_blue_flash);
            SG_SET_FLAG_OPTION("falshr", s_red_flash);

            res = true;
        }

        return res;
    }

    void QcomTowerLightMaintenanceAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();
            m_options->AddOption(ActionOption("y", "turn on yellow light"));
            m_options->AddOption(ActionOption("b", "turn on blue light"));
            m_options->AddOption(ActionOption("r", "turn on red light"));
            m_options->AddOption(ActionOption("falshy", "flash yellow light"));
            m_options->AddOption(ActionOption("falshb", "flash blue light"));
            m_options->AddOption(ActionOption("falshr", "flash red light"));
            m_options->AddOption(ActionOption("help,h", "help message"));
        }
    }

    ActionPtr QcomTowerLightMaintenanceAction::Clone()
    {
        return Action::DoClone<QcomTowerLightMaintenanceAction>();
    }

    const char* QcomTowerLightMaintenanceAction::Description() const
    {
        static const char * des = "\tTower Light Maintenance:\n\t\ttl,towerlight\n";

        return des;
    }

    uint8 QcomECTToEGMAction::s_cashless = 0;
    uint8 QcomECTToEGMAction::s_id = 0;
    uint32 QcomECTToEGMAction::s_eamt = 0;

    QcomECTToEGMAction::QcomECTToEGMAction()
        : Action(AT_QCOM_ECT_TO_EGM)
    {

    }

    QcomECTToEGMAction::~QcomECTToEGMAction()
    {

    }

    void QcomECTToEGMAction::ResetArgOptions()
    {
        s_id = 0;
        s_eamt = 0;
    }

    bool QcomECTToEGMAction::Parse(const ActionArgs & args)
    {
        bool res = false;

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: ect2egm/ete [options]\n");
            COMMS_PRINT_BLOCK(vis_desc);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();
        }
        else
        {
            SG_SET_FLAG_OPTION("cashless", s_cashless);

            res = true;
        }

        return res;
    }

    void QcomECTToEGMAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();
            m_options->AddOption(ActionOption("cashless", "turn on yellow light"));
            m_options->AddOption(ActionOption("id", "ECT source ID", Value<uint8>(&s_id)));
            m_options->AddOption(ActionOption("eamt", "Amount to add to current credit meter", Value<uint32>(&s_eamt)));
            m_options->AddOption(ActionOption("help,h", "help message"));
        }
    }

    ActionPtr QcomECTToEGMAction::Clone()
    {
        return Action::DoClone<QcomECTToEGMAction>();
    }

    const char* QcomECTToEGMAction::Description() const
    {
        static const char* des = "\tECT To EGM:\n\t\tete,ect2egm\n";

        return des;
    }

    QcomECTFromEGMLockupRequestAction::QcomECTFromEGMLockupRequestAction()
        : Action(AT_QCOM_ECT_FROM_EGM_REQ)
    {

    }

    QcomECTFromEGMLockupRequestAction::~QcomECTFromEGMLockupRequestAction()
    {

    }

    ActionPtr QcomECTFromEGMLockupRequestAction::Clone()
    {
        return Action::DoClone<QcomECTFromEGMLockupRequestAction>();
    }

    const char* QcomECTFromEGMLockupRequestAction::Description() const
    {
        static const char* des = "\tECT From EGM Lockup Request:\n\t\tefe,ectfromegm\n";
        
        return des;
    }

    uint8 QcomECTLockupResetAction::s_denied = 0;

    QcomECTLockupResetAction::QcomECTLockupResetAction()
        : Action(AT_QCOM_ECT_LOCKUP_RESET)
    {

    }

    QcomECTLockupResetAction::~QcomECTLockupResetAction()
    {

    }

    bool QcomECTLockupResetAction::Parse(const ActionArgs & args)
    {
        bool res = false;

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: ect [options]\n");
            COMMS_PRINT_BLOCK(vis_desc);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();
        }
        else
        {
            SG_SET_FLAG_OPTION("denied", s_denied);

            res = true;
        }

        return res;
    }

    void QcomECTLockupResetAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();
            m_options->AddOption(ActionOption("denied", "the credit transfer from the EGM is denied"));
            m_options->AddOption(ActionOption("help,h", "help message"));
        }
    }

    ActionPtr QcomECTLockupResetAction::Clone()
    {
        return Action::DoClone<QcomECTLockupResetAction>();
    }

    const char* QcomECTLockupResetAction::Description() const
    {
        static const char* des = "\tECT Lockup Reset:\n\t\tect\n";

        return des;
    }

    uint8          QcomListAction::s_all;
    uint8          QcomListAction::s_egm;
    uint8          QcomListAction::s_config;
    uint8          QcomListAction::s_devices;
    uint8          QcomListAction::s_settings;
    uint8          QcomListAction::s_hash;
    uint8          QcomListAction::s_psn;
    uint8          QcomListAction::s_parameters;
    uint8          QcomListAction::s_state;
    uint8          QcomListAction::s_concurrents;
    uint8          QcomListAction::s_note_acceptor;
    uint8          QcomListAction::s_extjpinfo;
    uint8          QcomListAction::s_levels;
    uint8          QcomListAction::s_games;
    uint16         QcomListAction::s_gvn;
    uint8          QcomListAction::s_variations;
    uint8          QcomListAction::s_progressive;
    uint8          QcomListAction::s_meters;
    uint8          QcomListAction::s_cmet;
    uint8          QcomListAction::s_prog;
    uint8          QcomListAction::s_multigame_var;
    uint8          QcomListAction::s_player_choice;
    uint8          QcomListAction::s_group;
    uint8_t        QcomListAction::s_no_opt;

    QcomListAction::QcomListAction()
        : Action(AT_QCOM_LIST)
    {

    }

    QcomListAction::~QcomListAction()
    {

    }

    void QcomListAction::ResetArgOptions()
    {
        s_egm = 0;
        s_gvn = 0;
        s_group = 0x10;
    }

#define SG_IGNORE_OPTION(option, func)      if (vm.count(option)) func();
#define SG_IGNORE_SUB_OPTION(parent, child, func) if (child && !parent) func();

    bool QcomListAction::Parse(const ActionArgs &args)
    {
        bool res = false;

        this->ResetArgOptions();

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: list [options]\n");
            COMMS_PRINT_BLOCK(vis_desc);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();
        }
        else
        {
            SG_SET_FLAG_OPTION("all", s_all);
            
            if (s_all)
            {
                SG_IGNORE_OPTION("egm", IgnoreEGM);
                SG_IGNORE_OPTION("config", IgnoreConfig);
                SG_IGNORE_OPTION("hash", IgnoreHash);
                SG_IGNORE_OPTION("psn", IgnorePSN);
                SG_IGNORE_OPTION("parameters", IgnoreParameters);
                SG_IGNORE_OPTION("state", IgnoreState);
                SG_IGNORE_OPTION("extjpinfo", IgnoreExtJPInfo);
                SG_IGNORE_OPTION("games", IgnoreGames);
                SG_IGNORE_OPTION("gvn", IgnoreGVN);

                return true;
            }

            SG_SET_FLAG_OPTION("config", s_config);
            SG_SET_FLAG_OPTION("devices", s_devices);
            SG_IGNORE_SUB_OPTION(s_config, s_devices, IgnoreDevices);
            SG_SET_FLAG_OPTION("settings", s_settings);
            SG_IGNORE_SUB_OPTION(s_config, s_settings, IgnoreSettings);

            SG_SET_FLAG_OPTION("hash", s_hash);

            SG_SET_FLAG_OPTION("psn", s_psn);

            SG_SET_FLAG_OPTION("parameters", s_parameters);

            SG_SET_FLAG_OPTION("state", s_state);
            SG_SET_FLAG_OPTION("concurrents", s_concurrents);
            SG_IGNORE_SUB_OPTION(s_state, s_concurrents, IgnoreConcurrents);
            SG_SET_FLAG_OPTION("noteacceptor", s_note_acceptor);
            SG_IGNORE_SUB_OPTION(s_state, s_note_acceptor, IgnoreNoteAcceptor);

            SG_SET_FLAG_OPTION("extjpinfo", s_extjpinfo);
            SG_SET_FLAG_OPTION("levels", s_levels);
            SG_IGNORE_SUB_OPTION(s_extjpinfo, s_levels, IgnoreLevels);

            SG_SET_FLAG_OPTION("games", s_games);

            uint8_t metgvn = 0;
            SG_SET_FLAG_OPTION("gvn", metgvn);
            //SG_SET_FLAG_OPTION("gvn", s_gvn);
            SG_SET_FLAG_OPTION("variations", s_variations);
            SG_IGNORE_SUB_OPTION(metgvn, s_variations, IgnoreVariations);
            SG_SET_FLAG_OPTION("progressive", s_progressive);
            SG_IGNORE_SUB_OPTION(metgvn, s_progressive, IgnoreProgressive);
            SG_SET_FLAG_OPTION("meters", s_meters);
            SG_IGNORE_SUB_OPTION(metgvn, s_meters, IgnoreMeters);
            if (!(metgvn && !s_meters))
            {
                SG_SET_FLAG_OPTION("cmet", s_cmet);
                SG_SET_FLAG_OPTION("prog", s_prog);
                SG_SET_FLAG_OPTION("multigamevar", s_multigame_var);
                SG_SET_FLAG_OPTION("playerchoice", s_player_choice);
                //SG_SET_FLAG_OPTION("group", s_group);
                uint8_t metgroup = 0;
                SG_SET_FLAG_OPTION("group", metgroup);

                if (!s_meters)
                {
                    SG_IGNORE_SUB_OPTION(s_meters, s_cmet, IgnoreCMET);
                    SG_IGNORE_SUB_OPTION(s_meters, s_prog, IgnorePROG);
                    SG_IGNORE_SUB_OPTION(s_meters, s_multigame_var, IgnoreMultiGameVar);
                    SG_IGNORE_SUB_OPTION(s_meters, s_player_choice, IgnorePlayerChoice);
                    SG_IGNORE_SUB_OPTION(s_meters, metgroup, IgnoreGroup);
                }
            }

            if (!s_config && !s_hash && !s_psn && !s_parameters && !s_state && !s_extjpinfo && !s_games && !s_gvn)
                s_no_opt = 1;
            else
                s_no_opt = 0;

            return true;
        }

        return res;
    }

    void QcomListAction::IgnoreEGM()
    {
        COMMS_LOG("--egm,-e will be ignored, unset --all before set\n", CLL_Warning);
        s_egm = 0;
    }

    void QcomListAction::IgnoreConfig()
    {
        COMMS_LOG("--config,--devices,--settings will be ignored, unset --all before set\n", CLL_Warning);
        s_config = 0;
        s_devices = 0;
        s_settings = 0;
    }

    void QcomListAction::IgnoreHash()
    {
        COMMS_LOG("--hash will be ignored, unset --all before set\n", CLL_Warning);
        s_hash = 0;
    }

    void QcomListAction::IgnorePSN()
    {
        COMMS_LOG("--psn will be ignored, unset --all before set\n", CLL_Warning);
        s_psn = 0;
    }

    void QcomListAction::IgnoreParameters()
    {
        COMMS_LOG("--parameters will be ignored, unset --all before set\n", CLL_Warning);
        s_parameters = 0;
    }

    void QcomListAction::IgnoreState()
    {
        COMMS_LOG("--state,--concurrents,--noteacceptor will be ignored, unset --all before set\n", CLL_Warning);
        s_state = 0;
        s_concurrents = 0;
        s_note_acceptor = 0;
    }

    void QcomListAction::IgnoreExtJPInfo()
    {
        COMMS_LOG("--extjpinfo,--levels will be ignored, unset --all before set\n", CLL_Warning);
        s_extjpinfo = 0;
        s_levels = 0;
    }

    void QcomListAction::IgnoreGames()
    {
        COMMS_LOG("--games will be ignored, unset --all before set\n", CLL_Warning);
        s_games = 0;
    }

    void QcomListAction::IgnoreGVN()
    {
        COMMS_LOG("--gvn,--varations,--progressive,--meters will be ignored, unset --all before set\n", CLL_Warning);
        s_gvn = 0;
        s_variations = 0;
        s_progressive = 0;
        s_meters = 0;
    }

    void QcomListAction::IgnoreDevices()
    {
        COMMS_LOG("--devices will be ignored, set --config before use\n", CLL_Warning);
        s_devices = 0;
    }

    void QcomListAction::IgnoreSettings()
    {
        COMMS_LOG("--settings will be ignored, set --config before use\n", CLL_Warning);
        s_settings = 0;
    }

    void QcomListAction::IgnoreConcurrents()
    {
        COMMS_LOG("--concurrent will be ignored, set --state before use\n", CLL_Warning);
        s_concurrents = 0;
    }

    void QcomListAction::IgnoreNoteAcceptor()
    {
        COMMS_LOG("--noteacceptor will be ignored, set --state before use\n", CLL_Warning);
        s_note_acceptor = 0;
    }

    void QcomListAction::IgnoreLevels()
    {
        COMMS_LOG("--levels will be ignored, set --extjpinfo before use\n", CLL_Warning);
        s_levels = 0;
    }

    void QcomListAction::IgnoreVariations()
    {
        COMMS_LOG("--variations will be ignored, set --gvn before use\n", CLL_Warning);
        s_variations = 0;
    }

    void QcomListAction::IgnoreProgressive()
    {
        COMMS_LOG("--progressive will be ignored, set --gvn before use\n", CLL_Warning);
        s_progressive = 0;
    }

    void QcomListAction::IgnoreMeters()
    {
        COMMS_LOG("--meters will be ignored, set --gvn before use\n", CLL_Warning);
        s_meters = 0;
        s_cmet = 0;
        s_prog = 0;
        s_multigame_var = 0;
        s_player_choice = 0;
        s_group = 0x10;
    }

    void QcomListAction::IgnoreCMET()
    {
        COMMS_LOG("--cmet will be ignored, set --gvn and --meters before use\n", CLL_Warning);
        s_cmet = 0;
    }

    void QcomListAction::IgnorePROG()
    {
        COMMS_LOG("--prog will be ignored, set --gvn and --meters before use\n", CLL_Warning);
        s_prog = 0;
    }

    void QcomListAction::IgnoreMultiGameVar()
    {
        COMMS_LOG("--multigamevar will be ignored, set --gvn and --meters before use\n", CLL_Warning);
        s_multigame_var = 0;
    }

    void QcomListAction::IgnorePlayerChoice()
    {
        COMMS_LOG("--playerchoice will be ignored, set --gvn and --meters before use\n", CLL_Warning);
        s_player_choice = 0;
    }

    void QcomListAction::IgnoreGroup()
    {
        COMMS_LOG("--group will be ignored, set --gvn and --meters before use\n", CLL_Warning);
        s_group = 0x10;
    }

    void QcomListAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();
            m_options->AddOption(ActionOption("all,a", "list all EGM information, if set, all other options will be ignored"));
            m_options->AddOption(ActionOption("egm,e", "list specified EGM information, if omit, current EGM information will be displayed", Value<uint8>(&s_egm)));
            m_options->AddOption(ActionOption("config", "list EGM configuration information"));
            m_options->AddOption(ActionOption("devices", "list EGM devices expectedness, must be used with config option, otherwise ignored"));
            m_options->AddOption(ActionOption("settings", "list EGM configuration settings, must be used with config option, otherwise ignored"));
            m_options->AddOption(ActionOption("hash", "list EGM hash information"));
            m_options->AddOption(ActionOption("psn", "list EGM PSN information"));
            m_options->AddOption(ActionOption("parameters", "list EGM parameters settings"));
            m_options->AddOption(ActionOption("state", "list EGM current state and door states"));
            m_options->AddOption(ActionOption("concurrents", "list EGM concurretns states, must be used with state option, otherwise ignored"));
            m_options->AddOption(ActionOption("noteacceptor", "list EGM note acceptor state, must be used with state option, otherwise ignored"));
            m_options->AddOption(ActionOption("extjpinfo", "list EGM external jackpot information"));
            m_options->AddOption(ActionOption("levels", "list all EGM external jackpot level information, must be used with extjpinfo option"));
            m_options->AddOption(ActionOption("games", "list all games of EGM"));
            m_options->AddOption(ActionOption("gvn", "list sepcified game of EGM", Value<uint16>(&s_gvn)));
            m_options->AddOption(ActionOption("variations", "list game variations information, must be used with gvn option"));
            m_options->AddOption(ActionOption("progressive", "list game progressive information, must be used with gvn option"));
            m_options->AddOption(ActionOption("meters", "list game bet meters information, must be used with gvn option"));
            m_options->AddOption(ActionOption("cmet", "list total number of bets made in this category for the game, must be used with meters option"));
            m_options->AddOption(ActionOption("prog", "list game progressive meters information, must be used with meters option"));
            m_options->AddOption(ActionOption("multigamevar", "list game multi-game/variation meters information, must be used with meters option"));
            m_options->AddOption(ActionOption("playerchoice", "list game player choice meters information, must be used with meters option"));
            m_options->AddOption(ActionOption("group", "list sepcified group meters information of game, must be used with meters option", Value<uint8>(&s_group)));
            m_options->AddOption(ActionOption("help,h", "help message"));
        }
    }

    ActionPtr QcomListAction::Clone()
    {
        return Action::DoClone<QcomListAction>();
    }

    const char* QcomListAction::Description() const
    {
        static const char* des = "\tQcom List:\n\t\tl,ls,list\n";

        return des;
    }




    
}




