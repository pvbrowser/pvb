#!/bin/bash
########################################################
# make pvbrowser server components only                #
########################################################
export LIBPTHREAD='-pthread'

cd fake_qmake
echo Compiling fake_qmake ...
g++ -c -m64 -pipe -O2 -Wall -W  -I. -I../rllib/lib -o main.o main.cpp
g++ -c -m64 -pipe -O2 -Wall -W  -I. -I../rllib/lib -o fake_qmake.o fake_qmake.cpp
g++ -c -m64 -pipe -O2 -Wno-implicit-fallthrough  -W  -I. -I../rllib/lib -o rlstring.o ../rllib/lib/rlstring.cpp
g++ -c -m64 -pipe -O2 -Wall -W  -I. -I../rllib/lib -o rlspreadsheet.o ../rllib/lib/rlspreadsheet.cpp
g++ -c -m64 -pipe -O2 -Wall -W  -I. -I../rllib/lib -o rlfileload.o ../rllib/lib/rlfileload.cpp
g++ -c -m64 -pipe -O2 -Wall -W  -I. -I../rllib/lib -o rlcutil.o ../rllib/lib/rlcutil.cpp
g++ -m64 -Wl,-O1 -o fake_qmake main.o fake_qmake.o rlstring.o rlspreadsheet.o rlfileload.o rlcutil.o
cd ..

cd pvserver
#./makelibs.sh
../fake_qmake/fake_qmake -fake pvsid.pro -o pvsid.mak
make -f pvsid.mak clean
make -f pvsid.mak
make -f pvsid.mak staticlib

../fake_qmake/fake_qmake -fake pvsmt.pro -o pvsmt.mak
make -f pvsmt.mak clean
make -f pvsmt.mak
make -f pvsmt.mak staticlib
cd ..

cd rllib/lib
../../fake_qmake/fake_qmake -fake lib.pro
make
cd ../..
cd rllib/rlsvg
../../fake_qmake/fake_qmake -fake rlsvgcat.pro
make
cd ../..
cd rllib/rlfind
../../fake_qmake/fake_qmake -fake rlfind.pro
make
cd ../..
cd rllib/rlhistory
../../fake_qmake/fake_qmake -fake rlhistory.pro
make
cd ../..
#echo the following tool needs qt
#cd rllib/rlhtml2pdf
#../../qmake.sh rlhtml2pdf.pro
#make
#cd ../..
echo '################################################################'
echo '# finished building server components !!!                      #'
echo '# now run:                                                     #'
echo '#   su                                                         #'
echo '#   ./install.sh                                               #'
echo '#   exit                                                       #'
echo '#                                                              #'
echo '# optional: if you want a LUA language binding                 #'
echo '# after the above installation run:                            #'
echo '#   cd language_bindings                                       #'
echo '#   ./build_lua_interface.sh                                   #'
echo '#   cd ..                                                      #'
echo '#   su                                                         #'
echo '#   ./install.sh                                               #'
echo '#   exit                                                       #'
echo '#                                                              #'
echo '################################################################'
