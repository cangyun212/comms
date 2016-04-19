#ifndef __CMD_PARSER_HPP__
#define __CMD_PARSER_HPP__

#include "core/core.hpp"

#include <string>
#include "boost/program_options.hpp"

#include "core/core_utils.hpp"
#include "core/core_singleton.hpp"


#include "simulator/predeclare.hpp"


namespace sg {

    class CmdParser : public Singleton<CmdParser>
    {
    public:
        CmdParser();
       ~CmdParser() {}

    public:
        void    Parse(int argc, char *argv[]);
        void    ShowHelp() const;
        void    SetUsage(std::string const& usage) { m_usage = usage; }

    public:
        std::string const&  GetDevPath() const { return m_dev_path; }
        int                 GetLogLevel() const { return m_log_level; }
#ifdef CORE_DEBUG
        bool                EnableDebugOutput() const { return m_enable_debug; }
        bool                ShowDebugOutputDetail() const { return m_show_detail; }
        bool                UseCustomWin() const { return m_custom_win; }
#endif

    private:
        void    SetupOptions();

    private:
        boost::program_options::options_description     m_desc;
        boost::program_options::options_description     m_visiable;
        boost::program_options::positional_options_description  m_pos_desc;

        boost::program_options::variables_map   m_vm;

    private:
        std::string     m_usage;
        std::string     m_dev_path;
        int             m_log_level;
#ifdef CORE_DEBUG
        bool            m_enable_debug;
        bool            m_show_detail;
        bool            m_custom_win;
#endif
    };

}




#endif



