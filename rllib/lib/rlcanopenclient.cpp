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
#include "rlcanopenclient.h"

rlCanClient::rlCanClient(int _port, char* _remoteadress, int _boardnr)
            :rlCanOpenClient(_port, _remoteadress)
{
  boardnr = _boardnr;
  rlCanOpenClient::connect();
}

rlCanClient::~rlCanClient()
{
  rlCanOpenClient::disconnect();
}

int rlCanClient::sdo_read(int _nodeid, int _objectindex, int _subindex, rlCanOpenTypes &_sdo_data) 
{
  int ret = rlCanOpenClient::sdo_read(boardnr, _nodeid, _objectindex, _subindex, _sdo_data);
  if(ret != 1) rlCanOpenClient::connect();
  return ret;
}

int rlCanClient::sdo_write(int _nodeid,int _objectindex, int _subindex, rlCanOpenTypes &_sdo_data) 
{
  int ret = rlCanOpenClient::sdo_write(boardnr, _nodeid, _objectindex, _subindex, _sdo_data);
  if(ret != 1) rlCanOpenClient::connect();
  return ret;
}

int rlCanClient::pdo_receive(int _nodeid, int _pdonr, int _mappingnr, rlCanOpenTypes &_pdo_data)
{
  int ret = rlCanOpenClient::pdo_receive(boardnr, _nodeid, _pdonr, _mappingnr, _pdo_data);
  if(ret != 1) rlCanOpenClient::connect();
  return ret;
}

int rlCanClient::pdo_receive(int _nodeid, int _pdonr, rlCanOpenTypes &_pdo_data)
{
  int ret = rlCanOpenClient::pdo_receive(boardnr, _nodeid, _pdonr, _pdo_data);
  if(ret != 1) rlCanOpenClient::connect();
  return ret;
}

int rlCanClient::pdo_transmit(int _nodeid, int _pdonr, int _mappingnr, rlCanOpenTypes &_pdo_data)
{
  int ret = rlCanOpenClient::pdo_receive(boardnr, _nodeid, _pdonr, _mappingnr, _pdo_data);
  if(ret != 1) rlCanOpenClient::connect();
  return ret;
}

int rlCanClient::pdo_transmit(int _nodeid, int _pdonr, rlCanOpenTypes &_pdo_data)
{
  int ret = rlCanOpenClient::pdo_receive(boardnr, _nodeid, _pdonr, _pdo_data);
  if(ret != 1) rlCanOpenClient::connect();
  return ret;
}

int rlCanClient::sendNMTCommand(int _nodeid, unsigned char _cmd, bool &returnstate)
{
  int ret = rlCanOpenClient::sendNMTCommand(boardnr, _nodeid, _cmd, returnstate);
  if(ret != 1) rlCanOpenClient::connect();
  return ret;
}

int rlCanClient::restartBoard(int _restarttype, bool &returnstate)
{
  int ret = rlCanOpenClient::restartBoard(boardnr, _restarttype, returnstate);
  if(ret != 1) rlCanOpenClient::connect();
  return ret;
}

int rlCanClient::getNodeState(int _nodeid, rlCanOpenTypes &_data)
{
  int ret = rlCanOpenClient::getNodeState(boardnr, _nodeid, _data);
  if(ret != 1) rlCanOpenClient::connect();
  return ret;
}

//#########################################################################################

//! initializes the client on port 5000
rlCanOpenClient::rlCanOpenClient()
{
   connected = false;   
   //cout<<"rlCanOpenClient Konstrukter erfolgreich aufgerufen\n";
   port=5000;      
   strcpy(remoteadress, "localhost");
   pid=getpid();
   client_timeout=1000;
   socket = new rlSocket(remoteadress,port,1);
}

//! initializes the client on given port and remove server adress
rlCanOpenClient::rlCanOpenClient(int _port, char* _remoteadress)
{
   connected = false;   
   //cout<<"Konstrukter erfolgreich aufgerufen\n";
   port=_port;
   pid=getpid();
   client_timeout=1000;
   strcpy(remoteadress, _remoteadress);
   socket = new rlSocket(remoteadress,port,1);
}

//! destructor disconnects client 
rlCanOpenClient::~rlCanOpenClient()
{
  //printf("Destruktor rlCanOpenClient\n");
  if (connected == true){       
    disconnect();
  }
  if (socket!=0){
    delete socket;
  }
}

//! opens a new connection to a running rlCanOpenDaemon
int rlCanOpenClient::connect()
{  
  
  socket->setAdr(remoteadress);
  socket->setPort(port);
  socket->setActive(1);
  
  socket->connect();
  connected=socket->isConnected();

  if(socket->isConnected() == 0) {
    printf("no connection to server!\n"); 
    return 0;
  }
  cout<<"connection established...\n";
  return 1;
}

//! disconnects from daemon
int rlCanOpenClient::disconnect()
{
  if (socket!=NULL){
    printf("disconnection...\n");
    socket->disconnect();
    connected=false;
  }
  return 1;     
}


//! remotely reads a certain object from the object dictionary of a node. 
int rlCanOpenClient::sdo_read(int _boardnr, 
                              int _nodeid,
                              int _objectindex, 
                              int _subindex, 
                              rlCanOpenTypes &_sdo_data)
{
  int ret;
  IPCMSGSTRUCT message;
  if (connected){
    message.msgtype = MSG_SDO_READ;
    message.transfertype = MSG_SEND;
    message.processid = pid;
    message.boardid = _boardnr;          
    message.nodeid=_nodeid;     
    message.objectindex=_objectindex;
    message.subindex=_subindex;   
    
    if(socket->isConnected() == 0)
      if ( connect() == 0 )
        return 0;
    
    socket->write((void*) &message, sizeof(message));         
    
    ret = socket->read((void*) &message,sizeof(message),client_timeout);
    if(ret <= 0)  // timeout
    {
      return 0;
    }
    _sdo_data.getIpcMsg(message);
  }
  else
  {
    return 0;
  }
  return 1; 
}

//! remotely writes data into a certain object from the object dictionary of a node
int rlCanOpenClient::sdo_write(int _boardnr, 
                               int _nodeid,
                               int _objectindex, 
                               int _subindex, 
                               rlCanOpenTypes &_sdo_data)
{
  int ret;
  IPCMSGSTRUCT message;
  if (connected){
    message = _sdo_data.createIpcMsg();
    message.msgtype = MSG_SDO_WRITE;
    message.transfertype = MSG_SEND;
    message.processid = pid;
    message.boardid = _boardnr;          
    message.nodeid=_nodeid;     
    message.objectindex=_objectindex;
    message.subindex=_subindex;   

    if(socket->isConnected() == 0)
      if ( connect() == 0 )
        return 0;

    
    socket->write((void*) &message, sizeof(message));         
    
    // recieve answer to check if write was successfull
    ret = socket->read((void*) &message,sizeof(message),client_timeout);
    if(ret <= 0)  // timeout
    {
      return 0;
    }
    _sdo_data.getIpcMsg(message);
  }
  else
  {
    return 0;
  }
  return 1; 
}

//! receives single mapped pdo object from daemon. 
int rlCanOpenClient::pdo_receive(int _boardnr, 
                                 int _nodeid,
                                 int _pdonr, 
                                 int _mappingnr, 
                                 rlCanOpenTypes &_pdo_data)
{
  int ret;
  IPCMSGSTRUCT message;
  if (connected){
    message.msgtype = MSG_PDO_RECEIVE;
    message.transfertype = MSG_SEND;
    message.processid = pid;
    message.boardid = _boardnr;          
    message.nodeid=_nodeid;     
    message.objectindex=0;
    message.subindex=0;  
    message.pdoid=_pdonr;
    message.mappingid=_mappingnr;  

    if(socket->isConnected() == 0)
      if ( connect() == 0 )
        return 0;

    socket->write((void*) &message, sizeof(message));         
    ret = socket->read((void*) &message,sizeof(message),client_timeout);
    if(ret <= 0)  // timeout
    {
      return 0;
    }
    _pdo_data.getIpcMsg(message);
  }
  else
  {
    return 0;
  }
  return 1;  
}

//!  receives an 8 byte pdo from daemon.  
int rlCanOpenClient::pdo_receive(int _boardnr, 
                                 int _nodeid,
                                 int _pdonr, 
                                 rlCanOpenTypes &_pdo_data)
{
  int ret = pdo_receive(_boardnr, _nodeid, _pdonr, -1, _pdo_data);
  return ret;
}


//! sends a single mapped pdo object to daemon.  
int rlCanOpenClient::pdo_transmit(int _boardnr, 
                                  int _nodeid,
                                  int _pdonr, 
                                  int _mappingnr, 
                                  rlCanOpenTypes &_pdo_data)
{
  int ret;
  IPCMSGSTRUCT message;
  if (connected){
    message = _pdo_data.createIpcMsg(); 
    message.msgtype = MSG_PDO_TRANSMIT;
    message.transfertype = MSG_SEND;
    message.processid = pid;
    message.boardid = _boardnr;          
    message.nodeid=_nodeid;     
    message.objectindex=0;
    message.subindex=0;  
    message.pdoid=_pdonr;
    message.mappingid=_mappingnr;  

    if(socket->isConnected() == 0)
      if ( connect() == 0 )
        return 0;

    socket->write((void*) &message, sizeof(message));         
    ret = socket->read((void*) &message,sizeof(message),client_timeout);
    if(ret <= 0)  // timeout
    {
      return 0;
    }
    _pdo_data.getIpcMsg(message);
  }
  else
  {
    return 0;
  }
  return 1;  
}


//! sends an 8 byte pdo to daemon.  
int rlCanOpenClient::pdo_transmit(int _boardnr, 
                                  int _nodeid,
                                  int _pdonr, 
                                  rlCanOpenTypes &_pdo_data)
{
  return pdo_transmit(_boardnr, _nodeid, _pdonr, -1, _pdo_data);
}

//!  sends a NMT command to daemon.
int rlCanOpenClient::sendNMTCommand(int _boardnr, 
                                    int _nodeid, 
                                    unsigned char _cmd, 
                                    bool &returnstate){
  int ret;
  IPCMSGSTRUCT message;
  if (connected){
    message.mtext[0] = _cmd;    
    message.msgtype = MSG_NMT_TRANSMIT;
    message.transfertype = MSG_SEND;
    message.processid = pid;
    message.boardid = _boardnr;          
    message.nodeid=_nodeid;     
    message.objectindex=0;
    message.subindex=0;  
    message.pdoid=0;
    message.mappingid=0;  

    if(socket->isConnected() == 0)
      if ( connect() == 0 )
        return 0;

    socket->write((void*) &message, sizeof(message));         
    ret = socket->read((void*) &message,sizeof(message),client_timeout);
    if(ret <= 0)  // timeout
    {
      return 0;
    }
  }
  else
  {
    return 0;
  }
  returnstate = (bool) message.transfertype;
  return 1;  
}


//! forces daemon to restart canopen device.
int rlCanOpenClient::restartBoard(int _boardnr, 
                                  int _restarttype, 
                                  bool &returnstate){
  int ret;
  IPCMSGSTRUCT message;
  if (connected){
    message.mtext[0] = (char) _restarttype;     
    message.msgtype = MSG_RESTART_BOARD;
    message.transfertype = MSG_SEND;
    message.processid = pid;
    message.boardid = _boardnr;          
    message.nodeid=0;     
    message.objectindex=0;
    message.subindex=0;  
    message.pdoid=0;
    message.mappingid=0;  

    if(socket->isConnected() == 0)
      if ( connect() == 0 )
        return 0;

    socket->write((void*) &message, sizeof(message));         
    ret = socket->read((void*) &message,sizeof(message),client_timeout);
    if(ret <= 0)  // timeout
    {
      return 0;
    }
  }
  else
  {
    return 0;
  }
  returnstate = (bool) message.transfertype;
  return 1;  
}

//! receives node state data of particular node from daemon.
int rlCanOpenClient::getNodeState(int _boardnr, 
                                  int _nodeid,
                                  rlCanOpenTypes &_data){
  int ret;
  IPCMSGSTRUCT message;
  if (connected){
    message = _data.createIpcMsg();             
    message.msgtype = MSG_GET_NODE_STATE;
    message.transfertype = MSG_SEND;
    message.processid = pid;
    message.boardid = _boardnr;          
    message.nodeid=_nodeid;     
    message.objectindex=0;
    message.subindex=0;  
    message.pdoid=0;
    message.mappingid=0;  

    if(socket->isConnected() == 0)
      if ( connect() == 0 )
        return 0;

    socket->write((void*) &message, sizeof(message));         
    ret = socket->read((void*) &message,sizeof(message),client_timeout);
    if(ret <= 0)  // timeout
    {
      return 0;
    }
    _data.getIpcMsg(message);
  }
  else
  {
    return 0;
  }
  return 1;  
}

void rlCanOpenClient::setPort(int _port){
  port = _port;
}
  
void rlCanOpenClient::setAdr(char* _adr){
  strcpy(remoteadress, _adr); 
}
