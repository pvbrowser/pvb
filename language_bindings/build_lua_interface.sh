#!/bin/bash
#
# usage: ./build_lua_interface.sh <swig>
#
echo "### building lua language bindings ###"
echo "Info for Linux: libreadline-dev must be installed in order to build the lua interface"
export PVBLANGUAGEDIR=$PWD
cd lua/lua-5.4.0/src/
 ../../../../qmake.sh src.pro
 make
cd ../../../

if [ "x${1}" = "xswig" ]; then
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

