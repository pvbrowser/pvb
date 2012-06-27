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

//!definition of CanOpen structs for CanOpen Deamon

#ifndef CanOpenStructsH
#define CanOpenStructsH


#define DIRECTION_RECEIVE 0
#define DIRECTION_TRANSMIT 1



#define RECEIVE_PDO_MAPPING_STARTADRESS 0x1600
#define RECEIVE_PDO_MAPPING_MAXADRESS 0x17FF
#define TRANSMIT_PDO_MAPPING_STARTADRESS 0x1A00
#define TRANSMIT_PDO_MAPPING_MAXADRESS 0x1BFF
#define MAX_NODES 127
#define MAX_SDO_BUFFERSIZE 246



//! Hilscher device-communication constants
#define CAN_TASK                    3
#define USER_AT_HOST               16
#define CMDCODE_GET_CONFIGURATION  15
#define CMDCODE_SDO_UPDOWNLOAD     74
#define CMDCODE_Node_Diag          66
#define CMD_NMT_Module_Protocol    96
        
//! NMT Command specifier
#define CMD_NMT_START_REMOTE_NODE      1
#define CMD_NMT_STOP_REMOTE_NODE       2
#define CMD_NMT_ENTER_PREOPERATIONAL 128
#define CMD_NMT_RESET_NODE           129
#define CMD_NMT_RESET_COMMUNICATION  130

#define TASK_TFC_READ 1
#define TASK_TFC_WRITE 2
#define RESPONSE_CODE_GET_CONFIGURATION 15

#define DATA_DIRECTION_MASK 0x30 
#define DIRECTION_INPUT 0x10
#define DIRECTION_OUTPUT 0x20

//! ERRCODES
#define ERRCODE_NOERROR 0
#define ERRCODE_INVALID_NODEID -2
#define ERRCODE_INVALID_PDOID  -3
#define ERRCODE_PDOSND_FAIL  -4
#define ERRCODE_INVALID_MAPID  -5
#define ERRCODE_INVALID_RLTYPE -6
#define ERRCODE_PDORCV_FAIL  -7

//! DEVRESET
#define COLDSTART           2
#define WARMSTART           3
#define BOOTSTART           4

//! mapping object which is part of the mappinglist of a PDO
struct rlCanMapping
{
  //! the mapping ID determines the mapping object within the PDO (1 to n)
  unsigned short mappingId; 
  
  //! its current type (int32, float, char, ...)
  int etype;                

  //! its current canoptentype (int24, int40, int48, ...)
  int canopentype;                
  
  //! its position in within the 8Byte
  unsigned short position;  
  
  //! length of mappend object in bits
  unsigned short length;    
} ; 



//! PDO object which is part of one of the two pdo lists in the node class
struct rlCanPDO
{
   //! 1 = receive PDO / 2 = send PDO  [indicate by PDO No]          
   short bPDODirection; 
   
   //! size of a PDO / max 8 Bytes 
   unsigned char bPDOSize;     
   
   //! if only one bit is set in a PDO, this value shows its position 
   unsigned char bDataPosition;
   
   //! fixes the PDO position in the card memory 
   unsigned short usPDOAddress;
   
   //! this list contains all mapping information of a PDO  
   QPtrVector<rlCanMapping> mappingList; 
   
   //! the adress of object directory of the node where the mapping is placed
   int mappingOvAdress;           
}; 



#endif                          

