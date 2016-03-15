#!/bin/bash
#######################################
# uninstall pvbrowser                 #
#######################################

# detect real OS on different linux distries
export PVB_OSTYPE="unknown"
if [ "$OSTYPE" == "linux" ]; then
  export PVB_OSTYPE="linux"
fi
if [ "$OSTYPE" == "gnu-linux" ]; then
  export PVB_OSTYPE="linux"
fi
if [ "$OSTYPE" == "linux-gnu" ]; then
  export PVB_OSTYPE="linux"
fi

if [ "$PVB_OSTYPE" == "linux" ]; then

rm -f /usr/local/bin/pvbrowser_system
rm -f /usr/local/bin/pvbrowser
rm -f /usr/local/bin/pvdevelop_system
rm -f /usr/local/bin/pvdevelop
rm -f /usr/local/bin/start_pvbapp
rm -f /usr/local/bin/rlsvgcat
rm -f /usr/local/bin/rlfind
rm -f /usr/local/bin/rlhistory
rm -f /usr/local/bin/rlhtml2pdf
rm -f /usr/local/bin/fake_qmake
rm -f /usr/local/bin/pvbrowserdemo
rm -f /usr/local/bin/update_pvbrowser
rm -f /usr/local/bin/pvslua
rm -f /usr/local/bin/pvapplua

rm -f /usr/bin/pvbrowser_system
rm -f /usr/bin/pvbrowser
rm -f /usr/bin/pvdevelop_system
rm -f /usr/bin/pvdevelop
rm -f /usr/bin/start_pvbapp
rm -f /usr/bin/rlsvgcat
rm -f /usr/bin/rlfind
rm -f /usr/bin/rlhistory
rm -f /usr/bin/rlhtml2pdf
rm -f /usr/bin/fake_qmake
rm -f /usr/bin/pvbrowserdemo
rm -f /usr/bin/update_pvbrowser
rm -f /usr/bin/pvslua
rm -f /usr/bin/pvapplua

rm -f /usr/lib/libpvsid.so
rm -f /usr/lib/libpvsid.so.1
rm -f /usr/lib/libpvsid.so.1.0
rm -f /usr/lib/libpvsid.so.1.0.0
rm -f /usr/lib/libpvsmt.so
rm -f /usr/lib/libpvsmt.so.1
rm -f /usr/lib/libpvsmt.so.1.0
rm -f /usr/lib/libpvsmt.so.1.0.0
rm -f /usr/lib/librllib.so
rm -f /usr/lib/librllib.so.1
rm -f /usr/lib/librllib.so.1.0
rm -f /usr/lib/librllib.so.1.0.0

else

rm -f /usr/bin/pvbrowser
rm -f /usr/bin/pvdevelop
rm -f /usr/bin/start_pvbapp
rm -f /usr/bin/rlsvgcat
rm -f /usr/bin/rlfind
rm -f /usr/bin/rlhistory
rm -f /usr/bin/rlhtml2pdf
rm -f /usr/bin/pvbrowserdemo
rm -f /usr/bin/update_pvbrowser
rm -f /usr/bin/rlsvgcat

rm -f /usr/lib/libpvsid.dylib
rm -f /usr/lib/libpvsid.dylib.1
rm -f /usr/lib/libpvsid.dylib.1.0
rm -f /usr/lib/libpvsid.dylib.1.0.0
rm -f /usr/lib/libpvsmt.dylib
rm -f /usr/lib/libpvsmt.dylib.1
rm -f /usr/lib/libpvsmt.dylib.1.0
rm -f /usr/lib/libpvsmt.dylib.1.0.0
rm -f /usr/lib/librllib.dylib
rm -f /usr/lib/librllib.dylib.1
rm -f /usr/lib/librllib.dylib.1.0
rm -f /usr/lib/librllib.dylib.1.0.0

fi

rm -rf /opt/pvb
echo '######################################'
echo '# pvbrowser has now been uninstalled #'
echo '######################################'
