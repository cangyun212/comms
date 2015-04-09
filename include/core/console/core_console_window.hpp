#ifndef __CORE_CONSOLE_WINDOW_HPP__
#define __CORE_CONSOLE_WINDOW_HPP__

#include "core/core.hpp"

#include "boost/signals2.hpp"
#include "boost/format.hpp"

#include "core/core_window.hpp"

#define CONSOLE_KEY_DOWN                        0402
#define CONSOLE_KEY_UP                          0403
#define CONSOLE_KEY_LEFT                        0404
#define CONSOLE_KEY_RIGHT                       0405
#define CONSOLE_KEY_BACKSPACE                   0407
#define CONSOLE_KEY_DC                          0512
#define CONSOLE_KEY_ENTER                       0527

namespace sg
{

    struct ConsoleEvent
    {
        int     ch;
    };

    class ConsoleWindow : public Window
    {
    private:
        ConsoleWindow(std::string const& name, uint32_t width, uint32_t height, int32_t x, int32_t y);
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

        void    GetMargin(uint32_t &t, uint32_t &b, uint32_t &l, uint32_t &r) const;
        void    Border(BorderStyle bs, uint64_t t, uint64_t b, uint64_t l, uint64_t r);
        void    ClearBorder();

        void    ClearHLine(int row, int col, int n) const;
        void    ClearVLine(int row, int col, int n) const;

        void    Scroll(int n);

        void    ColorOn(int16_t index) const;
        void    ColorOff(int16_t index) const;

        void    ActiveOutput(bool active) { m_active_output = active; }
        bool    ActiveOutput() const { return m_active_output; }

        friend  void NextEvent(ConsoleWindow const& win, ConsoleEvent &event);

    public:
        typedef boost::signals2::signal<void(ConsoleWindow const& wnd, int ch)> CharEventType;
        typedef boost::signals2::signal<void(ConsoleWindow const& wnd, int key)> KeyEventType;

        CharEventType&      CharEvent() { return m_char_event; }
        KeyEventType&       KeyEvent() { return m_key_event; }

        void    MsgProc(ConsoleEvent const& event);

    private:
        void    TBBorder(uint64_t t, uint64_t b, uint64_t l, uint64_t r);
        void    LRBorder(uint64_t t, uint64_t b, uint64_t l, uint64_t r);
        void    CRBorder(uint64_t t, uint64_t b, uint64_t l, uint64_t r);

    private:
        CharEventType       m_char_event;
        KeyEventType        m_key_event;
        uint32_t            m_top_margin;
        uint32_t            m_lef_margin;
        uint32_t            m_bot_margin;
        uint32_t            m_rht_margin;
        uint64_t            m_tc;
        uint64_t            m_lc;
        uint64_t            m_bc;
        uint64_t            m_rc;
        BorderStyle         m_bs;
        bool                m_active_output;
    };

}
#endif //__CORE_CONSOLE_HPP__
