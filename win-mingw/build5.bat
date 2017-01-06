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
                                                                           
cd rlfind                                                                   
qmake rlfind.pro -o Makefile                               
mingw32-make.exe                                                                
cd ..                                                                      
rem pause                                                                  
                                                                           
cd rlhistory                                                               
qmake rlhistory.pro -o Makefile                              
mingw32-make.exe                                                                
cd ..                                                                      
rem pause                                                                  
                                                                           
cd rlhtml2pdf                                                               
qmake rlhtml2pdf.pro -o Makefile                              
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
copy rlfind\release\rlfind.exe                                   bin\rlfind.exe                                
copy rlhistory\release\rlhistory.exe                             bin\rlhistory.exe                               
copy rlhtml2pdf\release\rlhtml2pdf.exe                           bin\rlhtml2pdf.exe                               
copy ..\start_pvbapp\release\\start_pvbapp.exe                   bin\start_pvbapp.exe                            
copy fake_qmake\release\fake_qmake.exe                           bin\fake_qmake.exe                              
xcopy /e /y "%QTDIR%\plugins\imageformats"                       bin\imageformats                                
xcopy /e /y "%QTDIR%\plugins\playlistformats"                    bin\playlistformats                                
xcopy /e /y "%QTDIR%\plugins\mediaservice"                       bin\mediaservice                                
xcopy /e /y "%QTDIR%\plugins\printsupport"                       bin\printsupport                               
xcopy /e /y "%QTDIR%\plugins\sqldrivers"                         bin\sqldrivers                                
mkdir                                                            bin\platforms
copy "%QTDIR%\plugins\platforms\qwindows.dll"                    bin\platforms\                                
copy "%QTDIR%\plugins\platforms\qoffscreen.dll"                  bin\platforms\                                
copy "%QTDIR%\plugins\platforms\qminimal.dll"                    bin\platforms\                                
copy ..\qwt\designer\plugins\designer\qwt_designer_plugin5.dll   bin\plugins\designer\qwt_designer_plugin5.dll   
copy ..\qwt\designer\plugins\designer\libqwt_designer_plugin5.a  bin\plugins\designer\libqwt_designer_plugin5.a  
copy ..\designer\plugins\pvb_designer_plugin.dll                 bin\plugins\designer\pvb_designer_plugin.dll    
copy ..\designer\plugins\libpvb_designer_plugin.a                bin\plugins\designer\libpvb_designer_plugin.a   
rem copy bin\plugins\designer\*designer_plugin*                      %QTDIR%\plugins\designer\                     

cd bin
copy %QTDIR%\bin\icu*.dll
copy %QTDIR%\bin\libgcc_s_dw2-1.dll
rem copy %QTDIR%\bin\libstdc++-6.dll
copy %QTDIR%\bin\libwinpthread-1.dll
copy %QTDIR%\bin\Qt5CLucene.dll
copy %QTDIR%\bin\Qt5Concurrent.dll
copy %QTDIR%\bin\Qt5Core.dll
copy %QTDIR%\bin\Qt5Declarative.dll
copy %QTDIR%\bin\Qt5Designer.dll
copy %QTDIR%\bin\Qt5DesignerComponents.dll
copy %QTDIR%\bin\Qt5Gui.dll
copy %QTDIR%\bin\Qt5Help.dll
copy %QTDIR%\bin\Qt5Multimedia.dll
copy %QTDIR%\bin\Qt5MultimediaQuick_p.dll
copy %QTDIR%\bin\Qt5MultimediaWidgets.dll
copy %QTDIR%\bin\Qt5Network.dll
copy %QTDIR%\bin\Qt5OpenGL.dll
copy %QTDIR%\bin\Qt5PrintSupport.dll
copy %QTDIR%\bin\Qt5Qml.dll
copy %QTDIR%\bin\Qt5Quick.dll
copy %QTDIR%\bin\Qt5QuickParticles.dll
copy %QTDIR%\bin\Qt5QuickTest.dll
copy %QTDIR%\bin\Qt5Script.dll
copy %QTDIR%\bin\Qt5ScriptTools.dll
copy %QTDIR%\bin\Qt5Sensors.dll
copy %QTDIR%\bin\Qt5SerialPort.dll
copy %QTDIR%\bin\Qt5Sql.dll
copy %QTDIR%\bin\Qt5Svg.dll
copy %QTDIR%\bin\Qt5Test.dll
copy %QTDIR%\bin\Qt5WebKit.dll
copy %QTDIR%\bin\Qt5WebKitWidgets.dll
copy %QTDIR%\bin\Qt5WebView.dll
copy %QTDIR%\bin\Qt5Widgets.dll
copy %QTDIR%\bin\Qt5Xml.dll
copy %QTDIR%\bin\Qt5XmlPatterns.dll
copy %QTDIR%\bin\Qt5Positioning.dll
copy %QTDIR%\bin\Qt5WebChannel.dll

copy %MINGWDIR%\bin\*.dll
cd ..

echo ##############################################################################
echo # run me again in order to verify that no errors occured                     #
echo # If you_want a Lua and Python binding then run: build_language_bindings.bat #
echo # Bulding the language binding will produce a lot of WARNINGS !!!            #    
echo ##############################################################################

