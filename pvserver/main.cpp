/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Son Nov 12 09:43:38 CET 2000
    copyright            : (C) 2000 by R. Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "pvapp.h"

int pvMain(PARAM *p)
{
int ret;

  pvSetCaption(p,"pvsdemo");
  pvResize(p,0,1280,1024);
  //pvScreenHint(p,1024,768); // this may be used to automatically set the zoom factor
  ret = 1;
  pvGetInitialMask(p);
  printf("pvMain::version=%s\n",p->version);
  if(strcmp(p->initial_mask,"1") == 0)        ret = 1;
  if(strcmp(p->initial_mask,"2") == 0)        ret = 2;
  if(strcmp(p->initial_mask,"3") == 0)        ret = 3;
  if(strcmp(p->initial_mask,"4") == 0)        ret = 4;
  if(strcmp(p->initial_mask,"periodic") == 0) ret = 5;
  if(strcmp(p->initial_mask,"vtk") == 0)      ret = 6;
  if(strcmp(p->initial_mask,"qwt") == 0)      ret = 7;

  while(1)
  {
    switch(ret)
    {
      case 1:
        pvStatusMessage(p,-1,-1,-1,"Mask1");
        ret = show_mask1(p);
        break;
      case 2:
        pvStatusMessage(p,RED,"Mask2");
        ret = show_mask2(p);
        break;
      case 3:
        pvStatusMessage(p,GREEN,"Mask3");
        ret = show_mask3(p);
        break;
      case 4:
        pvStatusMessage(p,BLUE,"Mask4");
        ret = show_mask4(p);
        break;
      case 5:
        pvStatusMessage(p,YELLOW,"Mask periodic");
        ret = show_periodic(p);
        break;
      case 6:
        pvStatusMessage(p,CYAN,"Mask vtk");
        ret = show_maskvtk(p);
        break;
      case 7:
        pvStatusMessage(p,WHEAT,"Mask qwt");
        ret = show_qwt(p);
        break;
      default:
        return 0;
    }
  }
}

#ifdef USE_INETD
int main(int ac, char **av)
{
PARAM p;

  pvInit(ac,av,&p);
  printf("main:version=%s\n",p.version);
  /* here you may interpret ac,av and set p->user to your data */
  pvMain(&p);
  return 0;
}
#else  // multi threaded server
int main(int ac, char **av)
{
PARAM p;
int   s,i;

  pvInit(ac,av,&p);
  /* here you may interpret ac,av and set p->user to your data */
  for(i=1; i<ac; i++)
  {
    if(strcmp(av[i],"-1") == 0)
    {
      s = pvAccept(&p);
      if(s > 0) 
      {
        p.s = s;
        p.free = 0;
        pvMain(&p);
      }
      else
      {
        printf("pvAccept error\n");
      }
      return 0;
    }
  }
  while(1)
  {
    s = pvAccept(&p);
    if(s != -1) pvCreateThread(&p,s);
    else        break;
  }
  return 0;
}
#endif
