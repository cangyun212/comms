#!/usr/bin/env python
#-*- coding: ascii -*-

from __future__ import print_function
import sys
from build_util import *

def build_core(build_info):
    for compiler_info in build_info.compilers:
        build_a_project("core", "build/core", build_info, compiler_info, "../../core/cmake")


def clean_core(build_info):
    bi = build_info()
    clean_a_project("core", "build/core", bi)


if __name__ == "__main__":
    bi = build_info()

    print("Building core...")
    build_core(bi)
