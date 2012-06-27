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

/*!
 * \section intro Introduction to rlCANopen-API
 *  
 * The rlCANopen-Software represents a developer friendly interface for
 * Hilscher CANopen Mastercard users, which ensures a comfortable
 * access to CANopen nodes and also Linux and Win32 portability.
 *
 * Using the Server-Client-architecture a network-wide availability of
 * data is feasible.
 *
 * <img src="uebersicht_engl.jpg">

 */


#ifndef NODES
#define NODES

#define err00 "no error, command executed"
#define err03 "DESCRIPT table not found. DEVICE is not configured by SyCon"
#define err57 "sequence error. Check requested DeviceAdr of continued message"
#define err58 "no entry found for requested DeviceAdr of request message."
#define err100 "Communication ERROR!!!"

#include "rlwthread.h"
#include "rlcannode.h"
#include <qvaluevector.h>
#include <qptrvector.h>

        
//! main class which provides canopen API functions and manages all nodes.
/*! This class contains a canopen specific API which accesses the canopen bus
    through the bus-independ Hilscher device API. It reads the configuration
    which has to be written previously to the device by SyCon. Based on this 
    configuration data it searches node corresponding EDS files to create 
    object directories.  These help to enshure type security and cast warnings
    when users try to write incorrect data types to can nodes.
    */
class rlCanOpen 
{
  public:
  rlCanOpen();
  rlCanOpen(char* _inifilename);
  ~rlCanOpen();
  
  //! returns the number of available boards
  int getBoardCount();
  
  //! returns the number of currently active board
  int getActiveBoard();
  
  //! prints out the configuration of a specific node
  bool showNodeConfiguration(int _boardnr,int _nodeid);
  
  //! prints out the configuration off all available nodes from all boards
  bool showNodeConfiguration();  // show all nodes
  
  /*! using the sdo_read function you can read a certain object from the object 
      dictionary of a node.  For this the SDO (Service Data Object) service 
      will be used. The enquired object will be written to the rlCanOpenTypes 
      class. To read out the correct value from this class use one of the 
      get_TYPE() {e.g. get_int() } functions. You can assert the type in the 
      class using rlCanOpenTypes::get_DaemonType(); 
      if sdo_read failures the current error can be read out using 
      rlCanOpenTypes::get_rlmsgerr()*/
  int sdo_read ( int _boardnr, 
                 int _nodeid,
                 int _objectindex, 
                 int _subindex, 
                 rlCanOpenTypes &_sdo_data);
  
  /*! using the sdo_write function you can write to a certain object from the
      object dictionary of a node. For this the SDO (Service Data Object) 
      Service will be used. The relevant data must be written into the 
      rlCanOpenTypes class before. To write the data into the rlCanOpenTypes 
      class use one of the datatype depending functions like
      rlCanOpenTypes::set_int(int _value);
      if sdo_write failures the current error can be read out using 
      rlCanOpenTypes::get_rlmsgerr()*/
  int sdo_write ( int _boardnr, 
                  int _nodeid,
                  int _objectindex, 
                  int _subindex, 
                  rlCanOpenTypes &_sdo_data); 
  
  /*! using the pdo_receive function you can receive a PDO (Process Data Object),
      which is send from the specific node to the device. The enquired object 
      will be written to the rlCanOpenTypes class. 
      The mapping is only available if mapping is configurated during the node 
      creation of the rlcanopen constructer. Therefore a eds file (electronic 
      data sheet) is needed. Use the cannode::hasMapping() function to check 
      if mapping is available. To read out the correct value from this class 
      use one of the get_TYPE() {e.g. get_int() } functions you can assert the 
      type in the class using rlCanOpenTypes::get_DaemonType(); */
  int pdo_receive( int _boardnr, 
                   int _nodeid,
                   int _pdonr, 
                   int _mappingnr, 
                   rlCanOpenTypes &_pdo_data);
  
  
  /*! using the pdo_receive function you can receive a PDO (Process Data Object),
      which is send from the specific node to the canOpen master card. The data 
      will be written to the rlCanOpenTypes class. This function doesn't use 
      mapping. So the whole 8Byte PDO data, will be received. To read out use 
      the rlCanOpenTypes::get_buffer({0-7}) function */
  int pdo_receive( int _boardnr, 
                   int _nodeid,
                   int _pdonr, 
                   rlCanOpenTypes &_pdo_data);
  
  
  
  /*! Using the pdo_transmit function you can transmit a PDO (Process Data 
      Object) to a specific node. The desired data must be written to the 
      rlCanOpenTypes class. The mapping is only available if mapping is 
      configurated during the node creation of the rlcanopen constructer.
      Therefore an eds file (electronic data sheet) is needed. 
      Use the cannode::hasMapping() function to check if mapping is available */
  int pdo_transmit( int _boardnr, 
                    int _nodeid,
                    int _pdonr, 
                    int _mappingnr, 
                    rlCanOpenTypes &_pdo_data);
  
  /*! Using the pdo_transmit function you can transmit a PDO (Process Data 
      Object) to a specific node. The desired data must be written to the 
      rlCanOpenTypes class. This function does not support mapping. So the 
      whole 8byte PDO data, will be send  to write to the rlCanOpenTypes use 
      the rlCanOpenTypes::set_buffer({0-7}) function */
  int pdo_transmit( int _boardnr, 
                    int _nodeid,
                    int _pdonr, 
                    rlCanOpenTypes &_pdo_data);
  
  /*! Because the nodes of all boards are stored in one list, the listindex is
      not equal to the nodeID(1-127). Use this function to get the listindex 
      of a desired node */
  int getNodeIndex( int _boardnr,
                    int _nodeid,
                    int & _index); 
  
  /*! returns daemontype number of an object. The type is retrieved from the 
      node's objectdir. When no objectdir exists (e.g. because of a missing 
      EDS file) or when the requested object does not exist, this function 
      will return 0xFF (the corresponding value to rlCanOpenTypes::RL_NOTYPE)
      */
  int getObjectType( int _boardnr, 
                     int _nodeid, 
                     int _objectindex, 
                     int _subindex);
  
  /*! Use this function to get information about a specific node.
      The nodestate, nodeerror and a nodestateFlag will be written to the 
      rlCanOpenTypes. Read the rlCanOpenTypes.h header to get further 
      information about these variables. Hilscher specific function (so no 
      CanOpoen services) are used to receive the information */
  int getNodeState( int _boardnr, 
                    int _nodeid,
                    rlCanOpenTypes &_data);
  
  /*! Using this function you are able to restart a CanOpenMaster board
      (device). There are 3 possibly kinds of restarting the master: 
      coldstart, warmstart, bootstart. 
      The corresponding '#defines' are declared in canopenstructs.h
      Hilscher specific function (so no CanOpoen services) are used to 
      execute this command */
  int restartBoard( int _boardnr, int _restarttype);
  
  /*! Using this function you can send a NMT Command to one or all nodes (of 
      one board). NMT (Network Management) is a CanOpen service. Because of the
      device there are constrictions for the user. 
      You can only execute the follow NMT commands:
      START_REMOTE_NODE, 
      STOP_REMOTE_NODE,
      ENTER_PREOPERATIONAL,
      RESET_NODE,
      RESET_COMMUNICATION
      The corresponding '#defines' are declared in canopenstructs.h */
  int sendNMTCommand( int _boardnr, 
                      int _nodeid, 
                      unsigned char _cmd);
                      
                      
                      
  bool is_twisted_type(int _canopentype);

  
  private:
  
  /*! This function reads out the configuration of a specific node using special 
      Hilscher commands (CMDCODE_GET_CONFIGURATION). If the master board isn't 
      configurated by SyCon it is not possibly to use this function with success.
      It returns false if the node is not available */
  bool getNodeConfiguration(int _nodeID); 
  
  /*! this function iterates through the recieve/tramsmit pdo lists and returns 
      the ID number of the pdo that fits the given objektindex. The PdoId 
      identifies the send/receive PDO on the given node */
  int getPdoID(int _boardnr, int _nodeid, int _objektindex, int _direction);
  
  //! the currently active board
  int activeboard;
  
  /*! read properties of inifile. EDS path, logfile location and user-specific 
      options are set on startup */
  void read_inifile(const char* _filename);
  
  //! initialize nodes and create nodelist
  bool ini();
  
  //! clear message struct by setting all elements to zero
  void delmsg();
  
  //! Read informations about installed devices
  bool iniboards();
  
  //! this string buffer is usually written with sprintf before sent to stdout
  char err_out_buf[255];
  
  //! return value for hilscher specific message functions
  short sRet;
  
  //! Pointer to error logfile
  FILE* err_fp; 
  
  //! name of logfile stdout is redirected to
  QString logFileName;
  
  //! hilscher specific message structur which will be sent to the master card 
  RCS_MESSAGETELEGRAM_10 message; 
  
  //! this list contains all nodes from all boards
  QPtrVector <rlCanNode> nodelist;
  
  //! the current count of CanOpenMaster cards
  int boardcount;
  
  //! message counter
  unsigned char messagenr;
  
  //! sets a desired CanOpenMaster active, so that it can be used
  short setBoardActive(int _boardnr);
  
  /*! if the user changes the mapping of a node using SDO functions, 
      the internal mapping list should be changed */
  int refreshMappingList( int _boardnr, 
                          int _nodeid,
                          int _pdoID,
                          int _pdoDirection);
  
  /*! this function checks the bus for available nodes and creates 
      a corresponding nodelist */
  int createNodes();
  
  //! this function writes the mapping objects for each pdo of a node 
  int createMappingObjects( int _boardid,
                            int _nodeid,
                            int _pdoDirection);
                           
 
  
  /*! this indicator is true when stdout is redirected to file. 
      it is set in the ini-file */
  bool enableLogging;
  
};
                        
#endif
