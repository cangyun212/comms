#include "core/core.hpp"

#include <iostream>

#include "simulator/cmd_parser.hpp"


namespace po = boost::program_options;

namespace sg {

    CmdParser::CmdParser()
        : m_desc("comms simulator")
        , m_visiable("comms simulator")
        , m_usage("Usage: simulator [OPTIONS]")
        , m_log_level(0)
#ifdef CORE_DEBUG
        , m_enable_debug(true)
        , m_show_detail(true)
        , m_custom_win(true)
#endif
    {
        this->SetupOptions();
    }

    void CmdParser::SetupOptions()
    {
       m_desc.add_options()
               ("help", "")
               ("dev,d", po::value<std::string>(&m_dev_path), "")
               ("log,l",po::value<int>(&m_log_level)->default_value(0) ,"") // TODO : use string instead of num
#ifdef CORE_DEBUG
               ("debug-no","")
               ("debug-simple","")
               ("custom-no", "")
#endif
               ("dummy", "");

       m_visiable.add_options()
           ("help",  "show help message")
           ("dev,d", "set the master terminal device path")
#ifdef CORE_DEBUG
           ("debug-no","disable debug output")
           ("debug-simple","prevent showing file detail for debug output")
           ("custom-no", "disable using ncurses custom window")
#endif
           ("log,l", "set the log level");

       m_pos_desc.add("dummy", -1);

    }

    void CmdParser::Parse(int argc, char *argv[])
    {
        m_vm.clear();

        po::store(po::command_line_parser(argc, argv).options(m_desc).positional(m_pos_desc).allow_unregistered().run(), m_vm);
        po::notify(m_vm);
#ifdef CORE_DEBUG
        if(m_vm.count("debug-no"))
        {
            m_enable_debug = false;
        }
        else if(m_vm.count("debug-simple"))
        {
            m_show_detail = false;
        }

        if (m_vm.count("custom-no"))
        {
            m_custom_win = false;
        }
#endif
    }

    void CmdParser::ShowHelp() const
    {
        // TODO : format output
        std::cout<<m_usage<<std::endl;
        std::cout<<m_visiable<<std::endl;
    }

}


