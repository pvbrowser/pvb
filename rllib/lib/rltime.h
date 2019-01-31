
/***************************************************************************
                          rltime.h  -  description
                             -------------------
    begin                : Tue Jan 02 2001
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
#ifndef _RL_TIME_H_
#define _RL_TIME_H_

#include "rldefine.h"

// define default version to use
#ifndef USE_RLTIME_V1
#ifndef USE_RLTIME_V2
#define USE_RLTIME_V2
#endif
#endif

#ifdef USE_RLTIME_V1
#warning "We use rltime_v1.cpp"
#include "rltime_v1.h"
using namespace ns_rltime_v1;
#endif

#ifdef USE_RLTIME_V2
#warning "We use rltime_v2.cpp"
#include "rltime_v2.h"
using namespace ns_rltime_v2;
#endif

#endif
