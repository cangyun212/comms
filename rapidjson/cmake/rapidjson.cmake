if(__RAPIDJSON_CMAKE__)
    return()
endif()

set(__RAPIDJSON_CMAKE__)

set(__RAPIDJSON_DIR ${CMAKE_CURRENT_LIST_DIR}/..)

include(${__RAPIDJSON_DIR}/../cmake/common.cmake)

set(RAPIDJSON_LIB_NAME RAPIDJSON)

set(${RAPIDJSON_LIB_NAME}_INCLUDE_DIR "${__RAPIDJSON_DIR}/include" CACHE PATH "Path to include for rapidjson" FORCE)
#set(${RAPIDJSON_LIB_NAME}_INCLUDE_DIRS "${${RAPIDJSON_LIB_NAME}_INCLUDE_DIR}" CACHE PATH "Path to include for rapidjson" FORCE)


