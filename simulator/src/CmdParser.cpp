#include "Core.hpp"

#include <iostream>

#include "CmdParser.hpp"


namespace po = boost::program_options;

namespace sg 
{

    CmdParser::CmdParser()
        : m_desc("comms simulator")
        , m_visiable("comms simulator")
        , m_usage("Usage: simulator [OPTIONS]")
        , m_log_level(0)
    {
        this->SetupOptions();
    }

    void CmdParser::SetupOptions()
    {
       m_desc.add_options()
               ("help", "")
               ("dev,d", po::value<std::string>(&m_dev_path), "")
               ("log,l", po::value<int>(&m_log_level)->default_value(0) ,"") // TODO : use string instead of num
               ("dummy", "");

       m_visiable.add_options()
           ("help",  "show help message")
           ("dev,d", "set the master terminal device path")
           ("log,l", "set the log level");

       m_pos_desc.add("dummy", -1);

    }

    bool CmdParser::Parse(int argc, char *argv[])
    {
        bool res = false;

        m_vm.clear();

        po::store(po::command_line_parser(argc, argv).options(m_desc).positional(m_pos_desc).allow_unregistered().run(), m_vm);
        po::notify(m_vm);

        res = true;

        return res;
    }

    void CmdParser::ShowHelp() const
    {
        // TODO : format output
        std::cout<< m_usage << std::endl;
        std::cout<< m_visiable << std::endl;
    }

}


