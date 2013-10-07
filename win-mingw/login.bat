@echo off
rem -------------------------------------------------------
rem set environment variables for running build batch files
rem adjust this
rem -------------------------------------------------------

rem set directories for needed tools
rem directories where qt and mingw are installed

rem ### for qt4.8.2 ###
rem SET QTDIR=C:\Qt\4.8.2
rem SET QTBIN=C:\Qt\4.8.2\bin
rem SET MINGWDIR=C:\mingw
rem SET MINGWBIN=C:\mingw\bin
rem ### for qt5 ###
SET    QTDIR=C:\Qt\5.1.0
SET  QTMINGW=C:\Qt\5.1.0\5.1.0\mingw48_32
SET    QTBIN=C:\Qt\5.1.0\5.1.0\mingw48_32\bin
SET MINGWDIR=C:\Qt\5.1.0\Tools\mingw48_32
SET MINGWBIN=C:\Qt\5.1.0\Tools\mingw48_32\bin

rem tools necessary for language bindings
SET SWIGDIR=Z:\win\swigwin-1.3.33
SET PYDIR=C:\Python25
SET VTKDIR=z:\Downloads\vtk\VTK5.10.1
SET TCLDIR=c:\Tcl
SET TCLLIBPATH=c:\Tcl\Tcl8.5;z:/Downloads/vtk/bin/Wrapping/Tcl
rem change to directory with our sources
SET PVBDIR=Z:\cc\priv\cvs\pvb
z:
cd  %PVBDIR%\win-mingw

rem setup path variables
rem SET PATH=%PVBDIR%\win-mingw\bin;%MINGWDIR%\bin;%QTDIR%\bin;%PATH%
SET PATH=%PVBDIR%\win-mingw\bin;%MINGWBIN%;%QTBIN%;c:\windows;c:\windows\system32
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
echo build.bat  // if you want to use Qt4
echo build5.bat // if you want to use Qt5

