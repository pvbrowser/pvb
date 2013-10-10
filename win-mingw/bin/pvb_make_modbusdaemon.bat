g++ modbusdaemon.cpp "%PVBDIR%\win-mingw\bin\librllib.a" -lws2_32 "-I%PVBDIR%\rllib\lib" -ladvapi32  -static-libgcc -o modbusdaemon.exe   
pause                                                                                                                                                                  
exit                                                                                                                                                                   
