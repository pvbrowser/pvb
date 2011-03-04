#!/bin/bash
#
# Build the Python interface for ProcessViewServer Library
#
echo "### Probe necessary components for python language binding ###"

if [ "${1}" = "swig" ]; then
  SWIG=$(which swig)
  if [ "x${SWIG}" = "x" ]; then
    echo "swig not found. Install package swig !"
    exit
  fi
fi

export PYTHON_INCLUDE=$(find /usr/include -name "python2.*")
if [ "x${PYTHON_INCLUDE}" = "x" ]; then
  echo "No Python Includes found. Install package python-devel !"
  exit
fi

export PYTHON_LIB=$(find /usr/lib64 -name "libpython2.*.so")
if [ "x${PYTHON_LIB}" = "x" ]; then
  export PYTHON_LIB=$(find /usr/lib -name "libpython2.*.so")
  if [ "x${PYTHON_LIB}" = "x" ]; then
    echo "No Python library found. Install package python-devel !"
    exit
  fi
fi

echo python_include=${PYTHON_INCLUDE}
echo python_lib=${PYTHON_LIB}

if [ "${1}" = "swig" ]; then
  mkdir -p python/id
  mkdir -p python/mt
  echo "running swig for pv id ..."
  swig -c++ -python -Dunix language_binding.i
  mv language_binding_wrap.cxx language_binding_wrap_id.cxx 
  mv pv.py python/id/
  echo "running swig for pv mt ..."
  swig -c++ -python -Dunix language_binding.i
  mv language_binding_wrap.cxx language_binding_wrap_mt.cxx
  mv pv.py python/mt/
  echo "running swig for rllib ..."
  swig -c++ -python -Dunix -DSWIG_SESSION language_binding_rllib.i
  mv language_binding_rllib_wrap.cxx language_binding_rllib_wrap_python.cxx
  cp rllib.py python/id/
  cp rllib.py python/mt/
  rm -f rllib.py
fi

cd python/mt
../../../qmake.sh mt.pro
make
../../../qmake.sh rllib.pro
make
cp lib_pv.so    _pv.so
cp lib_rllib.so _rllib.so
cd ../id
../../../qmake.sh id.pro
make
../../../qmake.sh rllib.pro
make
cp lib_pv.so    _pv.so
cp lib_rllib.so _rllib.so
cd ../../

