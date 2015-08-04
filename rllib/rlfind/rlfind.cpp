/***************************************************************************
                          rlsvgcat.cpp  -  description
                             -------------------
    begin                : Tue Jul 30 2015
    copyright            : (C) 2015 by pvbrowser
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#include <string.h> 
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
int printPath(const char *path)
{
  if(*path == '\0') return -1;
  if(*path == '\n') return -1;
  if(*path != '.' && *path != '\\') putchar('/');
  while(*path != '\0') 
  {
    if(*path == '\\') putchar('/');
    else              putchar(*path);
    path++;
  }  
  putchar('\n');
  return 0;
}

int find(const char *szDir, const char *szFunction, const char *szPattern)
{
   WIN32_FIND_DATA ffd;
   HANDLE hFind = INVALID_HANDLE_VALUE;
   char buf[strlen(szDir) + 4];
   strcpy(buf,szDir); strcat(buf,"\\*");
   
   hFind = FindFirstFile(buf, &ffd);
   if(INVALID_HANDLE_VALUE == hFind) return -1;
   do
   {
      if(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      {
	if(ffd.cFileName[0] != '.')
	{  
          char path[strlen(szDir) + strlen(ffd.cFileName) + 1];
          strcpy(path, szDir);
          strcat(path,"\\");
          strcat(path,ffd.cFileName);
	  if(0 && strstr(ffd.cFileName,szPattern) != NULL)
	  {  
	    printf("DIR=");
            printPath(path);
	  }  
          if(strcmp(szFunction,"-only") != 0) find(path,szFunction,szPattern);
	}  
      }
      else if(*szPattern=='\0' || strstr(ffd.cFileName,szPattern) != NULL)
      {
         //printf("/%s\n", ffd.cFileName);
        char path[strlen(szDir) + strlen(ffd.cFileName) + 1];
        strcpy(path, szDir);
        strcat(path,"\\");
        strcat(path,ffd.cFileName);
        printPath(path);
      }
   }
   while(FindNextFile(hFind, &ffd) != 0);
   FindClose(hFind);
   return 0;
}
#endif

int main(int argc, char **argv)
{
   if(argc == 2)
   {
#ifdef _WIN32   
     find(argv[1], "-only", "");
#else
     char cmd[strlen(argv[1]) + 80];
     sprintf(cmd,"find \"%s\" -maxdepth 1 -name \"*\" -printf \"%%p [%%y]\n\" | sort", argv[1]);
     system(cmd);
#endif
     return 0;   
   }
   else if(argc != 4)
   {
      printf("usage: %s <start_directory> <-name|-only> <pattern>\n", argv[0]);
      return (-1);
   }

#ifdef _WIN32   
   char mypattern[strlen(argv[3])];
   char       *dest = &mypattern[0];
   const char *cptr = argv[3];
   while(*cptr != '\0')
   {
     if(*cptr == '*') cptr++;
     else             *dest++ = *cptr++;
   }
   *dest = '\0';
   find(argv[1], argv[2], mypattern);
#else
   char cmd[strlen(argv[1]) + strlen(argv[2]) + strlen(argv[3]) + 80];
   if(strcmp(argv[2],"-only") == 0) sprintf(cmd,"find \"%s\" -maxdepth 1 -name  \"%s\" -printf \"%%p [%%y]\n\" | sort", argv[1],          argv[3]);
   else                             sprintf(cmd,"find \"%s\"             \"%s\" \"%s\" -printf \"%%p [%%y]\n\" | sort", argv[1], argv[2], argv[3]);
   system(cmd);
#endif   
   
   return 0;
}
