#!/bin/bash
../../qmake.sh pvbdummy.pro  -o Makefile.pvbdummy
make -f Makefile.pvbdummy $1
../../qmake.sh pvbplugin.pro -o Makefile.pvbplugin
make -f Makefile.pvbplugin $1

