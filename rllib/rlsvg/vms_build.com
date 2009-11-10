$ !
$ ! Build rlsvgcat.exe to use with ProcessViewServer
$ ! Please define a symbol rlsvgcat in sylogin.com or login.com
$ ! rlsvgcat := disk:[path]rlsvgcat.exe 
$ !
$ set veri
$
$ compiler := cxx
$ linker   := cxxlink
$
$ 'compiler' rlsvgcat.cpp
$ 'linker' rlsvgcat.obj
$ delete *.obj;*
$
$ set noveri
