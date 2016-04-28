#ifndef __SG_CORE_CONSOLE_STREAM_HPP__
#define __SG_CORE_CONSOLE_STREAM_HPP__

#include "Core.hpp"

#include <iostream>
#include <mutex>

#include "Console.hpp"
#include "Console/ConsoleWindow.hpp"

namespace sg
{
    class CORE_API ConsoleWindowStream
    {
    public:
        enum { colorable = true };

    public:
        ConsoleWindowStream(ConsoleWindowPtr const& wnd = nullptr) : m_wnd(wnd) {}

    public:

        template<typename T>
        ConsoleWindowStream& operator<< (T const& t)
        {
            BOOST_ASSERT(m_wnd);
            return *this << boost::format("%1%") % t;
        }

    public:
        void        flush();

    private:
        ConsoleWindowPtr        m_wnd;
    };

    template<>
    inline ConsoleWindowStream& ConsoleWindowStream::operator<< (boost::format const& fmt)
    {
        m_wnd->AddStr(fmt.str());
        return *this;
    }

    template<>
    inline ConsoleWindowStream& ConsoleWindowStream::operator<< (std::string const& str)
    {
        m_wnd->AddStr(str);
        return *this;
    }

    template<>
    inline ConsoleWindowStream& ConsoleWindowStream::operator<< (ConsoleColorHandle const& h)
    {
        if (h == Console::Instance().OffHandle())
            m_wnd->ColorOff(h);
        else
            m_wnd->ColorOn(h);

        return *this;
    }

    class CORE_API ConsoleStdColorStream
    {
    public:
        enum { colorable = true };
        
    public:
        template<typename T>
        ConsoleStdColorStream& operator<< (T const& t)
        {
            std::cout << t;
            return *this;
        }

    public:
        void        flush();

    };

    template<>
    inline ConsoleStdColorStream& ConsoleStdColorStream::operator<< (ConsoleColorHandle const& h)
    {
        Console & c = Console::Instance();
        if (h == c.OffHandle())
            c.ColorOff(h);
        else
            c.ColorOn(h);

        return *this;
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

    extern CORE_API ConsoleWindowStream      g_winstream;
    extern CORE_API ConsoleStdColorStream    g_colstream;

}



#endif



