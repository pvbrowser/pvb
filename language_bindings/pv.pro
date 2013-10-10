TEMPLATE    = lib
CONFIG      += qt console warn_on release dll
QT          += sql
INCLUDEPATH += $(PYDIR)/include
INCLUDEPATH += ../pvserver
DEFINES     += COMPILE_FOR_SCRIPT_LANGUAGE
DEFINES     += "WINVER=0x0501"

# Input
HEADERS += sql/qtdatabase.h
SOURCES += language_binding_wrap_python.cxx
SOURCES += pvmain.cpp 
SOURCES += sql/qtdatabase.cpp

LIBS        += $(PYDIR)/libs/libpython25.a
LIBS        += ../win-mingw/bin/libserverlib.a
LIBS        += -lws2_32
LIBS        += -ladvapi32
LIBS        += -luser32

TARGET = _pv 
