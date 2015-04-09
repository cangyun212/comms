#include "core/core.hpp"
#include "core/core_utils.hpp"
#include "core/console/core_console_printer.hpp"

#include <algorithm>

#include "boost/program_options.hpp"

#include "simulator/qcom_action.hpp"

namespace po = boost::program_options;

namespace sg {

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

    bool QcomEGMConfRequestAction::Parse(const ActionArgs &args, const ActionError **err)
    {
        bool res = false;

        BOOST_ASSERT(args.size());

        std::vector<std::string> argv(args.size());
        argv.assign(args.begin() + 1, args.end());

        po::variables_map vm;
        po::options_description desc;
        po::options_description vis_desc;
        po::positional_options_description pos_desc;
        desc.add_options()
                ("mef,m", "")
                ("gcr,g", "")
                ("psn,p", "")
                ("help,h", "")
                ("dummy", po::value< std::vector<std::string> >(), "");

        pos_desc.add("dummy", -1);
        vis_desc.add_options()
                ("mef,m", "enable egm machine")
                ("gcr,g", "commands egm queue the EGM Game configuration Response")
                ("psn,p", "reset all Poll Sequence Numbers")
                ("help,h", "help message");

        po::store(po::command_line_parser(argv).options(desc).positional(pos_desc).allow_unregistered().run(), vm);
        po::notify(vm);

        if (!vm.count("help"))
        {
            if (vm.count("mef"))
            {
                m_mef = 1;
                res = true;
            }
            if (vm.count("gcr"))
            {
                m_gcr = 1;
                res = true;
            }
            if (vm.count("psn"))
            {
                m_psn = 1;
                res = true;
            }

        }
        else
        {
            CORE_START_PRINT_BLOCK();
            CORE_PRINT_BLOCK("\nUsage: configreq [options]\n");
            CORE_PRINT_BLOCK(vis_desc);
            CORE_PRINT_BLOCK("\n");
            CORE_END_PRINT_BLOCK();
        }

        if (err)
        {
            *err = &m_err;
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

    bool QcomEGMConfAction::Parse(const Action::ActionArgs &args, const ActionError **err)
    {
        bool res = false;

        BOOST_ASSERT(args.size());

        std::vector<std::string> argv(args.size());
        argv.assign(args.begin() + 1, args.end());

        po::variables_map vm;
        po::options_description desc;
        po::options_description vis_desc;
        po::positional_options_description pos_desc;

        desc.add_options()
                ("jur", po::value<int>(&m_jur)->default_value(0), "")
                ("den", po::value<int>(&m_den)->default_value(1), "")
                ("tok", po::value<int>(&m_tok)->default_value(100), "")
                ("maxden", po::value<int>(&m_maxden)->default_value(100), "")
                ("minrtp", po::value<int>(&m_minrtp)->default_value(8500), "")
                ("maxrtp", po::value<int>(&m_maxrtp)->default_value(10000), "")
                ("maxsd", po::value<int>(&m_maxsd)->default_value(15), "")
                ("maxlines", po::value<int>(&m_maxlines)->default_value(243), "")
                ("maxbet", po::value<int>(&m_maxbet)->default_value(5000), "")
                ("maxnpwin", po::value<int>(&m_maxnpwin)->default_value(1000000), "")
                ("maxpwin", po::value<int>(&m_maxpwin)->default_value(2500000), "")
                ("maxect", po::value<int>(&m_maxect)->default_value(1000000), "")
                ("help,h", "")
                ("dummy", po::value< std::vector<std::string> >(), "");

        pos_desc.add("dummy", -1);
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

        po::store(po::command_line_parser(argv).options(desc).positional(pos_desc).allow_unregistered().run(), vm);
        po::notify(vm);

        if (!vm.count("help"))
        {
            if (m_jur >=0 && m_den >=0 && m_tok >= 0 &&
                    m_maxden >= 0 && m_minrtp >=0 &&
                    m_maxrtp >= 0 && m_maxsd >=0 &&
                    m_maxlines >= 0 && m_maxbet >= 0 &&
                    m_maxnpwin >= 0 && m_maxpwin >= 0 &&
                    m_maxect >= 0)
            {
                res = true;
            }
        }
        else
        {
            CORE_START_PRINT_BLOCK();
            CORE_PRINT_BLOCK("\nUsage: egmconfig [options]\n");
            CORE_PRINT_BLOCK(vis_desc);
            CORE_PRINT_BLOCK("\n");
            CORE_END_PRINT_BLOCK();
        }

        if (err)
        {
            *err = &m_err;
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

    bool QcomBroadcastAction::Parse(const Action::ActionArgs &args, const ActionError **err)
    {
        bool res = false;

        BOOST_ASSERT(args.size());

        std::vector<std::string> argv(args.size());
        argv.assign(args.begin() + 1, args.end());

        po::variables_map vm;
        po::options_description desc;
        po::options_description vis_desc;
        po::positional_options_description pos_desc;

        desc.add_options()
                ("type,t", po::value<uint32_t>(&m_broadcast_type)->default_value(0), "")
                ("gpmtext,g", po::value<std::string>(&m_gpm_text), "")
                ("sdstext,s", po::value<std::string>(&m_sds_text), "")
                ("sdltext,l", po::value<std::string>(&m_sdl_text), "")
                ("help,h", "")
                ("dummy", po::value< std::vector<std::string> >(), "");

        pos_desc.add("dummy", -1);
        vis_desc.add_options()
                ("type,t", "what is the type of broadcast \nSEEK_EGM = 1,\nTIME_DATA = 2,\nLJP_CUR_AMOUNT = 3,\nGPM = 4,\nPOLL_ADDRESS = 5,\nSITE_DETAILS = 6,")
                ("gpmtext,g", "gpm text")
                ("sdstext,s", "site details / Name of licensed venue")
                ("sdltext,l", "site details / Address  Contact details of licensed venue")
                ("help,h", "help message");

        po::store(po::command_line_parser(argv).options(desc).positional(pos_desc).allow_unregistered().run(), vm);
        po::notify(vm);

        if (!vm.count("help"))
        {
            res = true;
        }
        else
        {
            CORE_START_PRINT_BLOCK();
            CORE_PRINT_BLOCK("\nUsage: broadcast [options]\n");
            CORE_PRINT_BLOCK(vis_desc);
            CORE_PRINT_BLOCK("\n");
            CORE_END_PRINT_BLOCK();
        }

        if (err)
        {
            *err = &m_err;
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

    bool QcomGameConfigurationAction::Parse(const Action::ActionArgs &args, const ActionError **err)
    {
        bool res = false;

        BOOST_ASSERT(args.size());

        std::vector<std::string> argv(args.size());
        argv.assign(args.begin() + 1, args.end());

        po::variables_map vm;
        po::options_description desc;
        po::options_description vis_desc;
        po::positional_options_description pos_desc;

        desc.add_options()
                ("var", po::value<int>(&m_var)->default_value(0), "")
                ("varlock", po::value<int>(&m_var_lock)->default_value(0), "")
                ("gameenable", po::value<int>(&m_game_enable)->default_value(1), "")
                ("pnum", po::value<int>(&m_pnum)->default_value(0), "")
                ("linkJackpot", po::value< std::vector<int> >(&m_lp)->multitoken(), "")
                ("amount", po::value< std::vector<int> >(&m_camt)->multitoken(), "")
                ("help,h", "")
                ("dummy", po::value< std::vector<std::string> >(), "");

        pos_desc.add("dummy", -1);
        vis_desc.add_options()
                ("var", "define jurisdictions if set")
                ("varlock", "define credit denomination if set")
                ("gameenable", "define coin/token denomination if set")
                ("pnum", "define max denomination if set")
                ("linkJackpot", "define min")
                ("amount", "define max denomination if set")
                ("help,h", "help message");

        po::store(po::command_line_parser(argv).options(desc).positional(pos_desc).allow_unregistered().run(), vm);
        po::notify(vm);

        if (!vm.count("help"))
        {
            if (m_var >= 0 && m_var_lock >= 0 && m_game_enable >= 0 && m_pnum >= 0)
            {
                bool check = true;
                for (size_t i = 0; i < m_lp.size(); ++i)
                {
                    if (m_lp[i] < 0)
                    {
                        check = false;
                        break;
                    }
                }

                if (check)
                {
                    for (size_t i = 0; i < m_camt.size(); ++i)
                    {
                        if (m_camt[i] < 0)
                        {
                            check = false;
                            break;
                        }
                    }

                    res = check;
                }
            }
        }
        else
        {
            CORE_START_PRINT_BLOCK();
            CORE_PRINT_BLOCK("\nUsage: gameconfig [options]\n");
            CORE_PRINT_BLOCK(vis_desc);
            CORE_PRINT_BLOCK("\n");
            CORE_END_PRINT_BLOCK();
        }

        if (err)
        {
            *err = &m_err;
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

}




