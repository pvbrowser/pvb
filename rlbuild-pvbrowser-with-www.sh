#!/bin/bash
source rlsetup.sh
cd pvbrowser
../qmake.sh pvbrowser-with-www.pro
make $1
echo "################################################################"
echo "# finished compiling rlbuild-pvbrowser-with-www.sh !!!         #"
echo "################################################################"
