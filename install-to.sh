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

# dec 27 2017
#if [ "$PVB_OSTYPE" == "linux" ]; then
#  echo testing if you have root privileges ...
#  if [ $HOME != "/root" ]; then
#    if [ $LOGNAME != "root" ]; then
#    echo '##############################################'
#    echo '# install pvbrowser with:                    #'
#    echo '#   sudo bash                                #'
#    echo '#   ./install.sh                             #'
#    echo '#   exit                                     #'
#    echo '##############################################'
#    exit
#    fi
#  fi
#fi
#
#./uninstall.sh

echo 'installing new version of pvbrowser...'
#mkdir -p /srv/automation/shm
#mkdir -p /srv/automation/mbx
#mkdir -p /srv/automation/log
#chmod -R ugoa+rwx /srv/automation
mkdir -p  ${1}/pvb
mkdir -p  ${1}/pvb/doc
mkdir -p  ${1}/pvb/pvsexample
mkdir -p  ${1}/pvb/pvserver
mkdir -p  ${1}/pvb/pvbrowser
mkdir -p  ${1}/pvb/pvdevelop
mkdir -p  ${1}/pvb/start_pvbapp
mkdir -p  ${1}/pvb/designer/plugins
mkdir -p  ${1}/pvb/rllib/lib
mkdir -p  ${1}/pvb/rllib/rlsvg
mkdir -p  ${1}/pvb/rllib/rlfind
mkdir -p  ${1}/pvb/rllib/rlhistory
mkdir -p  ${1}/pvb/rllib/rlhtml2pdf
mkdir -p  ${1}/pvb/fake_qmake
mkdir -p  ${1}/pvb/language_bindings/lua/pvslua
mkdir -p  ${1}/pvb/language_bindings/lua/pvapplua
mkdir -p  ${1}/pvb/language_bindings/python/id
mkdir -p  ${1}/pvb/language_bindings/python/mt
mkdir -p  ${1}/pvb/language_bindings/python/template
mkdir -p  ${1}/pvb/browserplugin

echo 'copy documentation...'
cp -r doc                                                 ${1}/pvb/
echo 'copy pvsexample...'
cp -r pvsexample                                          ${1}/pvb/
echo 'copy other files...'
cp    LICENSE.GPL                                         ${1}/pvb/
cp    logo1.png                                           ${1}/pvb/
cp    custom.bmp                                          ${1}/pvb/
cp    gamsleiten.png                                      ${1}/pvb/
cp    pvbrowser.desktop                                   ${1}/pvb/
cp    pvdevelop.desktop                                   ${1}/pvb/
cp    pvserver/wthread.h                                  ${1}/pvb/pvserver/
cp    pvserver/BMP.h                                      ${1}/pvb/pvserver/
cp    pvserver/vmsglext.h                                 ${1}/pvb/pvserver/
cp    pvserver/pvbImage.h                                 ${1}/pvb/pvserver/
cp    pvserver/vmsgl.h                                    ${1}/pvb/pvserver/
cp    pvserver/processviewserver.h                        ${1}/pvb/pvserver/
cp    pvbrowser/pvbrowser                                 ${1}/pvb/pvbrowser/
cp    pvdevelop/pvdevelop                                 ${1}/pvb/pvdevelop/
cp    pvdevelop/pvdevelop.sh                              ${1}/pvb/pvdevelop/
cp    start_pvbapp/start_pvbapp                           ${1}/pvb/start_pvbapp/
cp    start_pvbapp/README.txt                             ${1}/pvb/start_pvbapp/
cp    start_pvbapp/example.ini.linux                      ${1}/pvb/start_pvbapp/example.ini
cp    start_pvbapp/start_if_not_already_running.sh        ${1}/pvb/start_pvbapp/
cp    rllib/rlsvg/rlsvgcat                                ${1}/pvb/rllib/rlsvg/
cp    rllib/rlfind/rlfind                                 ${1}/pvb/rllib/rlfind/
cp    rllib/rlhistory/rlhistory                           ${1}/pvb/rllib/rlhistory/
cp    rllib/rlhtml2pdf/rlhtml2pdf                         ${1}/pvb/rllib/rlhtml2pdf/
cp    fake_qmake/fake_qmake                               ${1}/pvb/fake_qmake/
cp    rllib/lib/*.h                                       ${1}/pvb/rllib/lib/
cp    rllib/lib/nodave.o                                  ${1}/pvb/rllib/lib/
cp    rllib/lib/setport.o                                 ${1}/pvb/rllib/lib/
cp    language_bindings/lua/pvslua/pvslua                 ${1}/pvb/language_bindings/lua/pvslua/
cp    language_bindings/lua/pvapplua/pvapplua             ${1}/pvb/language_bindings/lua/pvapplua/
cp    language_bindings/README_PYTHON.txt                 ${1}/pvb/language_bindings/
cp    language_bindings/python/template/main.cpp          ${1}/pvb/language_bindings/python/template/
cp    language_bindings/python/template/Makefile          ${1}/pvb/language_bindings/python/template/
cp    language_bindings/python/template/mask1.cpp         ${1}/pvb/language_bindings/python/template/
cp    language_bindings/python/template/mask1.py          ${1}/pvb/language_bindings/python/template/
cp    language_bindings/python/template/mask1_slots.h     ${1}/pvb/language_bindings/python/template/
cp    language_bindings/python/template/pvapp.h           ${1}/pvb/language_bindings/python/template/
cp    language_bindings/python/template/pvs_init.py       ${1}/pvb/language_bindings/python/template/
cp    language_bindings/python/template/pvs.pro           ${1}/pvb/language_bindings/python/template/
cp    language_bindings/python/template/pvs.pvproject     ${1}/pvb/language_bindings/python/template/
cp    designer/README.txt                                 ${1}/pvb/designer/
cp    browserplugin/pvbrowserplugin-example.html          ${1}/pvb/browserplugin/
cp    browserplugin/README.txt                            ${1}/pvb/browserplugin/
if [ "$PVB_OSTYPE" == "linux" ]; then
echo 'copy shared objects on linux...'
cp    browserplugin/libpvbrowser.so                       ${1}/pvb/browserplugin/
cp    language_bindings/python/mt/pv.py                   ${1}/pvb/language_bindings/python/mt/
cp    language_bindings/python/mt/_pv.so                  ${1}/pvb/language_bindings/python/mt/
cp    language_bindings/python/mt/rllib.py                ${1}/pvb/language_bindings/python/mt/
cp    language_bindings/python/mt/_rllib.so               ${1}/pvb/language_bindings/python/mt/
cp    language_bindings/python/id/pv.py                   ${1}/pvb/language_bindings/python/id/
cp    language_bindings/python/id/_pv.so                  ${1}/pvb/language_bindings/python/id/
cp    language_bindings/python/id/rllib.py                ${1}/pvb/language_bindings/python/id/
cp    language_bindings/python/id/_rllib.so               ${1}/pvb/language_bindings/python/id/
cp    designer/plugins/libpvb_designer_plugin.so          ${1}/pvb/designer/plugins/
cp    designer/plugins/libqwt_designer_plugin.so          ${1}/pvb/designer/plugins/
cp    pvserver/libpvsid.so                                ${1}/pvb/pvserver/
cp    pvserver/libpvsmt.so                                ${1}/pvb/pvserver/
cp    pvserver/libpvsid.a                                 ${1}/pvb/pvserver/
cp    pvserver/libpvsmt.a                                 ${1}/pvb/pvserver/
cp    rllib/lib/librllib.so                               ${1}/pvb/rllib/lib/
else
echo 'copy shared objects on OS-X ...'
cp    designer/plugins/libpvb_designer_plugin.dylib       ${1}/pvb/designer/plugins/
cp    designer/plugins/libqwt_designer_plugin.dylib       ${1}/pvb/designer/plugins/
cp    pvserver/libpvsid.a                                 ${1}/pvb/pvserver/
cp    pvserver/libpvsmt.a                                 ${1}/pvb/pvserver/
cp    rllib/lib/librllib.dylib                            ${1}/pvb/rllib/lib/
fi


if [ "$PVB_OSTYPE" == "linux" ]; then
echo 'set links on linux...'
#ln -sf ${1}/pvb/pvbrowser/pvbrowser                     /usr/bin/pvbrowser
#cp ${1}/pvb/pvdevelop/pvdevelop.sh                      /usr/bin/pvdevelop
#ln -sf ${1}/pvb/rllib/rlsvg/rlsvgcat                    /usr/bin/rlsvgcat
#ln -sf ${1}/pvb/rllib/rlfind/rlfind                     /usr/bin/rlfind
#ln -sf ${1}/pvb/start_pvbapp/start_pvbapp               /usr/bin/start_pvbapp
#ln -sf ${1}/pvb/rllib/rlhistory/rlhistory               /usr/bin/rlhistory
#ln -sf ${1}/pvb/rllib/rlhtml2pdf/rlhtml2pdf             /usr/bin/rlhtml2pdf
#ln -sf ${1}/pvb/fake_qmake/fake_qmake                   /usr/bin/fake_qmake
#ln -sf ${1}/pvb/update_pvbrowser.sh                     /usr/bin/update_pvbrowser
#ln -sf ${1}/pvb/language_bindings/lua/pvslua/pvslua     /usr/bin/pvslua
#ln -sf ${1}/pvb/language_bindings/lua/pvapplua/pvapplua /usr/bin/pvapplua

#ln -sf ${1}/pvb/rllib/lib/librllib.so         /usr/lib/librllib.so
#ln -sf ${1}/pvb/rllib/lib/librllib.so         /usr/lib/librllib.so.1
#ln -sf ${1}/pvb/rllib/lib/librllib.so         /usr/lib/librllib.so.1.0
#ln -sf ${1}/pvb/rllib/lib/librllib.so         /usr/lib/librllib.so.1.0.0

#ln -sf ${1}/pvb/pvserver/libpvsid.so          /usr/lib/libpvsid.so
#ln -sf ${1}/pvb/pvserver/libpvsid.so          /usr/lib/libpvsid.so.1
#ln -sf ${1}/pvb/pvserver/libpvsid.so          /usr/lib/libpvsid.so.1.0
#ln -sf ${1}/pvb/pvserver/libpvsid.so          /usr/lib/libpvsid.so.1.0.0

#ln -sf ${1}/pvb/pvserver/libpvsmt.so          /usr/lib/libpvsmt.so
#ln -sf ${1}/pvb/pvserver/libpvsmt.so          /usr/lib/libpvsmt.so.1
#ln -sf ${1}/pvb/pvserver/libpvsmt.so          /usr/lib/libpvsmt.so.1.0
#ln -sf ${1}/pvb/pvserver/libpvsmt.so          /usr/lib/libpvsmt.so.1.0.0
#echo 'running ldconfig...'
#ldconfig

else 
echo 'set links on OS-X ...'
cp -r  pvbrowser/pvbrowser.app                     ${1}/pvb/pvbrowser/
cp -r  pvdevelop/pvdevelop.app                     ${1}/pvb/pvdevelop/
cp -r  start_pvbapp/start_pvbapp.app               ${1}/pvb/start_pvbapp/
cp -r  rllib/rlsvg/rlsvgcat.app                    ${1}/pvb/rllib/rlsvg/
cp -r  rllib/rlfind/rlfind.app                     ${1}/pvb/rllib/rlfind/
cp -r  rllib/rlhistory/rlhistory.app               ${1}/pvb/rllib/rlhistory/
cp -r  rllib/rlhtml2pdf/rlhtml2pdf.app             ${1}/pvb/rllib/rlhtml2pdf/
cp -r  fake_qmake/fake_qmake.app                   ${1}/pvb/fake_qmake/
mkdir -p                                           ${1}/pvb/language_bindings/lua/pvslua
cp -r  language_bindings/lua/pvslua/pvslua.app     ${1}/pvb/language_bindings/lua/pvslua/
mkdir -p                                           ${1}/pvb/language_bindings/lua/pvapplua
cp -r  language_bindings/lua/pvapplua/pvapplua.app ${1}/pvb/language_bindings/lua/pvapplua/
mkdir -p                                           ${1}/pvb/spec/mac
cp -r  spec/mac/*                                  ${1}/pvb/spec/mac/

# ln -sf /usr/local/Cellar/pvb/pvbrowser/pvbrowser.app/Contents/MacOS/pvbrowser          /usr/local/bin/pvbrowser
# ln -sf /usr/local/Cellar/pvb/pvdevelop/pvdevelop.app/Contents/MacOS/pvdevelop          /usr/local/bin/pvdevelop
# ln -sf /usr/local/Cellar/pvb/start_pvbapp/start_pvbapp.app/Contents/MacOS/start_pvbapp /usr/local/bin/start_pvbapp
# ln -sf /usr/local/Cellar/pvb/rllib/rlsvg/rlsvgcat.app/Contents/MacOS/rlsvgcat          /usr/local/bin/rlsvgcat
# ln -sf /usr/local/Cellar/pvb/rllib/rlfind/rlfind.app/Contents/MacOS/rlfind             /usr/local/bin/rlfind
# ln -sf /usr/local/Cellar/pvb/rllib/rlhistory/rlhistory                                 /usr/local/bin/rlhistory
# ln -sf /usr/local/Cellar/pvb/rllib/rlhtml2pdf/rlhtml2pdf                               /usr/local/bin/rlhtml2pdf
# ln -sf /usr/local/Cellar/pvb/fake_qmake/fake_qmake.app/Contents/MacOS/fake_qmake       /usr/local/bin/fake_qmake
# ln -sf /usr/local/Cellar/qt/5.10.0/bin/qmake                       /usr/local/bin/qmake
# ln -sf /usr/local/Cellar/pvb/update_pvbrowser.sh                                       /usr/local/bin/update_pvbrowser
# ln -sf /usr/local/Cellar/pvb/pvslua/Contents/MacOS/pvslua      /usr/local/bin/pvslua
# ln -sf /usr/local/Cellar/pvb/pvapplua/Contents/MacOS/pvapplua  /usr/local/bin/pvapplua

# ln -sf /usr/local/Cellar/pvb/rllib/lib/librllib.dylib    /usr/local/lib/librllib.dylib
# ln -sf /usr/local/Cellar/pvb/rllib/lib/librllib.dylib    /usr/local/lib/librllib.1.dylib
# ln -sf /usr/local/Cellar/pvb/rllib/lib/librllib.dylib    /usr/local/lib/librllib.1.0.dylib
# ln -sf /usr/local/Cellar/pvb/rllib/lib/librllib.dylib    /usr/local/lib/librllib.1.0.0.dylib

# ln -sf /usr/local/Cellar/pvb/pvb/pvserver/libpvsid.dylib /usr/local/lib/libpvsid.dylib
# ln -sf /usr/local/Cellar/pvb/pvb/pvserver/libpvsid.dylib /usr/local/lib/libpvsid.1.dylib
# ln -sf /usr/local/Cellar/pvb/pvb/pvserver/libpvsid.dylib /usr/local/lib/libpvsid.1.0.dylib
# ln -sf /usr/local/Cellar/pvb/pvb/pvserver/libpvsid.dylib /usr/local/lib/libpvsid.1.0.0.dylib

# ln -sf /usr/local/Cellar/pvb/pvb/pvserver/libpvsmt.dylib /usr/local/lib/libpvsmt.dylib
# ln -sf /usr/local/Cellar/pvb/pvb/pvserver/libpvsmt.dylib /usr/local/lib/libpvsmt.1.dylib
# ln -sf /usr/local/Cellar/pvb/pvb/pvserver/libpvsmt.dylib /usr/local/lib/libpvsmt.1.0.dylib
# ln -sf /usr/local/Cellar/pvb/pvb/pvserver/libpvsmt.dylib /usr/local/lib/libpvsmt.1.0.0.dylib

fi

echo "makeing ${1}/pvb/pvsexample writeable..."
cd ${1}/pvb
chmod ugoa+w pvsexample
cd ${1}/pvb/pvsexample
chmod ugoa+w *

#if [ "$PVB_OSTYPE" == "linux" ]; then
#echo 'creating pvbrowserdemo...'
#echo '#!/bin/bash'                                                                     > /usr/bin/pvbrowserdemo
#echo '# run pvsexample'                                                               >> /usr/bin/pvbrowserdemo
#echo 'xterm -e "${1}/pvb/pvsexample/pvsexample -cd=${1}/pvb/pvsexample -sleep=300" &' >> /usr/bin/pvbrowserdemo
#echo 'pvbrowser &'                                                                    >> /usr/bin/pvbrowserdemo
#chmod ugoa+x /usr/bin/pvbrowserdemo

# copy qt designer plugins to destination
#if [ -d /usr/lib64/qt4/plugins ]; then
#  export PLUGIN=$(find /usr/lib64/qt4/plugins/ -name designer)
#  if [ "x$PLUGIN" != "x" ]; then
#  echo copy qt designer plugins to $PLUGIN/ ...
#  cp ${1}/pvb/designer/plugins/* $PLUGIN/
#  fi
#fi  
#if [ -d /usr/lib/qt4/plugins ]; then
#  export PLUGIN=$(find /usr/lib/qt4/plugins/ -name designer)
#  if [ "x$PLUGIN" != "x" ]; then
#  echo copy qt designer plugins to $PLUGIN/ ...
#  cp ${1}/pvb/designer/plugins/* $PLUGIN/
#  fi
#fi  
#fi

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
echo "#   ${1}/pvb/pvbrowser.desktop ${1}/pvb/pvdevelop.desktop      #"
echo "#                                                              #"
echo "# Example pvserver:                                            #"
echo "#   cd ${1}/pvb/pvsexample                                     #"
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
