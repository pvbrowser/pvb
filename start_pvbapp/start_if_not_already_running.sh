#!/bin/bash
# test if application $1 is running
export instance=$(pstree | grep $1)
if [ "x${instance}" = "x" ]; then
  # echo $1 not running. starting ...
  $1 $2 $3 $4 $5 $6 $7 $8 $9 &
else
  echo $1 already running
fi
# this shell script may be usefull for starting a daemon for data acquisition
# thus you can avoid that it is started twice
