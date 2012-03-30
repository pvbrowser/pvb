/***************************************************************************
*                                                                         *
*   This library is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
*   published by the Free Software Foundation                             *
*                                                                         *
***************************************************************************/
#include "rlstatemachine.h"

rlStatemachine::rlStatemachine(int numStates, int numProcessVariables, rlPlcState *state)
{
  state = 0;
  num_states = 0;
  var = NULL;
  num_states = numStates;
  num_process_variables = numProcessVariables;
  if(numProcessVariables <= 0) return;
  if(state == NULL)
  {
    haveto_free_state = 1;
    var = new rlPlcState(numProcessVariables, numProcessVariables, numProcessVariables);
  }
  else
  {
    haveto_free_state = 0;
    var = state;
  }
}

rlStatemachine::~rlStatemachine()
{
  if(var != NULL && haveto_free_state) delete var;
}

int rlStatemachine::setPlcStateInt(int index, int val)
{
  if(index < 0) return 0;
  if(index >= num_process_variables) return 0;
  if(var == NULL) return 0;
  var->i_old[index] = var->i[index];
  var->i[index] = val;
  return val;
}

float rlStatemachine::setPlcStateFloat(int index, float val)
{
  if(index < 0) return 0;
  if(index >= num_process_variables) return 0;
  if(var == NULL) return 0;
  var->f_old[index] = var->f[index];
  var->f[index] = val;
  return val;
}

double rlStatemachine::setPlcStateDouble(int index, double val)
{
  if(index < 0) return 0;
  if(index >= num_process_variables) return 0;
  if(var == NULL) return 0;
  var->d_old[index] = var->d[index];
  var->d[index] = val;
  return val;
}

void rlStatemachine::doState()
{
  // implement in subclass
}

void rlStatemachine::enterState(int newState, int whichEntry)
{
  // implement in subclass and call rlStatemachine::enterState(newState,whichEntry);
  if(newState < 0) return;
  if(newState >= num_states) return;
  if(whichEntry){}
  state = newState;
}

void rlStatemachine::exitState(int whichExit)
{
  // implement in subclass
  if(whichExit){}
}

int rlStatemachine::getPlcStateInt(int index)
{
  if(index < 0) return 0;
  if(index >= num_process_variables) return 0;
  if(var == NULL) return 0;
  return var->i[index];
}

float rlStatemachine::getPlcStateFloat(int index)
{
  if(index < 0) return 0;
  if(index >= num_process_variables) return 0;
  if(var == NULL) return 0;
  return var->f[index];
}

double rlStatemachine::getPlcStateDouble(int index)
{
  if(index < 0) return 0;
  if(index >= num_process_variables) return 0;
  if(var == NULL) return 0;
  return var->d[index];
}

int rlStatemachine::getState()
{
  return state;
}

rlPlcState *rlStatemachine::getVar()
{
  return var;
}

