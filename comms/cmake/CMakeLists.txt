
cmake_minimum_required(VERSION 3.1)

set(COMMS_MAJOR_VERSION 1)
set(COMMS_MINOR_VERSION 0)
set(COMMS_PATCH_VERSION 0)
set(COMMS_VERSION ${COMMS_MAJOR_VERSION}.${COMMS_MINOR_VERSION}.${COMMS_PATCH_VERSION})

project(comms VERSION ${COMMS_VERSION})

set(COMMS_PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(SG_PROJECT_DIR "${COMMS_PROJECT_DIR}/..")
set(CORE_PROJECT_DIR "${SG_PROJECT_DIR}/core")

include(${SG_PROJECT_DIR}/cmake/common.cmake)
include(${SG_PROJECT_DIR}/cmake/compiler.cmake)

set(SG_BIN_DIR "${SG_PROJECT_DIR}/bin/${SG_TARGET_PLATFORM_NAME}")
set(SG_LIB_DIR "${SG_PROJECT_DIR}/lib/${SG_TARGET_PLATFORM_NAME}")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# The project settings

set(LIB_NAME comms)

set(COMMS_HEADER_FILES
    ${COMMS_PROJECT_DIR}/include/comms_predeclare.hpp
    ${COMMS_PROJECT_DIR}/include/comms.hpp
    ${COMMS_PROJECT_DIR}/include/qcom/qcom.hpp
    ${COMMS_PROJECT_DIR}/include/qcom/qcom_general_status.hpp
    ${COMMS_PROJECT_DIR}/include/qcom/qcom_broadcast_seek.hpp
    ${COMMS_PROJECT_DIR}/include/qcom/qcom_broadcast.hpp
    ${COMMS_PROJECT_DIR}/include/qcom/qcom_egm_config_req.hpp
    ${COMMS_PROJECT_DIR}/include/qcom/qcom_egm_config.hpp
    ${COMMS_PROJECT_DIR}/include/qcom/qcom_game_config.hpp
    ${COMMS_PROJECT_DIR}/include/qcom/qogr/qogr_crc.h
    ${COMMS_PROJECT_DIR}/include/qcom/qogr/qogr_qcom.h
    ${COMMS_PROJECT_DIR}/include/qcom/qcom_inline.h
)
set(COMMS_SOURCE_FILES
    ${COMMS_PROJECT_DIR}/src/comms.cpp
    ${COMMS_PROJECT_DIR}/src/qcom/qcom.cpp
    ${COMMS_PROJECT_DIR}/src/qcom/qcom_general_status.cpp
    ${COMMS_PROJECT_DIR}/src/qcom/qcom_broadcast_seek.cpp
    ${COMMS_PROJECT_DIR}/src/qcom/qcom_broadcast.cpp
    ${COMMS_PROJECT_DIR}/src/qcom/qcom_egm_config_req.cpp
    ${COMMS_PROJECT_DIR}/src/qcom/qcom_egm_config.cpp
    ${COMMS_PROJECT_DIR}/src/qcom/qcom_game_config.cpp
    ${COMMS_PROJECT_DIR}/src/qcom/qogr/qogr_crc.c
)

source_group("Source Files" FILES ${COMMS_SOURCE_FILES})
source_group("Header Files" FILES ${COMMS_HEADER_FILES})

add_definitions(-DCOMMS_SOURCE)

include(${SG_PROJECT_DIR}/cmake/configboost.cmake)

include_directories(${Boost_INCLUDE_DIR})
include_directories(${CORE_PROJECT_DIR}/include)
include_directories(${COMMS_PROJECT_DIR}/include)
include_directories(${COMMS_PROJECT_DIR}/include/comms/qcom/qogr)

link_directories(${Boost_LIBRARY_DIR})

add_library(${LIB_NAME} SHARED 
	${COMMS_SOURCE_FILES} ${COMMS_HEADER_FILES}
)

target_link_libraries(${LIB_NAME} debug core${SG_OUTPUT_SUFFIX}_d optimized core${SG_OUTPUT_SUFFIX})

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
    OUTPUT_NAME ${LIB_NAME}${SG_OUTPUT_SUFFIX}
)

if(SG_COMPILER_MSVC)

    install(TARGETS ${LIB_NAME}
	RUNTIME DESTINATION ${SG_BIN_DIR}
	LIBRARY DESTINATION ${SG_BIN_DIR}
    ARCHIVE DESTINATION ${SG_LIB_DIR})
endif()
