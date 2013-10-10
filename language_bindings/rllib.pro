TEMPLATE = lib
CONFIG  += console warn_on release dll
CONFIG  -= qt
INCLUDEPATH += $(PYDIR)/include
DEFINES += "WINVER=0x0501"

# Input
# HEADERS += 
SOURCES += language_binding_rllib_wrap_python.cxx 
SOURCES += ../rllib/lib/CIFUSER_DUMMY.cpp

LIBS += $(PYDIR)/libs/libpython25.a
LIBS += ../win-mingw/bin/librllib.a
LIBS += -lws2_32 
LIBS += -ladvapi32
LIBS += -luser32

TARGET = _rllib 
