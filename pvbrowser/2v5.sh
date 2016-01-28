#!/bin/bash
cp pvbrowser.v5.pro              pvbrowser.pro
cp dlgmybrowser.v5.cpp           dlgmybrowser.cpp 
cp dlgmybrowser.v5.h             dlgmybrowser.h 
cp dlgmybrowser.v5.ui            dlgmybrowser.ui
cp dlgmybrowser_ui.v5.h          dlgmybrowser_ui.h 
cp webkit_ui_dlgtextbrowser.v5.h webkit_ui_dlgtextbrowser.h 
cp dlgtextbrowser.v5.cpp         dlgtextbrowser.cpp 
cp pvglwidget.v5.h               pvglwidget.h
cp pvglwidget.v5.cpp             pvglwidget.cpp

qmake-qt5 pvbrowser.pro
make clean
make

