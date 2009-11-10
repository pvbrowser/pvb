CALL %PVBDIR%\win\bin\pvenv.bat
cl /c ppidaemon.cpp  /I%PVBDIR%\rllib\lib
link /NODEFAULTLIB:LIBC ppidaemon.obj %PVBDIR%\win\bin\rllib.lib winspool.lib wsock32.lib advapi32.lib  %PVBDIR%\win\bin\nodave.obj
pause
