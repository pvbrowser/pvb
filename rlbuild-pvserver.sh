#!/bin/bash
source rlsetup.sh
cd pvserver
#rm util.o glencode.o
#../qmake.sh pvsid.pro -o pvsid.mak
#make -f pvsid.mak
#rm util.o glencode.o
#../qmake.sh pvsmt.pro -o pvsmt.mak
#make -f pvsmt.mak
./makelibs.sh

if [ "$PVB_OSTYPE" == "linux" ]; then
  # rm util.o glencode.o
  ../qmake.sh processviewserver.pro
  echo "################################################################"
  echo "# also build processviewserver (a very old example)            #"
  echo "################################################################"
  make
fi
echo "################################################################"
echo "# finished compiling rlbuild-pvserver.sh !!!                   #"
echo "################################################################"
