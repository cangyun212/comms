#include "Core.hpp"

#include "boost/algorithm/string.hpp"
#include "boost/program_options.hpp"

#include "CmdCompletion.hpp"


namespace po = boost::program_options;

namespace sg 
{

    std::string::size_type CmdCompletion::doCmdCompletion(std::string cmdStr, std::vector<std::string> &retVecStrings)
    {
        std::vector<std::string> vecStrings;
        std::string::size_type pos = std::string::npos;

        boost::algorithm::trim_left(cmdStr);
        boost::algorithm::split(vecStrings, cmdStr, boost::is_any_of(" "));

        if (vecStrings.empty())
            return pos;

        std::string const& cmd = vecStrings[0];
        auto it = m_commands.find(cmd);
        if (vecStrings.size() == 1)
        {
            if (it != m_commands.end())
            {
                if (it->second != nullptr)
                {
                    for (auto const& option : *(it->second))
                    {
                        retVecStrings.push_back(this->getLongOption(option->name));
                    }
                }
            }
            else
            {
                return this->matchCommand(cmd, retVecStrings);
            }
        }
        else
        {
            if (it != m_commands.end() && it->second != nullptr)
            {
                std::vector<std::string> options;
                options.assign(vecStrings.begin() + 1, vecStrings.end());

                po::options_description desc;

                for (auto const& option : *(it->second))
                {
                    if (option->value == nullptr)
                    {
                        desc.add_options()
                            (option->name.c_str(), "");
                    }
                    else
                    {
                        desc.add_options()
                            (option->name.c_str(), po::value<std::string>(), "");
                    }
                }

                std::vector<po::option> resOptions;
                try
                {
                    resOptions = po::command_line_parser(options).options(desc).run().options;
                }
                catch (po::ambiguous_option const &error)
                {
                    retVecStrings.assign(error.alternatives().begin(), error.alternatives().end());
                }
                catch (po::invalid_command_line_syntax const & error)
                {
                    std::string option = error.get_option_name();

                    std::string const &lastStr = vecStrings.back();

                    if (option.size() > lastStr.size())
                    {
                        retVecStrings.push_back(option);

                        pos = option.find(lastStr) + lastStr.size();
                    }

                }
                catch (po::error const&)
                {
                    //do nothing
                }

                if (retVecStrings.empty() && !resOptions.empty())
                {
                    std::string lastStr = vecStrings.back();

                    for (po::option const &option : resOptions)
                    {
                        if (!option.string_key.empty() && (option.original_tokens.size() == 1) && (option.original_tokens.back() == lastStr))
                        {
                            retVecStrings.push_back(option.string_key);
                        }
                    }

                    if (retVecStrings.size() == 1)
                    {
                        std::string const &option = retVecStrings.back();

                        lastStr.erase(0, 2);

                        pos = option.find(lastStr) + lastStr.size();
                    }
                }

            }
        }

        return pos;
    }

    void CmdCompletion::initCommand(const std::string &command, ActionOptionsPtr const &options)
    {
        if (!command.empty())
        {
            m_commands[command] = options;
        }
    }

    std::string CmdCompletion::getLongOption(const std::string &option)
    {
        std::string ret;
        if (!option.empty())
        {
            std::string::size_type pos = option.find(',');

            if (pos != std::string::npos)
            {
                ret.assign(option, 0, pos);
            }
            else
            {
                ret = option;
            }
        }

        return ret;
    }

    std::string::size_type CmdCompletion::matchCommand(const std::string &command, std::vector<std::string> &retVecCommands)
    {
        std::string::size_type pos = std::string::npos;

        for (auto const& it : m_commands)
        {
            std::string const &tmpStr = it.first;

            if (tmpStr.compare(0, command.size(), command.c_str()) == 0)
            {
                retVecCommands.push_back(tmpStr);
            }
        }

        if (retVecCommands.size() == 1)
        {
            pos = command.size();
        }

        return pos;
    }

}
