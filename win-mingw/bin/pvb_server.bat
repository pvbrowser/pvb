@echo off                                                                   
rem system("pvb_server.bat " + name);                                      
CALL "%PVBDIR%\win-mingw\bin\pvenv.bat"                              
qmake %1.pro -o Makefile.win                                              
mingw32-make.exe -f Makefile.win                                                
%1                                                                        
pause                                                                      
exit                                                                       
