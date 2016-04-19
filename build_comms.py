#!/usr/bin/env python
#-*- coding: ascii -*-

from __future__ import print_function
import sys
from build_util import *

def build_comms(build_info):
    for compiler_info in build_info.compilers:
        build_a_project("comms", "build/comms", build_info, compiler_info, "../../comms/cmake")

def clean_comms(build_info):
    bi = build_info()
    clean_a_project("comms", "build/comms", bi)


if __name__ == "__main__":
    bi = build_info()

    print("Building comms lib...")
    build_comms(bi)
