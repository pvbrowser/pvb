#!/bin/bash

../qmake.sh pvsid.pro -o pvsid.mak
make -f pvsid.mak clean
make -f pvsid.mak
make -f pvsid.mak staticlib

../qmake.sh pvsmt.pro -o pvsmt.mak
make -f pvsmt.mak clean
make -f pvsmt.mak
make -f pvsmt.mak staticlib


