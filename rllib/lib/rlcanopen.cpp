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

//class to manage the nodes in a CanOpen network

#include "rlcutil.h"
#include "rlcanopen.h"

#ifdef RLWIN32
#include <windows.h>
#include <math.h>
#endif

#include <time.h>

rlCanOpen::rlCanOpen(char * _inifilename)
{
  rlDebugPrintf("Constructer nodes class\n");
  read_inifile(_inifilename);
  if (enableLogging){
    freopen (logFileName.ascii(),"w",stdout); // redirekt stdout to textfile
  }
  ini();
}


rlCanOpen::rlCanOpen()
{
  rlDebugPrintf("Constructer nodes class\n");
  read_inifile("./canopen.ini");
  if (enableLogging){
    FILE* fp;
    fp = freopen (logFileName.ascii(),"w",stdout); // redirekt stdout to textfile
    
    /* enable linebuffering in order to write every line to the file immediately */
    setvbuf ( fp , NULL , _IOFBF , 1024 ); 
  }
  ini();
}
//---------------------------------------------------------------------------
rlCanOpen::~rlCanOpen()
{
  rlDebugPrintf("destruktor rlCanOpen\n");
  for (int i=0;i<boardcount;i++)
    DevExitBoard(i);

#ifdef RLWIN32
  DevCloseDriver(0);
#else
  DevCloseDriver();
#endif    
  if (enableLogging)    
    fclose (stdout);
  
}
//---------------------------------------------------------------------------
//! initialize nodes and create nodelist
bool rlCanOpen::ini()
{
  messagenr = 0;
  nodelist.setAutoDelete(true);

  time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

  sprintf(err_out_buf, "\nnew session startet: %s\n", asctime(timeinfo));
  rlDebugPrintf(err_out_buf);
 
  if (iniboards()){
    rlDebugPrintf("at least one board initialize!\n");
  }
  else
  {
    rlDebugPrintf("no board available or no board could be initialized correctly!\n");
    rlDebugPrintf("Programm -> Close\n");
    exit(0); 
  }
  
  // search for active nodes and create them
  
  createNodes();
  
 
  return 1;
}
//---------------------------------------------------------------------------
//! Search for nodes in all boards and append it into the nodelist
int rlCanOpen::createNodes()
{
 int nodeid;
  for (int boardid = 0;boardid<boardcount;boardid++)
  {
    setBoardActive(boardid);
    for(nodeid = 0; nodeid <= MAX_NODES; nodeid++)  // from node 0..127
      if (getNodeConfiguration(nodeid)) // writes received node-configuration 
                                        // into class-global message-struct
      {
        // new node found. insert it into the nodelist
        nodelist.resize( nodelist.size() + 1 );   
        
        // append the new node in the list
        nodelist.insert( nodelist.size()-1, 
                         new rlCanNode(boardid,nodeid,message));
        
       //append Mapping data                                  
        if (nodelist[nodelist.size()-1]->hasMapping()){
          if(!createMappingObjects(boardid,nodeid,DIRECTION_TRANSMIT))return 0;
          if(!createMappingObjects(boardid,nodeid,DIRECTION_RECEIVE))return 0;
        }
        else
        {
          rlDebugPrintf("WARNING: no Mapping available on node %d.", nodeid);
          rlDebugPrintf("Probably EDS file couldn't be found in EDS directory.\n");
        }
      }             
  }
  return 1;
}

//------------------------------------------------------------------------------
//! creates the mappings for each pdo
int rlCanOpen::createMappingObjects(int _boardid,int _nodeid,int _pdoDirection)
{
  int pdomax = 0;
  if (_pdoDirection == DIRECTION_RECEIVE)
    pdomax = nodelist[nodelist.size()-1]->getReceivePdoCount();
  else if(_pdoDirection == DIRECTION_TRANSMIT)
    pdomax = nodelist[nodelist.size()-1]->getTransmitPdoCount();
    
  for (int j = 1;j<=pdomax;j++)    //each receive/transmit pdo
  if(refreshMappingList(_boardid,_nodeid,j,_pdoDirection)==0) return 0;
  return 1;

}   


//---------------------------------------------------------------------------
//! Read informations about installed devices
bool rlCanOpen::iniboards()
{
  
  int currentboardcount=0;
  
  BOARD_INFO tBoardInfo;

#ifdef RLWIN32
    if ( (sRet = DevOpenDriver(0)) == DRV_NO_ERROR) 
#else
    if ( (sRet = DevOpenDriver()) == DRV_NO_ERROR) 
#endif    
  {
    // Driver successfully opend, read board information
#ifdef RLWIN32
    if ( (sRet = DevGetBoardInfo( 0, sizeof(tBoardInfo), &tBoardInfo)) != DRV_NO_ERROR) 
#else
    if ( (sRet = DevGetBoardInfo( &tBoardInfo)) != DRV_NO_ERROR) 
#endif    
          {
          // Function error
            rlDebugPrintf( "DevGetBoardInfo RetWert = %5d \n", sRet );
          } else 
          {
            // Information successfully read, save for further use
            // Check out which boards are available
            for ( int usIdx = 0; usIdx < MAX_DEV_BOARDS; usIdx++)
            {
              if ( tBoardInfo.tBoard[usIdx].usAvailable == TRUE) {
              // Board is configured, try to init the board
                sRet = DevInitBoard(
#ifdef RLWIN32
                tBoardInfo.tBoard[usIdx].usBoardNumber, NULL);
#else
                tBoardInfo.tBoard[usIdx].usBoard);
#endif    
                if ( sRet != DRV_NO_ERROR) {
                  // Function error
                  rlDebugPrintf( "DevInitBoard RetWert = %5d \n", sRet );
                } else {
                // DEVICE is available and ready............
                currentboardcount++;
                       }
              }
            }
          }
   }
  boardcount = currentboardcount;       
 
  if (currentboardcount!=0)
    return 1;   
  else
   return 0;    
}

//---------------------------------------------------------------------------
//! returns the current count of available CanOpenMaster cards
int rlCanOpen::getBoardCount()
{
  return boardcount;
}  
//---------------------------------------------------------------------------
//* this function reads out the configuration of a specific node
bool rlCanOpen::getNodeConfiguration(int _nodeID)
{
  //build message       
  message.rx            = CAN_TASK; 
  message.tx            = USER_AT_HOST;
  message.ln            = 8;
  message.nr            =messagenr++; 
  message.a             =0; 
  message.f             =0;
  message.b             =CMDCODE_GET_CONFIGURATION;
  message.e             =0x00;  //fragmentation
  message.device_adr    =_nodeID;  
  message.data_area     =0; 
  message.data_adr      =0; 
  message.data_idx      =0; 
  message.data_cnt      =0; 
  message.data_type     =0; 
  message.function      =0;
  
  //put configuration request
  DevPutMessage(0, (MSG_STRUC *)&message,5000L) ;  
  
  //receive answer message
  sRet = DevGetMessage (  0,                       ///Device Number            
                          sizeof(message),      
                          (MSG_STRUC *)&message,   /// Pointer to user buffer
                          2000L );                 /// Timeout  

  unsigned char msgnrlocal = messagenr -1;
  
  // receive correct answer 
  if((message.a == RESPONSE_CODE_GET_CONFIGURATION) && (message.nr == msgnrlocal))
    if (message.f == ERRCODE_NOERROR) // node active, no error
      if(message.e == 0x00) //configuration fits in the message
      {
        sprintf(err_out_buf, "Board %d node %d is active. No problems found\n", 
          getActiveBoard(), _nodeID);
        
        rlDebugPrintf(err_out_buf);
        return 1;
      }
      else
      {
         sprintf(err_out_buf, "Board %d node %d is active. \n", 
           getActiveBoard(), _nodeID);
           
         rlDebugPrintf(err_out_buf);
         rlDebugPrintf("Configuration data does not fit in one message,\n");
         rlDebugPrintf("so the current node does not include the complete data\n");
         rlDebugPrintf(" - multiplex configration data is not yet implemented!!! -\n");
         return 1;
      } 
    else return 0;
  else 
  {
    sprintf(err_out_buf, "---->message.a=%d, message.nr=%d unsere msg nr: %d\n", 
      message.a, message.nr, msgnrlocal);
      
    rlDebugPrintf(err_out_buf);
    sprintf(err_out_buf, "%s\n", "Communication ERROR!!!");
    rlDebugPrintf(err_out_buf);
    return 0;
  }
}


//---------------------------------------------------------------------------
//! sets a desired CanOpenMaster active, so that it can be used
short rlCanOpen::setBoardActive(int _boardnr)
{
                        
  if ( (sRet = DevSetHostState( _boardnr, HOST_READY, 0L) == DRV_NO_ERROR) )   
    activeboard = _boardnr;
 
  return sRet;
}

//---------------------------------------------------------------------------
//! returns the currently active board
int rlCanOpen::getActiveBoard()
{
  return activeboard;
}                                                                                                                                                                                

//---------------------------------------------------------------------------
//! prints out the configuration of a specific node
bool rlCanOpen::showNodeConfiguration(int _boardnr,int _nodeid)
{
  for( unsigned int i = 0; i < nodelist.size(); i++)
  {
    if ((nodelist[i]->getNodeID() == _nodeid) && 
        (nodelist[i]->getBoardID() == _boardnr))
    {
      nodelist[i]->showConfiguration();
      return true;
      
    }           
  }

  return false; 
} 
//---------------------------------------------------------------------------
//! prints out the configuration off all available nodes from all boards
bool rlCanOpen::showNodeConfiguration()
{
  if(nodelist.size() == 0) return 0;
  for(unsigned int i = 0; i < nodelist.size(); i++)
  {
    nodelist[i]->showConfiguration();   
  }
   return 1;
}
//---------------------------------------------------------------------------

//! read a certain object from the object dictionary of a node
int rlCanOpen::sdo_read( int _boardnr, 
                         int _nodeid,
                         int _objectindex, 
                         int _subindex, 
                         rlCanOpenTypes &_sdo_data)
{
    int datalength;
    int canopentype;
    int nodeindex; 
    
    if (getNodeIndex(_boardnr,_nodeid,nodeindex)== 0) 
    {
      rlDebugPrintf("ERROR: specified node does not exist! SDO write failed!");
      rlDebugPrintf("BoardId: %d NodeId: %d Objektindex: %d Subindex: %d\n", 
        _boardnr, _nodeid, _objectindex, _subindex);
        
      _sdo_data.set_rlmsgerr(ERRCODE_INVALID_NODEID);
      return 0;
    }


    delmsg();
    setBoardActive(_boardnr);
    
    message.rx          = CAN_TASK; 
    message.tx          = USER_AT_HOST;
    message.ln          = 8;
    message.nr          = messagenr++; 
    message.a           = 0; 
    message.f           = 0;
    message.b           = CMDCODE_SDO_UPDOWNLOAD;
    message.e           = 0;
    message.device_adr  = _nodeid;  
    message.data_area   = 0; 
    message.data_adr    = _objectindex; 
    message.data_idx    = _subindex; 
    message.data_cnt    = 0; 
    message.data_type   = 0; 
    message.function    = TASK_TFC_READ; 
 
   if(DevPutMessage(0, (MSG_STRUC *)&message,5000L) != DRV_NO_ERROR) return 0;
   
   delmsg();
   // receive the answer
  
   sRet = DevGetMessage( getActiveBoard(),                       // Device Number            
                         sizeof(message), 
                         (MSG_STRUC *)&message,   // Pointer to user buffer  
                         2000L );                 // Timeout   
   unsigned char msgnrlocal;
   msgnrlocal = messagenr - 1;                       
   
   // receive correct answer   
   if((message.a == CMDCODE_SDO_UPDOWNLOAD)&&(message.nr == msgnrlocal)){ 
      _sdo_data.set_rlmsgerr(message.f);
      if (message.f == ERRCODE_NOERROR) // node active, no error
      {
        canopentype=getObjectType(_boardnr, _nodeid, _objectindex, _subindex);
        if (canopentype!=rlCanOpenTypes::RL_NOTYPE){
          _sdo_data.set_CanOpenType(canopentype);
          datalength = _sdo_data.getLength();
        }
        else 
        {
          _sdo_data.set_DaemonType(rlCanOpenTypes::RL_BUFFER);
          datalength = MAX_SDO_BUFFERSIZE;
        }
        _sdo_data.clearBuffer();   
        for(int i = 0; i < datalength; i++)
          _sdo_data.set_buffer(i, message.d[i]);         
        
        // if CanOpen-Type is a twistet integer (etc. 52Bit ) the value will be converted
        if ((is_twisted_type(canopentype)) && (nodelist[nodeindex]->hasMapping()))
          _sdo_data.translate_CanOpenType(canopentype);
        
        return 1;
      }
      else
      {
        rlDebugPrintf("message.f = %d  message.f = %x Hex \n", message.f, message.f);
      }
    }
    else
    {
      rlDebugPrintf("message.a = %d message.nr = %d vergleich mit: %d\n", 
         message.a, message.nr, msgnrlocal);
         
      _sdo_data.set_rlmsgerr(-1);
    }
    rlDebugPrintf("ERROR during read SDO message!");
    rlDebugPrintf("BoardId: %d NodeId: %d Obj: %x Sub: %x\n", 
      _boardnr, _nodeid, _objectindex, _subindex);
      
      
    return 0;
}
//---------------------------------------------------------------------------

//! write data into a certain object from the object dictionary of a node
int rlCanOpen::sdo_write( int _boardnr, 
                          int _nodeid,
                          int _objectindex, 
                          int _subindex, 
                          rlCanOpenTypes &_sdo_data)
{
    int length;
    int pdoid;
    length = _sdo_data.getLength();
    int nodeindex;
    if (getNodeIndex(_boardnr,_nodeid,nodeindex)== 0) 
    {
      rlDebugPrintf("ERROR: specified node does not exist! SDO write failed!");
      rlDebugPrintf("BoardId: %d NodeId: %d Objektindex: %d Subindex: %d\n", 
        _boardnr, _nodeid, _objectindex, _subindex);
      
      _sdo_data.set_rlmsgerr(ERRCODE_INVALID_NODEID);
      return 0;
    }
    
    // if CanOpen-Type is a twistet integer (etc. 52Bit ) the datalength must be adjusted
    int canopentype = getObjectType(_boardnr, _nodeid, _objectindex, _subindex);
   
    if ((is_twisted_type(canopentype)) && (nodelist[nodeindex]->hasMapping()))
      length = _sdo_data.get_CanOpenTypeLength(canopentype);
   
     for(int i = 0;i<length;i++)
        message.d[i] = _sdo_data.get_buffer(i);

    
    setBoardActive(_boardnr);
    
    message.rx          = CAN_TASK; 
    message.tx          = USER_AT_HOST;
    message.ln          = 8+ length;
    message.nr          = messagenr++; 
    message.a           = 0; 
    message.f           = 0;
    message.b           = CMDCODE_SDO_UPDOWNLOAD;
    message.e           = 0;
    message.device_adr  = _nodeid;  
    message.data_area   = 0; 
    message.data_adr    = _objectindex; 
    message.data_idx    = _subindex; 
    message.data_cnt    = length; 
    message.data_type   = 0; 
    message.function    = TASK_TFC_WRITE; 
    
    if(DevPutMessage(0, (MSG_STRUC *)&message,5000L) != DRV_NO_ERROR) return 0;
         
    
  // receive the answer
  
  sRet = DevGetMessage ( _boardnr,                // Device Number            
                         sizeof(message), 
                         (MSG_STRUC *)&message,   // Pointer to user buffer  
                         2000L );                 // Timeout   
   
   unsigned char msgnrlocal = messagenr - 1;
   
   // receive correct answer   
   if((message.a == CMDCODE_SDO_UPDOWNLOAD)&&(message.nr == msgnrlocal)){  
      _sdo_data.set_rlmsgerr(message.f);
      if (message.f == ERRCODE_NOERROR) // node active, no error
      {
        //check if the mapping has changed
        if( (_objectindex >= TRANSMIT_PDO_MAPPING_STARTADRESS) &&
            (_objectindex <= TRANSMIT_PDO_MAPPING_MAXADRESS) && 
            (nodelist[nodeindex]->hasMapping()) )
        { 
          rlDebugPrintf("Mapping has changed: boardid %d, nodeid %d, objectindex %x\n", 
            _boardnr, _nodeid, _objectindex);
            
          pdoid = getPdoID(_boardnr, _nodeid, _objectindex, DIRECTION_TRANSMIT);
          refreshMappingList(_boardnr,_nodeid,pdoid,DIRECTION_TRANSMIT);
        }
        else if ( (_objectindex >= RECEIVE_PDO_MAPPING_STARTADRESS) && 
                  (_objectindex <= RECEIVE_PDO_MAPPING_MAXADRESS) &&
                  (nodelist[nodeindex]->hasMapping()) )
        {
          rlDebugPrintf("Mapping has changed: boardid %d, nodeid %d, objectindex %x\n", 
            _boardnr, _nodeid, _objectindex);
          pdoid = getPdoID(_boardnr, _nodeid, _objectindex, DIRECTION_RECEIVE);
          refreshMappingList(_boardnr,_nodeid,pdoid,DIRECTION_RECEIVE);
        }
        return 1;
      }
      else
      {
        sprintf(err_out_buf, "message.f = %d  message.f = %x Hex \n", 
          message.f, message.f);
          
        rlDebugPrintf(err_out_buf);
        _sdo_data.set_rlmsgerr(message.f);
      }
    }
    else
    {
      _sdo_data.set_rlmsgerr(-1);
      sprintf(err_out_buf, "message.a = %d message.nr = %d vergleich mit: %d\n", 
        message.a, message.nr, msgnrlocal);
        
      rlDebugPrintf(err_out_buf);
    }
    rlDebugPrintf("ERROR during read SDO message!");
    rlDebugPrintf("BoardId: %d NodeId: %d Obj: %x Sub: %x\n", 
      _boardnr, _nodeid, _objectindex, _subindex);
      
    rlDebugPrintf(err_out_buf);
    return 0;
}
//---------------------------------------------------------------------------
//!using the pdo_receive function you can receive a mapped object within PDO 
int rlCanOpen::pdo_receive( int _boardnr, 
                            int _nodeid,
                            int _pdonr, 
                            int _mappingnr, 
                            rlCanOpenTypes &_pdo_data)
{
 
 int nodeindex;
 unsigned char buffer[8];
 
 if (getNodeIndex(_boardnr,_nodeid,nodeindex)== 0) 
 {
   rlDebugPrintf("ERROR: specified node does not exist! PDO receive failed!");
   rlDebugPrintf("BoardId: %d NodeId: %d PDO ID: %d Mappingnr: %d\n", 
     _boardnr, _nodeid, _pdonr, _mappingnr);
     
   _pdo_data.set_rlmsgerr(ERRCODE_INVALID_NODEID);
   return 0;
 }
 if (nodelist[nodeindex]->pdoList[DIRECTION_TRANSMIT].size() < (unsigned int) _pdonr){
   rlDebugPrintf("ERROR: specified PDO ID does not exist! PDO receive failed!");
   rlDebugPrintf("BoardId: %d NodeId: %d PDO ID: %d Mappingnr: %d\n", 
     _boardnr, _nodeid, _pdonr, _mappingnr);
   
   _pdo_data.set_rlmsgerr(ERRCODE_INVALID_PDOID);
   return 0;
 }
 else if (nodelist[nodeindex]->pdoList[DIRECTION_TRANSMIT][_pdonr -1]->mappingList.size() 
          < (unsigned int) _mappingnr){
   rlDebugPrintf("ERROR: specified Mappingnr does not exist! PDO receive failed!");
   rlDebugPrintf("BoardId: %d NodeId: %d PDO ID: %d Mappingnr: %d\n", 
     _boardnr, _nodeid, _pdonr, _mappingnr);
     
   _pdo_data.set_rlmsgerr(ERRCODE_INVALID_MAPID);
   return 0;
 }      
 
 // PDO No.1 has the index 0 !!!
 int pdosize = nodelist[nodeindex]->pdoList[DIRECTION_TRANSMIT][_pdonr-1]->bPDOSize;      
 int pdooffset = nodelist[nodeindex]->pdoList[DIRECTION_TRANSMIT][_pdonr-1]->usPDOAddress; 
 
 //cout<<"size ="<<pdosize<<" offset = "<<pdooffset<<"\n";
 
 if ( (sRet =  DevExchangeIO( _boardnr,   // Board NO
                      0,                  //offset send data
                      0,                  //Length of the send IO data
                      NULL,               //Pointer to the user send data buffer
                      pdooffset,          //Byte offset in the receive IO data area
                      pdosize,            //Length of the receive IO data
                      &buffer[0],         //Pointer to the user read data buffer
                      100L)               //timeout in milliseconds
      ) == DRV_NO_ERROR)
 {
   
   int length_in_byte = nodelist[nodeindex]->
         pdoList[DIRECTION_TRANSMIT][_pdonr-1]->mappingList[_mappingnr-1]->length/8;
           
   int position_in_byte = nodelist[nodeindex]->
         pdoList[DIRECTION_TRANSMIT][_pdonr-1]->mappingList[_mappingnr-1]->position/8; 
         
   _pdo_data.clearBuffer();
   
   for(int i = position_in_byte; i< length_in_byte+position_in_byte;i++)
   {
        _pdo_data.set_buffer(i-position_in_byte,buffer[i]);
   }
        
   _pdo_data.set_DaemonType( nodelist[nodeindex]->pdoList[DIRECTION_TRANSMIT][_pdonr-1]->
      		mappingList[_mappingnr-1]->etype);
     
       
   _pdo_data.set_rlmsgerr(ERRCODE_NOERROR);
   
   
    // if CanOpen-Type is a twistet integer (etc. 52Bit ) the value will be converted
   int canopentype = nodelist[nodeindex]->pdoList[DIRECTION_TRANSMIT][_pdonr-1]->
      		mappingList[_mappingnr-1]->canopentype;
      		 
   if (is_twisted_type(canopentype))
     _pdo_data.translate_CanOpenType(canopentype);   
   
   return 1;  
 }
 rlDebugPrintf("ERROR: PDO receive failed!");
 rlDebugPrintf("BoardId: %d NodeId: %d PDO ID: %d Mappingnr: %d\n", 
   _boardnr, _nodeid, _pdonr, _mappingnr);
 
 _pdo_data.set_rlmsgerr(ERRCODE_PDORCV_FAIL);
 
  return 0;     
}
//---------------------------------------------------------------------------
 //!using the pdo_receive function you can receive a PDO (Process Data Object) 
int rlCanOpen::pdo_receive( int _boardnr, 
                            int _nodeid,
                            int _pdonr, 
                            rlCanOpenTypes &_pdo_data)
{
   
   int nodeindex;
   if (getNodeIndex(_boardnr,_nodeid,nodeindex)== 0) 
   {
     sprintf(err_out_buf, "ERROR: specified node does not exist! PDO \
                           receive failed! BoardId: %d NodeId: %d PDO \
                           ID: %d\n", _boardnr, _nodeid, _pdonr);
     rlDebugPrintf(err_out_buf);
     
     //cout<<"ERROR: specified node does not exist!\n";
     _pdo_data.set_rlmsgerr(ERRCODE_INVALID_NODEID);
     return 0;
   }
   
   if (nodelist[nodeindex]->pdoList[DIRECTION_TRANSMIT].size() < (unsigned int) _pdonr){
     sprintf(err_out_buf, "ERROR: specified PDO ID does not exist! \
                           PDO receive failed! BoardId: %d NodeId: \
                           %d PDO ID: %d\n", _boardnr, _nodeid, _pdonr);
     rlDebugPrintf(err_out_buf);

     //cout<<"PDO ID does not exists!\n";
     _pdo_data.set_rlmsgerr(ERRCODE_INVALID_PDOID);
     return 0;
   }
   
   int pdosize = nodelist[nodeindex]->pdoList[DIRECTION_TRANSMIT][_pdonr-1]->bPDOSize;
   
   int pdooffset = nodelist[nodeindex]->
                                pdoList[DIRECTION_TRANSMIT][_pdonr-1]->usPDOAddress;
   
   //cout<<"\nsize ="<<pdosize<<" offset = "<<pdooffset;
     
   if ( (sRet =  DevExchangeIO( _boardnr, // Board NO
                  0,                      //offset send data
                  0,                      //Length of the send IO data
                  NULL,                   //Pointer to the user send data buffer
                  pdooffset,              //Byte offset in the receive IO data area
                  pdosize,                //Length of the receive IO data
                  &_pdo_data.pdobuffer[0],//Pointer to the user read data buffer
                  100L)                   //timeout in milliseconds
        ) == DRV_NO_ERROR)
   {
     _pdo_data.pdobuf2buf();
     //rlDebugPrintf("PDO recieve successfull!\n");
     _pdo_data.set_rlmsgerr(ERRCODE_NOERROR);
     return 1;
   
   }
   sprintf(err_out_buf, "ERROR: PDO receive failed! \
                         BoardId: %d NodeId: %d PDO \
                         ID: %d\n", _boardnr, _nodeid, _pdonr);
   rlDebugPrintf(err_out_buf);

   //rlDebugPrintf("ERROR: PDO RECEIVE FAILED\n");
   _pdo_data.set_rlmsgerr(ERRCODE_PDORCV_FAIL);
     return 0;                          
}


//---------------------------------------------------------------------------
//! transmit a mapped object within a PDO to a specific node 
int rlCanOpen::pdo_transmit( int _boardnr, 
                             int _nodeid,
                             int _pdonr, 
                             int _mappingnr, 
                             rlCanOpenTypes &_pdo_data)
{
   int nodeindex;
   if (getNodeIndex(_boardnr,_nodeid,nodeindex)== 0) 
   {
     sprintf(err_out_buf, "ERROR: specified node does not exist! PDO transmit \
                           failed! BoardId: %d NodeId: %d PDO ID: %d Mappingnr:\
                            %d\n", _boardnr, _nodeid, _pdonr, _mappingnr);
     rlDebugPrintf(err_out_buf);
     _pdo_data.set_rlmsgerr(ERRCODE_INVALID_NODEID);
     return 0;
   }
   if (nodelist[nodeindex]->pdoList[DIRECTION_RECEIVE].size() < (unsigned int) _pdonr){
     sprintf(err_out_buf, "ERROR: specified PDO ID does not exist! PDO transmit\
                           failed! BoardId: %d NodeId: %d PDO ID: %d Mappingnr:\
                            %d\n", _boardnr, _nodeid, _pdonr, _mappingnr);
     rlDebugPrintf(err_out_buf);
        
     //cout<<"PDO ID does not exists!\n";
     _pdo_data.set_rlmsgerr(ERRCODE_INVALID_PDOID);
     return 0;
   }
   else if (nodelist[nodeindex]->pdoList[DIRECTION_RECEIVE][_pdonr -1]->mappingList.size()
              < (unsigned int) _mappingnr){
     sprintf(err_out_buf, "ERROR: specified mappingnr does not exist! PDO \
                           transmit failed! BoardId: %d NodeId: %d PDO \
                           ID: %d Mappingnr: %d\n", 
                           _boardnr, _nodeid, _pdonr, _mappingnr);
     rlDebugPrintf(err_out_buf);

     //cout<<"mapping ID does not exists!\n";
     _pdo_data.set_rlmsgerr(ERRCODE_INVALID_MAPID);
     return 0;
   }    
   
   // PDO No.1 has the index 0 !!!
   int pdosize = nodelist[nodeindex]->pdoList[DIRECTION_RECEIVE][_pdonr-1]->bPDOSize; 
   int pdooffset = nodelist[nodeindex]->pdoList[DIRECTION_RECEIVE][_pdonr-1]->usPDOAddress;

   int length_in_byte = nodelist[nodeindex]->pdoList[DIRECTION_RECEIVE][_pdonr-1]->
                          mappingList[_mappingnr-1]->length/8;
                          
   int position_in_byte = nodelist[nodeindex]->pdoList[DIRECTION_RECEIVE][_pdonr-1]->
                          mappingList[_mappingnr-1]->position/8; 
   
   if (_pdo_data.get_DaemonType() != (nodelist[nodeindex]->
               pdoList[DIRECTION_RECEIVE][_pdonr-1]->mappingList[_mappingnr-1]->etype))
   {  
     
     sprintf(err_out_buf, "ERROR: invalid type! Expected type: %d  Your Type:\
                            %d! PDO transmit failed! BoardId: %d NodeId: %d \
                            PDO ID: %d Mappingnr: %d\n", 
          nodelist[nodeindex]->pdoList[DIRECTION_RECEIVE][_pdonr-1]->
            mappingList[_mappingnr-1]->etype, 
          _pdo_data.get_DaemonType(), _boardnr, _nodeid, _pdonr, _mappingnr);
       
       
     rlDebugPrintf(err_out_buf);
     _pdo_data.get_DaemonType();
     _pdo_data.set_rlmsgerr(ERRCODE_INVALID_RLTYPE);
     return 0;
   }
   
    _pdo_data.buf2pdobuf();
   
   // no conversion necessary. correct length in mapping list

   pdooffset = pdooffset + position_in_byte;
   pdosize = length_in_byte;
   
   
   
   //cout<<"size ="<<pdosize<<" offset = "<<pdooffset<<"\n";
   
   if ( (sRet =  DevExchangeIO( _boardnr,       // Board NO
                        pdooffset,              //offset send data
                        pdosize,                //Length of the send IO data
                        &_pdo_data.pdobuffer[0],//Pointer to the user send data buffer
                        0,                      //Byte offset in the receive IO data area
                        0,                      //Length of the receive IO data
                        NULL,                   //Pointer to the user read data buffer
                        100L)                   //timeout in milliseconds
        ) == DRV_NO_ERROR)
   {
     _pdo_data.set_rlmsgerr(0);
     //rlDebugPrintf("PDO transmit successful! \n");
     return 1;  
   }
   sprintf(err_out_buf, "ERROR: PDO transmit failed! BoardId: %d NodeId: %d \
                         PDO ID: %d Mappingnr: %d\n", 
                         _boardnr, _nodeid, _pdonr, _mappingnr);
   rlDebugPrintf(err_out_buf);
   
   _pdo_data.set_rlmsgerr(ERRCODE_PDOSND_FAIL);
   return 0;    
}
//---------------------------------------------------------------------------

//! transmit a 8 Byte PDO to a specific node
int rlCanOpen::pdo_transmit( int _boardnr, 
                             int _nodeid,
                             int _pdonr, 
                             rlCanOpenTypes &_pdo_data)
{
   int nodeindex;
   _pdo_data.buf2pdobuf();
   
   if (getNodeIndex(_boardnr,_nodeid,nodeindex)== 0) 
   {
     sprintf(err_out_buf, "ERROR: specified node does not exist! PDO transmit\
                           failed! BoardId: %d NodeId: %d PDO ID: %d \n",
                            _boardnr, _nodeid, _pdonr);
                            
     rlDebugPrintf(err_out_buf);
     _pdo_data.set_rlmsgerr(ERRCODE_INVALID_NODEID);
     return 0;
   }
   if (nodelist[nodeindex]->pdoList[DIRECTION_RECEIVE].size() < (unsigned int) _pdonr){
     sprintf(err_out_buf, "ERROR: specified PDO ID does not exist! PDO \
                           transmit failed! BoardId: %d NodeId: %d PDO \
                           ID: %d \n", _boardnr, _nodeid, _pdonr);
     rlDebugPrintf(err_out_buf);
     _pdo_data.set_rlmsgerr(ERRCODE_INVALID_PDOID);
     return 0;
   }
   
   int pdosize = nodelist[nodeindex]->pdoList[DIRECTION_RECEIVE][_pdonr-1]->bPDOSize;
   int pdooffset = nodelist[nodeindex]->pdoList[DIRECTION_RECEIVE][_pdonr-1]->usPDOAddress; 
   
   if ( (sRet =  DevExchangeIO( _boardnr,       // Board NO
                        pdooffset,              //offset send data
                        pdosize,                //Length of the send IO data
                        &_pdo_data.pdobuffer[0],//Pointer to the user send data buffer
                        0,                      //Byte offset in the receive IO data area
                        0,                      //Length of the receive IO data
                        NULL,                   //Pointer to the user read data buffer
                        100L)                   //timeout in milliseconds
        ) == DRV_NO_ERROR)
   {
     _pdo_data.set_rlmsgerr(ERRCODE_NOERROR);
     return 1;
   }
   sprintf(err_out_buf, "ERROR: PDO transmit failed! BoardId: %d NodeId: %d \
                         PDO ID: %d \n", _boardnr, _nodeid, _pdonr);
   rlDebugPrintf(err_out_buf);
   
   _pdo_data.set_rlmsgerr(ERRCODE_PDOSND_FAIL);
    return 0;                           
}

//---------------------------------------------------------------------------
//! this function resets all elements of global message struct to zero
void rlCanOpen::delmsg(){
  message.rx            =0;
  message.tx            =0;
  message.ln            =0;
  message.nr            =0;
  message.a             =0;
  message.f             =0;
  message.b             =0;
  message.e             =0;
  message.device_adr    =0;
  message.data_area     =0;
  message.data_adr      =0;
  message.data_idx      =0;
  message.data_cnt      =0;
  message.data_type     =0;
  message.function      =0;
  
  for (int i = 0; i < MAX_SDO_BUFFERSIZE; i++)
    message.d[i] = 0;

}
//---------------------------------------------------------------------------
//! this function returns the listindex of a desired node
int rlCanOpen::getNodeIndex(int _boardnr,int _nodeid,int & _index)
{
  for(unsigned int i = 0; i < nodelist.size(); i++)
  {
    if ((nodelist[i]->getNodeID() == _nodeid) && 
        (nodelist[i]->getBoardID() == _boardnr))
    {
      _index = i;
      return true;
    }           
  }
  return false; 
}
//---------------------------------------------------------------------------
//! returns daemontype number of a specific object
int rlCanOpen::getObjectType( int _boardnr, 
                              int _nodeid, 
                              int _objectindex, 
                              int _subindex)
{
  int nodeindex;
  if (getNodeIndex(_boardnr, _nodeid, nodeindex)){
    if (nodelist[nodeindex]->hasMapping())
      return nodelist[nodeindex]->objecttype(_objectindex, _subindex);
    else
      return rlCanOpenTypes::RL_BUFFER;
  }
  else{
    return rlCanOpenTypes::RL_NOTYPE;
  }
}

//---------------------------------------------------------------------------
 /*! if the user changes the mapping of a node using SDO functions, 
     the internal mapping list should be changed */
int rlCanOpen::refreshMappingList( int _boardnr, 
                                   int _nodeid,
                                   int _pdoID,
                                   int _pdoDirection)
{
  //rlDebugPrintf("refresh mappinglist1\n");
  int nodeindex;
  int madress=0;
  
   
    madress = nodelist[nodelist.size()-1]->
                pdoList[_pdoDirection][_pdoID -1]->mappingOvAdress;
      
  if (getNodeIndex(_boardnr,_nodeid,nodeindex)== false) 
  {
    sprintf(err_out_buf, "ERROR: specified node does not exist! refresh \
                          mapping list failed! BoardId: %d NodeId: %d \
                          PDO ID: %d \n", _boardnr, _nodeid, _pdoID);
    rlDebugPrintf(err_out_buf);
    return 0;
  }
  
 
  if(nodelist[nodeindex]->pdoList[_pdoDirection].size() < (unsigned int) _pdoID)
  {
    sprintf(err_out_buf, "ERROR: PDO does not exist! refresh \
                          mapping list failed! BoardId: %d NodeId: %d \
                          PDO ID: %d \n", _boardnr, _nodeid, _pdoID);
    rlDebugPrintf(err_out_buf);
    return 0;
  }
   
  //append Mapping data                                 
  rlCanOpenTypes sdobuffer;                            
  int mappingcount;
  rlCanMapping * mappingobject; 
                                            
  //get mapping count                       
  if (sdo_read(_boardnr,_nodeid,madress,0,sdobuffer) == 0){
    sprintf(err_out_buf, "ERROR: read sdo data! refresh mapping list failed! \
                          BoardId: %d NodeId: %d PDO ID: %d \n", 
                          _boardnr, _nodeid, _pdoID);
    rlDebugPrintf(err_out_buf);
  }
  
  mappingcount = sdobuffer.get_uchar();   
  
  sprintf(err_out_buf, "Current mappingcount: %d\n", mappingcount);
  rlDebugPrintf(err_out_buf);                                           
  
  nodelist[nodeindex]->pdoList[_pdoDirection][_pdoID-1]->
                  mappingList.resize(mappingcount);

  int currentposition = 0;
  for (int mcount = 1; mcount <= mappingcount;mcount++)  //each mappingobject
  {
    mappingobject = new rlCanMapping;  //create new mappingobject in memory
    mappingobject->mappingId = mcount;
        
   
   if (sdo_read(_boardnr,_nodeid,madress,mcount,sdobuffer) == 0){
     sprintf(err_out_buf, "ERROR: read sdo data! refresh mapping list failed! \
                           BoardId: %d NodeId: %d PDO ID: %d \n", 
                           _boardnr, _nodeid, _pdoID);
     rlDebugPrintf(err_out_buf);
     //cout<<"Error during create Mappinglist!\n";
    }
       
    //get length of mapping object
    int mlength  = sdobuffer.get_buffer(0);
    mappingobject->length = mlength;
    
    //calculate the mapping position
    if (mcount>1)
      mappingobject->position = currentposition;         
    else       
      mappingobject->position = 0;           
    currentposition+=mlength;
    
    // get the ov adress of the current mapped object
    int mi  = (int) (sdobuffer.get_buffer(3)*pow(256,1) 
                                          + sdobuffer.get_buffer(2));
                                          
    unsigned char msi = sdobuffer.get_buffer(1);
        
    rlCanOpenTypes temptype;
    mappingobject->etype = temptype.canOpenType2DeamonType
                           (nodelist[nodelist.size()-1]->objecttype(mi, msi));
        
    // insert the mappingobject to mappinglist
    nodelist[nodeindex]->pdoList[_pdoDirection][_pdoID-1]->
                         mappingList.insert(mcount-1,mappingobject);
       
  }
  return 1;                                                   
}                                                               
//---------------------------------------------------------------------------
//! return PdoId of mapped object. 
int rlCanOpen::getPdoID( int _boardnr, 
                         int _nodeid, 
                         int _objektindex, 
                         int _direction){
  int pdomax=0;
  int nodeindex=0;
  int madress=0;
  // get index of node in nodelist depending on boardid and nodeid
  // index is stored in nodeindex variable
  getNodeIndex(_boardnr, _nodeid, nodeindex); 
  
  // get number of elements in particular PDO list
  if (_direction == DIRECTION_RECEIVE)
    pdomax = nodelist[nodeindex]->getReceivePdoCount();
  if (_direction == DIRECTION_TRANSMIT)
    pdomax = nodelist[nodeindex]->getTransmitPdoCount();
   
   /* iterate through every element of particular list until mapping 
      object adress matches given objectindex */
   for (int j=1;j<=pdomax;j++)  
   {                                                                                                    
      
     madress = nodelist[nodeindex]->pdoList[_direction][j-1]->mappingOvAdress;    
     if (madress == _objektindex)
       return j;  
   }
   return 0;
}
//---------------------------------------------------------------------------
//!use this function to get information about a specific node
int rlCanOpen::getNodeState(int _boardnr, int _nodeid, rlCanOpenTypes &_data)
{ 
 
 setBoardActive(_boardnr);
 
 COM_ND_DIAGNOSTICS * diag;
 diag = (COM_ND_DIAGNOSTICS*) &message.d[0];

  message.rx = CAN_TASK         ;//receiver = user at HOST
  message.tx = USER_AT_HOST     ;// transmitter = USR_INTF-Task
  message.ln = 8                ;//length of message
  message.nr = messagenr++      ;//number of the message
  message.a  = 0                ;//no answer
  message.f  = 0                ;//error, status
  message.b  = CMDCODE_Node_Diag;//command = COM_Node_Diag
  message.e =0                  ;//extension
  message.device_adr = _nodeid  ;//k Rem_Adr
  message.data_area = 0         ;// data area, unused
  message.data_adr  = 0         ;//data address, unused
  message.data_idx  = 0         ;//data index, unused
  message.data_cnt  = 0         ;//data count, unused
  message.data_type = 5         ;// data type byte string
  message.function =  1         ;//function read
  
  
 if ( (sRet = DevPutMessage(_boardnr, (MSG_STRUC *)&message, 5000L)) 
       == DRV_NO_ERROR )
 {
   while( (sRet = DevGetMessage(_boardnr, sizeof(message), 
                                (MSG_STRUC *)&message, 0)) 
          == DRV_DEV_GET_NO_MESSAGE )       
   {
     continue;
   }
   if ( sRet == DRV_NO_ERROR )
   {
     rlDebugPrintf("message successfully received\n");
     // continue;
   }
   else
   {
     rlDebugPrintf("Error  Message (f = %d))\n",message.f);
     rlDebugPrintf("\nDevGetMessage(%d, sizeof(message), &message, 500): %d\n",
             _boardnr, (int)sRet);
             
     return 0;
    } 
  }
  else
  {
    rlDebugPrintf("\nDevPutMessage(%d, &message, 5000): %d\n",_boardnr, (int)sRet);
    return 0;
  }   
  _data.set_nodestateFlags( diag->bNodeStatus_1.bNodeNoResponse, 
                            diag->bNodeStatus_1.bEmcyBuffOverflow,
                            diag->bNodeStatus_1.bPrmFault, 
                            diag->bNodeStatus_1.bGuardActive,
                            diag->bNodeStatus_1.bDeactivated );
                       
  _data.set_nodestate(diag->bNodeState);
  _data.set_nodeerror(diag->bActualError);
                       
  _data.set_DaemonType(rlCanOpenTypes::RL_NODESTATE);
  
  return 1;
}

//---------------------------------------------------------------------------
//!using this function you are able to restart a CanOpenMaster board
int rlCanOpen::restartBoard(int _boardnr, int _restarttype)
{
   //COLDSTART           2
   //WARMSTART           3
   //BOOTSTART           4
   
   if ( (sRet = DevReset(_boardnr, _restarttype, 5000L) != DRV_NO_ERROR ))
  {
    rlDebugPrintf("restart failed!\n");
    rlDebugPrintf("DevReset(%d, Restart, 5000): %d\n", _boardnr, sRet);
    return 0;
  }
  
   rlDebugPrintf("restart successfully\n");
   return 1;
}
//---------------------------------------------------------------------------
//! send a NMT Command to one or all nodes (of one board) 
int rlCanOpen::sendNMTCommand(int _boardnr, int _nodeid, unsigned char _cmd)
{
  MSG_STRUC message;
  setBoardActive(_boardnr);
  
  message.rx = CAN_TASK         ;//receiver = user at HOST
  message.tx = USER_AT_HOST     ;// transmitter = USR_INTF-Task
  message.ln = 2                ;//length of message
  message.nr = messagenr++      ;//number of the message
  message.a  = 0                ;//no answer
  message.f  = 0                ;//error, status
  message.b  = CMD_NMT_Module_Protocol;//command = COM_Node_Diag
  message.e =  0                ;//extension

        // NMT Command specifier
        //1     Start Remote Node
        //2     Stop Remote Node
        //128   Enter Pre-Operational
        //129   Reset Node
        //130   Reset Communication
  message.data[0] = _cmd                ;//NMT Command specifier
  message.data[1] = _nodeid     ;//NodeID
  
   if ( (sRet = DevPutMessage(_boardnr, (MSG_STRUC *)&message, 5000L)) 
         == DRV_NO_ERROR )
 {
   while( (sRet = DevGetMessage(_boardnr, sizeof(message), 
                               (MSG_STRUC *)&message, 0)) 
           == DRV_DEV_GET_NO_MESSAGE )
   {
     continue;
   }
   if ( sRet == DRV_NO_ERROR )
   {
     rlDebugPrintf("NMT message successfully received\n");
     // continue;
   }
   else
   {
     rlDebugPrintf("Error  Message (f = %d))\n",message.f);
     rlDebugPrintf("\nDevGetMessage(%d, sizeof(message), &message, 500): %d\n",
             _boardnr, (int)sRet);
     
     return 0;
    }
  }
  else
  {
    rlDebugPrintf("\nDevPutMessage(NMT)(%d, &message, 5000): %d\n",_boardnr, (int)sRet);
    return 0;
  } /* if ( (sRet = DevPutMessage(...)) == DRV_NO_ERROR ) else */
  
  return 1;     
}

//---------------------------------------------------------------------------
//! read properties of inifile
void rlCanOpen::read_inifile(const char* _filename){
  rlIniFile ini;   
  QString buffer;
  if (ini.read(_filename)!=-1){
    rlDebugPrintf("used inifile: %s\n", _filename);
    
    // read EDS directory location
    buffer=QString(ini.text("DIRECTORIES","EdsDir"));
    buffer.remove((char) ACSII_CODE_CARRIAGE_RETURN);
    rlDebugPrintf("EdsDir: %s\n", buffer.ascii());
    // store EDS directory location in environmet variable
#ifdef RLWIN32
    SetEnvironmentVariable("EdsDir", buffer.ascii());
#else
    setenv("EdsDir", buffer.ascii(), 1);
#endif    
    
    // read Logfilename
    logFileName=QString(ini.text("DIRECTORIES","LogFileName"));
    logFileName.remove((char) ACSII_CODE_CARRIAGE_RETURN);
    rlDebugPrintf("Log Filename: %s\n", logFileName.ascii());
    
    // read loggin-option 
    buffer=QString(ini.text("OPTIONS","enableLoggin"));
    if (buffer.contains("yes", false)==1)
      enableLogging=true;  
    else
      enableLogging=false;  
  }
  else
  {
    logFileName=  "./outfile.txt";
    rlDebugPrintf("ERROR: INIFILE NOT FOUND\n");
  }
}

//-------------------------------------------------------------------------
bool rlCanOpen::is_twisted_type(int _canopentype)
{
  switch (_canopentype){
  
    case rlCanOpenTypes::INTEGER24_t :return true;  
    case rlCanOpenTypes::INTEGER40_t :return true;  
    case rlCanOpenTypes::INTEGER48_t :return true;  
    case rlCanOpenTypes::INTEGER56_t :return true;  
    case rlCanOpenTypes::UNSIGNED24_t:return true;  
    case rlCanOpenTypes::UNSIGNED40_t:return true;  
    case rlCanOpenTypes::UNSIGNED48_t:return true; 
    case rlCanOpenTypes::UNSIGNED56_t:return true;   
    default                          :return false;
  };

}

