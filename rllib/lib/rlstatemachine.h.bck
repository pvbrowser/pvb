/***************************************************************************
*                                                                         *
*   This library is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
*   published by the Free Software Foundation                             *
*                                                                         *
***************************************************************************/
#ifndef _RL_STATEMACHINE_H_
#define _RL_STATEMACHINE_H_

#include "rldefine.h"
#include "rlplc.h"

/*! <pre>
class for a statemachine
principle:

// the user declares and implements his statemachine
class Machine1 :public rlStatemachine
{
  public:
    enum MyStates  // declare your states
    {
      state1 = 0,
      state2,
      state3,
      numberOfStates
    };

    enum MyIndexes // declare indexes for your process variables
    {
      temperature = 0,
      speed,
      voltage,
      numberOfProcessVariables
    };

    Machine1(int numProcessVariables=numberOfProcessVariables);
    virtual ~Machine1();
    virtual void doState();
    virtual void enterState(int newState, int whichEntry=0);
    virtual void exitState(int whichExit=0);
};

// the statemachines are all executed forever
Machine1 stm1; // declare some statemachines
Machine2 stm2; // all Machines are derived from rlStatemachine
Machine3 stm3; // user has to implement these Machines

while(1) // forever do the statemachines
{        // for example within your SoftPLC or within slotNullEvent
  rlsleep(cycletime);
  readProcessImageFromField(); // here the setPlcStateInt/Float/Double methods are called
                               // the inputs are read from the field
  stm1.doState();              // now we do the state transitions
  stm2.doState();
  stm3.doState();
  writeProcessImageToField();  // here the getPlcStateInt/Float/Double methods are called
                               // in case of SoftPLC write to field
                               // in case of slotNullEvent update GUI
}
</pre> */
class rlStatemachine
{
  public:
    rlStatemachine(int numStates, int numProcessVariables, rlPlcState *state=NULL);
    virtual ~rlStatemachine();
    int    setPlcStateInt(int index, int val);
    float  setPlcStateFloat(int index, float val);
    double setPlcStateDouble(int index, double val);
    virtual void doState();                                  // implement in your subclass
    virtual void enterState(int newState, int whichEntry=0); // implement in your subclass
    virtual void exitState(int whichExit=0);                 // implement in your subclass
    int    getPlcStateInt(int index);
    float  getPlcStateFloat(int index);
    double getPlcStateDouble(int index);
    int    getState();
    rlPlcState *getVar();
  protected:  
    int num_states, num_process_variables;
    int state;
    int haveto_free_state;
    rlPlcState *var;
};

#endif

