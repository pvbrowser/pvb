 @echo off                                                                            
 rem ###############################                                                 
 rem # start pvdevelop             #                                                 
 rem # no registry settings needed #                                                 
 rem ###############################                                                 
 if    [%QTDIR%]    == [] goto environment_not_set                                 
 if    [%MINGWDIR%] == [] goto environment_not_set                                 
 goto  start_pvdevelop                                                               
 :environment_not_set                                                                
 echo  ############################################################################# 
 echo  Please adjust environment variables within this file:                         
 echo  %PVBDIR%\win-mingw\bin\start_pvdevelop.bat                                  
 set   QTDIR=z:\win\qt
 set   MINGWDIR=z:\win\mingw
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
