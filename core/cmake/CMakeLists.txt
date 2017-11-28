
cmake_minimum_required(VERSION 3.1)

set(CORE_MAJOR_VERSION 1)
set(CORE_MINOR_VERSION 0)
set(CORE_PATCH_VERSION 0)
set(CORE_VERSION ${CORE_MAJOR_VERSION}.${CORE_MINOR_VERSION}.${CORE_PATCH_VERSION})

project(core VERSION ${CORE_VERSION})

set(CORE_PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(SG_PROJECT_DIR "${CORE_PROJECT_DIR}/..")
set(PDCURSES_PROJECT_DIR "${SG_PROJECT_DIR}/pdcurses")

include(${SG_PROJECT_DIR}/cmake/common.cmake)
include(${SG_PROJECT_DIR}/cmake/compiler.cmake)

set(SG_BIN_DIR "${SG_PROJECT_DIR}/bin/${SG_TARGET_PLATFORM_NAME}")
set(SG_LIB_DIR "${SG_PROJECT_DIR}/lib/${SG_TARGET_PLATFORM_NAME}")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# The project settings

set(LIB_NAME core)

set(CORE_CONSOLE_HEADER_FILES
    ${CORE_PROJECT_DIR}/include/Console/ConsoleTable.hpp
    ${CORE_PROJECT_DIR}/include/Console/ConsoleProgressbar.hpp
    ${CORE_PROJECT_DIR}/include/Console/ConsoleWindow.hpp
    ${CORE_PROJECT_DIR}/include/Console/ConsolePrinter.hpp
    ${CORE_PROJECT_DIR}/include/Console/ConsoleStream.hpp
    )

set(CORE_CONSOLE_SOURCE_FILES
    ${CORE_PROJECT_DIR}/src/Console/ConsoleTable.cpp
    ${CORE_PROJECT_DIR}/src/Console/ConsoleProgressbar.cpp
    ${CORE_PROJECT_DIR}/src/Console/ConsoleWindow.cpp
    ${CORE_PROJECT_DIR}/src/Console/ConsolePrinter.cpp
    ${CORE_PROJECT_DIR}/src/Console/ConsoleStream.cpp
    )

set(CORE_HEADER_FILES
    ${CORE_PROJECT_DIR}/include/Core.hpp
    ${CORE_PROJECT_DIR}/include/Config.hpp
    ${CORE_PROJECT_DIR}/include/Types.hpp
    ${CORE_PROJECT_DIR}/include/Utils.hpp
    ${CORE_PROJECT_DIR}/include/Singleton.hpp
    ${CORE_PROJECT_DIR}/include/Timer.hpp
    ${CORE_PROJECT_DIR}/include/Console.hpp
    ${CORE_PROJECT_DIR}/include/Window.hpp
    ${CORE_PROJECT_DIR}/include/BaseInteger.hpp
    ${CORE_PROJECT_DIR}/include/Concatenate.hpp
    )

set(CORE_SOURCE_FILES
    ${CORE_PROJECT_DIR}/src/Core.cpp
    ${CORE_PROJECT_DIR}/src/Utils.cpp
    ${CORE_PROJECT_DIR}/src/Timer.cpp
    ${CORE_PROJECT_DIR}/src/Console.cpp
    ${CORE_PROJECT_DIR}/src/BaseInteger.cpp
    ${CORE_PROJECT_DIR}/src/Concatenate.cpp
    )


source_group("Source Files" FILES ${CORE_SOURCE_FILES})
source_group("Source Files\\Console" FILES ${CORE_CONSOLE_SOURCE_FILES})
source_group("Header Files" FILES ${CORE_HEADER_FILES})
source_group("Header Files\\Console" FILES ${CORE_CONSOLE_HEADER_FILES})

# for autolink
add_definitions(-DCORE_SOURCE)

include(${SG_PROJECT_DIR}/cmake/configboost.cmake)
include(${SG_PROJECT_DIR}/rapidjson/cmake/rapidjson.cmake)

include_directories(${Boost_INCLUDE_DIR})
include_directories(${CORE_PROJECT_DIR}/include)
if(SG_PLATFORM_WINDOWS)
    include_directories(${PDCURSES_PROJECT_DIR}/pdcurses)
endif()

link_directories(${Boost_LIBRARY_DIR})

add_library(${LIB_NAME} SHARED 
    ${CORE_SOURCE_FILES} ${CORE_CONSOLE_SOURCE_FILES} ${CORE_HEADER_FILES} ${CORE_CONSOLE_HEADER_FILES}
)

target_include_directories(${LIB_NAME} PUBLIC ${RAPIDJSON_INCLUDE_DIR})

if(SG_PLATFORM_WINDOWS)
    target_compile_definitions(${LIB_NAME} PUBLIC -DSG_USE_PDCURSES)
    target_link_libraries(${LIB_NAME} ${Boost_FILESYSTEM_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_SYSTEM_LIBRARY}
        debug pdcurses${SG_OUTPUT_SUFFIX}_d optimized pdcurses${SG_OUTPUT_SUFFIX})
else()
    target_link_libraries(${LIB_NAME} ${Boost_FILESYSTEM_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_SYSTEM_LIBRARY}
        ncurses)
endif()

#if(SG_PLATFORM_LINUX)
#    target_compile_definitions(${LIB_NAME} PUBLIC -DSG_USE_PDCURSES)
#    include(${PDCURSES_PROJECT_DIR}/pdcurses/x11/pdcurses.cmake)
#    target_link_libraries(${LIB_NAME} ${PDCURSES_XLIBS} ${PDCURSES_EXTRA_LIBS})
#endif()

set_target_properties(${LIB_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${SG_LIB_DIR}
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${SG_LIB_DIR}
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${SG_LIB_DIR}
    ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${SG_LIB_DIR}
    ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL ${SG_LIB_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${SG_BIN_DIR}
    LIBRARY_OUTPUT_DIRECTORY_DEBUG ${SG_BIN_DIR}
    LIBRARY_OUTPUT_DIRECTORY_RELEASE ${SG_BIN_DIR}
    LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${SG_BIN_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${SG_BIN_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${SG_BIN_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${SG_BIN_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${SG_BIN_DIR}
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${SG_BIN_DIR}
    PROJECT_LABEL ${LIB_NAME}
	DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
    OUTPUT_NAME ${LIB_NAME}${SG_OUTPUT_SUFFIX})

if(SG_COMPILER_MSVC)

    install(TARGETS ${LIB_NAME}
            RUNTIME DESTINATION ${SG_BIN_DIR}
            LIBRARY DESTINATION ${SG_BIN_DIR}
            ARCHIVE DESTINATION ${SG_LIB_DIR})
endif()
