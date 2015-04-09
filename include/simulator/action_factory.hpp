#ifndef __ACTION_FACTORY_HPP__
#define __ACTION_FACTORY_HPP__

#include "core/core.hpp"

#include <string>
#include <map>
#include <vector>

#include "simulator/predeclare.hpp"

namespace sg {

    class ActionFactory
    {
    public:
        ActionFactory();
        virtual ~ActionFactory();

    public:
        virtual bool        Parse(std::string & line, std::vector<std::string> & arg) const;
        virtual ActionPtr   CreateAction(std::string const& act);
        virtual void        Init();
        virtual bool        IsValidAction(std::string const& act) const;

        virtual void        Help(ActionCenter const& , const ActionPtr&);
    protected:


        bool    PreParse(std::string & line) const;

    protected:
        typedef unordered_map<std::string, ActionPtr>   ActionMap;
        ActionMap   m_actions;
    };

}



#endif
