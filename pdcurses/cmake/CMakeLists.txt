Project(pdcurses)

set(LIB_NAME pdcurses)

cmake_minimum_required(VERSION 2.8.6)

if(COMMAND cmake_policy)
	CMAKE_POLICY(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set(PDCURSES_MAJOR_VERSION 3)
set(PDCURSES_MINOR_VERSION 40)
set(PDCURSES_VERSION ${PDCURSES_MAJOR_VERSION}.${PDCURSES_MINOR_VERSION})

set(PDCURSES_PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(SG_PROJECT_DIR "${PDCURSES_PROJECT_DIR}/..")

include(${SG_PROJECT_DIR}/cmake/common.cmake)
include(${SG_PROJECT_DIR}/cmake/compiler.cmake)

set(SG_LIB_DIR "${SG_PROJECT_DIR}/lib/${SG_TARGET_PLATFORM_NAME}")
set(SG_BIN_DIR "${SG_PROJECT_DIR}/bin/${SG_TARGET_PLATFORM_NAME}")

SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

if(MSVC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W3 /WX-")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /WX-")
else()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error")
endif()

set(PDCURSES_HEADER_FILES
    "${PDCURSES_PROJECT_DIR}/pdcurses/curses.h"
    "${PDCURSES_PROJECT_DIR}/pdcurses/panel.h"
    "${PDCURSES_PROJECT_DIR}/pdcurses/term.h"
    "${PDCURSES_PROJECT_DIR}/pdcurses/curspriv.h")

set(PDCURSES_SOURCE_FILES
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/addch.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/addchstr.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/addstr.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/attr.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/beep.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/bkgd.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/border.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/clear.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/color.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/debug.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/delch.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/deleteln.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/deprec.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/getch.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/getstr.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/getyx.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/inch.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/inchstr.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/initscr.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/inopts.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/insch.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/insstr.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/instr.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/kernel.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/keyname.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/mouse.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/move.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/outopts.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/overlay.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/pad.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/panel.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/printw.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/refresh.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/scanw.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/scr_dump.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/scroll.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/slk.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/termattr.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/terminfo.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/touch.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/util.c"
    "${PDCURSES_PROJECT_DIR}/pdcurses/pdcurses/window.c"
    )

if(SG_PLATFORM_WINDOWS)
    set(PDCURSES_HEADER_FILES 
        ${PDCURSES_HEADER_FILES} 
        "${PDCURSES_PROJECT_DIR}/pdcurses/win32/pdcwin.h")

    set(PDCURSES_SOURCE_FILES
        ${PDCURSES_SOURCE_FILES}
        "${PDCURSES_PROJECT_DIR}/pdcurses/win32/pdcclip.c"
        "${PDCURSES_PROJECT_DIR}/pdcurses/win32/pdcdisp.c"
        "${PDCURSES_PROJECT_DIR}/pdcurses/win32/pdcgetsc.c"
        "${PDCURSES_PROJECT_DIR}/pdcurses/win32/pdckbd.c"
        "${PDCURSES_PROJECT_DIR}/pdcurses/win32/pdcscrn.c"
        "${PDCURSES_PROJECT_DIR}/pdcurses/win32/pdcsetsc.c"
        "${PDCURSES_PROJECT_DIR}/pdcurses/win32/pdcutil.c"
        )
else()
    set(PDCURSES_HEADER_FILES
        ${PDCURSES_HEADER_FILES}
        "${PDCURSES_PROJECT_DIR}/pdcurses/x11/compose.h"
        #"${PDCURSES_PROJECT_DIR}/pdcurses/x11/config.h"
        "${PDCURSES_PROJECT_DIR}/pdcurses/x11/ncurses_cfg.h"
        "${PDCURSES_PROJECT_DIR}/pdcurses/x11/pdcx11.h"
        "${PDCURSES_PROJECT_DIR}/pdcurses/x11/ScrollBox.h"
        "${PDCURSES_PROJECT_DIR}/pdcurses/x11/ScrollBoxP.h"
        )

    set(PDCURSES_SOURCE_FILES
        ${PDCURSES_SOURCE_FILES}
        "${PDCURSES_PROJECT_DIR}/pdcurses/x11/pdcclip.c"
        "${PDCURSES_PROJECT_DIR}/pdcurses/x11/pdcdisp.c"
        "${PDCURSES_PROJECT_DIR}/pdcurses/x11/pdcgetsc.c"
        "${PDCURSES_PROJECT_DIR}/pdcurses/x11/pdckbd.c"
        "${PDCURSES_PROJECT_DIR}/pdcurses/x11/pdcscrn.c"
        "${PDCURSES_PROJECT_DIR}/pdcurses/x11/pdcsetsc.c"
        "${PDCURSES_PROJECT_DIR}/pdcurses/x11/pdcutil.c"
        "${PDCURSES_PROJECT_DIR}/pdcurses/x11/pdcx11.c"
        "${PDCURSES_PROJECT_DIR}/pdcurses/x11/sb.c"
        "${PDCURSES_PROJECT_DIR}/pdcurses/x11/ScrollBox.c"
        "${PDCURSES_PROJECT_DIR}/pdcurses/x11/x11.c"
        )

    set(PDCURSES_RES_FILES 
        "${PDCURSES_PROJECT_DIR}/pdcurses/x11/big_icon.xbm"
        "${PDCURSES_PROJECT_DIR}/pdcurses/x11/little_icon.xbm"

        )

    source_group("Resource Files" FILES ${PDCURSES_RES_FILES})

endif()

source_group("Source Files" FILES ${PDCURSES_SOURCE_FILES})
source_group("HEADER FILES" FILES ${PDCURSES_HEADER_FILES})

if(SG_PLATFORM_LINUX)
#    if(SG_ARCH_NAME MATCHES "x64")
#        set(PDCURSES_X11_LIBDIR "/usr/lib64/")
#    else()
#        set(PDCURSES_X11_LIBDIR "/usr/lib/")
#    endif()

    set(PDCURSES_CMAKE "${PDCURSES_PROJECT_DIR}/pdcurses/x11/pdcurses.cmake")
#    add_custom_command(OUTPUT ${PDCURSES_CMAKE}
#        COMMAND cd "${PDCURSES_PROJECT_DIR}/pdcurses/x11"
#        COMMAND ${PDCURSES_PROJECT_DIR}/pdcurses/x11/configure --x-libraries="${PDCURSES_X11_LIBDIR}"
#        COMMAND cd -
#        DEPENDS "${PDCURSES_PROJECT_DIR}/pdcurses/x11/configure"
#        COMMENT "Generating config.h and pdcurses.cmake")
#
#    add_custom_target(RunConfigure DEPENDS ${PDCURSES_CMAKE} COMMENT "checking if need to re-run configure")

    include(${PDCURSES_CMAKE})
endif()


include_directories("${PDCURSES_PROJECT_DIR}/pdcurses")
if(SG_PLATFORM_WINDOWS)
    include_directories("${PDCURSES_PROJECT_DIR}/pdcurses/win32")
else()
    include_directories("${PDCURSES_PROJECT_DIR}/pdcurses/x11")
    include_directories(${PDCURSES_XINC_DIR})
endif()

if(SG_PLATFORM_LINUX)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${PDCURSES_DEFS} ${PDCURSES_SYS_DEFS} -DXCURSES -D_GNU_SOURCE")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PDCURSES_DEFS} ${PDCURSES_SYS_DEFS} -DXCURSES -D_GNU_SOURCE")
endif()

add_library(${LIB_NAME} STATIC ${PDCURSES_HEADER_FILES} ${PDCURSES_SOURCE_FILES})

set(LIB_OUTPUT_NAME ${LIB_NAME}${SG_OUTPUT_SUFFIX})

set_target_properties(${LIB_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${SG_LIB_DIR}
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${SG_LIB_DIR}
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${SG_LIB_DIR}
    ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${SG_LIB_DIR}
    ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL ${SG_LIB_DIR}
    PROJECT_LABEL ${LIB_NAME}
    DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
    OUTPUT_NAME ${LIB_OUTPUT_NAME}
    )

if(SG_PLATFORM_WINDOWS)
    install(TARGETS ${LIB_NAME}
        RUNTIME DESTINATION ${SG_BIN_DIR}
        ARCHIVE DESTINATION ${SG_LIB_DIR}
        LIBRARY DESTINATION ${SG_BIN_DIR}
        )
endif()

