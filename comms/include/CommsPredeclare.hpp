#ifndef __SG_COMMS_PREDECLARE_HPP__
#define __SG_COMMS_PREDECLARE_HPP__

#include "core/core.hpp"

#ifdef CORE_HAS_DECLSPEC
    #ifdef COMMS_SOURCE
        #define COMMS_API       __declspec(dllexport)
    #else
        #define COMMS_API       __declspec(dllimport)
    #endif
#else
    #define COMMS_API
#endif

// for QCOM
typedef sg::uint8_t     u8;
typedef sg::uint16_t    u16;
typedef sg::uint32_t    u32;

#ifndef FALSE
    #define FALSE 0
    #define TRUE !FALSE
#endif

namespace sg {

    class Comms;
    class CommsPacketHandler;
    class CommsQcom;


    typedef shared_ptr<Comms>   CommsPtr;
    typedef shared_ptr<CommsPacketHandler>  CommsPacketHandlerPtr;
    typedef shared_ptr<CommsQcom>   CommsQcomPtr;

}


#endif
