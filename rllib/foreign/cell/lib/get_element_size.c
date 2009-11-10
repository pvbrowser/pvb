/* Program to return the size of an individual element.
   Copyright (c) 2002 any beyond - Ron Gage.  */


#include "libcell.h"

int get_element_size (int type)
{
int size = 0;


switch (type & 255)
	{
	case 0xc2:
	case 0xc6:
	case 0xd1:
		size = 1;
		break;
	case 0xc3:
	case 0xc7:
	case 0xd2:
		size = 2;
		break;
	case 0xc4:
	case 0xc8:
	case 0xca:
	case 0xd3:
		size = 4;
		break;
	case 0xc5:
	case 0xc9:
	case 0xcb:
	case 0xd4:
		size = 8;
		break;
	case 0x82:
		size = sizeof (_counter);
		break;
	case 0x83:
		size = sizeof (_timer);
		break;
	case 0x84:
		size = sizeof (_PID);
		break;
	default:
		size =0;
	}
return size;
}
