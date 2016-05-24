#ifndef CMD_COMPLETION_HPP
#define CMD_COMPLETION_HPP

#include "Core.hpp"

#include <string>
#include <vector>
#include <unordered_map>


#include "Utils.hpp"
#include "Singleton.hpp"

#include "Action.hpp"

namespace sg {

class CmdCompletion : public Singleton<CmdCompletion>
{
public:
    CmdCompletion(){}
    ~CmdCompletion(){}

    std::string::size_type doCmdCompletion(std::string cmdStr, std::vector<std::string> &retVecStrings);
    void initCommand(std::string const &command, ActionOptionsPtr const &options = nullptr);

private:
    std::string::size_type matchCommand(std::string const &command, std::vector<std::string> &matchVecCommands);
    std::string getLongOption(std::string const &option);


private:
    typedef std::unordered_map<std::string, ActionOptionsPtr> CmdMap;

    CmdMap m_commands;
};

}

#endif // CMD_COMPLETION_HPP
