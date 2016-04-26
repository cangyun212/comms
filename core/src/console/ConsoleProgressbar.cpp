#include "Core.hpp"

#include "boost/format.hpp"

#include "Console/ConsoleProgressbar.hpp"

namespace
{
#ifdef SG_COMPILER_MSVC
    SG_PUSH_WARN
    SG_DISABLE_WARN(SG_WARN_DYNA_INITIALIZE)
#endif
    sg::ConsoleProgressBar::SpinCharList    s_spins({ '-', '\\', '|', '/' });
#ifdef SG_COMPILER_MSVC
    SG_POP_WARN
#endif


    sg::uint sCalcProgressWidth(sg::uint curr, sg::uint max, sg::uint width)
    {
        if (curr == 0)
            return 0;
        else if (curr == max)
            return width;

        return static_cast<sg::uint>(static_cast<float>(width * curr) / max);
    }
}

namespace sg
{
    ConsoleProgressBar::~ConsoleProgressBar()
    {
        this->Stop();
    }

    void ConsoleProgressBar::Start()
    {
        if (!m_spins)
            m_spin = &s_spins;
        else
            m_spin = m_spins.get();

        BOOST_ASSERT(m_spin->size());

        m_worker = std::thread(std::bind(&ConsoleProgressBar::Render, this));
    }

    void ConsoleProgressBar::Stop()
    {
        m_progress = m_max;

        if (m_worker.joinable())
            m_worker.join();
    }

    std::string ConsoleProgressBar::GenStr() const
    {
        uint curr = m_progress;

        boost::format fmt("%|1$c|[%|2$3|%%]");

        fmt % (*m_spin)[m_next] % curr;

        std::string out(fmt.str());
        if (m_width > out.size())
        {
            uint width = m_width - static_cast<uint>(out.size());
            uint n = sCalcProgressWidth(curr, m_max, width);
            if (m_type == CPBT_Move)
            {
                out = std::string(n, m_finish) + out + std::string(m_width - n - out.size(), ' ') + std::string("\r");
            }
            else
            {
                out = std::string(n, m_finish) + std::string(width - n, m_unfinish) + out + std::string("\r");
            }
        }

        return out;
    }

    void ConsoleProgressBar::Render()
    {
        uint curr = m_progress;

        while (curr <= m_max)
        {
            m_printer(this->GenStr());
            m_next = (m_next + 1) % (m_spin->size());

            if (curr >= m_max)
                break;

            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<uint>(1000.0f / m_fps)));
        }

        return;
    }
}
