
#include "Core.hpp"

#include <algorithm>


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

    void Action::AddOption(const std::string &option, ValueSematicPtr const &value, const std::string &message)
    {
        BOOST_ASSERT(m_options != nullptr);
        m_options->push_back(MakeSharedPtr<ActionOption>(option, value, message));
    }

    void Action::AddPosOption(const ActionPosOptionPtr &option)
    {
        BOOST_ASSERT(m_pos_options != nullptr);
        m_pos_options->push_back(option);
    }

    void Action::FillOptionsDescription(po::options_description &desc, po::options_description &vis_desc, po::positional_options_description &pos_desc)
    {
        std::vector<std::string>::size_type i;
        for(i = 0; i < m_options->size(); ++i)
        {
            std::string &option_desc = (*m_options)[i]->name;
            std::string &option_vis = (*m_options)[i]->message;
            ValueSematicPtr value = (*m_options)[i]->value;
            if(value != nullptr)
            {
                desc.add_options()
                        (option_desc.c_str(), value->value(), "");
            }
            else
            {
                desc.add_options()
                        (option_desc.c_str(), "");
            }

            if(!option_vis.empty())
            {
                vis_desc.add_options()
                            (option_desc.c_str(), option_vis.c_str());
            }
        }

        desc.add_options()
                ("help,h", "");
        vis_desc.add_options()
                ("help,h", "help message");

        for(i = 0; i < m_pos_options->size(); ++i)
        {
            ActionPosOptionPtr &pos_option = (*m_pos_options)[i];
            desc.add_options()
                        (pos_option->option.c_str(), pos_option->value->value(), "");
            pos_desc.add(pos_option->option.c_str(), pos_option->max_count);
        }
	} 

    void Action::FillOptionsDescription(po::options_description &desc, po::options_description &vis_desc)
    {
        std::vector<std::string>::size_type i;
        for(i = 0; i < m_options->size(); ++i)
        {
            std::string &option_desc = (*m_options)[i]->name;
            std::string &option_vis = (*m_options)[i]->message;
            ValueSematicPtr value = (*m_options)[i]->value;
            if(value != nullptr)
            {
                desc.add_options()
                        (option_desc.c_str(), value->value(), "");
            }
            else
            {
                desc.add_options()
                        (option_desc.c_str(), "");
            }

            if(!option_vis.empty())
            {
                vis_desc.add_options()
                            (option_desc.c_str(), option_vis.c_str());
            }
        }

        desc.add_options()
                ("help,h", "");
        vis_desc.add_options()
                ("help,h", "help message");
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
   
	bool ListEGMAction::m_list_all = false;

    ListEGMAction::ListEGMAction()
        : Action(Action::AT_LIST_EGM)
    {
		m_options = MakeSharedPtr<ActionOptions>();

		this->AddOption("all,a", nullptr, "list all information of an EGM");
		this->AddOption("help", nullptr, "help message");
			
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

		this->FillOptionsDescription(desc, vis_desc);

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

	uint8 PickEGMAction::m_egm = 0;

    PickEGMAction::PickEGMAction()
        : Action(Action::AT_PICK_EGM)
    {
		m_options = MakeSharedPtr<ActionOptions>();

		this->AddOption("help", nullptr, "help message");

        m_pos_options = MakeSharedPtr<VectorPosOptionPtr>();
        this->AddPosOption(MakeSharedPtr<ActionPosOption>("egm", Value<uint8>(&m_egm), 1));
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

		this->FillOptionsDescription(desc, vis_desc, pos_desc);

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

	std::string ResetDevAction::m_dev;

    ResetDevAction::ResetDevAction() : Action(Action::AT_RESET_DEV)
	{
		m_options = MakeSharedPtr<ActionOptions>();

		this->AddOption("help", nullptr, "help message");

        m_pos_options = MakeSharedPtr<VectorPosOptionPtr>();
        this->AddPosOption(MakeSharedPtr<ActionPosOption>("dev", Value<std::string>(&m_dev), 1));
	}

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

		this->FillOptionsDescription(desc, vis_desc, pos_desc);

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


