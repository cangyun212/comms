#ifndef __SG_CORE_CONSOLE_PROGRESS_BAR_HPP__
#define __SG_CORE_CONSOLE_PROGRESS_BAR_HPP__

#include "Core.hpp"

#include <functional>
#include <string>
#include <atomic>
#include <thread>

#include "Utils.hpp"

#define SG_CONSOLE_PB_MIN_WIDTH     7

namespace sg
{
    enum ConsoleProgressBarType
    {
        CPBT_Move = 0,
        CPBT_Hold
    };

    class ConsoleProgressBar
    {
    public:
        typedef std::vector<char>   SpinCharList;
        typedef std::shared_ptr<SpinCharList>   SpinCharListPtr;
        typedef std::function<void(std::string const&)> Printer;

    public:
        ConsoleProgressBar(
            uint width,
            Printer const& printer,
            ConsoleProgressBarType type = CPBT_Move,
            uint fps = 4,
            uint maximum = 100,
            uint minimum = 0,
            char finish = '=',
            char unfinish = '-',
            SpinCharListPtr const& spin_chars = nullptr)
            : m_progress(minimum)
            , m_printer(printer)
            , m_width(width)
            , m_type(type)
            , m_max(maximum)
            , m_min(minimum)
            , m_fps(fps)
            , m_finish(finish)
            , m_unfinish(unfinish)
            , m_spins(spin_chars)
            , m_spin(nullptr)
            , m_next(0)
        {
            SG_ASSERT(width >= SG_CONSOLE_PB_MIN_WIDTH && minimum < maximum && fps > 0);
        }

        ~ConsoleProgressBar();

    public:
        void    Start();
        void    Stop();
        void    Update(uint p) { SG_ASSERT(p >= m_min && p <= m_max);  m_progress = p; }
        void    Render();

    private:
        std::string    GenStr() const;

    private:
        std::atomic_uint        m_progress;
        Printer                 m_printer;
        uint                    m_width;
        ConsoleProgressBarType  m_type;
        const uint              m_max;
        const uint              m_min;
        const uint              m_fps;
        const char              m_finish;
        const char              m_unfinish;
        SpinCharListPtr         m_spins;
        SpinCharList           *m_spin;
        uint                    m_next;
        std::thread             m_worker;
    };
}
#endif //__SG_CORE_PROGRESS_BAR_HPP__
