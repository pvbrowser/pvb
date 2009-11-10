cd ../qwt
nmake clean
cd ../win

cd pvbrowser
nmake clean
cd ..

cd pvdevelop
nmake clean
cd ..

cd pvserver
nmake clean
cd ..

cd rllib
nmake clean
cd ..

cd rlsvg
nmake clean
cd ..

cd rlhistory
nmake clean
cd ..

cd fake_qmake
del *.obj
del *.exe
cd ..

cd setupregistry
del *.obj
del *.exe
cd ..

cd language_bindings
del *.pdb
del swig\release\*.*
del Makefile*.* 
cd ..
