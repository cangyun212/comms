set(BOOST_ROOT "${SG_PROJECT_DIR}/boost")
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(BOOST_LIBRARYDIR "${SG_BIN_DIR}")
elseif(WIN32)
    set(BOOST_LIBRARYDIR "${SG_LIB_DIR}")
endif()
set(BOOST_COMPONENTS "")
set(Boost_NO_SYSTEM_PATHS ON)

set(BOOST_COMPONENTS program_options filesystem system)

find_package(Boost COMPONENTS ${BOOST_COMPONENTS})

if(NOT Boost_LIBRARY_DIR)
    set(Boost_LIBRARY_DIR ${SG_LIB_DIR} ${SG_BIN_DIR})
endif()
