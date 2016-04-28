#ifndef __SG_ACTION_FACTORY_HPP__
#define __SG_ACTION_FACTORY_HPP__

#include "Core.hpp"

#include <string>
#include <unordered_map>
#include <vector>

#include "Predeclare.hpp"

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
        typedef std::unordered_map<std::string, ActionPtr>   ActionMap;
        ActionMap   m_actions;
    };

}



#endif
