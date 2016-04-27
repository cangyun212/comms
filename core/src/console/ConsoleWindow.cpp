#include "Core.hpp"

#ifdef SG_PLATFORM_WINDOWS
#include "curses.h"
#elif defined(SG_PLATFORM_LINUX)
#include <ncurses.h>
#endif

#include "Utils.hpp"
#include "Console/ConsoleWindow.hpp"

namespace
{
    inline static WINDOW* sHandle2Win(void *handle)
    {
        return reinterpret_cast<WINDOW*>(handle);
    }

    sg::cwctype s_color = 0;
}

namespace sg
{
    ConsoleWindow::ConsoleWindow(const std::string &name, uint width, uint height, int x, int y)
        : Window(name)
        , m_top_margin(0)
        , m_lef_margin(0)
        , m_bot_margin(0)
        , m_rht_margin(0)
        , m_tc('\0')
        , m_lc('\0')
        , m_bc('\0')
        , m_rc('\0')
        , m_bs(BS_NO)
    {
        BOOST_ASSERT((width <= static_cast<uint>(std::numeric_limits<int>::max())) &&
            height <= static_cast<uint>(std::numeric_limits<int>::max()));

        m_height = height;
        m_width = width;
        m_top = y;
        m_left = x;

        WINDOW *handle = newwin(static_cast<int>(m_height), static_cast<int>(m_width), m_top, m_left);
        scrollok(handle, TRUE);
        keypad(handle, TRUE);

        m_native = handle;
    }

    ConsoleWindow::ConsoleWindow(const std::string &name, void *extern_handle)
        : Window(name)
        , m_top_margin(0)
        , m_lef_margin(0)
        , m_bot_margin(0)
        , m_rht_margin(0)
        , m_tc('\0')
        , m_lc('\0')
        , m_bc('\0')
        , m_rc('\0')
        , m_bs(BS_NO)
    {
        WINDOW *wnd = sHandle2Win(extern_handle);
        scrollok(wnd, TRUE);
        keypad(wnd, TRUE);

        getmaxyx(wnd, m_height, m_width);
        m_top = m_left = 0;

        m_native = wnd;
    }

    ConsoleWindow::~ConsoleWindow()
    {
        delwin(sHandle2Win(m_native));
        m_native = nullptr;
    }

    void ConsoleWindow::GetMargin(uint &t, uint &b, uint &l, uint &r) const
    {
        t = m_top_margin;
        b = m_bot_margin;
        l = m_lef_margin;
        r = m_rht_margin;
    }

    void ConsoleWindow::Update()
    {
        wnoutrefresh(sHandle2Win(m_native));
    }

    void ConsoleWindow::Render()
    {
        wrefresh(sHandle2Win(m_native));
    }

    void ConsoleWindow::Clear()
    {
        wclear(sHandle2Win(m_native));
    }

    int ConsoleWindow::Row() const
    {
        return getcury(sHandle2Win(m_native));
    }

    int ConsoleWindow::Coloumn() const
    {
        return getcurx(sHandle2Win(m_native));
    }

    void ConsoleWindow::CurCoord(int &row, int &col) const
    {
        getyx(sHandle2Win(m_native), row, col);
    }

    void ConsoleWindow::MoveTo(int row, int col) const
    {
        wmove(sHandle2Win(m_native), row, col);
    }

    int ConsoleWindow::CursMode(int visibility) const
    {
        return curs_set(visibility);
    }

    void ConsoleWindow::Beep() const
    {
        beep();
    }

    void ConsoleWindow::AddStr(const std::string &str) const
    {
        waddstr(sHandle2Win(m_native), str.c_str());
    }

    void ConsoleWindow::AddStr(const char *str, int n) const
    {
        waddnstr(sHandle2Win(m_native), str, n);
    }

    void ConsoleWindow::AddStrTo(int row, int col, const std::string &str) const
    {
        mvwaddstr(sHandle2Win(m_native), row, col, str.c_str());
    }

    void ConsoleWindow::AddStrTo(int row, int col, const char *str, int n) const
    {
        mvwaddnstr(sHandle2Win(m_native), row, col, str, n);
    }

    void ConsoleWindow::ColorOn(ConsoleColorHandle const& h) const
    {
        s_color = h.i;
        BOOST_ASSERT(s_color > 0 && s_color < (uint)COLOR_PAIRS);
        wattron(sHandle2Win(m_native), COLOR_PAIR(s_color));
    }

    void ConsoleWindow::ColorOff(ConsoleColorHandle const& h) const
    {
        SG_UNREF_PARAM(h);

        BOOST_ASSERT(s_color > 0 && s_color < (uint)COLOR_PAIRS);
        wattroff(sHandle2Win(m_native), COLOR_PAIR(s_color));
    }

    void ConsoleWindow::ClearBorder()
    {
        if (m_bs != BS_NO)
        {
            int row, col;
            CurCoord(row, col);

            WINDOW *wnd = sHandle2Win(m_native);
            if (m_top_margin)
            {
                if (m_bs == BS_TB)
                {
                    mvwhline(wnd, 0, 0, ' ', m_width);
                }
                else if (m_width > 2)
                {
                    mvwhline(wnd, 0, 1, ' ', m_width - 2);
                }
                m_top_margin = 0;
            }

            if (m_bot_margin)
            {
                if (m_bs == BS_TB)
                {
                    mvwhline(wnd, m_height - 1, 0, ' ', m_width);
                }
                else if (m_width > 2)
                {
                    mvwhline(wnd, m_height - 1, 1, ' ', m_width - 2);
                }
                m_bot_margin = 0;
            }

            if (m_lef_margin)
            {
                if (m_bs == BS_LR)
                {
                    mvwvline(wnd, 0, 0, ' ', m_height);
                }
                else if (m_height > 2)
                {
                    mvwvline(wnd, 1, 0, ' ', m_height -2);
                }
                m_lef_margin = 0;
            }

            if (m_rht_margin)
            {
                if (m_bs == BS_LR)
                {
                    mvwvline(wnd, 0, m_width - 1, ' ', m_height);
                }
                else if (m_height > 2)
                {
                    mvwvline(wnd, 1, m_width - 1, ' ', m_height - 2);
                }
                m_rht_margin = 0;
            }

            MoveTo(row, col);
        }
    }

    void ConsoleWindow::Border(BorderStyle bs, cwctype t, cwctype b, cwctype l, cwctype r)
    {
        ClearBorder();

        m_bs = bs;

        m_tc = t;
        m_bc = b;
        m_lc = l;
        m_rc = r;

        int row, col;
        CurCoord(row, col);

        switch(m_bs)
        {
        case BS_TB:
            TBBorder(t, b, l, r);
            break;
        case BS_LR:
            LRBorder(t, b, l, r);
            break;
        case BS_CROSS:
            CRBorder(t, b, l, r);
            break;
        default:
            break;
        }

        MoveTo(row, col);
    }

    void ConsoleWindow::TBBorder(cwctype t, cwctype b, cwctype l, cwctype r)
    {
        WINDOW *wnd = sHandle2Win(m_native);

        if (t != '\0')
        {
            mvwhline(wnd, 0, 0, t, m_width);
            m_top_margin = 1;
        }
        else
            m_top_margin = 0;

        if (b != '\0')
        {
            mvwhline(wnd, m_height - 1, 0, b, m_width);
            m_bot_margin = 1;
        }
        else
            m_bot_margin = 0;

        if (l != '\0' && m_height > 2)
        {
            mvwvline(wnd, 1, 0, l, m_height - 2);
            m_lef_margin = 1;
        }
        else
            m_lef_margin = 0;

        if (r != '\0' && m_height > 2)
        {
            mvwvline(wnd, 1, m_width - 1, r, m_height - 2);
            m_rht_margin = 1;
        }
        else
            m_rht_margin = 0;
    }

    void ConsoleWindow::LRBorder(cwctype t, cwctype b, cwctype l, cwctype r)
    {
        WINDOW *wnd = sHandle2Win(m_native);

        if (l != '\0')
        {
            mvwvline(wnd, 0, 0, l, m_height);
            m_lef_margin = 1;
        }
        else
            m_lef_margin = 0;

        if (r != '\0')
        {
            mvwvline(wnd, 0, m_width - 1, r, m_height);
            m_rht_margin = 1;
        }
        else
            m_rht_margin = 0;

        if (t != '\0' && m_width > 2)
        {
            mvwhline(wnd, 0, 1, t, m_width - 2);
            m_top_margin = 1;
        }
        else
            m_top_margin = 0;

        if (b != '\0' && m_width > 2)
        {
            mvwhline(wnd, m_height - 1, 1, b, m_width - 2);
            m_bot_margin = 1;
        }
        else
            m_bot_margin = 0;
    }

    void ConsoleWindow::CRBorder(cwctype t, cwctype b, cwctype l, cwctype r)
    {
        WINDOW *wnd = sHandle2Win(m_native);

        if (t != '\0' && m_width > 2)
        {
            mvwhline(wnd, 0, 1, t, m_width - 2);
            m_top_margin = 1;
        }
        else
            m_top_margin = 0;

        if (b != '\0' && m_width > 2)
        {
            mvwhline(wnd, m_height - 1, 1, b, m_width - 2);
            m_bot_margin = 1;
        }
        else
            m_bot_margin = 0;

        if (l != '\0' && m_height > 2)
        {
            mvwvline(wnd, 1, 0, l, m_height - 2);
            m_lef_margin = 1;
        }
        else
            m_lef_margin = 0;

        if (r != '\0' && m_height > 2)
        {
            mvwvline(wnd, 1, m_width - 1, r, m_height - 2);
            m_rht_margin = 1;
        }
        else
            m_rht_margin = 0;
    }


    void ConsoleWindow::Scroll(int n)
    {
        wscrl(sHandle2Win(m_native), n);
        Border(m_bs, m_tc, m_bc, m_lc, m_rc);
    }

    void ConsoleWindow::ClearHLine(int row, int col, int n) const
    {
        mvwhline(sHandle2Win(m_native), row, col, ' ', n);
    }

    void ConsoleWindow::ClearVLine(int row, int col, int n) const
    {
        mvwvline(sHandle2Win(m_native), row, col, ' ', n);
    }

    void ConsoleWindow::MsgProc(const ConsoleEvent &event)
    {
        if (isprint(event.ch))
        {
            this->CharEvent()(*this, event.ch);
        }
        else
        {
            this->KeyEvent()(*this, event.ch);
        }
    }

    void NextEvent(ConsoleWindow const& win, ConsoleEvent &event)
    {
        event.ch = wgetch(sHandle2Win(win.m_native));
    }
}
