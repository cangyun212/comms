
#include "core/core.hpp"

#include "simulator/simulator.hpp"

namespace sg {

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

    Simulator::Connection& Simulator::GetConnection(int32_t type)
    {
        BOOST_ASSERT(this->HasConnection(type));
        return m_slots[type];
    }

    void Simulator::Install(int32_t type)
    {
        BOOST_ASSERT(m_slots.insert(std::make_pair(type, Connection())).second);
    }

    bool Simulator::HasConnection(int32_t type) const
    {
        return m_slots.find(type) != m_slots.end();
    }

    void Simulator::ResetConnections()
    {
        CORE_FOREACH(SlotsType::reference slot, m_slots)
        {
            slot.second.disconnect();
        }
    }

}

