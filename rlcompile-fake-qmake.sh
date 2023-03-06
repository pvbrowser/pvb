#!/bin/csh -v
cd fake_qmake
g++ -c -m64 -pipe -O2 -Wall -W  -I. -I../rllib/lib -o main.o main.cpp
g++ -c -m64 -pipe -O2 -Wall -W  -I. -I../rllib/lib -o fake_qmake.o fake_qmake.cpp
g++ -c -m64 -pipe -O2 -Wall -W -Wno-implicit-fallthrough -I. -I../rllib/lib -o rlstring.o ../rllib/lib/rlstring.cpp
g++ -c -m64 -pipe -O2 -Wall -W  -I. -I../rllib/lib -o rlspreadsheet.o ../rllib/lib/rlspreadsheet.cpp
g++ -c -m64 -pipe -O2 -Wall -W  -I. -I../rllib/lib -o rlfileload.o ../rllib/lib/rlfileload.cpp
g++ -c -m64 -pipe -O2 -Wall -W  -I. -I../rllib/lib -o rlcutil.o ../rllib/lib/rlcutil.cpp
g++ -m64 -Wl,-O1 -o fake_qmake main.o fake_qmake.o rlstring.o rlspreadsheet.o rlfileload.o rlcutil.o
cd ..
