#################################################
# plugin for pvbrowser widgets within Qt Designer
#################################################
TEMPLATE        = lib
TARGET          = pvbdummy 
CONFIG         += release staticlib
QT             += widgets

HEADERS        += pvbdummy.h
SOURCES        += pvbdummy.cpp

RESOURCES       = pvb_designer_plugin.qrc 
