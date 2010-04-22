g++ modbusdaemon.cpp "%PVBDIR%\win-mingw\bin\librllib.a" "%MINGWDIR%\lib\libws2_32.a" "-I%PVBDIR%\rllib\lib" "%MINGWDIR%\lib\libadvapi32.a"  -static-libgcc -o modbusdaemon.exe   
pause                                                                                                                                                                  
exit                                                                                                                                                                   
