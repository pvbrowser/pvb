This is pvbrowser:
#######################################################
Make sure you have installed:
  Linux:
    g++
    make
    qt-4.x
  Windows:
    Qt SDK from qtsoftware
    MinGW 
    Make sure you have not blocked port 5050 with your firewall
  OS-X:
    xcode
    X11User.pkg
    qt-4.x
  OpenVMS:
    CXX
#######################################################
The package includes the 
compiled programs + libs + sourecode

Install with:

Linux:
  tar -zxf pvb.tar.gz
  cd pvb
  ./clean.sh
  ./build.sh
  su
  ./install.sh
  exit

Windows:
  Use executables within
  pvb\win-mingw\bin
  or install from install-pvbrowser.exe available on our homepage

OS-X:
  read Readme_for_Apple_Users.txt

Have a lot of fun !
Yours pvbrowser community

### Linux #############################################
build:
  ./build.sh

clean:
  ./clean.sh

install:
  su
  ./install.sh
  exit

uninstall:
  su
  cd /opt/pvb
  ./uninstall.sh
  exit 

run:
  pvbrowser
  pvdevelop

start example pvserver:
  cd /opt/pvb/pvserver
  ./processviewserver -sleep=100
  OR
  cd /opt/pvb/pvsexample
  ./pvsexample -sleep=100

Build with VTK:
  download VTK5.x from http://www.kitware.com/VTK
  download cmake (See: readme.html within VTK package)
  configure VTK with:
    build shared libraries: ON
    wrap Tcl: ON
  install VTK
  cd pvb/pvbrowser
  edit pvbrowser.pro and uncomment USE_VTK
  make clean
  make
  su
  cp pvbrowser /usr/local/bin/pvbrowservtk
  exit

### Windows ##########################################
build:
  cd win-mingw
  # edit login.bat and define environment variables QTDIR and MINGWDIR 
  # to point to the executables of qmake.exe, mingw32-make.exe and the C++ compiler
  login.bat
  build.bat

run:
  pvbrowser
  pvbrowservtk
  pvdevelop

### OpenVMS ##########################################
build:
  @vms_build.com
  In SYLOGIN.COM or LOGIN.COM define symbol
  rlsvgcat := disk:[path]rlsvgcat.exe

### Building and installing server components only ###

This is usefull for linux servers that do not have
an up to date Qt library and you want to use it as
server only. In this case only the libraries and
the command line utilities are build.

./clean.sh
./build_server_components_only.sh
su
./install.sh
exit

######################################################

