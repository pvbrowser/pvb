#!/bin/bash

# detect real OS on different linux distries
ostest=$(echo $OSTYPE | grep -i linux)
if [ "unknown${ostest}" == "unknown" ]; then
  export PVB_OSTYPE="unknown"
else
  export PVB_OSTYPE="linux"
fi
echo OSTYPE $PVB_OSTYPE

#export PVB_OSTYPE="unknown"
#if [ "$OSTYPE" == "linux" ]; then
#  export PVB_OSTYPE="linux"
#fi
#if [ "$OSTYPE" == "gnu-linux" ]; then
#  export PVB_OSTYPE="linux"
#fi
#if [ "$OSTYPE" == "linux-gnu" ]; then
#  export PVB_OSTYPE="linux"
#fi

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

echo 'installing new version of pvbrowser...'
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
mkdir -p  /opt/pvb/rllib/rlfind
mkdir -p  /opt/pvb/rllib/rlhistory
mkdir -p  /opt/pvb/rllib/rlhtml2pdf
mkdir -p  /opt/pvb/fake_qmake
mkdir -p  /opt/pvb/language_bindings/lua/pvslua
mkdir -p  /opt/pvb/language_bindings/lua/pvapplua
mkdir -p  /opt/pvb/language_bindings/python/id
mkdir -p  /opt/pvb/language_bindings/python/mt
mkdir -p  /opt/pvb/language_bindings/python/template
mkdir -p  /opt/pvb/browserplugin

echo 'copy documentation...'
cp -r doc                                                 /opt/pvb/
echo 'copy pvsexample...'
cp -r pvsexample                                          /opt/pvb/
echo 'copy other files...'
cp    LICENSE.GPL                                         /opt/pvb/
cp    logo1.png                                           /opt/pvb/
cp    custom.bmp                                          /opt/pvb/
cp    gamsleiten.png                                      /opt/pvb/
cp    pvbrowser.desktop                                   /opt/pvb/
cp    pvdevelop.desktop                                   /opt/pvb/
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
cp    rllib/rlfind/rlfind                                 /opt/pvb/rllib/rlfind/
cp    rllib/rlhistory/rlhistory                           /opt/pvb/rllib/rlhistory/
cp    rllib/rlhtml2pdf/rlhtml2pdf                         /opt/pvb/rllib/rlhtml2pdf/
cp    fake_qmake/fake_qmake                               /opt/pvb/fake_qmake/
cp    rllib/lib/*.h                                       /opt/pvb/rllib/lib/
cp    rllib/lib/nodave.o                                  /opt/pvb/rllib/lib/
cp    rllib/lib/setport.o                                 /opt/pvb/rllib/lib/
cp    language_bindings/lua/pvslua/pvslua                 /opt/pvb/language_bindings/lua/pvslua/
cp    language_bindings/lua/pvapplua/pvapplua             /opt/pvb/language_bindings/lua/pvapplua/
cp    language_bindings/README_PYTHON.txt                 /opt/pvb/language_bindings/
cp    language_bindings/python/template/main.cpp          /opt/pvb/language_bindings/python/template/
cp    language_bindings/python/template/Makefile          /opt/pvb/language_bindings/python/template/
cp    language_bindings/python/template/mask1.cpp         /opt/pvb/language_bindings/python/template/
cp    language_bindings/python/template/mask1.py          /opt/pvb/language_bindings/python/template/
cp    language_bindings/python/template/mask1_slots.h     /opt/pvb/language_bindings/python/template/
cp    language_bindings/python/template/pvapp.h           /opt/pvb/language_bindings/python/template/
cp    language_bindings/python/template/pvs_init.py       /opt/pvb/language_bindings/python/template/
cp    language_bindings/python/template/pvs.pro           /opt/pvb/language_bindings/python/template/
cp    language_bindings/python/template/pvs.pvproject     /opt/pvb/language_bindings/python/template/
cp    designer/README.txt                                 /opt/pvb/designer/
cp    browserplugin/pvbrowserplugin-example.html          /opt/pvb/browserplugin/
cp    browserplugin/README.txt                            /opt/pvb/browserplugin/
if [ "$PVB_OSTYPE" == "linux" ]; then
echo 'copy shared objects on linux...'
cp    browserplugin/libpvbrowser.so                       /opt/pvb/browserplugin/
cp    language_bindings/python/mt/pv.py                   /opt/pvb/language_bindings/python/mt/
cp    language_bindings/python/mt/_pv.so                  /opt/pvb/language_bindings/python/mt/
cp    language_bindings/python/mt/rllib.py                /opt/pvb/language_bindings/python/mt/
cp    language_bindings/python/mt/_rllib.so               /opt/pvb/language_bindings/python/mt/
cp    language_bindings/python/id/pv.py                   /opt/pvb/language_bindings/python/id/
cp    language_bindings/python/id/_pv.so                  /opt/pvb/language_bindings/python/id/
cp    language_bindings/python/id/rllib.py                /opt/pvb/language_bindings/python/id/
cp    language_bindings/python/id/_rllib.so               /opt/pvb/language_bindings/python/id/
cp    designer/plugins/libpvb_designer_plugin.so          /opt/pvb/designer/plugins/
cp    designer/plugins/libqwt_designer_plugin.so          /opt/pvb/designer/plugins/
cp    pvserver/libpvsid.so                                /opt/pvb/pvserver/
cp    pvserver/libpvsmt.so                                /opt/pvb/pvserver/
cp    pvserver/libpvsid.a                                 /opt/pvb/pvserver/
cp    pvserver/libpvsmt.a                                 /opt/pvb/pvserver/
cp    rllib/lib/librllib.so                               /opt/pvb/rllib/lib/
else
echo 'copy shared objects on OS-X ...'
cp    designer/plugins/libpvb_designer_plugin.dylib       /opt/pvb/designer/plugins/
cp    designer/plugins/libqwt_designer_plugin.dylib       /opt/pvb/designer/plugins/
cp    pvserver/libpvsid.dylib                             /opt/pvb/pvserver/
cp    pvserver/libpvsmt.dylib                             /opt/pvb/pvserver/
cp    pvserver/libpvsid.a                                 /opt/pvb/pvserver/
cp    pvserver/libpvsmt.a                                /opt/pvb/pvserver/
cp    rllib/lib/librllib.dylib                            /opt/pvb/rllib/lib/
fi


if [ "$PVB_OSTYPE" == "linux" ]; then
echo 'set links on linux...'
ln -sf /opt/pvb/pvbrowser/pvbrowser                     /usr/bin/pvbrowser
cp /opt/pvb/pvdevelop/pvdevelop.sh                      /usr/bin/pvdevelop
ln -sf /opt/pvb/rllib/rlsvg/rlsvgcat                    /usr/bin/rlsvgcat
ln -sf /opt/pvb/rllib/rlfind/rlfind                     /usr/bin/rlfind
ln -sf /opt/pvb/start_pvbapp/start_pvbapp               /usr/bin/start_pvbapp
ln -sf /opt/pvb/rllib/rlhistory/rlhistory               /usr/bin/rlhistory
ln -sf /opt/pvb/rllib/rlhtml2pdf/rlhtml2pdf             /usr/bin/rlhtml2pdf
ln -sf /opt/pvb/fake_qmake/fake_qmake                   /usr/bin/fake_qmake
ln -sf /opt/pvb/update_pvbrowser.sh                     /usr/bin/update_pvbrowser
ln -sf /opt/pvb/language_bindings/lua/pvslua/pvslua     /usr/bin/pvslua
ln -sf /opt/pvb/language_bindings/lua/pvapplua/pvapplua /usr/bin/pvapplua

ln -sf /opt/pvb/rllib/lib/librllib.so         /usr/lib/librllib.so
ln -sf /opt/pvb/rllib/lib/librllib.so         /usr/lib/librllib.so.1
ln -sf /opt/pvb/rllib/lib/librllib.so         /usr/lib/librllib.so.1.0
ln -sf /opt/pvb/rllib/lib/librllib.so         /usr/lib/librllib.so.1.0.0

ln -sf /opt/pvb/pvserver/libpvsid.so          /usr/lib/libpvsid.so
ln -sf /opt/pvb/pvserver/libpvsid.so          /usr/lib/libpvsid.so.1
ln -sf /opt/pvb/pvserver/libpvsid.so          /usr/lib/libpvsid.so.1.0
ln -sf /opt/pvb/pvserver/libpvsid.so          /usr/lib/libpvsid.so.1.0.0

ln -sf /opt/pvb/pvserver/libpvsmt.so          /usr/lib/libpvsmt.so
ln -sf /opt/pvb/pvserver/libpvsmt.so          /usr/lib/libpvsmt.so.1
ln -sf /opt/pvb/pvserver/libpvsmt.so          /usr/lib/libpvsmt.so.1.0
ln -sf /opt/pvb/pvserver/libpvsmt.so          /usr/lib/libpvsmt.so.1.0.0
echo 'running ldconfig...'
ldconfig

else 
echo 'set links on OS-X ...'
cp -r  pvbrowser/pvbrowser.app                     /opt/pvb/pvbrowser/
cp -r  pvdevelop/pvdevelop.app                     /opt/pvb/pvdevelop/
cp -r  start_pvbapp/start_pvbapp.app               /opt/pvb/start_pvbapp/
cp -r  rllib/rlsvg/rlsvgcat.app                    /opt/pvb/rllib/rlsvg/
cp -r  rllib/rlfind/rlfind.app                     /opt/pvb/rllib/rlfind/
cp -r  rllib/rlhistory/rlhistory                   /opt/pvb/rllib/rlhistory/
cp -r  rllib/rlhtml2pdf/rlhtml2pdf                 /opt/pvb/rllib/rlhtml2pdf/
cp -r  fake_qmake/fake_qmake.app                   /opt/pvb/fake_qmake/
cp -r  language_bindings/lua/pvslua/pvslua.app     /opt/pvb/pvslua/
cp -r  language_bindings/lua/pvapplua/pvapplua.app /opt/pvb/pvapplua/

ln -sf /opt/pvb/pvbrowser/pvbrowser.app/Contents/MacOS/pvbrowser                     /usr/bin/pvbrowser
ln -sf /opt/pvb/pvdevelop/pvdevelop.app/Contents/MacOS/pvdevelop                     /usr/bin/pvdevelop
ln -sf /opt/pvb/start_pvbapp/start_pvbapp.app/Contents/MacOS/start_pvbapp            /usr/bin/start_pvbapp
ln -sf /opt/pvb/rllib/rlsvg/rlsvgcat.app/Contents/MacOS/rlsvgcat                     /usr/bin/rlsvgcat
ln -sf /opt/pvb/rllib/rlfind/rlfind.app/Contents/MacOS/rlfind                        /usr/bin/rlfind
ln -sf /opt/pvb/rllib/rlhistory/rlhistory                                            /usr/bin/rlhistory
ln -sf /opt/pvb/rllib/rlhtml2pdf/rlhtml2pdf                                          /usr/bin/rlhtml2pdf
ln -sf /opt/pvb/fake_qmake/fake_qmake.app/Contents/MacOS/fake_qmake                  /usr/bin/fake_qmake
ln -sf /opt/pvb/update_pvbrowser.sh                                                  /usr/bin/update_pvbrowser
ln -sf /opt/pvb/pvslua/Contents/MacOS/pvslua      /usr/bin/pvslua
ln -sf /opt/pvb/pvapplua/Contents/MacOS/pvapplua  /usr/bin/pvapplua

ln -sf /opt/pvb/rllib/lib/librllib.dylib    /usr/lib/librllib.dylib
ln -sf /opt/pvb/rllib/lib/librllib.dylib    /usr/lib/librllib.1.dylib
ln -sf /opt/pvb/rllib/lib/librllib.dylib    /usr/lib/librllib.1.0.dylib
ln -sf /opt/pvb/rllib/lib/librllib.dylib    /usr/lib/librllib.1.0.0.dylib

ln -sf /opt/pvb/pvb/pvserver/libpvsid.dylib /usr/lib/libpvsid.dylib
ln -sf /opt/pvb/pvb/pvserver/libpvsid.dylib /usr/lib/libpvsid.1.dylib
ln -sf /opt/pvb/pvb/pvserver/libpvsid.dylib /usr/lib/libpvsid.1.0.dylib
ln -sf /opt/pvb/pvb/pvserver/libpvsid.dylib /usr/lib/libpvsid.1.0.0.dylib

ln -sf /opt/pvb/pvb/pvserver/libpvsmt.dylib /usr/lib/libpvsmt.dylib
ln -sf /opt/pvb/pvb/pvserver/libpvsmt.dylib /usr/lib/libpvsmt.1.dylib
ln -sf /opt/pvb/pvb/pvserver/libpvsmt.dylib /usr/lib/libpvsmt.1.0.dylib
ln -sf /opt/pvb/pvb/pvserver/libpvsmt.dylib /usr/lib/libpvsmt.1.0.0.dylib
fi

echo 'makeing /opt/pvb/pvsexample writeable...'
cd /opt/pvb
chmod ugoa+w pvsexample
cd /opt/pvb/pvsexample
chmod ugoa+w *

if [ "$PVB_OSTYPE" == "linux" ]; then
echo 'creating pvbrowserdemo...'
echo '#!/bin/bash'                                                                     > /usr/bin/pvbrowserdemo
echo '# run pvsexample'                                                               >> /usr/bin/pvbrowserdemo
echo 'xterm -e "/opt/pvb/pvsexample/pvsexample -cd=/opt/pvb/pvsexample -sleep=300" &' >> /usr/bin/pvbrowserdemo
echo 'pvbrowser &'                                                                    >> /usr/bin/pvbrowserdemo
chmod ugoa+x /usr/bin/pvbrowserdemo

# copy qt designer plugins to destination
if [ -d /usr/lib64/qt4/plugins ]; then
  export PLUGIN=$(find /usr/lib64/qt4/plugins/ -name designer)
  if [ "x$PLUGIN" != "x" ]; then
  echo copy qt designer plugins to $PLUGIN/ ...
  cp /opt/pvb/designer/plugins/* $PLUGIN/
  fi
fi  
if [ -d /usr/lib/qt4/plugins ]; then
  export PLUGIN=$(find /usr/lib/qt4/plugins/ -name designer)
  if [ "x$PLUGIN" != "x" ]; then
  echo copy qt designer plugins to $PLUGIN/ ...
  cp /opt/pvb/designer/plugins/* $PLUGIN/
  fi
fi  
fi

echo "################################################################"
echo "#             _                                                #"                   
echo "#  _ ____   _| |__  _ __ _____      _____  ___ _ __            #"     
echo "# | '_ \ \ / / '_ \| '__/ _ \ \ /\ / / __|/ _ \ '__|           #"
echo "# | |_) \ V /| |_) | | | (_) \ V  V /\__ \  __/ |              #"    
echo "# | .__/ \_/ |_.__/|_|  \___/ \_/\_/ |___/\___|_|              #"     
echo "# |_|                                                          #"
echo "#                                         is now installed !!! #"
echo "# Have a lot of fun                                            #"
echo "# commands:                                                    #"
echo "#   pvbrowserdemo         - a small demo                       #"
echo "#   pvbrowser             - pvbrowser client                   #"
echo "#   pvdevelop             - integrated development environment #"
echo "#   pvslua                - the lua pvserver                   #"
echo "#   Hint: copy the following shortcuts to your desktop         #"
echo "#   /opt/pvb/pvbrowser.desktop /opt/pvb/pvdevelop.desktop      #"
echo "#                                                              #"
echo "# Example pvserver:                                            #"
echo "#   cd /opt/pvb/pvsexample                                     #"
echo "#   qmake pvsexample.pro                                       #"
echo "#   make clean                                                 #"
echo "#   make                                                       #"
echo "#   ./pvsexample -sleep=100                                    #"
echo "#   pvbrowser # in other terminal                              #"
echo "#                                                              #"
echo "# if you want to use Qt Designer for designing your masks,     #"
echo "#   copy the plugins to Qt Designer (read designer/README.txt) #"
echo "# Now type:                                                    #"
echo "#   exit                                                       #"
echo "################################################################"
