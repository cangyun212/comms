#include "Core.hpp"

#include "Console/ConsolePrinter.hpp"

namespace sg
{
    void ConsolePrinter::Init(ConsoleLogLevel level, std::string const & prompt)
    {
        m_level = level;
        m_log_prompt = prompt;

        Console &c = Console::Instance();

        if (c.IsColorSupport())
        {
            ConsoleColorGroup g = { CC_Green, CC_Black };

            m_info_color = c.RegisterColorGroup(g);

            g.f = CC_Yellow;
            m_warning_color = c.RegisterColorGroup(g);

            g.f = CC_Red;
            m_error_color = c.RegisterColorGroup(g);

            m_off_color = c.OffHandle();
        }

        m_init = true;
    }

}
