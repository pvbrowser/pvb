#######################################                                     
# project file for pvbrowser          #                                    
# you can uncomment CONFIG += USE_VTK #                                    
#######################################                                    
#CONFIG       += USE_VTK                                                   
#DEFINES     += NO_QWT                                                     
TARGET        = pvbrowser                                                  
                                                                           
QT           += opengl svg webkit network                                  
INCLUDEPATH  += ../../pvbrowser                                            
#INCLUDEPATH  += ../../qwt/include                                         
INCLUDEPATH  += ../../qwt/src                                              
DEFINES      += BROWSERPLUGIN                                              
DEFINES      -= UNICODE                                                    
QT           += opengl svg webkit network                                  
CONFIG       += uitools warn_on release                                    
DEFINES      += "WINVER=0x0501"

HEADERS       = ../../pvbrowser/mainwindow.h \                             
                ../../pvbrowser/dlgopt.h \                                 
                ../../pvbrowser/opt.h \                                    
                ../../pvbrowser/tcputil.h \                                
                ../../pvbrowser/interpreter.h \                            
                ../../pvbrowser/pvserver.h \                               
                ../../pvbrowser/MyWidgets.h \                              
                ../../pvbrowser/qimagewidget.h \                           
                ../../pvbrowser/qdrawwidget.h \                            
                ../../pvbrowser/pvglwidget.h \                             
                ../../pvbrowser/qwtwidgets.h \                             
                ../../pvbrowser/qwtplotwidget.h \                          
                ../../pvbrowser/dlgtextbrowser.h \                         
                ../../pvbrowser/webkit_ui_dlgtextbrowser.h \               
                ../../pvbrowser/dlgmybrowser.h                             
                                                                           
SOURCES       = ../../browserplugin/pvpluginmain.cpp \                     
                ../../pvbrowser/main.cpp \                                 
                ../../pvbrowser/mainwindow.cpp \                           
                ../../pvbrowser/dlgopt.cpp \                               
                ../../pvbrowser/opt.cpp \                                  
                ../../pvbrowser/tcputil.cpp \                              
                ../../pvbrowser/interpreter.cpp \                          
                ../../pvbrowser/MyWidgets.cpp \                            
                ../../pvbrowser/QDrawWidget.cpp \                          
                ../../pvbrowser/QImageWidget.cpp \                         
                ../../pvbrowser/pvglwidget.cpp \                           
                ../../pvbrowser/gldecode.cpp \                             
                ../../pvbrowser/qwtplotwidget.cpp \                        
                ../../pvbrowser/dlgtextbrowser.cpp \                       
                ../../pvbrowser/dlgmybrowser.cpp                           
                                                                           
                                                                           
RESOURCES     = ../../browserplugin/pvbrowser.qrc                          
                                                                           
LIBS         += ../../qwt/lib/libqwt.a                                     
LIBS         += $(MINGWDIR)/lib/libws2_32.a                                
LIBS         += $(MINGWDIR)/lib/libimm32.a                                 
LIBS         += $(MINGWDIR)/lib/libopengl32.a                              
LIBS         += $(MINGWDIR)/lib/libglu32.a                                 
LIBS         += $(MINGWDIR)/lib/libadvapi32.a                              
                                                                           
win32 {                                                                    
   RC_FILE    = ../../browserplugin/pvpluginmain.rc                        
} else:mac {                                                               
   QMAKE_INFO_PLIST = Info.plist                                           
   REZ_FILES += pvpluginmain.r                                             
   rsrc_files.files = pvpluginmain.rsrc                                    
   rsrc_files.path = Contents/Resources                                    
   QMAKE_BUNDLE_DATA += rsrc_files                                         
}                                                                          
                                                                           
include(../../browserplugin/qtbrowserplugin.pri)                           
