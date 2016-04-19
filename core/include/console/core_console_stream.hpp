#ifndef __SG_CORE_CONSOLE_STREAM_HPP__
#define __SG_CORE_CONSOLE_STREAM_HPP__

#include "core/core.hpp"
#include "core/console/core_console_window.hpp"

#include <vector>
#include <iostream>

namespace sg
{

    enum ConsoleStreamColor
    {
        CSC_Black = 0,
        CSC_Red,
        CSC_Green,
        CSC_Yellow,
        CSC_Blue,
        CSC_Magenta,
        CSC_Cyan,
        CSC_White,
        CSC_Num
    };

    struct ConsoleStreamColorGroup
    {
        ConsoleStreamColor  f;
        ConsoleStreamColor  b;
    };

    class ConsoleStream
    {
    public:
        enum { colorable = true };

    public:
        ConsoleStream(ConsoleWindow *win) : m_win(win) { BOOST_ASSERT(m_win); }
        ConsoleStream() : m_win(nullptr) {}

    public:
        static int16_t sRegisterColorGroup(ConsoleStreamColorGroup const& group);
        static bool sIsColorSupport();

    private:
        static int16_t  s_col_id;
        static bool s_col_sup;
        static bool s_col_check;
        static std::vector<ConsoleStreamColorGroup> s_col_groups;

    public:

        template<typename T>
        ConsoleStream& operator<< (T const& t)
        {
            if (m_win)
            {
                return *this << boost::format("%1%") % t;
            }
            else
            {
                std::cout << t;
                return *this;
            }
        }

        void flush();

        // called only if color is support
        void ColorOn(int16_t index) const;
        void ColorOff(int16_t index) const;

    private:
        ConsoleWindow      *m_win;
    };

    template <>
    inline ConsoleStream& ConsoleStream::operator<< (boost::format const& fmt)
    {
        if (m_win)
        {
            m_win->AddStr(fmt.str());
            return *this;
        }
        else
        {
            std::cout << fmt;
            return *this;
        }
    }

    template <>
    inline ConsoleStream& ConsoleStream::operator<< (std::string const& str)
    {
        if (m_win)
        {
            m_win->AddStr(str);
            return *this;
        }
        else
        {
            std::cout << str;
            return *this;
        }
    }

    template <typename T>
    class has_color_fuc_stream
    {
    public:
        static const bool value = T::colorable;
    };

    template <>
    class has_color_fuc_stream<std::ostream>
    {
    public:
        static const bool value = false;
    };

    template <>
    class has_color_fuc_stream<std::ofstream>
    {
    public:
        static const bool value = false;
    };
}



#endif



