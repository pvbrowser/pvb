#!/bin/bash
echo "##################################################################"
echo "# this shell script switches to version v5                       #"
echo "# that is: use Google Blink on Linux and Windows                 #"
echo "#          fall back to QTextBrowser on OS-X and mobile devices  #"
echo "# use this version on systems with Qt Version bigger than Qt 5.4 #"
echo "##################################################################"
cp pvbrowser.v5.pro              pvbrowser.pro
cp dlgmybrowser.v5.cpp           dlgmybrowser.cpp 
cp dlgmybrowser.v5.h             dlgmybrowser.h 
cp dlgmybrowser.v5.ui            dlgmybrowser.ui
cp dlgmybrowser_ui.v5.h          dlgmybrowser_ui.h 
cp webkit_ui_dlgtextbrowser.v5.h webkit_ui_dlgtextbrowser.h 
cp dlgtextbrowser.v5.cpp         dlgtextbrowser.cpp 
cp pvglwidget.v5.h               pvglwidget.h
cp pvglwidget.v5.cpp             pvglwidget.cpp

../qmake.sh pvbrowser.pro
make clean

echo "Please adjust manually:"
echo "vi pvdevelop/pvdevelop.pro"
echo "vi designer/src/pvbplugin.pro"

