#!/usr/bin/env python
#-*- coding: ascii -*-

from build_util import *


def build_comms(build_info):
    for arch in build_info.arch_list:
        build_a_project("comms", "comms", build_info, arch)


def clean_comms(build_info):
    for arch in build_info.arch_list:
        clean_a_project("comms", "comms", build_info, arch)


if __name__ == "__main__":
    bi = build_info()

    print "Building comms lib..."
    build_comms(bi)
