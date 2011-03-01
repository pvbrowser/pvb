#!/bin/bash

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
  echo testing if you have root privileges ...
  if [ $HOME != "/root" ]; then
    if [ $LOGNAME != "root" ]; then
    echo '##############################################'
    echo '# install pvbrowser with:                    #'
    echo '#   sudo bash                                #'
    echo '#   ./install.sh                             #'
    echo '#   exit                                     #'
    echo '##############################################'
    exit
    fi
  fi
fi

./uninstall.sh

echo 'Now installing new version of pvbrowser...'
mkdir -p /srv/automation/shm
mkdir -p /srv/automation/mbx
mkdir -p /srv/automation/log
chmod -R ugoa+rwx /srv/automation
mkdir -p  /opt/pvb
mkdir -p  /opt/pvb/doc
mkdir -p  /opt/pvb/pvsexample
mkdir -p  /opt/pvb/pvserver
mkdir -p  /opt/pvb/pvbrowser
mkdir -p  /opt/pvb/pvdevelop
mkdir -p  /opt/pvb/start_pvbapp
mkdir -p  /opt/pvb/designer/plugins
mkdir -p  /opt/pvb/rllib/lib
mkdir -p  /opt/pvb/rllib/rlsvg
mkdir -p  /opt/pvb/rllib/rlhistory
mkdir -p  /opt/pvb/language_bindings/lua/pvslua
mkdir -p  /opt/pvb/language_bindings/lua/pvapplua
mkdir -p  /opt/pvb/language_bindings/python/id
mkdir -p  /opt/pvb/language_bindings/python/mt
mkdir -p  /opt/pvb/language_bindings/python/template
mkdir -p  /opt/pvb/browserplugin

cp -r doc                                                 /opt/pvb/
cp -r pvsexample                                          /opt/pvb/
cp    LICENSE.GPL                                         /opt/pvb/
cp    logo1.xpm                                           /opt/pvb/
cp    custom.bmp                                          /opt/pvb/
cp    gamsleiten.png                                      /opt/pvb/
cp    pvbrowser.desktop                                   /opt/pvb/
cp    pvdevelop.desktop                                   /opt/pvb/
cp    pvserver/libpvsid.so.1.0.0                          /opt/pvb/pvserver/
cp    pvserver/libpvsmt.so.1.0.0                          /opt/pvb/pvserver/
cp    pvserver/wthread.h                                  /opt/pvb/pvserver/
cp    pvserver/BMP.h                                      /opt/pvb/pvserver/
cp    pvserver/vmsglext.h                                 /opt/pvb/pvserver/
cp    pvserver/pvbImage.h                                 /opt/pvb/pvserver/
cp    pvserver/vmsgl.h                                    /opt/pvb/pvserver/
cp    pvserver/processviewserver.h                        /opt/pvb/pvserver/
cp    pvbrowser/pvbrowser                                 /opt/pvb/pvbrowser/
cp    pvdevelop/pvdevelop                                 /opt/pvb/pvdevelop/
cp    pvdevelop/pvdevelop.sh                              /opt/pvb/pvdevelop/
cp    start_pvbapp/start_pvbapp                           /opt/pvb/start_pvbapp/
cp    start_pvbapp/README.txt                             /opt/pvb/start_pvbapp/
cp    start_pvbapp/example.ini.linux                      /opt/pvb/start_pvbapp/example.ini
cp    start_pvbapp/start_if_not_already_running.sh        /opt/pvb/start_pvbapp/
cp    rllib/rlsvg/rlsvgcat                                /opt/pvb/rllib/rlsvg/
cp    rllib/rlhistory/rlhistory                           /opt/pvb/rllib/rlhistory/
cp    rllib/lib/librllib.so.1.0.0                         /opt/pvb/rllib/lib/
cp    rllib/lib/*.h                                       /opt/pvb/rllib/lib/
cp    rllib/lib/nodave.o                                  /opt/pvb/rllib/lib/
cp    rllib/lib/setport.o                                 /opt/pvb/rllib/lib/
cp    language_bindings/lua/pvslua/pvslua                 /opt/pvb/language_bindings/lua/pvslua/
cp    language_bindings/lua/pvapplua/pvapplua             /opt/pvb/language_bindings/lua/pvapplua/
cp    language_bindings/python/id/pv.py                   /opt/pvb/language_bindings/python/id/
cp    language_bindings/python/id/_pv.so                  /opt/pvb/language_bindings/python/id/
cp    language_bindings/python/id/rllib.py                /opt/pvb/language_bindings/python/id/
cp    language_bindings/python/id/_rllib.so               /opt/pvb/language_bindings/python/id/
cp    language_bindings/README_PYTHON.txt                 /opt/pvb/language_bindings/
cp    language_bindings/python/mt/pv.py                   /opt/pvb/language_bindings/python/mt/
cp    language_bindings/python/mt/_pv.so                  /opt/pvb/language_bindings/python/mt/
cp    language_bindings/python/mt/rllib.py                /opt/pvb/language_bindings/python/mt/
cp    language_bindings/python/mt/_rllib.so               /opt/pvb/language_bindings/python/mt/
cp    language_bindings/python/template/main.cpp          /opt/pvb/language_bindings/python/template/
cp    language_bindings/python/template/Makefile          /opt/pvb/language_bindings/python/template/
cp    language_bindings/python/template/mask1.cpp         /opt/pvb/language_bindings/python/template/
cp    language_bindings/python/template/mask1.py          /opt/pvb/language_bindings/python/template/
cp    language_bindings/python/template/mask1_slots.h     /opt/pvb/language_bindings/python/template/
cp    language_bindings/python/template/pvapp.h           /opt/pvb/language_bindings/python/template/
cp    language_bindings/python/template/pvs_init.py       /opt/pvb/language_bindings/python/template/
cp    language_bindings/python/template/pvs.pro           /opt/pvb/language_bindings/python/template/
cp    language_bindings/python/template/pvs.pvproject     /opt/pvb/language_bindings/python/template/
cp    designer/plugins/libpvb_designer_plugin.so          /opt/pvb/designer/plugins/
cp    designer/plugins/libqwt_designer_plugin.so          /opt/pvb/designer/plugins/
cp    designer/README.txt                                 /opt/pvb/designer/
cp    browserplugin/libpvbrowser.so                       /opt/pvb/browserplugin/
cp    browserplugin/pvbrowserplugin-example.html          /opt/pvb/browserplugin/
cp    browserplugin/README.txt                            /opt/pvb/browserplugin/

if [ "$PVB_OSTYPE" == "linux" ]; then

echo 'set links on linux'
ln -sf /opt/pvb/pvserver/libpvsid.so   /usr/lib/libpvsid.so
ln -sf /opt/pvb/pvserver/libpvsid.so   /usr/lib/libpvsid.so.1
ln -sf /opt/pvb/pvserver/libpvsid.so   /usr/lib/libpvsid.so.1.0
ln -sf /opt/pvb/pvserver/libpvsid.so   /usr/lib/libpvsid.so.1.0.0

ln -sf /opt/pvb/pvserver/libpvsmt.so   /usr/lib/libpvsmt.so
ln -sf /opt/pvb/pvserver/libpvsmt.so   /usr/lib/libpvsmt.so.1
ln -sf /opt/pvb/pvserver/libpvsmt.so   /usr/lib/libpvsmt.so.1.0
ln -sf /opt/pvb/pvserver/libpvsmt.so   /usr/lib/libpvsmt.so.1.0.0

#ln -sf /opt/pvb/pvbrowser/pvbrowser                /usr/local/bin/pvbrowser_system
#ln -sf /opt/pvb/tested_qt/pvbrowser_tested.sh      /usr/local/bin/pvbrowser
#ln -sf /opt/pvb/pvdevelop/pvdevelop                /usr/local/bin/pvdevelop_system
#ln -sf /opt/pvb/tested_qt/pvdevelop_tested.sh      /usr/local/bin/pvdevelop
ln -sf /opt/pvb/pvbrowser/pvbrowser                 /usr/local/bin/pvbrowser
#ln -sf /opt/pvb/pvdevelop/pvdevelop                 /usr/local/bin/pvdevelop
cp /opt/pvb/pvdevelop/pvdevelop.sh                  /usr/local/bin/pvdevelop
ln -sf /opt/pvb/rllib/rlsvg/rlsvgcat                /usr/local/bin/rlsvgcat
ln -sf /opt/pvb/rllib/rlsvg/rlsvgcat                /usr/bin/rlsvgcat
ln -sf /opt/pvb/start_pvbapp/start_pvbapp           /usr/local/bin/start_pvbapp
ln -sf /opt/pvb/rllib/rlhistory/rlhistory           /usr/local/bin/rlhistory
ln -sf /opt/pvb/update_pvbrowser.sh                 /usr/local/bin/update_pvbrowser
ln -sf /opt/pvb/language_bindings/lua/pvslua/pvslua     /usr/local/bin/pvslua
ln -sf /opt/pvb/language_bindings/lua/pvapplua/pvapplua /usr/local/bin/pvapplua

ln -sf /opt/pvb/rllib/lib/librllib.so   /usr/lib/librllib.so
ln -sf /opt/pvb/rllib/lib/librllib.so   /usr/lib/librllib.so.1
ln -sf /opt/pvb/rllib/lib/librllib.so   /usr/lib/librllib.so.1.0
ln -sf /opt/pvb/rllib/lib/librllib.so   /usr/lib/librllib.so.1.0.0

echo 'Now running ldconfig...'
ldconfig

else 

echo 'set links on OS-X'

ln -sf /opt/pvb/pvbrowser/pvbrowser.app/Contents/MacOS/pvbrowser           /usr/bin/pvbrowser
ln -sf /opt/pvb/pvdevelop/pvdevelop.app/Contents/MacOS/pvdevelop           /usr/bin/pvdevelop
ln -sf /opt/pvb/start_pvbapp/start_pvbapp.app/Contents/MacOS/start_pvbapp  /usr/local/bin/start_pvbapp
ln -sf /opt/pvb/rllib/rlsvg/rlsvgcat.app/Contents/MacOS/rlsvgcat           /usr/bin/rlsvgcat
ln -sf /opt/pvb/rllib/rlhistory/rlhistory.app/Contents/MacOS/rlhistory     /usr/bin/rlhistory
ln -sf /opt/pvb/update_pvbrowser.sh                                        /usr/bin/update_pvbrowser

ln -sf /opt/pvb/rllib/lib/librllib.dylib  /usr/lib/librllib.dylib
ln -sf /opt/pvb/rllib/lib/librllib.dylib  /usr/lib/librllib.dylib.1
ln -sf /opt/pvb/rllib/lib/librllib.dylib  /usr/lib/librllib.dylib.1.0
ln -sf /opt/pvb/rllib/lib/librllib.dylib  /usr/lib/librllib.dylib.1.0.0

fi

echo 'Makeing /opt/pvb/pvsexample writeable...'
cd /opt/pvb
chmod ugoa+w pvsexample
cd /opt/pvb/pvsexample
chmod ugoa+w *

if [ "$PVB_OSTYPE" == "linux" ]; then

echo 'creating pvbrowserdemo...'
echo '#!/bin/bash'                                                                     > /usr/local/bin/pvbrowserdemo
echo '# run pvsexample'                                                               >> /usr/local/bin/pvbrowserdemo
echo 'xterm -e "/opt/pvb/pvsexample/pvsexample -cd=/opt/pvb/pvsexample -sleep=300" &' >> /usr/local/bin/pvbrowserdemo
echo 'pvbrowser &'                                                                    >> /usr/local/bin/pvbrowserdemo
chmod ugoa+x /usr/local/bin/pvbrowserdemo
#cp /opt/pvb/tested_qt/qt.conf /opt/pvb/pvbrowser/
#cp /opt/pvb/tested_qt/qt.conf /opt/pvb/pvdevelop/

echo 'begin copy Qt Designer plugins to destination'
export PLUGIN=$(find /usr/lib64/qt4/plugins/ -name designer)
if [ "x$PLUGIN" != "x" ]; then
echo copy plugins to $PLUGIN/
cp /opt/pvb/designer/plugins/* $PLUGIN/
fi
export PLUGIN=$(find /usr/lib/qt4/plugins/ -name designer)
if [ "x$PLUGIN" != "x" ]; then
echo copy plugins to $PLUGIN/
cp /opt/pvb/designer/plugins/* $PLUGIN/
fi
echo 'end copy Qt Designer plugins to destination'
fi

echo '################################################################'
echo '# pvbrowser is now installed !!!                               #'
echo '# Have a lot of fun                                            #'
echo '# commands:                                                    #'
echo '#   pvbrowserdemo                                              #'
echo '#   pvbrowser                                                  #'
echo '#   pvdevelop                                                  #'
echo '#   Hint: copy the following shortcuts to your desktop         #'
echo '#   /opt/pvb/pvbrowser.desktop /opt/pvb/pvdevelop.desktop      #'
echo '#                                                              #'
echo '# Example pvserver:                                            #'
echo '#   cd /opt/pvb/pvsexample                                     #'
echo '#   make clean                                                 #'
echo '#   make                                                       #'
echo '#   ./pvsexample -sleep=100                                    #'
echo '#   pvbrowser # in other terminal                              #'
echo '#                                                              #'
echo '# if you want to use Qt Designer for designing your masks,     #'
echo '#   copy the plugins to Qt Designer (read designer/README.txt) #'
echo '# Now type:                                                    #'
echo '#   exit                                                       #'
echo '################################################################'
