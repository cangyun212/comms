#include "Core.hpp"

#include <chrono>

#include "Timer.hpp"


namespace sg {

    Timer::Timer()
    {
        this->ReStart();
    }

    void Timer::ReStart()
    {
        m_start_time = this->CurrentTime();
    }

    double Timer::Elapsed() const
    {
        return this->CurrentTime() - m_start_time;
    }

    double Timer::CurrentTime() const
    {
        std::chrono::steady_clock::time_point tp = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::duration<double, std::milli> >(tp.time_since_epoch()).count();
    }

}

