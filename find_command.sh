#!/bin/bash
if [ "x${1}" = "x" ]; then
  echo "usage: ./find_command.sh commandString"
  grep strncmp pvbrowser/interpreter.cpp pvbrowser/QDrawWidget.cpp 
  # pvbrowser/gldecode.cpp | less
else  
  echo "util.cpp :"
  grep "${1}" pvserver/util.cpp
  echo "interpreter.cpp :"
  grep "${1}" pvbrowser/interpreter.cpp
  echo "QDrawWidget.cpp :"
  grep "${1}" pvbrowser/QDrawWidget.cpp 
fi  

