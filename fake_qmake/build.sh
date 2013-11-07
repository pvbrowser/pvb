#!/bin/bash
#
# fake_qmake is a simple replacement for qmake which is part of the Qt SDK
# If you do not want to install the Qt SDK you can develop pvserver(s) using fake_qmake/make/g++ only
# This is especially interesting on embedded devices where there is no GUI at all
# If you want to use "server_components_only" you can delete the following directories from "/opt/pvb"
#   /opt/pvb/doc
#   /opt/pvb/qwt
#   /opt/pvb/pvdevelop
#   /opt/pvb/pvbrowser
#   /opt/pvb/browserplugin
#   /opt/pvb/designer
#
# me@mybox:~> fake_qmake --help
# fake_qmake is a fake of qmake from qt with a very reduced set of features.
# but sufficient to build a pvserver or other simple commandline tools with MinGW.
# if you need all the qmake functions install the qt development package.
# usage:   fake_qmake <-fake> <name.pro> <-o makefile>   <-builddir directory>
# default: fake_qmake          name.pro   -o Makefile
#
echo Compiling fake_qmake ...
g++ -c -m64 -pipe -O2 -Wall -W  -I. -I../rllib/lib -o main.o main.cpp
g++ -c -m64 -pipe -O2 -Wall -W  -I. -I../rllib/lib -o fake_qmake.o fake_qmake.cpp
g++ -c -m64 -pipe -O2 -Wall -W  -I. -I../rllib/lib -o rlstring.o ../rllib/lib/rlstring.cpp
g++ -c -m64 -pipe -O2 -Wall -W  -I. -I../rllib/lib -o rlspreadsheet.o ../rllib/lib/rlspreadsheet.cpp
g++ -c -m64 -pipe -O2 -Wall -W  -I. -I../rllib/lib -o rlfileload.o ../rllib/lib/rlfileload.cpp
g++ -c -m64 -pipe -O2 -Wall -W  -I. -I../rllib/lib -o rlcutil.o ../rllib/lib/rlcutil.cpp
g++ -m64 -Wl,-O1 -o fake_qmake main.o fake_qmake.o rlstring.o rlspreadsheet.o rlfileload.o rlcutil.o      
echo "###################################################################"
echo "# We have build fake_qmake without having qmake installed         #"
echo "# Now use the following commands to install fake_qmake            #"
echo "###################################################################"
echo   su
echo   mkdir -p /opt/pvb/fake_qmake
echo   cp fake_qmake /opt/pvb/fake_qmake/
echo   ln -s /opt/pvb/fake_qmake/fake_qmake /usr/bin/fake_qmake
echo   exit
echo "### Then you may compile and install the server components only ###"
echo   cd ..
echo   ./build_server_components_only.sh
echo   "./build_server_components_only.sh     # check if no errors occured"
echo   su
echo   ./install.sh
echo   exit
