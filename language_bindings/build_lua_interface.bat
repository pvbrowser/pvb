rem ------------------------------
rem - build lua language binding -
rem ------------------------------

%swigdir%\swig -c++ -lua -DPVWIN32 -D_WIN32 -DSWIG_SESSION language_binding.i
copy language_binding_wrap.cxx        language_binding_wrap_lua.cxx
%swigdir%\swig -c++ -lua -DRLWIN32 -D_WIN32 -DSWIG_SESSIOM language_binding_rllib.i
copy language_binding_rllib_wrap.cxx  language_binding_rllib_wrap_lua.cxx

cd lua\lua-5.4.0\src
qmake src.pro
mingw32-make.exe
cd ..\..\..

cd lua\pvslua
qmake pvslua.pro
mingw32-make.exe
mingw32-make.exe clean

cd ..\pvapplua
qmake pvapplua.pro
mingw32-make.exe
mingw32-make.exe clean

cd ..\..
del *.cxx

copy lua\lua-5.4.0\src\release\lua.dll ..\win-mingw\bin\

