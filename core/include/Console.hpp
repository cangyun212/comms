#ifndef __SG_CORE_CONSOLE_HPP__
#define __SG_CORE_CONSOLE_HPP__

#include "Core.hpp"

#include <unordered_map>

#include "Singleton.hpp"

namespace sg
{
    enum ConsoleType
    {
        CT_Custom = 0,
        CT_Control,
        CT_Std,

        CT_Num
    };

    enum ConsoleColor
    {
        CC_Black = 0,
        CC_Red,
        CC_Green,
        CC_Yellow,
        CC_Blue,
        CC_Magenta,
        CC_Cyan,
        CC_White,
        
        CC_Num
    };

    struct ConsoleColorGroup
    {
        ConsoleColor    f;
        ConsoleColor    b;
    };

    struct ConsoleColorHandle
    {
        uint            i;
    };

    bool operator==(ConsoleColorHandle const& left, ConsoleColorHandle const& right);
    bool operator!=(ConsoleColorHandle const& left, ConsoleColorHandle const& right);

    class Console : public Singleton<Console>
    {
    public:
        void    Init(ConsoleType type); // must be called before everything else
        void    Shutdown();
        bool    IsColorSupport() const { return m_has_color; }
        void    ColorOn(ConsoleColorHandle const& h) const;
        void    ColorOff(ConsoleColorHandle const& h) const;

        ConsoleType         Type() const { return m_type; }

        ConsoleWindowPtr    MakeWindow(std::string const& name, uint32_t width, uint32_t height, int32_t x, int32_t y);
        ConsoleWindowPtr    MakeWindow(std::string const& name, void* extern_handle);

        ConsoleWindowPtr    FindWindow(std::string const& name);
        ConsoleWindowPtr    GetActiveOutputWnd() const { return m_active; }
        void                SetActiveOutputWnd(ConsoleWindowPtr const& wnd);
        bool                SetActiveOutputWnd(std::string const& name);

        ConsoleColorHandle  RegisterColorGroup(ConsoleColorGroup const& group);
        ConsoleColorHandle const& OffHandle() const;

    public:
        Console();
       ~Console() {}

    private:
        typedef std::unordered_map<std::string, ConsoleWindowPtr>  WndMap;
        WndMap  m_wnds;

        typedef std::vector<ConsoleColorGroup> ColorList;
        ColorList   m_groups;
        uint        m_colId;

        ConsoleWindowPtr    m_active;
        ConsoleType         m_type;
        bool                m_has_color;
    };
}


#endif


