TARGET       = pvbrowser

lessThan(QT_MAJOR_VERSION, 5) {
  QT         += xml svg webkit network opengl
}else{
  QT         += printsupport multimedia uitools webkitwidgets widgets xml svg webkit network opengl printsupport
}    
INCLUDEPATH += ../pvbrowser
#INCLUDEPATH += ../qwt/include
INCLUDEPATH += ../qwt/src
DEFINES     += BROWSERPLUGIN
DEFINES     += NO_WEBKIT

HEADERS      = ../pvbrowser/mainwindow.h \
               ../pvbrowser/dlgopt.h \
               ../pvbrowser/opt.h \
               ../pvbrowser/tcputil.h \
               ../pvbrowser/interpreter.h \
               ../pvbrowser/pvserver.h \
               ../pvbrowser/MyWidgets.h \
               ../pvbrowser/MyTextBrowser_v4.h \
               ../pvbrowser/qimagewidget.h \
               ../pvbrowser/qdrawwidget.h \
               ../pvbrowser/pvglwidget.v4.h \
               ../pvbrowser/qwtwidgets.h \
               ../pvbrowser/qwtplotwidget.h \
               ../pvbrowser/dlgtextbrowser.h \
               ../pvbrowser/webkit_ui_dlgtextbrowser.h \
               ../pvbrowser/dlgmybrowser.h \
               ../pvbrowser/pvdefine.h

SOURCES      = pvpluginmain.cpp \
               ../pvbrowser/main.cpp \
               ../pvbrowser/mainwindow.cpp \
               ../pvbrowser/dlgopt.cpp \
               ../pvbrowser/opt.cpp \
               ../pvbrowser/tcputil.cpp \
               ../pvbrowser/interpreter.cpp \
               ../pvbrowser/MyWidgets.cpp \
               ../pvbrowser/MyTextBrowser_v4.cpp \
               ../pvbrowser/QDrawWidget.cpp \
               ../pvbrowser/QImageWidget.cpp \
               ../pvbrowser/pvglwidget.v4.cpp \
               ../pvbrowser/gldecode.cpp \
               ../pvbrowser/qwtplotwidget.cpp \
               ../pvbrowser/dlgtextbrowser.cpp \
               ../pvbrowser/dlgmybrowser.cpp

# FORMS       += ../pvbrowser/dlgtextbrowser.ui

LIBS        += ../qwt/lib/libqwt.a
LIBS        += -ldl

RESOURCES    = pvbrowser.qrc

win32 {
   RC_FILE    = pvpluginmain.rc
} else:mac {
   QMAKE_INFO_PLIST = Info.plist
   REZ_FILES += pvpluginmain.r
   rsrc_files.files = pvpluginmain.rsrc
   rsrc_files.path = Contents/Resources
   QMAKE_BUNDLE_DATA += rsrc_files
   DEFINES += PVUNIX
   DEFINES += PVMAC
}

include(qtbrowserplugin.pri)
