#######################################
# project file for start_pvbapp       #
#######################################
SOURCES              = main.cpp
CONFIG               = warn_on release
CONFIG              -= qt


!macx {
unix::LIBS          += ../rllib/lib/librllib.so -pthread
unix::INCLUDEPATH   += ../rllib/lib
}

macx::LIBS          += ../rllib/lib/librllib.dylib 
macx::INCLUDEPATH   += ../rllib/lib

win32-g++ {
win32:LIBS        += $(PVBDIR)/win-mingw/bin/librllib.a $(MINGWDIR)/lib/libwsock32.a
win32:INCLUDEPATH += $(PVBDIR)/rllib/lib
}
else {
win32::LIBS         += ../win/bin/rllib.lib wsock32.lib imm32.lib advapi32.lib
win32::INCLUDEPATH  += ../rllib/lib
}

TARGET               = start_pvbapp
