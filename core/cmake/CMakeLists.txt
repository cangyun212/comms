
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

set(CORE_HEADER_FILES
    ${CORE_PROJECT_DIR}/include/core.hpp
    ${CORE_PROJECT_DIR}/include/core_config.hpp
    ${CORE_PROJECT_DIR}/include/core_types.hpp
    ${CORE_PROJECT_DIR}/include/core_utils.hpp
    ${CORE_PROJECT_DIR}/include/core_singleton.hpp
    ${CORE_PROJECT_DIR}/include/core_thread.hpp
    ${CORE_PROJECT_DIR}/include/core_timer.hpp
    ${CORE_PROJECT_DIR}/include/core_debug.hpp
    ${CORE_PROJECT_DIR}/include/core_console.hpp
    ${CORE_PROJECT_DIR}/include/core_window.hpp
    ${CORE_PROJECT_DIR}/include/console/core_console_table.hpp
    ${CORE_PROJECT_DIR}/include/console/core_console_progress_bar.hpp
    ${CORE_PROJECT_DIR}/include/console/core_console_window.hpp
    ${CORE_PROJECT_DIR}/include/console/core_console_printer.hpp
    ${CORE_PROJECT_DIR}/include/console/core_console_stream.hpp
)
set(CORE_SOURCE_FILES
    ${CORE_PROJECT_DIR}/src/core.cpp
    ${CORE_PROJECT_DIR}/src/core_utils.cpp
    ${CORE_PROJECT_DIR}/src/core_timer.cpp
    ${CORE_PROJECT_DIR}/src/core_debug.cpp
    ${CORE_PROJECT_DIR}/src/core_console.cpp
    ${CORE_PROJECT_DIR}/src/console/core_console_table.cpp
    ${CORE_PROJECT_DIR}/src/console/core_console_progress_bar.cpp
    ${CORE_PROJECT_DIR}/src/console/core_console_window.cpp
    ${CORE_PROJECT_DIR}/src/console/core_console_printer.cpp
    ${CORE_PROJECT_DIR}/src/console/core_console_stream.cpp
)


source_group("Source Files" FILES ${CORE_SOURCE_FILES})
source_group("Header Files" FILES ${CORE_HEADER_FILES})

# for autolink
add_definitions(-DCORE_SOURCE)

include(${SG_PROJECT_DIR}/cmake/configboost.cmake)

include_directories(${Boost_INCLUDE_DIR})
include_directories(${PDCURSES_PROJECT_DIR}/pdcurses)
include_directories(${CORE_PROJECT_DIR}/include)

link_directories(${Boost_LIBRARY_DIR})

add_library(${LIB_NAME} SHARED 
	${CORE_SOURCE_FILES} ${CORE_HEADER_FILES}
)

target_link_libraries(${LIB_NAME} ${Boost_FILESYSTEM_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_SYSTEM_LIBRARY}
    debug pdcurses${SG_OUTPUT_SUFFIX}_d optimized pdcurses${SG_OUTPUT_SUFFIX})

if(SG_PLATFORM_LINUX)
    include(${PDCURSES_PROJECT_DIR}/pdcurses/x11/pdcurses.cmake)
    target_link_libraries(${LIB_NAME} ${PDCURSES_XLIBS} ${PDCURSES_EXTRA_LIBS})
endif()

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
