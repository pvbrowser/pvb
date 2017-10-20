#################################################
# plugin for pvbrowser widgets within Qt Designer
#################################################
TEMPLATE         = lib
INCLUDEPATH     += ../pvbrowser 
TARGET           = pvb_designer_plugin
DESTDIR          = ../plugins
lessThan(QT_VERSION, 5.5.0) {
  QT            += designer widgets webkitwidgets
  CONFIG        += plugin release
}else{
  QT            += designer widgets webenginewidgets
  CONFIG        += plugin release
}
target.path      = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS        += target
unix:LIBS       += libpvbdummy.a
win32-msvc:LIBS += release/pvbdummy.lib
win32-g++:LIBS  += release/libpvbdummy.a

HEADERS         += pvb_designer_plugin.h
SOURCES         += pvb_designer_plugin.cpp
RESOURCES       += pvb_designer_plugin.qrc

