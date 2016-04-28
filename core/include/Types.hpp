#ifndef __CORE_TYPES_HPP__
#define __CORE_TYPES_HPP__

#include <limits>
#include <complex>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <boost/assert.hpp>

namespace sg
{
    typedef unsigned int                            uint;
    typedef unsigned long                           ulong;

#ifdef SG_COMPILER_MSVC
    typedef ulong                                   cwctype;
#elif defined(SG_COMPILER_GCC)
#ifdef SG_CPU_X64
    typedef uint                                    cwctype;
#else
    typedef ulong                                   cwctype;
#endif // SG_CPU_X64
#endif

    class Window;
    class ConsoleWindow;
    class ConsoleWindowStream;
    class ConsoleStdColorStream;


    typedef std::shared_ptr<Window>                 WindowPtr;
    typedef std::shared_ptr<ConsoleWindow>          ConsoleWindowPtr;
    typedef std::shared_ptr<ConsoleWindowStream>    ConsoleWindowStreamPtr;
    typedef std::shared_ptr<ConsoleStdColorStream>  ConsoleStdColorStreamPtr;
}

#endif
