#######################################
# project file for pvbrowser          #
# you can uncomment CONFIG += USE_VTK #
#######################################
#CONFIG      += USE_VTK
#DEFINES     += NO_QWT
linux-g++-gles2 {
  DEFINES    += USE_MAEMO
}  
android-g++ {
  DEFINES    += USE_ANDROID
  DEFINES    += USE_MAEMO
}  
macx:DEFINES += PVMAC
macx:DEFINES += unix
QT           += xml opengl svg webkit network
#mobile devices without opengl
#QT           += xml svg webkit network
QMAKE_LFLAGS += -static-libgcc

HEADERS       = mainwindow.h \
                dlgopt.h \
                opt.h \
                tcputil.h \
                interpreter.h \
                pvserver.h \
                MyWidgets.h \
                qimagewidget.h \
                qdrawwidget.h \
                pvglwidget.h \
                qwtwidgets.h \
                qwtplotwidget.h \
                dlgtextbrowser.h \
                webkit_ui_dlgtextbrowser.h \
                dlgmybrowser.h \
                pvdefine.h

SOURCES       = main.cpp \
                mainwindow.cpp \
                dlgopt.cpp \
                opt.cpp \
                tcputil.cpp \
                interpreter.cpp \
                MyWidgets.cpp \
                QDrawWidget.cpp \
                QImageWidget.cpp \
                pvglwidget.cpp \
                gldecode.cpp \
                qwtplotwidget.cpp \
                dlgtextbrowser.cpp \
                dlgmybrowser.cpp

# FORMS        += dlgtextbrowser.ui
#               dlgmybrowser.ui

#INCLUDEPATH  += ../qwt/include
INCLUDEPATH  += ../qwt/src
LIBS         += ../qwt/lib/libqwt.a

### begin USE_VTK #############################################
USE_VTK {

DEFINES      += USE_VTK

HEADERS      += pvVtkTclWidget.h \
                QVTKWidget.h

SOURCES      += tkAppInit.cpp \
                pvVtkTclWidget.cpp \
                QVTKWidget.cpp

INCLUDEPATH  += /usr/local/include/vtk-5.0
LIBS += /usr/local/lib/libvtkCommon.so
LIBS += /usr/local/lib/libvtkCommonTCL.so
LIBS += /usr/local/lib/libvtkDICOMParser.so
LIBS += /usr/local/lib/libvtkexoIIc.so
LIBS += /usr/local/lib/libvtkexpat.so
LIBS += /usr/local/lib/libvtkFiltering.so
LIBS += /usr/local/lib/libvtkFilteringTCL.so
LIBS += /usr/local/lib/libvtkfreetype.so
LIBS += /usr/local/lib/libvtkftgl.so
LIBS += /usr/local/lib/libvtkGenericFiltering.so
LIBS += /usr/local/lib/libvtkGenericFilteringTCL.so
LIBS += /usr/local/lib/libvtkGraphics.so
LIBS += /usr/local/lib/libvtkGraphicsTCL.so
LIBS += /usr/local/lib/libvtkHybrid.so
LIBS += /usr/local/lib/libvtkHybridTCL.so
LIBS += /usr/local/lib/libvtkImaging.so
LIBS += /usr/local/lib/libvtkImagingTCL.so
LIBS += /usr/local/lib/libvtkIO.so
LIBS += /usr/local/lib/libvtkIOTCL.so
LIBS += /usr/local/lib/libvtkjpeg.so
LIBS += /usr/local/lib/libvtkMPEG2Encode.so
LIBS += /usr/local/lib/libvtkNetCDF.so
LIBS += /usr/local/lib/libvtkpng.so
LIBS += /usr/local/lib/libvtkRendering.so
LIBS += /usr/local/lib/libvtkRenderingTCL.so
LIBS += /usr/local/lib/libvtksys.so
LIBS += /usr/local/lib/libvtktiff.so
LIBS += /usr/local/lib/libvtkVolumeRendering.so
LIBS += /usr/local/lib/libvtkVolumeRenderingTCL.so
LIBS += /usr/local/lib/libvtkWidgets.so
LIBS += /usr/local/lib/libvtkWidgetsTCL.so
LIBS += /usr/local/lib/libvtkzlib.so
LIBS += /usr/lib/libtcl8.4.so

}
### end USE_VTK ###############################################

RESOURCES     = pvbrowser.qrc
TARGET        = pvbrowser

# install
target.path   = /usr/local/bin
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS pvbrowser.pro images
sources.path  = /opt/pvb/pvbrowser
INSTALLS     += target sources
