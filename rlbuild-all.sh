#!/bin/bash
# please adjust this shell script to your needs
echo "################################################################"
echo "# start compiling !!!                                          #"
echo "# if you have problems with qmake you may adjust file qmake.sh #"
echo "################################################################"
echo "on the OpenSUSE buildservice you may ignore missing csh error because"
echo "there we use the standard qmake and not our own fake_qmake"
./rlcompile-fake-qmake.sh
echo "################################################################"
echo "build qwt"
./rlbuild-qwt.sh
echo "build plugins"
./rlbuild-plugins.sh
echo "build pvdevelop"
./rlbuild-pvdevelop.sh
echo "build pvbrowser"
#./rlbuild-pvbrowser-basic.sh
./rlbuild-pvbrowser-with-www.sh
echo "build pvserver libraries for inetd and for multithreaded pvserver + build a ver old pvserver"
./rlbuild-pvserver.sh
echo "build rllib"
./rlbuild-rllib.sh
echo "build pvserver language binding for lua"
./rlbuild-language-lua.sh
echo "################################################################"
echo "# finished compiling !!!                                       #"
echo "#              _                                               #"
echo "#   _ ____   _| |__  _ __ _____      _____  ___ _ __           #"
echo "#  | '_ \ \ / / '_ \| '__/ _ \ \ /\ / / __|/ _ \ '__|          #"
echo "#  | |_) \ V /| |_) | | | (_) \ V  V /\__ \  __/ |             #"
echo "#  | .__/ \_/ |_.__/|_|  \___/ \_/\_/ |___/\___|_|             #"
echo "#  |_|                                                         #"
echo "# verify that no errors occured by running me again            #"
echo "# now run:                                                     #"
echo "#   su                                                         #"
echo "#   ./install.sh                                               #"
echo "#   exit                                                       #"
echo "# if you want to use Qt Designer for designing your masks,     #"
echo "#   copy the plugins to Qt Designer (read designer/README.txt) #"
echo "# Have a lot of fun (Yours pvbrowser community)                #"
echo "################################################################"

