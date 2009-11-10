
#define TRUE 1
#define FALSE 0

#define CONNECT_CMD 1
#define PCCC_CMD 7
#define DH_CMD 6

#define _ENET_HEADER_LEN 28
#define _CUSTOM_LEN 16
#define PCCC_DATA_LEN 9
#define PCCC_WORD_LEN 13
#define PCCC_MEM_LEN 14

#define PCCC_VERSION 4
#define PCCC_BACKLOG 5

#define ETHERNET 1
#define RS232 2
#define KT 3

#define READ_ONLY 1
#define READ_WRITE 3

#define OK 0
#define NOHOST -1
#define BADADDR -2
#define NOCONNECT -3
#define BADCMDRET -1

#define PROGRAM_MODE 0
#define TEST_MODE 1
#define RUN_MODE 2
#define SLC_PROGRAM_MODE 1
#define SLC_RUN_MODE 6
#define SLC_TEST_CONT 7
#define SLC_TEST_SINGLE 8
#define SLC_TEST_DEBUG 9
#define MICRO_PROGRAM_MODE 1
#define MICRO_RUN_MODE 2


#define PLC5 1
#define PLC5250 2
#define SLC 3
#define MICRO 4

typedef unsigned char byte;

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

#pragma pack(1)

struct floatstuff
{
unsigned short data1;
unsigned short data2;
};

union cdata
{
unsigned short idata[2];
float fdata;
};

struct _file
{
byte protection;
short number;
byte type;
};

struct results
{
byte sts;
byte extsts;
byte good;
};


struct pccc_data
{
byte dst;
byte control;
byte src;
byte lsap;
byte cmd;
byte sts;
short tns;
byte fnc;
byte data[250];
};


struct pccc_normal
{
byte dst;
byte control;
byte src;
byte lsap;
byte cmd;
byte sts;
short tns;
byte fnc;
};

struct pccc_memread
{
byte dst;
byte control;
byte src;
byte lsap;
byte cmd;
byte sts;
short tns;
byte fnc;
long addr;
byte count;
};

struct pccc_word_read
{
byte dst;
byte control;
byte src;
byte lsap;
byte cmd;
byte sts;
short tns;
byte fnc;
short offset;
short trans;
byte data[240];
};

struct plc5stat 
{
byte mode;
byte faulted;
byte download;
byte testedits;
byte plctype;
byte offline;
byte edithere;
byte type;
byte expansion_type;
byte interface;
byte series;
byte rev;
unsigned long memsize;
byte dhaddr;
byte io_addr;
byte io_conf;
unsigned short tablefiles;
unsigned short progfiles;
byte forces_here;
byte forces_on;
byte memprot;
byte mem_bad;
byte debug;
short hold_file;
short hold_element;
short edit_time_sec;
short edit_time_min;
short edit_time_hour;
short edit_time_day;
short edit_time_month;
short edit_time_year;
byte backup_active;
byte partner_active;
byte edit_resource_allocated;
byte outputs_reset;
byte sfc_forces_active;
short prog_change_count;
short data_change_count;
short user_data_change_count;
byte name[11];
short error_word;
byte comm_active;
byte protection_power_loss;
byte startup_protect_fault;
byte load_rom_on_error;
byte load_rom_always;
byte load_rom_and_run;
byte plc_halted;
byte locked;
byte first_scan;
short program_id;
byte program_owner;
byte cmdport;
};


struct ethernet_header
{
byte mode;
byte submode;
unsigned short pccc_length;
unsigned long conn;
unsigned long status;
byte custom[16];
byte df1_stuff[255];
};

struct custom_connect
{
short version;
short backlog;
byte junk[12];
};

struct custom_pccc
{
unsigned long request_id;
unsigned long name_id;
byte junk[8];
};

struct custom_route
{
unsigned long request_id;
unsigned long name_id;
byte pushwheel;
byte module;
byte channel;
byte protocol;
byte flags;
byte junk[3];
};


struct _comm
{
short handle;
short error;
short tns;
long commid;
byte mode;
};


struct _upload_data
{
byte sts;
byte extsts;
byte uploadable_seg_count;
long upload_seg_start[100];
long upload_seg_end[100];
byte comparable_seg_count;
long comparable_seg_start[100];
long comparable_seg_end[100];
};


struct namedata
{
int module;
int area;
int subarea;
int section;
int file;
int element;
int subelement;
int mask;
int type;
int typelen;
int bit;
byte len;
byte floatdata;
byte data[24];
};

struct _size_data
{
short size_lo;
short size_hi;
short count;
byte write_priv;
byte read_priv;
byte type;
byte sts;
byte extsts;
};

struct _data
{
struct namedata name;
short len;
byte floatdata;
unsigned short data[250];
};

struct _typed_data
{
struct namedata name;
byte maintype;
byte type;
byte mainlen;
byte typelen;
byte floatdata;
byte len;
short data[240];
};

#pragma pack()

extern int errno;
extern char *ext_errors[];
extern char *errors[];
extern char *runmode[];
extern char *plc_id[];
extern char *slcmodes[];
extern int id_count;
extern int plc_index[];
extern char *datatypes[];
extern char *addrtypes[];
extern int addrtypescount;

extern struct namedata nameconv5(char *dataaddr, int plctype, int debug);
extern struct _comm abel_attach (char *host, int debug);
extern struct plc5stat getstatus (struct _comm comm1, int debug);
extern struct results uploaddone (struct _comm comm1, int debug);
extern struct _upload_data uploadreq (struct _comm comm1, int debug);
extern struct results setplcmode (struct _comm comm1, byte plctype, byte plcmode, int debug);
extern struct _typed_data typed_read (struct _comm comm1, char *addr, int count, int plctype, int debug);
extern struct _data typed_write (struct _comm comm1, char *addr, int count,
      struct _data data, int plctype, int debug);
extern struct _data readphysical (struct _comm comm1, long start_addr,
      int count, int debug);
  
extern float itof (unsigned int data1, unsigned int data2);
extern struct floatstuff ftoi (float data);   
extern struct results getedit (struct _comm comm1, int debug);
extern struct results dropedit (struct _comm comm1, int debug);
extern struct results resetmemory (struct _comm comm1, int debug);
extern struct results creatememory (struct _comm comm1, char *addr, int plctype, int debug);
extern struct _size_data readsectionsize (struct _comm comm1, struct namedata name, int plctype, int debug); 
extern struct results setpriv (struct _comm comm1, int debug);
extern struct _data protread3 (struct _comm comm1, char *addr, int count, int plctype, int debug);
extern struct _data protwrite3 (struct _comm comm1, char *addr, int count, struct _data outdata, int plctype, int debut);
extern struct _data protread2 (struct _comm comm1, char *addr, int count, int plctype, int debug);
extern struct _data protwrite2 (struct _comm comm1, char *addr, int count, struct _data outdata, int plctype, int debug);
extern struct _data protread4 (struct _comm comm1, char *addr, int count, int bitmask, int plctype, int debug);
extern struct _data protwrite4 (struct _comm comm1, char *addr, int count, struct _data outdata, int mask, int plctype, int debug);
extern struct ethernet_header senddf1 (struct _comm comm1, struct ethernet_header header, int hlen, int debug);
extern struct results downloaddone (struct _comm comm1, int debug);
extern struct results downloadreq (struct _comm comm1, int debug);
extern struct results enableforces (struct _comm comm1, int debug);
extern struct results disableforces (struct _comm comm1, int debug);
extern struct results closefile (struct _comm comm1, int filetag, int debug);
extern struct results echo (struct _comm comm1, struct _data data, int debug);
extern struct _data fileread (struct _comm comm1, char *addr, int count, int plctype, int debug);
extern struct _data filewrite (struct _comm comm1, char *addr, int count, 
      struct _data outdata, int plctype, int debug);
extern struct _data word_read (struct _comm comm1, char *addr, int count, int plctype, int debug);
extern struct _data word_write (struct _comm comm1, char *addr, int count, 
      struct _data outdata, int plctype, int debug);
extern struct _data openfile (struct _comm comm1, struct _file file, int debug);
extern char *get_version(void);
