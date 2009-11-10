#!/bin/bash

cd ../qwt
rm -f src/obj/*.obj
rm -f lib/qwt.lib
rm -f designer/obj/*
rm -f designer/moc/*
cd ../win

cd pvbrowser
rm -rf ../../pvbrowser/release
rm -rf ../../pvbrowser/debug
rm -f  ../../pvbrowser/Makefile.Release
rm -f  ../../pvbrowser/Makefile.Debug
rm -rf release
rm -rf debug
cd ..

cd pvdevelop
rm -rf ../../pvdevelop/release
rm -rf ../../pvdevelop/debug
rm -f  ../../pvdevelop/Makefile.Release
rm -f  ../../pvdevelop/Makefile.Debug
rm -rf release
rm -rf debug
cd ..

rm -rf ../start_pvbapp/release
rm -rf ../start_pvbapp/debug
rm -f  ../start_pvbapp/*.o
rm -f  ../start_pvbapp/Makefile*

rm -rf ../designer/src/release
rm -rf ../designer/src/debug
rm -f  ../designer/src/Makefile.Release
rm -f  ../designer/src/Makefile.Debug
rm -f  ../designer/plugins/*.lib
rm -f  ../designer/plugins/*.dll
rm -f  ../designer/plugins/*.exp

rm -f ../pvsexample/*.obj
rm -f ../pvsexample/PVTMP*

cd pvserver
rm -rf release
rm -rf debug
cd ..

cd rllib
rm -rf release
rm -rf debug
rm -f *.o
rm -f *.obj
cd ..

cd rlsvg
rm -rf release
rm -rf debug
cd ..

cd fake_qmake
rm -f *.obj
rm -f *.exe
cd ..

cd setupregistry
rm -f *.obj
rm -f *.exe
cd ..

./cleanup_mingw.sh

