#######################################
# project file for pvbrowser          #
# you can uncomment CONFIG += USE_VTK #
#######################################
CONFIG       += warn_on release
#CONFIG      += USE_VTK
#DEFINES     += NO_QWT
DEFINES      += USE_GOOGLE_WEBKIT_FORK
#DEFINES      += PVB_FOOTPRINT_OLD_VERSION_BEFORE_QWT62
#mobile devices without opengl
#QT           += opengl
QT           += printsupport multimedia uitools webenginewidgets widgets xml svg network printsupport

linux-g++-gles2 {
  DEFINES    += USE_MAEMO
  QT         -= opengl
}  
symbian:CONFIG += USE_SYMBIAN
USE_SYMBIAN {
  DEFINES    += USE_SYMBIAN
  DEFINES    += USE_MAEMO
  DEFINES    += NO_PRINTER
  TARGET.CAPABILITY = "NetworkServices ReadUserData WriteUserData"
  LIBS       += -lqwt.lib
  QT         -= opengl
}

macx:DEFINES += PVMAC
macx:DEFINES += unix
unix:!symbian:LIBS    += -ldl
!symbian:QMAKE_LFLAGS += -static-libgcc

HEADERS       = mainwindow.h \
                dlgopt.h \
                opt.h \
                tcputil.h \
                interpreter.h \
                pvserver.h \
                MyWidgets.h \
                MyTextBrowser_with_www.h \
                mywebenginepage.h \
                qimagewidget.h \
                qdrawwidget.h \
                qwtwidgets.h \
                qwtplotwidget.h \
                dlgtextbrowser.h \
                webkit_ui_dlgtextbrowser.h \
                dlgmybrowser_with_www.h \
                pvdefine.h

SOURCES       = main.cpp \
                mainwindow.cpp \
                dlgopt.cpp \
                opt.cpp \
                tcputil.cpp \
                interpreter.cpp \
                MyWidgets.cpp \
                MyTextBrowser_with_www.cpp \
                mywebenginepage.cpp \
                QDrawWidget.cpp \
                QImageWidget.cpp \
                qwtplotwidget.cpp \
                dlgtextbrowser.cpp \
                dlgmybrowser_with_www.cpp

message($$QMAKE_HOST.arch)

contains(QMAKE_HOST.arch, "i686") {
message("i686 -> USE_OPEN_GL")
DEFINES      += USE_OPEN_GL
HEADERS      += pvglwidget.h
SOURCES      += pvglwidget.cpp \
                gldecode.cpp
}
contains(QMAKE_HOST.arch, "x86_64") {
message("x86_64 -> USE_OPEN_GL")
DEFINES      += USE_OPEN_GL
HEADERS      += pvglwidget.h
SOURCES      += pvglwidget.cpp \
                gldecode.cpp
}

# FORMS        += dlgtextbrowser.ui
#               dlgmybrowser.ui

#INCLUDEPATH  += ../qwt/include
INCLUDEPATH  += ../qwt/src
symbian {

}
else {
LIBS         += ../qwt/lib/libqwt.a
}

### begin USE_VTK #############################################
USE_VTK {

DEFINES      += USE_VTK

HEADERS      += pvVtkTclWidget.h \
                QVTKWidget.h

SOURCES      += tkAppInit.cpp \
                pvVtkTclWidget.cpp \
                QVTKWidget.cpp

INCLUDEPATH  += /usr/local/include/vtk-5.10
LIBS += /usr/local/lib/vtk-5.10/libvtkCommon.so
LIBS += /usr/local/lib/vtk-5.10/libvtkCommonTCL.so
LIBS += /usr/local/lib/vtk-5.10/libvtkDICOMParser.so
LIBS += /usr/local/lib/vtk-5.10/libvtkexoIIc.so
LIBS += /usr/local/lib/vtk-5.10/libvtkexpat.so
LIBS += /usr/local/lib/vtk-5.10/libvtkFiltering.so
LIBS += /usr/local/lib/vtk-5.10/libvtkFilteringTCL.so
LIBS += /usr/local/lib/vtk-5.10/libvtkfreetype.so
LIBS += /usr/local/lib/vtk-5.10/libvtkftgl.so
LIBS += /usr/local/lib/vtk-5.10/libvtkGenericFiltering.so
LIBS += /usr/local/lib/vtk-5.10/libvtkGenericFilteringTCL.so
LIBS += /usr/local/lib/vtk-5.10/libvtkGraphics.so
LIBS += /usr/local/lib/vtk-5.10/libvtkGraphicsTCL.so
LIBS += /usr/local/lib/vtk-5.10/libvtkHybrid.so
LIBS += /usr/local/lib/vtk-5.10/libvtkHybridTCL.so
LIBS += /usr/local/lib/vtk-5.10/libvtkImaging.so
LIBS += /usr/local/lib/vtk-5.10/libvtkImagingTCL.so
LIBS += /usr/local/lib/vtk-5.10/libvtkIO.so
LIBS += /usr/local/lib/vtk-5.10/libvtkIOTCL.so
LIBS += /usr/local/lib/vtk-5.10/libvtkjpeg.so
#LIBS += /usr/local/lib/vtk-5.10/libvtkMPEG2Encode.so # commented out for vtk-5.10
LIBS += /usr/local/lib/vtk-5.10/libvtkNetCDF.so
LIBS += /usr/local/lib/vtk-5.10/libvtkpng.so
LIBS += /usr/local/lib/vtk-5.10/libvtkRendering.so
LIBS += /usr/local/lib/vtk-5.10/libvtkRenderingTCL.so
LIBS += /usr/local/lib/vtk-5.10/libvtksys.so
LIBS += /usr/local/lib/vtk-5.10/libvtktiff.so
LIBS += /usr/local/lib/vtk-5.10/libvtkVolumeRendering.so
LIBS += /usr/local/lib/vtk-5.10/libvtkVolumeRenderingTCL.so
LIBS += /usr/local/lib/vtk-5.10/libvtkWidgets.so
LIBS += /usr/local/lib/vtk-5.10/libvtkWidgetsTCL.so
LIBS += /usr/local/lib/vtk-5.10/libvtkzlib.so
LIBS += /usr/lib64/libtcl8.5.so
LIBS += /usr/lib64/libX11.so.6
LIBS += /usr/local/lib/vtk-5.10/libvtksqlite.so
LIBS += /usr/local/lib/vtk-5.10/libvtkNetCDF_cxx.so
LIBS += /usr/local/lib/vtk-5.10/libvtkNetCDF.so
LIBS += /usr/local/lib/vtk-5.10/libvtkhdf5.so
LIBS += /usr/local/lib/vtk-5.10/libvtkhdf5_hl.so
LIBS += /usr/local/lib/vtk-5.10/libvtkverdict.so
LIBS += /usr/local/lib/vtk-5.10/libvtkmetaio.so
LIBS += /usr/local/lib/vtk-5.10/libLSDyna.so
}
### end USE_VTK ###############################################
android-g++ {
  DEFINES    += USE_ANDROID
  DEFINES    += USE_MAEMO
  QT         -= opengl
  DEFINES    += NO_WEBKIT
  QT         -= webkit
  QT         -= webkitwidgets
  QT         -= webenginewidgets
  HEADERS    -= dlgmybrowser.h
  SOURCES    -= dlgmybrowser.cpp
}

RESOURCES     = pvbrowser.qrc
TARGET        = pvbrowser

# install
target.path   = /usr/local/bin
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS pvbrowser.pro images
sources.path  = /opt/pvb/pvbrowser
INSTALLS     += target sources
