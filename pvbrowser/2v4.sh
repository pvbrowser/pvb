#!/bin/bash
cp pvbrowser.v4.pro              pvbrowser.pro
cp dlgmybrowser.v4.cpp           dlgmybrowser.cpp 
cp dlgmybrowser.v4.h             dlgmybrowser.h 
cp dlgmybrowser.v4.ui            dlgmybrowser.ui
cp dlgmybrowser_ui.v4.h          dlgmybrowser_ui.h 
cp webkit_ui_dlgtextbrowser.v4.h webkit_ui_dlgtextbrowser.h 
cp dlgtextbrowser.v4.cpp         dlgtextbrowser.cpp 
cp pvglwidget.v4.h               pvglwidget.h
cp pvglwidget.v4.cpp             pvglwidget.cpp

qmake-qt5 pvbrowser.pro
make clean
make


