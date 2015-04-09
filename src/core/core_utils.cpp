
#include "core/core.hpp"
#include "core/core_utils.hpp"

#ifdef CORE_PLATFORM_LINUX
    #include <cerrno>
#endif


namespace sg{

    uint32_t LastError()
    {
#ifdef CORE_PLATFORM_LINUX
        return errno;
#endif
    }
}

