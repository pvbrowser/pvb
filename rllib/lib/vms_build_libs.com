$ !
$ ! Build rllib library to use with ProcessViewServer
$ !
$ set veri
$ compiler := cxx
$ linker   := cxxlink
$
$ 'compiler' rl3964r.cpp
$ ! 'compiler' rlbussignaldatabase.cpp
$ ! 'compiler' rlcannode.cpp
$ ! 'compiler' rlcanopenclient.cpp
$ ! 'compiler' rlcanopen.cpp
$ ! 'compiler' rlcanopendaemon.cpp
$ ! 'compiler' rlcanopentypes.cpp
$ 'compiler' rlcontroller.cpp
$ ! 'compiler' rlcorba.cpp
$ 'compiler' rlcutil.cpp
$ 'compiler' rldataprovider.cpp
$ 'compiler' rleibnetip.cpp
$ 'compiler' rlevent.cpp
$ 'compiler' rleventlogserver.cpp
$ 'compiler' rlfifo.cpp
$ 'compiler' rlfileload.cpp
$ 'compiler' rlhistorylogger.cpp
$ 'compiler' rlhistoryreader.cpp
$ 'compiler' rlinifile.cpp
$ 'compiler' rlinterpreter.cpp
$ 'compiler' rlmailbox.cpp
$ 'compiler' rlmodbusclient.cpp
$ 'compiler' rlmodbus.cpp
$ 'compiler' rlpcontrol.cpp
$ 'compiler' rlppiclient.cpp
$ 'compiler' rlserial.cpp
$ 'compiler' rlsharedmemory.cpp
$ 'compiler' rlsiemenstcpclient.cpp
$ 'compiler' rlsiemenstcp.cpp
$ 'compiler' rlsocket.cpp
$ ! 'compiler' rlspawn.cpp
$ 'compiler' rlspreadsheet.cpp
$ ! 'compiler' rlsvganimator.cpp
$ 'compiler' rlthread.cpp
$ 'compiler' rltime.cpp
$ 'compiler' rludpsocket.cpp
$ 'compiler' rlwthread.cpp
$ 'compiler' rlstring.cpp
$
$ lib/create  [-.obj]rllib.olb
$ lib/replace [-.obj]rllib.olb rl3964r.obj
$ ! lib/replace [-.obj]rllib.olb rlbussignaldatabase.obj
$ ! lib/replace [-.obj]rllib.olb rlcannode.obj
$ ! lib/replace [-.obj]rllib.olb rlcanopenclient.obj
$ ! lib/replace [-.obj]rllib.olb rlcanopen.obj
$ ! lib/replace [-.obj]rllib.olb rlcanopendaemon.obj
$ ! lib/replace [-.obj]rllib.olb rlcanopentypes.obj
$ lib/replace [-.obj]rllib.olb rlcontroller.obj
$ ! lib/replace [-.obj]rllib.olb rlcorba.obj
$ lib/replace [-.obj]rllib.olb rlcutil.obj
$ lib/replace [-.obj]rllib.olb rldataprovider.obj
$ lib/replace [-.obj]rllib.olb rleibnetip.obj
$ lib/replace [-.obj]rllib.olb rlevent.obj
$ lib/replace [-.obj]rllib.olb rleventlogserver.obj
$ lib/replace [-.obj]rllib.olb rlfifo.obj
$ lib/replace [-.obj]rllib.olb rlfileload.obj
$ lib/replace [-.obj]rllib.olb rlhistorylogger.obj
$ lib/replace [-.obj]rllib.olb rlhistoryreader.obj
$ lib/replace [-.obj]rllib.olb rlinifile.obj
$ lib/replace [-.obj]rllib.olb rlinterpreter.obj
$ lib/replace [-.obj]rllib.olb rlmailbox.obj
$ lib/replace [-.obj]rllib.olb rlmodbusclient.obj
$ lib/replace [-.obj]rllib.olb rlmodbus.obj
$ lib/replace [-.obj]rllib.olb rlpcontrol.obj
$ lib/replace [-.obj]rllib.olb rlppiclient.obj
$ lib/replace [-.obj]rllib.olb rlserial.obj
$ lib/replace [-.obj]rllib.olb rlsharedmemory.obj
$ lib/replace [-.obj]rllib.olb rlsiemenstcpclient.obj
$ lib/replace [-.obj]rllib.olb rlsiemenstcp.obj
$ lib/replace [-.obj]rllib.olb rlsocket.obj
$ ! lib/replace [-.obj]rllib.olb rlspawn.obj
$ lib/replace [-.obj]rllib.olb rlspreadsheet.obj
$ ! lib/replace [-.obj]rllib.olb rlsvganimator.obj
$ lib/replace [-.obj]rllib.olb rlthread.obj
$ lib/replace [-.obj]rllib.olb rltime.obj
$ lib/replace [-.obj]rllib.olb rludpsocket.obj
$ lib/replace [-.obj]rllib.olb rlwthread.obj
$ lib/replace [-.obj]rllib.olb rlstring.obj
$ delete *.obj;*
$
$ set noveri
