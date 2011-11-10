 @echo off                                                                            
 rem #########################################################################################                                                 
 rem # This is the file start_pvdevelop.bat                                                  #                                                 
 rem # You must install the Qt4 SDK including MinGW.                                         #
 rem # During installation of Qt you are asked if you want to install MinGW also.            #
 rem # You will have to define the environment variables %QTDIR% and %MINGWDIR% to point     #
 rem # to the installation directories where the executables from Qt and MinGW can be found  #
 rem # (qmake.exe, mingw32-make.exe and the C++ compiler).                                   #
 rem # The qmake tool from Qt will be used to create a Makefile for your projects.           #
 rem # The installation procedure of pvbrowser already defined the environment variable      #
 rem # %PVBDIR% and added %PVBDIR%\win-mingw\bin to your %PATH% environment variable.        #
 rem # Since it did not know where the Qt SDK is located this must be defined manually       #
 rem # either in this file or within your system control center.                             #            
 rem #########################################################################################                                                 
 if    [%QTDIR%]    == [] goto environment_not_set                                 
 if    [%MINGWDIR%] == [] goto environment_not_set                                 
 goto  start_pvdevelop                                                               
 :environment_not_set                                                                
 echo  ############################################################################# 
 echo  Please adjust environment variables within %PVBDIR%\win-mingw\bin\start_pvdevelop.bat                                  
 set   QTDIR=c:\Qt\4.7.4
 set   MINGWDIR=c:\mingw
 rem   if you want to use python
 set   PYDIR=c:\Python25
 start notepad "%PVBDIR%\win-mingw\bin\start_pvdevelop.bat"                        
 pause
 exit                                                                                
 echo  ############################################################################# 
 rem   remove start notepad, pause and exit above                                    
 rem   instead of setting QTDIR and MINGWDIR here you could also set them            
 rem   within the registry using the system control                                  
 :start_pvdevelop                                                                    
 set   PATH=%PVBDIR%\win-mingw\bin;%QTDIR%\bin;%MINGWDIR%\bin;c:\windows;
 set   PYTHONPATH=%PVBDIR%\win-mingw\bin;%PYTHONPATH%
 echo  PVBDIR     = %PVBDIR%                                                       
 echo  QTDIR      = %QTDIR%                                                        
 echo  MINGWDIR   = %MINGWDIR%                                                     
 echo  PYDIR      = %PYDIR%                                                        
 echo  PATH       = %PATH%                                                         
 echo  PYTHONPATH = %PYTHONPATH%                                                   
 echo  starting pvdevelop                                                            
 echo  you can set the above environment variables also within the system control    
 echo  and start pvdevelop.exe directly                                              
 start pvdevelop                                                                     
 :end                                                                                
 pause                                                                               
