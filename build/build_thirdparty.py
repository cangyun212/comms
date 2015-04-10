#!/usr/bin/env python
#-*- coding: ascii -*-

import os
from build_util import *


def build_Boost(build_info, compiler_arch):
    b2_name = ""
    os.chdir("../thirdparty/boost")
    if "linux" == build_info.host_platform:
        b2_name = "./b2"
        if not os.path.exists(b2_name):
            os.system("./bootstrap.sh")

    boost_toolset = "gcc"

    options = ""

    if ("x86" == compiler_arch[0]):
        options += "address-model=32"
    elif ("x64" == compiler_arch[0]):
        options += "address-model=64"

    options += " --without-date_time --disable-filesystem2"

    if ("gcc" == build_info.compiler_name):
        if (build_info.compiler_version < 47):
            options += " cxxflags=-std=c++0x linkflags=-std=c++0x"
        else:
            options += " cxxflags=-std=c++11 linkflags=-std=c++11"

    if ("Debug" in build_info.cfg):
        options += " variant=debug"
    if ("Release" in build_info.cfg):
        options += " variant=release"

    build_cmd = batch_command(build_info.host_platform)
    if build_info.prefer_static:
        link = "static"
    else:
        link = "shared"
        options += " define=BOOST_ATOMIC_DYN_LINK"

    build_cmd.add_command('%s --toolset=%s --stagedir=./lib/%s_%s --builddir=./ --layout=versioned %s link=%s runtime-link=%s threading=multi stage' % (b2_name, boost_toolset, build_info.target_platform, compiler_arch[0], options, link, link))
    if build_cmd.execute() != 0:
        build_info.logger.error("Build boost failed.")

    os.chdir("../../build")

def build_UnitTestPP( build_info , compiler_arch ):
    if "linux" == build_info.host_platform:
        os.chdir("../thirdparty")
        build_cmd = batch_command(build_info.host_platform)
        build_cmd.add_command('make -C unittestpp LDFLAGS= ')
        if build_cmd.execute() != 0:
            build_info.logger.error("Build unittestpp failed.")
        build_cmd = batch_command(build_info.host_platform)
        build_cmd.add_command('rm -rf bin lib ')
        build_cmd.execute()
        os.chdir("../build")


def build_thirdparty_libs(build_info):
    import glob

    if "linux" == build_info.target_platform:
        dll_suffix = "so"

    for arch in build_info.arch_list:
        platform_dir = "%s_%s" % (build_info.target_platform, arch[0])
        dst_dir = "../bin/%s/" % platform_dir

        print "\nBuilding boost...\n"
        build_Boost(build_info, arch)
        build_UnitTestPP( build_info , arch )

        if not build_info.prefer_static:
            for fname in glob.iglob("../thirdparty/boost/lib/%s_%s/lib/*.%s" % (build_info.target_platform, arch[0], dll_suffix)):
                copy_to_dst(fname, dst_dir)


def clean_boost(build_info):
    import shutil

    os.chdir("../thirdparty/boost")

    if os.path.exists("bin.v2"):
        shutil.rmtree("bin.v2")

    if "linux" == build_info.target_platform:
        if os.path.exists("b2"):
            os.remove("b2")
        if os.path.exists("bjam"):
            os.remove("bjam")

    if os.path.exists("bootstrap.log"):
        os.remove("bootstrap.log")

    if os.path.exists("project-config.jam"):
        os.remove("project-config.jam")

    for arch in build_info.arch_list:
        platform_dir = "%s_%s" % (build_info.target_platform, arch[0])
        dst_dir = "lib/%s" % platform_dir

        if os.path.exists(dst_dir):
            shutil.rmtree(dst_dir)

        build_arch_str = ""
        if "x86" == arch[0] or "x64" == arch[0]:
            build_arch_str = "x86_64"

        build_bin_dir = ""
        if "linux" == build_info.target_platform:
            build_bin_dir = "tools/build/v2/engine/bin.%s%s" % (build_info.target_platform, build_arch_str)

        build_bootstrap_dir = "tools/build/v2/engine/bootstrap"

        if os.path.exists(build_bin_dir):
            shutil.rmtree(build_bin_dir)
        if os.path.exists(build_bootstrap_dir):
            shutil.rmtree(build_bootstrap_dir)

    os.chdir("../../build")

def clean_unittestpp(build_info):
    if "linux" == build_info.target_platform:
        print "Clean unittestpp..." 
        os.chdir("../thirdparty/unittestpp")
        build_cmd = batch_command(build_info.host_platform)
        build_cmd.add_command('rm -rf bin lib')
        build_cmd.execute()
        build_cmd = batch_command(build_info.host_platform)
        build_cmd.add_command('make  clean')
        build_cmd.execute()
        os.chdir("../../build")

def clean_thirdparty_libs(build_info):
    print "Cleaning boost...\n"
    clean_boost(build_info)
    clean_unittestpp(build_info)


if __name__ == "__main__":
    bi = build_info()

    build_thirdparty_libs(bi)
