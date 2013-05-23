/***************************************************************************
                          rlstate.cpp  -  description
                             -------------------
    begin                : Sat Dec 29 2012
    copyright            : (C) 2012 by R. Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#include "rlstate.h"

rlState::rlState()
{
  user = NULL;
  stepCounter = -1;
  cycletime = 1000;
  nextStep = NULL;
  lastState = NULL;
}

rlState::~rlState()
{
}

void rlState::gotoState( void (*funcPtr)(rlState *sm) ) 
{
  stepCounter = -1;
  lastState = nextStep;
  nextStep = funcPtr;
}

int rlState::runSteps(int _cycletime)
{
  cycletime = _cycletime;
  lastState = NULL;;
  stepCounter = 0;
  
  while(nextStep != NULL)
  {
    (*nextStep)(this);
    rlsleep(cycletime);
    stepCounter++;
    if(stepCounter > MAX_STEP) stepCounter = 1;
  }

  return 0;
}

static void *stepsThread(void *arg)
{
  THREAD_PARAM *tp = (THREAD_PARAM *) arg;
  rlState *st = (rlState *) tp->user;
  int cycletime = st->cycletime;
  st->runSteps(cycletime);
  return arg;
}

int rlState::startSteps(int _cycletime)
{
  if(cycletime < 0) return 0;
  cycletime = _cycletime;
  thread.create(stepsThread,this);
  return 0;
}

