$ !
$ ! Build demo of ProcessViewServer
$ !
$ set veri
$ !
$ ! compiler := cxx/define=USE_INETD
$ compiler := cxx
$ linker   := cxxlink
$
$ 'compiler' main.cpp          
$ 'compiler' mask1.cpp         
$ 'compiler' mask2.cpp
$ 'compiler' mask3.cpp         
$ 'compiler' mask4.cpp         
$ 'compiler' maskvtk.cpp       
$ 'compiler' modal1.cpp
$ 'compiler' periodic.cpp      
$ 'compiler' qwt.cpp
$ 'compiler' util.cpp
$ 'compiler' wthread.cpp
$ 'compiler' pvbimage.cpp
$ 'compiler' glencode.cpp
$ 'linker'   main.obj,mask1.obj,mask2.obj,mask3.obj,mask4.obj,maskvtk.obj,modal1.obj,periodic.obj,qwt.obj,util.obj,wthread.obj,pvbimage.obj,glencode.obj
$ set noveri
