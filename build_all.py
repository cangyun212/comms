#!/usr/bin/env python
#-*- coding: ascii -*-

from __future__ import print_function
import sys
from build_util import *
from build_boost import *
from build_pdcurses import * 

def build_all(build_info):
    for compiler_info in build_info.compilers:
        build_a_project("all", "build/all", build_info, compiler_info, "../../all")

def clean_all():
    bi = build_info()
    clean_a_project("all", "build/all", bi)

if __name__ == "__main__":
    bi = build_info()

    print("Building boost...")
    build_boost(bi)

    print("Building pdcurses...")
    build_pdcurses(bi)

    print("Building all...")
    build_all(bi) 
