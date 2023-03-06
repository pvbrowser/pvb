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
./rlcompile-fake-qmake.sh
echo "###################################################################"
echo "# We have build fake_qmake without having qmake installed         #"
echo "# Now use the following commands to install fake_qmake            #"
echo "###################################################################"
echo "### now install fake_qmake with:"
echo   su
echo   mkdir -p /opt/pvb/fake_qmake
echo   cp fake_qmake/fake_qmake /opt/pvb/fake_qmake/
echo   ln -s /opt/pvb/fake_qmake/fake_qmake /usr/bin/fake_qmake
echo   exit
echo "### Then you may compile and install the server components only ###"
echo   ./rlbuild-pvserver.sh
echo   ./rlbuild-rllib.sh
echo   ./rlbuild-language-lua.sh
echo   "### You may repeat the above commands in order to check if no errors occured"
echo "### finally everything can be installed with:"
echo   su
echo   ./install.sh
echo   exit
