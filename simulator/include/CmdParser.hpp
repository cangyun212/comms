#ifndef __SG_CMD_PARSER_HPP__
#define __SG_CMD_PARSER_HPP__

#include "Core.hpp"

#include <string>

#include "Utils.hpp"
#include "Singleton.hpp"
#include "Predeclare.hpp"


namespace sg 
{

    class CmdParser : public Singleton<CmdParser>
    {
    public:
        CmdParser();
       ~CmdParser() {}

    public:
        bool    Parse(int argc, char *argv[]);
        void    SetUsage(std::string const& usage) { m_usage = usage; }

    public:
        std::string const&  GetDevPath() const { return m_dev_path; }
        int                 GetLogLevel() const { return m_log_level; }

    private:
        std::string     m_usage;
        std::string     m_dev_path;
        int             m_log_level;

    };

}




#endif



