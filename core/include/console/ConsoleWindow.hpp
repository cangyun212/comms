#ifndef __CORE_CONSOLE_WINDOW_HPP__
#define __CORE_CONSOLE_WINDOW_HPP__

#include "Core.hpp"

#include "boost/signals2.hpp"
#include "boost/format.hpp"

#include "Window.hpp"
#include "Console.hpp"

#define SG_CONSOLE_KEY_DOWN                        0402
#define SG_CONSOLE_KEY_UP                          0403
#define SG_CONSOLE_KEY_LEFT                        0404
#define SG_CONSOLE_KEY_RIGHT                       0405
#define SG_CONSOLE_KEY_BACKSPACE                   0407
#define SG_CONSOLE_KEY_DC                          0512
#define SG_CONSOLE_KEY_ENTER                       0527

namespace sg
{
    struct ConsoleEvent
    {
        int     ch;
    };

    class ConsoleWindow : public Window
    {
    private:
        ConsoleWindow(std::string const& name, uint width, uint height, int x, int y);
        ConsoleWindow(std::string const& name, void *extern_handle);

    public:
       ~ConsoleWindow();

    public:
        enum BorderStyle
        {
            BS_TB = 0,
            BS_LR,
            BS_CROSS,
            BS_NO
        };

        friend class Console;

    public:

        std::string const&  Name() const { return m_name; }

        void    Update();
        void    Render();
        void    Clear();

        int     Row() const;
        int     Coloumn() const;
        void    CurCoord(int &row, int &col) const;
        void    MoveTo(int row, int col) const;

        int     CursMode(int visibility) const;
        void    Beep() const;

        void    AddStr(std::string const& str) const;
        void    AddStr(const char *str, int n) const;
        void    AddStrTo(int row, int col, std::string const& str) const;
        void    AddStrTo(int row, int col, const char *str, int n) const;

        void    GetMargin(uint &t, uint &b, uint &l, uint &r) const;
        void    Border(BorderStyle bs, cwctype t, cwctype b, cwctype l, cwctype r);
        void    ClearBorder();

        void    ClearHLine(int row, int col, int n) const;
        void    ClearVLine(int row, int col, int n) const;

        void    Scroll(int n);

        void    ColorOn(ConsoleColorHandle const& h) const;
        void    ColorOff(ConsoleColorHandle const& h) const;

        friend  void NextEvent(ConsoleWindow const& win, ConsoleEvent &event);

    public:
        typedef boost::signals2::signal<void(ConsoleWindow const& wnd, int ch)> CharEventType;
        typedef boost::signals2::signal<void(ConsoleWindow const& wnd, int key)> KeyEventType;

        CharEventType&      CharEvent() { return m_char_event; }
        KeyEventType&       KeyEvent() { return m_key_event; }

        void    MsgProc(ConsoleEvent const& event);

    private:
        void    TBBorder(cwctype t, cwctype b, cwctype l, cwctype r);
        void    LRBorder(cwctype t, cwctype b, cwctype l, cwctype r);
        void    CRBorder(cwctype t, cwctype b, cwctype l, cwctype r);

    private:
        CharEventType       m_char_event;
        KeyEventType        m_key_event;
        uint                m_top_margin;
        uint                m_lef_margin;
        uint                m_bot_margin;
        uint                m_rht_margin;
        cwctype             m_tc;
        cwctype             m_lc;
        cwctype             m_bc;
        cwctype             m_rc;
        BorderStyle         m_bs;
    };

}
#endif //__CORE_CONSOLE_HPP__
