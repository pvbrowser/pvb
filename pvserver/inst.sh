#!/bin/bash
source makelibs.sh
chgrp users  *
chown lehrig *
# make -f pvsmt.mak clean
# make -f pvsmt.mak
# make -f pvsid.mak clean
# make -f pvsid.mak
cp processviewserver.h /usr/include
cp wthread.h           /usr/include
chmod ugoa+r /usr/include/processviewserver.h
chmod ugoa+r /usr/include/wthread.h
rm /usr/lib/libpvs*
cp libpvsmt.so.1.0.0 /usr/lib
cp libpvsid.so.1.0.0 /usr/lib
rm *.o
cd /usr/lib
ln -s libpvsmt.so.1.0.0 libpvsmt.so
ln -s libpvsmt.so.1.0.0 libpvsmt.so.1
ln -s libpvsmt.so.1.0.0 libpvsmt.so.1.0
ln -s libpvsid.so.1.0.0 libpvsid.so
ln -s libpvsid.so.1.0.0 libpvsid.so.1
ln -s libpvsid.so.1.0.0 libpvsid.so.1.0
ldconfig

