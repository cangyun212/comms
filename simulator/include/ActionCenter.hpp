#ifndef __SG_ACTION_CENTER_HPP__
#define __SG_ACTION_CENTER_HPP__

#include "Core.hpp"

#include <string>
#include <map>

#include "boost/signals2.hpp"

#include "Singleton.hpp"
#include "Predeclare.hpp"

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
        typedef std::shared_ptr<ActionEvent> ActionEventPtr;

        void            Install(uint type);
        bool            HasEvent(uint type) const;
        ActionEventPtr  GetEvent(uint type);

    private:
        typedef std::map<uint, ActionEventPtr>      SignalType;
        SignalType      m_signals;
    };


}

#endif
