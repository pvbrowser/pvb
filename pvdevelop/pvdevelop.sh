#!/bin/bash

#export PYTHON_INCLUDE=$(find /usr/include -name "python2.*")
#if [ "x${PYTHON_INCLUDE}" = "x" ]; then
#  echo "python support was not found"
#fi
#
#if [ -d /usr/lib64 ]
#then
#  echo find
#  export PYTHON_LIB=$(find /usr/lib64 -name "libpython2.*.so")
#fi
#
#if [ "x${PYTHON_LIB}" = "x" ]; then
#  export PYTHON_LIB=$(find /usr/lib -name "libpython2.*.so")
#  if [ "x${PYTHON_LIB}" = "x" ]; then
#    echo "python support was not found"
#  fi
#fi

/opt/pvb/pvdevelop/pvdevelop $1 $2 $3 $4 $5 $6 $7 $8 $9
