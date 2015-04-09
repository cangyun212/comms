#!/usr/bin/env python
#-*- coding: ascii -*-

import os
import sys
import multiprocessing
import subprocess
import argparse
import logging

CURRENT_VERSION = "1.0.0"


class work_context:
    def __init__(self):

        self.parser = argparse.ArgumentParser(description='Build xrender project')
        self.parser.add_argument(
            '-c', '--compiler',
            choices=["gcc"],
            default="",
            help='specify the compiler used to compile the project')
        self.parser.add_argument(
            '-a', '--arch',
            choices=["x86", "x64"],
            nargs='*',
            default="",
            help='specify the target cpu architecture')
        self.parser.add_argument(
            '-g', '--config',
            choices=["Debug", "Release"],
            nargs='*',
            default="",
            help='specif the build configuration')
        self.parser.add_argument(
            '-v', '--verbos',
            action='store_true',
            help='increse output verborsity')
        self.parser.add_argument(
            '--log',
            action='store_true',
            help='create a log file')
        self.parser.add_argument(
            '--version',
            action='version',
            version='%(prog)s ' + CURRENT_VERSION)

        self.args = self.parser.parse_args()

        self.logger = logging.getLogger('Build_csim_Logger')
        self.logger.setLevel(logging.DEBUG)

        if self.logger.handlers:
            self.logger.handlers = []

        ch = logging.StreamHandler()
        formatter = logging.Formatter('%(levelname)s - %(lineno)d: %(message)s')
        ch.setFormatter(formatter)
        self.logger.addHandler(ch)

        ch.setLevel(logging.WARNING)
        if self.args.verbos:
            ch.setLevel(logging.INFO)

        if self.args.log:
            fh = logging.FileHandler('build_project.log')
            fh.setFormatter(formatter)
            self.logger.addHandler(fh)
            fh.setLevel(logging.DEBUG)

    def GetCompiler(self):
        return self.args.compiler

    def GetArch(self):
        return self.args.arch

    def GetBuildConfig(self):
        return self.args.config

    def GetLogger(self):
        return self.logger


try:
    import cfg_build
except:
    cfg_build_f = open("cfg_build.py", "w")
    cfg_build_f.write("""################################################
# !!!! DO NOT DELETE ANY FIELD OF THIS FILE !!!!
################################################

# Compiler name.
#   On Linux, could be "gcc", "auto".
compiler        = "auto"

# Target CPU architecture.
#   On Linux, could be "x86", "x64".
arch            = ("x64", )

# Configuration. Could be "Debug", "Release".
config          = ("Debug", "Release")

# Target platform for cross compiling. Could be "auto".
target          = "auto"
""")
    cfg_build_f.close()
    import cfg_build


# generate build information based on user configuration
class build_info:
    def __init__(self):
        try:
            cfg_build.compiler
        except:
            cfg_build.compiler = "auto"
        try:
            cfg_build.arch
        except:
            cfg_build.arch = ("x64", )
        try:
            cfg_build.config
        except:
            cfg_build.config = ("Debug", "Release")
        try:
            cfg_build.target
        except:
            cfg_build.target = "auto"

        self.context = work_context()
        self.logger = self.context.GetLogger()

        compiler = self.context.GetCompiler()
        archs = self.context.GetArch()
        cfg = self.context.GetBuildConfig()

        env = os.environ

        host_platform = sys.platform
        if 0 == host_platform.find("linux"):
            host_platform = "linux"
        if "auto" == cfg_build.target:
            target_platform = host_platform
        else:
            target_platform = cfg_build.target

        prefer_static = False

        self.host_platform = host_platform
        self.target_platform = target_platform
        self.prefer_static = prefer_static

        if "" == compiler:
            if ("" == cfg_build.compiler) or ("auto" == cfg_build.compiler):
                if "linux" == target_platform:
                    compiler = "gcc"
                else:
                    self.logger.error("Unsupported host platform")
            else:
                compiler = cfg_build.compiler

        # default architecture is x64
        if "" == archs:
            archs = cfg_build.arch
            if "" == archs:
                archs = ("x64", )

        # default configuration are debug and release
        if "" == cfg:
            cfg = cfg_build.config
            if "" == cfg:
                cfg = ("Debug", "Release")

        arch_list = []
        if "gcc" == compiler:
            compiler_name = "gcc"
            compiler_version = self.retrive_gcc_version()
            for arch in archs:
                arch_list.append((arch, "Unix Makefiles"))
        else:
            compiler_name = ""
            compiler_version = 0
            self.logger.error("Wrong configuration")

        self.compiler_name = compiler_name
        self.compiler_version = compiler_version
        self.arch_list = arch_list
        self.cfg = cfg

        # get gcc version
    def retrive_gcc_version(self):
        gcc_ver = subprocess.check_output(["gcc", "-dumpversion"])
        gcc_ver_components = gcc_ver.split(".")
        return int(gcc_ver_components[0] + gcc_ver_components[1])

# create a template batch script to run the commond specified by user 
class batch_command:
    def __init__(self, host_platform):
        self.commands_ = []
        self.host_platform_ = host_platform

    def add_command(self, cmd):
        self.commands_ += [cmd]

    def execute(self):
        batch_file = "xbuild."
        if "linux" == self.host_platform_:
            batch_file += "sh"

        batch_f = open(batch_file, "w")
        batch_f.writelines([cmd_line + "\n" for cmd_line in self.commands_])
        batch_f.close()
        if "linux" == self.host_platform_:
            os.system("chmod 777 " + batch_file)
            ret_code = os.system("./" + batch_file)
        os.remove(batch_file)
        return ret_code

# use cmake to generate a project file and compile the project
def build_a_project(name, build_path, build_info, compiler_arch, need_install=False, additional_options=""):
    curdir = os.path.abspath(os.curdir)

    if "gcc" == build_info.compiler_name:
        make_name = "make -j%d" % multiprocessing.cpu_count()

        for config in build_info.cfg: # debug or realase or both
            build_dir = "%s/build/%s%d_%s_%s-%s" % (build_path, build_info.compiler_name, build_info.compiler_version, build_info.target_platform, compiler_arch[0], config)
            if not os.path.exists(build_dir):
                os.makedirs(build_dir)

            os.chdir(build_dir)

            additional_options += " -DSG_ARCH_NAME:STRING=\"%s\"" % compiler_arch[0]  # overide the cmake value here
            additional_options += " -DCMAKE_BUILD_TYPE:STRING=\"%s\"" % config
            # execute cmake command here
            cmake_cmd = batch_command(build_info.host_platform)
            cmake_cmd.add_command('cmake -G "%s" %s %s' % (compiler_arch[1], additional_options, "../../cmake"))
            if cmake_cmd.execute() != 0:
                build_info.logger.error("Config %s failed.", name)
            
            install_str = ""
            if need_install and (not build_info.prefer_static):
                install_str = "install"
            build_cmd = batch_command(build_info.host_platform)
            build_cmd.add_command("%s %s" % (make_name, install_str)) #make command construct here
            build_cmd.add_command('if (($? != 0)); then exit 1; fi')
            if build_cmd.execute() != 0:
                build_info.logger.error("Build %s failed.", name)

            os.chdir(curdir)

#remove those folders directly
def clean_a_project(name, build_path, build_info, compiler_arch):
    import shutil

    if "gcc" == build_info.compiler_name:
        for config in build_info.cfg:
            build_dir = "%s/build/%s%d_%s_%s-%s" % (build_path, build_info.compiler_name, build_info.compiler_version, build_info.target_platform, compiler_arch[0], config)
            if os.path.exists(build_dir):
                shutil.rmtree(build_dir)

    lib_dir = "%s/lib/%s_%s" % (build_path, build_info.target_platform, compiler_arch[0])
    if os.path.exists(lib_dir):
        shutil.rmtree(lib_dir)
    bin_dir = "%s/bin/%s_%s" % (build_path, build_info.target_platform, compiler_arch[0])
    if os.path.exists(bin_dir):
        shutil.rmtree(bin_dir)


def copy_to_dst(src_name, dst_dir):
    print "Copy %s to %s" % (src_name, dst_dir)
    import shutil
    shutil.copy2(src_name, dst_dir)
