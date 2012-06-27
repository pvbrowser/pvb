/***************************************************************************
                          rlcontroller.h  -  description
                             -------------------
    begin                : Wed Jun 16 2004
    copyright            : (C) 2004 by R. Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#ifndef _RL_CONTROLLER_H_
#define _RL_CONTROLLER_H_

#include "rldefine.h"
#include "rlthread.h"

/*! <pre>
class for closed loop control
According to: F. Doerrscheid/W. Latzel, Grundlagen der Regelungstechnik, B.G. Teubner Stuttgart
Page 436-437, Regelalgorithmen mit der Trapezregel
</pre> */
class rlController : public rlThread
{
public:
  enum ControllerType
  {
    P          = 1,
    I          = 2,
    D_T1       = 3,
    PI         = 4,
    PD_T1      = 5,
    PID_T1     = 6,
    PI_SUM     = 7,
    PD_T1_SUM  = 8,
    PID_T1_SUM = 9
  };
  rlController(double (*_getMeasurement)() ,void (_writeOutput)(double output));
  ~rlController();
  void start();
  void stop();
  /*! <pre>
  Set the reference value for the controller
  </pre> */
  void setReference  (double _reference);
  /*! <pre>
  Transfer function: Gr(s) = Kp
  </pre> */
  void setP          (double _T, double _Kp);
  /*! <pre>
                               1
  Transfer function: Gr(s) = ------
                             T1 * s
  T = cycle time in seconds
  </pre> */
  void setI          (double _T, double _T1);
  /*! <pre>
                              TD * s
  Transfer function: Gr(s) = ---------
                             1 + Td*s
  T = cycle time in seconds
  </pre> */
  void setD_T1       (double _T, double _TD, double _Td);
  /*! <pre>
                                  1 + Tn*s
  Transfer function: Gr(s) = Kp * --------
                                   Tn*s
  T = cycle time in seconds
  </pre> */
  void setPI         (double _T, double _Kp, double _Tn);
  /*! <pre>
                                  1 + TvP*s
  Transfer function: Gr(s) = Kp * ---------
                                  1 + Td*s
  T = cycle time in seconds
  </pre> */
  void setPD_T1      (double _T, double _Kp, double _TvP, double _Td);
  /*! <pre>
                                   1 + TnP*s   1 + TvP*s
  Transfer function: Gr(s) = Kpp * --------- * ---------
                                     TnP*s     1 + Td*s
  T = cycle time in seconds
  </pre> */
  void setPID_T1     (double _T, double _Kpp, double _TnP, double _TvP, double _Td);
  /*! <pre>
                                         1
  Transfer function: Gr(s) = Kp * ( 1 + ---- )
                                        Tn*s
  T = cycle time in seconds
  </pre> */
  void setPI_SUM     (double _T, double _Kp, double _Tn);
  /*! <pre>
                                          Tv*s
  Transfer function: Gr(s) = Kp * ( 1 + -------- )
                                        1 + Td*s
  T = cycle time in seconds
  </pre> */
  void setPD_T1_SUM  (double _T, double _Kp, double _Tv, double _Td);
  /*! <pre>
                                         1       Tv*s
  Transfer function: Gr(s) = Kp * ( 1 + ---- + -------- )
                                        Tn*s   1 + Td*s
  T = cycle time in seconds
  </pre> */
  void setPID_T1_SUM (double _T, double _Kp, double _Tn, double _Tv, double _Td);
  /*! <pre>
  Set limits for the controller output
  </pre> */
  void setLimits(double _yk_min, double _yk_max);
  /*! <pre>
  Reset limits for the controller output
  </pre> */
  void resetLimits();
  /*! <pre>
  Don't set the controller parameters directly
  Use the set methods, because they will also set the coefficients
  Controller parameters are for reading only
  </pre> */
  double Kp,Kpp,T,T1,Td,TD,Tn,TnP,TvP,Tv,reference;
  int type;
  int running;
  double d0,d1,d2,dD;
  double c1,c2,cD;
  double yk,yk_1,yk_2;
  double ek,ek_1,ek_2;
  double y1k,y1k_1,ydk,ydk_1; 
  int   dt;
  // callbacks
  /*! <pre>
  You have to supply this function for getting the measurement
  </pre> */
  double (*getMeasurement)();
  /*! <pre>
  You have to supply this function for writing the output
  </pre> */
  void  (*writeOutput)(double output);

  /*! <pre>
  Default sleepLocally = 1
  But:
  Sleeping locally might me inaccurate.
  It might be better to have a central timer and wait for it in
  double (*_getMeasurement)();
  T = cycle time in seconds
  </pre> */
  int sleepLocally;
  /*! <pre>
  last measurement 
  </pre> */
  double measurement;
  /*! <pre>
  limits 
  </pre> */
  double yk_min;
  double yk_max;
  int limited;
};

#endif
