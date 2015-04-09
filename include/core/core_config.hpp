
#ifndef __CORE_CONFIG_HPP__
#define __CORE_CONFIG_HPP__

#if !defined(__cplusplus)
    #error C++ compiler required.
#endif

#if defined(DEBUG) | defined(_DEBUG)
    #define CORE_DEBUG
#endif

// Defines supported compilers
#if defined(__GNUC__)
    // GNU C++

    #define CORE_COMPILER_GCC

    #include <bits/c++config.h> // for _GLIBCXX_HAS_GTHREADS

    #if __GNUC__ >= 4
        #if __GNUC_MINOR__ >= 9
            #define CORE_COMPILER_VERSION 49
        #elif __GNUC_MINOR__ >= 8
            #define CORE_COMPILER_VERSION 48
        #elif __GNUC_MINOR__ >= 7
            #define CORE_COMPILER_VERSION 47
        #elif __GNUC_MINOR__ >= 6
            #define CORE_COMPILER_VERSION 46
        #elif __GNUC_MINOR__ >= 5
            #define CORE_COMPILER_VERSION 45
        #elif __GNUC_MINOR__ >= 4
            #define CORE_COMPILER_VERSION 44
        #elif __GNUC_MINOR__ >= 3
            #define CORE_COMPILER_VERSION 43
        #elif __GNUC_MINOR__ >= 2
            #define CORE_COMPILER_VERSION 42
        #elif __GNUC_MINOR__ >= 1
            #define CORE_COMPILER_VERSION 41
        #elif __GNUC_MINOR__ >= 0
            #define CORE_COMPILER_VERSION 40
        #endif

        #ifdef __GXX_EXPERIMENTAL_CXX0X__
            #if __GNUC_MINOR__ >= 3
                #define CORE_CXX11_CORE_STATIC_ASSERT_SUPPORT
                #define CORE_CXX11_CORE_DECLTYPE_SUPPORT
                #define CORE_CXX11_CORE_RVALUE_REFERENCES_SUPPORT
                #define CORE_CXX11_CORE_EXTERN_TEMPLATES_SUPPORT
                #define CORE_CXX11_CORE_VARIADIC_TEMPLATES
                #define CORE_CXX11_LIBRARY_ALGORITHM_SUPPORT
                #define CORE_CXX11_LIBRARY_ARRAY_SUPPORT
                #define CORE_CXX11_LIBRARY_CSTDINT_SUPPORT
                #define CORE_CXX11_LIBRARY_SMART_PTR_SUPPORT
                #define CORE_CXX11_LIBRARY_UNORDERED_SUPPORT
                #define CORE_CXX11_LIBRARY_TYPE_TRAITS_SUPPORT
                #define CORE_CXX11_LIBRARY_MEM_FN_SUPPORT
            #endif
            #if __GNUC_MINOR__ >= 4
                #define CORE_CXX11_CORE_STRONGLY_TYPED_ENUMS_SUPPORT
                #define CORE_CXX11_LIBRARY_SYSTEM_ERROR_SUPPORT
                #define CORE_CXX11_LIBRARY_ATOMIC_SUPPORT
                #ifdef _GLIBCXX_HAS_GTHREADS
                    #ifndef _GLIBCXX_USE_NANOSLEEP
                        #define _GLIBCXX_USE_NANOSLEEP // we must define this manually for sleep_for
                    #endif
                    #define CORE_CXX11_LIBRARY_CHRONO_SUPPORT
                    #define CORE_CXX11_LIBRARY_THREAD_SUPPORT
                #endif
            #endif
            #if __GNUC_MINOR__ >= 6
                #define CORE_CXX11_CORE_NULLPTR_SUPPORT
                #define CORE_CXX11_CORE_FOREACH_SUPPORT
                #define CORE_CXX11_CORE_CONSTEXPR_SUPPORT
            #endif
            #if __GNUC_MINOR__ >= 7
                #define CORE_CXX11_CORE_OVERRIDE_SUPPORT
            #endif
            #if __GNUC_MINOR__ >= 9
                #define CORE_CXX11_LIBRARY_REGEX_SUPPORT
            #endif
        #endif
    #else
        #error Unknown compiler.
    #endif
#else
    #error Unknown compiler.
#endif

// Defines supported platforms
#if defined(linux) || defined(__linux) || defined(__linux__)
    #define CORE_PLATFORM_LINUX
    #define CORE_COMPILER_NAME gcc
#else
    #error Unknown platform.
#endif

// Defines supported CPUs
#if defined(CORE_COMPILER_GCC)
    #if defined(__x86_64__)
        #define CORE_CPU_X64
        #define CORE_COMPILER_TARGET x64
    #elif defined(__i386__)
        #define CORE_CPU_X86
        #define CORE_COMPILER_TARGET x86
    #endif
#endif

// Defines the native endian
#if defined(CORE_CPU_X86) || defined(CORE_CPU_X64)
    #define CORE_LITTLE_ENDIAN
#else
    #define CORE_BIG_ENDIAN
#endif

// Deprecated features in Boost.System are excluded.
#ifndef BOOST_SYSTEM_NO_DEPRECATED
    #define BOOST_SYSTEM_NO_DEPRECATED
#endif

#if defined(CORE_COMPILER_GCC)
    // Prevent Boost to link the Boost.DateTime
    #ifndef BOOST_DATE_TIME_NO_LIB
        #define BOOST_DATE_TIME_NO_LIB
    #endif
#endif

#ifdef CORE_HAS_DECLSPEC
    #ifdef CORE_SOURCE
        #define CORE_API    __declspec(dllexport)
    #else
        #define CORE_API    __declspec(dllimport)
    #endif
#else
    #define CORE_API
#endif


#endif
