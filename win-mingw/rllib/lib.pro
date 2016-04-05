TEMPLATE  = lib
#CONFIG  += warn_on release Hilscher qt
CONFIG   += warn_on release staticlib
#CONFIG  += warn_on release
CONFIG   -= qt
DEFINES  -= UNICODE
DEFINES  += "WINVER=0x0501"

win32-g++ {
}
else {
DEFINES  += IS_OLD_MSVCPP
}

HEADERS  = ../../rllib/lib/rlwthread.h                \
           ../../rllib/lib/rlspawn.h                  \
           ../../rllib/lib/rlthread.h                 \
           ../../rllib/lib/rlsocket.h                 \
           ../../rllib/lib/rltime.h                   \
           ../../rllib/lib/rlmailbox.h                \
           ../../rllib/lib/rlfifo.h                   \
           ../../rllib/lib/rlsharedmemory.h           \
           ../../rllib/lib/rlspreadsheet.h            \
           ../../rllib/lib/rlinifile.h                \
           ../../rllib/lib/rlinterpreter.h            \
           ../../rllib/lib/rlpcontrol.h               \
           ../../rllib/lib/rlcutil.h                  \
           ../../rllib/lib/rldefine.h                 \
           ../../rllib/lib/rlevent.h                  \
           ../../rllib/lib/rleventlogserver.h         \
           ../../rllib/lib/rldataprovider.h           \
           ../../rllib/lib/rlserial.h                 \
           ../../rllib/lib/rlmodbus.h                 \
           ../../rllib/lib/rlmodbusclient.h           \
           ../../rllib/lib/rl3964r.h                  \
           ../../rllib/lib/rlsiemenstcp.h             \
           ../../rllib/lib/rlsiemenstcpclient.h       \
           ../../rllib/lib/rlcontroller.h             \
           ../../rllib/lib/rlppiclient.h              \
           ../../rllib/lib/rlsvganimator.h            \
           ../../rllib/lib/rlsvgcat.h                 \
           ../../rllib/lib/rlfileload.h               \
           ../../rllib/lib/rlhistorylogger.h          \
           ../../rllib/lib/rlhistoryreader.h          \
           ../../rllib/lib/rlhilschercif.h            \
           ../../rllib/lib/rludpsocket.h              \
           ../../rllib/lib/rleibnetip.h               \
           ../../rllib/lib/rlopcxmlda.h               \
           ../../rllib/lib/rldataacquisition.h         \
           ../../rllib/lib/rldataacquisitionprovider.h \
           ../../rllib/lib/rlstring.h                 \
           ../../rllib/lib/rlplc.h                    \
           ../../rllib/lib/rlwebcam.h                 \
           ../../rllib/lib/rlcommandlineinterface.h   \
           ../../rllib/lib/rlreport.h                 \
           ../../rllib/lib/rldf1.h                    \
           ../../rllib/lib/rlstate.h                  \
           ../../rllib/lib/rlbuffer.h                 \
           ../../rllib/lib/rlhtmldir.h                \ 
           ../../rllib/lib/rlhtml.h                   \
           ../../rllib/lib/rlsvgvdi.h
#           ../../rllib/lib/rlstatemachine.h           \

SOURCES  = ../../rllib/lib/rlwthread.cpp                \
           ../../rllib/lib/rlspawn.cpp                  \
           ../../rllib/lib/rlthread.cpp                 \
           ../../rllib/lib/rlsocket.cpp                 \
           ../../rllib/lib/rltime.cpp                   \
           ../../rllib/lib/rlmailbox.cpp                \
           ../../rllib/lib/rlfifo.cpp                   \
           ../../rllib/lib/rlsharedmemory.cpp           \
           ../../rllib/lib/rlspreadsheet.cpp            \
           ../../rllib/lib/rlinifile.cpp                \
           ../../rllib/lib/rlinterpreter.cpp            \
           ../../rllib/lib/rlpcontrol.cpp               \
           ../../rllib/lib/rlcutil.cpp                  \
           ../../rllib/lib/rlevent.cpp                  \
           ../../rllib/lib/rleventlogserver.cpp         \
           ../../rllib/lib/rldataprovider.cpp           \
           ../../rllib/lib/rlserial.cpp                 \
           ../../rllib/lib/rlmodbus.cpp                 \
           ../../rllib/lib/rlmodbusclient.cpp           \
           ../../rllib/lib/rl3964r.cpp                  \
           ../../rllib/lib/rlsiemenstcp.cpp             \
           ../../rllib/lib/rlsiemenstcpclient.cpp       \
           ../../rllib/lib/rlcontroller.cpp             \
           ../../rllib/lib/rlppiclient.cpp              \
           ../../rllib/lib/rlsvganimator.cpp            \
           ../../rllib/lib/rlsvgcat.cpp                 \
           ../../rllib/lib/rlfileload.cpp               \
           ../../rllib/lib/rlhistorylogger.cpp          \
           ../../rllib/lib/rlhistoryreader.cpp          \
           ../../rllib/lib/rlhilschercif.cpp            \
           ../../rllib/lib/rludpsocket.cpp              \
           ../../rllib/lib/rleibnetip.cpp               \
           ../../rllib/lib/rlopcxmlda.cpp               \
           ../../rllib/lib/rldataacquisition.cpp         \
           ../../rllib/lib/rldataacquisitionprovider.cpp \
           ../../rllib/lib/rlstring.cpp                 \
           ../../rllib/lib/rlplc.cpp                    \
           ../../rllib/lib/rlwebcam.cpp                 \
           ../../rllib/lib/rlcommandlineinterface.cpp   \
           ../../rllib/lib/rlreport.cpp                 \
           ../../rllib/lib/rldf1.cpp                    \
           ../../rllib/lib/rlstate.cpp                  \
           ../../rllib/lib/rlbuffer.cpp                 \
           ../../rllib/lib/rlhtmldir.cpp                \
           ../../rllib/lib/rlhtml.cpp                   \
           ../../rllib/lib/rlsvgvdi.cpp
#           ../../rllib/lib/rlstatemachine.cpp           \


Hilscher {
  HEADERS += ../../rllib/lib/rlcannode.h           \
             ../../rllib/lib/rlcanopendaemon.h     \
             ../../rllib/lib/rlcanopentypes.h      \
             ../../rllib/lib/rlcanopenstructs.h    \
             ../../rllib/objdir.h                  \
             ../../rllib/lib/rlcanopenclient.h     \
             ../../rllib/lib/rlcanopen.h
  SOURCES += ../../rllib/lib/rlcanopendaemon.cpp   \
             ../../rllib/lib/rlcanopentypes.cpp    \
             ../../rllib/objdir.cpp                \
             ../../rllib/lib/rlcanopenclient.cpp   \
             ../../rllib/lib/rlcannode.cpp         \
             ../../rllib/lib/rlcanopen.cpp
  INCLUDEPATH = ../usr-inc
}

#unix:OBJECTS += ../foreign/abel/lib/libabplc5.a
#unix:OBJECTS += ../foreign/cell/lib/libcell.a
#unix:OBJECTS += ../foreign/libnodave/openSocket.o 
#unix:OBJECTS += ../foreign/libnodave/setport.o
#OBJECTS +=  ../../rllib/foreign/libnodave/win/libnodave.lib

TARGET   = rllib

unix:header.path = /usr/local/include/rllib 
unix:header.files = *.h ../foreign/abel/lib/*.h ../foreign/cell/lib/*.h ../foreign/libnodave/*.h 
unix:header.extra = mkdir -p /usr/local/include/rllib
unix:target.path = /usr/lib
unix:target.files = librllib.*
unix:INSTALLS = target header
