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
};

}

#endif // CMD_COMPLETION_HPP
