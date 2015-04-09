#include "core/core.hpp"

#include "core/core_timer.hpp"


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
        chrono::steady_clock::time_point tp = chrono::steady_clock::now();
        return chrono::duration_cast<chrono::duration<double, milli> >(tp.time_since_epoch()).count();
    }

}

