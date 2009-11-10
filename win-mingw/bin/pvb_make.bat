@echo off                                                                   
rem system("pvb_make.bat + name");                                         
CALL "%PVBDIR%\win-mingw\bin\pvenv.bat"                              
qmake %1.pro -o Makefile.win                                              
mingw32-make.exe -f Makefile.win                                                
pause                                                                      
exit                                                                       
