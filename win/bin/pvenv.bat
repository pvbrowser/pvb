echo Please adjust these paths for Visual Studio 6.0 or Express Edition
echo Edit %PVBDIR%\WIN\BIN\PVENV.BAT
echo Yours: pvbrowser community


REM currently Microsoft Express Edition C++ 2008 is selected
call "C:\Programme\Microsoft Visual Studio 9.0\Common7\Tools\vsvars32.bat"
GOTO END


IF "%MSVCDIR%" == "" GOTO EXPRESS
GOTO END

:EXPRESS
IF "%VSINSTALLDIR%" == "" GOTO DEFINE
GOTO END

:DEFINE
REM Adjust these directories
SET VSINSTALLDIR=C:\Programme\Microsoft Visual Studio 8
SET PLATINSTALLDIR=C:\Programme\Microsoft Platform SDK

SET Path=%PATH%;%VSINSTALLDIR%\Common7\IDE;%VSINSTALLDIR%\VC\BIN;%VSINSTALLDIR%\Common7\Tools;%VSINSTALLDIR%\SDK\v2.0\bin;%VSINSTALLDIR%\VC\VCPackages;
SET INCLUDE=%VSINSTALLDIR%\VC\INCLUDE;%PLATINSTALLDIR%\Include;
SET LIB=%VSINSTALLDIR%\VC\LIB;%VSINSTALLDIR%\SDK\v2.0\lib;%PLATINSTALLDIR%\Lib;
SET LIBPATH=%PLATINSTALLDIR%\Lib
SET VCINSTALLDIR=%VSINSTALLDIR%\VC
SET VS80COMNTOOLS=%VSINSTALLDIR%\Common7\Tools\

:END
