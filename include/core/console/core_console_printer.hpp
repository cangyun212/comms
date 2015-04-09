#ifndef __SG_CORE_CONSOLE_PRINTER__
#define __SG_CORE_CONSOLE_PRINTER__

#include "core/core.hpp"

#include <iostream>


#include "core/core_singleton.hpp"
#include "core/core_thread.hpp"
#include "core/console/core_console_stream.hpp"

#include "boost/format.hpp"

namespace sg{

    template <typename Stream>
    class LockableStream
    {
    public:
        enum { lockable = true };
        enum { colorable = has_color_fuc_stream<Stream>::value };

    public:
        LockableStream()
            : m_s(nullptr)
        {

        }

        LockableStream(Stream *s)
            : m_s(s)
        {
            BOOST_ASSERT(m_s);
        }

        LockableStream(LockableStream const& l)
            : m_s(l.m_s)
        {

        }

    public:
        void lock()
        {
            m_gurad.lock();
        }

        void unlock()
        {
            m_gurad.unlock();
        }

        void flush()
        {
             m_s->flush();
        }

        void ColorOn(int16_t index)
        {
            if (has_color_fuc_stream<Stream>::value)
                m_s->ColorOn(index);
        }

        void ColorOff(int16_t index)
        {
            if (has_color_fuc_stream<Stream>::value)
                m_s->ColorOff(index);
        }

        template <typename Text>
        LockableStream& operator<< (Text const& t)
        {
            (*m_s) << t;
            return *this;
        }

        LockableStream& operator = (LockableStream const& r)
        {
            m_s = r.m_s;

            return *this;
        }

    private:
        mutex       m_gurad;
        Stream     *m_s;
    };

    extern LockableStream<ConsoleStream> sg_out;

    template<typename T>
    class is_lockable_stream
    {
    public:
        static const bool value = T::lockable;
    };

    template<>
    class is_lockable_stream<std::ostream>
    {
    public:
        static const bool value = false;
    };

    template<>
    class is_lockable_stream<std::ofstream>
    {
    public:
        static const bool value = false;
    };

    enum LogLevel
    {
        LL_Info = 0,
        LL_Warning,
        LL_Error
    };

    class ConsolePrinter : public Singleton<ConsolePrinter>
    {
    public:
        ConsolePrinter()
            : m_level(LL_Info)
#ifdef CORE_DEBUG
            , m_show_detail(false)
            , m_enable_debug(false)
            , m_ready(false)
#endif
            , m_info_color(-1)
            , m_warning_color(-1)
            , m_error_color(-1)
        {

        }

    public:
        void Init(LogLevel level,
    #ifdef CORE_DEBUG
                  bool detail,
                  bool enable_debug,
    #endif
                  std::string const & prompt);

    public:
        template <typename Stream, typename Text>
        ConsolePrinter& PrintLog(Stream & s, Text const& t, LogLevel level = LL_Info)
        {
            if (is_lockable_stream<Stream>::value)
            {
                return PrintLogLock(s, t, level);
            }
            else
            {
                return PrintLogLockFree(s, t, level);
            }
        }

        template <typename Stream, typename Text>
        ConsolePrinter& Print(Stream &s, Text const& t)
        {
            if (is_lockable_stream<Stream>::value)
            {
                return PrintLock(s, t);
            }
            else
            {
                return PrintLockFree(s, t);
            }
        }

#ifdef CORE_DEBUG
        template <typename Stream, typename Text>
        ConsolePrinter& PrintDebug(Stream & s, Text const& t, std::string const& file, std::string const & function,std::string const& line)
        {
            if (is_lockable_stream<Stream>::value)
            {
                return PrintDebugLock(s, t, file, function,line);
            }
            else
            {
                return PrintDebugLockFree(s, t, file,function, line);
            }
        }
#endif

        template <typename Stream, typename Text>
        ConsolePrinter& PrintLogLock(Stream &s, Text const& t, LogLevel level)
        {
            if (level >= m_level)
            {
                s.lock();
                PrintLogLockFree(s, t, level);
                s.unlock();
            }

            return *this;
        }

        template <typename Stream, typename Text>
        ConsolePrinter& PrintLogLockFree(Stream & s, Text const& t, LogLevel level)
        {
            if (level >= m_level)
            {
                int16_t color = -1;
                if (has_color_fuc_stream<Stream>::value)
                {
                    if (ConsoleStream::sIsColorSupport())
                    {
                        switch (level)
                        {
                        case LL_Info:
                            color = m_info_color;
                            break;
                        case LL_Warning:
                            color = m_warning_color;
                            break;
                        case LL_Error:
                            color = m_error_color;
                            break;
                        default:
                            break;
                        }

                        if (color != -1)
                            s.ColorOn(color);
                    }
                    else
                    {
                        // TODO:
                    }
                }

                s << m_log_prompt << t;

                if (has_color_fuc_stream<Stream>::value)
                {
                    if (ConsoleStream::sIsColorSupport())
                    {
                        if (color != -1)
                            s.ColorOff(color);
                    }
                }
            }

            return *this;
        }

        template <typename Stream, typename Text>
        ConsolePrinter& PrintLock(Stream & s, Text const& t)
        {
            s.lock();
            PrintLockFree(s, t);
            s.unlock();

            return *this;
        }

        template <typename Stream, typename Text>
        ConsolePrinter& PrintLockFree(Stream & s, Text const& t)
        {
            s << t;

            return *this;
        }

#ifdef CORE_DEBUG
        template <typename Stream, typename Text>
        ConsolePrinter& PrintDebugLock(Stream &s, Text const& t, std::string const & file,std::string const & function ,std::string const & line)
        {
            if (m_enable_debug)
            {
                s.lock();
                PrintDebugLockFree(s, t, file, function ,line);
                s.unlock();
            }

            return *this;
        }

        template <typename Stream, typename Text>
        ConsolePrinter& PrintDebugLockFree(Stream &s, Text const& t, std::string const & file, std::string const & function, std::string const & line)
        {
            if (m_enable_debug)
            {
                if (m_show_detail)
                {
                    s << file << " <file> " <<function<<" <function> "<< line << " <line> ";
                }

                s << t;
            }

            return *this;
        }
        bool            IsReady() { return m_ready;}
#endif
    private:
        LogLevel        m_level;
#ifdef CORE_DEBUG
        bool            m_show_detail;
        bool            m_enable_debug;
        bool            m_ready;
#endif
        std::string     m_log_prompt;
        int16_t         m_info_color;
        int16_t         m_warning_color;
        int16_t         m_error_color;
    };

}
// Thread safe output facilties

#define CORE_LOG(l, t)              sg::ConsolePrinter::Instance().PrintLog(sg::sg_out, (t), (l));     \
                                    sg::sg_out.flush()

#define CORE_START_LOG_BLOCK()      sg::sg_out.lock()
#define CORE_LOG_BLOCK(l, t)        sg::ConsolePrinter::Instance().PrintLogLockFree(sg::sg_out, (t), (l))
#define CORE_END_LOG_BLOCK()        sg::sg_out.flush();      \
                                    sg::sg_out.unlock();

#define CORE_PRINT(t)               sg::ConsolePrinter::Instance().Print(sg::sg_out, (t));     \
                                    sg::sg_out.flush()

#define CORE_START_PRINT_BLOCK()    sg::sg_out.lock()
#define CORE_PRINT_BLOCK(t)         sg::ConsolePrinter::Instance().PrintLockFree(sg::sg_out, (t))
#define CORE_END_PRINT_BLOCK()      sg::sg_out.flush();     \
                                    sg::sg_out.unlock()

#define CORE_S_LOG(s, l, t)         sg::ConsolePrinter::Instance().PrintLog((s), (t), (l)); \
                                    s.flush()

#define CORE_S_START_LOG_BLOCK(s)   (s).lock()
#define CORE_S_LOG_BLOCK(s, l, t)   sg::ConsolePrinter::Instance().PrintLogLockFree((s), (t), (l))
#define CORE_S_END_LOG_BLOCK(s)     (s).flush();      \
                                    (s).unlock()

#define CORE_S_PRINT(s, t)          sg::ConsolePrinter::Instance().Print((s), (t))      \
                                    (s).flush()

#define CORE_S_START_PRINT_BLOCK(s) (s).lock()
#define CORE_S_PRINT_BLOCK(s, t)    sg::ConsolePrinter::Instance().PrintLockFree((s), (t))
#define CORE_S_END_PRINT_BLOCK(s)   (s).flush();      \
                                    (s).unlock()

#ifdef CORE_DEBUG

#ifdef CORE_PLATFORM_LINUX
    #define __FILE_NAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#else
    #define __FILE_NAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#endif

#define __CORE_PRINT_INT_TO_CHAR(x) #x
#define _CORE_PRINT_INT_TO_CHAR(x)  __CORE_PRINT_INT_TO_CHAR(x)
#define CORE_PRINT_DEBUG(t)         sg::ConsolePrinter::Instance().PrintDebug(sg::sg_out, (t), std::string(__FILE_NAME__), std::string(__FUNCTION__), std::string(_CORE_PRINT_INT_TO_CHAR(__LINE__))); \
                                    sg::sg_out.flush()

#define CORE_PRINT_DEBUG_NO_BLOCK(t)    sg::ConsolePrinter::Instance().PrintDebugLockFree(sg::sg_out, (t), std::string(__FILE_NAME__), std::string(__FUNCTION__), std::string(_CORE_PRINT_INT_TO_CHAR(__LINE__))); \
                                        sg::sg_out.flush()


#define CORE_START_PDEBUG_BLOCK()   sg::sg_out.lock()
#define CORE_PDEBUG_BLOCK(t)        sg::ConsolePrinter::Instance().PrintDebugLockFree(sg::sg_out, (t), std::string(__FILE_NAME__),std::string(__FUNCTION__), std::string(_CORE_PRINT_INT_TO_CHAR(__LINE__)))
#define CORE_END_PDEBUG_BLOCK()     sg::sg_out.flush();      \
                                    sg::sg_out.unlock();
#else

#define CORE_PRINT_DEBUG(t)
#define CORE_PRINT_DEBUG_NO_BLOCK(t)
#define CORE_START_PDEBUG_BLOCK()
#define CORE_PDEBUG_BLOCK(t)
#define CORE_END_PDEBUG_BLOCK()

#endif


#endif //__SG_CORE_PRINTER__
