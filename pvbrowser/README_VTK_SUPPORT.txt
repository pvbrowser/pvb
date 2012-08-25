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

###############################################
Here is how VTK must be configured with vtk-5.10:

 BUILD_EXAMPLES                   OFF                                                                                                            
 BUILD_SHARED_LIBS                ON                                                                                                             
 BUILD_TESTING                    OFF                                                                                                            
 CMAKE_BACKWARDS_COMPATIBILITY    2.4                                                                                                            
 CMAKE_BUILD_TYPE                 Debug                                                                                                          
 CMAKE_INSTALL_PREFIX             /usr/local                                                                                                     
 OpenQube_DIR                     OpenQube_DIR-NOTFOUND                                                                                          
 QT_QMAKE_EXECUTABLE              /usr/bin/qmake                                                                                                 
 VTK_DATA_ROOT                    /home/lehrig/Downloads/VTKData (not necessary)                                                                                 
 VTK_EXTRA_COMPILER_WARNINGS      OFF                                                                                                            
 VTK_LARGE_DATA_ROOT              VTK_LARGE_DATA_ROOT-NOTFOUND                                                                                   
 VTK_USE_CHEMISTRY                OFF                                                                                                            
 VTK_USE_INFOVIS                  OFF                                                                                                            
 VTK_USE_N_WAY_ARRAYS             ON                                                                                                             
 VTK_USE_PARALLEL                 OFF                                                                                                            
 VTK_USE_QT                       ON                                                                                                             
 VTK_USE_QTCHARTS                 OFF                                                                                                            
 VTK_USE_RENDERING                ON                                                                                                             
 VTK_USE_TEXT_ANALYSIS            OFF                                                                                                            
 VTK_WRAP_JAVA                    OFF                                                                                                            
 VTK_WRAP_PYTHON                  OFF                                                                                                            
 VTK_WRAP_PYTHON_SIP              OFF                                                                                                            
 VTK_WRAP_TCL                     ON                                                                                                             
