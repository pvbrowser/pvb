#!/bin/bash
#
# Attention: you must install
#            - gcc
#            - qt-devel
#            - lua-devel
#            first
#
# Build the Lua interface and create pvb/language_bindings/lua/pvslua/pvslua
# Then do the following for installation of pvslua:
# su
# cp lua/pvslua/pvslua /usr/bin/
# exit
#
# Attention: there will be a lot of warnings that are due to foreign software
#            You can ignore these warnings
#
export liblua=$(find /usr/lib -name liblua.a)
if [ "x${liblua}" = "x" ]; then
  export liblua=$(find /usr/lib64 -name liblua.a)
fi
echo liblua = $liblua

mkdir -p lua
if [ "x${1}swig" = "xswig" ]; then
  echo "predefined colors within processviewserver.h can't be used in lua. You can ignore the following warnings"
  echo "running swig ..."
  swig -c++ -lua -Dunix language_binding.i
  swig -c++ -lua -Dunix language_binding_rllib.i
fi

echo compiling pvsbinding ...
cp language_binding_wrap.cxx language_binding_wrap_lua.cxx
g++ -c -fpic language_binding_wrap_lua.cxx
echo compiling rllibbinding ... You can ignore the 'PACKED' warnings from cif_user.h
cp language_binding_rllib_wrap.cxx language_binding_rllib_wrap_lua.cxx
g++ -c -fpic language_binding_rllib_wrap_lua.cxx
g++ -c -fpic -DLUA pvmain.cpp -o pvmain.o
mv language_binding_wrap_lua.o       lua/
mv language_binding_rllib_wrap_lua.o lua/
mv pvmain.o                          lua/
rm language_binding_wrap.cxx
rm language_binding_rllib_wrap.cxx

echo "created:"
ls -al lua

cd lua/pvslua
qmake pvslua.pro
make  clean
make

echo created lua pvserver lua/pvslua/pvslua
echo "####################################################"
echo "# usage: ./build_lua_interface.sh <swig>           #"
echo "# Now run the following for installation of pvslua #"
echo "# su                                               #"
echo "# cp lua/pvslua/pvslua /usr/bin/                   #"
echo "# exit                                             #"
echo "####################################################"
