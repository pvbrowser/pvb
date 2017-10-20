@echo off
rem -------------------------------------------------------
rem set environment variables for running build batch files
rem adjust this
rem -------------------------------------------------------

rem set directories for needed tools
rem directories where qt and mingw are installed

rem ### for qt4.8.2 ###
rem SET    QTDIR=C:\Qt\4.8.2
rem MINGWDIR=C:\mingw-old
rem ### for qt5.1.0 ###
SET    QTDIR=C:\Qt\Qt5.4.2\5.4\mingw491_32
SET MINGWDIR=C:\Qt\Qt5.4.2\Tools\mingw491_32

rem tools necessary for language bindings
SET SWIGDIR=c:\rl\tools\swigwin-1.3.33
SET PYDIR=c:\Python25
SET VTKDIR=z:\Downloads\vtk\VTK5.10.1
SET TCLDIR=c:\Tcl
SET TCLLIBPATH=c:\Tcl\Tcl8.5;z:/Downloads/vtk/bin/Wrapping/Tcl
rem change to directory with our sources
SET PVBDIR=c:\rl\cc\pvb
c:
cd  %PVBDIR%\win-mingw

rem setup path variables
SET PATH=%PVBDIR%\win-mingw\bin;%MINGWDIR%\bin;%QTDIR%\bin;c:\windows;c:\windows\system32
SET PYTHONPATH=%PVBDIR%\win-mingw\bin;%PYTHONPATH%

echo The following has been set:
echo Is this correct on your machine ?
echo If not then edit this file.
echo QTDIR=%QTDIR%
echo MINGWDIR=%MINGWDIR%
echo SWIGDIR=%SWIGDIR%
echo PYDIR=%PYDIR%
echo PATH=%PATH%
echo now run: 
echo build.bat  // if you want to use MinGW for complete 32 bit development system
echo and 
echo for 64 Bit pvbrowser client with MS Visual C++ see pvb/win-msvc/build.bat

