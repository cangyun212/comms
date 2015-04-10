#include "core/core.hpp" 

#include "simulator/action.hpp"
#include <vector>
#include <string>

#include "unittestpp/unittestpp.h"

TEST(PickEGMAction)
{
    std::vector<std::string> argv ;
    argv.push_back("pick");
    argv.push_back("--egm");
    argv.push_back("4");
    sg::PickEGMAction test;

    test.Parse(argv, NULL);
    CHECK_EQUAL( 4 , test.Target() ); 
}
