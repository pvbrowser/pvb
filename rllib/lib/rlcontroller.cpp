/***************************************************************************
                          rlcontroller.cpp  -  description
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
#include "rlcontroller.h"

static void *control(void *arg)
{
  THREAD_PARAM *p = (THREAD_PARAM *) arg;
  rlController *c = (rlController *) p->user;
  while(c->running == 1)
  {
    if(c->sleepLocally) rlsleep(c->dt);
    c->measurement = c->getMeasurement();
    c->lock(); // lock mutex because user might set another controller type
    c->ydk_1 = c->ydk;
    c->y1k_1 = c->y1k;
    c->yk_2  = c->yk_1;
    c->yk_1  = c->yk;
    c->ek_2  = c->ek_1;
    c->ek_1  = c->ek;
    c->ek    = c->reference - c->measurement;
    switch(c->type)
    {
      case rlController::P:
        c->yk = c->d0*c->ek;
        break;
      case rlController::I:
      case rlController::D_T1:
      case rlController::PI:
      case rlController::PD_T1:
        c->yk = c->d0*c->ek + c->d1*c->ek_1 + c->c1*c->yk_1;
        break;
      case rlController::PID_T1:
        c->yk = c->d0*c->ek + c->d1*c->ek_1 + c->d2*c->ek_2 + c->c1*c->yk_1 + c->c2*c->yk_2;
        break;
      case rlController::PI_SUM:
        c->yk = c->Kp*c->ek + c->y1k;
        c->y1k = c->y1k_1 + c->d1*(c->ek+c->ek_1);
        break;
      case rlController::PD_T1_SUM:
        c->yk = c->Kp*c->ek + c->ydk;
        c->ydk = c->cD*c->ydk_1 + c->dD*(c->ek-c->ek_1);
        break;
      case rlController::PID_T1_SUM:
        c->yk = c->Kp*c->ek + c->y1k + c->ydk;
        c->y1k = c->y1k_1 + c->d1*(c->ek+c->ek_1);
        c->ydk = c->cD*c->ydk_1 + c->dD*(c->ek-c->ek_1);
        break;
      default:
        break;
    }
    if(c->limited)
    {
      if(c->yk  > c->yk_max) c->yk  = c->yk_max;
      if(c->yk  < c->yk_min) c->yk  = c->yk_min;
      if(c->y1k > c->yk_max) c->y1k = c->yk_max;
      if(c->y1k < c->yk_min) c->y1k = c->yk_min;
      if(c->ydk > c->yk_max) c->ydk = c->yk_max;
      if(c->ydk < c->yk_min) c->ydk = c->yk_min;
    }
    c->unlock();
    c->writeOutput(c->yk);
  }
  return arg;
} 

rlController::rlController(double (*_getMeasurement)() ,void (_writeOutput)(double output))
             :rlThread()
{
  type = -1;
  running = 0;
  getMeasurement = _getMeasurement;
  writeOutput    = _writeOutput;
  ydk_1 = 0.0f;
  ydk   = 0.0f;
  y1k_1 = 0.0f;
  y1k   = 0.0f;
  yk_2  = 0.0f;
  yk_1  = 0.0f;
  yk    = 0.0f;
  ek_2  = 0.0f;
  ek_1  = 0.0f;
  ek    = 0.0f;
  limited = 0;
  yk_min = -999999;
  yk_max =  999999;
  reference = measurement = 0.0;
  sleepLocally = 1;
}

rlController::~rlController()
{
  stop();
}

void rlController::start()
{
  running = 1;
  ydk_1 = 0.0f;
  ydk   = 0.0f;
  y1k_1 = 0.0f;
  y1k   = 0.0f;
  yk_2  = 0.0f;
  yk_1  = 0.0f;
  yk    = 0.0f;
  ek_2  = 0.0f;
  ek_1  = 0.0f;
  ek    = 0.0f;
  rlThread::create(control,this);
}

void rlController::stop()
{
  if(running) rlThread::cancel();
  running = 0;
}

void rlController::setReference(double _reference)
{
  reference = _reference;
}

void rlController::setP(double _T, double _Kp)
{
  rlThread::lock();
  type = P;
  T  = _T;
  Kp = _Kp;
  dt = (int) (1000.0 * T);
  d0 = Kp;
  rlThread::unlock();
}

void rlController::setI(double _T, double _T1)
{
  rlThread::lock();
  type = I;
  T  = _T;
  T1 = _T1;
  dt = (int) (1000.0 * T);
  d0 = T/(2.0*T1);
  d1 = T/(2.0*T1);
  c1 = 1.0;
  rlThread::unlock();
}

void rlController::setD_T1(double _T, double _TD, double _Td)
{
  rlThread::lock();
  type = D_T1;
  T  = _T;
  TD = _TD;
  Td = _Td;
  dt = (int) (1000.0 * T);
  d0 = TD / (Td + T/2.0);
  d1 = -TD / (Td + T/2.0);
  c1 = (Td-T/2.0)/(Td+T/2.0);
  rlThread::unlock();
}

void rlController::setPI(double _T, double _Kp, double _Tn)
{
  rlThread::lock();
  type = PI;
  T  = _T;
  Kp = _Kp;
  Tn = _Tn;
  dt = (int) (1000.0 * T);
  d0 = Kp * (Tn+T/2.0) / Tn;
  d1 = -Kp * (Tn - T/2.0) / Tn;
  c1 = 1.0;
  rlThread::unlock();
}

void rlController::setPD_T1(double _T, double _Kp, double _TvP, double _Td)
{
  rlThread::lock();
  type = PD_T1;
  T   = _T;
  Kp  = _Kp;
  TvP = _TvP;
  Td  = _Td;
  dt = (int) (1000.0 * T);
  d0 = Kp * (TvP+T/2.0) / (Td + T/2.0);
  d1 = -Kp * (TvP - T/2.0) / (Td + T/2.0);
  c1 = (Td - T/2.0)/(Td + T/2.0);
  rlThread::unlock();
}

void rlController::setPID_T1(double _T, double _Kpp, double _TnP, double _TvP, double _Td)
{
  rlThread::lock();
  type = PID_T1;
  T   = _T;
  Kpp = _Kpp;
  TnP = _TnP;
  TvP = _TvP;
  Td  = _Td;
  dt = (int) (1000.0 * T);
  d0 = Kpp * ((TnP+T/2.0) / TnP) * ((TvP+T/2.0))/(Td+T/2.0);
  d1 = -2.0*Kpp * (TnP*TvP - (T/2.0)*(T/2.0)) / (TnP*(Td+T/2.0));
  d2 = Kpp*((TnP-T/2.0)/TnP)*((TvP-T/2.0)/(Td+T/2.0));
  c1 = 2*Td/(Td+T/2.0);
  c2 = -(Td-T/2.0)/(Td+T/2.0);
  rlThread::unlock();
}

void rlController::setPI_SUM(double _T, double _Kp, double _Tn)
{
  rlThread::lock();
  type = PI_SUM;
  T   = _T;
  Kp  = _Kp;
  Tn  = _Tn;
  dt = (int) (1000.0 * T);
  d1 = Kp * (T/2.0)/Tn;
  rlThread::unlock();
}

void rlController::setPD_T1_SUM(double _T, double _Kp, double _Tv, double _Td)
{
  rlThread::lock();
  type = PD_T1_SUM;
  T   = _T;
  Kp  = _Kp;
  Tv  = _Tv;
  Td  = _Td;
  dt = (int) (1000.0 * T);
  dD = Kp * (Tv/(Td+T/2.0));
  cD = (Td-T/2.0)/(Td+T/2.0);
  rlThread::unlock();
}

void rlController::setPID_T1_SUM(double _T, double _Kp, double _Tn, double _Tv, double _Td)
{
  rlThread::lock();
  type = PID_T1_SUM;
  T   = _T;
  Kp  = _Kp;
  Tn  = _Tn;
  Tv  = _Tv;
  Td  = _Td;
  dt = (int) (1000.0 * T);
  d1 = Kp*T/(2.0*Tn);
  dD = Kp * (Tv/(Td+T/2.0));
  cD = (Td-T/2.0)/(Td+T/2.0);
  rlThread::unlock();
}

void rlController::setLimits(double _yk_min, double _yk_max)
{
  rlThread::lock();
  yk_min = _yk_min;
  yk_max = _yk_max;
  limited = 1;
  rlThread::unlock();
}

void rlController::resetLimits()
{
  rlThread::lock();
  yk_min = -999999;
  yk_max =  999999;
  limited = 0;
  rlThread::unlock();
}

