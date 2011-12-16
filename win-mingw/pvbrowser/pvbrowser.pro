#######################################                                     
# project file for pvbrowser          #                                    
# you can uncomment CONFIG += USE_VTK #                                    
#######################################                                    
#CONFIG        += USE_VTK                                                   
#DEFINES       += NO_QWT                                                     
DEFINES        -= UNICODE                                                    
QT             += opengl svg webkit network                                  
CONFIG         += uitools warn_on release                                    
QMAKE_CXXFLAGS += -mthreads
QMAKE_LFLAGS   += -mthreads

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
                                                                           
SOURCES       = ../../pvbrowser/main.cpp \                                 
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
                                                                           
                                                                           
#INCLUDEPATH  += ../../qwt/include                                         
INCLUDEPATH  += ../../qwt/src                                              
LIBS         += ../../qwt/lib/libqwt.a                                     
LIBS         += $(MINGWDIR)/lib/libws2_32.a                                
LIBS         += $(MINGWDIR)/lib/libimm32.a                                 
LIBS         += $(MINGWDIR)/lib/libopengl32.a                              
LIBS         += $(MINGWDIR)/lib/libglu32.a                                 
LIBS         += $(MINGWDIR)/lib/libadvapi32.a                              
                                                                           
### begin USE_VTK #############################################            
USE_VTK {                                                                  
                                                                           
DEFINES      += USE_VTK                                                    
                                                                           
HEADERS      += ../../pvbrowser/pvVtkTclWidget.h \                         
                ../../pvbrowser/QVTKWidget.h                               
                                                                           
SOURCES      += ../../pvbrowser/pvVtkTclWidget.cpp \                       
                ../../pvbrowser/QVTKWidget.cpp                             
                                                                           
INCLUDEPATH  += $(HOME)\vtk5\VTK\Rendering                                 
INCLUDEPATH  += $(HOME)\vtk5\VTK\Common                                    
INCLUDEPATH  += $(HOME)\vtk5\VTK\Filtering                                 
INCLUDEPATH  += $(HOME)\vtk5\VTK\Graphics                                  
INCLUDEPATH  += $(VTKDIR)                                                  
INCLUDEPATH  += $(TCLDIR)/include                                          
                                                                           
LIBS += $(VTKDIR)\bin\vtkCommon.lib                                        
LIBS += $(VTKDIR)\bin\vtkCommonTCL.lib                                     
LIBS += $(VTKDIR)\bin\vtkDICOMParser.lib                                   
LIBS += $(VTKDIR)\bin\vtkexoIIc.lib                                        
LIBS += $(VTKDIR)\bin\vtkexpat.lib                                         
LIBS += $(VTKDIR)\bin\vtkFiltering.lib                                     
LIBS += $(VTKDIR)\bin\vtkFilteringTCL.lib                                  
LIBS += $(VTKDIR)\bin\vtkfreetype.lib                                      
LIBS += $(VTKDIR)\bin\vtkftgl.lib                                          
LIBS += $(VTKDIR)\bin\vtkGenericFiltering.lib                              
LIBS += $(VTKDIR)\bin\vtkGenericFilteringTCL.lib                           
LIBS += $(VTKDIR)\bin\vtkGraphics.lib                                      
LIBS += $(VTKDIR)\bin\vtkGraphicsTCL.lib                                   
LIBS += $(VTKDIR)\bin\vtkHybrid.lib                                        
LIBS += $(VTKDIR)\bin\vtkHybridTCL.lib                                     
LIBS += $(VTKDIR)\bin\vtkImaging.lib                                       
LIBS += $(VTKDIR)\bin\vtkImagingTCL.lib                                    
LIBS += $(VTKDIR)\bin\vtkIO.lib                                            
LIBS += $(VTKDIR)\bin\vtkIOTCL.lib                                         
LIBS += $(VTKDIR)\bin\vtkjpeg.lib                                          
LIBS += $(VTKDIR)\bin\vtkMPEG2Encode.lib                                   
LIBS += $(VTKDIR)\bin\vtkNetCDF.lib                                        
LIBS += $(VTKDIR)\bin\vtkpng.lib                                           
LIBS += $(VTKDIR)\bin\vtkRendering.lib                                     
LIBS += $(VTKDIR)\bin\vtkRenderingTCL.lib                                  
LIBS += $(VTKDIR)\bin\vtksys.lib                                           
LIBS += $(VTKDIR)\bin\vtktiff.lib                                          
LIBS += $(VTKDIR)\bin\vtkVolumeRendering.lib                               
LIBS += $(VTKDIR)\bin\vtkVolumeRenderingTCL.lib                            
LIBS += $(VTKDIR)\bin\vtkWidgets.lib                                       
LIBS += $(VTKDIR)\bin\vtkWidgetsTCL.lib                                    
LIBS += $(VTKDIR)\bin\vtkzlib.lib                                          
LIBS += $(TCLDIR)\lib\tcl84.lib                                            
                                                                           
}                                                                          
### end USE_VTK ###############################################            
                                                                           
RESOURCES     = ../../pvbrowser/pvbrowser.qrc                              
TARGET        = pvbrowser                                                  
