#include "Core.hpp"

#include "boost/algorithm/string/classification.hpp"
#include "boost/algorithm/string/split.hpp"

#include <algorithm>

#include "Utils.hpp"
#include "Comms.hpp"
#include "Action.hpp"
#include "ActionFactory.hpp"
#include "LineReader.hpp"

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
            return true;
        }
        
        return false;
    }

    ActionPtr ActionFactory::CreateAction(const std::string &act)
    {
        auto it = m_actions.find(act);
        BOOST_ASSERT(it != m_actions.end());
        return it->second->Clone();
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

        COMMS_START_PRINT_BLOCK();
        COMMS_PRINT_BLOCK("\nList of all commands :\n");

        for (auto const& it : m_actions)
        {
            if (std::find(as.begin(), as.end(), it.second->GetType()) == as.end())
            {
                as.push_back(it.second->GetType());
                if (Action::AT_HELP != it.second->GetType()) // print the help description at last
                {
                    COMMS_PRINT_BLOCK(it.second->Description());
                }
                else
                {
                    help_action = it.second;
                }
            }
        }

        COMMS_PRINT_BLOCK(help_action->Description());
        COMMS_PRINT_BLOCK("\n");

        COMMS_END_PRINT_BLOCK();
    }
}


