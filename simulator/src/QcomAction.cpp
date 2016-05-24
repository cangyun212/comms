#include "Core.hpp"

#include <algorithm>

#include "boost/program_options.hpp"

#include "Utils.hpp"
#include "Comms.hpp"
#include "QcomAction.hpp"

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
        m_options = MakeSharedPtr<ActionOptions>();

        this->AddOption("mef,m", nullptr, "enable egm machine");
        this->AddOption("gcr,g", nullptr, "commands egm queue the EGM Game configuration Response");
        this->AddOption("psn,p", nullptr, "reset all Poll Sequence Numbers");

    }

    QcomEGMConfRequestAction::~QcomEGMConfRequestAction()
    {

    }

    bool QcomEGMConfRequestAction::Parse(const ActionArgs &args)
    {
        bool res = false;

        BOOST_ASSERT(args.size());

        std::vector<std::string> argv(args.size());
        argv.assign(args.begin() + 1, args.end());

        po::variables_map vm;
        po::options_description desc;
        po::options_description vis_desc;

        this->FillOptionsDescription(desc, vis_desc);

        try
        {
            po::store(po::command_line_parser(argv).options(desc).run(), vm);
            po::notify(vm);
        }
        catch (po::error const& error)
        {
            COMMS_LOG(boost::format("%1%\n") % error.what(), CLL_Error);
            return false;
        }
        catch (nm::bad_numeric_cast const&)
        {
            COMMS_LOG("Option value is out of range\n", CLL_Error);
            return false;
        }
        catch (boost::bad_lexical_cast const&)
        {
            COMMS_LOG("Invalid option value\n", CLL_Error);
            return false;
        }

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
        m_options = MakeSharedPtr<ActionOptions>();

		this->AddOption("jur", Value<uint8>(&m_jur), "jurisdictions");
		this->AddOption("den", Value<uint32>(&m_den), "credit denomination");
		this->AddOption("tok", Value<uint32>(&m_tok), "coin/token denomination");
		this->AddOption("maxden", Value<uint32>(&m_maxden), "EGM credit meter max denomination");
		this->AddOption("minrtp", Value<uint16>(&m_minrtp), "minimum RTP");
		this->AddOption("maxrtp", Value<uint16>(&m_maxrtp), "maximum RTP");
		this->AddOption("maxsd", Value<uint16>(&m_maxsd), "regulatory maximum EGM game theoretical standard deviation");
		this->AddOption("maxlines", Value<uint16>(&m_maxlines), "regulatory maximum number of playlines in any game in the EGM");
		this->AddOption("maxbet", Value<uint32>(&m_maxbet), "maximum bet per play");
		this->AddOption("maxnpwin", Value<uint32>(&m_maxnpwin), "regulatory maximum non-progressive EGM win permitted uint32 any game element");
		this->AddOption("maxpwin", Value<uint32>(&m_maxpwin), "regulatory maximum SAP progressive EGM win permitted per SAP hit");
		this->AddOption("maxect", Value<uint32>(&m_maxect), "regulatory maximum allowable ECT to/from the EGM");
	}

    QcomEGMConfAction::~QcomEGMConfAction()
    {

    }

    bool QcomEGMConfAction::Parse(const Action::ActionArgs &args)
    {
        bool res = false;

        BOOST_ASSERT(args.size());

        std::vector<std::string> argv(args.size());
        argv.assign(args.begin() + 1, args.end());

        po::variables_map vm;
        po::options_description desc;
        po::options_description vis_desc;

        this->FillOptionsDescription(desc, vis_desc);


        try
        {
            po::store(po::command_line_parser(argv).options(desc).run(), vm);
            po::notify(vm);
        }
        catch (po::error const& error)
        {
            COMMS_LOG(boost::format("%1%\n") % error.what(), CLL_Error);
            return false;
        }
        catch (nm::bad_numeric_cast const&)
        {
            COMMS_LOG("Option value is out of range\n", CLL_Error);
            return false;
        }
        catch (boost::bad_lexical_cast const&)
        {
            COMMS_LOG("Invalid option value\n", CLL_Error);
            return false;
        }

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
        m_options = MakeSharedPtr<ActionOptions>();

        this->AddOption("type,t", Value<uint32>(&m_broadcast_type), "what is the type of broadcast \nSEEK_EGM = 1,\nTIME_DATA = 2,\nLJP_CUR_AMOUNT = 3,\nGPM = 4,\nPOLL_ADDRESS = 5,\nSITE_DETAILS = 6,");
        this->AddOption("gpmtext,g", Value<std::string>(&m_gpm_text), "gpm text");
        this->AddOption("sdstext,s", Value<std::string>(&m_sds_text), "site details / Name of licensed venue");
        this->AddOption("sdltext,l", Value<std::string>(&m_sdl_text), "site details / Address  Contact details of licensed venue");

    }

    QcomBroadcastAction::~QcomBroadcastAction()
    {

    }

    bool QcomBroadcastAction::Parse(const Action::ActionArgs &args)
    {
        bool res = false;

        BOOST_ASSERT(args.size());

        std::vector<std::string> argv(args.size());
        argv.assign(args.begin() + 1, args.end());

        po::variables_map vm;
        po::options_description desc;
        po::options_description vis_desc;

        this->FillOptionsDescription(desc, vis_desc);


        try
        {
            po::store(po::command_line_parser(argv).options(desc).run(), vm);
            po::notify(vm);
        }
        catch (po::error const& error)
        {
            COMMS_LOG(boost::format("%1%\n") % error.what(), CLL_Error);
            return false;
        }
        catch (nm::bad_numeric_cast const&)
        {
            COMMS_LOG("Option value is out of range\n", CLL_Error);
            return false;
        }
        catch (boost::bad_lexical_cast const&)
        {
            COMMS_LOG("Invalid option value\n", CLL_Error);
            return false;
        }

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
        m_options = MakeSharedPtr<ActionOptions>();

        this->AddOption("var", Value<uint8>(&m_var), "define jurisdictions if set");
        this->AddOption("varlock", Value<uint8>(&m_var_lock), "define credit denomination if set");
        this->AddOption("gameenable", Value<uint8>(&m_game_enable), "define coin/token denomination if set");
        this->AddOption("pnum", Value<uint8>(&m_pnum), "define max denomination if set");
        this->AddOption("linkJackpot", Value< std::vector<uint8> >(&m_lp), "define min");
        this->AddOption("amount", Value< std::vector<uint32> >(&m_camt), "define max denomination if set");

    }

    QcomGameConfigurationAction::~QcomGameConfigurationAction()
    {

    }

    bool QcomGameConfigurationAction::Parse(const Action::ActionArgs &args)
    {
        bool res = false;

        BOOST_ASSERT(args.size());

        std::vector<std::string> argv(args.size());
        argv.assign(args.begin() + 1, args.end());

        po::variables_map vm;
        po::options_description desc;
        po::options_description vis_desc;

        this->FillOptionsDescription(desc, vis_desc);


        try
        {
            po::store(po::command_line_parser(argv).options(desc).run(), vm);
            po::notify(vm);
        }
        catch (po::error const& error)
        {
            COMMS_LOG(boost::format("%1%\n") % error.what(), CLL_Error);
            return false;
        }
        catch (nm::bad_numeric_cast const&)
        {
            COMMS_LOG("Option value is out of range\n", CLL_Error);
            return false;
        }
        catch (boost::bad_lexical_cast const&)
        {
            COMMS_LOG("Invalid option value\n", CLL_Error);
            return false;
        }

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

        this->AddOption("var", Value<uint8>(&m_var), "define jurisdictions if set");
        this->AddOption("gameenable", Value<uint8>(&m_game_enable), "define coin/token denomination if set");
    }

    QcomGameConfigurationChangeAction::~QcomGameConfigurationChangeAction()
    {

    }
    bool QcomGameConfigurationChangeAction::Parse(const Action::ActionArgs &args)
    {
        bool res = false;

        BOOST_ASSERT(args.size());

        std::vector<std::string> argv(args.size());
        argv.assign(args.begin() + 1, args.end());

        po::variables_map vm;
        po::options_description desc;
        po::options_description vis_desc;

        this->FillOptionsDescription(desc, vis_desc);

        try
        {
            po::store(po::command_line_parser(argv).options(desc).run(), vm);
            po::notify(vm);
        }
        catch (po::error const& error)
        {
            COMMS_LOG(boost::format("%1%\n") % error.what(), CLL_Error);
            return false;
        }
        catch (nm::bad_numeric_cast const&)
        {
            COMMS_LOG("Option value is out of range\n", CLL_Error);
            return false;
        }
        catch (boost::bad_lexical_cast const&)
        {
            COMMS_LOG("Invalid option value\n", CLL_Error);
            return false;
        }

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
        m_options = MakeSharedPtr<ActionOptions>();

        this->AddOption("reserve", Value<uint8>(&m_reserve), "Enable or Disable RESERV");
        this->AddOption("autoplay", Value<uint8>(&m_autoplay), "Enable or Disable autoplay");
        this->AddOption("crlimitmode", Value<uint8>(&m_crlimitmode), "credit limit");
        this->AddOption("opr", Value<uint8>(&m_opr), "Monitoring system operator ID");
        this->AddOption("lwin", Value<uint32>(&m_lwin), "Large win lockup threshold");
        this->AddOption("crlimit", Value<uint32>(&m_crlimit), "Credit-in lockout value");
        this->AddOption("dumax", Value<uint8>(&m_dumax), "Maximum allowable number of Gambles");
        this->AddOption("dulimit", Value<uint32>(&m_dulimit), "Double-Up/Gamble Limit");
        this->AddOption("tzadj", Value<uint16>(&m_tzadj), "Time zone adjust");
        this->AddOption("pwrtime", Value<uint32>(&m_pwrtime), "Power-save Time-out value");
        this->AddOption("pid", Value<uint8>(&m_pid), "Player Information Display");
        this->AddOption("eodt", Value<uint16>(&m_eodt), "End of the day time");
        this->AddOption("npwinp", Value<uint32>(&m_npwinp), "Non-Progressive Win Payout Threshold");
        this->AddOption("sapwinp", Value<uint32>(&m_sapwinp), "Stand alone progressive win payout threshold");
    }

    QcomEGMParametersAction::~QcomEGMParametersAction()
    {

    }

    bool QcomEGMParametersAction::Parse(const Action::ActionArgs &args)
    {
        bool res = false;

        BOOST_ASSERT(args.size());

        std::vector<std::string> argv(args.size());
        argv.assign(args.begin() + 1, args.end());

        po::variables_map vm;
        po::options_description desc;
        po::options_description vis_desc;

        this->FillOptionsDescription(desc, vis_desc);

        try
        {
            po::store(po::command_line_parser(argv).options(desc).run(), vm);
            po::notify(vm);
        }
        catch (po::error const& error)
        {
            COMMS_LOG(boost::format("%1%\n") % error.what(), CLL_Error);
            return false;
        }
        catch (nm::bad_numeric_cast const&)
        {
            COMMS_LOG("Option value is out of range\n", CLL_Error);
            return false;
        }
        catch (boost::bad_lexical_cast const&)
        {
            COMMS_LOG("Invalid option value\n", CLL_Error);
            return false;
        }

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
        m_options = MakeSharedPtr<ActionOptions>();

        this->AddOption("psn", Value<uint8>(&m_psn), "Poll sequence number");
        this->AddOption("evnto", Value<uint8>(&m_evtno), "Event sequence number");
    }

    QcomPurgeEventsAction::~QcomPurgeEventsAction()
    {

    }

    bool QcomPurgeEventsAction::Parse(const ActionArgs &args)
    {
        bool res = false;

        BOOST_ASSERT(args.size());

        std::vector<std::string> argv(args.size());
        argv.assign(args.begin() + 1, args.end());

        po::variables_map vm;
        po::options_description desc;
        po::options_description vis_desc;

        this->FillOptionsDescription(desc, vis_desc);

        try
        {
            po::store(po::command_line_parser(argv).options(desc).run(), vm);
            po::notify(vm);
        }
        catch (po::error const& error)
        {
            COMMS_LOG(boost::format("%1%\n") % error.what(), CLL_Error);
            return false;
        }
        catch (nm::bad_numeric_cast const&)
        {
            COMMS_LOG("Option value is out of range\n", CLL_Error);
            return false;
        }
        catch (boost::bad_lexical_cast const&)
        {
            COMMS_LOG("Invalid option value\n", CLL_Error);
            return false;
        }

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




