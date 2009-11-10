VTK is not compiled into pvbrowser by default.
In order to use it you have to recompile.

Edit pvbrowser.pro and uncomment
#CONFIG       += USE_VTK
Also verify that the path to the VTK library is set correctly for your system
INCLUDEPATH  += /usr/local/include/vtk-5.0
LIBS += /usr/local/lib/libvtkCommon.so
LIBS += /usr/local/lib/libvtkCommonTCL.so
LIBS += /usr/local/lib/libvtkDICOMParser.so
...

Install VTK as described there.
Install the Tcl/Tk development package

Now recompile.
make clean
make

Test it on our example server.
./pvbrowser pv://pvbrowser.org
Select VTK there

For Windows you have to install:
- Qt open source edition
- MinGW
- VTK
- Tcl/Tk
run
pvb/win-mingw/CreatePvbWithMinGW.bat
Edit pvb/win-mingw/pvbrowser/pvbrowser.pro as described above
Recompile pvbrowser.
cd pvb/win-wingw/pvbrowser
%MINGWDIR%\bin\mingw32-make.exe clean
%MINGWDIR%\bin\mingw32-make.exe
