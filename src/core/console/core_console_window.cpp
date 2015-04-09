#include "core/core.hpp"

#include "core/console/core_console_window.hpp"

#ifdef CORE_PLATFORM_LINUX
#include <ncurses.h>
#endif

namespace sg
{
    ConsoleWindow::ConsoleWindow(const std::string &name, uint32_t width, uint32_t height, int32_t x, int32_t y)
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
        m_height = height;
        m_width = width;
        m_top = y;
        m_left = x;

        WINDOW *handle = newwin(m_height, m_width, m_top, m_left);
        scrollok(handle, TRUE);
        keypad(handle, TRUE);

        m_native_handle = handle;
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
        WINDOW *handle = (WINDOW*)extern_handle;
        scrollok(handle, TRUE);
        keypad(handle, TRUE);

        getmaxyx(handle, m_height, m_width);
        m_top = m_left = 0;

        m_native_handle = handle;
    }

    ConsoleWindow::~ConsoleWindow()
    {
        if (m_native_handle)
        {
            delwin((WINDOW*)m_native_handle);
        }
    }

    void ConsoleWindow::GetMargin(uint32_t &t, uint32_t &b, uint32_t &l, uint32_t &r) const
    {
        t = m_top_margin;
        b = m_bot_margin;
        l = m_lef_margin;
        r = m_rht_margin;
    }

    void ConsoleWindow::Update()
    {
        wnoutrefresh((WINDOW*)m_native_handle);
    }

    void ConsoleWindow::Render()
    {
        wrefresh((WINDOW*)m_native_handle);
    }

    void ConsoleWindow::Clear()
    {
        wclear((WINDOW*)m_native_handle);
    }

    int ConsoleWindow::Row() const
    {
        return getcury((WINDOW*)m_native_handle);
    }

    int ConsoleWindow::Coloumn() const
    {
        return getcurx((WINDOW*)m_native_handle);
    }

    void ConsoleWindow::CurCoord(int &row, int &col) const
    {
        getyx((WINDOW*)m_native_handle, row, col);
    }

    void ConsoleWindow::MoveTo(int row, int col) const
    {
        wmove((WINDOW*)m_native_handle, row, col);
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
        waddstr((WINDOW*)m_native_handle, str.c_str());
    }

    void ConsoleWindow::AddStr(const char *str, int n) const
    {
        waddnstr((WINDOW*)m_native_handle, str, n);
    }

    void ConsoleWindow::AddStrTo(int row, int col, const std::string &str) const
    {
        mvwaddstr((WINDOW*)m_native_handle, row, col, str.c_str());
    }

    void ConsoleWindow::AddStrTo(int row, int col, const char *str, int n) const
    {
        mvwaddnstr((WINDOW*)m_native_handle, row, col, str, n);
    }

    void ConsoleWindow::ColorOn(int16_t index) const
    {
        wattron((WINDOW*)m_native_handle, COLOR_PAIR(index));
    }

    void ConsoleWindow::ColorOff(int16_t index) const
    {
        wattroff((WINDOW*)m_native_handle, COLOR_PAIR(index));
    }

    void ConsoleWindow::ClearBorder()
    {
        if (m_bs != BS_NO)
        {
            int row, col;
            CurCoord(row, col);

            WINDOW *handle = (WINDOW*)m_native_handle;
            if (m_top_margin)
            {
                if (m_bs == BS_TB)
                {
                    mvwhline(handle, 0, 0, ' ', m_width);
                }
                else if (m_width > 2)
                {
                    mvwhline(handle, 0, 1, ' ', m_width - 2);
                }
                m_top_margin = 0;
            }

            if (m_bot_margin)
            {
                if (m_bs == BS_TB)
                {
                    mvwhline(handle, m_height - 1, 0, ' ', m_width);
                }
                else if (m_width > 2)
                {
                    mvwhline(handle, m_height - 1, 1, ' ', m_width - 2);
                }
                m_bot_margin = 0;
            }

            if (m_lef_margin)
            {
                if (m_bs == BS_LR)
                {
                    mvwvline(handle, 0, 0, ' ', m_height);
                }
                else if (m_height > 2)
                {
                    mvwvline(handle, 1, 0, ' ', m_height -2);
                }
                m_lef_margin = 0;
            }

            if (m_rht_margin)
            {
                if (m_bs == BS_LR)
                {
                    mvwvline(handle, 0, m_width - 1, ' ', m_height);
                }
                else if (m_height > 2)
                {
                    mvwvline(handle, 1, m_width - 1, ' ', m_height - 2);
                }
                m_rht_margin = 0;
            }

            MoveTo(row, col);
        }
    }

    void ConsoleWindow::Border(BorderStyle bs, uint64_t t, uint64_t b, uint64_t l, uint64_t r)
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

    void ConsoleWindow::TBBorder(uint64_t t, uint64_t b, uint64_t l, uint64_t r)
    {
        WINDOW *handle = (WINDOW*)m_native_handle;

        if (t != '\0')
        {
            mvwhline(handle, 0, 0, t, m_width);
            m_top_margin = 1;
        }
        else
            m_top_margin = 0;

        if (b != '\0')
        {
            mvwhline(handle, m_height - 1, 0, b, m_width);
            m_bot_margin = 1;
        }
        else
            m_bot_margin = 0;

        if (l != '\0' && m_height > 2)
        {
            mvwvline(handle, 1, 0, l, m_height - 2);
            m_lef_margin = 1;
        }
        else
            m_lef_margin = 0;

        if (r != '\0' && m_height > 2)
        {
            mvwvline(handle, 1, m_width - 1, r, m_height - 2);
            m_rht_margin = 1;
        }
        else
            m_rht_margin = 0;
    }

    void ConsoleWindow::LRBorder(uint64_t t, uint64_t b, uint64_t l, uint64_t r)
    {
        WINDOW *handle = (WINDOW*)m_native_handle;

        if (l != '\0')
        {
            mvwvline(handle, 0, 0, l, m_height);
            m_lef_margin = 1;
        }
        else
            m_lef_margin = 0;

        if (r != '\0')
        {
            mvwvline(handle, 0, m_width - 1, r, m_height);
            m_rht_margin = 1;
        }
        else
            m_rht_margin = 0;

        if (t != '\0' && m_width > 2)
        {
            mvwhline(handle, 0, 1, t, m_width - 2);
            m_top_margin = 1;
        }
        else
            m_top_margin = 0;

        if (b != '\0' && m_width > 2)
        {
            mvwhline(handle, m_height - 1, 1, b, m_width - 2);
            m_bot_margin = 1;
        }
        else
            m_bot_margin = 0;
    }

    void ConsoleWindow::CRBorder(uint64_t t, uint64_t b, uint64_t l, uint64_t r)
    {
        WINDOW *handle = (WINDOW*)m_native_handle;

        if (t != '\0' && m_width > 2)
        {
            mvwhline(handle, 0, 1, t, m_width - 2);
            m_top_margin = 1;
        }
        else
            m_top_margin = 0;

        if (b != '\0' && m_width > 2)
        {
            mvwhline(handle, m_height - 1, 1, b, m_width - 2);
            m_bot_margin = 1;
        }
        else
            m_bot_margin = 0;

        if (l != '\0' && m_height > 2)
        {
            mvwvline(handle, 1, 0, l, m_height - 2);
            m_lef_margin = 1;
        }
        else
            m_lef_margin = 0;

        if (r != '\0' && m_height > 2)
        {
            mvwvline(handle, 1, m_width - 1, r, m_height - 2);
            m_rht_margin = 1;
        }
        else
            m_rht_margin = 0;
    }


    void ConsoleWindow::Scroll(int n)
    {
        wscrl((WINDOW*)m_native_handle, n);
        Border(m_bs, m_tc, m_bc, m_lc, m_rc);
    }

    void ConsoleWindow::ClearHLine(int row, int col, int n) const
    {
        mvwhline((WINDOW*)m_native_handle, row, col, ' ', n);
    }

    void ConsoleWindow::ClearVLine(int row, int col, int n) const
    {
        mvwvline((WINDOW*)m_native_handle, row, col, ' ', n);
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
        event.ch = wgetch((WINDOW*)win.m_native_handle);
    }
}
