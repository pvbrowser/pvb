@echo off                                          
                                                  
CALL "Z:\cc\priv\cvs\pvb\win-mingw\bin\pvenv.bat"       
                                                  
IF [%1] == [] GOTO MAKE_ONLY                     
                                                  
echo generating Makefile.win ...                  
fake_qmake %1.pro -o Makefile.win                     
                                                  
:MAKE_ONLY                                        
mingw32-make.exe -f Makefile.win                       
pause                                             
exit                                              
