#include "libcell.h"

#ifdef WIN32
int debug;
void dprint(int a,const char *b,...)  
{
	va_list vl; 
	va_start (vl,b); 
	if (debug>=a)  
		vfprintf(stdout,b,vl); 
	va_end(vl); 
}
#endif

char *ENCAPS_errors[] = { "Success",
  "Sender issued an unsupported command",
  "Receiver ran out of memory to handle command",
  "Incorrect Data in command (encaps section)",
  "Invalid Session Handle used.",
  "Invalid message Length",
  "Unsupported Protocol Revision"
};
int ENCAPS_errors_id[] = { 0, 1, 2, 3, 0x64, 0x65, 0x69 };
int ENCAPS_error_count = 7;
char *PROFILE_Names[] = { "Generic Device",
  "Not Defined - 0x01",
  "A/C Drives",
  "Motor Overload Device",
  "Limit Switch",
  "Inductive Prox Switch",
  "Photoelectric Sensor",
  "General Purpose Discreet I/O",
  "Not Defined - 0x08",
  "Resolver",
  "Analog I/O Device - 0x0a",
  "Not Defined - 0x0b",
  "Communications Adaptor",
  "Not Defined - 0x0d",
  "Control Processor",
  "Not Defined - 0x0f",
  "Position Controller",
  "Not Defined - 0x11",
  "Not Defined - 0x12",
  "DC Drive",
  "Not Defined - 0x14",
  "Contactor",
  "Motor Starter",
  "Soft Motor Starter",
  "Human Machine Interface",
  "Not Defined - 0x19",
  "Not Defined - 0x1a" "Pneumatic Valves",
  "Vacuum Pressure Gauge",
  "Not Defined - 0x1d",
  "Not Defined - 0x1e",
  "Not Defined - 0x1f",
  "Not Defined - 0x20",
  "Not Defined - 0x21",
  "Not Defined - 0x22",
  "Not Defined - 0x23",
  "Not Defined - 0x24",
  "Not Defined - 0x25",
  "Not Defined - 0x26",
  "Not Defined - 0x27",
  "Not Defined - 0x28",
  "Not Defined - 0x29",
  "Not Defined - 0x2a",
  "Not Defined - 0x2b",
  "Not Defined - 0x2c",
  "Not Defined - 0x2d",
  "Not Defined - 0x2e",
  "Not Defined - 0x2f",
  "Not Defined - 0x30",
  "Not Defined - 0x31",
  "ControlNet Physical Layer"
};
