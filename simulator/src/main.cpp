#include "core.hpp"

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
        sg::CmdParser::Instance().ShowHelp();
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


