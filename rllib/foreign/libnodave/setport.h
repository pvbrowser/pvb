/*
 (C) Thomas Hergenhahn (thomas.hergenhahn@nexans.com) 2001.

 Libnodave is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2, or (at your option)
 any later version.

 Libnodave is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Libnodave; see the file COPYING.  If not, write to
 the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  
*/


#ifdef BCCWIN

#ifdef DOEXPORT
#define EXPORTSPEC __declspec (dllexport)
#else
#define EXPORTSPEC __declspec (dllimport)
#endif

EXPORTSPEC HANDLE WINAPI setPort(char * name, char* baud,char parity);
#endif


#ifdef LINUX
int setPort(char * name, char* baud,char parity);
#endif

#ifdef CYGWIN
int setPort(char * name, char* baud,char parity);
#endif
