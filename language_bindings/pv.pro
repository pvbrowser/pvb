TEMPLATE    = lib
CONFIG      = qt console warn_on release dll
QT          += sql
INCLUDEPATH += $(PYDIR)/include
INCLUDEPATH += ../pvserver

# Input
HEADERS += sql/qtdatabase.h
SOURCES += language_binding_wrap_python.cxx
SOURCES += pvmain.cpp 
SOURCES += sql/qtdatabase.cpp

LIBS        += $(PYDIR)/libs/libpython25.a
LIBS        += ../win-mingw/bin/libserverlib.a
LIBS        += $(MINGWDIR)/lib/libws2_32.a
LIBS        += $(MINGWDIR)/lib/libadvapi32.a
LIBS        += $(MINGWDIR)/lib/libuser32.a

TARGET = _pv 
