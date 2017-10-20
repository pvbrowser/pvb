@echo off
cd ..\qwt                                                                   
cd src                                                                     
%QTDIR%\bin\qmake src.pro -o Makefile                                    
mingw32-make.exe                                                                
cd ..                                                                      
cd designer                                                                
%QTDIR%\bin\qmake designer.pro -o Makefile                               
mingw32-make.exe                                                                
cd ..                                                                      
cd textengines                                                             
%QTDIR%\bin\qmake textengines.pro -o Makefile                            
mingw32-make.exe                                                                
cd ..                                                                      
%QTDIR%\bin\qmake qwt.pro -o Makefile                                    
mingw32-make.exe                                                                
cd ..\win-mingw                                                        
rem pause                                                                  
                                                                           
cd ..\designer                                                             
cd src                                                                     
%QTDIR%\bin\qmake  pvbdummy.pro -o Makefile                              
mingw32-make.exe                                                                
%QTDIR%\bin\qmake  pvbplugin.pro -o Makefile                             
mingw32-make.exe                                                                
cd ..                                                                      
cd ..                                                                      
cd win-mingw                                                               
rem pause                                                                  

cd pvbrowser                                                               
%QTDIR%\bin\qmake pvbrowser.pro -o Makefile                              
mingw32-make.exe                                                                
cd ..                                                                      
rem pause                                                                  
                                                                           
echo Netscape plugins are not supported in Qt5
rem cd browserplugin                                                           
rem %QTDIR%\bin\qmake pvpluginmain.pro -o Makefile                           
rem mingw32-make.exe                                                                
rem cd ..                                                                      
rem pause                                                                  
                                                                           
cd pvdevelop                                                               
%QTDIR%\bin\qmake pvdevelop.pro -o Makefile                              
mingw32-make.exe                                                                
cd ..                                                                      
rem pause                                                                  
                                                                           
cd pvserver                                                                
%QTDIR%\bin\qmake pvsmt.pro -o Makefile                                  
mingw32-make.exe                                                                
cd ..                                                                      
rem pause                                                                  
                                                                           
cd rllib                                                                   
%QTDIR%\bin\qmake lib.pro -o Makefile                                    
mingw32-make.exe                                                                
cd ..                                                                      
copy rllib\release\librllib.a bin\librllib.a                                       
rem pause                                                                  
                                                                           
cd rlsvg                                                                   
%QTDIR%\bin\qmake rlsvgcat.pro -o Makefile                               
mingw32-make.exe                                                                
cd ..                                                                      
rem pause                                                                  
                                                                           
cd rlfind                                                                   
%QTDIR%\bin\qmake rlfind.pro -o Makefile                               
mingw32-make.exe                                                                
cd ..                                                                      
rem pause                                                                  
                                                                           
cd rlhistory                                                               
%QTDIR%\bin\qmake rlhistory.pro -o Makefile                              
mingw32-make.exe                                                                
cd ..                                                                      
rem pause                                                                  
                                                                           
cd rlhtml2pdf                                                               
%QTDIR%\bin\qmake rlhtml2pdf.pro -o Makefile                              
mingw32-make.exe                                                                
cd ..                                                                      
rem pause                                                                  
                                                                           
cd fake_qmake                                                               
%QTDIR%\bin\qmake fake_qmake.pro -o Makefile                              
mingw32-make.exe                                                                
cd ..                                                                      
rem pause                                                                  
                                                                           
cd ..\start_pvbapp                                                         
%QTDIR%\bin\qmake start_pvbapp.pro -o Makefile                           
mingw32-make.exe                                                                
cd ..\win-mingw                                                            
rem pause                                                                  
                                                                           
cd pvmore                                                              
%QTDIR%\bin\qmake pvmore.pro -o Makefile                           
mingw32-make.exe                                                                
cd ..                                                                      

%mingwdir%\bin\gcc.exe nodave.c -o nodave_mingw.o -c -D_WIN32 -DBCCWIN -I%mingwdir%\include
                                                                           
copy pvbrowser\release\pvbrowser.exe                             bin\pvbrowser.exe                               
rem copy browserplugin\release\nppvbrowser.dll                       bin\nppvbrowser.dll                             
copy pvdevelop\release\pvdevelop.exe                             bin\pvdevelop.exe                               
copy pvserver\release\libserverlib.a                             bin\libserverlib.a                              
copy rlsvg\release\rlsvgcat.exe                                  bin\rlsvgcat.exe                                
copy rlfind\release\rlfind.exe                                   bin\rlfind.exe                                
copy rlhistory\release\rlhistory.exe                             bin\rlhistory.exe                               
copy rlhtml2pdf\release\rlhtml2pdf.exe                           bin\rlhtml2pdf.exe                               
copy ..\start_pvbapp\release\\start_pvbapp.exe                   bin\start_pvbapp.exe                            
copy fake_qmake\release\fake_qmake.exe                           bin\fake_qmake.exe                              
xcopy /e /y "%QTDIR%\plugins\imageformats"                       bin\imageformats                                
copy ..\qwt\designer\plugins\designer\qwt_designer_plugin5.dll   bin\plugins\designer\qwt_designer_plugin5.dll   
copy ..\qwt\designer\plugins\designer\libqwt_designer_plugin5.a  bin\plugins\designer\libqwt_designer_plugin5.a  
copy ..\designer\plugins\pvb_designer_plugin.dll                 bin\plugins\designer\pvb_designer_plugin.dll    
copy ..\designer\plugins\libpvb_designer_plugin.a                bin\plugins\designer\libpvb_designer_plugin.a   
copy bin\plugins\designer\*designer_plugin*                      %QTDIR%\plugins\designer\                     
rem copy %MINGWDIR%\bin\mingwm10.dll                                 bin\
rem copy %MINGWDIR%\bin\libgcc_s_dw2-1.dll                           bin\
rem copy %QTDIR%\bin\phonon4.dll                                     bin\
rem copy %QTDIR%\bin\QtCLucene4.dll                                  bin\
rem copy %QTDIR%\bin\QtCore4.dll                                     bin\
rem copy %QTDIR%\bin\QtDeclarative4.dll                              bin\
rem copy %QTDIR%\bin\QtGui4.dll                                      bin\
rem copy %QTDIR%\bin\QtHelp4.dll                                     bin\
rem copy %QTDIR%\bin\QtMultimedia4.dll                               bin\
rem copy %QTDIR%\bin\QtNetwork4.dll                                  bin\
rem copy %QTDIR%\bin\QtOpenGL4.dll                                   bin\
rem copy %QTDIR%\bin\QtScript4.dll                                   bin\
rem copy %QTDIR%\bin\QtScriptTools4.dll                              bin\
rem copy %QTDIR%\bin\QtSql4.dll                                      bin\
rem copy %QTDIR%\bin\QtSvg4.dll                                      bin\
rem copy %QTDIR%\bin\QtTest4.dll                                     bin\
rem copy %QTDIR%\bin\QtWebKit4.dll                                   bin\
rem copy %QTDIR%\bin\QtXml4.dll                                      bin\
rem copy %QTDIR%\bin\QtXmlPatterns4.dll                              bin\
windeployqt bin/pvbrowser.exe

echo ##############################################################################
echo # run me again in order to verify that no errors occured                     #
echo # If you_want a Lua and Python binding then run: build_language_bindings.bat #
echo # Bulding the language binding will produce a lot of WARNINGS !!!            #    
echo ##############################################################################

