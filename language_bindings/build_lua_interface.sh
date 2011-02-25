#!/bin/bash
#
# usage: ./build_lua_interface.sh <swig>
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
cd lua/lua-5.1/src/
 ../../../../qmake.sh src.pro
 make
cd ../../../

if [ "x${1}" = "xswig" ]; then
  echo "predefined colors within processviewserver.h can't be used in lua. You can ignore the following warnings"
  echo "running swig ..."
  rm -f lua/language_binding_wrap_lua.o
  rm -f lua/language_binding_rllib_wrap_lua.o
  rm -f lua/pvmain.o
  swig -c++ -lua -Dunix -DSWIG_SESSION language_binding.i
  mv language_binding_wrap.cxx         language_binding_wrap_lua.cxx
  swig -c++ -lua -Dunix -DSWIG_SESSIOM language_binding_rllib.i
  mv language_binding_rllib_wrap.cxx   language_binding_rllib_wrap_lua.cxx
fi

cd lua/pvslua
../../../qmake.sh pvslua.pro
make
cd ../pvapplua
../../../qmake.sh pvapplua.pro
make
cd ../..

