#######################################
# project file for start_pvbapp       #
#######################################
SOURCES              = main.cpp
CONFIG              += warn_on release
CONFIG              -= qt


!macx {
unix::LIBS          += ../rllib/lib/librllib.so -lpthread
unix::INCLUDEPATH   += ../rllib/lib
}

macx::LIBS          += ../rllib/lib/librllib.dylib 
macx::INCLUDEPATH   += ../rllib/lib

win32-g++ {
win32:LIBS        += ../win-mingw/bin/librllib.a 
win32:LIBS        += -lws2_32
win32:INCLUDEPATH += ../rllib/lib
}
else {
win32::LIBS         += ../win/bin/rllib.lib wsock32.lib imm32.lib advapi32.lib
win32::INCLUDEPATH  += ../rllib/lib
}

TARGET               = start_pvbapp
