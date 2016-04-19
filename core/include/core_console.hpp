#ifndef __SG_CORE_CONSOLE_HPP__
#define __SG_CORE_CONSOLE_HPP__

#include "core.hpp"
#include "core_singleton.hpp"
#include "console/core_console_window.hpp"


namespace sg
{

    class Console : public Singleton<Console>
    {
    public:
        void    Init(bool custom_win = false); // must be called before everything else
        bool    IsCustomWin() const { return m_custom_wnd; }

        ConsoleWindow* MakeWindow(std::string const& name, uint32_t width, uint32_t height, int32_t x, int32_t y);
        ConsoleWindow* MakeWindow(std::string const& name, void* extern_handle);

        ConsoleWindow*  FindWindow(std::string const& name);
        ConsoleWindow*  GetActiveOutputWnd() const { return m_active_output_wnd; }
        void            SetActiveOutputWnd(ConsoleWindow* wnd);

    public:
        Console();
       ~Console();

    private:
        void DealWindow(std::string const& name, ConsoleWindow *p);

    private:
        typedef unordered_map<std::string, ConsoleWindow*>  WndMap;
        WndMap  m_wnds;

        ConsoleWindow   *m_active_output_wnd;
        bool    m_custom_wnd;
    };
}


#endif


