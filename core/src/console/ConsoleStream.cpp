#include "Core.hpp"
#include "Console/ConsoleStream.hpp"

namespace sg
{
    ConsoleWindowStream     g_winstream;
    ConsoleStdColorStream   g_colstream;

    void ConsoleWindowStream::flush()
    {
        m_wnd->Render();
    }

    void ConsoleStdColorStream::flush()
    {
        std::cout.flush();
    }

}




