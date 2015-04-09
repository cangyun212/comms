#ifndef __SIMULATOR_HPP__
#define __SIMULATOR_HPP__

#include "core/core.hpp"

#include <string>
#include <map>

#include "boost/signals2.hpp"

#include "simulator/predeclare.hpp"


namespace sg {

    class Simulator
    {
    public:
        Simulator();
        virtual ~Simulator();

    public:
        void    Init();
        bool    IsReady();

        typedef boost::signals2::connection Connection;
        Connection&     GetConnection(int32_t type);
        bool            HasConnection(int32_t type) const;
        void            ResetConnections();
        void            Install(int32_t type);
       
    public:
        virtual void    DoInit() { }
        virtual bool    DoReady() { return true; }

    protected:

        int     m_fd;

        typedef std::map<int32_t, Connection>  SlotsType;
        SlotsType   m_slots;

    };

}


#endif
