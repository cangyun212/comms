#ifndef __SG_SIMULATOR_HPP__
#define __SG_SIMULATOR_HPP__

#include "Core.hpp"

#include <string>
#include <map>

#include "boost/signals2.hpp"

#include "Predeclare.hpp"


namespace sg 
{

    class Simulator
    {
    public:
        Simulator();
        virtual ~Simulator();

    public:
        void    Init();
        bool    IsReady();

        typedef boost::signals2::connection Connection;
        Connection&     GetConnection(uint type);
        bool            HasConnection(uint type) const;
        void            ResetConnections();
        void            Install(uint type);
       
    public:
        virtual void    DoInit() { }
        virtual bool    DoReady() { return true; }

    protected:

        int     m_fd;

        typedef std::map<uint, Connection>  SlotsType;
        SlotsType   m_slots;

    };

}


#endif
