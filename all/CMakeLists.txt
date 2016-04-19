
cmake_minimum_required(VERSION 3.1)

set(ALL_PROJECT_DIR "${CMAKE_SOURCE_DIR}")
set(SG_PROJECT_DIR "${ALL_PROJECT_DIR}/..")
set(CORE_PROJECT_DIR "${SG_PROJECT_DIR}/core")
set(COMMS_PROJECT_DIR "${SG_PROJECT_DIR}/comms")
set(SIMULATOR_PROJECT_DIR "${SG_PROJECT_DIR}/simulator")

add_subdirectory("${CORE_PROJECT_DIR}/cmake" "core")
add_subdirectory("${COMMS_PROJECT_DIR}/cmake" "comms")
add_subdirectory("${SIMULATOR_PROJECT_DIR}/cmake" "simulator")

