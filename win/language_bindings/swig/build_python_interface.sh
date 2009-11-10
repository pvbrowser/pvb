#!/bin/bash
#
# Build the Python interface for ProcessViewServer Library
#
echo "### Probe for necessary components #######################################################################"

if [ "x${1}" = "x" ]; then
  SWIG=$(which swig)
  if [ "x${SWIG}" = "x" ]; then
    echo "swig not found. Install package swig !"
    return
  fi
fi

PYTHON_INCLUDE=$(find /usr/include -name "python*")
if [ "x${PYTHON_INCLUDE}" = "x" ]; then
  echo "No Python Includes found. Install package python-devel !"
  return
fi

PYTHON_H=$(find /usr/include -name Python.h -printf "%h\n")
if [ "x${PYTHON_H}" = "x" ]; then
  echo "No Python.h found. Install package python-devel !"
  return
fi

echo "### Create python bindings for pv:xinetd ... ##############################################################"
echo "predefined colors within processviewserver.h can't be used in python. You can ignore the following warnings"
mkdir -p python/id
if [ "x${1}" = "x" ]; then
  echo "running swig for pv id ..."
  swig -c++ -python -Dunix language_binding.i
  cp language_binding_wrap.cxx language_binding_wrap_id.cxx 
  mv pv.py pv_id.py
fi
echo "you can ignore the following fatal error"
g++ -c -fpic language_binding_wrap_id.cxx -I$PYTHON_INCLUDE
g++ -c -fpic pvmain.cpp -o pvmain.o
g++ -shared pvmain.o ../pvserver/libpvsid.so.1 language_binding_wrap_id.o -o python/id/_pv.so
sed "s/None = _pv.None/pvNone = _pv.None/" pv_id.py > python/id/pv.py
rm -rf language_binding_wrap.cxx
rm -rf *.o

echo "### Create python bindings for pv:multi_threaded_server ... ###############################################"
echo "predefined colors within processviewserver.h can't be used in python. You can ignore the following warnings"
mkdir -p python/mt
if [ "x${1}" = "x" ]; then
  echo "running swig for pv mt ..."
  swig -c++ -python -Dunix language_binding.i
  cp language_binding_wrap.cxx language_binding_wrap_mt.cxx
  mv pv.py pv_mt.py
fi
echo "you can ignore the following fatal error"
g++ -c -fpic language_binding_wrap_mt.cxx -I$PYTHON_INCLUDE
g++ -c -fpic pvmain.cpp -o pvmain.o
g++ -shared pvmain.o ../pvserver/libpvsmt.so.1 language_binding_wrap_mt.o -o python/mt/_pv.so
sed "s/None = _pv.None/pvNone = _pv.None/" pv_mt.py > python/mt/pv.py
rm -rf language_binding_wrap.cxx
rm -rf *.o

echo "### Create python bindings for rllib ... ##################################################################"
if [ "x${1}" = "x" ]; then
  echo "running swig for rllib ..."
  swig -c++ -python -Dunix -DSWIG_SESSION language_binding_rllib.i
fi
echo "you can ignore the following fatal error"
g++ -c -fpic language_binding_rllib_wrap.cxx -I$PYTHON_INCLUDE
g++ -shared ../rllib/lib/librllib.so.1 language_binding_rllib_wrap.o -o _rllib.so
cp _rllib.so python/id/
cp _rllib.so python/mt/
cp rllib.py python/id/
cp rllib.py python/mt/
rm *.so
rm -rf *.o

echo "### Do not feel confused by the above fatal error ########################################################"
echo "### Do not feel confused by the above warnings ###########################################################"
echo "### rlController can't be supported by python (because no callbacks are allowed in python)  ##############"
echo "created python/id/: "
ls python/id
echo "created python/mt/: "
ls python/mt
echo "usage: ./build_python_interface.sh <noswig>"
