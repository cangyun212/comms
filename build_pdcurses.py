#!/usr/bin/env python
#-*- coding: ascii -*-

from __future__ import print_function
import sys
from build_util import *

def build_pdcurses(build_info):
    for compiler_info in build_info.compilers:
        if build_info.target_platform == "linux":
            pdcurses_cmake = "pdcurses/pdcurses/x11/pdcurses.cmake"
            pdcurses_configh = "pdcurses/pdcurses/x11/config.h"
            if not os.path.exists(pdcurses_cmake) or not os.path.exists(pdcurses_configh):
                cur_dir = os.path.abspath(os.curdir)
                pdcurses_configure = "./configure"
                config_cmd = batch_command(build_info.host_platform)
                if compiler_info.arch == "x64":
                    config_cmd.add_command("%s --x-libraries=\"%s\"" % (pdcurses_configure, "/usr/lib64/"))
                else:
                    config_cmd.add_command("%s --x-libraries=\"%s\"" % (pdcurses_configure, "/usr/lib/"))
                os.chdir("pdcurses/pdcurses/x11")
                if config_cmd.execute() != 0:
                    build_info.quit("Run pdcurses configure script failed.")
                os.chdir(cur_dir)
        build_a_project("pdcurses", "build/pdcurses", build_info, compiler_info, "../../pdcurses/cmake")

def clean_pdcurses():
    bi = build_info()
    clean_a_project("pdcurses", "build/pdcurses", bi)

if __name__ == "__main__":
    bi = build_info()

    print("Building pdcurses...")
    build_pdcurses(bi)
