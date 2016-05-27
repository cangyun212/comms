#include "Core.hpp"

#include <string>
#include <vector>
#include <list>

#include "Utils.hpp"
#include "Console.hpp"
#include "Console/ConsolePrinter.hpp"
#include "Console/ConsoleWindow.hpp"

#include "Predeclare.hpp"
#include "ActionCenter.hpp"
#include "Action.hpp"
#include "ActionFactory.hpp"
#include "Simulator.hpp"
#include "SimUtils.hpp"
#include "CmdParser.hpp"
#include "LineReader.hpp"



int main(int argc, char *argv[])
{
    int ret = 0;

    if (!sg::CmdParser::Instance().Parse(argc, argv)) // console is not ready, use std::cout instead
    {
        ret = 1;
        return ret;
    }

    sg::Console::Instance().Init(sg::CT_Custom); // must be called first

    sg::ConsoleWindowPtr p = sg::Console::Instance().GetActiveOutputWnd();
    sg::uint width = p->Width();
    sg::uint height = p->Height();

    sg::ConsoleWindowPtr input = sg::Console::Instance().MakeWindow(std::string("input"), width, 4, 0, height - 4);
    sg::ConsoleWindowPtr output = sg::Console::Instance().MakeWindow(std::string("output"), width, height - 4, 0, 0);
    sg::Console::Instance().SetActiveOutputWnd(output); // must be set before printer init if you don't like the default one
    input->Border(sg::ConsoleWindow::BS_TB, '=', '\0', '\0', '\0');

    sg::LineReader::Instance().Init(input);

    sg::ConsolePrinter::Instance().Init((sg::ConsoleLogLevel)sg::CmdParser::Instance().GetLogLevel(), "Sim>> ");

    sg::setup_sim();
    sg::setup_action_factory();
    sg::ActionCenter& center = sg::ActionCenter::Instance();

    sg::g_sim->Init();
    sg::g_fac->Init();

    std::string line;
    sg::Action::ActionArgs args;
    args.reserve(10);

    while(!sg::quit_sim())
    {
        args.clear();
        if (sg::g_sim->IsReady())
        {
            line = sg::LineReader::Instance().ReadLine();

            if (sg::g_fac->Parse(line, args))
            {
                sg::ActionPtr action = sg::g_fac->CreateAction(args.front());
                if (action)
                {
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


