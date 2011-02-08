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
mkdir -p /opt/pvb
cp -r . /opt/pvb/

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
ln -sf /opt/pvb/pvdevelop/pvdevelop                 /usr/local/bin/pvdevelop
ln -sf /opt/pvb/rllib/rlsvg/rlsvgcat                /usr/local/bin/rlsvgcat
ln -sf /opt/pvb/rllib/rlsvg/rlsvgcat                /usr/bin/rlsvgcat
ln -sf /opt/pvb/start_pvbapp/start_pvbapp           /usr/local/bin/start_pvbapp
ln -sf /opt/pvb/rllib/rlhistory/rlhistory           /usr/local/bin/rlhistory
ln -sf /opt/pvb/update_pvbrowser.sh                 /usr/local/bin/update_pvbrowser
ln -sf /opt/pvb/language_bindings/lua/pvslua/pvslua /usr/local/bin/pvslua

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

echo 'Deleting not needed files...'
cd /opt/pvb
rm -rf win
rm -rf qwt
rm -rf inc
rm -f  install* build* clean* *.spec edit porting* todo* vms* *.bat
export notNeeded='*~ *.cpp *.h *.pro *.ui *.o *.exe *.obj *.bat *.sh *.xpm *.qrc *.txt PVT* Makefile edit'
cd /opt/pvb/pvbrowser
rm -rf images
rm -f $notNeeded
cd /opt/pvb/browserplugin
rm -rf images
rm -f $notNeeded
cd /opt/pvb/pvdevelop
rm -rf images
rm -f $notNeeded
cd /opt/pvb/pvserver
rm -f *~ *.o *.exe *.obj PVT*
cd /opt/pvb/pvsexample
rm -f *~ *.o *.exe *.obj PVT*
cd /opt/pvb/rllib/lib
rm -f *~ *.cpp rl*.o *.pro Makefile
cd /opt/pvb/rllib/rlsvg
rm -f *~ *.cpp *.o *.pro Makefile
cd /opt/pvb/rllib
rm -rf foreign
cd /opt/pvb/designer
rm -rf src
cd /opt/pvb/language_bindings
rm -f *.cxx *.o *.i *.py *.cpp *.sh *.txt Makefile
cd /opt/pvb
rm -rf win-mingw
rm -f Readme_for_*.txt

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
