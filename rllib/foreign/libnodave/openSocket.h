/*
 Part of Libnodave, a free communication libray for Siemens S7 300/400.
 
 (C) Thomas Hergenhahn (thomas.hergenhahn@web.de) 2002, 2003.2004

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

#ifndef __openSocket
#define __openSocket

#ifdef BCCWIN
#ifdef DOEXPORT
#define EXPORTSPEC __declspec (dllexport)
#else
#define EXPORTSPEC __declspec (dllimport)
#endif
EXPORTSPEC int openSocket(const int port, const char * peer);
#endif

#ifdef LINUX
#define EXPORTSPEC
int openSocket(const int port, const char * peer);
#endif

#ifdef CYGWIN
#define EXPORTSPEC
int openSocket(const int port, const char * peer);
#endif
#endif
/*
    Changes: 
    07/12/2003	moved openSocket to it's own file, because it can be reused in other TCP clients
    04/07/2004  ported C++ version to C
    12/17/2004  additonal defines for WIN32
*/
