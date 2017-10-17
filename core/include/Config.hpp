
#ifndef __SG_CORE_CONFIG_HPP__
#define __SG_CORE_CONFIG_HPP__

#if !defined(__cplusplus)
    #error C++ compiler required.
#endif

#if defined(DEBUG) | defined(_DEBUG)
    #define SG_DEBUG
#endif

#define SG_STRINGIFY(x)     SG_DO_STRINGIFY(x)
#define SG_DO_STRINGIFY(x)  #x

#define SG_CORE_MAJOR_VERSION       1
#define SG_CORE_MINOR_VERSION       0
#define SG_CORE_PATCH_VERSION       0
#define SG_CORE_VERSION_STRING \
    SG_STRINGIFY(SG_CORE_MAJOR_VERSION.SG_CORE_MINOR_VERSION.SG_CORE_PATCH_VERSION)

// Defines supported compilers
#if defined(__GNUC__)
    // GNU C++

    #define SG_COMPILER_GCC
    #define SG_COMPILER_NAME gcc

    #include <bits/c++config.h> // for _GLIBCXX_HAS_GTHREADS

    #define GCC_VERSION (__GNUC__ * 10 + __GNUC_MINOR__)
    #if GCC_VERSION >= 51
        #define SG_COMPILER_VERSION 51
    #elif GCC_VERSION >= 49
        #define SG_COMPILER_VERSION 49
    #elif GCC_VERSION >= 48
        #define SG_COMPILER_VERSION 48
    #elif GCC_VERSION >= 47
        #define SG_COMPILER_VERSION 47
    #else
        #error "Unsupported compiler version. Please install g++ 4.7 or up"
    #endif

    #if !defined(__GXX_EXPERIMENTAL_CXX0X__) && (__cplusplus < 201103L)
        #error "-std=c++11 or -std=c++0x must be turned on."
    #endif

    #define _GLIBCXX_USE_NANOSLEEP// fix gcc thread bug for sleep_for

    #define SG_EXPORT   __attribute__((__visibility__("default")))
    #define SG_IMPORT

    #define SG_WARN_EXPAND(w)           #w

    #define SG_PUSH_WARN                _Pragma( SG_WARN_EXPAND(GCC diagnostic push) )
    #define SG_DISABLE_WARN(w)          _Pragma( SG_WARN_EXPAND(GCC diagnostic ignored w) )
    #define SG_POP_WARN                 _Pragma( SG_WARN_EXPAND(GCC diagnostic pop) )

    #define SG_WARN_DISABLE(s, w)       _Pragma( SG_WARN_EXPAND(GCC diagnostic push) ) \
                                        _Pragma( SG_WARN_EXPAND(GCC diagnostic ignored w)) \
                                        s; \
                                        _Pragma( SG_WARN_EXPAND(GCC diagnostic pop) )

    #define SG_WARN_CONVERSION          "-Wconversion"

#elif defined(_MSC_VER)

    #define SG_COMPILER_MSVC
    #define SG_COMPILER_NAME vc

    #define SG_HAS_DECLSPEC
    #define SG_EXPORT   __declspec(dllexport)
    #define SG_IMPORT   __declspec(dllimport)

    #if _MSC_VER >= 1900
        #define SG_COMPILER_VERSION 140
    #elif _MSC_VER >= 1800
        #define SG_COMPILER_VERSION 120
    #else
        #error "Unsupported compiler version. Please install vc12 or up"
    #endif

    #pragma warning(disable: 4503) // Some decorated name in boost are very long
    #pragma warning(disable: 4819) // Allow non-ANSI characters.

    #ifndef _CRT_SECURE_NO_DEPRECATE
        #define _CRT_SECURE_NO_DEPRECATE
    #endif

    #ifndef _SCL_SECURE_NO_DEPRECATE
        #define _SCL_SECURE_NO_DEPRECATE
    #endif

    #define SG_PUSH_WARN                    __pragma(warning(push))
    #define SG_DISABLE_WARN(w)              __pragma(warning(disable: w))
    #define SG_POP_WARN                     __pragma(warning(pop))

    #define SG_WARN_DISABLE(s,w)            __pragma(warning(push)) \
                                            __pragma(warning(disable: w)) \
                                            s; \
                                            __pragma(warning(pop))

    #define SG_WARN_CONVERSION              4244
    #define SG_WARN_DYNA_INITIALIZE         4592

    #pragma warning(disable:4251) // STL class are not dllexport

#else
    #error Unknown compiler.
#endif

// Defines supported platforms
#if defined(linux) || defined(__linux) || defined(__linux__)
    #define SG_PLATFORM_LINUX
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    #define SG_PLATFORM_WINDOWS

    #if defined(_WIN64)
        #define SG_PLATFORM_WIN64
    #else
        #define SG_PLATFORM_WIN32
    #endif

    #ifndef BOOST_ALL_NO_LIB
        #define BOOST_ALL_NO_LIB
    #endif

    #ifndef BOOST_ALL_DYN_LINK
        #define BOOST_ALL_DYN_LINK
    #endif

    // Shut min/max in windows.h
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #ifndef WINDOWS_LEAN_AND_MEAN
        #define WINDOWS_LEAN_AND_MEAN
    #endif

#else
    #error Unknown platform.
#endif

// Defines supported CPUs
#if defined(SG_COMPILER_GCC)
    #if defined(__x86_64__)
        #define SG_CPU_X64
        #define SG_COMPILER_TARGET x64
    #elif defined(__i386__)
        #define SG_CPU_X86
        #define SG_COMPILER_TARGET x86
    #else
        #error "Unknown CPU type. x86, x64 are supported"
    #endif
#elif defined(SG_COMPILER_MSVC)
    #if defined(_M_X64)
        #define SG_CPU_X64
        #define SG_COMPILER_TARGET x64
    #elif defined(_M_IX86)
        #define SG_CPU_X86
        #define SG_COMPILER_TARGET x86
    #else
        #error "Unknown CPU type. x86, x64 are supported"
    #endif
#endif

// Defines the native endian
#if defined(SG_CPU_X86) || defined(SG_CPU_X64)
    #define SG_LITTLE_ENDIAN
#else
    #define SG_BIG_ENDIAN
#endif

// Deprecated features in Boost.System are excluded.
#ifndef BOOST_SYSTEM_NO_DEPRECATED
    #define BOOST_SYSTEM_NO_DEPRECATED
#endif

// Prevent Boost to link the Boost.DateTime
#ifndef BOOST_DATE_TIME_NO_LIB
    #define BOOST_DATE_TIME_NO_LIB
#endif

#ifdef SG_HAS_DECLSPEC
    #ifdef CORE_SOURCE
        #define CORE_API    SG_EXPORT
    #else
        #define CORE_API    SG_IMPORT
    #endif
#else
    #define CORE_API
#endif


#endif
