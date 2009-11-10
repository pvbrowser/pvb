rem ###############################
rem # start pvdevelop             #
rem # no registry settings needed #
rem ###############################
@echo off
set PVBDIR=%CD%
set PATH=%CD%\win\bin;%PATH%;
cd "%CD%\win\bin"
pvdevelop
