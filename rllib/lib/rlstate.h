/***************************************************************************
                          rlstate.h  -  description
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
#ifndef _RL_STATE_H
#define _RL_STATE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "rlthread.h"

// frame functions
static int  readIO();
static int  runSteps(int cycletime);
static int  writeIO();
static void gotoState( void (*funcPtr)() );

// our static variables
static void (*nextStep)() = NULL;
static void (*lastState)() = NULL;
static int stepCounter = 0;
static int running = 0;

// our constants
static const long MAX_STEP=1000*1000*1000;

static void gotoState( void (*funcPtr)() ) 
{
  stepCounter = -1;
  lastState = nextStep;
  nextStep = funcPtr;
}

static int runSteps(int cycletime)
{
  lastState = NULL;;
  stepCounter = 0;
  running = 1;
  while(running && nextStep != NULL)
  {
    if(readIO() >= 0)
    {
      (*nextStep)();
      if(writeIO() < 0)
      {
        printf("ERROR: runSteps could not writeIO()\n");
      }  
    }
    else
    {
      printf("ERROR: runSteps could not readIO()\n");
    }
    rlsleep(cycletime);
    stepCounter++;
    if(stepCounter > MAX_STEP) stepCounter = 1;
  }
  running = 0;
  return 0;
}

/*** Example usage ****************************************************
#include "rlstate.h"

//TODO: define the global variables that will be readIO() and writeIO()

//TODO: define our states
static void stStart();
static void stProcess();
static void stFinish();

//TODO: implement our states
static void stStart()
{
  printf("stStart stepCounter=%d\n", stepCounter);
  if(lastState == stStart) printf("stStart:   lastState=stStart\n");
  else                     printf("stStart:   lastState!=stStart\n");
  if(stepCounter > 5) gotoState(stProcess);
}

static void stProcess()
{
  printf("stProcess stepCounter=%d\n", stepCounter);
  if(lastState == stStart) printf("stProcess: lastState=stStart\n");
  else                     printf("stProcess: lastState!=stStart\n");
  if(stepCounter > 3) gotoState(stFinish);
}

static void stFinish()
{
  printf("stFinish stepCounter=%d\n", stepCounter);
  if(lastState == stStart) printf("stFinish:  lastState=stStart\n");
  else                     printf("stFinish:  lastState!=stStart\n");
  gotoState(NULL);
}

static int readIO() // read all IO values to global variables
{
  return 0; //TODO: implement reading
}

static int writeIO() // write all IO values from global variables
{
  return 0; //TODO: implement writing
}

int main() // how to start
{
  gotoState(stStart); // goto nextState
  runSteps(1000);     // run while nextState != NULL
  return 0;
}
**********************************************************************/

#endif

