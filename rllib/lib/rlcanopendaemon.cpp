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
#include "rlcanopendaemon.h"

/*! gets called from clientconnection thread. 
    performs a sdo_read command on the canopen interface. 
    received data is stored in an IPCMSGSTRUCT and sent back to the clientprocess */
void sdo_read(rlCanOpenDaemon *_daemonptr, 
              rlSocket* _socket, 
              IPCMSGSTRUCT* _message){
  rlCanOpenTypes cifmsg;
  _daemonptr->daemon_thread.lock();
    _daemonptr->nodes->sdo_read(_message->boardid, 
                                _message->nodeid, 
                                _message->objectindex, 
                                _message->subindex, cifmsg);
  _daemonptr->daemon_thread.unlock();
  *_message = cifmsg.createIpcMsg();
  _message->msgtype=MSG_SDO_READ; _message->transfertype=MSG_RECEIVE;
  _socket->write((void*) _message, sizeof(*_message));
}

/*! gets called from clientconnection thread. 
    performs a sdo_write command on the canopen interface. 
    received data is stored in an IPCMSGSTRUCT and sent back to the clientprocess */
void sdo_write(rlCanOpenDaemon *_daemonptr, 
               rlSocket* _socket, 
               IPCMSGSTRUCT* _message){
  rlCanOpenTypes cifmsg;
  cifmsg.getIpcMsg(*_message);
  _daemonptr->daemon_thread.lock();
    _daemonptr->nodes->sdo_write(_message->boardid, 
                                 _message->nodeid, 
                                 _message->objectindex, 
                                 _message->subindex, cifmsg);
  _daemonptr->daemon_thread.unlock();
  *_message = cifmsg.createIpcMsg();
  _message->msgtype=MSG_SDO_WRITE; _message->transfertype=MSG_RECEIVE;
  _socket->write((void*) _message, sizeof(*_message));   
}

/*! gets called from clientconnection thread. 
    performs a pdo_receive command on the canopen interface. 
    received data is stored in an IPCMSGSTRUCT and sent back to the clientprocess */
void pdo_receive(rlCanOpenDaemon *_daemonptr,
                 rlSocket* _socket, 
                 IPCMSGSTRUCT* _message){
  int returnvalue;
  rlCanOpenTypes cifmsg;
  cifmsg.getIpcMsg(*_message);
  _daemonptr->daemon_thread.lock();
    if (_message->mappingid==-1){
      returnvalue=_daemonptr->nodes->pdo_receive(_message->boardid, 
                                                _message->nodeid, 
                                                _message->pdoid, 
                                                cifmsg);
    }
    else
    {
      returnvalue=_daemonptr->nodes->pdo_receive(_message->boardid, 
                                                _message->nodeid, 
                                                _message->pdoid, 
                                                _message->mappingid, 
                                                cifmsg);  
    }
  _daemonptr->daemon_thread.unlock();
  *_message = cifmsg.createIpcMsg();
  _message->msgtype=MSG_PDO_RECEIVE; _message->transfertype=returnvalue;
  _socket->write((void*) _message, sizeof(*_message));   
}

/*! gets called from clientconnection thread. 
    performs a pdo_transmit command on the canopen interface. 
    received data is stored in an IPCMSGSTRUCT and sent back to the clientprocess */
void pdo_transmit(rlCanOpenDaemon *_daemonptr,
                  rlSocket* _socket,
                  IPCMSGSTRUCT* _message){
  int returnvalue;
  rlCanOpenTypes cifmsg;
  cifmsg.getIpcMsg(*_message);
  _daemonptr->daemon_thread.lock();
    if (_message->mappingid==-1){
      returnvalue=_daemonptr->nodes->pdo_transmit(_message->boardid, 
                                                  _message->nodeid, 
                                                  _message->pdoid, 
                                                  cifmsg);
    }
    else
    {
      returnvalue=_daemonptr->nodes->pdo_transmit(_message->boardid, 
                                                  _message->nodeid, 
                                                  _message->pdoid, 
                                                  _message->mappingid, 
                                                  cifmsg);  
    }
  _daemonptr->daemon_thread.unlock();
  *_message = cifmsg.createIpcMsg();
  _message->msgtype=MSG_PDO_TRANSMIT; _message->transfertype=returnvalue;
  _socket->write((void*) _message, sizeof(*_message));   
}

/*! gets called from clientconnection thread. 
    performs a nmt_transmit command on the canopen interface. 
    received data is stored in an IPCMSGSTRUCT and sent back to the clientprocess */
void nmt_transmit(rlCanOpenDaemon *_daemonptr, 
                  rlSocket* _socket, 
                  IPCMSGSTRUCT* _message){
  char returnvalue;
  _daemonptr->daemon_thread.lock();
    returnvalue=_daemonptr->nodes->sendNMTCommand(_message->boardid, 
                                                  _message->nodeid, 
                                                  _message->mtext[0]);
  _daemonptr->daemon_thread.unlock();
  _message->msgtype=MSG_PDO_TRANSMIT; _message->transfertype=returnvalue;
  _socket->write((void*) _message, sizeof(*_message));   
}

/*! gets called from clientconnection thread. 
    performs a restartBoard command on the canopen interface. 
    received data is stored in an IPCMSGSTRUCT and sent back to the clientprocess */
void restart_board(rlCanOpenDaemon *_daemonptr, 
                   rlSocket* _socket, 
                   IPCMSGSTRUCT* _message){
  char returnvalue;
  _daemonptr->daemon_thread.lock();
    returnvalue=_daemonptr->nodes->restartBoard(_message->boardid, 
                                                (int) _message->mtext[0]);
  _daemonptr->daemon_thread.unlock();
  _message->msgtype=MSG_PDO_TRANSMIT; _message->transfertype=returnvalue;
  _socket->write((void*) _message, sizeof(*_message));   
}

/*! gets called from clientconnection thread. 
    performs a getNodeState command on the canopen interface. 
    received data is stored in an IPCMSGSTRUCT and sent back to the clientprocess */
void getNodeState(rlCanOpenDaemon *_daemonptr, 
                  rlSocket* _socket, 
                  IPCMSGSTRUCT* _message){
  int returnvalue;
  rlCanOpenTypes cifmsg;
  cifmsg.getIpcMsg(*_message);
  _daemonptr->daemon_thread.lock();
    returnvalue=_daemonptr->nodes->getNodeState(_message->boardid,
                                                _message->nodeid, 
                                                cifmsg);
  _daemonptr->daemon_thread.unlock();
  *_message = cifmsg.createIpcMsg();
  _message->msgtype=MSG_GET_NODE_STATE; _message->transfertype=returnvalue;
  _socket->write((void*) _message, sizeof(*_message));   
}

/*! handels a new client connection established by listener thread. 
    Saves socket descriptor in new socket object. 
    Runs a loop which reads IPCMSGSTRUCTs containing client requests
    and calls appropiate handler functions which communicate with 
    the canopen device and send answers back to the client. 
    clientconnection Thread is terminated when client disconnects.
    */
static void *clientconnection(void *arg)
{
  //cout<<"\n client connection established...\n";
  
  rlCanOpenTypes cifmsg;
  THREAD_PARAM *p = (THREAD_PARAM *) arg;
  THREADTRANSFER *tr = (THREADTRANSFER*)p->user;
  IPCMSGSTRUCT message;
  int socketdescr = tr->socketdescr;
  rlCanOpenDaemon *daemonptr = tr->daemonptr;
  int ret;
  rlSocket socket(socketdescr);
  
  rlDebugPrintf("client connection established...\n");
  while(socket.isConnected())
  {
    ret = socket.read((void*) &message, sizeof(message));
    if(ret <= 0) break;
    if ((message.msgtype==MSG_SDO_READ)&&(message.transfertype==MSG_SEND)){
      sdo_read(daemonptr, &socket, &message);
    }
    if ((message.msgtype==MSG_SDO_WRITE)&&(message.transfertype==MSG_SEND)){
      sdo_write(daemonptr, &socket, &message);
    }
    if ((message.msgtype==MSG_PDO_RECEIVE)&&(message.transfertype==MSG_SEND)){
      pdo_receive(daemonptr, &socket, &message);
    }
    if ((message.msgtype==MSG_PDO_TRANSMIT)&&(message.transfertype==MSG_SEND)){
      pdo_transmit(daemonptr, &socket, &message);
    }
    if ((message.msgtype==MSG_NMT_TRANSMIT)&&(message.transfertype==MSG_SEND)){
      nmt_transmit(daemonptr, &socket, &message);
    }
    if ((message.msgtype==MSG_RESTART_BOARD)&&(message.transfertype==MSG_SEND)){
      restart_board(daemonptr, &socket, &message);
    }
    if ((message.msgtype==MSG_GET_NODE_STATE)&&(message.transfertype==MSG_SEND)){
      getNodeState(daemonptr, &socket, &message);
    }    
    
  }
  rlDebugPrintf("client disconnect...\n");
  //cout<<"\n client disconnect...\n";
  return NULL;
}


/*! listenerthread waits for incoming client connections on the 
    confugured port and spawns a new clientthread for each of them. */
static void *listenerthread(void *arg)
{
  
  // socketdescriptor;
  int socketdescr;
  // parameter struct for accessing function parameters
  THREAD_PARAM *p = (THREAD_PARAM *) arg;
  // new parameter struct for passing parameters to handler thread
  THREADTRANSFER tr;
  rlCanOpenDaemon *daemonptr = (rlCanOpenDaemon*) p->user;
  tr.daemonptr = daemonptr;
  int port = daemonptr->getPort();
  // wait for connections with accept();
  rlDebugPrintf("\n starting listener on port %d...\n", port);
  // initialize socket with daemonport
  rlSocket s("localhost",port,0);
  // listener main loop
  while(1){
    socketdescr = s.connect();
    if(socketdescr == -1) break;
    tr.socketdescr = socketdescr;
    daemonptr->daemon_thread.create(clientconnection, &tr);
    // wait for clientconnection to save socketdescriptor
    rlsleep(100); 
  }
  rlDebugPrintf("\n killing listener...\n");
  return NULL;
}



//! empty constructor initializes on port 5000.
rlCanOpenDaemon::rlCanOpenDaemon(){
  nodes = new rlCanOpen();
  port = 5000;
}

//! main constructor sets port and inifilename. 
rlCanOpenDaemon::rlCanOpenDaemon(int _port, char* _iniFileName){
  if (_iniFileName!=0){
    nodes = new rlCanOpen(_iniFileName);
  }
  else{
    nodes = new rlCanOpen();
  }
  port = _port;
  
        
}

//! destructor destroys rlcanopen object 
rlCanOpenDaemon::~rlCanOpenDaemon(){
  rlDebugPrintf("destruktor rlCanOpenDaemon\n");
  if (nodes!=0){
    delete nodes;
  }
}

//! returns value of current port 
int rlCanOpenDaemon::getPort(){
  return port;
}

//! runs service in endless loop
/*! daemon creates a listener thread which handels incoming 
    connections by himself creating clientconnection threads for 
    each client connecting to the daemon. */
void rlCanOpenDaemon::start(){
  daemon_thread.create(listenerthread, this);
  /* NO NO this has to be done by the user RL 17.06.2004
    while(1){
    rlsleep(100);
  }
  */
}  


