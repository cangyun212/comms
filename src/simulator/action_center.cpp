
#include "core/core.hpp"
#include "core/core_utils.hpp"

#include <utility>

#include "simulator/action.hpp"
#include "simulator/action_center.hpp"

namespace sg {

    ActionCenter::ActionCenter()
    {

    }

    ActionCenter::~ActionCenter()
    {

    }

    void ActionCenter::SendAction(const ActionPtr &action)
    {
        if (HasEvent(action->GetType()))
        {
            (*(this->GetEvent(action->GetType())))(*this, action);
        }
    }

    void ActionCenter::Install(int32_t type)
    {
        BOOST_ASSERT(m_signals.insert(std::make_pair(type, MakeSharedPtr<ActionEvent>())).second);
    }

    bool ActionCenter::HasEvent(int32_t type) const
    {
        return m_signals.find(type) != m_signals.end();
    }

    ActionCenter::ActionEventPtr ActionCenter::GetEvent(int32_t type)
    {
        BOOST_ASSERT(this->HasEvent(type));
        return m_signals[type];
    }
}

