%swigdir%\swig -c++ -python -DRLWIN32 -D_WIN32 language_binding.i
%swigdir%\swig -c++ -python -DRLWIN32 -D_WIN32 -DSWIG_SESSION language_binding_rllib.i
%qtdir%\bin\qmake pv.pro
nmake -f Makefile
%qtdir%\bin\qmake rllib.pro
nmake -f Makefile
echo "results are within debug. Ignore the error."
copy *.py          ..\..\bin
copy release\_*.*  ..\..\bin 
