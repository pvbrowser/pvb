/***************************************************************************
                          cannode.h  -  description
                             -------------------
    begin                : Tue March 03 2004
    copyright            : (C) 2004 by R. Lehrig
    email                : lehrig@t-online.de
    authors              : Marc Br�tigam, Christian Wilmes
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/

#ifndef NODE
#define NODE

#include "rldefine.h"
#include <iostream>
    using std::cin;
    using std::cout;
#include <qptrvector.h>
#include <qdir.h>
#include <qstring.h>
#include <qstringlist.h>

#include "rlcanopenstructs.h"
#include "rlcanopentypes.h"
#include "rlinifile.h"

#ifndef TOOL
#define TOOL
//#include "ToolBox.cpp"
#endif

#include "objdir.h"       /* Include file for EDS file manager */

#include "cif_user.h"     /* Include file for device driver API */
#include "rcs_user.h"     /* Include file for RCS definition    */
#include "asc_user.h"     /* Include file for ASCII protocols   */
#include "nvr_user.h"     /* Include file for 3964R protocol    */
#include "COM_USER.H"

//! class to manage one node of a CanOpenMaster board in a CanOpen network
class rlCanNode 
{
  public:
  rlCanNode ();
  
  /*! initializes a new node obj. node is defined by boardid, nodeid and a 
      telegram which  contains node specific information from device's dual 
      ported memory */
  rlCanNode (int boardnr, int nodeid, RCS_MESSAGETELEGRAM_10& _telegramm);
  
  //! calls destructors of rlinifile obj and objdir 
  ~rlCanNode();
  
  //!returns the type of a specific entry in the object directory
  int objecttype(int objindex, int subindex);
  
  //!prints out the configuration of the node 
  void showConfiguration();
  
  //!returns the current node ID of the node 
  int getNodeID();
  
  //!returns the board ID the node is connected to. 
  int getBoardID();
  
  //!returns the current number of all installed PDOs (Process Data Objects)
  int getPdoCount();
  
  //!returns the current number of all installed reiceive PDOs
  int getReceivePdoCount();
  
  //!returns the current number of all installed transmit PDOs
  int getTransmitPdoCount();
  
  /*! indicates if node mapping is available. When mapping is active, it is 
      possible to catch a specific Object within a PDO */
  bool hasMapping();
  
  //! contains transmit PDOs 
  //QPtrVector<rlCanPDO> transmit_pdoList;
  
  //! contains receive PDOs 
  //QPtrVector<rlCanPDO> receive_pdoList;
  
  // RECEIVE_LIST  = 0
  // TRANSMIT_LIST = 1
  QPtrVector<rlCanPDO> pdoList[2];   
  
  private:
  //! current node ID (1-127)
  int nodeID;
  
  //! curent board ID (0-3)
  int boardID;
  
  /*! iterates through all files in eds directory. opens every eds file and 
      returns a pointer to an ini-file object of the eds file matching the
      product string in [DeviceInfo] ProductName */
  rlIniFile* getEDS(const char* _productstr);
  
  /*! pointer to inifile. object which handles access to the EDS file 
      describing the object directory of this node */
  rlIniFile* ini;  
  
  /*! pointer to objdir which handles access to all items in the object 
      directory of this node */
  ObjDir* objdir;  
  
  /*! reads out the configuration of a node from the device. 
      For success the device must configured by SYCon. */
  void readConfigurationMessage(RCS_MESSAGETELEGRAM_10& _message);
  
  //!Unique device number if available.
  unsigned short usIdentNumber;    
  
  //!Unique vendor number if available.
  unsigned char usVendorIdent;     
  
  //! specific node informations. extracted from message telegramm
  QString abVendorName;
  QString abDeviceName;
  QString abDescription;
  QString edslocation;
  unsigned char pdocount;
  unsigned char bMasterAddress;
  unsigned char bSettings; 
};

#endif

