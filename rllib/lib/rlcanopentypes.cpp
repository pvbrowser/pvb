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
#ifndef RLCANOPENTYPES
#define RLCANOPENTYPES

#include "rlcutil.h"
#include "rlcanopentypes.h"


//! Constructor initializes with RL_NOTYPE
rlCanOpenTypes::rlCanOpenTypes(){
  typenumber = RL_NOTYPE;
  rlmsgerr = 0;
  clearBuffer();
}

//! returns number of bytes used for current type
int rlCanOpenTypes::getLength()
{
  switch(typenumber)   {                               
    case RL_INT:       return 4;
    case RL_FLOAT:     return 4;
    case RL_DOUBLE:    return 8;
    case RL_SHORT:     return 2;
    case RL_USHORT:    return 2;
    case RL_LONGLONG:  return 8;
    case RL_STRING:    return 246;
    case RL_BUFFER:    return 246;
    case RL_UCHAR:     return 1;
    case RL_BOOL:      return 1;
    case RL_LONG:      return 4;
    case RL_ULONG:     return 4;
    case RL_ULONGLONG: return 8;
    case RL_PDO:       return 8;
    case RL_NOTYPE:    return 0;
    default:           return 0;
  } 
}   

//! sets all bytes in databuffer to zero
void rlCanOpenTypes::clearBuffer(){
  for (int i=0; i<247; i++){
    t_databytes[i] = 0;
  }
}

//! sets the type of data stored in databuffer. 
void rlCanOpenTypes::set_DaemonType(int _type){
  typenumber  = _type;
}

//! alternative setter receives integer typenumber
void rlCanOpenTypes::set_DaemonType(rl_types _type){
  typenumber  = _type;
}

//! returns current of data stored in databuffer 
int  rlCanOpenTypes::get_DaemonType(){
  return typenumber;
}

//! recieves an CANopen typenumber and returns appropiate RL-Type
void rlCanOpenTypes::set_CanOpenType(int _type){
  typenumber  = canOpenType2DeamonType(_type);
}

//! returns the current type converted to CANopen typenumber
int rlCanOpenTypes::get_CanOpenType(){
  return deamonType2CanOpenType(typenumber);
}

//! Function for typenumber conversion. 
int rlCanOpenTypes::canOpenType2DeamonType(int _canopentype)
{
  // refer table Data Types on page 11 in CiA CANopen Specification
  
  switch(_canopentype)   { 
    case 0x0001:return RL_BOOL;   // c_BOOLEAN         
    case 0x0002:return RL_SHORT;   // INTEGER8_t        
    case 0x0003:return RL_SHORT;   // INTEGER16_t       
    case 0x0004:return RL_LONG;   // INTEGER32_t        
    case 0x0005:return RL_UCHAR;   // UNSIGNED8_t        
    case 0x0006:return RL_USHORT;   // UNSIGNED16_t       
    case 0x0007:return RL_ULONG;   // UNSIGNED32_t       
    case 0x0008:return RL_FLOAT;   // REAL32_t           
    case 0x0009:return RL_STRING;   // VISIBLE_STRING
    case 0x000A:return RL_STRING;   // OCTET_STRING_t     
    case 0x000B:return RL_STRING;   // UNICODE_STRING
    case 0x000C:return RL_ULONGLONG;   // TIME_OF_DAY_t      
    case 0x000D:return RL_ULONGLONG;   // TIME_DIFFERENC
    case 0x000E:return RL_BUFFER;   // BIT_STRING_t       
    case 0x000F:return RL_BUFFER;   // DOMAIN_t           
    case 0x0010:return RL_ULONG;   // INTEGER24_t        
    case 0x0011:return RL_DOUBLE;   // REAL64_t           
    case 0x0012:return RL_LONGLONG;   // INTEGER40_t        
    case 0x0013:return RL_LONGLONG;   // INTEGER48_t        
    case 0x0014:return RL_LONGLONG;   // INTEGER56_t        
    case 0x0015:return RL_LONGLONG;   // INTEGER64_t        
    case 0x0016:return RL_ULONG;   // UNSIGNED24_t       
    case 0x0017:return RL_BUFFER;   // RESERVED1_t        
    case 0x0018:return RL_ULONGLONG;   // UNSIGNED40_t       
    case 0x0019:return RL_ULONGLONG;   // UNSIGNED48_t       
    case 0x001A:return RL_ULONGLONG;   // UNSIGNED56_t       
    case 0x001B:return RL_ULONGLONG;   // UNSIGNED64_t  
    default    :return RL_NOTYPE;   // ERROR
  }
}


//! Function for typenumber conversion. 
int rlCanOpenTypes::deamonType2CanOpenType(int _deamontype)
{

  switch(_deamontype)   {                               
   case RL_INT:return 0x0007;
   case RL_FLOAT:return 0x0008;
   case RL_DOUBLE:return 0x0011;
   case RL_SHORT:return 0x0003;
   case RL_USHORT:return 0x0006;
   case RL_LONGLONG:return 0x0015;
   case RL_STRING:return 0x0009;
   case RL_BUFFER:return 0x000A;
   case RL_UCHAR:return 0x0005;//???
   case RL_BOOL:return 0x0001;
   case RL_LONG:return 0x0004;
   case RL_ULONG:return 0x0007;
   case RL_ULONGLONG:return 0x001B;
   case RL_PDO:return 0x000E;
   case RL_NOTYPE:return 0x0;
   default:return 0x0;
  } 
}         

//! sets type to RL_INT and stores passed parameter data in databuffer 
void rlCanOpenTypes::set_int(int _value){
  t_int = _value;
  typenumber = RL_INT;
}

//! returns databuffer-content as integer type. 
int rlCanOpenTypes::get_int(){
  if (typenumber != RL_INT){
    invalidTypeError(RL_INT);
    
  }
  return t_int;
}

//! sets type to RL_FLOAT and stores passed parameter data in databuffer
void rlCanOpenTypes::set_float(float _value){
  t_float = _value;
  typenumber = RL_FLOAT;
}

//! returns databuffer-content as float type. 
float rlCanOpenTypes::get_float(){
  if (typenumber != RL_FLOAT){
    invalidTypeError(RL_FLOAT);
  }
  return t_float;
}

//! sets type to RL_DOUBLE and stores passed parameter data in databuffer
void rlCanOpenTypes::set_double(double _value){
  t_double = _value;
  typenumber = RL_DOUBLE;
}

//! returns databuffer-content as double type. 
double rlCanOpenTypes::get_double(){
  if (typenumber != RL_DOUBLE){
    invalidTypeError(RL_DOUBLE);
  }
  return t_double;
}

//! sets type to RL_SHORT and stores passed parameter data in databuffer
void rlCanOpenTypes::set_short(short _value){
  t_short = _value;
  typenumber = RL_SHORT;
}

//! returns databuffer-content as short type
short rlCanOpenTypes::get_short(){
  if (typenumber != RL_SHORT){
    invalidTypeError(RL_SHORT);    
  }
  return t_short;
}

//! sets type to RL_USHORT and stores passed parameter data in databuffer
void rlCanOpenTypes::set_ushort(ushort _value){
  t_ushort = _value;
  typenumber = RL_USHORT;
}

//! returns databuffer-content as short type
unsigned short rlCanOpenTypes::get_ushort(){
  if (typenumber != RL_USHORT){
    invalidTypeError(RL_USHORT);
  }
  return t_ushort;
}


//! sets type to RL_LONGLONG and stores passed parameter data in databuffer
void rlCanOpenTypes::set_longlong(long int _value){
  t_longlong = _value;
  typenumber = RL_LONGLONG;
}

//! returns databuffer-content as long long type
long int rlCanOpenTypes::get_longlong(){
  if (typenumber != RL_LONGLONG){
    invalidTypeError(RL_LONGLONG);
  }
  return t_longlong;
}

/*! sets type to RL_STRING. the passed string-constant is copied byte-wise 
    to databuffer */
void rlCanOpenTypes::set_string(const char* _value){
  int i=0;
  while ((_value[i] != '\0')&&(i<247)){
    t_string[i] = _value[i];
    i++;        
  }
  typenumber = RL_STRING;
}

//! creates string filled with databuffer-content and returns a pointer
char* rlCanOpenTypes::get_string(){
  if (typenumber != RL_STRING){
    invalidTypeError(RL_STRING);
  }
  char* outstring = new char[247];
  int i=0;
  while ((t_string[i] != '\0')&&(i<247)){
    outstring[i] = t_string[i];
    i++;        
  }
  return outstring;
}

//! transfer raw data into the databuffer 
void rlCanOpenTypes::set_buffer(int _index, unsigned char _databyte){
  if (_index<247){
    t_databytes[_index] = _databyte;
  }
  else
  {
    fprintf(stderr, "ERROR! set_buffer: INDEX OUT OF RANGE!\n");  
  }
}


//! returns 1 byte from databuffer 
unsigned char rlCanOpenTypes::get_buffer(int _index){
  if (_index<247){
    return t_databytes[_index];
  }
  else
  {
    fprintf(stderr, "ERROR! get_buffer: INDEX OUT OF RANGE!\n");  
    return 0;
  }
}

//! sets type to RL_UCHAR and stores passed parameter data in databuffer
void rlCanOpenTypes::set_uchar(unsigned char _value){
  t_databyte = _value;
  typenumber = RL_UCHAR;
}

//! returns databuffer-content as uchar type
unsigned char rlCanOpenTypes::get_uchar(){
  if (typenumber != RL_UCHAR){
    invalidTypeError(RL_UCHAR);
  }
  return t_databyte;
}

//! sets type to RL_BOOL and stores passed parameter data in databuffer
void rlCanOpenTypes::set_bool(bool _value){
  t_bool = _value;
  typenumber = RL_BOOL;
}

//! returns databuffer-content as bool type
bool rlCanOpenTypes::get_bool(){
  if (typenumber != RL_BOOL){
    invalidTypeError(RL_BOOL);
  }
  return t_bool;
}
//! sets type to RL_LONG and stores passed parameter data in databuffer
void rlCanOpenTypes::set_long(long _value){
  t_long = _value;
  typenumber = RL_LONG;
}

//! returns databuffer-content as long type
long  rlCanOpenTypes::get_long(){
  if (typenumber != RL_LONG){
    invalidTypeError(RL_LONG);          
  }
  return t_long;
}
//! sets type to RL_ULONGLONG and stores passed parameter data in databuffer
void rlCanOpenTypes::set_ulonglong(unsigned long int _value){
  t_ulonglong = _value;
  typenumber = RL_ULONGLONG;
}

//! returns databuffer-content as unsigned long long type
unsigned long int rlCanOpenTypes::get_ulonglong(){
  if (typenumber != RL_ULONGLONG)
  {
    invalidTypeError(RL_ULONGLONG);
  }
  return t_ulonglong;
}

//! sets type to RL_ULONG and stores passed parameter data in databuffer
void rlCanOpenTypes::set_ulong(unsigned long _value){
  t_ulong = _value;
  typenumber = RL_ULONG;
}

//! returns databuffer-content as unsigned long type
unsigned long rlCanOpenTypes::get_ulong(){
  if (typenumber != RL_ULONG){
    invalidTypeError(RL_ULONG);
  }
  return t_ulong;
}

//! sets the errornumber. refer private varibale rlmsgerr for details
void rlCanOpenTypes::set_rlmsgerr(long _errnr){
  rlmsgerr = _errnr;
}

//! returns current errnumber
long rlCanOpenTypes::get_rlmsgerr(){
  return rlmsgerr;
}


//! copies all data from right object into left object
rlCanOpenTypes& rlCanOpenTypes::operator = (rlCanOpenTypes &cp){
  typenumber = cp.typenumber;
  rlmsgerr = cp.rlmsgerr;
  for (int i=0; i<246; i++){
    t_databytes[i]=cp.t_databytes[i];
  }
  return *this;
}

//! produce an invalid-type-warning to stdout. used by nearly all getter functions
void  rlCanOpenTypes::invalidTypeError(int _typenr){
  char* curTypeStr; 
  char* usrTypeStr;
  curTypeStr = type2str(typenumber);
  usrTypeStr = type2str(_typenr);
  rlDebugPrintf("Warning! %s invalid type! current data type is: %s \n"
         , usrTypeStr, curTypeStr);
  delete curTypeStr;
  delete usrTypeStr;
}

//! returns a pointer to a new 12 character string containing the name of the type
char* rlCanOpenTypes::type2str(int _typenr){
  char* typestr = new char[12];
  switch(_typenr)   {                               
   case RL_INT:       strncpy(typestr, "RL_INT      ", 12); break;
   case RL_FLOAT:     strncpy(typestr, "RL_FLOAT    ", 12); break;
   case RL_DOUBLE:    strncpy(typestr, "RL_DOUBLE   ", 12); break;
   case RL_SHORT:     strncpy(typestr, "RL_SHORT    ", 12); break;
   case RL_USHORT:    strncpy(typestr, "RL_USHORT   ", 12); break;
   case RL_LONGLONG:  strncpy(typestr, "RL_LONGLONG ", 12); break;
   case RL_STRING:    strncpy(typestr, "RL_STRING   ", 12); break;
   case RL_BUFFER:    strncpy(typestr, "RL_BUFFER   ", 12); break;
   case RL_UCHAR:     strncpy(typestr, "RL_UCHAR    ", 12); break;
   case RL_BOOL:      strncpy(typestr, "RL_BOOL     ", 12); break;
   case RL_LONG:      strncpy(typestr, "RL_LONG     ", 12); break;
   case RL_ULONG:     strncpy(typestr, "RL_ULONG    ", 12); break;
   case RL_ULONGLONG: strncpy(typestr, "RL_ULONGLONG", 12); break;
   case RL_PDO:       strncpy(typestr, "RL_PDO      ", 12); break;
   case RL_NODESTATE: strncpy(typestr, "RL_NODESTATE", 12); break;
   case RL_NOTYPE:    strncpy(typestr, "RL_NOTYPE   ", 12); break;
   default:           strncpy(typestr, "UNDEFINED   ", 12); 
  } 
  return typestr;
}

//! prints error message to StdOut
void rlCanOpenTypes::rlMsgErrOut(){
  char* errOut = rlMsgErrStr();
  rlDebugPrintf("ERROR: \"%s\" \n", errOut);
  delete errOut;
}
//!* returns a pointer to a new 22 character string containing name of error in rlmsgerr
char* rlCanOpenTypes::rlMsgErrStr(){
  char* errstr = new char[25];
  switch(rlmsgerr)   {                               
   case 0:  strncpy(errstr, "ERRCODE_NOERROR       ", 25); break;
   case -1: strncpy(errstr, "                      ", 25); break;
   case -2: strncpy(errstr, "ERRCODE_INVALID_NODEID", 25); break;
   case -3: strncpy(errstr, "ERRCODE_INVALID_PDOID ", 25); break;
   case -4: strncpy(errstr, "ERRCODE_PDOSND_FAIL   ", 25); break;
   case -5: strncpy(errstr, "ERRCODE_INVALID_MAPID ", 25); break;
   case -6: strncpy(errstr, "ERRCODE_INVALID_RLTYPE", 25); break;
   case -7: strncpy(errstr, "ERRCODE_PDORCV_FAIL   ", 25); break;
   default: strncpy(errstr, "ERRCODE_MSGF          ", 25); break;
  } 
  return errstr;
}


//! exchange bytes 0-7 from databuffer to pdobuffer
void rlCanOpenTypes::buf2pdobuf(){
  for (int i=0; i<8; i++){
    pdobuffer[i]=t_databytes[i];
  }
  typenumber = RL_PDO;
}

//! exchange bytes 0-7 from pdobuffer to databuffer
void rlCanOpenTypes::pdobuf2buf(){
  for (int i=0; i<8; i++){
    t_databytes[i]=pdobuffer[i];
  }
  typenumber = RL_PDO;
}


//! returns a IPCMSGSTRUCT filled with current object data
IPCMSGSTRUCT rlCanOpenTypes::createIpcMsg(){
  IPCMSGSTRUCT istruct;
  for (int i=0; i<247; i++){
    istruct.mtext[i] = t_string[i];
  }
  istruct.typenumber = typenumber;
  istruct.msgtype     = -1;
  istruct.transfertype= -1;
  istruct.processid   = -1;
  istruct.boardid     = -1;
  istruct.nodeid      = -1;
  istruct.objectindex = -1;
  istruct.subindex    = -1;
  istruct.pdoid       = -1;
  istruct.mappingid   = -1;
  istruct.rlmsgerr    = rlmsgerr;
  return istruct;
}

//! overwrites all data with IPCMSGSTRUCT data
void rlCanOpenTypes::getIpcMsg(IPCMSGSTRUCT _myIpcMsg){
  for (int i=0; i<247; i++){
    t_string[i] = _myIpcMsg.mtext[i];
  }
  typenumber = _myIpcMsg.typenumber;
  rlmsgerr   = _myIpcMsg.rlmsgerr;
}

//! sets type to RL_NODESTATE. Stores flag-data received from the device. 
void rlCanOpenTypes::set_nodestateFlags( unsigned char _bNodeNoResponse,
                                         unsigned char _bEmcyBuffOverflow,
                                         unsigned char _bPrmFault,
                                         unsigned char _bGuardActive ,
                                         unsigned char _bDeactivated)
{
  bNodeFlagState.bNodeNoResponse   =  _bNodeNoResponse;  
  bNodeFlagState.bEmcyBuffOverflow =  _bEmcyBuffOverflow;
  bNodeFlagState.bPrmFault         =  _bPrmFault;        
  bNodeFlagState.bGuardActive      =  _bGuardActive;     
  bNodeFlagState.bDeactivated      =  _bDeactivated;     
  typenumber = RL_NODESTATE;
}

//! sets current state of node.
void rlCanOpenTypes::set_nodestate(unsigned char _nodestate)
{
  bNodeFlagState.bNodeState = _nodestate;
  typenumber = RL_NODESTATE;
}  

/*! sets the actual node error. refer com_pie.pdf page 15 for detailed 
    error code description */
void rlCanOpenTypes::set_nodeerror(unsigned char _nodeerror)
{
  bNodeFlagState.bActualError = _nodeerror;
}

/*! returns false when typenumber is not RL_NODESTATE. if true states 
    are stored in parameter variables */
bool rlCanOpenTypes::get_nodestateFlags( bool &_bNodeNoResponse, 
                                         bool &_bEmcyBuffOverflow, 
                                         bool &_bPrmFault, 
                                         bool &_bGuardActive, 
                                         bool &_bDeactivated)
{
  if (typenumber == RL_NODESTATE){
    _bNodeNoResponse    = bNodeFlagState.bNodeNoResponse;
    _bEmcyBuffOverflow  = bNodeFlagState.bEmcyBuffOverflow;
    _bPrmFault          = bNodeFlagState.bPrmFault;
    _bGuardActive       = bNodeFlagState.bGuardActive;
    _bDeactivated       = bNodeFlagState.bDeactivated;
    return true;
  }
  else
  {
    invalidTypeError(RL_NODESTATE);    
    return false;
  }
}                     

//! returns the current nodestate code number
unsigned char rlCanOpenTypes::get_nodestate()
{
  if (typenumber != RL_NODESTATE) invalidTypeError(RL_NODESTATE); 
  return bNodeFlagState.bNodeState;     
}

//! returns actual node error
unsigned char rlCanOpenTypes::get_nodeerror()
{
    if (typenumber != RL_NODESTATE) invalidTypeError(RL_NODESTATE); 
  return bNodeFlagState.bActualError;
}

//!  read particular nodestade 
bool rlCanOpenTypes::get_nodestateFlag_NoResponse()
{
  if (typenumber != RL_NODESTATE) invalidTypeError(RL_NODESTATE); 
  return bNodeFlagState.bNodeNoResponse;
}
bool rlCanOpenTypes::get_nodestateFlag_EmcyBuffOverflow()
{
  if (typenumber != RL_NODESTATE) invalidTypeError(RL_NODESTATE); 
  return bNodeFlagState.bEmcyBuffOverflow;
}
bool rlCanOpenTypes::get_nodestateFlag_PrmFault()
{
  if (typenumber != RL_NODESTATE) invalidTypeError(RL_NODESTATE); 
  return bNodeFlagState.bPrmFault;
}
bool rlCanOpenTypes::get_nodestateFlag_GuardActive()
{
  if (typenumber != RL_NODESTATE) invalidTypeError(RL_NODESTATE); 
  return bNodeFlagState.bGuardActive;
}
bool rlCanOpenTypes::get_nodestateFlag_Deactivated()
{
  if (typenumber != RL_NODESTATE) invalidTypeError(RL_NODESTATE); 
  return bNodeFlagState.bDeactivated;
}


bool rlCanOpenTypes::translate_CanOpenType(int _canopentype)
{
  switch (_canopentype){
    case rlCanOpenTypes::INTEGER24_t :
      if (t_int<0) {
        t_int = t_int * -1;
        t_databytes[3] = 00;
        t_int = t_int * -1;
      }
      else{
        t_databytes[3] = 00;
      } 
    break;
    
    case rlCanOpenTypes::INTEGER40_t :
      if (t_longlong<0) {
        t_longlong = t_longlong * -1;
        t_databytes[5] = 00;
        t_databytes[6] = 00;
        t_databytes[7] = 00;
        t_longlong = t_longlong * -1;
      }
      else{
        t_databytes[5] = 00;
        t_databytes[6] = 00;
        t_databytes[7] = 00;
      } 
    break;
    	
    case rlCanOpenTypes::INTEGER48_t :return true;  
      if (t_longlong<0) {
        t_longlong = t_longlong * -1;
        t_databytes[6] = 00;
        t_databytes[7] = 00;
        t_longlong = t_longlong * -1;
      }
      else{
        t_databytes[6] = 00;
        t_databytes[7] = 00;
      } 
    break;


    case rlCanOpenTypes::INTEGER56_t :return true;  
      if (t_longlong<0) {
        t_longlong = t_longlong * -1;
        t_databytes[7] = 00;
        t_longlong = t_longlong * -1;
      }
      else{
        t_databytes[7] = 00;
      } 
    break;

    case rlCanOpenTypes::UNSIGNED24_t:
      t_databytes[3] = 00;
    break;
    case rlCanOpenTypes::UNSIGNED40_t:
      t_databytes[5] = 00;
      t_databytes[6] = 00;      
      t_databytes[7] = 00;      
    break;    
    case rlCanOpenTypes::UNSIGNED48_t:
      t_databytes[6] = 00;      
      t_databytes[7] = 00;      
    break;        
    case rlCanOpenTypes::UNSIGNED56_t:
      t_databytes[7] = 00;      
    break;        
    default                          :return false;
  };
  
  return false;
}
                          
int  rlCanOpenTypes::get_CanOpenTypeLength(int _canopentype)
{
  switch (_canopentype){
    case rlCanOpenTypes::INTEGER24_t :return 3;  
    case rlCanOpenTypes::INTEGER40_t :return 5;  
    case rlCanOpenTypes::INTEGER48_t :return 6;  
    case rlCanOpenTypes::INTEGER56_t :return 7;  
    case rlCanOpenTypes::UNSIGNED24_t:return 3;  
    case rlCanOpenTypes::UNSIGNED40_t:return 5;  
    case rlCanOpenTypes::UNSIGNED48_t:return 6; 
    case rlCanOpenTypes::UNSIGNED56_t:return 7;   
    default                          :return false;
  }; 
}   
#endif

