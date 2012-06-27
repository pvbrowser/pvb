/***************************************************************************
                          rlhistory.cpp  -  description
                             -------------------
    begin                : Wed Dec 12 2006
    copyright            : (C) 2006 by R. Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#include "rlhistoryreader.h"

int main(int ac, char **av)
{
  rlHistoryReader reader;

  if(ac == 2)
  {
    reader.cat(av[1], stdout);
    return 0;
  }
  if(ac == 3)
  {
    FILE *fout = fopen(av[2],"w");
    if(fout != NULL)
    {
      reader.cat(av[1], fout);
      fclose(fout);
      return 0;
    }
    else
    {
      printf("could not write %s\n",av[2]);
      return -1;
    }
  }
  printf("usage: rlhistory name <outputfile>\n");
  return -1;
}
