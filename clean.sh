#!/bin/bash
find . -name "*~" -exec rm "{}" ";"

rm -f *.lnk

cd qwt/src/obj
rm -f *.o
rm -f ../moc/*
cd ../../..

cd qwt/designer
make clean
rm obj/*.obj
rm resources/qrc_qwt_designer_plugin.cpp
cd ../..

cd pvbrowser
rm -f *.o
rm -f moc_*
rm -f ui_*.h
rm -f qrc_pvbrowser.cpp
cd ..

cd browserplugin
rm -f *.dll
rm -f *.so
rm -f *.o
rm -f moc_*
rm -f ui_*.h
rm -f qrc_pvbrowser.cpp
cd ..

cd pvdevelop
rm -f *.o
rm -f moc_*
rm -f ui_*.h
rm -f qrc_pvdevelop.cpp
cd ..

cd designer/src
./build.sh clean
cd ../..

cd start_pvbapp
make clean
cd ..

cd pvserver
rm -f *.o
cd ..

cd rllib/lib
rm -f *.o
cd ../..

cd rllib/rlsvg
rm -f *.o
cd ../..

cd rllib/rlhistory
rm -f *.o
cd ../..

cd pvsexample
rm -f *.o
rm -f *.obj
cd ..

cd language_bindings
find . -name "*.o" -exec rm "{}" ";"
rm -f lua/pvslua/*.exe
rm -f lua/pvslua/Makefile.win
cd ..
