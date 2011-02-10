#!/bin/bash

find . -name "*.a"               -exec rm "{}" ";"
find . -name "*.py"              -exec rm "{}" ";"

cd ..

find . -name "*~"                -exec rm "{}" ";"
find . -name "*.o"               -exec rm "{}" ";"
find . -name "moc_*"             -exec rm "{}" ";"
find . -name "qrc_*"             -exec rm "{}" ";"
find . -name "ui_*"              -exec rm "{}" ";"
find . -name "Makefilei.*"       -exec rm "{}" ";"
find . -name "*.Debug"           -exec rm "{}" ";"
find . -name "*.Release"         -exec rm "{}" ";"
find . -name "*.pdb"             -exec rm "{}" ";"
find . -name "*.exe"             -exec rm "{}" ";"
find . -name "*.obj"             -exec rm "{}" ";"
find . -name "*.dll"             -exec rm "{}" ";"
find . -name "*object_script.*"  -exec rm "{}" ";"
find . -name "debug"             -exec rm -rf "{}" ";"
find . -name "release"           -exec rm -rf "{}" ";"

cd win-mingw

