

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

char *errors[] = {
	"No Error",
	"Illegal Command or Format",
	"Host has a problem and will not communicate",
	"Remote Node Host is missing, disconnected, or shutdown",
	"Host could not complete function due to hardware fault",
	"Addressing problem or memory protected rungs",
	"Function not allowed due to command protection selection",
	"Processor is in Program Mode",
	"Compatability mode file missing or communication zone problem",
	"Remote node cannot buffer command",
	"Wait ACK (1775-KA buffer full)",
	"Remote Node problem due to download",
	"Wait ACK (1775-KA buffer full)",
	"Not Used 1",
	"Not Used 2",
	"Extended error data received"};

char *slcmodes[] = {
	"Download",
	"Remote Program",
	"reserved - 2",
	"Idle - SUS command",
	"reserved - 4",
	"reserved - 5",
	"Run",
	"Test - Continuous Scan",
	"Test - Single Scan",
	"Test - Single Step",
	"reserved - 10",
	"reserved - 11",
	"reserved - 12",
	"reserved - 13",
	"reserved - 14",
	"reserved - 15",
	"Download",
	"Program",
	"reserved - 18",
	"reserved - 19",
	"reserved - 20",
	"reserved - 21",
	"reserved - 22",
	"reserved - 23",
	"reserved - 24",
	"reserved - 25",
	"reserved - 26",
	"Idle - SUS command",
	"reserved - 28",
	"reserved - 29",
	"Run"};
	


char *runmode[] = {
	"Program Load",
	"Not Used 1",
	"Run Mode",
	"Not Used 2",
	"Remote Program Load",
	"Remote Test",
	"Remote Run",
	"Not Used 3"};

char *plc_id[] = {
	"1785 - L40B (PLC5/40 ) ",
	"1785 - LT4  (PLC5/10 ) ",
	"1785 - L60B (PLC5/60 ) ",
	"1785 - L40L (PLC5/40L) ",
	"1785 - L60L (PLC5/60L) ",
	"1785 - L11B (PLC5/11 ) ",
	"1785 - L20B (PLC5/20 ) ",
	"1785 - L30B (PLC5/30 ) ",
	"1785 - L20E (PLC5/20E) ",
	"1785 - L40E (PLC5/40E) ",
	"1785 - L80B (PLC5/25 ) ",
	"1785 - L80E (PLC5/80E) ",
	"1785 - xxxx (SoftLogix PLC)"};
int id_count=17;

int plc_index[] = 	{0x15, 0x22, 0x23, 0x28, 0x29, 0x31, 0x32, 0x33,
			 0x4a, 0x4b, 0x55, 0x59, 0x7b};

char *ext_errors[] = {
	"No Error",
	"A field has an illegal value",
	"Less Levels specified in address than minimum for any address",
	"More Levels specified in address than system supports",
	"Symbol Not Found",
	"Symbol is of improper format",
	"Address doesn't point to something usable",
	"File is wrong size",
	"Can not complete request, situation has changed.",
	"Data or file is too large",
	"Transaction size plus word address is too large",
	"Access denied, improper privilege",
	"Condition can not be generated",
	"Condition already exists",
	"Command cannot be executed",
	"Histogram Overflow",
	"No access",
	"Illegal Data Type",
	"Invalid parameter or data type",
	"Address reference exists to deleted area",
	"Command execution failure for unknown reason",
	"Data Conversion error",
	"Scanner not able to communicate with 1771 rack adaptor",
	"Type Mismatch",
	"1771 module response was not valid",
	"Duplicated label",
	"Remote Rack fault",
	"Timeout",
	"Unknown error",
	"File is open, another node owns it",
	"Another node is program owner",
	"Reserved",
	"Reserved",
	"Data table element protection violation",
	"Temporary internal problem"};


char *datatypes[] = 	{"bit",
			"bit string",
			"byte string",
			"integer",
			"timer",
			"counter",
			"control",
			"IEEE floating point",
			"array (byte)",
			"not defined - 10",
			"not defined - 11",
			"not defined - 12",
			"Rung data",
			"not defined - 14",
			"address data",
			"BCD"};

char *addrtypes[] = 	{"O",	//Output
			"I",	//Input
			"S",	//Status
			"B",	//Binary
			"T",	//Timer
			"C",	//Counter
			"R",	//Control
			"N",	//Integer
			"F",	//Float
			"A",	//ASCII
			"D",	//BCD
			"BT",	//Block Transfer
			"L",	//Long Integer
			"MG",	//Message
			"PD",	//PID
			"SC",	// ??
			"ST",	//String
			"PN",	//PLC Name
			"RG",	//Rung
			"FO",	//Output Force Table
			"FI",	//Input Force Table
			"XA",	//Section 3 File
			"XB",	//Section 4 File
			"XC",	//Section 5 File
			"XD",	//Section 6 File
			"FF"};	// Force File Section

int addrtypescount = 26;


