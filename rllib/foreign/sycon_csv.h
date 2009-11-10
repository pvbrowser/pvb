#define CSV_STATIONADDRESS  1
#define CSV_RECORDTYPE      2
#define CSV_IDENTNUMBER     3
#define CSV_VENDORNUMBER    4
#define CSV_VENDORNAME      5
#define CSV_DEVICE          6
#define CSV_DESCRIPTION     7
#define CSV_MASTERADDRESS   8
#define CSV_SETTINGS        9
#define CSV_RESERVED        10
#define CSV_MODULCOUNT      11
#define CSV_DATASIZE0       12
#define CSV_DATATYPE0       13
#define CSV_DATAPOSITION0   14
#define CSV_ADDRESS0        15

#define CSV_MAX_NUMBER      59
 
#define CSV_DATASIZE_WORD          0x01 // else byte
#define CSV_DATASIZE_LITTLE_ENDIAN 0x02 // else big endian
#define CSV_DATATYPE_