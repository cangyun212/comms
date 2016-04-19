#!/usr/bin/env python
#-*- coding: ascii -*-

import os, sys, multiprocessing, subprocess, argparse, logging

CURRENT_VERSION = "1.0.0"

try:
    import cfg_build
except:
    print("Generating cfg_build.py ...")
    import shutil
    shutil.copyfile("cfg_build_default.py", "cfg_build.py")
    import cfg_build

class work_context:
    def __init__(self):

        self.parser = argparse.ArgumentParser(description='Build SG project')
        self.parser.add_argument(
            '-c', '--compiler',
            choices=["gcc", "vc140", "vc120", "auto"],
            default="auto",
            help='specify the compiler used to compile the project')
        self.parser.add_argument(
            '-a', '--arch',
            choices=["x86", "x64"],
            nargs='*',
            default=("x64", ),
            help='specify the target cpu architecture')
        self.parser.add_argument(
            '-g', '--config',
            choices=["Debug", "Release", "RelWithDebInfo", "MinSizeRel"],
            nargs='*',
            default=("Debug", ),
            help='specify the build configuration')
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

        self.logger = logging.getLogger('Build_SG_Logger')
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

class compiler_info:
    def __init__(self, arch, gen_name, toolset, target_platform):
        self.arch = arch
        self.generator = gen_name
        self.toolset = toolset

        self.is_windows = False
        self.is_windows_desktop = False
        self.is_linux = False

        if "win" == target_platform:
            self.is_windows = True
            self.is_windows_desktop = True
        elif "linux" == target_platform:
            self.is_linux = True

        self.is_dev_platform = (self.is_windows_desktop or self.is_linux)

class build_info:
    def __init__(self):
        try:
            cfg_build.compiler
        except:
            cfg_build.compiler = "auto"
        try:
            cfg_build.toolset
        except:
            cfg_build.toolset = "auto"
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
        target_platform = "auto"
        
        env = os.environ

        host_platform = sys.platform
        if 0 == host_platform.find("win"):
            host_platform = "win"
        elif 0 == host_platform.find("linux"):
            host_platform = "linux"

        if "auto" == target_platform:
            if "auto" == cfg_build.target:
                target_platform = host_platform
            else:
                target_platform = cfg_build.target

        prefer_static = False

        self.host_platform = host_platform
        self.target_platform = target_platform
        self.prefer_static = prefer_static

        if "auto" == compiler:
            if "auto" == cfg_build.compiler:
                if 0 == target_platform.find("win"):
                    if "VS140COMNTOOLS" in env:
                        compiler = "vc140"
                    elif "VS120COMNTOOLS" in env:
                        compiler = "vc120"
                elif "linux" == target_platform:
                    compiler = "gcc"
                else:
                    quit("Unsupported target platform.")
            else:
                compiler = cfg_build.compiler

        toolset = cfg_build.toolset
        if "auto" == toolset:
            if 0 == target_platform.find("win"):
                if "vc140" == compiler:
                    toolset = "v140"
                elif "vc120" == compiler:
                    toolset = "v120"
                
        if "" == cfg:
            cfg = cfg_build.config
            if "" == cfg:
                cfg = ("Debug", "Release")

        multi_config = False
        compilers = []
        if "vc140" == compiler:
            compiler_name = "vc"
            compiler_version = 140
            multi_config = True
            for arch in archs:
                if "x86" == arch:
                    gen_name = "Visual Studio 14"
                elif "x64" == arch:
                    gen_name = "Visual Studio 14 Win64"
                compilers.append(compiler_info(arch, gen_name, toolset, target_platform))
        elif "vc120" == compiler:
            compiler_name = "vc"
            compiler_version = 120
            multi_config = True
            for arch in archs:
                if "x86" == arch:
                    gen_name = "Visual Studio 12"
                elif "x64" == arch:
                    gen_name = "Visual Studio 12 Win64"
                compilers.append(compiler_info(arch, gen_name, toolset, target_platform))
        elif "gcc" == compiler:
            compiler_name = "gcc"
            self.toolset = toolset
            compiler_version = self.retrive_gcc_version()
            gen_name = "Unix Makefiles"
            for arch in archs:
                compilers.append(compiler_info(arch, gen_name, toolset, target_platform))
        else:
            compiler_name = ""
            compiler_version = 0
            quit("Unsupported compiler.")

        if "vc" == compiler_name:
            self.proj_ext_name = "vcxproj"
        else:
            self.proj_ext_name = ""

        self.compiler_name = compiler_name
        self.compiler_version = compiler_version
        self.multi_config = multi_config
        self.compilers = compilers
        self.cfg = cfg

    def msvc_add_build_command(self, batch_cmd, sln_name, proj_name, config, arch = ""):
        batch_cmd.add_command('@SET VisualStudioVersion=%d.0' % (self.compiler_version / 10))
        if len(proj_name) != 0:
            file_name = "%s.%s" % (proj_name, self.proj_ext_name)
        else:
            file_name = "%s.sln" % sln_name
        config_str = "Configuration=%s" % config
        if len(arch) != 0:
            config_str = "%s,Platform=%s" % (config_str, arch)
        batch_cmd.add_command('@MSBuild %s /nologo /m /v:m /p:%s' % (file_name, config_str))
        batch_cmd.add_command('@if ERRORLEVEL 1 exit /B 1')
        
    def retrive_gcc_version(self):
        gcc_ver = subprocess.check_output(["gcc", "-dumpversion"])
        gcc_ver_components = gcc_ver.split(".")
        return int(gcc_ver_components[0] + gcc_ver_components[1])

    def quit(self, message):
        self.logger.error(message)
        os.system("pause")
        sys.exit(1)

class batch_command:
    def __init__(self, host_platform):
        self.commands_ = []
        self.host_platform_ = host_platform

    def add_command(self, cmd):
        self.commands_ += [cmd]

    def execute(self):
        batch_file = "vgit_build."
        if "win" == self.host_platform_:
            batch_file += "bat"
        else:
            batch_file += "sh"
        batch_f = open(batch_file, "w")
        batch_f.writelines([cmd_line + "\n" for cmd_line in self.commands_])
        batch_f.close()
        if "win" == self.host_platform_:
            ret_code = os.system(batch_file)
        else:
            os.system("chmod 777 " + batch_file)
            ret_code = os.system("./" + batch_file)
        os.remove(batch_file)
        return ret_code

def build_a_project(name, build_path, build_info, compiler_info, script_path, need_install = False, additional_options = ""):
    curdir = os.path.abspath(os.curdir)

    toolset_name = ""
    if "vc" == build_info.compiler_name:
        toolset_name = "-T %s" % compiler_info.toolset

    additional_options += " -DSG_COMPILER_NAME:STRING=\"%s\"" % build_info.compiler_name
    additional_options += " -DSG_COMPILER_VERSION=%d" % build_info.compiler_version
    additional_options += " -DSG_PLATFORM_NAME:STRING=\"%s\"" % build_info.target_platform
    additional_options += " -DSG_ARCH_NAME:STRING=\"%s\"" % compiler_info.arch

    if build_info.multi_config:
        if "vc" == build_info.compiler_name:
            if "x86" == compiler_info.arch:
                vc_option = "x86"
                vc_arch = "Win32"
            elif "x64" == compiler_info.arch:
                vc_option = "x86_amd64"
                vc_arch = "x64"
            else:
                build_info.quit("Unsupported architecture on windows.")

        build_dir = "%s/%s%d_%s_%s" % (build_path, build_info.compiler_name, build_info.compiler_version, build_info.target_platform, compiler_info.arch)
        if not os.path.exists(build_dir):
            os.makedirs(build_dir)

        os.chdir(build_dir)

        cmake_cmd = batch_command(build_info.host_platform)
        cmake_cmd.add_command('cmake -G "%s" %s %s %s' % (compiler_info.generator, toolset_name, additional_options, "../%s" % script_path))
        if cmake_cmd.execute() != 0:
            build_info.quit("Config %s failed." % name)

        build_cmd = batch_command(build_info.host_platform)
        if "vc" == build_info.compiler_name:
            build_cmd.add_command('@CALL "%%VS%dCOMNTOOLS%%..\\..\\VC\\vcvarsall.bat" %s' % (build_info.compiler_version, vc_option))
        for config in build_info.cfg:
            if "vc" == build_info.compiler_name:
                build_info.msvc_add_build_command(build_cmd, name, "ALL_BUILD", config, vc_arch)
                if need_install:
                    build_info.msvc_add_build_command(build_cmd, name, "INSTALL", config, vc_arch)
        if build_cmd.execute() != 0:
            build_info.quit("Build %s failed." % name)

        os.chdir(curdir)
    else:
        make_name = "make -j%d" % multiprocessing.cpu_count()

        for config in build_info.cfg:
            build_dir = "%s/%s%d_%s_%s-%s" % (build_path, build_info.compiler_name, build_info.compiler_version, build_info.target_platform, compiler_info.arch, config)
            if not os.path.exists(build_dir):
                os.makedirs(build_dir)

            os.chdir(build_dir)
            
            config_options = "-DCMAKE_BUILD_TYPE:STRING=\"%s\"" % config
            
            cmake_cmd = batch_command(build_info.host_platform)
            cmake_cmd.add_command('cmake -G "%s" %s %s %s %s' % (compiler_info.generator, toolset_name, additional_options, config_options, "../%s" % script_path))
            if cmake_cmd.execute() != 0:
                build_info.quit("Config %s failed." % name)		

            install_str = ""
            if need_install and (not build_info.prefer_static):
                install_str = "install"
            build_cmd = batch_command(build_info.host_platform)
            if "win" == build_info.host_platform:
                build_cmd.add_command("@%s %s" % (make_name, install_str))
                build_cmd.add_command('@if ERRORLEVEL 1 exit /B 1')
            else:
                build_cmd.add_command("%s %s" % (make_name, install_str))
                build_cmd.add_command('if [ $? -ne 0 ]; then exit 1; fi')
            if build_cmd.execute() != 0:
                build_info.quit("Build %s failed." % name)

            os.chdir(curdir)

def clean_a_project(name, build_path, build_info):
    import shutil

    if build_info.multi_config:
        build_dir = "%s/%s%d_%s_%s" % (build_path, build_info.compiler_name, build_info.compiler_version, build_info.target_platform, compiler_info.arch)
        if os.path.exists(build_dir):
            shutil.rmtree(build_dir);
    else:
        for config in build_info.cfg:
            build_dir = "%s/%s%d_%s_%s-%s" % (build_path, build_info.compiler_name, build_info.compiler_version, build_info.target_platform, compiler_info.arch, config)
            if os.path.exists(build_dir):
                shutil.rmtree(build_dir);
    # TODO: remove bin & lib
