@echo off
cd ..\qwt                                                                   
cd src                                                                     
qmake src.pro -o Makefile                                    
mingw32-make.exe                                                                
cd ..                                                                      
cd designer                                                                
qmake designer.pro -o Makefile                               
mingw32-make.exe                                                                
cd ..                                                                      
cd textengines                                                             
qmake textengines.pro -o Makefile                            
mingw32-make.exe                                                                
cd ..                                                                      
qmake qwt.pro -o Makefile                                    
mingw32-make.exe                                                                
cd ..\win-mingw                                                        
rem pause                                                                  
    
cd ..\designer                                                             
cd src                                                                     
qmake  pvbdummy.pro -o Makefile                              
mingw32-make.exe                                                                
qmake  pvbplugin.pro -o Makefile                             
mingw32-make.exe                                                                
cd ..                                                                      
cd ..                                                                      
cd win-mingw                                                               
rem rem pause                                                                  

cd pvbrowser                                                               
qmake pvbrowser.pro -o Makefile                              
mingw32-make.exe                                                                
cd ..                                                                      
rem pause                                                                  
                                                                           
echo Netscape plugins are not supported in Qt5    
rem cd browserplugin                                                           
rem qmake pvpluginmain.pro -o Makefile                           
rem mingw32-make.exe                                                                
rem cd ..                                                                      
rem rem pause                                                                  
                                                                           
cd pvdevelop                                                               
qmake pvdevelop.pro -o Makefile                              
mingw32-make.exe                                                                
cd ..                                                                      
rem pause                                                                  
                                                                           
cd pvserver                                                                
qmake pvsmt.pro -o Makefile                                  
mingw32-make.exe                                                                
cd ..                                                                      
rem pause                                                                  
                                                                           
cd rllib                                                                   
qmake lib.pro -o Makefile                                    
mingw32-make.exe                                                                
cd ..                                                                      
copy rllib\release\librllib.a bin\librllib.a                                       
rem pause                                                                  
                                                                           
cd rlsvg                                                                   
qmake rlsvgcat.pro -o Makefile                               
mingw32-make.exe                                                                
cd ..                                                                      
rem pause                                                                  
                                                                           
cd rlhistory                                                               
qmake rlhistory.pro -o Makefile                              
mingw32-make.exe                                                                
cd ..                                                                      
rem pause                                                                  
                                                                           
cd fake_qmake                                                               
qmake fake_qmake.pro -o Makefile                              
mingw32-make.exe                                                                
cd ..                                                                      
rem pause                                                                  
                                                                           
cd ..\start_pvbapp                                                         
qmake start_pvbapp.pro -o Makefile                           
mingw32-make.exe                                                                
cd ..\win-mingw                                                            
rem pause                                                                  
                                                                           
cd pvmore                                                              
qmake pvmore.pro -o Makefile                           
mingw32-make.exe                                                                
cd ..                                                                      

%mingwdir%\bin\gcc.exe nodave.c -o nodave_mingw.o -c -D_WIN32 -DBCCWIN -I%mingwdir%\include
                                                                           
copy pvbrowser\release\pvbrowser.exe                             bin\pvbrowser.exe                               
rem copy browserplugin\release\nppvbrowser.dll                       bin\nppvbrowser.dll                             
copy pvdevelop\release\pvdevelop.exe                             bin\pvdevelop.exe                               
copy pvserver\release\libserverlib.a                             bin\libserverlib.a                              
copy rlsvg\release\rlsvgcat.exe                                  bin\rlsvgcat.exe                                
copy rlhistory\release\rlhistory.exe                             bin\rlhistory.exe                               
copy ..\start_pvbapp\release\\start_pvbapp.exe                   bin\start_pvbapp.exe                            
copy fake_qmake\release\fake_qmake.exe                           bin\fake_qmake.exe                              
xcopy /e /y "%QTMINGW%\plugins\imageformats"                       bin\imageformats                                
copy ..\qwt\designer\plugins\designer\qwt_designer_plugin5.dll   bin\plugins\designer\qwt_designer_plugin5.dll   
copy ..\qwt\designer\plugins\designer\libqwt_designer_plugin5.a  bin\plugins\designer\libqwt_designer_plugin5.a  
copy ..\designer\plugins\pvb_designer_plugin.dll                 bin\plugins\designer\pvb_designer_plugin.dll    
copy ..\designer\plugins\libpvb_designer_plugin.a                bin\plugins\designer\libpvb_designer_plugin.a   
rem copy bin\plugins\designer\*designer_plugin*                      %QTDIR%\plugins\designer\                     

cd bin
copy %QTBIN%\icudt51.dll
copy %QTBIN%\icuin51.dll
copy %QTBIN%\icuuc51.dll
copy %QTBIN%\libgcc_s_dw2-1.dll
rem copy %QTBIN%\libstdc++-6.dll
copy %QTBIN%\libwinpthread-1.dll
copy %QTBIN%\Qt5CLucene.dll
copy %QTBIN%\Qt5Concurrent.dll
copy %QTBIN%\Qt5Core.dll
copy %QTBIN%\Qt5Declarative.dll
copy %QTBIN%\Qt5Designer.dll
copy %QTBIN%\Qt5DesignerComponents.dll
copy %QTBIN%\Qt5Gui.dll
copy %QTBIN%\Qt5Help.dll
copy %QTBIN%\Qt5Multimedia.dll
copy %QTBIN%\Qt5MultimediaQuick_p.dll
copy %QTBIN%\Qt5MultimediaWidgets.dll
copy %QTBIN%\Qt5Network.dll
copy %QTBIN%\Qt5OpenGL.dll
copy %QTBIN%\Qt5PrintSupport.dll
copy %QTBIN%\Qt5Qml.dll
copy %QTBIN%\Qt5Quick.dll
copy %QTBIN%\Qt5QuickParticles.dll
copy %QTBIN%\Qt5QuickTest.dll
copy %QTBIN%\Qt5Script.dll
copy %QTBIN%\Qt5ScriptTools.dll
copy %QTBIN%\Qt5Sensors.dll
copy %QTBIN%\Qt5SerialPort.dll
copy %QTBIN%\Qt5Sql.dll
copy %QTBIN%\Qt5Svg.dll
copy %QTBIN%\Qt5Test.dll
copy %QTBIN%\Qt5V8.dll
copy %QTBIN%\Qt5WebKit.dll
copy %QTBIN%\Qt5WebKitWidgets.dll
copy %QTBIN%\Qt5Widgets.dll
copy %QTBIN%\Qt5Xml.dll
copy %QTBIN%\Qt5XmlPatterns.dll

copy %MINGWDIR%\bin\*.dll
cd ..

echo ##############################################################################
echo # run me again in order to verify that no errors occured                     #
echo # If you_want a Lua and Python binding then run: build_language_bindings.bat #
echo # Bulding the language binding will produce a lot of WARNINGS !!!            #    
echo ##############################################################################

