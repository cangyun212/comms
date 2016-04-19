#!/usr/bin/env python
#-*- coding: ascii -*-

from __future__ import print_function
import sys
from build_util import *

def build_simulator(build_info):
    for compiler_info in build_info.compilers:
        build_a_project("simulator", "build/simulator", build_info, compiler_info, "../../simulator/cmake")

def clean_simulator(build_info):
    bi = build_info()
    clean_a_project("simulator", "build/simulator", bi)

if __name__ == "__main__":
    bi = build_info()

    print("Building simulator...")
    build_simulator(bi)
