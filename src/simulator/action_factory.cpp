#include "core/core.hpp"
#include "core/core_types.hpp"
#include "core/console/core_console_printer.hpp"

#include "boost/algorithm/string/classification.hpp"
#include "boost/algorithm/string/split.hpp"

#include <algorithm>

#include "simulator/action.hpp"
#include "simulator/action_factory.hpp"
#include "simulator/line_reader.hpp"

namespace sg {

    ActionFactory::ActionFactory()
    {

    }

    ActionFactory::~ActionFactory()
    {

    }

    bool ActionFactory::PreParse(std::string &line) const
    {
        if (line.empty())
        {
            line = LineReader::Instance().ReadPreLine();
            if (line.empty())
            {
                return false;
            }
            return true;
        }

        return true;
    }

    bool ActionFactory::IsValidAction(const std::string &act) const
    {
        return m_actions.find(act) != m_actions.end();
    }

    bool ActionFactory::Parse(std::string &line, std::vector<std::string> &arg) const
    {
        if (this->PreParse(line))
        {
            boost::algorithm::split(arg, line, boost::is_any_of(" "));
        }

        return true;
    }

    ActionPtr ActionFactory::CreateAction(const std::string &act)
    {
        BOOST_ASSERT(this->IsValidAction(act));
        return m_actions[act]->Clone();
    }

    void ActionFactory::Init()
    {
        ActionPtr ptr = MakeSharedPtr<QuitAction>();
        m_actions["quit"] = ptr;
        m_actions["q"] = ptr;

        ptr = MakeSharedPtr<ListEGMAction>();
        m_actions["list"] = ptr;
        m_actions["l"] = ptr;
        m_actions["ls"] = ptr;

        ptr = MakeSharedPtr<PickEGMAction>();
        m_actions["pick"] = ptr;
        m_actions["pk"] = ptr;

        ptr = MakeSharedPtr<HelpAction>();
        m_actions["help"] = ptr;
        m_actions["h"]    = ptr;

        ptr = MakeSharedPtr<ResetDevAction>();
        m_actions["resetdev"] = ptr;
        m_actions["dev"] = ptr;
    }

    void ActionFactory::Help(const ActionCenter &, const ActionPtr&)
    {
        std::vector<Action::ActionType> as(m_actions.size());

        ActionPtr help_action = nullptr;

        CORE_START_PRINT_BLOCK();
        CORE_PRINT_BLOCK("\nList of all commands :\n");

        CORE_FOREACH(ActionMap::const_reference it, m_actions)
        {
            if (std::find(as.begin(), as.end(), it.second->GetType()) == as.end())
            {
                as.push_back(it.second->GetType());
                if (Action::AT_HELP != it.second->GetType()) // print the help description at last
                {
                    CORE_PRINT_BLOCK(it.second->Description());
                }
                else
                {
                    help_action = it.second;
                }
            }
        }

        CORE_PRINT_BLOCK(help_action->Description());
        CORE_PRINT_BLOCK("\n");

        CORE_END_PRINT_BLOCK();
    }
}


