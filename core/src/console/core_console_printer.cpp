#include "core/core.hpp"

#include "core/core_console.hpp"
#include "core/console/core_console_printer.hpp"

namespace sg{

    namespace
    {
        ConsoleStream   s_stream;
    }


    LockableStream<ConsoleStream> sg_out;


    void ConsolePrinter::Init(LogLevel level,
#ifdef CORE_DEBUG
              bool detail,
              bool enable_debug,
#endif
              std::string const & prompt)
    {
        m_level = level;
#ifdef CORE_DEBUG
        m_show_detail = detail;
        m_enable_debug = enable_debug;
        m_ready = true;
#endif
        m_log_prompt = prompt;

        if (Console::Instance().IsCustomWin())
        {
            ConsoleWindow *win = Console::Instance().GetActiveOutputWnd();
            BOOST_ASSERT(win);

            s_stream = ConsoleStream(win);
        }

        sg_out = LockableStream<ConsoleStream>(&s_stream);

        if (ConsoleStream::sIsColorSupport())
        {
            ConsoleStreamColorGroup g;
            g.b = CSC_Black;
            g.f = CSC_Green;

            m_info_color = ConsoleStream::sRegisterColorGroup(g);

            g.f = CSC_Yellow;

            m_warning_color = ConsoleStream::sRegisterColorGroup(g);

            g.f = CSC_Red;

            m_error_color = ConsoleStream::sRegisterColorGroup(g);
        }
    }

}
