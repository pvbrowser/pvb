TEMPLATE = lib
#CONFIG  += warn_on release Hilscher qt
#CONFIG  += warn_on release staticlib
CONFIG   += warn_on release
CONFIG   -= qt
CXXFLAGS += -fPIC -fno-strict-aliasing
CFLAGS    = -pipe -Wall -W -O0 -fmessage-length=0 -DNO_DEBUG -fPIC -fno-strict-aliasing
# CFLAGS   = -pipe -Wall -W -O0 -march=i586 -mcpu=i686 -fmessage-length=0 -DNO_DEBUG -fPIC
# LITTLEENDIAN and LINUX is only for libnodave to work
DEFINES           += LITTLEENDIAN
unix:DEFINES      += LINUX
unix:QMAKE_LFLAGS += -pthread -lpthread
macx:DEFINES      += unix
macx:DEFINES      += PVMAC

HEADERS  = rllib.h                     \
           rlspawn.h                   \
           rlwthread.h                 \
           rlthread.h                  \
           rlsocket.h                  \
           rltime.h                    \
           rlmailbox.h                 \
           rlfifo.h                    \
           rlsharedmemory.h            \
           rlspreadsheet.h             \
           rlinifile.h                 \
           rlinterpreter.h             \
           rlpcontrol.h                \
           rlcutil.h                   \
           rldefine.h                  \
           rlevent.h                   \
           rleventlogserver.h          \
           rldataprovider.h            \
           rlserial.h                  \
           rlmodbus.h                  \
           rlmodbusclient.h            \
           rl3964r.h                   \
           rlsiemenstcp.h              \
           rlsiemenstcpclient.h        \
           rlcontroller.h              \
           rlppiclient.h               \
           rlsvganimator.h             \
           rlsvgcat.h                  \
           rlfileload.h                \
           rlhistorylogger.h           \
           rlhistoryreader.h           \
           rlhilschercif.h             \
           rludpsocket.h               \
           rleibnetip.h                \
           rlopcxmlda.h                \
           rldataacquisition.h         \
           rldataacquisitionprovider.h \
           rlstring.h                  \
           rlplc.h                     \
           rlwebcam.h                  \
           rlcommandlineinterface.h    \
           rlreport.h                  \
#           rlstatemachine.h            \
           rldf1.h                     \
           rlstate.h                   \
           rlbuffer.h                  \
           rlhtmldir.h                 \
           rlhtml.h                    \
           rlsvgvdi.h                  \
           rljson.h

SOURCES  = rlspawn.cpp                   \
           rlwthread.cpp                 \
           rlthread.cpp                  \
           rlsocket.cpp                  \
           rltime.cpp                    \
           rlmailbox.cpp                 \
           rlfifo.cpp                    \
           rlsharedmemory.cpp            \
           rlspreadsheet.cpp             \
           rlinifile.cpp                 \
           rlinterpreter.cpp             \
           rlpcontrol.cpp                \
           rlcutil.cpp                   \
           rlevent.cpp                   \
           rleventlogserver.cpp          \
           rldataprovider.cpp            \
           rlserial.cpp                  \
           rlmodbus.cpp                  \
           rlmodbusclient.cpp            \
           rl3964r.cpp                   \
           rlsiemenstcp.cpp              \
           rlsiemenstcpclient.cpp        \
           rlcontroller.cpp              \
           rlppiclient.cpp               \
           rlsvganimator.cpp             \
           rlsvgcat.cpp                  \
           rlfileload.cpp                \
           rlhistorylogger.cpp           \
           rlhistoryreader.cpp           \
           rlhilschercif.cpp             \
           rludpsocket.cpp               \
           rleibnetip.cpp                \
           rlopcxmlda.cpp                \
           rldataacquisition.cpp         \
           rldataacquisitionprovider.cpp \
           rlstring.cpp                  \
           rlplc.cpp                     \
           rlwebcam.cpp                  \
           rlcommandlineinterface.cpp    \
           rlreport.cpp                  \
#           rlstatemachine.cpp            \
           rldf1.cpp                     \
           rlstate.cpp                   \
           rlbuffer.cpp                  \
           rlhtmldir.cpp                 \
           rlhtml.cpp                    \
           rlsvgvdi.cpp                  \ 
           rljson.cpp                    \
           nodave.c                      \
           setport.c

macx:SOURCES -= rlhilschercif.cpp

Hilscher {
  HEADERS += rlcannode.h           \
             rlcanopendaemon.h     \
             rlcanopentypes.h      \
             rlcanopenstructs.h    \
             objdir.h              \
             rlcanopenclient.h     \
             rlcanopen.h
  SOURCES += rlcanopendaemon.cpp   \
             rlcanopentypes.cpp    \
             objdir.cpp            \
             rlcanopenclient.cpp   \
             rlcannode.cpp         \
             rlcanopen.cpp
  INCLUDEPATH = ../usr-inc
}

#unix:OBJECTS += ../foreign/abel/lib/libabplc5.a
#unix:OBJECTS += ../foreign/cell/lib/libcell.a
#unix:OBJECTS += ../foreign/libnodave/openSocket.o 
#unix:OBJECTS += ../foreign/libnodave/setport.o     
#unix:OBJECTS += ../foreign/libnodave/nodave.o    

TARGET   = rllib

#unix:header.path = /usr/local/include/rllib 
#unix:header.files = *.h ../foreign/abel/lib/*.h ../foreign/cell/lib/*.h ../foreign/libnodave/*.h 
#unix:header.extra = mkdir -p /usr/local/include/rllib
#unix:target.path = /usr/lib
#unix:target.files = librllib.*
#unix:INSTALLS = target header
