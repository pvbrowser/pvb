 @echo off                                                                            
 rem #########################################################################################                                                 
 rem # This is the file start_pvdevelop.bat                                                  #
 rem #                                                                                       #
 rem # If you only want to develop in Lua you do not need to set any environment variables   #
 rem # If you want to develop in C/C++ you must install MinGW and set %MINGWDIR%             #
 rem #   If you use the standard path from the MinGW installer c:\mingw should be ok         #
 rem #   The MinGW installation must include mingw32-make.exe                                #
 rem # Optionally you might install the Qt development package and set %QTDIR%               #
 rem #   If the Qt development package is installed we use qmake.exe to generate Makefile    #
 rem #   Otherwise we fall back to our simple fake_qmake.exe tool                            #
 rem # Otionally you can set %PYDIR% if you want to use Python                               #
 rem #                                                                                       #
 rem #########################################################################################                                                 
 if    [%QTDIR%]    == [] goto environment_not_set                                 
 if    [%MINGWDIR%] == [] goto environment_not_set                                 
 goto  start_pvdevelop                                                               
 :environment_not_set                                                                
 rem   ############################################################################# 
 rem
 rem   Please adjust environment variables within %PVBDIR%\win-mingw\bin\start_pvdevelop.bat                                  
 REM   set   QTDIR=c:\Qt\4.8.2
 REM   set   MINGWDIR=c:\mingw
 set   QTDIR=C:\Qt\Qt5.4.2\5.4\mingw491_32
 set   MINGWDIR=C:\Qt\Qt5.4.2\Tools\mingw491_32
 set   PYDIR=C:\Python25
 rem
 rem   ############################################################################# 
 rem   instead of setting QTDIR and MINGWDIR here you could also set them            
 rem   within the registry using the system control                                  
 :start_pvdevelop                                                                    
 set   PATH=%MINGWDIR%\bin;%QTDIR%\bin;%PVBDIR%\win-mingw\bin;c:\windows;c:\windows\system32;
 set   PYTHONPATH="%PVBDIR%\win-mingw\bin";%PYTHONPATH%
 echo  PVBDIR     = %PVBDIR%
 echo  QTDIR      = %QTDIR%
 echo  MINGWDIR   = %MINGWDIR%
 echo  PYDIR      = %PYDIR%
 echo  PATH       = %PATH%
 echo  PYTHONPATH = %PYTHONPATH%
 echo  starting pvdevelop
 echo  Please verify if the above environment variables are correct on your system.
 echo  If not please adjust by editing file 
 echo    %PVBDIR%\win-mingw\bin\start_pvdevelop.bat
 start pvdevelop                                                                     
 :end                                                                                
 pause                                                                               
