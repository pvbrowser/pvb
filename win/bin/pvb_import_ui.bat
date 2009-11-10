@echo off
rem echo %1 %2
call pvenv.bat
echo pvdevelop -action=importUi:%2
pvdevelop -action=importUi:%2
call pvmake.bat %1
pause
