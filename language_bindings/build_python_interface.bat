rem ---------------------------------
rem - build python language binding -
rem ---------------------------------

%swigdir%\swig -c++ -python -DPVWIN32 -D_WIN32 -DSWIG_SESSION language_binding.i
copy language_binding_wrap.cxx        language_binding_wrap_python.cxx
%swigdir%\swig -c++ -python -DRLWIN32 -D_WIN32 -DSWIG_SESSIOM language_binding_rllib.i
copy language_binding_rllib_wrap.cxx  language_binding_rllib_wrap_python.cxx

%qtdir%\bin\qmake pv.pro
mingw32-make.exe
copy lib_pv.a    ..\win-mingw\bin\
mingw32-make.exe clean

%qtdir%\bin\qmake rllib.pro
mingw32-make.exe
copy lib_rllib.a ..\win-mingw\bin\
mingw32-make.exe clean

