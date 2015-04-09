#!/usr/bin/env python
#-*- coding: ascii -*-

import os
from build_thirdparty import clean_thirdparty_libs
from build_core import *
from build_comms import *
from build_simulator import *


def clean_bin_files(build_info):
    import glob

    for arch in build_info.arch_list:
        platform_dir = "%s_%s" % (build_info.target_platform, arch[0])
        dst_dir = "../bin/%s/" % platform_dir

        if os.path.exists(dst_dir):
            for fname in glob.iglob("%s/*" % (dst_dir)):
                os.remove(fname)
            #recreate dummy file
            dummy_file = dst_dir + "dummy"
            df = open(dummy_file, "w")
            df.write(" \n")
            df.close()

if __name__ == "__main__":
    bi = build_info()

    print "Cleaning thirdparty libs...\n"
    clean_thirdparty_libs(bi)

    print "Cleaning bin files...\n"
    clean_bin_files(bi)

    print "Cleaning core lib..\n"
    clean_core(bi)

    print "Cleaning comms lib...\n"
    clean_comms(bi)

    print "Cleaning simulator...\n"
    clean_simulator(bi)
