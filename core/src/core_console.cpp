#include "core/core.hpp"
#include "core/console/core_console_stream.hpp"
#include "core/core_console.hpp"

#ifdef CORE_PLATFORM_LINUX
#include <ncurses.h>
#endif

#include <iostream>


namespace sg
{
    void Console::Init(bool custom_win)
    {
        if (custom_win)
        {
            m_custom_wnd = true;

            initscr();
            cbreak();
            noecho();
            keypad(stdscr, TRUE);

            if (ConsoleStream::sIsColorSupport())
            {
                start_color();
            }

            SetActiveOutputWnd(MakeWindow(std::string("std"), stdscr));
        }
        else
        {
            // call it explicitly
            ConsoleStream::sIsColorSupport();
            // ref http://stackoverflow.com/questions/14010147/corrupted-output-with-c-cin-cout-threads-and-sync-with-stdio
            // cannot mix using c++ output and c output
            std::ios_base::sync_with_stdio(false);

            // call flush for each output if the buf is ready
            std::cin.tie(nullptr);
            std::cerr.tie(nullptr);
            // do not use std::endl anymore, it will call flush
        }
    }

    Console::Console()
        : m_active_output_wnd(nullptr)
        , m_custom_wnd(false)
    {

    }

    Console::~Console()
    {
        if (m_custom_wnd)
        {
            m_active_output_wnd = nullptr;

            CORE_FOREACH(WndMap::const_reference it, m_wnds)
            {
                delete it.second;
            }

            m_wnds.clear();

            endwin();

            m_custom_wnd = false;
        }

    }

    void Console::DealWindow(const std::string &name, ConsoleWindow *p)
    {
        WndMap::iterator it = m_wnds.find(name);
        if (it != m_wnds.end())
        {
            if (m_active_output_wnd == it->second)
            {
                p->ActiveOutput(true);
                m_active_output_wnd = p;
            }

            delete it->second;
            it->second = p;
        }
        else
        {
            m_wnds[name] = p;
        }
    }

    ConsoleWindow* Console::MakeWindow(const std::string &name, uint32_t width, uint32_t height, int32_t x, int32_t y)
    {
        ConsoleWindow *p = nullptr;

        if (m_custom_wnd)
        {
            p = new ConsoleWindow(name, width, height, x, y);
            DealWindow(name, p);
        }

        return p;
    }

    ConsoleWindow* Console::MakeWindow(const std::string &name, void *extern_handle)
    {
        ConsoleWindow *p = nullptr;

        if (m_custom_wnd)
        {
            p = new ConsoleWindow(name, extern_handle);
            DealWindow(name, p);
        }

        return p;
    }

    ConsoleWindow* Console::FindWindow(const std::string &name)
    {
        WndMap::iterator it = m_wnds.find(name);
        if (it != m_wnds.end())
        {
            return it->second;
        }

        return nullptr;
    }

    void Console::SetActiveOutputWnd(ConsoleWindow *wnd)
    {
        BOOST_ASSERT(wnd);
        wnd->ActiveOutput(true);

        if (m_active_output_wnd)
        {
            if (m_active_output_wnd != wnd)
            {
                m_active_output_wnd->ActiveOutput(false);
            }
        }

        m_active_output_wnd = wnd;
    }

}




