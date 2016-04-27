#ifndef __SG_COMMS_PREDECLARE_HPP__
#define __SG_COMMS_PREDECLARE_HPP__

#include "Core.hpp"

#ifdef SG_HAS_DECLSPEC
    #ifdef COMMS_SOURCE
        #define COMMS_API       SG_EXPORT
    #else
        #define COMMS_API       SG_IMPORT
    #endif
#else
    #define COMMS_API
#endif

// for QCOM
typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;

#ifndef FALSE
    #define FALSE 0
    #define TRUE !FALSE
#endif

#ifdef SG_COMPILER_MSVC
    #define __inline__      __inline
#endif

#ifdef SG_PLATFORM_LINUX
    #define COMMS_HAS_XTIMER_R
#endif // SG_PLATFORM_LINUX

#ifndef COMMS_HAS_XTIMER_R
    #include <time.h>
    inline struct tm * localtime_r(time_t *_clock, struct tm * _result)
    {
        struct tm *p = localtime(_clock);
        if (p)
            *(_result) = *p;

        return _result;
    }
#endif // !COMMS_HAS_XTIMER_R


namespace sg {

    class Comms;
    class CommsPacketHandler;
    class CommsQcom;


    typedef std::shared_ptr<Comms>              CommsPtr;
    typedef std::shared_ptr<CommsPacketHandler> CommsPacketHandlerPtr;
    typedef std::shared_ptr<CommsQcom>          CommsQcomPtr;

}


#endif
