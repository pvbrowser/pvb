TEMPLATE = lib
CONFIG  += warn_on debug
CONFIG  -= qt
CFLAGS   = -pipe -Wall -W -O0 -march=i586 -mcpu=i686 -fmessage-length=0 -DNO_DEBUG -fPIC
HEADERS  = rlbussignaldatabase.h
SOURCES  = rlbussignaldatabase.cpp
INCLUDEPATH += /usr/include/mysql
TARGET   = rllibsql

target.path = /usr/lib
target.files = librllibsql.*
INSTALLS = target

