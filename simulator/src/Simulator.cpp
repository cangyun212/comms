
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

    Simulator::Connection* Simulator::GetConnection(uint type)
    {
        auto it = m_slots.find(type);
        if (it != m_slots.end())
            return &(it->second);
        else
            return nullptr;
    }

    Simulator::Connection* Simulator::Install(uint type)
    {
        auto it = m_slots.insert(std::make_pair(type, Connection()));
        if (it.second)
            return &(it.first->second);
        else
            return nullptr;
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

