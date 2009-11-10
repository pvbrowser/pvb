$ !
$ ! Build libraries to use with ProcessViewServer
$ !
$ set veri
$ !
$ ! Library to use as multithreaded server
$ !
$ compiler := cxx
$ linker   := cxxlink
$
$ 'compiler' pvbimage.cpp
$ 'compiler' util.cpp
$ 'compiler' wthread.cpp
$ 'compiler' glencode.cpp
$
$ lib/create  [-.obj]pvsmt.olb
$ lib/replace [-.obj]pvsmt.olb pvbimage.obj
$ lib/replace [-.obj]pvsmt.olb util.obj
$ lib/replace [-.obj]pvsmt.olb wthread.obj
$ lib/replace [-.obj]pvsmt.olb glencode.obj
$ delete *.obj;*
$ !
$ ! Library to use with: uxc set service
$ !
$ compiler := cxx/define=USE_INETD
$ linker   := cxxlink
$
$ 'compiler' pvbimage.cpp
$ 'compiler' util.cpp
$ 'compiler' wthread.cpp
$ 'compiler' glencode.cpp
$
$ lib/create  [-.obj]pvsid.olb
$ lib/replace [-.obj]pvsid.olb pvbimage.obj
$ lib/replace [-.obj]pvsid.olb util.obj
$ lib/replace [-.obj]pvsid.olb wthread.obj
$ lib/replace [-.obj]pvsid.olb glencode.obj
$ delete *.obj;*
$
$ set noveri
