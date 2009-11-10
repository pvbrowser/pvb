#!/bin/bash
# Building an RPM for pvbrowser-4.x.y on openSUSE x86_64
#------------------------------------------------------------------
#echo "Have you already installed the Qt4    development package ?"
#echo "Have you already installed the mesa   development package ?"
#echo "Have you already installed the Tcl/Tk development package ?"
#echo "Have you already installed the Vtk    development package ?"
#echo "If Yes hit RETURN else type no"
#read answer
#if test "nix$answer" != "nix"
#then
#  echo "Exit"
#  exit
#fi
echo "Doit"
##################
# Now Doit       #
##################
cd ..
mv pvb pvbrowser-4
tar -zcf pvbrowser-4.tar.gz pvbrowser-4
cd pvbrowser-4
#./clean.sh
su -c "./uninstall.sh;cp ../pvbrowser-4.tar.gz /usr/src/packages/SOURCES/pvb.tar.gz;rpmbuild -ba pvbrowser-42.spec"
cd ..
cp /usr/src/packages/RPMS/x86_64/pvbrowser-4-2.0.x86_64.rpm .
mv pvbrowser-4 pvb
rm pvbrowser-4.tar.gz
# Done
