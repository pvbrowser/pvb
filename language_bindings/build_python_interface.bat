rem ---------------------------------
rem - build python language binding -
rem ---------------------------------

%swigdir%\swig -c++ -python -DPVWIN32 -D_WIN32 -DSWIG_SESSION language_binding.i
copy language_binding_wrap.cxx        language_binding_wrap_python.cxx
%swigdir%\swig -c++ -python -DRLWIN32 -D_WIN32 -DSWIG_SESSIOM language_binding_rllib.i
copy language_binding_rllib_wrap.cxx  language_binding_rllib_wrap_python.cxx

qmake pv.pro
mingw32-make.exe
copy release\_pv.dll    ..\win-mingw\bin\
mingw32-make.exe clean

qmake rllib.pro
mingw32-make.exe
copy release\_rllib.dll ..\win-mingw\bin\
mingw32-make.exe clean

