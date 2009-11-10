#!/bin/bash
qmake pvsmt.pro -o pvsmt.mak
qmake pvsid.pro -o pvsid.mak
gmake -f pvsmt.mak clean
gmake -f pvsmt.mak
gmake -f pvsid.mak clean
gmake -f pvsid.mak
rm *.o
