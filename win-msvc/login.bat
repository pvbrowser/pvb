@echo off
rem -------------------------------------------------------
rem set environment variables for running build batch files
rem adjust this
rem -------------------------------------------------------

rem set directories for needed tools
rem directories where qt and mingw are installed

SET QTDIR=C:\Qt\Qt5.9.1\5.9.1\msvc2015_64
SET QMAKESPEC=%QTDIR%\mkspecs\win32-msvc

rem tools necessary for language bindings
SET SWIGDIR=c:\rl\tools\swigwin-1.3.33
SET PVBDIR=c:\rl\cc\pvb
c:
cd  %PVBDIR%\win-mingw

rem setup path variables
SET PATH=%PVBDIR%\win-msvc\bin;%QTDIR%\bin;c:\windows;c:\windows\system32

echo The following has been set:
echo Is this correct on your machine ?
echo If not then edit this file.
echo QTDIR=%QTDIR%
echo SWIGDIR=%SWIGDIR%
echo PATH=%PATH%
echo now run: 
echo build.bat  // if you want to use MinGW for complete 32 bit development system
echo and 
echo for 64 Bit pvbrowser client with MS Visual C++ see pvb/win-msvc/build.bat

