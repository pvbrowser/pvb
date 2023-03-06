#!/bin/bash
source rlsetup.sh
cd pvbrowser
../qmake.sh pvbrowser-with-basic-footprint.pro
make $1
echo "################################################################"
echo "# finished compiling rlbuild-pvbrowser-basic.sh !!!            #"
echo "################################################################"
