#!/bin/bash
# setup build environment
echo "which qmake"
which qmake
echo "which qmake-qt5"
which qmake-qt5

# detect real OS on different linux distries
export PVB_OSTYPE="unknown"
if [ "$OSTYPE" == "linux" ]; then
  export PVB_OSTYPE="linux"
fi
if [ "$OSTYPE" == "gnu-linux" ]; then
  export PVB_OSTYPE="linux"
fi
if [ "$OSTYPE" == "linux-gnu" ]; then
  export PVB_OSTYPE="linux"
fi
if [ "$2" == "buildservice" ]; then
  export PVB_OSTYPE="linux"
fi

echo OSTYPE = $PVB_OSTYPE
if [ "$PVB_OSTYPE" != "linux"  ]; then
if [ "$2"    != "buildservice" ]; then
if [ "$HOME" != "/root"        ]; then
if [ "$HOME" != "/home/lehrig" ]; then
  echo "############################################################################"
  echo "We are not at home. Please edit this file.                                  "
  echo "You have to define QTDIR, QMAKESPEC and perhaps TCLLIBPATH (if you want VTK)"
  # echo "If you want VTK you have to uncomment USE_VTK in pvb/pvbrowser/pvbrowser.pro"
  # echo "If you want VTK you need Tcl/Tk                                             "
  # echo "If you want VTK install from http://www.kitware.com/vtk and configure with: "
  # echo "  build shared libraries: ON                                                "
  # echo "  wrap Tcl: ON                                                              "
  # echo "You need to install the openGL development package (mesa) !!!               "
  echo "please adjust the following:                                                "
  echo "Or leave it as it is, when your distribution has Qt4 preinstalled           "
  echo "############################################################################"
  #
  # OS-X
  # this export we use under OS-X with xcode_2.4.1_8m1910_6936315.dmg and Qt
  # export QMAKESPEC=/usr/local/Qt4.6/mkspecs/macx-g++
  #
  echo "######################################"
  echo "# then remove the exit command below #"
  echo "######################################"
if [ "$2" != "homebrew" ]; then
  exit
fi
fi
fi
fi
if [ "$HOME" == "/home/lehrig" ]; then
  echo "We are at home"
  export QTDIR=/usr/lib64/qt5
  export QMAKESPEC=/usr/lib64/qt5/mkspecs/linux-g++
  #export QTDIR=/usr/share/qt4
  #export QMAKESPEC=/usr/share/qt4/mkspecs/linux-g++
  #export QTDIR=/usr/local/Trolltech/Qt-4.2.1
  #export QMAKESPEC=/usr/local/Trolltech/Qt-4.2.1/mkspecs/linux-g++
  #export PATH=/usr/local/Trolltech/Qt-4.2.1/bin:$PATH
  export TCLLIBPATH=/home/lehrig/temp/VTK5/VTK/Wrapping/Tcl
fi
fi

export LIBPTHREAD='-pthread'

