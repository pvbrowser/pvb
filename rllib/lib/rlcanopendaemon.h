/***************************************************************************
                          rlcanopen.cpp  -  description
                             -------------------
    begin     : Tue March 03 2004
    copyright : (C) 2004 by Marc Bräutigam, Christian Wilmes, R. Lehrig
    email     : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#ifndef DAEMON
#define DAEMON

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rlthread.h"
#include "rlcutil.h"
#include "rlsocket.h"
#include "rlwthread.h"
#include "rlcanopen.h"


  enum rl_msg {
    MSG_SEND      = 0,
    MSG_RECEIVE   = 1,
    MSG_SDO_READ = 0,
    MSG_SDO_WRITE = 1,
    MSG_PDO_RECEIVE = 3,
    MSG_CONNECT = 5,
    MSG_DISCONNECT = 6,
    MSG_PDO_TRANSMIT = 4,
    MSG_NMT_TRANSMIT = 7,
    MSG_RESTART_BOARD = 8,
    MSG_GET_NODE_STATE = 9
  };


//! canopen tcp/ip interface for concurrent device-access of multiple clientprocesses 
/*! this is a multi threaded server process, which listens on a tcp port for incomming 
    connections. It runs the only recommended instance of rlcanopen class for beeing the 
    very only process accessing the device. 
    Different processes who concurrently try to access the device will be queued autmaticly
    by daemonprocess. These processes are inteded to communicate through rlcanopenclient.  */
   
class rlCanOpenDaemon{
  public:
  
  /*! empty constructor initializes on port 5000. 
      creates a new rlCanOpen Object which 
      performs all access to canopen device */
  rlCanOpenDaemon();                                
  
  /*! main constructor sets port and inifilename.  
      creates a new rlCanOpen Object which 
      performs all access to canopen device */
  rlCanOpenDaemon(int _port, char* _iniFileName=0);
  
  /*! destructor destroys rlcanopen object */
  ~rlCanOpenDaemon();
  
  /*! returns value of current port */
  int getPort();                      
  
  /*! daemon creates a listener thread which handels 
      incoming connections by himself creating
      clientconnection threads for each client connecting to the daemon. */
  void start();
  
  /*! pointer to rlCanOpen Object which performs all access to canopen device */
  rlCanOpen* nodes;                                                             
  
  /*! rlThread object which manages basic thread functionality e.g. 
      locking to prevent threads
      from concurrently accessing the canopen device */
  rlThread daemon_thread;  

  private:
  int port;

};

// parameter struct for passing parameters to handler thread
struct THREADTRANSFER
{
  int socketdescr; // socket descriptor identifying the new connection
  rlCanOpenDaemon* daemonptr; // pointer to daemon object
};






#endif       
