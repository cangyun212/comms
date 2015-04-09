PROJECT(core)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.6)

IF(COMMAND cmake_policy)
	CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

SET(CORE_MAJOR_VERSION 1)
SET(CORE_MINOR_VERSION 0)
SET(CORE_PATCH_VERSION 0)
SET(CORE_VERSION ${CORE_MAJOR_VERSION}.${CORE_MINOR_VERSION}.${CORE_PATCH_VERSION})

SET(CORE_PROJECT_DIR "${CMAKE_HOME_DIRECTORY}/..")
SET(SG_ROOT_DIR "${CORE_PROJECT_DIR}/../..")


INCLUDE(${SG_ROOT_DIR}/build/cmake/Common.cmake)
INCLUDE(${SG_ROOT_DIR}/build/cmake/Platform.cmake)

SET(SG_BIN_DIR "${SG_ROOT_DIR}/bin/${SG_PLATFORM_NAME}")

INCLUDE(${SG_ROOT_DIR}/build/cmake/Compiler.cmake)

SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

SET(CORE_OUTPUT_DIR "${CORE_PROJECT_DIR}/lib/${SG_PLATFORM_NAME}")

# The project settings

SET(LIB_NAME core)

SET(CORE_HEADER_FILES
    ${SG_ROOT_DIR}/include/core/core.hpp
    ${SG_ROOT_DIR}/include/core/core_config.hpp
    ${SG_ROOT_DIR}/include/core/core_types.hpp
    ${SG_ROOT_DIR}/include/core/core_utils.hpp
    ${SG_ROOT_DIR}/include/core/core_singleton.hpp
    ${SG_ROOT_DIR}/include/core/core_thread.hpp
    ${SG_ROOT_DIR}/include/core/core_timer.hpp
    ${SG_ROOT_DIR}/include/core/core_debug.hpp
    ${SG_ROOT_DIR}/include/core/core_console.hpp
    ${SG_ROOT_DIR}/include/core/core_window.hpp
    ${SG_ROOT_DIR}/include/core/console/core_console_table.hpp
    ${SG_ROOT_DIR}/include/core/console/core_console_progress_bar.hpp
    ${SG_ROOT_DIR}/include/core/console/core_console_window.hpp
    ${SG_ROOT_DIR}/include/core/console/core_console_printer.hpp
    ${SG_ROOT_DIR}/include/core/console/core_console_stream.hpp
)
SET(CORE_SOURCE_FILES
    ${SG_ROOT_DIR}/src/core/core.cpp
    ${SG_ROOT_DIR}/src/core/core_utils.cpp
    ${SG_ROOT_DIR}/src/core/core_timer.cpp
    ${SG_ROOT_DIR}/src/core/core_debug.cpp
    ${SG_ROOT_DIR}/src/core/core_console.cpp
    ${SG_ROOT_DIR}/src/core/console/core_console_table.cpp
    ${SG_ROOT_DIR}/src/core/console/core_console_progress_bar.cpp
    ${SG_ROOT_DIR}/src/core/console/core_console_window.cpp
    ${SG_ROOT_DIR}/src/core/console/core_console_printer.cpp
    ${SG_ROOT_DIR}/src/core/console/core_console_stream.cpp
)


SOURCE_GROUP("Source Files" FILES ${CORE_SOURCE_FILES})
SOURCE_GROUP("Header Files" FILES ${CORE_HEADER_FILES})

# for autolink
ADD_DEFINITIONS(-DCORE_SOURCE)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${SG_ROOT_DIR}/include)
LINK_DIRECTORIES(${Boost_LIBRARY_DIR})

IF(UNIX)
    SET(EXTRA_LINKED_LIBRARIES ${EXTRA_LINKED_LIBRARIES}
        ${Boost_CHRONO_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY})
ENDIF()

ADD_LIBRARY(${LIB_NAME} ${SG_PREFERRED_LIB_TYPE} 
	${CORE_SOURCE_FILES} ${CORE_HEADER_FILES}
)

SET(LIB_OUTPUT_NAME ${LIB_NAME}${SG_OUTPUT_SUFFIX})

SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ${CORE_OUTPUT_DIR}
	LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CORE_OUTPUT_DIR}
	LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CORE_OUTPUT_DIR}
	PROJECT_LABEL ${LIB_NAME}
	DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
	OUTPUT_NAME ${LIB_OUTPUT_NAME}
)

TARGET_LINK_LIBRARIES(${LIB_NAME} ${EXTRA_LINKED_LIBRARIES})

IF(SG_PREFERRED_LIB_TYPE STREQUAL "SHARED")
    ADD_POST_BUILD(${LIB_NAME} "")
    
    INSTALL(TARGETS ${LIB_NAME}
            RUNTIME DESTINATION ${SG_BIN_DIR}
            LIBRARY DESTINATION ${SG_BIN_DIR}
            ARCHIVE DESTINATION ${CORE_OUTPUT_DIR})
ENDIF()
