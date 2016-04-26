#ifndef __SG_CORE_WINDOW_HPP__
#define __SG_CORE_WINDOW_HPP__

#include "Core.hpp"

#include <string>

namespace sg
{
    class Window
    {
    public:
        Window(std::string const& name)
            : m_name(name)
            , m_top(0)
            , m_left(0)
            , m_width(0)
            , m_height(0)
            , m_native(nullptr)
        {

        }

        virtual ~Window() {}

    public:
        int             Top() const { return m_top; }
        int             Left() const { return m_left; }
        uint            Width() const { return m_width; }
        uint            Height() const {return m_height; }
        void*           Handle() const { return m_native; }

    protected:
        std::string     m_name;
        int             m_top;
        int             m_left;
        uint            m_width;
        uint            m_height;
        void           *m_native;
    };
}



#endif
