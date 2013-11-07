#!/bin/bash
########################################################
# make pvbrowser server components only                #
########################################################
export LIBPTHREAD='-pthread'
cd pvserver
rm util.o glencode.o
../qmake.sh pvsid.pro -o pvsid.mak
make -f pvsid.mak
rm util.o glencode.o
../qmake.sh pvsmt.pro -o pvsmt.mak
make -f pvsmt.mak
cd ..
cd rllib/lib
../../qmake.sh lib.pro
make
cd ../..
cd rllib/rlsvg
../../qmake.sh rlsvgcat.pro
make
cd ../..
cd rllib/rlhistory
../../qmake.sh rlhistory.pro
make
cd ../..
cd fake_qmake
../qmake.sh fake_qmake.pro
make
cd ..
echo '################################################################'
echo '# finished building server components !!!                      #'
echo '# now run:                                                     #'
echo '#   su                                                         #'
echo '#   ./install.sh                                               #'
echo '#   exit                                                       #'
echo '################################################################'
