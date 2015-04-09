#include "core/core.hpp"
#include "core/console/core_console_stream.hpp"
#include "core/core_console.hpp"

#ifdef CORE_PLATFORM_LINUX
#include <ncurses.h>
#endif

namespace sg
{

#ifdef CORE_PLATFORM_LINUX
    namespace
    {
        // ref http://ascii-table.com/ansi-escape-sequences.php
        std::string s_escape_color_code(ConsoleStreamColorGroup const& group)
        {
            static std::string f_black  ("\033[30m");
            static std::string f_red    ("\033[31m");
            static std::string f_green  ("\033[32m");
            static std::string f_yellow ("\033[33m");
            static std::string f_blue   ("\033[34m");
            static std::string f_magenta("\033[35m");
            static std::string f_cyan   ("\033[36m");
            static std::string f_white  ("\033[37m");

            static std::string b_black  ("\033[40m");
            static std::string b_red    ("\033[41m");
            static std::string b_green  ("\033[42m");
            static std::string b_yellow ("\033[43m");
            static std::string b_blue   ("\033[44m");
            static std::string b_magenta("\033[45m");
            static std::string b_cyan   ("\033[46m");
            static std::string b_white  ("\033[47m");

            static std::string d("\033[0m");

            std::string f, b;
            switch(group.f)
            {
            case CSC_Black:
                f = f_black;
                break;
            case CSC_Red:
                f = f_red;
                break;
            case CSC_Green:
                f = f_green;
                break;
            case CSC_Yellow:
                f = f_yellow;
                break;
            case CSC_Blue:
                f = f_blue;
                break;
            case CSC_Magenta:
                f = f_magenta;
                break;
            case CSC_Cyan:
                f = f_cyan;
                break;
            case CSC_White:
                f = f_white;
                break;
            default:
                f = d;
                break;
            }

            switch(group.b)
            {
            case CSC_Black:
                b = b_black;
                break;
            case CSC_Red:
                b = b_red;
                break;
            case CSC_Green:
                b = b_green;
                break;
            case CSC_Yellow:
                b = b_yellow;
                break;
            case CSC_Blue:
                b = b_blue;
                break;
            case CSC_Magenta:
                b = b_magenta;
                break;
            case CSC_Cyan:
                b = b_cyan;
                break;
            case CSC_White:
                b = b_white;
                break;
            default:
                b = d;
                break;
            }

            return b + f;
        }
    }
#endif

    int16_t ConsoleStream::s_col_id = 1;
    bool ConsoleStream::s_col_sup = false;
    bool ConsoleStream::s_col_check = false;
    std::vector<ConsoleStreamColorGroup> ConsoleStream::s_col_groups;

    int16_t ConsoleStream::sRegisterColorGroup(const ConsoleStreamColorGroup &group)
    {
        s_col_groups.push_back(group);
        if (Console::Instance().IsCustomWin())
        {
            init_pair(s_col_id, group.f, group.b);
        }
        return s_col_id++;
    }

    bool ConsoleStream::sIsColorSupport()
    {
        if (!s_col_check)
        {
            if (Console::Instance().IsCustomWin())
            {
                s_col_sup = has_colors();
            }
            else
            {
                // TODO : maybe we should do with terminfo
                // if IsCustomWin return false, we should
                // check color at very begin in the Console::Init
                // because initscr will clear the current screen
                initscr();
                s_col_sup = has_colors();
                endwin();
            }
            s_col_check = true;
        }

        return s_col_sup;
    }

    void ConsoleStream::flush()
    {
        if (m_win)
        {
            m_win->Render();
        }
        else
        {
            std::cout.flush();
        }
    }

    void ConsoleStream::ColorOn(int16_t index) const
    {
        if (m_win)
        {
            m_win->ColorOn(index);
        }
        else
        {
#ifdef CORE_PLATFORM_LINUX
            std::cout << s_escape_color_code(s_col_groups[index-1]);
#endif
        }
    }

    void ConsoleStream::ColorOff(int16_t index) const
    {
        if (m_win)
        {
            m_win->ColorOff(index);
        }
        else
        {
#ifdef CORE_PLATFORM_LINUX
            std::cout << "\033[0m";
#endif
        }
    }

}




