set(LIB_NAME filesystem)

set(SOURCE_FILES
    ${BOOST_PROJECT_DIR}/libs/filesystem/src/codecvt_error_category.cpp
    ${BOOST_PROJECT_DIR}/libs/filesystem/src/operations.cpp
    ${BOOST_PROJECT_DIR}/libs/filesystem/src/path.cpp
    ${BOOST_PROJECT_DIR}/libs/filesystem/src/path_traits.cpp
    ${BOOST_PROJECT_DIR}/libs/filesystem/src/portability.cpp
    ${BOOST_PROJECT_DIR}/libs/filesystem/src/unique_path.cpp
    ${BOOST_PROJECT_DIR}/libs/filesystem/src/utf8_codecvt_facet.cpp
    ${BOOST_PROJECT_DIR}/libs/filesystem/src/windows_file_codecvt.cpp
    )

add_definitions(-DBOOST_FILESYSTEM_DYN_LINK=1)

if(NOT MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -frtti")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -frtti")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -frtti")

    set(EXTRA_LINKED_LIBRARIES ${EXTRA_LINKED_LIBRARIES}
        debug boost_system${BOOST_LIB_NAME_SUFFIX_DEBUG}
        optimized boost_system${BOOST_LIB_NAME_SUFFIX_OPTIMIZED})
endif()

SETUP_BOOST_LIBRARY(${LIB_NAME})

add_dependencies(${LIB_NAME} system)
