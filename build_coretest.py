#!/usr/bin/env python
#-*- coding: ascii -*-

from __future__ import print_function
import sys
from build_util import *

def build_coretest(build_info):
    for compiler_info in build_info.compilers:
        build_a_project("coretest", "build/coretest", build_info, compiler_info, "../../coretest/cmake")

def clean_coretest():
    bi = build_info()
    clean_a_project("coretest", "build/coretest", bi)

if __name__ == "__main__":
    bi = build_info()

    print("Building coretest...")
    build_coretest(bi)
