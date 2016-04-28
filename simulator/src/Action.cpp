
#include "Core.hpp"

#include <algorithm>

#include "boost/program_options.hpp"

#include "Utils.hpp"
#include "Comms.hpp"
#include "Action.hpp"

namespace po = boost::program_options;

namespace sg 
{

    std::string ActionError::GetErrStr() const
    {
        switch (code) {
        case ET_NO_ERROR:
            return "";
        default:
            return "Unknown error";
        }
    }

    bool Action::Parse(const ActionArgs &args, const ActionError **err)
    {
        SG_UNREF_PARAM(args);

        m_err.code = ActionError::ET_NO_ERROR;

        if (err)
        {
            *err = &m_err;
        }

        return true;
    }

    QuitAction::QuitAction()
        : Action(Action::AT_QUIT)
    {

    }

    QuitAction::~QuitAction()
    {

    }

    ActionPtr QuitAction::Clone()
    {
        return DoClone<QuitAction>();
    }

    const char* QuitAction::Description() const
    {
        static const char* des = "\tQuit this simulator :\n\t\tq,quit\n";
        return des;
    }
   
    ListEGMAction::ListEGMAction()
        : Action(Action::AT_LIST_EGM)
        , m_list_all(false)
    {

    }

    ListEGMAction::~ListEGMAction()
    {

    }

    ActionPtr ListEGMAction::Clone()
    {
        return DoClone<ListEGMAction>();
    }

    bool ListEGMAction::Parse(const ActionArgs &args, const ActionError **err)
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
                ("all,a", "")
                ("help", "")
                ("dummy", po::value< std::vector<std::string> >(), "");
        pos_desc.add("dummy", -1);

        vis_desc.add_options()
                ("all,a", "list all information of an EGM")
                ("help", "help message");

        // TODO : handle exception later

        //try
        //{
            po::store(
                po::command_line_parser(argv).options(desc).positional(pos_desc).allow_unregistered().run(),
                vm);
        //}
        //catch(po::error &e)
        //{

        //}

        if (!vm.count("help"))
        {
            if (vm.count("all"))
            {
                m_list_all = true;
            }

            res = true;
        }
        else
        {
            COMMS_START_LOG_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: list [options]\n");
            COMMS_PRINT_BLOCK(vis_desc);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();

            res = false;
        }

        if (err)
        {
            *err = &m_err;
        }

        return res;
    }

    const char* ListEGMAction::Description() const
    {
        static const char* des = "\tList all EGM :\n\t\tl,ls,list\n";
        return des;
    }

    PickEGMAction::PickEGMAction()
        : Action(Action::AT_PICK_EGM)
        , m_egm(0)
    {

    }

    PickEGMAction::~PickEGMAction()
    {

    }

    ActionPtr PickEGMAction::Clone()
    {
        return DoClone<PickEGMAction>();
    }

    bool PickEGMAction::Parse(const ActionArgs &args, const ActionError **err)
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
                ("help,h", "")
                ("egm", po::value<uint8_t>(&m_egm)->default_value(0), "")
                ("dummy", po::value< std::vector<std::string> >(), "");
        pos_desc.add("egm", 1).add("dummy", -1);
        vis_desc.add_options()
                ("help,h", "help message");

        po::store(po::command_line_parser(argv).options(desc).positional(pos_desc).allow_unregistered().run(), vm);

        if (!vm.count("help"))
        {
            if (vm.count("egm"))
            {
                if (m_egm > 0)
                {
                    res = true;
                }
                // invalid argument
            }

            // too few argument

            // unregistered and too much arguments
        }
        else
        {
            COMMS_START_LOG_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: pick <egm>\n");
            COMMS_PRINT_BLOCK(vis_desc);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();
        }

        if (err)
        {
            *err = &m_err;
        }

        return res;
    }

    const char* PickEGMAction::Description() const
    {
        static const char* des = "\tPick an EGM :\n\t\tpk,pick\n";
        return des;
    }

    HelpAction::HelpAction() : Action(Action::AT_HELP) {}
    HelpAction::~HelpAction() {}

    ActionPtr HelpAction::Clone()
    {
        return DoClone<HelpAction>();
    }

    const char* HelpAction::Description() const
    {
        static const char* des = "\tPrint all command name :\n\t\th,help\n";
        return des;
    }

    ResetDevAction::ResetDevAction() : Action(Action::AT_RESET_DEV) , m_dev(""){}
    ResetDevAction::~ResetDevAction() {}

    ActionPtr ResetDevAction::Clone()
    {
        return DoClone<ResetDevAction>();
    }

    bool ResetDevAction::Parse(const ActionArgs &args, const ActionError **err)
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
            ("help,h", "Show usage")
            ("dev,d",boost::program_options::value<std::string>(), "set the dev path")
            ("dummy", po::value< std::vector<std::string> >(), "");
        pos_desc.add("dummy", -1);
        vis_desc.add_options()
            ("help,h", "help message");

        po::store(po::command_line_parser(argv).options(desc).positional(pos_desc).allow_unregistered().run(), vm);

        if (!vm.count("help"))
        {
            if (vm.count("dev"))
            {
                m_dev = vm["dev"].as<std::string>();
                res = true;
            }
        }
        else
        {
            COMMS_START_LOG_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: dev/d --dev/-d dev_path\n");
            COMMS_END_PRINT_BLOCK();
        }

        if (err)
        {
            *err = &m_err;
        }
        return res;
    }

    const char* ResetDevAction::Description() const
    {
        static const char* des = "\tReset the dev path :\n\t\tdev,resetdev\n";
        return des;
    }
}


