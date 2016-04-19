#include "core/core.hpp"
#include "core/core_utils.hpp"
#include "core/console/core_console_printer.hpp"
#include "core/core_console.hpp"
#include "core/core_debug.hpp"
#include <string>

#include "simulator/predeclare.hpp"

#include "simulator/action_center.hpp"
#include "simulator/action.hpp"
#include "simulator/action_factory.hpp"
#include "simulator/simulator.hpp"
#include "simulator/utils.hpp"
#include "simulator/cmd_parser.hpp"
#include "simulator/line_reader.hpp"
#include <ncurses.h>
#include <string.h>
#include <ctype.h>

#include <string>
#include <vector>
#include <list>


int main(int argc, char *argv[])
{
#ifdef CORE_DEBUG
    setup_sig_segv();
#endif

    int ret = 0;

    sg::CmdParser::Instance().Parse(argc, argv);

#ifdef CORE_DEBUG
    if (sg::CmdParser::Instance().UseCustomWin())
    {
#endif
        sg::Console::Instance().Init(true); // must be called first

        sg::ConsoleWindow *p = sg::Console::Instance().GetActiveOutputWnd();
        uint32_t width = p->Width();
        uint32_t height = p->Height();

        sg::ConsoleWindow *input = sg::Console::Instance().MakeWindow(std::string("input"), width, 4, 0, height -4);
        sg::ConsoleWindow *output = sg::Console::Instance().MakeWindow(std::string("output"), width, height - 4, 0, 0);
        sg::Console::Instance().SetActiveOutputWnd(output); // must be set before printer init if you don't like the default one
        input->Border(sg::ConsoleWindow::BS_TB, '=', '\0', '\0', '\0');

        sg::LineReader::Instance().Init(input);
#ifdef CORE_DEBUG
    }
    else
    {
        sg::LineReader::Instance().Init(nullptr);
    }
#endif

    sg::ConsolePrinter::Instance().Init((sg::LogLevel)sg::CmdParser::Instance().GetLogLevel(),
#ifdef CORE_DEBUG
                      sg::CmdParser::Instance().ShowDebugOutputDetail(),
                      sg::CmdParser::Instance().EnableDebugOutput(),
#endif
                      "Sim>> "
                      );

    sg::SimulatorPtr sim = sg::setup_sim();
    sg::ActionFactoryPtr fac = sg::setup_action_factory();
    sg::ActionCenter& center = sg::ActionCenter::Instance();

    sim->Init();
    fac->Init();


    std::string line;
    sg::Action::ActionArgs args;
    args.reserve(10);
    while(!sg::quit_sim())
    {
        args.clear();
        if (sim->IsReady())
        {
            line = sg::LineReader::Instance().ReadLine();

            if (fac->Parse(line, args))
            {
                if (fac->IsValidAction(args.front()))
                {
                    sg::ActionPtr action = fac->CreateAction(args.front());
                    if (action->Parse(args))
                    {
                        center.SendAction(action);
                    }
                }
            }
        }
    }

    return ret;

}


