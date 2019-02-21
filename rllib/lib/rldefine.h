/***************************************************************************
                          rldefine.h  -  description
                             -------------------
    begin                : Wed Dec 04 2002
    copyright            : (C) 2001 by R. Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
/*! <pre>
The header that is included in every file of rllib.
</pre> */
#ifndef _RL_DEFINE_H_
#define _RL_DEFINE_H_

// CPP Standard
#if __cplusplus >= 199711L
#define RLCPP98
#endif

#if __cplusplus >= 201103L
#define RLCPP11
#endif

#if __cplusplus >= 201402L
#define RLCPP14
#endif

#if __cplusplus >= 201703L
#define RLCPP17
#endif


// define WIN
#ifdef _WIN32
#define RLWIN32
#include <winsock2.h>
#include <windows.h>
#define WTHREAD_GNUC0 ( __GNUC__ * 1000 ) + __GNUC_MINOR__
#if WTHREAD_GNUC0 >= 4008
#endif
#endif

// we always use this and are very lazy
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// define unix if not already defined
#ifdef __unix__
#ifndef unix
#define unix
#endif
#endif
#ifdef unix
#define RLUNIX
#endif

#if defined(__APPLE__) && defined(__MACH__)
#ifndef RLUNIX
#define RLUNIX
#endif
#endif

#define rl_PRINTF_LENGTH             4096
#define rl_PRINTF_LENGTH_SPREADSHEET 4096

#define BIT0  1
#define BIT1  2
#define BIT2  4
#define BIT3  8
#define BIT4  16
#define BIT5  32
#define BIT6  64
#define BIT7  128
#define BIT8  256*1
#define BIT9  256*2
#define BIT10 256*4
#define BIT11 256*8
#define BIT12 256*16
#define BIT13 256*32
#define BIT14 256*64
#define BIT15 256*128
#define BIT16 256*256*1
#define BIT17 256*256*2
#define BIT18 256*256*4
#define BIT19 256*256*8
#define BIT20 256*256*16
#define BIT21 256*256*32
#define BIT22 256*256*64
#define BIT23 256*256*128
#define BIT24 256*256*256*1
#define BIT25 256*256*256*2
#define BIT26 256*256*256*4
#define BIT27 256*256*256*8
#define BIT28 256*256*256*16
#define BIT29 256*256*256*32
#define BIT30 256*256*256*64
#define BIT31 256*256*256*128

#define RLCRLF "\r\n"

#if __cplusplus >= 201103
#define RLCPP11
#endif

#endif
