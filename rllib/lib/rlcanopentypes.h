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

#include <iostream>
    using std::cin;
    using std::cout;

#include "rlinifile.h"
#include <stdlib.h>
#include <string.h>

struct IPCMSGSTRUCT;

//!class to handle CANopen types
/*! Can nodes contain data objects of any type. It is necessary 
    to convert type secure data into type insecure rawdata to transfer it over 
    the canbus. Therefore this class helps to convert raw canbus data easily. 
    To prevent users from reading out raw data with a wrong type (e.g. performing
    get_int() when a float number is stored in this class), a type number indicates
    the currently stored datatype. Everytime such a type violation occurs, a warning
    is written to stdout. 
*/


class rlCanOpenTypes {
  public:
  enum rl_types{ 
    RL_BOOL=0,
    RL_UCHAR,
    RL_SHORT,     
    RL_USHORT,       
    RL_INT ,      
    RL_LONG,
    RL_FLOAT,      
    RL_DOUBLE,  
    RL_LONGLONG,     
    RL_STRING,   
    RL_BUFFER,
    RL_ULONG,
    RL_ULONGLONG,
    RL_PDO = 13,
    RL_NODESTATE,
    RL_NOTYPE = 0xFF
  };


 enum canopen_types{
 
 c_BOOLEAN      =  0x0001, 
 INTEGER8_t     =  0x0002, 
 INTEGER16_t    =  0x0003, 
 INTEGER32_t    =  0x0004,  
 UNSIGNED8_t    =  0x0005,
 UNSIGNED16_t   =  0x0006,	
 UNSIGNED32_t   =  0x0007,
 REAL32_t       =  0x0008,
 VISIBLE_STRING =  0x0009,	
 OCTET_STRING_t =  0x000A,	
 UNICODE_STRING =  0x000B,	
 TIME_OF_DAY_t  =  0x000C,  
 TIME_DIFFERENC =  0x000D,  
 BIT_STRING_t   =  0x000E,	
 DOMAIN_t       =  0x000F,	
 INTEGER24_t    =  0x0010,
 REAL64_t       =  0x0011,	
 INTEGER40_t    =  0x0012,  
 INTEGER48_t    =  0x0013,  
 INTEGER56_t    =  0x0014,  
 INTEGER64_t    =  0x0015,  
 UNSIGNED24_t   =  0x0016,
 RESERVED1_t    =  0x0017,	
 UNSIGNED40_t   =  0x0018,  
 UNSIGNED48_t   =  0x0019, 
 UNSIGNED56_t   =  0x001A, 
 UNSIGNED64_t   =  0x001B                                               
};  

  //! Constructor initializes with RL_NOTYPE
  rlCanOpenTypes();
  
  //! returns number of bytes used for current type
  int getLength();
  
  //! sets all bytes in databuffer to zero
  void clearBuffer();
  
  /*! sets the type of data stored in databuffer. All getter-functions forced to 
      return a type different from this will produce an invalid-type-warning to
      stdout
  */
  void set_DaemonType(rl_types _type);

  //! alternative setter receives integer typenumber
  void set_DaemonType(int _type);
  
  //! returns current of data stored in databuffer 
  int get_DaemonType();
  
  /*! recieves an CANopen typenumber as defined in CiA DS-301 specification. 
      This type is converted to the appropiate RL-type. Therefore type-numbers
      may be extracted directly from EDS file without external conversion.
  */
  void set_CanOpenType(int _type);
 
  //! returns the current type converted to CANopen typenumber
  int get_CanOpenType();
  
  /*! Function for typenumber conversion. CANopen-Type differs between 27 primitive 
      data types while DaemonType only consists of 13 ANSI C data types an two
      more daemon specific tyes: RL_PDO and RL_NOTYPE which indicates that no
      type is defined for raw data placed in this classes databuffer.
  */
  static int canOpenType2DeamonType(int _canopentype);
  
  //! Function for typenumber conversion. 
  static int deamonType2CanOpenType(int _deamontype);
  
  //! sets type to RL_INT and stores passed parameter data in databuffer
  void set_int(int _value);

  /*! returns databuffer-content as integer type. When type is not RL_INT 
      this will produce an invalid-type-warning to stdout*/
  int get_int();

  //! sets type to RL_FLOAT and stores passed parameter data in databuffer
  void set_float(float _value);

  /*! returns databuffer-content as float type. When type is not RL_FLOAT 
      this will produce an invalid-type-warning to stdout*/
  float get_float();

  //! sets type to RL_DOUBLE and stores passed parameter data in databuffer
  void set_double(double _value);

  /*! returns databuffer-content as double type. When type is not RL_DOUBLE 
      this will produce an invalid-type-warning to stdout*/
  double get_double();
  
  //! sets type to RL_SHORT and stores passed parameter data in databuffer
  void set_short(short _value);
  
  /*! returns databuffer-content as short type. When type is not RL_SHORT 
    this will produce an invalid-type-warning to stdout*/
  short get_short();

  //! sets type to RL_USHORT and stores passed parameter data in databuffer
  void set_ushort(unsigned short _value);
  
  /*! returns databuffer-content as short type. When type is not RL_USHORT 
      this will produce an invalid-type-warning to stdout*/
  unsigned short get_ushort();

  //! sets type to RL_LONGLONG and stores passed parameter data in databuffer
  void set_longlong(long int _value);
  
  /*! returns databuffer-content as long long type. When type is not RL_LONGLONG
      this will produce an invalid-type-warning to stdout*/
  long int get_longlong();

  /*! sets type to RL_STRING. the passed string-constant is copied byte-wise 
      to databuffer */
  void set_string(const char* _value);
  
  /*! creates a new 246 bytes string filled with databuffer-content and returns a 
      pointer. this pointer must be stored and memory must be freed with delete,
      when the string is not longer needed, to prevent programm from producing 
      memory leak. */
  char* get_string();

  /*! This setter does not modify the current type. it is intended to transfer 
      raw data recieved from canbus into the databuffer and afterwards set the
      approbiate daemontype with set_daemontype. _index must be within range of
      246 bytes databuffer size.
   */
  void set_buffer(int _index, unsigned char _databyte);
  
  //! returns 1 byte from databuffer 
  unsigned char get_buffer(int _index);

  //! sets type to RL_UCHAR and stores passed parameter data in databuffer
  void set_uchar(unsigned char _value);

  /*! returns databuffer-content as uchar type. When type is not RL_UCHAR 
      this will produce an invalid-type-warning to stdout*/
  unsigned char get_uchar();
  
  //! sets type to RL_BOOL and stores passed parameter data in databuffer
  void set_bool(bool _value);
  
  /*! returns databuffer-content as bool type. When type is not RL_BOOL
      this will produce an invalid-type-warning to stdout*/
  bool get_bool();

  //! sets type to RL_LONG and stores passed parameter data in databuffer
  void set_long(long _value);
  
  /*! returns databuffer-content as long type. When type is not RL_LONG
      this will produce an invalid-type-warning to stdout*/
  long get_long();
  
  //! sets type to RL_ULONG and stores passed parameter data in databuffer  
  void set_ulong(unsigned long _value);
  
  /*! returns databuffer-content as ulong type. When type is not RL_ULONG
      this will produce an invalid-type-warning to stdout*/
  unsigned long get_ulong();

  //! sets type to RL_ULONGLONG and stores passed parameter data in databuffer  
  void set_ulonglong(unsigned long int _value);
  unsigned long int get_ulonglong();

  //! sets the errornumber. refer private varibale rlmsgerr for details
  void set_rlmsgerr(long _errnr);
  //! returns current errnumber
  long get_rlmsgerr();
  
  /*! sets type to RL_NODESTATE. This is function is intended to store flag-data 
      received from the device. */
  void set_nodestateFlags( unsigned char _bNodeNoResponse,
                           unsigned char _bEmcyBuffOverflow,
                              unsigned char _bPrmFault,
                           unsigned char _bGuardActive ,
                           unsigned char _bDeactivated);
  
  /*! returns false when typenumber is not RL_NODESTATE. if true states are stored 
      in parameter variables */
  bool get_nodestateFlags( bool &_bNodeNoResponse, 
                           bool &_bEmcyBuffOverflow, 
                           bool &_bPrmFault, 
                           bool &_bGuardActive, 
                           bool &_bDeactivated);
  
  //!  read particular nodestade with the following functions
  bool get_nodestateFlag_NoResponse();
  bool get_nodestateFlag_EmcyBuffOverflow();
  bool get_nodestateFlag_PrmFault();
  bool get_nodestateFlag_GuardActive();
  bool get_nodestateFlag_Deactivated();

  /*! sets current state of node. this is a code number the following meaning:
     alias              nr
     DISCONNECTED       1
     CONNECTING         2
     PREPARING          3
     PREPARED           4
     OPERATIONAL        5
     PRE_OPERATIONAL  127  */
  void set_nodestate(unsigned char _nodestate);
  
  //! returns the nodestate. refer code numbers above
  unsigned char get_nodestate();
  
  /*! sets the actual node error. refer com_pie.pdf page 15 for detailed 
      error code description */
  void set_nodeerror(unsigned char _nodeerror);
  
  //! returns actual node error
  unsigned char get_nodeerror();

  //! external buffer for pdotransfer
  unsigned char pdobuffer[8];
  
  //! exchange bytes 0-7 from databuffer to pdobuffer
  void buf2pdobuf();
  
  //! exchange bytes 0-7 from pdobuffer to databuffer
  void pdobuf2buf();
  
  //! copies all data from right object into left object
  rlCanOpenTypes& operator = (rlCanOpenTypes &cp); 
  
  //! returns a IPCMSGSTRUCT filled with current object data
  IPCMSGSTRUCT createIpcMsg();

  //! overwrites all data with IPCMSGSTRUCT data
  void getIpcMsg(IPCMSGSTRUCT _myIpcMsg);

  //! prints MsgErrStr to StdOut
  void rlMsgErrOut();

  
  /*! returns a pointer to a new 22 character string containing the name of 
      the error in rlmsgerr. This pointer must be stored and memory must be 
      freed with delete, when the string is not longer needed, to prevent 
      programm from producing memory leak.  */
  char* rlMsgErrStr();  

  /*! returns a pointer to a new 12 character string containing the name of 
      the type passed by _typnr. This pointer must be stored and memory must 
      be freed with delete, when the string is not longer needed, to prevent 
      programm from producing  memory leak.   */
  char* type2str(int _typenr);


  bool translate_CanOpenType(int _canopentype);
  int  get_CanOpenTypeLength(int _canopentype);

  private:
  
  /*! produce an invalid-type-warning to stdout. 
      used by nearly all getter functions */
  void invalidTypeError(int _typenr);
  

  /*! variable that stores the current datatype. only rl_types enum values 
      are indented to be stored here */
  int typenumber; 

  /*!   errnumber  // alias                  //  description
    rlmsgerr = 0   // ERRCODE_NOERROR        //  no error
    rlmsgerr > 0   //                        //  msg.f error code. Refer com_pie.pdf page 44 for details.
    rlmsgerr = -1  //                        //  msg.a or msg.nr inconsistency
    rlmsgerr = -2  // ERRCODE_INVALID_NODEID //  nodeid does not exist
    rlmsgerr = -3  // ERRCODE_INVALID_PDOID  //  pdoid does not exist
    rlmsgerr = -4  // ERRCODE_PDOSND_FAIL    //  transmit pdo failed
    rlmsgerr = -5  // ERRCODE_INVALID_MAPID  //  mappingid does not exist
    rlmsgerr = -6  // ERRCODE_INVALID_RLTYPE //  type does not fit during pdo transmit
    rlmsgerr = -7  // ERRCODE_PDORCV_FAIL    //  pdo receive failed  */
  long rlmsgerr; 


  //all different types are stored together in the same data area
  union {
    int t_int;                      //   4 Bytes   0x00
    float t_float;                  //   4 Bytes   0x01
    double t_double;                //   8 Bytes   0x02
    short t_short;                  //   2 Bytes   0x03
    unsigned short t_ushort;        //   2 Bytes   0x04
    long int t_longlong;           //   8 Bytes   0x05
    char t_string[247];             // 246 Bytes   0x06
    unsigned char t_databytes[247]; // 246 Bytes   0x07    
    unsigned char t_databyte;       //   1 Byte    0x08
    bool t_bool;                    //   1 Byte    0x09
    long t_long;                    //   4 Bytes   0x0A
    unsigned long t_ulong;          //   4 Bytes   0x0B
    unsigned long int t_ulonglong; //   8 Bytes   0x0C
      //                  Error =>                 0xFF
    struct
    {
      unsigned char    bNodeNoResponse   : 1; /* no response */
      unsigned char    bEmcyBuffOverflow : 1; /* emcy.buffer overflow */
      unsigned char    bPrmFault         : 1; /* parameters faulty */
      unsigned char    bGuardActive      : 1; /* node guarding active */
      unsigned char    bDeactivated      : 1; /* not configured */
      unsigned char    bNodeState;
      #define DISCONNECTED       1
      #define CONNECTING         2
      #define PREPARING          3
      #define PREPARED           4
      #define OPERATIONAL        5
      #define PRE_OPERATIONAL  127
      unsigned char  bActualError;
    } bNodeFlagState;
  }; 

};



//! the IPCMSGSTRUCT is the transfer buffer which is send trough TCP sockets 
/*
  MSG_SEND    0
  MSG_RECEIVE 1

#define MSG_SDO_READ 0
#define MSG_SDO_WRITE 1
#define MSG_PDO_RECEIVE 3
#define MSG_CONNECT 5
#define MSG_DISCONNECT 6
#define MSG_PDO_TRANSMIT 4
#define MSG_NMT_TRANSMIT 7
#define MSG_RESTART_BOARD 8
#define MSG_GET_NODE_STATE 9
*/

struct IPCMSGSTRUCT
{
  long typenumber;
 /* msgtype can be one of this values
    MSG_SDO_READ       0
    MSG_SDO_WRITE      1
    MSG_PDO_RECEIVE    3
    MSG_CONNECT        5
    MSG_DISCONNECT     6
    MSG_PDO_TRANSMIT   4
    MSG_NMT_TRANSMIT   7
    MSG_RESTART_BOARD  8
    MSG_GET_NODE_STATE 9  */
  long msgtype; 
  
 /* transfer type can be one of this values
    MSG_SEND    0
    MSG_RECEIVE 1 */
  long transfertype; 
  
  long processid;
  long boardid;
  long nodeid;
  long objectindex;
  long subindex;
  long pdoid;
  long mappingid;
  long rlmsgerr;
  char mtext[247];
};

