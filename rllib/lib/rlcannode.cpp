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
#include "rlcutil.h"
#include "rlcannode.h"


//---------------------------------------------------------------------------
rlCanNode::rlCanNode()
{
  rlDebugPrintf("\n Constructer clear");
}
//---------------------------------------------------------------------------
//! initializes a new cannode by means of SyCon-pre-configurated parameters
rlCanNode::rlCanNode
           (int boardnr, int nodeid, RCS_MESSAGETELEGRAM_10& _telegramm){
  nodeID = nodeid;
  boardID = boardnr;
  readConfigurationMessage(_telegramm);    
  rlDebugPrintf("Initialising new node...\n BoardNr. %d NodeId %d Productstr %s ...",
    boardnr, nodeid, abDeviceName.ascii());
    
  ini = getEDS(abDeviceName);

  if (ini!=0){
    rlDebugPrintf(" SUCCESS! \n ");            
    rlDebugPrintf(" ProductName=%s \n ", ini->text("DeviceInfo","ProductName"));    
    rlDebugPrintf(" Filename=%s  \n ", ini->text("FileInfo","FileName"));
    rlDebugPrintf("Lege Objektverzeichnis an:  ");
    objdir = new ObjDir(ini);
  }
  else
  {
    rlDebugPrintf("ERROR!\n NO EDS FILE FOUND FOR \"%s\"!\n", abDeviceName.ascii());
    objdir = 0;
  } 
  
  if (objdir){
    // observe the adress for pdo mapping in OV
    unsigned int i,adr;
    adr = TRANSMIT_PDO_MAPPING_STARTADRESS;
    
    rlDebugPrintf(" PDO Gr�en:transmit PDOs: %d     receive PDOs: %d\n",
             pdoList[DIRECTION_TRANSMIT].size(),pdoList[DIRECTION_RECEIVE].size());
             
    for (i = 0; i< pdoList[DIRECTION_TRANSMIT].size();i++)
    {
      while(objdir->OVAdressExists(adr,0) == 0)
      {
        adr++;
        
        if (adr>TRANSMIT_PDO_MAPPING_MAXADRESS) {
          adr=0;
          rlDebugPrintf("ERROR IN EDS FILE: errorous number of Mapping entries\n");
          break;
        }
      }
      pdoList[DIRECTION_TRANSMIT][i]->mappingOvAdress = adr;
    
      adr++;
    }
    
    adr = RECEIVE_PDO_MAPPING_STARTADRESS ;
    for (i = 0; i< pdoList[DIRECTION_RECEIVE].size();i++)
    {
      while(objdir->OVAdressExists(adr,0) == 0)
      {
        adr++;
        
        if (adr>RECEIVE_PDO_MAPPING_MAXADRESS) {
          adr=0;
          rlDebugPrintf("ERROR IN EDS FILE: errorous number of Mapping entries\n");
          break;
        }
      }
      pdoList[DIRECTION_RECEIVE][i]->mappingOvAdress = adr;
      adr++;
    
    }
  }
                  
}   
//---------------------------------------------------------------------------
//! calls destructors of rlinifile obj and objdir 
rlCanNode::~rlCanNode(){
  rlDebugPrintf("Destructor rlCanNode\n");
  if (ini!=0){
    delete ini;
  }
  if (objdir!=0){
    delete objdir;
  }
}

//---------------------------------------------------------------------------
//! finds the eds file matching the product string in [DeviceInfo] ProductName 
rlIniFile* rlCanNode::getEDS(const char* _productstr){
  rlIniFile* _ini;
  // get eds dir
  QString dirname(getenv("EdsDir"));
  if (dirname.isEmpty()){
    dirname = "./";
  }
  QString productname(_productstr);
  QDir edsdir(dirname);
  edsdir.setNameFilter("*.EDS; *.eds");
  QStringList::Iterator dirit;
  QStringList dirlist;
  dirlist = edsdir.entryList();
  _ini = new rlIniFile();
  for ( dirit = dirlist.begin(); dirit != dirlist.end(); ++dirit ) {
    int searchval;
    
    QString exportname;

    edslocation = dirname + *dirit;
    _ini->read(edslocation.ascii());
    exportname =QString(_ini->text("DeviceInfo","ProductName"));
    searchval = exportname.contains(productname, false);
    if (searchval == 1){
      return _ini;
    }
  }
  delete _ini;
  return 0;
}

//---------------------------------------------------------------------------
//! reads out the configuration of a node from the CanOpenMaster board
void rlCanNode::readConfigurationMessage(RCS_MESSAGETELEGRAM_10& _message)
{
   
  int i;

  usIdentNumber = (unsigned short) (_message.d[2]+_message.d[1]*pow(256,1));
  usVendorIdent = (unsigned short) (_message.d[4]+_message.d[3]*pow(256,1));
    
  for(i=0;i<32;i++)
  {
    abVendorName+=_message.d[5+i]; 
    abDeviceName+=_message.d[37+i]; 
    abDescription+=_message.d[69+i]; 
  }
  
  bMasterAddress = _message.d[101];
  bSettings  = _message.d[102];
  pdocount = _message.d[117];
  
  //transmit_pdoList.resize(pdocount);
  
  int startadress=118; // start index of pdos in buffer
  int k = 0;  
  int tpdocounter = 0;
  int rpdocounter = 0;                                                      

 for(i=1;i<=int(pdocount);i++)                                      
 {                                                                                                                                                                             
    rlCanPDO *tempPDO = new rlCanPDO;
    tempPDO->bPDOSize      = _message.d[startadress+k+1];               
    tempPDO->bDataPosition = _message.d[startadress+k+2];               
    tempPDO->usPDOAddress  = (unsigned short) 
                             (_message.d[startadress+k+3]*pow(256,0)
                             + _message.d[startadress+k+4]);
  
   if  ((_message.d[startadress+k] & DATA_DIRECTION_MASK) == DIRECTION_INPUT)
   {
     rlDebugPrintf("\n tranceive pdo found ................\n");
     tpdocounter++;
     tempPDO->bPDODirection = DIRECTION_TRANSMIT; // => Tranceive PDO
     pdoList[DIRECTION_TRANSMIT].resize(pdoList[DIRECTION_TRANSMIT].size()+1);
     pdoList[DIRECTION_TRANSMIT].insert(tpdocounter-1,tempPDO);
   }
   else if(
       (_message.d[startadress+k] & DATA_DIRECTION_MASK) == DIRECTION_OUTPUT)
   {
      rlDebugPrintf("\n receive pdo found ................");
      rpdocounter++;
      tempPDO->bPDODirection = DIRECTION_RECEIVE; // => receive PDO
      pdoList[DIRECTION_RECEIVE].resize(pdoList[DIRECTION_RECEIVE].size()+1);
      pdoList[DIRECTION_RECEIVE].insert(rpdocounter-1,tempPDO);
   } 
   k=k+5;
  } 

}                                                                 

//!print out the configuration of the node
void rlCanNode::showConfiguration()
{
  cout<<"\n\n******** Configuration Board "
                          <<boardID<<" Node "<<nodeID <<" ********";
  
  cout<<"\n IdentNumber   :" << usIdentNumber;    
  cout<<"\n VendorIdent   :" <<usVendorIdent;    
  cout<<"\n Vendor Name   :"<<abVendorName;
  cout<<"\n Device Name   :"<<abDeviceName;
  cout<<"\n Description   :"<<abDescription;
  cout<<"\n PDO count     :"<<int(pdocount);
  cout<<"\n Master Adress :"<<int(bMasterAddress);
  cout<<"\n Settings      :"<<int(bSettings); 
 
  cout<<"\n\n - - - - PDOs -> - - - - -";
  unsigned int j;
  for (j = 1;j<=pdoList[DIRECTION_TRANSMIT].size();j++)
  {
    cout<<"\n+ PDO No. "<<j;
    cout<<"\nPDO Direction  :Send PDO";
    printf("\nPDO Size       :%i",pdoList[DIRECTION_TRANSMIT][j-1]->bPDOSize);
    printf("\nData Position  :%i",pdoList[DIRECTION_TRANSMIT][j-1]->bDataPosition);
    printf("\nPDO Adress     :%i",pdoList[DIRECTION_TRANSMIT][j-1]->usPDOAddress);
  }
  
   for (j = 1;j<=pdoList[DIRECTION_RECEIVE].size();j++)
  {
    cout<<"\n+ PDO No. "<<j;     
    cout<<"\nPDO Direction  :Receive PDO";  
    printf("\nPDO Size       :%i",pdoList[DIRECTION_RECEIVE][j-1]->bPDOSize);
    printf("\nData Position  :%i",pdoList[DIRECTION_RECEIVE][j-1]->bDataPosition);
    printf("\nPDO Adress     :%i",pdoList[DIRECTION_RECEIVE][j-1]->usPDOAddress);
  }
  
  cout<<"\n **********************************************\n\n";  
}

//---------------------------------------------------------------------------
//!returns the current node ID of the node 
int rlCanNode::getNodeID()
{
        return nodeID;
}
//---------------------------------------------------------------------------
//!returns the board ID in witch the node is connected
int rlCanNode::getBoardID()
{
        return boardID;
}                                                                    
//---------------------------------------------------------------------------
int rlCanNode::objecttype(int objindex, int subindex)
{
  QString edsbuffer;
  bool ok;
  int typenr;
  if (objdir != 0) {
    edsbuffer = objdir->get_objparameter(ObjDir::DATATYPE, objindex, subindex);
  }
  else
  {
    return rlCanOpenTypes::RL_NOTYPE;
  } 

  if (edsbuffer == "ERROR"){
    return rlCanOpenTypes::RL_NOTYPE;
  }
  edsbuffer.remove("0x");
  typenr = edsbuffer.toInt(&ok, 16);
  if (!ok){
    return rlCanOpenTypes::RL_NOTYPE;
  }
  else
  {
    return typenr;
  
  }
}

//---------------------------------------------------------------------------
 //!returns the current number of -all- installed PDOs (Process Data Objects)
int rlCanNode::getPdoCount()
{
  return pdocount;
}
//!returns the current number of all installed reiceive PDOs 
int rlCanNode::getReceivePdoCount()
{
  return pdoList[DIRECTION_RECEIVE].size();
}
//!returns the current number of all installed transmit PDOs 
int rlCanNode::getTransmitPdoCount()
{
  return pdoList[DIRECTION_TRANSMIT].size();
}
//!indicates if a node available mapping
bool rlCanNode::hasMapping(){
  if (objdir==0) return false;
  else return true;
}
