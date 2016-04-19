set(LIB_NAME system)

set(SOURCE_FILES
    ${BOOST_PROJECT_DIR}/libs/system/src/error_code.cpp
    )

add_definitions(-DBOOST_SYSTEM_DYN_LINK=1)

SETUP_BOOST_LIBRARY(${LIB_NAME})
