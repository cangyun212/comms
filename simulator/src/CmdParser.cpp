#include "Core.hpp"

#include <iostream>

#include "boost/program_options.hpp"

#include "CmdParser.hpp"


namespace po = boost::program_options;
namespace nm = boost::numeric;

namespace sg 
{

    CmdParser::CmdParser()
        : m_usage("Usage: simulator [OPTIONS]")
        , m_log_level(0)
    {
    }

    bool CmdParser::Parse(int argc, char *argv[])
    {
        bool res = false;

        po::variables_map vm;
        po::options_description desc("comms simulator");
        po::options_description vis_desc("comms simulator");

        desc.add_options()
            ("help,h", "")
            ("dev,d", po::value<std::string>(&m_dev_path), "")
            ("log,l", po::value<int>(&m_log_level)->default_value(0), ""); // TODO : use string instead of num

        vis_desc.add_options()
            ("help,h", "show help message")
            ("dev,d", "set the master terminal device path")
            ("log,l", "set the log level");

        try
        {
            po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
            po::notify(vm);
        }
        catch (po::error const& error)
        {
            std::cout << error.what();
            return false;
        }
        catch (nm::bad_numeric_cast const&)
        {
            std::cout << "Option value is out of range" << std::endl;
            return false;
        }
        catch (boost::bad_lexical_cast const&)
        {
            std::cout << "Invalid option value" << std::endl;
            return false;
        }

        if (vm.count("help"))
        {

            std::cout << m_usage << std::endl;
            std::cout << vis_desc << std::endl;
            res = false;
        }
        else
        {
            res = true;
        }

        return res;
    }

}


