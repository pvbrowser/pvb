#!/bin/bash
source rlsetup.sh
cd pvdevelop
#../qmake.sh pvdevelop-with-qwt-before-qwt62.pro
../qmake.sh pvdevelop-with-qwt62.pro
make $1
echo "################################################################"
echo "# finished compiling rlbuild-pvdevelop.sh !!!                  #"
echo "################################################################"
