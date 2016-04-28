
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

    void ActionCenter::Install(uint type)
    {
        BOOST_ASSERT(m_signals.insert(std::make_pair(type, MakeSharedPtr<ActionEvent>())).second);
    }

    bool ActionCenter::HasEvent(uint type) const
    {
        return m_signals.find(type) != m_signals.end();
    }

    ActionCenter::ActionEventPtr ActionCenter::GetEvent(uint type)
    {
        auto it = m_signals.find(type);
        BOOST_ASSERT(it != m_signals.end());
        return it->second;
    }
}

