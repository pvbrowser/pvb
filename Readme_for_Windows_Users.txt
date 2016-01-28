pvbrowser can be used without any registry settings !
pvdevelop needs some environment variables !

### pvbrowser uses these environment variables ###

%PVBDIR%     # point to pvbrowser installation directory
%QTDIR%      # point to qt installation directory
%MINGWDIR%   # point to MinGW installation directory

(only needed if you want python)
%PYDIR%      # point to python installation directory
%SWIGDIR%    # point to swig installation directory
%PYTHONPATH% # search path for python modules

# for MinGW:
set PATH=%PVBDIR%\win-mingw\bin;%QTDIR%\bin;%MINGWDIR%\bin;%PATH%
SET PYTHONPATH=%PVBDIR%\win-mingw\bin;%PYTHONPATH%
              
# for Visual C++:
set PATH=%PVBDIR%\win\bin;%QTDIR%\bin;%PATH%
SET PYTHONPATH=%PVBDIR%\win\bin;%PYTHONPATH%

### build ###
cd win-mingw
rem adjust environment variables in login.bat
rem SET    QTDIR=C:\Qt\Qt5.5.1\5.5\mingw492_32
rem SET MINGWDIR=C:\Qt\Qt5.5.1\Tools\mingw492_32
edit login.bat
rem run login.bat in order to set environment variables
login.bat
rem run build5.bat
build5.bat
rem run build5.bat again to see that no error occured
build5.bat

### run ###
pvbrowser
pvdevelop

Have a lot of fun:
Yours pvbrowser community
