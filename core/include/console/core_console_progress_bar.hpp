#ifndef __SG_CORE_CONSOLE_PROGRESS_BAR_HPP__
#define __SG_CORE_CONSOLE_PROGRESS_BAR_HPP__

#include "core/core.hpp"
#include "core/core_thread.hpp"
#include "core/console/core_console_window.hpp"

#include <string>

namespace sg{

    class ConsoleProgressBar
    {
        public:
            ConsoleProgressBar(const std::string name, /* At least give me a name */
                               bool detail = true,      /* show the details like :( NAME 13% ) */
                               unsigned int check_point = 100,
                               unsigned int flush_rate = 250, /* how much millisec to flush */
                               ConsoleWindow *wnd = nullptr
                               ) 
                : m_detail(detail)
                , m_name(name)
                , m_value(0)
                , m_next_spin_char(0)
                , m_rate( flush_rate)
                , m_check_point(check_point)
                , m_wnd(wnd)
                { 
                    m_printer = thread(bind(&ConsoleProgressBar::ContinuePrint, this));
                }

            ~ConsoleProgressBar(){
                m_value = m_check_point;
                m_printer.join();
            }
            void Update(unsigned int i){
                m_value = i ;
            }
            /* if m_value >= 100 , the print thread will exit */ 
            void Kill(){
                    m_value = m_check_point;
            }
        public:
            std::string ToString();
            void ContinuePrint (); 
        private:
            const bool                  m_detail;
            const std::string           m_name;
            thread                      m_printer;
            atomic<unsigned int>        m_value;
            int                         m_next_spin_char;
            const unsigned int          m_rate;
            const unsigned int          m_check_point;
            ConsoleWindow              *m_wnd;
    };
}
#endif //__SG_CORE_PROGRESS_BAR_HPP__
