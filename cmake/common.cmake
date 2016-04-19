if(__SG_COMMON_CMAKE__)
    return()
endif()
set(__SG_COMMON_CMAKE__)

#===============================================================================
# set SG_COMPILER_NAME and SG_COMPILER_VERSION
if((NOT SG_COMPILER_NAME) OR (NOT SG_COMPILER_VERSION))
    if(MSVC)
        set(SG_COMPILER_NAME "vc")
        if(MSVC_VERSION EQUAL 1900)
            set(SG_COMPILER_VERSION "140")
        elseif(MSVC_VERSION EQUAL 1800)
            set(SG_COMPILER_VERSION "120")
        endif()
    else()
        if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
            set(SG_COMPILER_NAME "gcc")
            execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
            string(REGEX MATCHALL "[0-9]+" GCC_VERSION_COMPONENTS ${GCC_VERSION})
            list(GET GCC_VERSION_COMPONENTS 0 GCC_MAJOR)
            list(GET GCC_VERSION_COMPONENTS 1 GCC_MINOR)
            set(SG_COMPILER_VERSION ${GCC_MAJOR}${GCC_MINOR})
        endif()
    endif()
endif()

# set compiler flag
if (SG_COMPILER_NAME MATCHES "vc")
    set(SG_COMPILER_MSVC TRUE)
elseif(SG_COMPILER_NAME MATCHES "gcc")
    set(SG_COMPILER_GCC TRUE)
endif()

#===============================================================================
# set SG_PLATFORM_NAME
if (NOT SG_PLATFORM_NAME)
    if (WIN32)
        set(SG_PLATFORM_NAME "win")
    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        set(SG_PLATFORM_NAME "linux")
    endif()
endif()

if (SG_PLATFORM_NAME MATCHES "win")
    set(SG_PLATFORM_WINDOWS TRUE)
elseif(SG_PLATFORM_NAME MATCHES "linux")
    set(SG_PLATFORM_LINUX TRUE)
else()
    message(FATAL_ERROR "unsupported target platform")
endif()

#===============================================================================
# set SG_ARCH_NAME
if(NOT SG_ARCH_NAME)
    if((SG_PLATFORM_NAME MATCHES "win") OR (SG_PLATFORM_NAME MATCHES "linux"))
        if((CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64") OR (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64"))
            set(SG_ARCH_NAME "x64")
        else()
            set(SG_ARCH_NAME "x86")
        endif()
    endif()
endif()

set(SG_TARGET_PLATFORM_NAME ${SG_PLATFORM_NAME}_${SG_ARCH_NAME})

#===============================================================================
# set output suffix
set(SG_OUTPUT_SUFFIX _${SG_COMPILER_NAME}${SG_COMPILER_VERSION})

#===============================================================================
# set config postfix
set(CMAKE_DEBUG_POSTFIX "_d" CACHE STRING "Add a postfix, usually _d on windows")
set(CMAKE_RELEASE_POSTFIX "" CACHE STRING "Add a postfix, usually empty on windows")
set(CMAKE_RELWITHDEBINFO_POSTFIX "" CACHE STRING "Add a postfix, usually empty on windows")
set(CMAKE_MINSIZEREL_POSTFIX "" CACHE STRING "Add a postfix, usually empty on windows")

#===============================================================================
# helper function
#function(sg_copy_depends_dll _target)
#    if(NOT SG_PLATFORM_WINDOWS)
#        message(AUTHOR_WARNING "sg_copy_depends_dll only for win32.")
#        return()
#    endif()
#    if(NOT TARGET ${_target})
#        message(AUTHOR_WARNING "Input target '${_target}' is invalid.")
#    endif()
#    
#    set(_DEPENDS_DLL "" CACHE INTERNAL "" FORCE)
#endfunction()
