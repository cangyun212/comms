#include "Core.hpp"

#include <algorithm>

#include "boost/program_options.hpp"

#include "Utils.hpp"
#include "Comms.hpp"
#include "QcomAction.hpp"
#include "ActionOptions.hpp"

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

    uint8 QcomEGMConfRequestAction::m_mef = 1;
    uint8 QcomEGMConfRequestAction::m_gcr = 1;
    uint8 QcomEGMConfRequestAction::m_psn = 1;

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

        if (vm.count("mef"))
        {
            m_mef = 1;
        }

        if (vm.count("gcr"))
        {
            m_gcr = 1;
        }

        if (vm.count("psn"))
        {
            m_psn = 1;
        }

        if (vm.count("help") || (!m_mef && !m_gcr && !m_psn))
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

            m_options->AddOption(ActionOption("mef,m", "enable egm machine"));
            m_options->AddOption(ActionOption("gcr,g", "commands egm queue the EGM Game configuration Response"));
            m_options->AddOption(ActionOption("psn,p", "reset all Poll Sequence Numbers"));
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

    uint8     QcomEGMConfAction::m_jur = 0;
    uint32    QcomEGMConfAction::m_den = 1;
    uint32    QcomEGMConfAction::m_tok = 100;
    uint32    QcomEGMConfAction::m_maxden = 100;
    uint16    QcomEGMConfAction::m_minrtp = 8500;
    uint16    QcomEGMConfAction::m_maxrtp = 10000;
    uint16    QcomEGMConfAction::m_maxsd = 15;
    uint16    QcomEGMConfAction::m_maxlines = 1024;
    uint32    QcomEGMConfAction::m_maxbet = 5000;
    uint32    QcomEGMConfAction::m_maxnpwin = 100000;
    uint32    QcomEGMConfAction::m_maxpwin = 2500000;
    uint32    QcomEGMConfAction::m_maxect = 1000000;

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

            m_options->AddOption(ActionOption("jur","jurisdictions", Value<uint8>(&m_jur)));
            m_options->AddOption(ActionOption("den", "credit denomination", Value<uint32>(&m_den)));
            m_options->AddOption(ActionOption("tok", "coin/token denomination", Value<uint32>(&m_tok)));
            m_options->AddOption(ActionOption("maxden", "EGM credit meter max denomination", Value<uint32>(&m_maxden)));
            m_options->AddOption(ActionOption("minrtp", "minimum RTP", Value<uint16>(&m_minrtp)));
            m_options->AddOption(ActionOption("maxrtp", "maximum RTP", Value<uint16>(&m_maxrtp)));

            m_options->AddOption(ActionOption(
                "maxsd",
                "regulatory maximum EGM game theoretical standard deviation", 
                Value<uint16>(&m_maxsd)));

            m_options->AddOption(ActionOption(
                "maxlines", 
                "regulatory maximum number of playlines in any game in the EGM", 
                Value<uint16>(&m_maxlines)));

            m_options->AddOption(ActionOption("maxbet", "maximum bet per play", Value<uint32>(&m_maxbet)));

            m_options->AddOption(ActionOption(
                "maxnpwin", 
                "regulatory maximum non-progressive EGM win permitted uint32 any game element", 
                Value<uint32>(&m_maxnpwin)));

            m_options->AddOption(ActionOption(
                "maxpwin", 
                "regulatory maximum SAP progressive EGM win permitted per SAP hit", 
                Value<uint32>(&m_maxpwin)));

            m_options->AddOption(ActionOption("maxect", "regulatory maximum allowable ECT to/from the EGM", Value<uint32>(&m_maxect)));
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

    uint32  QcomBroadcastAction::m_broadcast_type = 0;
    std::string QcomBroadcastAction::m_gpm_text;
    std::string QcomBroadcastAction::m_sds_text;
    std::string QcomBroadcastAction::m_sdl_text;

    //Class for Broadcast Action
    QcomBroadcastAction::QcomBroadcastAction()
                    : Action(Action::AT_QCOM_BROADCAST)
    {
    }

    QcomBroadcastAction::~QcomBroadcastAction()
    {

    }

    bool QcomBroadcastAction::Parse(const Action::ActionArgs &args)
    {
        bool res = false;

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: broadcast [options]\n");
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

    void QcomBroadcastAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();

            m_options->AddOption(ActionOption(
                "type,t", 
                "what is the type of broadcast \n\
                SEEK_EGM = 1,\nTIME_DATA = 2,\nLJP_CUR_AMOUNT = 3,\nGPM = 4,\nPOLL_ADDRESS = 5,\nSITE_DETAILS = 6,",
                Value<uint32>(&m_broadcast_type)));

            m_options->AddOption(ActionOption("gpmtext,g", "gpm text", Value<std::string>(&m_gpm_text)));
            m_options->AddOption(ActionOption("sdstext,s", "site details / Name of licensed venue", Value<std::string>(&m_sds_text)));

            m_options->AddOption(ActionOption(
                "sdltext,l", 
                "site details / Address  Contact details of licensed venue", 
                Value<std::string>(&m_sdl_text)));

            m_options->AddOption(ActionOption("help,h", "help message"));
        }
    }

    ActionPtr QcomBroadcastAction::Clone()
    {
        return Action::DoClone<QcomBroadcastAction>();
    }

    const char* QcomBroadcastAction::Description() const
    {
        static const char* des = "\tQcom Broadcast :\n\t\tbc,broadcast\n";
        return des;
    }

    uint8  QcomGameConfigurationAction::m_var = 0;
    uint8  QcomGameConfigurationAction::m_var_lock = 0;
    uint8  QcomGameConfigurationAction::m_game_enable = 0;
    uint8  QcomGameConfigurationAction::m_pnum = 0;
    std::vector<uint8> QcomGameConfigurationAction::m_lp(0);
    std::vector<uint32> QcomGameConfigurationAction::m_camt(0);

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

            res = false;
        }
        else
        {
            res = true;
        }

        return res;
    }

    void QcomGameConfigurationAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();

            m_options->AddOption(ActionOption("var", "TODO:", Value<uint8>(&m_var)));
            m_options->AddOption(ActionOption("varlock", "TODO", Value<uint8>(&m_var_lock)));
            m_options->AddOption(ActionOption("gameenable", "TODO", Value<uint8>(&m_game_enable)));
            m_options->AddOption(ActionOption("pnum", "TODO", Value<uint8>(&m_pnum)));
            m_options->AddOption(ActionOption("linkJackpot", "TODO", Value< std::vector<uint8> >(&m_lp)));
            m_options->AddOption(ActionOption("amount", "TODO", Value< std::vector<uint32> >(&m_camt)));
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

    void QcomGameConfigurationAction::LP(std::vector<uint8_t>& lp) const
    {
        lp.clear();
        lp.reserve(m_lp.size());
        std::for_each(m_lp.begin(), m_lp.end(), [&](uint8 const&_lp) { lp.push_back(_lp.value); });
    }

    void QcomGameConfigurationAction::CAMT(std::vector<uint32_t>& camt) const
    {
        camt.clear();
        camt.reserve(m_camt.size());
        std::for_each(m_camt.begin(), m_camt.end(), [&](uint32 const& _camt) { camt.push_back(_camt.value); });
    }
	
	uint8  QcomGameConfigurationChangeAction::m_var = 0;
    uint8  QcomGameConfigurationChangeAction::m_game_enable = 1;

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

    void QcomGameConfigurationChangeAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();

            m_options->AddOption(ActionOption("var", "TODO", Value<uint8>(&m_var)));
            m_options->AddOption(ActionOption("gameenable", "TODO", Value<uint8>(&m_game_enable)));
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


     uint8   QcomEGMParametersAction::m_reserve = 1;
     uint8   QcomEGMParametersAction::m_autoplay = 0;
     uint8   QcomEGMParametersAction::m_crlimitmode = 0;
     uint8   QcomEGMParametersAction::m_opr = 0;
     uint32  QcomEGMParametersAction::m_lwin = 1000000;
     uint32  QcomEGMParametersAction::m_crlimit = 1000000;
     uint8   QcomEGMParametersAction::m_dumax = 5;
     uint32  QcomEGMParametersAction::m_dulimit = 1000000;
     uint16  QcomEGMParametersAction::m_tzadj = 0;
     uint32  QcomEGMParametersAction::m_pwrtime = 900;
     uint8   QcomEGMParametersAction::m_pid = 0;
     uint16  QcomEGMParametersAction::m_eodt = 180;
     uint32  QcomEGMParametersAction::m_npwinp = 1000000;
     uint32  QcomEGMParametersAction::m_sapwinp = 1000000;

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

    void QcomEGMParametersAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();

            m_options->AddOption(ActionOption("reserve", "Enable or Disable RESERV", Value<uint8>(&m_reserve)));
            m_options->AddOption(ActionOption("autoplay", "Enable or Disable autoplay", Value<uint8>(&m_autoplay)));
            m_options->AddOption(ActionOption("crlimitmode", "credit limit", Value<uint8>(&m_crlimitmode)));
            m_options->AddOption(ActionOption("opr", "Monitoring system operator ID", Value<uint8>(&m_opr)));
            m_options->AddOption(ActionOption("lwin", "Large win lockup threshold", Value<uint32>(&m_lwin)));
            m_options->AddOption(ActionOption("crlimit", "Credit-in lockout value", Value<uint32>(&m_crlimit)));
            m_options->AddOption(ActionOption("dumax", "Maximum allowable number of Gambles", Value<uint8>(&m_dumax)));
            m_options->AddOption(ActionOption("dulimit", "Double-Up/Gamble Limit", Value<uint32>(&m_dulimit)));
            m_options->AddOption(ActionOption("tzadj", "Time zone adjust", Value<uint16>(&m_tzadj)));
            m_options->AddOption(ActionOption("pwrtime", "Power-save Time-out value", Value<uint32>(&m_pwrtime)));
            m_options->AddOption(ActionOption("pid", "Player Information Display", Value<uint8>(&m_pid)));
            m_options->AddOption(ActionOption("eodt", "End of the day time", Value<uint16>(&m_eodt)));
            m_options->AddOption(ActionOption("npwinp", "Non-Progressive Win Payout Threshold", Value<uint32>(&m_npwinp)));
            m_options->AddOption(ActionOption("sapwinp", "Stand alone progressive win payout threshold", Value<uint32>(&m_sapwinp)));
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


    uint8  QcomPurgeEventsAction::m_psn = 1;
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

            m_options->AddOption(ActionOption("psn", "Poll sequence number", Value<uint8>(&m_psn)));
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

}




