#!/bin/bash
echo "##################################################################"
echo "# this shell script switches to version v4                       #"
echo "# that is: use Apple WebKit on Linux and Windows                 #"
echo "#          fall back to QTextBrowser on OS-X and mobile devices  #"
echo "# use this version on systems up to Qt 5.4 Version               #"
echo "##################################################################"
cp pvbrowser.v4.pro              pvbrowser.pro
cp dlgmybrowser.v4.cpp           dlgmybrowser.cpp 
cp dlgmybrowser.v4.h             dlgmybrowser.h 
cp dlgmybrowser.v4.ui            dlgmybrowser.ui
cp dlgmybrowser_ui.v4.h          dlgmybrowser_ui.h 
cp webkit_ui_dlgtextbrowser.v4.h webkit_ui_dlgtextbrowser.h 
cp dlgtextbrowser.v4.cpp         dlgtextbrowser.cpp 
cp pvglwidget.v4.h               pvglwidget.h
cp pvglwidget.v4.cpp             pvglwidget.cpp

../qmake.sh pvbrowser.pro
make clean

echo "Please adjust manually:"
echo "vi pvdevelop/pvdevelop.pro"     "QT+CONFIG webkit/webengine"
echo "vi designer/src/pvbplugin.pro"  "QT+CONFIG webkit/webengine"
#rem  #include <QDesignerCustomWidgetInterface>
#rem  #include <QtUiPlugin/QDesignerCustomWidgetInterface>
echo "vi qwt/designer/qwt_designer_plugin.h"    
echo "vi pvb/designer/src/pvb_designer_plugin.h"


