#!/bin/bash
cd ..
mv win-mingw win-mingw-old
mkdir -p win-mingw/bin/plugins/designer
cp win-mingw-old/CreatePvbWithMinGW.bat     win-mingw/
cp win-mingw-old/Readme*.txt                win-mingw/
cp win-mingw-old/set_env.bat                win-mingw/
cp win-mingw-old/logo1.ico                  win-mingw/
cp win-mingw-old/nodave_mingw.o             win-mingw/
cp win-mingw-old/bin/*                      win-mingw/bin/
cp win-mingw-old/bin/plugins/designer/*.dll win-mingw/bin/plugins/designer/
cp win-mingw-old/bin/plugins/designer/*.a   win-mingw/bin/plugins/designer/
rm -rf win-mingw-old
cd win-mingw
ls
