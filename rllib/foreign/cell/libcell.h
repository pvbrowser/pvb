
/* 
   Revised 5/7/02 William Hays - CTI 

   5/7/2002 WRH  Added ms_timeout to comm_header for faster error comm loss handling
   5/7/2002 WRH  included tcp.h to support setsockopt changes in attach.c
*/
#ifdef WIN32
#define __EXPORT __declspec(dllexport)
#include <windows.h>
#include <winsock.h>
#include <memory.h>
#include <stdarg.h>
#endif

typedef unsigned char byte;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <time.h>


#ifdef unix
#define __EXPORT
#define closesocket close
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#define OK 0
#define NOHOST -1
#define NOCONNECT -2
#define BADADDR -3

/* 

DEBUG stuff is here.  This defines a debug printing macro called dprint.
This function should work just like a simple printf statement that is
conditioned by an "if" statement.

NOTE: A debug level includes all lower debug levels.  For example: a debug 
level of DEBUG_TRACE will also turn on DEBUG_VALUES.

The various debug levels are as follows:
0 - NIL   	Print no debug info (default level)
1 - VALUES	Print values as they are discovered
2 - TRACE	Print entry and exit from various routines
3 - DATA	Print data as it is sent and received from the CL
4 - BUILD	Basically print everything, including the building of 
		Data packet information.
		
		
*/	

#ifndef WIN32
#define dprint(a,b,c...)   if (debug>=a) fprintf (stdout,b, ## c)
#endif

#ifdef WIN32
#ifndef HAVE_DPRINT
#define HAVE_DPRINT
__EXPORT int debug;
__EXPORT extern void dprint(int a,const char *b,...);
#endif
#endif

#define DEBUG_NIL 0
#define DEBUG_VALUES 1
#define DEBUG_TRACE 2
#define DEBUG_DATA 3
#define DEBUG_BUILD 4
 

#define TRUE 1
#define FALSE 0


#define DESCRIPTION_PROP 0x01
#define VALUE_PROP 0x02


/*              * * * Data Types * * *               */

#define PROGRAM 0x69
#define MAP 0x68
#define COUNTER 0x82
#define TIMER 0x83
#define PID 0x84
#define BOOL 0xc1
#define SINT 0xc2
#define INT 0xc3
#define DINT 0xc4
#define LINT 0xc5
#define USINT 0xc6
#define UINT 0xc7
#define UDINT 0xc8
#define ULINT 0xc9
#define REAL 0xca
#define LREAL 0xcb
#define STIME 0xcc
#define DATE 0xcd
#define TIME_OF_DAY 0xce
#define DATE_AND_TIME 0xcf
#define STRING 0xd0
#define BYTE 0xd1
#define WORD 0xd2
#define DWORD 0xd3
#define LWORD 0xd4
#define STRING2 0xd5
#define FTIME 0xd6
#define LTIME 0xd7
#define ITIME 0xd8
#define STRINGN 0xd9
#define SHORT_STRING 0xda
#define TIME 0xdb
#define EPATH 0xdc
#define ENGUNIT 0xdd

#define PID_EN 0x80000000
#define PID_CT 0x40000000
#define PID_CL 0x20000000
#define PID_PVT 0x10000000
#define PID_DOE 0x08000000
#define PID_SWM 0x04000000
#define PID_CA 0x02000000
#define PID_MO 0x01000000
#define PID_PE 0x00800000
#define PID_NDF 0x00400000
#define PID_NOBC 0x00200000
#define PID_NOZC 0x00100000
#define PID_INI 0x00008000
#define PID_SPOR 0x00004000
#define PID_OLL 0x00002000
#define PID_OLH 0x00001000
#define PID_EWD 0x00000800
#define PID_DVNA 0x00000400
#define PID_DVPA 0x00000200
#define PID_PVLA 0x00000100
#define PID_PVHA 0x00000080

#define TIMER_EN 0x80
#define TIMER_TT 0x40
#define TIMER_DN 0x20
#define TIMER_FS 0x10
#define TIMER_LS 0x08
#define TIMER_OV 0x04
#define TIMER_ER 0x02

#define COUNTER_CU 0x80
#define COUNTER_CD 0x40
#define COUNTER_DN 0x20
#define COUNTER_OV 0x10
#define COUNTER_UN 0x08


/* These are the objects - things that you send commands to...  */

#define IDENTITY 0x01
#define ROUTER 0x02
#define CONNECTION 0x05
#define CONNECTION_MANAGER 0x06
#define REGISTER 0x07
#define BACKPLANE_DATA 0x66
#define OBJECT_SUB_OBJECT 0x68
#define OBJECT_CONFIG 0x6a
#define OBJECT_DETAILS 0x6b
#define STRUCT_LIST 0x6c
#define DATA_OBJECT 0x72
#define PORT_OBJECT 0xf4

/* These are the "services" - commands by any other name...  */

#define GET_ATTRIBUTE_ALL 0x01
#define SET_ATTRIBUTE_ALL 0x02
#define GET_ATTRIBUTE_LIST 0x03
#define SET_ATTRIBUTE_LIST 0x04
#define RESET 0x05
#define START 0x06
#define STOP 0x07
#define CREATE 0x08
#define CIP_DELETE 0x09
#define APPLY_ATTRIBUTES 0x0d
#define GET_ATTRIBUTE_SINGLE 0x0e
#define SET_ATTRIBUTE_SINGLE 0x10
#define FIND_NEXT_OBJECT 0x11
#define RESTORE 0x15
#define SAVE 0x16
#define NOP 0x17
#define GET_MEMBER 0x18
#define SET_MEMBER 0x19
#define INSERT_MEMBER 0x1a
#define REMOVE_MEMBER 0x1b
#define CONN_KICK_TIMER 0x4b
#define CONN_OPEN 0x4c
#define CONN_CLOSE 0x4d
#define CONN_STOP 0x4e
#define CONN_CHANGE_START 0x4f
#define CONN_GET_STATUS 0x50
#define CONN_CHANGE_COMPLETE 0x51
#define CONN_AUDIT_CHANGE 0x52
#define LIST_ALL_OBJECTS 0x4b
#define DATA_TYPE_CONFIG 0x4c
#define GET_SINGLE_PROP 0x4f
#define PUT_SINGLE_PROP 0x50




#define MAX_INSTANCE 0x02
#define NUM_INSTANCES 0x03
#define ENTRY_PORT 0x08
#define ALL_PORTS 0x09
 



#define BASE_INSTANCE 0x00
#define FIRST_INSTANCE 0x01
#define SECOND_INSTANCE 0x02

/* TPC = Transport Class */

#define TPC_NULL 0x00
#define TPC_DUPLICATE_DETECT 0x01
#define TPC_ACKNOWLEDGED 0x02
#define TPC_VERIFIED 0x03
#define TPC_NONBLOCKING 0x04
#define TPC_FRAGMENTING 0x05
#define TPC_MULTICAST_FRAG 0x06

/* TPM = Trigger Mode */

#define TPM_CYCLIC 0x00
#define TPM_CHANGE_OF_STATE 0x01
#define TPM_APPLICATION 0x02

#define VENDOR_ID 0xfeed
#define VENDOR_SN 0xdeadbeef

#define CPH_Null 0x00
#define CPH_Connection_based 0xa1
#define CPH_Sequenced_address 0x8002
#define CPH_Transport_PDU 0xb1
#define CPH_Unconnected_message 0xb2
#define CPH_Sockinfo_ot 0x8000
#define CPH_Sockinfo_to 0x8001

#define PDU_Forward_Close 0x4e
#define PDU_Unconnected_Send 0x52
#define PDU_Forward_Open 0x54
#define PDU_GetConnectionData 0x56
#define PDU_SearchConnectionData 0x57
#define PDU_ExForwardOpen 0x59
#define PDU_GetConnectionOwner 0x5a



#define ENCAPS_NOP 0x00
#define ENCAPS_List_Targets 0x01
#define ENCAPS_List_Services 0x04
#define ENCAPS_ListIdentity 0x63
#define ENCAPS_List_Interfaces 0x64
#define ENCAPS_Register_Session 0x65
#define ENCAPS_UnRegister_Session 0x66
#define ENCAPS_SendRRData 0x6f
#define ENCAPS_SendUnitData 0x70
#define ENCAPS_Header_Length 24


#define SERVICE_Communications 0x100

#define CELL_DFLT_TIMEOUT 500  /* Default sleep value (ms) */
#define CELL_MAX_TAGS 2048
#define CELL_MAX_PROGS 128
#define CELL_MAX_SBASE 256
#define CELL_MAX_RACKID 32


#define DATA_Buffer_Length 20480

#pragma pack(1)

typedef struct {
unsigned short type;
unsigned short length;
unsigned short version;
unsigned short flags;
byte name[16];
} _services;

typedef struct {
unsigned short type;
char name[32];
byte *data;
} _element;

typedef struct {
unsigned long base;
unsigned short linkid;
unsigned short count;
unsigned short detailsize;
char name[32];
_element *data[64];
} _struct_base;

typedef struct {
unsigned short count;
_struct_base *base[CELL_MAX_SBASE];
} _struct_list;

typedef struct {
unsigned long base;
unsigned long linkid;
byte name[32];
} _prog_detail;

typedef struct {
_prog_detail *prog[CELL_MAX_PROGS];
int count;
} _prog_list;

typedef struct {
unsigned long topbase;
unsigned long base;
unsigned long alias_topbase;
unsigned long alias_base;
unsigned long id;
unsigned long alias_id;
unsigned long linkid;
unsigned long memory;
unsigned long alias_linkid;
unsigned short type;
unsigned short alias_type;
unsigned short displaytype;
unsigned short size;
unsigned short alias_size;
unsigned long arraysize1;
unsigned long arraysize2;
unsigned long arraysize3;
unsigned short datalen;
byte dirty;
byte *data;	
byte name[32];
} _tag_detail;

typedef struct {
_tag_detail *tag[CELL_MAX_TAGS];
int count;
int reference;
} _tag_data;

typedef struct {
int count;
_tag_detail *tag[10];
} _tag_list;

typedef struct {
long class;
long instance;
long member;
long point;
long attribute;
byte *tagname;
} _ioi_data;

typedef struct {
byte rx_bad_m;
byte err_threshold;
byte rx_bad_crc;
byte rx_bus_timeout;
byte tx_bad_crc;
byte tx_bus_timeout;
byte tx_retry;
byte status;
unsigned short address;
byte rev_major;
byte rev_minor;
unsigned long serial_number;
unsigned short rack_size;
} _backplane_data;

typedef struct {
short number1;
byte number2;
byte slot;
short number3;
short number4;
} _port;

typedef struct {
short family;
unsigned short port;
unsigned long address;
byte zero[8];
} _sock;

typedef struct {
unsigned short ID;
unsigned short type;
unsigned short product_code;
byte rev_hi;
byte rev_lo;
short status;
unsigned long serial;
byte namelen;
char name[32];
} _identity;

typedef struct {
int size;
int slot;
int cpulocation;
_identity *identity[CELL_MAX_RACKID];
} _rack;


typedef struct {
short number1;
short number2;
short len;
short number3;
short number4;
short number5;
unsigned short port;
unsigned long ip_addr;
short number6;
short number7;
short number8;
short number9;
char address[16];
} _interfaces;


typedef struct {
byte data[DATA_Buffer_Length];
unsigned short len;
unsigned short overall_len; 
} _data_buffer;

typedef struct {
int device1;
int device2;
int device3;
int device4;
int device5;
int device6;
int device7;
int device8;
} _path;

#define BUFFER_LENGTH DATA_Buffer_Length - ENCAPS_Header_Length

typedef struct {

unsigned short command;
unsigned short len;
unsigned long handle;
unsigned long status;
byte context[8];
unsigned long options;
} _encaps_header;

typedef struct {
unsigned long session_id;
unsigned long ms_timeout;  /* Communications layer timeout milliseconds */
int file_handle;
byte *hostname;
byte context[8];
char error;
} _comm_header;

typedef struct {
unsigned long ctl;
float sp;
float kp;
float ki;
float kd;
float bias;
float maxs;
float mins;
float db;
float so;
float max0;
float min0;
float upd;
float pv;
float err;
float out;
float pvh;
float pvl;
float dvp;
float dvn;
float pvdb;
float dvdb;
float max1;
float min1;
float tie;
float maxcv;
float mincv;
float mintie;
float maxtie;
float data[17];
} _PID ;

typedef struct {
byte heartbeat[3];
byte ctl;
long preset;
long accumulator;
} _timer;

typedef struct {
byte junk[3];
byte ctl;
long preset;
long accumulator;
} _counter;


extern int cell_errno;
extern char cell_err_msg[256];
extern int cell_line;
extern char cell_file[256];

#define CELLERROR(num,msg) { cell_line=__LINE__; strcpy(cell_file,__FILE__); cell_errno = num; strcpy(cell_err_msg,msg); }

#pragma pack()

extern char *PROFILE_Names[];
extern char *ENCAPS_errors[];
extern int ENCAPS_errors_id[];
extern int ENCAPS_error_count;


__EXPORT extern void cell_perror(const char *s, int debug);
__EXPORT extern int ioi ( _data_buffer *buff, _ioi_data *ioi1, _ioi_data *ioi2, int debug);
__EXPORT extern int list_interfaces (_comm_header *comm, int debug);
__EXPORT extern int list_services (_comm_header *comm, _services *services, int debug);
__EXPORT extern int readdata (_data_buffer *buff, _comm_header *comm, int debug);
__EXPORT extern int senddata (_data_buffer *buff, _comm_header *comm, int debug);
__EXPORT extern int nop (_comm_header *comm, int debug);
__EXPORT extern int register_session (_comm_header *comm, int debug);
__EXPORT extern int unregister_session (_comm_header *comm, int debug);
extern void fill_header (_comm_header *comm, _encaps_header *head, int debug);
__EXPORT extern void establish_connection (_comm_header *comm, _services *services, int debug);
__EXPORT extern void attach (_comm_header *comm1, int debug);
__EXPORT extern int list_targets (_comm_header *comm, int debug);
extern int sendRRdata ( unsigned short timeout, _comm_header *comm, _encaps_header *head, _data_buffer *buff, int debug);
extern void cpf_data (short datatype, _data_buffer *cpf_data, _data_buffer *buff, int debug);
extern void cpf (short addrtype, _data_buffer *addrbuff, short datatype, _data_buffer *databuff, _data_buffer *buff, int debug);
__EXPORT extern int get_backplane_data (_comm_header *comm, _backplane_data *backplane, _rack *rack, _path *path, int debug);
extern int unconnected_send (_data_buffer *buff, int debug);
extern int makepath (_path *path, _data_buffer *buff, int debug);
extern int settimeout (byte priority, byte timeout, _data_buffer *buff, int debug);
__EXPORT extern int get_device_data (_comm_header *comm, _path *path, _identity *identity, int debug);
__EXPORT extern int get_port_data (_comm_header *comm, _path *path, _port *port, int debug);
__EXPORT extern int who (_comm_header *comm, _rack *rack, _path *path, int debug);
__EXPORT extern int get_object_config_list (_comm_header *comm, _path *path, unsigned long base, _tag_data *tags, int debug);
__EXPORT extern int get_object_details_list (_comm_header *comm, _path *path, unsigned long base, _tag_data *tags, int debug);
__EXPORT extern int get_program_list (_comm_header *comm, _path *path, _prog_list *progs, int debug);
__EXPORT extern int get_program_details (_comm_header *comm, _path *path, _prog_detail *prog, int debug);
__EXPORT extern int get_object_config (_comm_header *comm, _path *path, _tag_detail *tag, int debug);
__EXPORT extern int get_object_details (_comm_header *comm, _path *path, _tag_detail *tag, int debug);
__EXPORT extern unsigned short getsize (_tag_detail *tag);
__EXPORT extern int read_object_value (_comm_header *comm, _path *path, _tag_detail *tag, int debug);
__EXPORT extern int write_object_value (_comm_header *comm, _path *path, _tag_detail *tag, int debug);
__EXPORT extern int read_multi_object_value (_comm_header *comm, _path *path, _tag_list *list, int debug);
__EXPORT extern int get_struct_list (_comm_header *comm, _path *path, _struct_list *structs, int debug);
__EXPORT extern int get_struct_details (_comm_header *comm, _path *path, _struct_base *tag, int debug);
__EXPORT extern int get_struct_config (_comm_header *comm, _path *path, _struct_base *tag, int debug);
__EXPORT extern int aliascheck (_tag_detail *tag, _tag_data *mainlist, _tag_data *proglist, int debug);

