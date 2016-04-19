#!/usr/bin/env python
#-*- coding: ascii -*-

from __future__ import print_function
import sys
from build_util import *

def build_boost(build_info):
    for compiler_info in build_info.compilers:
        build_a_project("boost", "build/boost", build_info, compiler_info, "../../boost/cmake")

def clean_boost():
    bi = build_info()
    clean_a_project("boost", "build/boost", bi)

if __name__ == "__main__":
    bi = build_info()

    print("Building boost...")
    build_boost(bi)
