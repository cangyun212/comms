#include "Core.hpp"

#include "SimUtils.hpp"
#include "CmdCompletion.hpp"


namespace po = boost::program_options;

namespace sg 
{

    std::string::size_type CmdCompletion::doCmdCompletion(std::string cmdStr, std::vector<std::string> &retVecStrings)
    {
        std::vector<std::string> vecStrings;
        std::string::size_type pos = std::string::npos;

        if (cmdStr.empty())
            return pos;

        if (!g_fac->Parse(cmdStr, vecStrings))
            return pos;


        if (vecStrings.size() == 1)
        {
            ActionPtr action = g_fac->CreateAction(vecStrings.front());

            if (action)
            {
                action->GetAllOptionsName(retVecStrings);
            }
            else
            {
                g_fac->MatchActions(vecStrings.front(), retVecStrings);

                if (retVecStrings.size() == 1)
                    pos = vecStrings.front().size();
            }
        }
        else
        {
            if (vecStrings.back().compare(0, 2, "--") == 0 || vecStrings.back().compare(0, 1, "-"))
            {
                ActionPtr action = g_fac->CreateAction(vecStrings.front());

                if (action)
                {
                    ActionParsedOptions ops;
                    if (action->TryParse(vecStrings, ops))
                    {
                        if (!ops.empty())
                        {
                            for (auto const& op : ops)
                            {
                                if (!op.key.empty() && op.token == vecStrings.back())
                                {
                                    retVecStrings.push_back(op.key);
                                    vecStrings.back().erase(0, 2);
                                    pos = op.key.find(vecStrings.back()) + vecStrings.back().size();
                                    break;
                                }
                            }
                        }
                    }
                    else
                    {
                        if (ops.size() == 1)
                        {
                            if (ops.front().key.size() > vecStrings.back().size())
                            {
                                retVecStrings.push_back(ops.front().key);
                                pos = ops.front().key.find(vecStrings.back()) + vecStrings.back().size();
                            }
                        }
                        else
                        {
                            for (auto const& op : ops)
                            {
                                retVecStrings.push_back(op.key);
                            }
                        }
                    }
                }
            }
        }

        return pos;
    }

}
