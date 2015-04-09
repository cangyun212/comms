#include "core/core.hpp"
#include "core/console/core_console_printer.hpp"
#include "core/console/core_console_progress_bar.hpp"
#include "core/console/core_console_window.hpp"

#ifdef CORE_PLATFORM_LINUX
#include <sys/ioctl.h>
#endif

namespace sg
{
    const static char s_spin_mask[] = { '-' , '\\' , '|' , '/'};
    std::string ConsoleProgressBar::ToString()
    {
        unsigned int curr = m_value;

        uint32_t window_w = 0, window_h = 0;
        if (m_wnd)
        {
            window_w = m_wnd->Width();
            window_h = m_wnd->Height();
        }
        else
        {
#ifdef CORE_PLATFORM_LINUX
            struct winsize ws;
            ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
            window_w = ws.ws_col;
            window_h = ws.ws_row;
#endif
        }

        int used_w = window_w;
        std::string details("");
        if( m_detail  && used_w > 100)
        {
            details =  std::string(" ( ")
                +  m_name + std::string( " ")
                +  std::to_string(curr)
                +  std::string("% )");
            if( used_w - details.size() -3 >= 100  ) 
                 used_w -= details.size() ; 
            else 
                details.clear();

        }
        used_w -= 3; // [-] [\] [|] [/]
        std::string line("") ; 
        if( used_w > 0) 
        {
            float scale_factor = 1.0f;
            scale_factor = float(used_w) / float(100) ;

            if( int(curr* scale_factor)  != 0  )
                line += std::string(curr * scale_factor ,'>');
            if( int((100-curr) * scale_factor) != 0 )
                line += std::string(int((100-curr) * scale_factor),'.');
        }
        line += std::string("[")+std::string(1,s_spin_mask[m_next_spin_char]) +std::string("]");
        if( window_w - line.size() - details.size() != 0)
            line += std::string(window_w - line.size() - details.size() , ' ');
        line += details;
        line += std::string("\r"); //back to orignial position.
        return line;
    }

    void ConsoleProgressBar::ContinuePrint()
    {
        while( m_value <= m_check_point )
        {
            CORE_PRINT(ToString());
            m_next_spin_char = (m_next_spin_char +1 ) %4;
            if( m_value >= m_check_point )
            {
                CORE_PRINT('\n');
                break;
            }
            this_thread::sleep_for(chrono::milliseconds(m_rate)); 
        }
        return;
    }
}
