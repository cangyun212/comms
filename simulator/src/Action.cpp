
#include "Core.hpp"

#include <algorithm>

#include "boost/program_options.hpp"

#include "Utils.hpp"
#include "Comms.hpp"
#include "Action.hpp"

namespace po = boost::program_options;
namespace nm = boost::numeric;

namespace sg 
{

    bool Action::Parse(const ActionArgs &args)
    {
        SG_UNREF_PARAM(args);

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

    bool ListEGMAction::Parse(const ActionArgs &args)
    {
        bool res = false;

        BOOST_ASSERT(args.size());

        std::vector<std::string> argv(args.size());
        argv.assign(args.begin() + 1, args.end());

        po::variables_map vm;
        po::options_description desc;
        po::options_description vis_desc;
        desc.add_options()
            ("all,a", "")
            ("help", "");

        vis_desc.add_options()
                ("all,a", "list all information of an EGM")
                ("help", "help message");

        try
        {
            po::store(po::command_line_parser(argv).options(desc).run(), vm);
            po::notify(vm);
        }
        catch(po::error const& error)
        {
            COMMS_LOG(boost::format("%1%\n") % error.what(), CLL_Error);
            return false;
        }

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: list [options]\n");
            COMMS_PRINT_BLOCK(vis_desc);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();

            res = false;
        }
        else
        {
            if (vm.count("all"))
            {
                m_list_all = true;
            }

            res = true;
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

    bool PickEGMAction::Parse(const ActionArgs &args)
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
            ("egm", po::value<uint8>(&m_egm)->default_value(0), "");
        pos_desc.add("egm", 1);
        vis_desc.add_options()
                ("help,h", "help message");

        try
        {
            po::store(po::command_line_parser(argv).options(desc).positional(pos_desc).run(), vm);
            po::notify(vm);
        }
        catch (po::error const& error)
        {
            COMMS_LOG(boost::format("%1%\n") % error.what(), CLL_Error);
            return false;
        }
        catch (nm::bad_numeric_cast const& )
        {
            COMMS_LOG("Option value is out of range\n", CLL_Error);
            return false;
        }
        catch (boost::bad_lexical_cast const& )
        {
            COMMS_LOG("Invalid option value\n", CLL_Error);
            return false;
        }


        if (vm.count("help") || !vm.count("egm"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: pick <egm>\n");
            COMMS_PRINT_BLOCK(vis_desc);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();

            res = false;

        }
        else
        {
            if (m_egm > 0)
            {
                res = true;
            }
            else
            {
                COMMS_LOG("Invalid egm value '0', egm value must larger than 0", CLL_Error);
                res = false;
            }
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

    bool ResetDevAction::Parse(const ActionArgs &args)
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
            ("dev", po::value<std::string>(&m_dev), "");
        pos_desc.add("dev", 1);
        vis_desc.add_options()
            ("help,h", "help message");

        try
        {
            po::store(po::command_line_parser(argv).options(desc).positional(pos_desc).run(), vm);
            po::notify(vm);
        }
        catch (po::error const& error)
        {
            COMMS_LOG(boost::format("%1%\n") % error.what(), CLL_Error);
            return false;
        }
        catch (nm::bad_numeric_cast const& )
        {
            COMMS_LOG("Option value is out of range\n", CLL_Error);
            return false;
        }
        catch (boost::bad_lexical_cast const& )
        {
            COMMS_LOG("Invalid option value\n", CLL_Error);
            return false;
        }

        if (vm.count("help") || !vm.count("dev"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: resetdev/dev <dev_path>\n");
            COMMS_PRINT(vis_desc);
            COMMS_PRINT("\n");
            COMMS_END_PRINT_BLOCK();

            res = false;

        }
        else
        {
            res = true;
        }


        return res;
    }

    const char* ResetDevAction::Description() const
    {
        static const char* des = "\tReset the dev path :\n\t\tdev,resetdev\n";
        return des;
    }
}


