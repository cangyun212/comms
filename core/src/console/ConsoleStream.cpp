#include "Core.hpp"
#include "Console/ConsoleStream.hpp"

namespace sg
{
    // a known vc bug
#ifdef SG_COMPILER_MSVC
    SG_PUSH_WARN
    SG_DISABLE_WARN(SG_WARN_DYNA_INITIALIZE)
#endif
    ConsoleWindowStream     g_winstream;
    ConsoleStdColorStream   g_colstream;
#ifdef SG_COMPILER_MSVC
    SG_POP_WARN
#endif

    void ConsoleWindowStream::flush()
    {
        m_wnd->Render();
    }

    void ConsoleStdColorStream::flush()
    {
        std::cout.flush();
    }

}




