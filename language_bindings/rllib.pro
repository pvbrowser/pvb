TEMPLATE = lib
CONFIG  += console warn_on release dll
CONFIG  -= qt
INCLUDEPATH += $(PYDIR)/include

# Input
# HEADERS += 
SOURCES += language_binding_rllib_wrap_python.cxx 
SOURCES += ../rllib/lib/CIFUSER_DUMMY.cpp

LIBS += $(PYDIR)/libs/libpython25.a
LIBS += ../win-mingw/bin/librllib.a
LIBS += $(MINGWDIR)/lib/libws2_32.a 
LIBS += $(MINGWDIR)/lib/libadvapi32.a
LIBS += $(MINGWDIR)/lib/libuser32.a

TARGET = _rllib 
