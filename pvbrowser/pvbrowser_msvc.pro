#######################################
# project file for pvbrowser          #
# for Windows 64 Bit MSVC             #
# you can uncomment CONFIG += USE_VTK #
#######################################
CONFIG       += warn_on release
#CONFIG      += USE_VTK
#DEFINES     += NO_QWT
DEFINES      += USE_OPEN_GL
DEFINES      += USE_GOOGLE_WEBKIT_FORK
QT           += printsupport multimedia uitools webenginewidgets widgets xml svg network printsupport


HEADERS       = mainwindow.h \
                dlgopt.h \
                opt.h \
                tcputil.h \
                interpreter.h \
                pvserver.h \
                MyWidgets.h \
                MyTextBrowser_v5.h \
                mywebenginepage.h \
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
                MyTextBrowser_v5.cpp \
                mywebenginepage.cpp \
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
LIBS         += ../build-qwt-Desktop_Qt_5_7_1_MSVC2015_64bit-Debug/lib/qwt.lib
LIBS         += ws2_32.lib
LIBS         += imm32.lib
LIBS         += opengl32.lib
LIBS         += glu32.lib
LIBS         += advapi32.lib

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

RESOURCES     = pvbrowser.qrc
TARGET        = pvbrowser

# install
target.path   = /usr/local/bin
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS pvbrowser.pro images
sources.path  = /opt/pvb/pvbrowser
INSTALLS     += target sources
