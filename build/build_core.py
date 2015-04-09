#!/usr/bin/env python
#-*- coding: ascii -*-

from build_util import *


def build_core(build_info):
    for arch in build_info.arch_list:
        build_a_project("core", "core", build_info, arch)


def clean_core(build_info):
    for arch in build_info.arch_list:
        clean_a_project("core", "core", build_info, arch)


if __name__ == "__main__":
    bi = build_info()

    print "Building xcommon..."
    build_core(bi)
