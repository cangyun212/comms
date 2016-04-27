#ifndef __CORE_TIMER_HPP__
#define __CORE_TIMER_HPP__

#include "Core.hpp"

namespace sg {

    class CORE_API Timer
    {
    public:
        Timer();

        void ReStart();

        // return elapsed time in milliseconds
        double Elapsed() const;

        double CurrentTime() const;

    private:
        double m_start_time;
    };

}

#endif

