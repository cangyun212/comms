#!/usr/bin/env python
#-*- coding: ascii -*-

from build_util import *


def build_simulator_test(build_info):
    for arch in build_info.arch_list:
        build_a_project("simulator_test", "simulator_test", build_info, arch)
    
    dirname = "simulator_test/build/"
    dirname += "%s%d_%s_%s-%s" % (build_info.compiler_name, build_info.compiler_version, build_info.target_platform, arch[0], build_info.cfg[0])
    print dirname
    os.chdir(dirname)
    build_cmd = batch_command(build_info.host_platform)
    build_cmd.add_command('./simulator_test')
    build_cmd.execute()
    os.chdir("../../")


def clean_simulator_test(build_info):
    for arch in build_info.arch_list:
        clean_a_project("simulator_test", "simulator_test", build_info, arch)

if __name__ == "__main__":
    bi = build_info()

    print "Building simulator..."
    build_simulator_test(bi)
