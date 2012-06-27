/***************************************************************************
                          rlcanopen.cpp  -  description
                             -------------------
    begin     : Tue March 03 2004
    copyright : (C) 2004 by Marc Br�tigam, Christian Wilmes, R. Lehrig
    email     : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "rlsocket.h"
#include "rlthread.h"
#include "rlcutil.h"
#include "rlwthread.h"

#include "rlcanopentypes.h"

//! Class performs API rlcanopen functions remotely through tcp sockets
/*! this class provides the client API funcions which should be 
    similar to the rlcanopen API. 
    Main difference is that underlying code does not communicate with 
    the canopen device itself but with a rlcanopendaemon, which should 
    run in backbround or on an remote computer. */ 
    
class rlCanOpenClient
{
public:

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

  //! initializes the client on localhost port 5000
  rlCanOpenClient();
  //! initializes the client on given port and remove server adress
  rlCanOpenClient(int _port, char* _remoteadress);
  //! destructor disconnects client 
  ~rlCanOpenClient();
  //! opens a new connection to a running rlCanOpenDaemon
  int connect();
  //! disconnects from daemon
  int disconnect();
  

  /*! using the sdo_read function you can read a certain object from the 
      object dictionary of a node. 
      Function will return NULL if connection to daemon broke down. 
      CanOpen interface related errors are
      accessable in _sdo_data.get_rlmsgerr() */
  int sdo_read( int _boardnr, 
                int _nodeid,int _objectindex, 
                int _subindex, 
                rlCanOpenTypes &_sdo_data); 

  /*! using the sdo_write function you can write to a certain object 
      from the object dictionary of a node 
      Function will return NULL if connection to daemon broke down. 
      CanOpen interface related errors are
      accessable in _sdo_data.get_rlmsgerr() */
  int sdo_write(int _boardnr, 
                int _nodeid,int _objectindex, 
                int _subindex, 
                rlCanOpenTypes &_sdo_data); 
  
  /*! receives single mapped pdo object from daemon. 
      Function will return NULL if connection to daemon broke down. 
      CanOpen interface related errors are 
      accessable in _sdo_data.get_rlmsgerr() */
  int pdo_receive(int _boardnr, 
                  int _nodeid,int _pdonr, 
                  int _mappingnr, 
                  rlCanOpenTypes &_pdo_data);

  /*! receives an 8 byte pdo from daemon.  
      Function will return NULL if connection to daemon broke down. 
      CanOpen interface related errors are
      accessable in _sdo_data.get_rlmsgerr() */
  int pdo_receive(int _boardnr, 
                  int _nodeid,int _pdonr, 
                  rlCanOpenTypes &_pdo_data);

  /*! sends a single mapped pdo object to daemon.  
      Function will return NULL if connection to daemon broke down. 
      CanOpen interface related errors are
      accessable in _pdo_data.get_rlmsgerr() */
  int pdo_transmit(int _boardnr, 
                   int _nodeid,int _pdonr, 
                   int _mappingnr, 
                   rlCanOpenTypes &_pdo_data);

  /*! sends an 8 byte pdo to daemon.  
      Function will return NULL if connection to daemon broke down. 
      CanOpen interface related errors are
      accessable in _pdo_data.get_rlmsgerr() */
  int pdo_transmit(int _boardnr, 
                   int _nodeid,
                   int _pdonr, 
                   rlCanOpenTypes &_pdo_data);

  /*! sends a NMT command to daemon.  */
  int sendNMTCommand(   int _boardnr, 
                        int _nodeid, 
                        unsigned char _cmd, 
                        bool &returnstate);
  
  /*! forces daemon to restart canopen device.  */
  int restartBoard(int _boardnr, int _restarttype, bool &returnstate);
  
  /*! receives node state data of particular node from daemon.  */
  int getNodeState(int _boardnr, int _nodeid,rlCanOpenTypes &_data);
  
  //! setter for private port variable
  void setPort(int _port);
  
  //! setter for private remoteadress variable  
  void setAdr(char* _adr);

private:
  //! variable contains process id 
  int pid;

  //! flag indicates connection status
  bool connected;

  //! variable contains port
  int port;

  //! string of remoteadress
  char remoteadress[40];

  //! pointer to socket object
  rlSocket* socket;
  //! timeout in ms
  int client_timeout;
};

//#####################################################################################      
// convenience class for client users
class rlCanClient : public rlCanOpenClient
{
public:
  rlCanClient(int _port, char* _remoteadress, int _boardnr);
  ~rlCanClient();
  int sdo_read(int _nodeid, int _objectindex, int _subindex, rlCanOpenTypes &_sdo_data); 
  int sdo_write(int _nodeid,int _objectindex, int _subindex, rlCanOpenTypes &_sdo_data); 
  int pdo_receive(int _nodeid, int _pdonr, int _mappingnr, rlCanOpenTypes &_pdo_data);
  int pdo_receive(int _nodeid, int _pdonr, rlCanOpenTypes &_pdo_data);
  int pdo_transmit(int _nodeid, int _pdonr, int _mappingnr, rlCanOpenTypes &_pdo_data);
  int pdo_transmit(int _nodeid, int _pdonr, rlCanOpenTypes &_pdo_data);
  int sendNMTCommand(int _nodeid, unsigned char _cmd, bool &returnstate);
  int restartBoard(int _restarttype, bool &returnstate);
  int getNodeState(int _nodeid, rlCanOpenTypes &_data);
private:
  int boardnr;  
};
