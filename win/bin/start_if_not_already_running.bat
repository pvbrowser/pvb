@echo off
rem #####################################################
rem # start a program only if it is not alreday running #
rem #####################################################

tasklist /fi "Imagename eq %1" > pvbrowser.tasklist
find "%1" pvbrowser.tasklist   > NUL
if %errorlevel% EQU 0 goto skip
  echo %1 Starting ... 
  start %1 %2 %3 %4 %5 %6 %7 %8 %9
  goto end
:skip
  echo %1 already running
:end
  del /f pvbrowser.tasklist
