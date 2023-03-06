#!/bin/bash
cd rllib/lib
../../qmake.sh lib.pro
make
make staticlib
cd ../..
cd rllib/rlsvg
../../qmake.sh rlsvgcat.pro
make
cd ../..
cd rllib/rlfind
../../qmake.sh rlfind.pro
make
cd ../..
cd rllib/rlhistory
../../qmake.sh rlhistory.pro
make
cd ../..
cd rllib/rlhtml2pdf
../../qmake.sh rlhtml2pdf.pro
make
cd ../..
cd start_pvbapp
../qmake.sh start_pvbapp.pro
make
cd ..
echo "################################################################"
echo "# finished compiling rlbuild-rllib.sh !!!                      #"
echo "# + build staticlib                                            #"
echo "# + build rlsvg                                                #"
echo "# + build rlfind                                               #"
echo "# + build rlhistory                                            #"
echo "# + build rlhtml2pdf                                           #"
echo "# + build start_pvbapp                                         #"
echo "################################################################"
