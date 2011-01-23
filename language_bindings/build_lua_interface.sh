#!/bin/bash
#
# Build the Lua interface for ProcessViewServer Library
#
export liblua=$(find /usr/lib -name liblua.a)
if [ "x${liblua}" = "x" ]; then
  export liblua=$(find /usr/lib64 -name liblua.a)
fi
echo liblua = $liblua

mkdir -p lua
if [ "x${1}" = "x" ]; then
  echo "predefined colors within processviewserver.h can't be used in lua. You can ignore the following warnings"
  echo "running swig ..."
  swig -c++ -lua -Dunix language_binding.i
  cp         language_binding_wrap.cxx language_binding_wrap_lua.cxx 
  swig -c++ -lua -Dunix language_binding_rllib.i
  cp         language_binding_rllib_wrap.cxx language_binding_rllib_wrap_lua.cxx 
fi

echo compiling pvsbinding ...
g++ -c -fpic language_binding_wrap_lua.cxx
echo compiling rllibbinding ... You can ignore the 'PACKED' warnings from cif_user.h
g++ -c -fpic language_binding_rllib_wrap_lua.cxx
g++ -c -fpic pvmain.cpp -o pvmain.o
ar  -rs      lua/libluawrap.a pvmain.o language_binding_wrap_lua.o language_binding_rllib_wrap_lua.o $liblua 
rm  -f       language_binding_wrap.cxx
rm  -f       language_binding_rllib_wrap.cxx

echo "created lua/:"
ls -al lua
echo "usage: ./build_lua_interface.sh <noswig>"
