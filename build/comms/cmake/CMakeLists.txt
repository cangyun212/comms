PROJECT(comms)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.6)

IF(COMMAND cmake_policy)
	CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

SET(COMMS_MAJOR_VERSION 1)
SET(COMMS_MINOR_VERSION 0)
SET(COMMS_PATCH_VERSION 0)
SET(COMMS_VERSION ${COMMS_MAJOR_VERSION}.${COMMS_MINOR_VERSION}.${COMMS_PATCH_VERSION})

SET(COMMS_PROJECT_DIR "${CMAKE_HOME_DIRECTORY}/..")
SET(SG_ROOT_DIR "${COMMS_PROJECT_DIR}/../..")

INCLUDE(${SG_ROOT_DIR}/build/cmake/Common.cmake)
INCLUDE(${SG_ROOT_DIR}/build/cmake/Platform.cmake)

SET(SG_BIN_DIR "${SG_ROOT_DIR}/bin/${SG_PLATFORM_NAME}")

INCLUDE(${SG_ROOT_DIR}/build/cmake/Compiler.cmake)

SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

SET(COMMS_OUTPUT_DIR "${COMMS_PROJECT_DIR}/lib/${SG_PLATFORM_NAME}")

# The project settings

SET(LIB_NAME comms)

SET(COMMS_HEADER_FILES
    ${SG_ROOT_DIR}/include/comms/comms_predeclare.hpp
    ${SG_ROOT_DIR}/include/comms/comms.hpp
    ${SG_ROOT_DIR}/include/comms/qcom/qcom.hpp
    ${SG_ROOT_DIR}/include/comms/qcom/qcom_general_status.hpp
    ${SG_ROOT_DIR}/include/comms/qcom/qcom_broadcast_seek.hpp
    ${SG_ROOT_DIR}/include/comms/qcom/qcom_broadcast.hpp
    ${SG_ROOT_DIR}/include/comms/qcom/qcom_egm_config_req.hpp
    ${SG_ROOT_DIR}/include/comms/qcom/qcom_egm_config.hpp
    ${SG_ROOT_DIR}/include/comms/qcom/qcom_game_config.hpp
    ${SG_ROOT_DIR}/include/comms/qcom/qogr/qogr_crc.h
    ${SG_ROOT_DIR}/include/comms/qcom/qogr/qogr_qcom.h
    ${SG_ROOT_DIR}/include/comms/qcom/qcom_inline.h
)
SET(COMMS_SOURCE_FILES
    ${SG_ROOT_DIR}/src/comms/comms.cpp
    ${SG_ROOT_DIR}/src/comms/qcom/qcom.cpp
    ${SG_ROOT_DIR}/src/comms/qcom/qcom_general_status.cpp
    ${SG_ROOT_DIR}/src/comms/qcom/qcom_broadcast_seek.cpp
    ${SG_ROOT_DIR}/src/comms/qcom/qcom_broadcast.cpp
    ${SG_ROOT_DIR}/src/comms/qcom/qcom_egm_config_req.cpp
    ${SG_ROOT_DIR}/src/comms/qcom/qcom_egm_config.cpp
    ${SG_ROOT_DIR}/src/comms/qcom/qcom_game_config.cpp
    ${SG_ROOT_DIR}/src/comms/qcom/qogr/qogr_crc.c
)


SOURCE_GROUP("Source Files" FILES ${COMMS_SOURCE_FILES})
SOURCE_GROUP("Header Files" FILES ${COMMS_HEADER_FILES})

ADD_DEFINITIONS(-DCOMMS_SOURCE)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${SG_ROOT_DIR}/include)
INCLUDE_DIRECTORIES(${SG_ROOT_DIR}/include/comms/qcom/qogr)

LINK_DIRECTORIES(${Boost_LIBRARY_DIR})
LINK_DIRECTORIES(${SG_ROOT_DIR}/build/core/lib/${SG_PLATFORM_NAME})

ADD_LIBRARY(${LIB_NAME} ${SG_PREFERRED_LIB_TYPE} 
	${COMMS_SOURCE_FILES} ${COMMS_HEADER_FILES}
)

SET(LIB_OUTPUT_NAME ${LIB_NAME}${SG_OUTPUT_SUFFIX})


SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ${COMMS_OUTPUT_DIR}
	LIBRARY_OUTPUT_DIRECTORY_DEBUG ${COMMS_OUTPUT_DIR}
	LIBRARY_OUTPUT_DIRECTORY_RELEASE ${COMMS_OUTPUT_DIR}
	PROJECT_LABEL ${LIB_NAME}
	DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
	OUTPUT_NAME ${LIB_OUTPUT_NAME}
)

IF(UNIX)
	SET(EXTRA_LINKED_LIBRARIES ${EXTRA_LINKED_LIBRARIES} 
		debug core${SG_OUTPUT_SUFFIX}_d optimized core${SG_OUTPUT_SUFFIX})
ENDIF()

TARGET_LINK_LIBRARIES(${LIB_NAME} ${EXTRA_LINKED_LIBRARIES})

IF(SG_PREFERRED_LIB_TYPE STREQUAL "SHARED")
    ADD_POST_BUILD(${LIB_NAME} "")
    
    INSTALL(TARGETS ${LIB_NAME}
	RUNTIME DESTINATION ${SG_BIN_DIR}
	LIBRARY DESTINATION ${SG_BIN_DIR}
	ARCHIVE DESTINATION ${COMMS_OUTPUT_DIR}
)
ENDIF()
