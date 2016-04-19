#ifndef __SG_CORE_WINDOW_HPP__
#define __SG_CORE_WINDOW_HPP__

#include "core.hpp"

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
            , m_native_handle(nullptr)
        {

        }

        virtual ~Window() {}

    public:
        int32_t         Top() const { return m_top; }
        int32_t         Left() const { return m_left; }
        uint32_t        Width() const { return m_width; }
        uint32_t        Height() const {return m_height; }
        void*           Handle() const { return m_native_handle; }


    protected:
        std::string     m_name;
        int32_t         m_top;
        int32_t         m_left;
        uint32_t        m_width;
        uint32_t        m_height;
        void           *m_native_handle;
    };
}



#endif
