@echo off

CALL %PVBDIR%\win\bin\pvenv.bat

IF [%1] == [] GOTO MAKE_ONLY

echo generating Makefile.win ...
fake_qmake %1.pro -o Makefile.win

:MAKE_ONLY
nmake /f Makefile.win
