$ ucx set service pvserver /file=dka100:[lehrig]pvserver_startup.com -
                           /user=lehrig                              -
                           /protocol=tcp                             -
                           /port=5050                                -
                           /process=pvserver                         -
                           /limit=10
$ ucx enable service pvserver 
$ ucx show service pvserver /full
