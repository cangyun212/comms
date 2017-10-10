#ifndef __SG_CORE_CONSOLE_PRINTER__
#define __SG_CORE_CONSOLE_PRINTER__

#include "Core.hpp"

#include <mutex>
#include <iostream>

#ifdef SG_DEBUG
#include "boost/filesystem.hpp"
#endif

#include "Singleton.hpp"
#include "Console/ConsoleStream.hpp"

namespace sg
{
    extern CORE_API std::mutex               g_log_guard;


    enum ConsoleLogLevel
    {
        CLL_Info = 0,
        CLL_Warning,
        CLL_Error
    };

    class CORE_API ConsolePrinter : public Singleton<ConsolePrinter>
    {
    public:
        ConsolePrinter()
            : m_level(CLL_Info)
            , m_init(false)
        {

        }

    public:
        void Init(ConsoleLogLevel level, std::string const & prompt);

    public:
        template <typename Stream, typename Text>
        ConsolePrinter& SafeLog(Stream &s, Text const& t, ConsoleLogLevel level = CLL_Info)
        {
            SG_ASSERT(m_init);
            
            if (level >= m_level)
            {
                std::lock_guard<std::mutex> lock(g_log_guard);
                return this->Log(s, t, level);
            }

            return *this;
        }

        template <typename Stream, typename Text>
        ConsolePrinter& Log(Stream &s, Text const& t, ConsoleLogLevel level = CLL_Info)
        {
            SG_ASSERT(m_init);

            if (level >= m_level)
            {
                bool color = false;
                if (has_color_fuc_stream<Stream>::value)
                {
                    color = Console::Instance().IsColorSupport();
                    if (color)
                    {
                        switch (level)
                        {
                        case sg::CLL_Info:
                            s << m_info_color;
                            break;
                        case sg::CLL_Warning:
                            s << m_warning_color;
                            break;
                        case sg::CLL_Error:
                            s << m_error_color;
                            break;
                        default:
                            SG_ASSERT(false);
                            break;
                        }
                    }
                }

                if (!color)
                {
                    // TODO:
                }

                s << m_log_prompt << t;

                if (color)
                {
                    s << m_off_color;
                }
            }

            return *this;
        }

        template <typename Stream, typename Text>
        ConsolePrinter& SafePrint(Stream &s, Text const& t)
        {
            SG_ASSERT(m_init);
            std::lock_guard<std::mutex> lock(g_log_guard);
            return this->Print(s, t);
        }

        template <typename Stream, typename Text>
        ConsolePrinter& Print(Stream &s, Text const& t)
        {
            SG_ASSERT(m_init);
            s << t;
            return *this;
        }

#ifdef SG_DEBUG
        template <typename Stream, typename Text>
        ConsolePrinter& SafeDebug(Stream & s, Text const& t, 
            std::string const& file, 
            std::string const & function,
            std::string const& line)
        {
            SG_ASSERT(m_init);
            std::lock_guard<std::mutex> lock(g_log_guard);
            return this->Debug(s, t, file, function, line);
        }

        template <typename Stream, typename Text>
        ConsolePrinter& Debug(Stream & s, Text const& t, 
            std::string const& file,
            std::string const& function, 
            std::string const& line)
        {
            SG_ASSERT(m_init);
            boost::filesystem::path p(file);
            s << p.filename().string() << "(" << line << "): " << function << ": " << t;
            return *this;
        }
#endif

        bool    IsInit() const { return m_init; }

    private:
        ConsoleLogLevel     m_level;
        bool                m_init;

        std::string         m_log_prompt;
        ConsoleColorHandle  m_info_color;
        ConsoleColorHandle  m_warning_color;
        ConsoleColorHandle  m_error_color;
        ConsoleColorHandle  m_off_color;
    };

}
// Thread safe output facilties

// TODO : shall we do some extra work to make sure ConsoleWindow::Render is thread safe.
#define SG_SAFE_LOG(s, t, l)        sg::ConsolePrinter::Instance().SafeLog(s, t, l);\
                                    s.flush()

#define SG_START_LOG_BLOCK(s)       {   std::lock_guard<std::mutex> __lock(g_log_guard)
#define SG_LOG_BLOCK(s, t, l)           sg::ConsolePrinter::Instance().Log(s, t, l)
#define SG_END_LOG_BLOCK(s)             s.flush();}

#define SG_LOG(s, t, l)             sg::ConsolePrinter::Instance().Log(s, t, l)

#define SG_SAFE_PRINT(s, t)         sg::ConsolePrinter::Instance().SafePrint(s, t);\
                                    s.flush()

#define SG_START_PRINT_BLOCK(s)     {   std::lock_guard<std::mutex> __lock(g_log_guard)
#define SG_PRINT_BLOCK(s, t)            sg::ConsolePrinter::Instance().Print(s, t)
#define SG_END_PRINT_BLOCK(s)           s.flush();}

#define SG_PRINT(s, t)              sg::ConsolePrinter::Instance().Print(s, t)

#define SG_WS_SAFE_LOG(t, l)        SG_SAFE_LOG(g_winstream, t, l)

#define SG_START_WS_LOG_BLOCK()     SG_START_LOG_BLOCK(g_winstream)
#define SG_WS_LOG_BLOCK(t, l)       SG_LOG_BLOCK(g_winstream, t, l)
#define SG_END_WS_LOG_BLOCK()       SG_END_LOG_BLOCK(g_winstream)

#define SG_WS_LOG(t, l)             SG_LOG(g_winstream, t, l)

#define SG_WS_SAFE_PRINT(t)         SG_SAFE_PRINT(g_winstream, t)

#define SG_START_WS_PRINT_BLOCK()   SG_START_PRINT_BLOCK(g_winstream)
#define SG_WS_PRINT_BLOCK(t)        SG_PRINT_BLOCK(g_winstream, t)
#define SG_END_WS_PRINT_BLOCK()     SG_END_PRINT_BLOCK(g_winstream)

#define SG_WS_PRINT(t)              SG_PRINT(g_winstream, t)

#define SG_CS_SAFE_LOG(t, l)        SG_SAFE_LOG(g_colstream, t, l)

#define SG_START_CS_LOG_BLOCK()     SG_START_LOG_BLOCK(g_colstream)
#define SG_SS_LOG_BLOCK(t, l)       SG_LOG_BLOCK(g_colstream, t, l)
#define SG_END_CS_LOG_BLOCK()       SG_END_LOG_BLOCK(g_colstream)

#define SG_CS_LOG(t, l)             SG_LOG(g_colstream, t, l)

#define SG_CS_SAFE_PRINT(t)         SG_SAFE_PRINT(g_colstream, t)

#define SG_START_CS_PRINT_BLOCK()   SG_START_PRINT_BLOCK(g_colstream)
#define SG_CS_PRINT_BLOCK(t)        SG_PRINT_BLOCK(g_colstream, t)
#define SG_END_CS_PRINT_BLOCK()     SG_END_PRINT_BLOCK(g_colstream)

#define SG_CS_PRINT(t)              SG_PRINT(g_colstream, t)

#define SG_STD_SAFE_LOG(t, l)       SG_SAFE_LOG(std::cout, t, l)

#define SG_START_STD_LOG_BLOCK()    SG_START_LOG_BLOCK(std::cout)
#define SG_STD_LOG_BLOCK(t, l)      SG_LOG_BLOCK(std::cout, t, l)
#define SG_END_STD_LOG_BLOCK()      SG_END_LOG_BLOCK(std::cout)

#define SG_STD_LOG(t, l)            SG_LOG(std::cout, t, l)

#define SG_STD_SAFE_PRINT(t)        SG_SAFE_PRINT(std::cout, t)

#define SG_START_STD_PRINT_BLOCK()  SG_START_PRINT_BLOCK(std::cout)
#define SG_STD_PRINT_BLOCK(t)       SG_PRINT_BLOCK(std::cout, t)
#define SG_END_STD_PRINT_BLOCK()    SG_END_PRINT_BLOCK(std::cout)

#define SG_STD_PRINT(t)             SG_PRINT(std::cout, t)

#ifdef SG_DEBUG

#define __SG_PRINT_I2C(x)           #x
#define _SG_PRINT_I2C(x)            __SG_PRINT_I2C(x)

#define SG_SAFE_PRINT_DEBUG(s, t)   sg::ConsolePrinter::Instance().SafeDebug(s, t, std::string(__FILE__), std::string(__FUNCTION__), std::string(_SG_PRINT_I2C(__LINE__)));\
                                    s.flush()

#define SG_START_DEBUG_BLOCK(s)     {   std::lock_guard<std::mutex> __lock(g_log_guard)
#define SG_DEBUG_BLOCK(s, t)            sg::ConsolePrinter::Instance().Debug(s, t, std::string(__FILE__), std::string(__FUNCTION__), std::string(_SG_PRINT_I2C(__LINE__)));
#define SG_END_DEBUG_BLOCK(s)           s.flush();}

#define SG_PRINT_DEBUG(s, t)        sg::ConsolePrinter::Instance().Debug(s, t, std::string(__FILE__), std::string(__FUNCTION__), std::string(_SG_PRINT_I2C(__LINE__)))

#define SG_WS_SAFE_PRINT_DEBUG(t)   SG_SAFE_PRINT_DEBUG(g_winstream, t)

#define SG_START_WS_DEBUG_BLOCK()   SG_START_DEBUG_BLOCK(g_winstream)
#define SG_WS_DEBUG_BLOCK(t)        SG_DEBUG_BLOCK(g_winstream, t)
#define SG_END_WS_DEBUG_BLOCK()     SG_END_DEBUG_BLOCK(g_winstream)

#define SG_WS_PRINT_DEBUG(t)        SG_PRINT_DEBUG(g_winstream, t)

#define SG_CS_SAFE_PRINT_DEBUG(t)   SG_SAFE_PRINT_DEBUG(g_colstream, t)

#define SG_START_CS_DEBUG_BLOCK()   SG_START_DEBUG_BLOCK(g_colstream)
#define SG_CS_DEBUG_BLOCK(t)        SG_DEBUG_BLOCK(g_colstream, t)
#define SG_END_CS_DEBUG_BLOCK()     SG_END_DEBUG_BLOCK(g_colstream)

#define SG_CS_PRINT_DEBUG(t)        SG_PRINT_DEBUG(g_colstream, t)

#define SG_STD_SAFE_PRINT_DEBUG(t)  SG_SAFE_PRINT_DEBUG(std::cout, t)

#define SG_START_STD_DEBUG_BLOCK()  SG_START_DEBUG_BLOCK(std::cout)
#define SG_STD_DEBUG_BLOCK(t)       SG_DEBUG_BLOCK(std::cout, t)
#define SG_END_STD_DEBUG_BLOCK()    SG_END_DEBUG_BLOCK(std::cout)

#define SG_STD_PRINT_DEBUG(t)       SG_PRINT_DEBUG(std::cout, t)

#else

#define SG_SAFE_DEBUG(s, t)
#define SG_START_DEBUG_BLOCK()
#define SG_DEBUG_BLOCK(s, t)
#define SG_END_DEBUG_BLOCK()
#define SG_DEBUG(s, t)
#define SG_WS_SAFE_DEBUG(t)
#define SG_START_WS_DEBUG_BLOCK()
#define SG_WS_DEBUG_BLOCK(t)
#define SG_END_WS_DEBUG_BLOCK()
#define SG_WS_DEBUG(t)
#define SG_CS_SAFE_DEBUG(t)
#define SG_START_CS_DEBUG_BLOCK()
#define SG_CS_DEBUG_BLOCK(t)
#define SG_END_CS_DEBUG_BLOCK()
#define SG_CS_DEBUG(t)
#define SG_STD_SAFE_DEBUG(t)
#define SG_START_STD_DEBUG_BLOCK()
#define SG_STD_DEBUG_BLOCK(t)
#define SG_END_STD_DEBUG_BLOCK()
#define SG_STD_DEBUG(t)

#endif



#endif //__SG_CORE_PRINTER__
