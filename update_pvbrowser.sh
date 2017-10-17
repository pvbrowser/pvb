#!/bin/bash
# Install new version of pvbrowser
cd
mkdir -p pvbrowser
cd pvbrowser

echo "Get pvbrowser"
rm -f pvb.tar.gz
wget http://pvbrowser.de/pvbrowser/tar/pvb.tar.gz
tar -zxf pvb.tar.gz

if [ "${HOSTTYPE}" = "x86_64" ]; then
  echo "Enter root password for installation"
  su -c "cd pvb; ./install.sh"
  rm -rf pvb/
  echo "If you have not installed Qt5 you will have todo it"
  echo "now trying to run /opt/pvb/pvsexample/pvsexample"
  cd /opt/pvb/pvsexample
  xterm -e "./pvsexample -sleep=100" &
  pvbrowser &
else
  echo "You seem to have a 32 bit system."
  echo "Please recompile and install with:"
  echo "cd pvb"
  echo "./build.sh"
  echo "su"
  echo "./install.sh"
  echo "exit"
fi

