#######################################                                     
# project file for pvbrowser          #                                    
# you can uncomment CONFIG += USE_VTK #                                    
#######################################                                    
#CONFIG        += USE_VTK                                                   
#DEFINES       += NO_QWT                                                     
DEFINES        -= UNICODE                                                    
DEFINES        += "WINVER=0x0501"
QT             += opengl                                  
lessThan(QT_MAJOR_VERSION, 5) {
  QT           += xml svg webkit network
  CONFIG       += uitools warn_on release                                    
}else{
  QT           += printsupport multimedia uitools webkitwidgets widgets xml svg webkit network opengl
  CONFIG       += warn_on release                                    
  LIBS         += -lws2_32
}    
QMAKE_CXXFLAGS += -mthreads
QMAKE_LFLAGS   += -mthreads

HEADERS       = ../../pvbrowser/mainwindow.h \                             
                ../../pvbrowser/dlgopt.h \                                 
                ../../pvbrowser/opt.h \                                    
                ../../pvbrowser/tcputil.h \                                
                ../../pvbrowser/interpreter.h \                            
                ../../pvbrowser/pvserver.h \                               
                ../../pvbrowser/MyWidgets.h \                              
                ../../pvbrowser/MyTextBrowser_v4.h \
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
                ../../pvbrowser/MyTextBrowser_v4.cpp \
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
LIBS         += $(MINGWDIR)/i686-w64-mingw32/lib/libopengl32.a
lessThan(QT_MAJOR_VERSION, 5) {
LIBS         += $(MINGWDIR)/lib/libws2_32.a                                
LIBS         += $(MINGWDIR)/lib/libimm32.a                                 
LIBS         += $(MINGWDIR)/lib/libopengl32.a                              
LIBS         += $(MINGWDIR)/lib/libglu32.a                                 
LIBS         += $(MINGWDIR)/lib/libadvapi32.a                              
}else{
LIBS         += $(MINGWDIR)/lib/libbfd.a
LIBS         += $(MINGWDIR)/lib/libiberty.a
LIBS         += $(MINGWDIR)/lib/libmangle.a
LIBS         += $(MINGWDIR)/lib/libopcodes.a
}    
                                                                           
### begin USE_VTK #############################################            
USE_VTK {                                                                  
                                                                           
DEFINES      += USE_VTK                                                    
                                                                           
HEADERS      += ../../pvbrowser/pvVtkTclWidget.h \                        
                ../../pvbrowser/QVTKWidget.h                               
                                                                           
SOURCES      += ../../pvbrowser/pvVtkTclWidget.cpp \                     
                ../../pvbrowser/QVTKWidget.cpp                             

QMAKE_LFLAGS   += --enable-auto-import
                                                                           
# the following paths have been used for testing.
# you might need to modify these paths.
# for testing vtk was build with "../bin" outside of the VTK source directory
INCLUDEPATH  += $(VTKDIR)/Rendering                                 
INCLUDEPATH  += $(VTKDIR)/Common                                    
INCLUDEPATH  += $(VTKDIR)/Filtering                                 
INCLUDEPATH  += $(VTKDIR)/Graphics                                  
INCLUDEPATH  += $(VTKDIR)                                                  
INCLUDEPATH  += $(VTKDIR)/../bin                                                  
INCLUDEPATH  += $(VTKDIR)/../bin/Rendering                                 
INCLUDEPATH  += $(VTKDIR)/GUISupport/Qt
INCLUDEPATH  += $(TCLDIR)/include                                          
                                                                           
LIBS += $(VTKDIR)\..\bin\bin\libvtkCommon.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtkCommonTCL.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libLSDyna.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libMapReduceMPI.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libmpistubs.dll.a
#LIBS += $(VTKDIR)\..\bin\bin\libQVTK.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libQVTKWidgetPlugin.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtkalglib.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtkDICOMParser.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtkexoIIc.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtkexpat.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtkFiltering.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtkFilteringTCL.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtkfreetype.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtkftgl.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtkGenericFiltering.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtkGenericFilteringTCL.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtkGraphics.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtkGraphicsTCL.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtkHybrid.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtkHybridTCL.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtkImaging.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtkImagingTCL.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtkIO.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtkIOTCL.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtkjpeg.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtklibxml2.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtkmetaio.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtkNetCDF_cxx.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtkNetCDF.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtkpng.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtkproj4.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtkRendering.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtkRenderingTCL.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtksys.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtktiff.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtkverdict.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtkVolumeRendering.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtkVolumeRenderingTCL.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtkWidgets.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtkWidgetsTCL.dll.a
LIBS += $(VTKDIR)\..\bin\bin\libvtkzlib.dll.a
LIBS += $(TCLDIR)\lib\tcl85.lib                                                                    
}                                                                          
### end USE_VTK ###############################################            
                                                                           
RESOURCES     = ../../pvbrowser/pvbrowser.qrc                              
TARGET        = pvbrowser                                                  
