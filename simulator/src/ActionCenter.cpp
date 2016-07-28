
#include "Core.hpp"

#include <utility>

#include "Utils.hpp"
#include "Action.hpp"
#include "ActionCenter.hpp"

namespace sg {

    ActionCenter::ActionCenter()
    {

    }

    ActionCenter::~ActionCenter()
    {

    }

    void ActionCenter::SendAction(const ActionPtr &action)
    {
        auto it = m_signals.find(action->GetType());
        if (it != m_signals.end())
        {
            (*(it->second))(*this, action);
        }
    }

    ActionCenter::ActionEventPtr ActionCenter::Install(uint type)
    {
        auto it = m_signals.insert(std::make_pair(type, MakeSharedPtr<ActionEvent>()));
        if (it.second)
            return it.first->second;
        else
            return nullptr;
    }

    bool ActionCenter::HasEvent(uint type) const
    {
        return m_signals.find(type) != m_signals.end();
    }

    ActionCenter::ActionEventPtr ActionCenter::GetEvent(uint type) const
    {
        auto it = m_signals.find(type);
        if (it != m_signals.end())
            return it->second;
        else
            return nullptr;
    }
}

