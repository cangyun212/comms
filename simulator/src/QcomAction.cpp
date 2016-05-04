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

    QcomEGMConfRequestAction::QcomEGMConfRequestAction()
        : Action(Action::AT_QCOM_EGM_CONF_REQ)
        , m_mef(0)
        , m_gcr(0)
        , m_psn(0)
    {

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
        desc.add_options()
            ("mef,m", "")
            ("gcr,g", "")
            ("psn,p", "")
            ("help,h", "");

        vis_desc.add_options()
                ("mef,m", "enable egm machine")
                ("gcr,g", "commands egm queue the EGM Game configuration Response")
                ("psn,p", "reset all Poll Sequence Numbers")
                ("help,h", "help message");

        try
        {
            po::store(po::command_line_parser(argv).options(desc).run(), vm);
            po::notify(vm);
        }
        catch (po::error const& error)
        {
            COMMS_LOG(error.what(), CLL_Error);
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

        if (!vm.count("help") || (!m_mef && !m_gcr && !m_psn))
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

    QcomEGMConfAction::QcomEGMConfAction()
                    : Action(Action::AT_QCOM_EGM_CONF)
                    , m_jur(0)
                    , m_den(0)
                    , m_tok(0)
                    , m_maxden(0)
                    , m_minrtp(0)
                    , m_maxrtp(0)
                    , m_maxsd(0)
                    , m_maxlines(0)
                    , m_maxbet(0)
                    , m_maxnpwin(0)
                    , m_maxpwin(0)
                    , m_maxect(0)
    {

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

        desc.add_options()
            ("jur", po::value<uint8>(&m_jur)->default_value(0), "")
            ("den", po::value<uint32>(&m_den)->default_value(1), "")
            ("tok", po::value<uint32>(&m_tok)->default_value(100), "")
            ("maxden", po::value<uint32>(&m_maxden)->default_value(100), "")
            ("minrtp", po::value<uint16>(&m_minrtp)->default_value(8500), "")
            ("maxrtp", po::value<uint16>(&m_maxrtp)->default_value(10000), "")
            ("maxsd", po::value<uint16>(&m_maxsd)->default_value(15), "")
            ("maxlines", po::value<uint16>(&m_maxlines)->default_value(243), "")
            ("maxbet", po::value<uint32>(&m_maxbet)->default_value(5000), "")
            ("maxnpwin", po::value<uint32>(&m_maxnpwin)->default_value(1000000), "")
            ("maxpwin", po::value<uint32>(&m_maxpwin)->default_value(2500000), "")
            ("maxect", po::value<uint32>(&m_maxect)->default_value(1000000), "")
            ("help,h", "");

        vis_desc.add_options()
                ("jur", "jurisdictions")
                ("den", "credit denomination")
                ("tok", "coin/token denomination")
                ("maxden", "EGM credit meter max denomination")
                ("minrtp", "minimum RTP")
                ("maxrtp", "maximum RTP")
                ("maxsd", "regulatory maximum EGM game theoretical standard deviation")
                ("maxlines", "regulatory maximum number of playlines in any game in the EGM")
                ("maxbet", "maximum bet per play")
                ("maxnpwin", "regulatory maximum non-progressive EGM win permitted int any game element")
                ("maxpwin", "regulatory maximum SAP progressive EGM win permitted per SAP hit")
                ("maxect", "regulatory maximum allowable ECT to/from the EGM")
                ("help,h", "help message");

        try
        {
            po::store(po::command_line_parser(argv).options(desc).run(), vm);
            po::notify(vm);
        }
        catch (po::error const& error)
        {
            COMMS_LOG(error.what(), CLL_Error);
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

    //Class for Broadcast Action
    QcomBroadcastAction::QcomBroadcastAction()
                    : Action(Action::AT_QCOM_BROADCAST)
                    , m_broadcast_type(0)
    {
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

        desc.add_options()
            ("type,t", po::value<uint32>(&m_broadcast_type)->default_value(0), "")
            ("gpmtext,g", po::value<std::string>(&m_gpm_text), "")
            ("sdstext,s", po::value<std::string>(&m_sds_text), "")
            ("sdltext,l", po::value<std::string>(&m_sdl_text), "")
            ("help,h", "");

        vis_desc.add_options()
                ("type,t", "what is the type of broadcast \nSEEK_EGM = 1,\nTIME_DATA = 2,\nLJP_CUR_AMOUNT = 3,\nGPM = 4,\nPOLL_ADDRESS = 5,\nSITE_DETAILS = 6,")
                ("gpmtext,g", "gpm text")
                ("sdstext,s", "site details / Name of licensed venue")
                ("sdltext,l", "site details / Address  Contact details of licensed venue")
                ("help,h", "help message");

        try
        {
            po::store(po::command_line_parser(argv).options(desc).run(), vm);
            po::notify(vm);
        }
        catch (po::error const& error)
        {
            COMMS_LOG(error.what(), CLL_Error);
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

    QcomGameConfigurationAction::QcomGameConfigurationAction()
        : Action(Action::AT_QCOM_GAME_CONF)
        , m_var(0)
        , m_var_lock(0)
        , m_game_enable(0)
        , m_pnum(0)
    {
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

        desc.add_options()
            ("var", po::value<uint8>(&m_var)->default_value(0), "")
            ("varlock", po::value<uint8>(&m_var_lock)->default_value(0), "")
            ("gameenable", po::value<uint8>(&m_game_enable)->default_value(1), "")
            ("pnum", po::value<uint8>(&m_pnum)->default_value(0), "")
            ("linkJackpot", po::value< std::vector<uint8> >(&m_lp)->multitoken(), "")
            ("amount", po::value< std::vector<uint32> >(&m_camt)->multitoken(), "")
            ("help,h", "");

        vis_desc.add_options()
                ("var", "define jurisdictions if set")
                ("varlock", "define credit denomination if set")
                ("gameenable", "define coin/token denomination if set")
                ("pnum", "define max denomination if set")
                ("linkJackpot", "define min")
                ("amount", "define max denomination if set")
                ("help,h", "help message");

        try
        {
            po::store(po::command_line_parser(argv).options(desc).run(), vm);
            po::notify(vm);
        }
        catch (po::error const& error)
        {
            COMMS_LOG(error.what(), CLL_Error);
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
        std::for_each(m_lp.begin(), m_lp.end(), [&](auto const&_lp) { lp.push_back(_lp.value); });
    }

    void QcomGameConfigurationAction::CAMT(std::vector<uint32_t>& camt) const
    {
        camt.clear();
        camt.reserve(m_camt.size());
        std::for_each(m_camt.begin(), m_camt.end(), [&](auto const& _camt) { camt.push_back(_camt.value); });
    }

}




