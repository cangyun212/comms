

#CommsSim

## Table of Contents ##
*[Environment](#Environment)
*[Project Structure](#ProjectStructure)
*[Building](#Building)
*[Feature](#Feature)
*[Design](#Design)
*[Debug](#Debug)
*[Authors](#Authors)

<a name="Environment"></a>
## Environment ##
*CommsSim* can run on both windows and Linux system. 
As a minimum requirement, C++ runtime is mandatory for both target system since it's written by C++ program language.<br/>
To build *CommsSim*, the following environment are required:

 * g++ 4.7 or above (Linux)/Visual Studio 2013 or above (Windows)
 * Python 2.7 
 * CMake 3.1 or above

Note: we assume the ncurses library is exist on your Linux system by default (at least this is true for our OpenSUSE 12.3) but if you failed link to this library, you can install it manually or you can use pdcurses instead (check more detail in building section).
<a name="ProjectStructure"></a>
## Project Structure ##
The current project structure is:

 * all (a top level cmake project to build all projects)
 * boost (boost, third-party library) 
 * cmake (common cmake file for all projects)
 * comms (comms library used for communicating with EGM)
 * core (a foundation library which provide a lot config/implementation/console utilities and can be widely used in many projects)
 * doc (comms protocol documents)
 * pdcurses (a cross platform implementation for curses specification, it's used for Windows platform in this project currently)
 * simulator (simulator provide a console interface for interactive with user)
 * build_xx scripts (build scripts used for each project)

<a name="Building"></a>
## Building ##
It's very easy to build CommsSim. Type the following command in your console terminal: <br/>
*build_all.py*<br/>
This will build the debug x64 version of CommsSim by default. If you want to build release version or x86 version, you can use command line option:<br/>
*build_all.py -g Release -a x86*<br/>
You can change the build configuration by modifying *cfg_build_default.py* also. Please ref *cfg_buid_default.py* or *build_util.py* to find all available configuration.
 If no error happened during the build, three folder will be generated after the build:

 * bin (all binary file will be put in this folder including dll library and execute application file)
 * build (immediate building file will be put in this folder)
 * lib (archive library will be put int this folder)

All files in above folders will be categorized by the build configuration. For example, if you build a x64 debug version on Linux, then all generated file will be put into *linux_x64* folder and all binary files will have a "*_d*" postfix.<br/>
You can use *build_xx.py* to build specific library instead of building all of them. For example, if you just want to build core library, then you can use *build_core.py* with any build configuration. Please make sure all dependence are exist before building the specific library. For example, you must use *build_boost.py* and *build_pdcurses.py* with the same building configuration before building the core library.<br/>
Currently CommsSim use CMake as the build system which is an open-source, cross-platform family of tools designed to build, test and package software. CMake is used to control the software compilation process using simple platform and compiler independent configuration files, and generate native makefiles and workspaces that can be used in the compiler environment of your choice.  This give us an opportunity to provide an uniform build method on all target platform. To simplify the build process, we also integrate all third-party library which use different build tools into our CMake building system.<br/>
pdcurses library is a cross platform implementation for curses specification. Now we only use it on Windows platform since many Linux distribute with its own ncurses implementation. But if you met any issues with linking ncurses library on your Linux system, you can use pdcurse instead with a little effort to change the build script and CMake project file. To use pdcurse on Linux target platform, open *build_all.py* and comment line 24:

    if "win" == bi.host_platform:
this will enable the building of pdcurses on Linux.
Then open *core/cmake/CMakeLists.txt* and find the following codes:

    if(SG_PLATFORM_WINDOWS)
        include_directories(${PDCURSES_PROJECT_DIR}/pdcurses)
    endif()
and

    if(SG_PLATFORM_WINDOWS)
        target_link_libraries(${LIB_NMAE} ${Boost_FILESYSTEM_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_SYSTEM_LIBRARY} debug pdcurses${SG_OUTPUT_SUFFIX}_d optimized pdcurses${SG_OUTPUT_SUFFIX})
    else()
        target_link_libaries(${LIB_NAME} ${Boost_FILESYSTEM_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_SYSTEM_LIBRARY} ncurses)
    endif()
and

    #if(SG_PLATFORM_LINUX)
    #    include(${PDCURSES_PROJECT_DIR}/pdcurses/x11/pdcurses.cmake)
    #    target_link_libraries(${LIB_NAME} ${PDCURSES_XLIBS} ${PDCURSES_EXTRA_LIBS})
    #endif()
Comment the condition compile statement and the ncurses linkage. Uncomment special link setting for Linux. Now you're able to use pdcurses on Linux.
  
<a name="Feature"></a>
## Feature ##

<a name="Design"></a>
## Design ##
The design purpose of CommsSim is try to make the implementation flexible and modulized. This will make us to re-use or extend the code easily.
###C++1x
The new c++1x standard offers many useful features which help us to write clean, safe and fast code in a modern C++ way. So we decide to hug the C++1x even before starting the whole design. This make a great effect on our coding style, programming idioms and the API design. Since not all features are fully supported by the current compiler we use (You can go to gcc/msdn website to check the latest C++1x feature support), we choose a subset of C++1x features, especially those features which make C++1x really feel like a new-language compare to C++98, which supported by all compilers. For other features, we use the functions supplied by some third-part, such as boost. Those third-part libraries not only provide a cross platform implementation but also their ideas may go into the next version of C++ standard.
Here is a brief list including some C++1x features widely used in our current code.
 * **auto**
 * **smart pointer**
 * **nullptr**
 * **ranged for**
 * **lambda**
####RAII
Resource Acquisition Is Initialization or RAII, is a C++ programming technique, which binds the life cycle of a resource (allocated memory, thread of execution, open socket, open file, locked mutex, database connection—anything that exists in limited supply) to the lifetime of an object. RAII guarantees that the resource is available to any function that may access the object. It also guarantees that all resources are released when the lifetime of their controlling objectends, in reverse order of acquisition. By the help of C++1x features, such as smart pointer and move semantic, we can write more stable and more exception safe code. Following code show the two implementation of our thread safe log, one without RAII and the other one with RAII after refactor:

    extern CORE_API std::mutex g_log_guard;
Before refactor:

    #define SG_START_LOG_BLOCK(s) { g_log_guard.lock()
    #define SG_LOG_BLOCK(s, t, l) sg::ConsolePrinter::Instance().Log(s, t, l)
    #define SG_END_LOG_BLOCK(s) s.flush(); g_log_guard.unlock();}
After refactor:

    #define SG_START_LOG_BLOCK(s) { std::lock_guard<std::mutex> lock(g_log_guard)
    #define SG_LOG_BLOCK(s,t,l) sg::ConsolePrinter::Instance().Log(s,t,l)
    #define SG_END_LOG_BLOCK(s) s.flush();}

###Modules
####Core
The core lib is designed and implemented as a very fundamental lib which could be shared by various libs and apps. So we try to keep the API of core lib clean and safe as much as possible. We should not expose too much implement detail or put too many restriction to the usage of the lib.

 * Console. These facilities can be used in console style applications to provide the control of input and output. There are 3 major use cases of console input/output:
  * Multi-thread output with input, need split output/input window and multi-thread output control
  * Multi-thread output without input, need multi-thread output control
  * Single thread input and output, normal std::iostream is enough. 
 
 The current console utilities can handle all three cases by using different initial types. If you initialize console by custom window type, you can treat a console window as a normal GUI window. You can even register message handler to handle message (this means you can handle console input by register a message handler, it's very like windows GUI programming that you get a windows message then dispatch it to your window message callback). Besides the thread safe control, the console utilities also provide some other useful function, such as color output, table output and progress bar output. There are also some limitation of console module, for example you cannot mix using sg::ConsolePrinter with C std output function after initialization. But it won't bother user a lot since user can customize the console module to minimize these limitation.
 * *uintx_t* vs *uintx*. We can use *uintx_t* as a variable type when we need a fix width integer. For example, we can use uint8_t to represent a 8 bit width unsigned int value. This works fine in most case but failed when we use them with boost program option module which provide a simple way to parse command line options of your application. Let's assume you have an option named 'a' and it's take a value which target type is *uint8_t*, when boost program option parse the command line *"-a 0"* it will treat *'0'* as a *uint8_t* value. Unfortunately the underling type of *uint8_t* on our Windows/Linux system is *unsigned char* and inside boost program option uses std iostream to cast lexical *'0'* to it's target type, *unsigned char* in this case, so the actual value stored is the ASCII code of *'0'* which is 49. This is a disaster for our application. So that's why we implement a group of new type *uintx*. These new types overload operator >> and << which are used by program option to do the lexical cast. At first, they cast string literal to integer type and then cast those integer to the target type by using boost numeric cast. Through this way we can get the correct value of those fix width types variable. There is one more thing need to be noticed that boost numeric cast could raise  bad numeric cast exception which will not be handled by boost program option, so when you use it you need to handle it by yourself.
####Comms
####Simulator
<a name="Debug"></a>
## Debug ##

<a name="Authors"></a>
## Authors ##
**Zhang Zhen** <ZhenZhang@ballytech.com>
**Lu Xinyu** <XLu@ballytech.com>
**Guo Bin** <BGUO@ballytech.com>
