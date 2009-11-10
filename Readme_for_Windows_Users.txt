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
CreatePvbWithMinGW.bat

### run ###
pvbrowser
pvdevelop

Have a lot of fun:
Yours pvbrowser community
