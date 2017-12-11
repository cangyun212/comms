
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
    ${COMMS_PROJECT_DIR}/include/CommsPredeclare.hpp
    ${COMMS_PROJECT_DIR}/include/Comms.hpp
    )

set(COMMS_SOURCE_FILES
    ${COMMS_PROJECT_DIR}/src/Comms.cpp
    )

set(COMMS_QCOM_HEADER_FILES
    ${COMMS_PROJECT_DIR}/include/Qcom/Qcom.hpp
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomInline.h
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomGeneralStatus.hpp
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomBroadcastSeek.hpp
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomBroadcast.hpp
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomEgmConfigReq.hpp
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomEgmConfig.hpp
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomGameConfig.hpp
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomGameConfigChange.hpp
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomEgmParameters.hpp
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomPurgeEvents.hpp
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomEvent.hpp
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomProgressiveConfig.hpp
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomExtJPInfo.hpp
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomProgramHashRequest.hpp
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomSystemLockupRequest.hpp
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomCashTicketOutRequestAck.hpp
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomCashTicketInRequestAck.hpp
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomCashOutRequest.hpp
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomEgmGeneralMaintenance.hpp
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomRequestAllLoggedEvents.hpp
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomNoteAcceptorMaintenance.hpp
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomHTPMaintenance.hpp
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomLPAwardAck.hpp
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomGeneralReset.hpp
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomSPAM.hpp
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomTowerLightMaintenance.hpp
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomProgressiveMeters.hpp
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomBetMeters.hpp
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomMultiGameVarMeters.hpp
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomPlayerChoiceMeters.hpp
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomMeterGroupContribution.hpp
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomECTToEGM.hpp
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomECTFromEGMRequest.hpp
    ${COMMS_PROJECT_DIR}/include/Qcom/QcomECTLockupReset.hpp
    )

set(COMMS_QCOM_SOURCE_FILES
    ${COMMS_PROJECT_DIR}/src/Qcom/Qcom.cpp
    ${COMMS_PROJECT_DIR}/src/Qcom/QcomGeneralStatus.cpp
    ${COMMS_PROJECT_DIR}/src/Qcom/QcomBroadcastSeek.cpp
    ${COMMS_PROJECT_DIR}/src/Qcom/QcomBroadcast.cpp
    ${COMMS_PROJECT_DIR}/src/Qcom/QcomEgmConfigReq.cpp
    ${COMMS_PROJECT_DIR}/src/Qcom/QcomEgmConfig.cpp
    ${COMMS_PROJECT_DIR}/src/Qcom/QcomGameConfig.cpp
    ${COMMS_PROJECT_DIR}/src/Qcom/QcomGameConfigChange.cpp
    ${COMMS_PROJECT_DIR}/src/Qcom/QcomEgmParameters.cpp
    ${COMMS_PROJECT_DIR}/src/Qcom/QcomPurgeEvents.cpp
    ${COMMS_PROJECT_DIR}/src/Qcom/QcomEvent.cpp
    ${COMMS_PROJECT_DIR}/src/Qcom/QcomProgressiveConfig.cpp
    ${COMMS_PROJECT_DIR}/src/Qcom/QcomExtJPInfo.cpp
    ${COMMS_PROJECT_DIR}/src/Qcom/QcomProgramHashRequest.cpp
    ${COMMS_PROJECT_DIR}/src/Qcom/QcomSystemLockupRequest.cpp
    ${COMMS_PROJECT_DIR}/src/Qcom/QcomCashTicketOutRequestAck.cpp
    ${COMMS_PROJECT_DIR}/src/Qcom/QcomCashTicketInRequestAck.cpp
    ${COMMS_PROJECT_DIR}/src/Qcom/QcomCashOutRequest.cpp
    ${COMMS_PROJECT_DIR}/src/Qcom/QcomEgmGeneralMaintenance.cpp
    ${COMMS_PROJECT_DIR}/src/Qcom/QcomRequestAllLoggedEvents.cpp
    ${COMMS_PROJECT_DIR}/src/Qcom/QcomNoteAcceptorMaintenance.cpp
    ${COMMS_PROJECT_DIR}/src/Qcom/QcomHTPMaintenance.cpp
    ${COMMS_PROJECT_DIR}/src/Qcom/QcomLPAwardAck.cpp
    ${COMMS_PROJECT_DIR}/src/Qcom/QcomGeneralReset.cpp
    ${COMMS_PROJECT_DIR}/src/Qcom/QcomSPAM.cpp
    ${COMMS_PROJECT_DIR}/src/Qcom/QcomTowerLightMaintenance.cpp
    ${COMMS_PROJECT_DIR}/src/Qcom/QcomProgressiveMeters.cpp
    ${COMMS_PROJECT_DIR}/src/Qcom/QcomBetMeters.cpp
    ${COMMS_PROJECT_DIR}/src/Qcom/QcomMultiGameVarMeters.cpp
    ${COMMS_PROJECT_DIR}/src/Qcom/QcomPlayerChoiceMeters.cpp
    ${COMMS_PROJECT_DIR}/src/Qcom/QcomMeterGroupContribution.cpp
    ${COMMS_PROJECT_DIR}/src/Qcom/QcomECTToEGM.cpp
    ${COMMS_PROJECT_DIR}/src/Qcom/QcomECTFromEGMRequest.cpp
    ${COMMS_PROJECT_DIR}/src/Qcom/QcomECTLockupReset.cpp
    )

set(COMMS_QCOM_QOGR_HEADER_FILES
    ${COMMS_PROJECT_DIR}/include/Qcom/qogr/qogr_crc.h
    ${COMMS_PROJECT_DIR}/include/Qcom/qogr/qogr_qcom.h
    )

set(COMMS_QCOM_QOGR_SOURCE_FILES
    ${COMMS_PROJECT_DIR}/src/Qcom/qogr/qogr_crc.c
    )

source_group("Source Files" FILES ${COMMS_SOURCE_FILES})
source_group("Source Files\\Qcom" FILES ${COMMS_QCOM_SOURCE_FILES})
source_group("Source Files\\Qcom\\qorg" FILES ${COMMS_QCOM_QOGR_SOURCE_FILES})
source_group("Header Files" FILES ${COMMS_HEADER_FILES})
source_group("Header Files\\Qcom" FILES ${COMMS_QCOM_HEADER_FILES})
source_group("Header Files\\Qcom\\qorg" FILES ${COMMS_QCOM_QOGR_HEADER_FILES})

add_definitions(-DCOMMS_SOURCE)

include(${SG_PROJECT_DIR}/cmake/configboost.cmake)

include_directories(${Boost_INCLUDE_DIR})
include_directories(${CORE_PROJECT_DIR}/include)
include_directories(${COMMS_PROJECT_DIR}/include)
include_directories(${COMMS_PROJECT_DIR}/include/Qcom/qogr)

link_directories(${Boost_LIBRARY_DIR})

add_library(${LIB_NAME} SHARED 
	${COMMS_SOURCE_FILES} 
    ${COMMS_QCOM_SOURCE_FILES}
    ${COMMS_QCOM_QOGR_SOURCE_FILES}
    ${COMMS_HEADER_FILES} 
    ${COMMS_QCOM_HEADER_FILES} 
    ${COMMS_QCOM_QOGR_HEADER_FILES}
)

if(SG_COMMSSIM_BUILD_ALL)
    target_link_libraries(${LIB_NAME} core)
else()
    # Build standalone, so no target core, link it manually
    target_link_libraries(${LIB_NAME} debug core${SG_OUTPUT_SUFFIX}_d optimized core${SG_OUTPUT_SUFFIX})
    if(SG_PLATFORM_WINDOWS) # it's suck, match the core lib here
        add_definitions(-DSG_USE_PDCURSES)
    endif()
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
    OUTPUT_NAME ${LIB_NAME}${SG_OUTPUT_SUFFIX}
)

if(SG_COMPILER_MSVC)

    install(TARGETS ${LIB_NAME}
	RUNTIME DESTINATION ${SG_BIN_DIR}
	LIBRARY DESTINATION ${SG_BIN_DIR}
    ARCHIVE DESTINATION ${SG_LIB_DIR})
endif()
