#ifndef __ACTION_CENTER_HPP__
#define __ACTION_CENTER_HPP__

#include "core/core.hpp"
#include "core/core_singleton.hpp"

#include <string>
#include <map>

#include "boost/signals2.hpp"

#include "simulator/predeclare.hpp"

namespace sg {


    class ActionCenter : public Singleton<ActionCenter>
    {
    public:
        ActionCenter();
       ~ActionCenter();

    public:
        void        SendAction(ActionPtr const& action);

    public:
        typedef boost::signals2::signal<void(const ActionCenter &sender, ActionPtr const& action)> ActionEvent;
        typedef shared_ptr<ActionEvent> ActionEventPtr;

        void            Install(int32_t type);
        bool            HasEvent(int32_t type) const;
        ActionEventPtr  GetEvent(int32_t type);

    private:
        typedef std::map<int32_t, ActionEventPtr>      SignalType;
        SignalType      m_signals;
    };


}

#endif
