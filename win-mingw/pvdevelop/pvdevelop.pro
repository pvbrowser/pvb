lessThan(QT_MAJOR_VERSION, 5) {
  QT         += opengl svg webkit
  CONFIG     += uitools warn_on release
  LIBS       += $(MINGWDIR)/lib/libws2_32.a                                
  LIBS       += $(MINGWDIR)/lib/libimm32.a                                 
  LIBS       += $(MINGWDIR)/lib/libopengl32.a                              
  LIBS       += $(MINGWDIR)/lib/libglu32.a                                 
  LIBS       += $(MINGWDIR)/lib/libadvapi32.a                              
}else{
  QT         += printsupport uitools webkitwidgets widgets opengl svg webkit
  CONFIG     += warn_on release
  LIBS       += -lws2_32                                
  LIBS       += -limm32                                
  LIBS       += -lopengl32                              
  LIBS       += -lglu32                                 
  LIBS       += -ladvapi32                              
}    
DEFINES      += PVDEVELOP                                                  
#DEFINES     += NO_QWT                                                     
DEFINES      -= UNICODE                                                    
DEFINES      += "WINVER=0x0501"

HEADERS       = ../../pvbrowser/webkit_ui_dlgtextbrowser.h \               
                ../../pvbrowser/MyWidgets.h \                              
                ../../pvbrowser/MyTextBrowser_v4.h \
                ../../pvbrowser/qwtwidgets.h \                             
                ../../pvbrowser/qwtplotwidget.h \                          
                ../../pvbrowser/qdrawwidget.h \                            
                ../../pvbrowser/qimagewidget.h \                           
                ../../pvbrowser/pvglwidget.h \                             
                ../../pvbrowser/interpreter.h \                            
                ../../pvbrowser/pvserver.h \                               
                ../../pvbrowser/dlgtextbrowser.h \                         
                ../../pvdevelop/mainwindow.h \                             
                ../../pvdevelop/opt.h \                                    
                ../../pvdevelop/editor.h \                                 
                ../../pvdevelop/designer.h \                               
                ../../pvdevelop/syntax.h \                                 
                ../../pvdevelop/cutil.h \                                  
                ../../pvdevelop/dlgopt.h \                                 
                ../../pvdevelop/dlgnewprj.h \                              
                ../../pvdevelop/dlgsearchreplace.h \                       
                ../../pvdevelop/dlginsertFunction.h \                      
                ../../pvdevelop/dlgpastewidget.h \                         
                ../../pvdevelop/dlgdaemon.h \                              
                ../../pvdevelop/dlgproperty.h \                            
                ../../pvdevelop/dlginsertwidget.h \                        
                ../../pvdevelop/dlgeditlayout.h \                          
                ../../pvdevelop/widgetgenerator.h \                        
                ../../pvdevelop/generatepython.h \                         
                ../../pvdevelop/generatephp.h \                            
                ../../pvdevelop/generateperl.h \                           
                ../../pvdevelop/generatetcl.h \                            
                ../../pvdevelop/ui_dlgnewprj.h \                           
                ../../pvdevelop/ui_dlgsearchreplace.h \                    
                ../../pvdevelop/ui_dlginsertfunction.h \                   
                ../../pvdevelop/ui_dlgpastewidget.h \                      
                ../../pvdevelop/ui_dlgdaemon.h \                           
                ../../pvdevelop/ui_dlgproperty.h \                         
                ../../pvdevelop/ui_dlginsertwidget.h \                     
                ../../pvdevelop/ui_dlgeditlayout.h \                       
                                                                           
SOURCES       = ../../pvbrowser/MyWidgets.cpp \                            
                ../../pvbrowser/MyTextBrowser_v4.cpp \
                ../../pvbrowser/qwtplotwidget.cpp \                        
                ../../pvbrowser/QDrawWidget.cpp \                          
                ../../pvbrowser/QImageWidget.cpp \                         
                ../../pvbrowser/pvglwidget.cpp \                           
                ../../pvbrowser/tcputil.cpp \                              
                ../../pvdevelop/main.cpp \                                 
                ../../pvdevelop/mainwindow.cpp \                           
                ../../pvdevelop/opt.cpp \                                  
                ../../pvdevelop/editor.cpp \                               
                ../../pvdevelop/designer.cpp \                             
                ../../pvdevelop/syntax.cpp \                               
                ../../pvdevelop/cutil.cpp \                                
                ../../pvdevelop/dlgopt.cpp \                               
                ../../pvdevelop/dlgnewprj.cpp \                            
                ../../pvdevelop/dlgsearchreplace.cpp \                     
                ../../pvdevelop/dlginsertFunction.cpp \                    
                ../../pvdevelop/dlgpastewidget.cpp \                       
                ../../pvbrowser/dlgtextbrowser.cpp \                       
                ../../pvdevelop/dlgdaemon.cpp \                            
                ../../pvdevelop/dlgproperty.cpp \                          
                ../../pvdevelop/dlginsertwidget.cpp \                      
                ../../pvdevelop/dlgeditlayout.cpp \                        
                ../../pvdevelop/gmodbus.cpp \                              
                ../../pvdevelop/gppi.cpp \                                 
                ../../pvdevelop/gsiemens.cpp \                             
                ../../pvdevelop/widgetgenerator.cpp \                      
                                                                           
FORMS         = ../../pvdevelop/dlgnewprj.ui \                             
                ../../pvdevelop/dlgsearchreplace.ui \                      
                ../../pvdevelop/dlginsertfunction.ui \                     
                ../../pvdevelop/dlgpastewidget.ui \                        
                ../../pvdevelop/dlgdaemon.ui \                             
                ../../pvdevelop/dlgproperty.ui \                           
                ../../pvdevelop/dlginsertwidget.ui \                       
                ../../pvdevelop/dlgeditlayout.ui                           
                                                                           
#INCLUDEPATH  += ../../qwt/include                                         
INCLUDEPATH  += ../../qwt/src                                              
INCLUDEPATH  += ../../pvbrowser                                            
INCLUDEPATH  += ../../pvdevelop                                            
                                                                           
LIBS         += ../../qwt/lib/libqwt.a                                     
                                                                           
RESOURCES     = ../../pvdevelop/pvdevelop.qrc                              
TARGET = pvdevelop                                                         
