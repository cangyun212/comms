
#ifndef __SG_CORETEST_HPP__
#define __SG_CORETEST_HPP__

#include "Core.hpp"
#ifdef SG_COMPILER_GCC
    SG_PUSH_WARN
    SG_DISABLE_WARN(SG_WARN_SIGN_COMPARE)
#endif
#include "gtest/gtest.h"
#ifdef SG_COMPILER_GCC
    SG_POP_WARN
#endif


#endif
