#########################################################################
# Installing and starting the pvbrowser development environment on OS-X #
#########################################################################

Before you can use the pvbrowser development environment on OS-X:
install http://developer.apple.com/tools/xcode/                 +
install the "X11User.pkg" package                               +
install the Qt libraries for Mac http://qt.nokia.com/downloads/downloads#qt-lib

Download:
http://pvbrowser.org/pvbrowser/tar/pvb.tar.gz

Open Utilities/Terminal.app and issue the following command:
tar -zxf pvb.tar.gz
cd pvb
./build.sh
  # you have to adjust the following section within build.sh first using an ascii editor
  # export QMAKESPEC=/usr/local/Qt4.5/mkspecs/macx-g++  // remove the comment before this line
  #
  echo "######################################"
  echo "# then remove the exit command below #"
  echo "######################################"
  exit                                                  // comment out the exit command
sudo ./install.sh

For developing your own pvserver:
start X11 and open a xterm terminal
mkdir your_dir
cd your_dir
pvdevelop

After installation you may copy
pvb/pvbrowser/pvbrowser.app
to your desktop in order to start it from there.

Have a lot of fun:
Yours pvbrowser community
