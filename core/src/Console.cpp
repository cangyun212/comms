#include "Core.hpp"

#include <iostream>

#ifdef SG_PLATFORM_WINDOWS
#include <Windows.h>
#ifdef MOUSE_MOVED
#define _WIN32_MOUSE_MOVED  MOUSE_MOVED
#undef MOUSE_MOVED
#endif
#include "curses.h"
#elif defined(SG_PLATFORM_LINUX)
#include <ncurses.h>
#endif

#include "Utils.hpp"
#include "Console.hpp"
#include "Console/ConsoleWindow.hpp"
#include "Console/ConsolePrinter.hpp"

namespace
{
#ifdef SG_PLATFORM_LINUX

    std::string sd("\033[0m");

    void sEscapeColorCode(sg::ConsoleColorGroup const& group, std::string &code)
    {
        static std::string f_black("\033[30m");
        static std::string f_red("\033[31m");
        static std::string f_green("\033[32m");
        static std::string f_yellow("\033[33m");
        static std::string f_blue("\033[34m");
        static std::string f_magenta("\033[35m");
        static std::string f_cyan("\033[36m");
        static std::string f_white("\033[37m");

        static std::string b_black("\033[40m");
        static std::string b_red("\033[41m");
        static std::string b_green("\033[42m");
        static std::string b_yellow("\033[43m");
        static std::string b_blue("\033[44m");
        static std::string b_magenta("\033[45m");
        static std::string b_cyan("\033[46m");
        static std::string b_white("\033[47m");

        std::string *f, *b;
        switch (group.f)
        {
        case sg::CC_Black:
            f = &f_black;
            break;
        case sg::CC_Red:
            f = &f_red;
            break;
        case sg::CC_Green:
            f = &f_green;
            break;
        case sg::CC_Yellow:
            f = &f_yellow;
            break;
        case sg::CC_Blue:
            f = &f_blue;
            break;
        case sg::CC_Magenta:
            f = &f_magenta;
            break;
        case sg::CC_Cyan:
            f = &f_cyan;
            break;
        case sg::CC_White:
            f = &f_white;
            break;
        default:
            f = &sd;
            break;
        }

        switch (group.b)
        {
        case sg::CC_Black:
            b = &b_black;
            break;
        case sg::CC_Red:
            b = &b_red;
            break;
        case sg::CC_Green:
            b = &b_green;
            break;
        case sg::CC_Yellow:
            b = &b_yellow;
            break;
        case sg::CC_Blue:
            b = &b_blue;
            break;
        case sg::CC_Magenta:
            b = &b_magenta;
            break;
        case sg::CC_Cyan:
            b = &b_cyan;
            break;
        case sg::CC_White:
            b = &b_white;
            break;
        default:
            b = &sd;
            break;
        }

        code = *b + *f;
    }

#else
    WORD    s_color_attrs;
    HANDLE  s_hstdout;

    void sConsoleColorAttribute(sg::ConsoleColorGroup const& group, WORD & attrs)
    {
        static WORD f_black = 0;
        static WORD f_red = FOREGROUND_RED;
        static WORD f_green = FOREGROUND_GREEN;
        static WORD f_yellow = FOREGROUND_GREEN | FOREGROUND_RED;
        static WORD f_blue = FOREGROUND_BLUE;
        static WORD f_magenta = FOREGROUND_BLUE | FOREGROUND_RED;
        static WORD f_cyan = FOREGROUND_BLUE | FOREGROUND_GREEN;
        static WORD f_white = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;

        static WORD b_black = 0;
        static WORD b_red = BACKGROUND_RED;
        static WORD b_green = BACKGROUND_GREEN;
        static WORD b_yellow = BACKGROUND_GREEN | BACKGROUND_RED;
        static WORD b_blue = BACKGROUND_BLUE;
        static WORD b_magenta = BACKGROUND_BLUE | BACKGROUND_RED;
        static WORD b_cyan = BACKGROUND_BLUE | BACKGROUND_GREEN;
        static WORD b_white = BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_GREEN;

        attrs = 0;
        switch (group.f)
        {
        case sg::CC_Red:
            attrs |= f_red;
            break;
        case sg::CC_Green:
            attrs |= f_green;
            break;
        case sg::CC_Yellow:
            attrs |= f_yellow;
            break;
        case sg::CC_Blue:
            attrs |= f_blue;
            break;
        case sg::CC_Magenta:
            attrs |= f_magenta;
            break;
        case sg::CC_Cyan:
            attrs |= f_cyan;
            break;
        case sg::CC_White:
            attrs |= f_white;
            break;
        default:
            break;
        }

        switch (group.b)
        {
        case sg::CC_Red:
            attrs |= b_red;
            break;
        case sg::CC_Green:
            attrs |= b_green;
            break;
        case sg::CC_Yellow:
            attrs |= b_yellow;
            break;
        case sg::CC_Blue:
            attrs |= b_blue;
            break;
        case sg::CC_Magenta:
            attrs |= b_magenta;
            break;
        case sg::CC_Cyan:
            attrs |= b_cyan;
            break;
        case sg::CC_White:
            attrs |= b_white;
            break;
        default:
            break;
        }
    }

#endif // !SG_PLATFORM_LINUX

}


namespace sg
{
    void Console::Init(ConsoleType type)
    {
        m_type = type;

        switch (type)
        {
        case sg::CT_Custom:
            initscr();
            cbreak();
            noecho();
            keypad(stdscr, TRUE);
            if (has_colors())
            {
                m_has_color = true;
                start_color();
            }

            m_active = this->MakeWindow(std::string("std"), stdscr);

            g_winstream = ConsoleWindowStream(m_active);

            break;

        case sg::CT_Control:
        case sg::CT_Std:
#ifdef SG_PLATFORM_WINDOWS
            m_has_color = true;
            s_hstdout = ::GetStdHandle(STD_OUTPUT_HANDLE);
#endif
            if (type == CT_Control)
            {
                // ref http://stackoverflow.com/questions/14010147/corrupted-output-with-c-cin-cout-threads-and-sync-with-stdio
                // cannot mix using c++ output and c output
                std::ios_base::sync_with_stdio(false);

                // call flush for each output if the buf is ready
                std::cin.tie(nullptr);
                std::cerr.tie(nullptr);
                // do not use std::endl anymore, it will call flush
            }
            break;
        default:
            break;
        }

        m_init = true;
    }

    void Console::Shutdown()
    {
        if (m_type == CT_Custom)
        {
            m_active = nullptr;
            m_wnds.clear();

            endwin();

            m_init = false;
        }
    }

    void Console::ColorOn(ConsoleColorHandle const & h) const
    {
        BOOST_ASSERT(m_has_color && m_type != CT_Custom && h.i > 0 && h.i <= m_groups.size());

#ifdef SG_PLATFORM_WINDOWS
        if (s_hstdout != INVALID_HANDLE_VALUE)
        {
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            if (::GetConsoleScreenBufferInfo(s_hstdout, &csbi))
            {
                s_color_attrs = csbi.wAttributes;

                WORD nattrs;
                sConsoleColorAttribute(m_groups[h.i - 1], nattrs);

                ::SetConsoleTextAttribute(s_hstdout, nattrs);
            }
        }
#else
        std::string code;
        sEscapeColorCode(m_groups[h.i - 1], code);

        std::cout << code;
#endif

    }

    void Console::ColorOff(ConsoleColorHandle const & h) const
    {
        BOOST_ASSERT(m_has_color && m_type != CT_Custom && h.i == 0);

#ifdef SG_PLATFORM_WINDOWS
        if (s_hstdout != INVALID_HANDLE_VALUE)
        {
            ::SetConsoleTextAttribute(s_hstdout, s_color_attrs);
        }
#else
        std::cout << sd;
#endif
    }

    Console::Console()
        : m_colId(1)
        , m_active(nullptr)
        , m_type(CT_Std)
        , m_has_color(false)
    {

    }

    Console::~Console()
    {
        if (m_init)
        {
            this->Shutdown();
        }
    }

    ConsoleWindowPtr Console::MakeWindow(const std::string &name, uint32_t width, uint32_t height, int32_t x, int32_t y)
    {
        auto res = m_wnds.insert(std::make_pair(name, ConsoleWindowPtr(new ConsoleWindow(name, width, height, x, y), std::default_delete<ConsoleWindow>())));

        BOOST_ASSERT(res.second);

        return res.first->second;
    }

    ConsoleWindowPtr Console::MakeWindow(const std::string &name, void *extern_handle)
    {
        auto res = m_wnds.insert(std::make_pair(name, ConsoleWindowPtr(new ConsoleWindow(name, extern_handle), std::default_delete<ConsoleWindow>())));

        BOOST_ASSERT(res.second);

        return res.first->second;
    }

    ConsoleWindowPtr Console::FindWindow(const std::string &name)
    {
        auto it = m_wnds.find(name);
        if (it != m_wnds.end())
        {
            return it->second;
        }

        return nullptr;
    }

    void Console::SetActiveOutputWnd(ConsoleWindowPtr const& wnd)
    {
        BOOST_ASSERT(wnd); 
        m_active = wnd; 

        g_winstream = ConsoleWindowStream(m_active);
    }

    bool Console::SetActiveOutputWnd(std::string const & name)
    {
        ConsoleWindowPtr p = this->FindWindow(name);
        if (p)
        {
            this->SetActiveOutputWnd(p);
            return true;
        }

        return false;
    }

    ConsoleColorHandle Console::RegisterColorGroup(ConsoleColorGroup const & group)
    {
        BOOST_ASSERT((COLOR_PAIRS > 0) && (m_colId < (uint)COLOR_PAIRS));

        m_groups.push_back(group);
        if (m_type == CT_Custom)
        {
            BOOST_ASSERT(init_pair(static_cast<short>(m_colId), static_cast<short>(group.f), static_cast<short>(group.b)) != ERR);
        }

        ConsoleColorHandle h = { m_colId++ };

        return h;
    }

    ConsoleColorHandle const & Console::OffHandle() const
    {
        static ConsoleColorHandle h = { 0 };

        return h;
    }

    bool operator==(ConsoleColorHandle const & left, ConsoleColorHandle const & right)
    {
        return left.i == right.i;
    }

    bool operator!=(ConsoleColorHandle const & left, ConsoleColorHandle const & right)
    {
        return !(left == right);
    }

}




