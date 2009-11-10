@echo off
rem system("pvb_server.bat " + name);
CALL %PVBDIR%\win\bin\pvenv.bat
fake_qmake %1.pro -o Makefile.win
nmake /f Makefile.win
%1
