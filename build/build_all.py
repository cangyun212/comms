#!/usr/bin/env python
#-*- coding: ascii -*-

# add the first line then you can execute this script without using python xx.py but only xx.py after xx.py was given the excution permession
# add the second line to indicate the coding standard


from build_thirdparty import build_thirdparty_libs
from build_core import build_core
from build_comms import build_comms
from build_simulator import build_simulator
from build_simulator_test import build_simulator_test
from build_util import *

if __name__ == "__main__":
    bi = build_info()

    print "Building thirdparty libs..."
    build_thirdparty_libs(bi)

    print "Building core lib..."
    build_core(bi)

    print "Building comms lib..."
    build_comms(bi)

    print "Building simulator..."
    build_simulator(bi)

    print "Building simulator test ..."
    build_simulator_test(bi)
