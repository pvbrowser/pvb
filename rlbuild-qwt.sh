#!/bin/bash
#
cd qwt
cd src
../../qmake.sh src.pro
make $1
cd ..
cd designer
../../qmake.sh designer.pro
make $1
#cd ..
#cd textengines
#../../qmake.sh textengines.pro
#make $1
#cd ..
#../qmake.sh qwt.pro 
#make $1
echo "################################################################"
echo "# finished rlbuild-qwt.sh !!!                                  #"
echo "################################################################"
