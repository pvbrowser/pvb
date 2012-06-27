/***************************************************************************
                          rlcorba.h  -  description
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
#ifndef _RL_CORBA_H_
#define _RL_CORBA_H_

#include "rldefine.h"
#define rlMICO
//#define rlVISIBROKER
//#define rlOMNIORB

#include <CORBA.h>
#ifdef rlMICO
#include <mico/throw.h>
#endif

template <class T, class Tvar>
class rlCorbaClient
{
public:
#ifdef rlMICO
  rlCorbaClient(int ac, char **av, const char *iname)
  {
    idlname = adr = NULL;
    if(iname == NULL) return;
    idlname = new char[strlen(iname)+1];
    strcpy(idlname,iname);
    for(int i=0; i<ac; i++)
    {
      if(strncmp(av[i],"inet:",5) == 0)
      {
        if(adr != NULL) delete adr;
        adr = new char[strlen(av[i])+1];
        strcpy(adr,av[i]);
      }
    }
    orb = CORBA::ORB_init(ac,av);
    obj = orb->bind(idlname, adr);
    if(CORBA::is_nil(obj))
    {
      cerr << "cannot bind to " << adr << endl;
      delete adr;
      adr = NULL;
      return;
    }
    client = T::_narrow(obj);
    if(CORBA::is_nil(client))
    {
      cerr << "Argument is not a " << idlname << " reference" << endl;
    }
  }
  ~rlCorbaClient()
  {
    if(idlname != NULL) delete idlname;
    if(adr     != NULL) delete adr;
  }
#endif
  char               *idlname;
  char               *adr;
  CORBA::ORB_var     orb;
  CORBA::Object_var  obj;
  Tvar               client;
};

template <class Timpl, class Tvar>
class rlCorbaServer
{
public:
#ifdef rlMICO
  rlCorbaServer(int ac, char **av, const char *)
  {
    orb = CORBA::ORB_init(ac,av);
    obj = orb->resolve_initial_references("RootPOA");
    poa = PortableServer::POA::_narrow(obj);
    mgr = poa->the_POAManager();
    server = server_servant._this();
    CORBA::String_var  str = orb->object_to_string(server);
    cout << str << endl;   // write reference to stdout
    mgr->activate();
  }
  ~rlCorbaServer()
  {
  }
#endif
  void run()
  {
    orb->run();
  }
  char *adr;
  CORBA::ORB_var                 orb;
  CORBA::Object_var              obj;
  PortableServer::POA_var        poa;
  PortableServer::POAManager_var mgr;
  Timpl                          server_servant;
  Tvar                           server;
};

#endif
