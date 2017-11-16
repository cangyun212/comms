
#include "Core.hpp"

#include <algorithm>


#include "Utils.hpp"
#include "Comms.hpp"
#include "Action.hpp"
#include "ActionOptions.hpp"

namespace po = boost::program_options;
namespace nm = boost::numeric;

namespace sg 
{

    bool Action::Parse(const ActionArgs &args)
    {
        SG_UNREF_PARAM(args);

        return true;
    }

    bool Action::TryParse(ActionArgs const & args, ActionParsedOptions & options, bool ignore_pos)
    {
        if (args.size() && m_options)
        {
            std::vector<std::string> argv(args.begin() + 1, args.end());
            
            po::options_description desc;
            po::positional_options_description pos_desc;

            m_options->Traverse(
                [&](ActionOption const& option)
            {
                if (option.max > 0 && ignore_pos)
                    return;

                if (option.value)
                    desc.add_options()(option.name.c_str(), po::value<std::string>(), "");
                else
                    desc.add_options()(option.name.c_str(), "");

                if (option.max > 0)
                    pos_desc.add(option.name.c_str(), option.max);
            });

            try
            {
                std::vector<po::option> result;
                if (ignore_pos)
                    result = po::command_line_parser(argv).options(desc).run().options;
                else
                    result = po::command_line_parser(argv).options(desc).positional(pos_desc).run().options;

                for (auto const& o : result)
                {
                    options.push_back({ o.string_key, o.original_tokens.front() });
                }

                return true;
            }
            catch (po::ambiguous_option const& error)
            {
                for (auto const& a : error.alternatives())
                {
                    options.push_back({ a, a });
                }
            }
            catch (po::invalid_command_line_syntax const& error)
            {
                std::string name = error.get_option_name();
                options.push_back({ name, name });
            }
            catch (po::error const&)
            {

            }
        }

        return false;
    }

    void Action::GetAllOptionsName(std::vector<std::string>& names, bool long_name) const
    {
        if (m_options)
        {
            m_options->Traverse(
            [&](ActionOption const& option) 
            {
                std::string::size_type pos = option.name.find(',');
                if (pos != std::string::npos)
                {
                    if (long_name)
                        names.push_back(std::string(option.name, 0, pos));
                    else if (pos + 1 < option.name.size())
                        names.push_back(std::string(option.name, pos + 1, option.name.size() - pos - 1));
                }
                else
                {
                    names.push_back(option.name);
                }
            });
        }
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
   
    bool ListEGMAction::s_list_all = false;

    ListEGMAction::ListEGMAction()
        : Action(Action::AT_LIST_EGM)
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

        SG_PARSE_OPTION(args, m_options);

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
                s_list_all = true;
            }
            else
            {
                s_list_all = false;
            }

            res = true;
        }

        return res;
    }

    void ListEGMAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();

            m_options->AddOption(ActionOption("all,a", "list all information of an EGM"));
            m_options->AddOption(ActionOption("help,h", "help message"));
        }
    }

    const char* ListEGMAction::Description() const
    {
        static const char* des = "\tList all EGM :\n\t\tl,ls,list\n";
        return des;
    }

    uint8 PickEGMAction::s_egm = 0;

    PickEGMAction::PickEGMAction()
        : Action(Action::AT_PICK_EGM)
    {
    }

    PickEGMAction::~PickEGMAction()
    {

    }

    void PickEGMAction::ResetArgOptions()
    {
        s_egm = 0;
    }

    ActionPtr PickEGMAction::Clone()
    {
        return DoClone<PickEGMAction>();
    }

    void PickEGMAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();

            m_options->AddOption(ActionOption("egm", "", Value<uint8>(&s_egm), false, 1));
            m_options->AddOption(ActionOption("help,h", "help message"));
        }
    }

    bool PickEGMAction::Parse(const ActionArgs &args)
    {
        bool res = false;

        this->ResetArgOptions();

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
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
            if (s_egm > 0)
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

    std::string ResetDevAction::s_dev;

    ResetDevAction::ResetDevAction() 
        : Action(Action::AT_RESET_DEV)
    {
    }

    ResetDevAction::~ResetDevAction() 
    {
    }

    void ResetDevAction::ResetArgOptions()
    {
        s_dev.clear();
    }

    ActionPtr ResetDevAction::Clone()
    {
        return DoClone<ResetDevAction>();
    }

    bool ResetDevAction::Parse(const ActionArgs &args)
    {
        bool res = false;

        this->ResetArgOptions();

        SG_PARSE_OPTION(args, m_options);

        if (vm.count("help"))
        {
            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\nUsage: dev <dev_path>\n");
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

    void ResetDevAction::BuildOptions()
    {
        if (!m_options)
        {
            m_options = MakeSharedPtr<ActionOptions>();

            m_options->AddOption(ActionOption("d", "", Value<std::string>(&s_dev), false, 1));
            m_options->AddOption(ActionOption("help,h", "help message"));
        }
    }

    const char* ResetDevAction::Description() const
    {
        static const char* des = "\tReset the dev path :\n\t\tdev\n";
        return des;
    }
}


