@echo off                                                                   
rem system("pvb_make.bat + name");                                         
CALL "%PVBDIR%\win-mingw\bin\pvenv.bat"                              
fake_qmake %1.pro -o Makefile.win
if exist "%MINGWDIR%\BIN\MINGW32-MAKE.EXE" (
  mingw32-make.exe -f Makefile.win
) else (
  echo %MINGWDIR%\BIN\MINGW32-MAKE.EXE not found !!! 
  echo Please install MinGW and adjust %PVBDIR%\win-mingw\bin\start_pvdevelop.bat
)
pause                                                                      
exit                                                                       
