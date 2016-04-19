
cmake_minimum_required(VERSION 3.1)

set(SIMULATOR_MAJOR_VERSION 1)
set(SIMULATOR_MINOR_VERSION 0)
set(SIMULATOR_PATCH_VERSION 0)
set(SIMULATOR_VERSION ${SIMULATOR_MAJOR_VERSION}.${SIMULATOR_MINOR_VERSION}.${SIMULATOR_PATCH_VERSION})

project(simulator VERSION ${SIMULATOR_VERSION})

set(SIMULATOR_PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(SG_PROJECT_DIR "${SIMULATOR_PROJECT_DIR}/..")
set(CORE_PROJECT_DIR "${SG_PROJECT_DIR}/core")
set(COMMS_PROJECT_DIR "${SG_PROJECT_DIR}/comms")

include(${SG_PROJECT_DIR}/cmake/common.cmake)
include(${SG_PROJECT_DIR}/cmake/compiler.cmake)

set(SG_BIN_DIR "${SG_PROJECT_DIR}/bin/${SG_TARGET_PLATFORM_NAME}")
set(SG_LIB_DIR "${SG_PROJECT_DIR}/lib/${SG_TARGET_PLATFORM_NAME}")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# The project settings

set(EXE_NAME simulator)

set(SIMULATOR_HEADER_FILES 
    ${SIMULATOR_PROJECT_DIR}/include/simulator.hpp
    ${SIMULATOR_PROJECT_DIR}/include/qcom_simulator.hpp
    ${SIMULATOR_PROJECT_DIR}/include/line_reader.hpp
    ${SIMULATOR_PROJECT_DIR}/include/action.hpp
    ${SIMULATOR_PROJECT_DIR}/include/qcom_action.hpp
    ${SIMULATOR_PROJECT_DIR}/include/action_factory.hpp
    ${SIMULATOR_PROJECT_DIR}/include/qcom_action_factory.hpp
    ${SIMULATOR_PROJECT_DIR}/include/action_center.hpp
    ${SIMULATOR_PROJECT_DIR}/include/predeclare.hpp
    ${SIMULATOR_PROJECT_DIR}/include/utils.hpp
    ${SIMULATOR_PROJECT_DIR}/include/cmd_parser.hpp
    )

set(SIMULATOR_SOURCE_FILES
    ${SIMULATOR_PROJECT_DIR}/src/main.cpp
    ${SIMULATOR_PROJECT_DIR}/src/simulator.cpp
    ${SIMULATOR_PROJECT_DIR}/src/qcom_simulator.cpp
    ${SIMULATOR_PROJECT_DIR}/src/line_reader.cpp
    ${SIMULATOR_PROJECT_DIR}/src/action.cpp
    ${SIMULATOR_PROJECT_DIR}/src/qcom_action.cpp
    ${SIMULATOR_PROJECT_DIR}/src/action_factory.cpp
    ${SIMULATOR_PROJECT_DIR}/src/qcom_action_factory.cpp
    ${SIMULATOR_PROJECT_DIR}/src/action_center.cpp
    ${SIMULATOR_PROJECT_DIR}/src/utils.cpp
    ${SIMULATOR_PROJECT_DIR}/src/cmd_parser.cpp
    )

source_group("Source Files" FILES ${SIMULATOR_SOURCE_FILES})
source_group("Header Files" FILES ${SIMULATOR_HEADER_FILES})

include(${SG_PROJECT_DIR}/cmake/configboost.cmake)

include_directories(${Boost_INCLUDE_DIR})
include_directories(${CORE_PROJECT_DIR}/include)
include_directories(${COMMS_PROJECT_DIR}/include)
include_directories(${SIMULATOR_PROJECT_DIR}/include)

link_directories(${Boost_LIBRARY_DIR})

add_executable(${EXE_NAME} ${SIMULATOR_SOURCE_FILES} ${SIMULATOR_HEADER_FILES})

target_link_libraries(${EXE_NAME} debug core${SG_OUTPUT_SUFFIX}_d optimized core${SG_OUTPUT_SUFFIX}
    debug comms${SG_OUTPUT_SUFFIX}_d optimized comms${SG_OUTPUT_SUFFIX}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    )

set_target_properties(${EXE_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${SG_BIN_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${SG_BIN_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${SG_BIN_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${SG_BIN_DIR}
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${SG_BIN_DIR}
    PROJECT_LABEL ${EXE_NAME}
    DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
    OUT_PUTNAME ${EXE_NAME}${SG_OUTPUT_SUFFIX}
)

if(SG_COMPILER_MSVC)
    install(TARGETS ${EXE_NAME}
        RUNTIME DESTINATION ${SG_BIN_DIR}
        LIBRARY DESTINATION ${SG_BIN_DIR}
        ARCHIVE DESTINATION ${SG_LIB_DIR})
endif()


