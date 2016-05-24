
#include "Core.hpp"
#include "Utils.hpp"

#include "Simulator.hpp"

namespace sg 
{

    Simulator::Simulator()
    {

    }

    Simulator::~Simulator()
    {

    }

    void Simulator::Init()
    {
        this->DoInit();
    }

    bool Simulator::IsReady()
    {
        return this->DoReady();
    }

    Simulator::Connection& Simulator::GetConnection(uint type)
    {
        auto it = m_slots.find(type);
        BOOST_ASSERT(it != m_slots.end());
        return it->second;
    }

    void Simulator::Install(uint type)
    {
		SG_UNREF_PARAM(type);
        BOOST_ASSERT(m_slots.insert(std::make_pair(type, Connection())).second);
    }

    bool Simulator::HasConnection(uint type) const
    {
        return m_slots.find(type) != m_slots.end();
    }

    void Simulator::ResetConnections()
    {
        for(auto & slot : m_slots)
        {
            slot.second.disconnect();
        }
    }

}

