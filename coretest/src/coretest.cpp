
#include "coretest.hpp"

#include <iostream>

int main(int argc, char **argv) 
{
    ::testing::InitGoogleTest(&argc, argv);

    std::cout << "Unit test for Core library " << SG_CORE_VERSION_STRING << std::endl;

    int ret = RUN_ALL_TESTS();

    return ret;
}
