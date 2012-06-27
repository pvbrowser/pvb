/***************************************************************************
                     SetupRegistry.cpp  -  description
                             -------------------
    begin                : Sun Oct 06 2002
    copyright            : (C) 2000-2002 by R. Lehrig
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
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <direct.h>
#include <conio.h>

//#define USE_VTK
#define MAX_LEN 1024*32

#ifdef USE_VTK
static const char mypath[] = "%PVBDIR%\\win\\bin;%VTKDIR%\\bin;%TCLDIR%\\bin";
#else
static const char mypath[] = "%PVBDIR%\\win\\bin";
#endif
static HKEY keysystem,currentcontrolset,control,sessionmanager,environment;
static HKEY pv_environment,pv_shell_environment,pv_shell_open_environment,pv_shell_open_command_environment;

int get_pv_environment(const char *name)
{
  long ret;

  ret = RegOpenKey(
    HKEY_CLASSES_ROOT,        // handle to open key
    name,                     // address of name of subkey to open
    &pv_environment           // address of handle to open key
  );

  if(ret == ERROR_SUCCESS) return 0;

  ret = RegCreateKey(
    HKEY_CLASSES_ROOT,        // handle to an open key
    name,                     // address of name of subkey to open
    &pv_environment           // address of buffer for opened handle
  );

  if(ret == ERROR_SUCCESS) return 0;

  return -1;
}

int get_pv_shell_environment()
{
  long ret;

  ret = RegOpenKey(
    pv_environment,           // handle to open key
    "shell",                  // address of name of subkey to open
    &pv_shell_environment     // address of handle to open key
  );

  if(ret == ERROR_SUCCESS) return 0;

  ret = RegCreateKey(
    pv_environment,           // handle to an open key
    "shell",                  // address of name of subkey to open
    &pv_shell_environment     // address of buffer for opened handle
  );

  if(ret == ERROR_SUCCESS) return 0;

  return -1;
}

int get_pv_shell_open_environment()
{
  long ret;

  ret = RegOpenKey(
    pv_shell_environment,       // handle to open key
    "open",                     // address of name of subkey to open
    &pv_shell_open_environment  // address of handle to open key
  );

  if(ret == ERROR_SUCCESS) return 0;

  ret = RegCreateKey(
    pv_shell_environment,       // handle to an open key
    "open",                     // address of name of subkey to open
    &pv_shell_open_environment  // address of buffer for opened handle
  );

  if(ret == ERROR_SUCCESS) return 0;

  return -1;
}

int get_pv_shell_open_command_environment()
{
  long ret;

  ret = RegOpenKey(
    pv_shell_open_environment,          // handle to open key
    "command",                          // address of name of subkey to open
    &pv_shell_open_command_environment  // address of handle to open key
  );

  if(ret == ERROR_SUCCESS) return 0;

  ret = RegCreateKey(
    pv_shell_open_environment,          // handle to an open key
    "command",                          // address of name of subkey to open
    &pv_shell_open_command_environment  // address of buffer for opened handle
  );

  if(ret == ERROR_SUCCESS) return 0;

  return -1;
}

int privatgetKey()
{
  long ret;

  ret = RegOpenKey(
    HKEY_CURRENT_USER,        // handle to open key
    "Environment",            // address of name of subkey to open
    &environment              // address of handle to open key
  );

  if(ret == ERROR_SUCCESS) return 0;
  return -1;
}

int getKey()
{
  long ret;

  ret = RegOpenKey(
    HKEY_LOCAL_MACHINE,       // handle to open key
    "System",                 // address of name of subkey to open
    &keysystem                // address of handle to open key
  );

  if(ret != ERROR_SUCCESS) return -1;

  ret = RegOpenKey(
    keysystem,                // handle to open key
    "CurrentControlSet",      // address of name of subkey to open
    &currentcontrolset        // address of handle to open key
  );

  if(ret != ERROR_SUCCESS) return -1;

  ret = RegOpenKey(
    currentcontrolset,        // handle to open key
    "Control",                // address of name of subkey to open
    &control                  // address of handle to open key
  );

  if(ret != ERROR_SUCCESS) return -1;

  ret = RegOpenKey(
    control,                  // handle to open key
    "Session Manager",        // address of name of subkey to open
    &sessionmanager           // address of handle to open key
  );

  if(ret != ERROR_SUCCESS) return -1;

  ret = RegOpenKey(
    sessionmanager,           // handle to open key
    "Environment",            // address of name of subkey to open
    &environment              // address of handle to open key
  );

  if(ret != ERROR_SUCCESS) return -1;

  return 0;
}

int getRegistry(const char *subkey, char *buf)
{
  long ret;
  unsigned long size,type;

  buf[0] = '\0';
  ret = RegQueryValueEx(
    environment,           // handle to key to query
    subkey,                // address of name of value to query
    NULL,                  // reserved
    &type,                 // address of buffer for value type
    (unsigned char *) buf, // address of data buffer
    &size                  // address of data buffer size
  );

  if(ret == ERROR_SUCCESS) return 0;
  return -1;
}

int putRegistry(const char *subkey, int type, const char *data)
{
  long ret;

  ret = RegSetValueEx(
    environment,                                // handle to key to set value for
    subkey,                                     // name of the value to set
    0,                                          // reserved
    type,                                       // flag for value type
    (const unsigned char *) data,               // address of value data
    strlen(data)                                // size of value data
  );
  
  if(ret == ERROR_SUCCESS) return 0;
  return -1;
}

int putpvRegistry(const char *subkey, int type, const char *data)
{
  long ret;

  ret = RegSetValueEx(
    pv_environment,                             // handle to key to set value for
    subkey,                                     // name of the value to set
    0,                                          // reserved
    type,                                       // flag for value type
    (const unsigned char *) data,               // address of value data
    strlen(data)                                // size of value data
  );
  
  if(ret == ERROR_SUCCESS) return 0;
  return -1;
}

int putpv_shell_open_commandRegistry(const char *subkey, int type, const char *data)
{
  long ret;

  ret = RegSetValueEx(
    pv_shell_open_command_environment,          // handle to key to set value for
    subkey,                                     // name of the value to set
    0,                                          // reserved
    type,                                       // flag for value type
    (const unsigned char *) data,               // address of value data
    strlen(data)                                // size of value data
  );
  
  if(ret == ERROR_SUCCESS) return 0;
  return -1;
}

void dir2slash(const char *dir, char *slash)
{
  int i=0;
  while(dir[i] != '\0')
  {
    if(dir[i] == '\\') slash[i] = '/';
    else               slash[i] = dir[i]; 
    i++;
  }
  slash[i] = '\0';
}

int main()
{
  char buf[MAX_LEN],param[MAX_LEN],dir[1024],dirslash[1024];
  FILE *fp;

  if(getKey() < 0)
  {
    printf("could not get key to HKEY_CURRENT_USER\\Environment\n");
    getch();
    return 1;
  }

  ExpandEnvironmentStrings("%USERPROFILE%",buf,sizeof(buf)-1);
  strcat(buf,"\\pvbrz.ini");
  unlink(buf);

  fp = fopen("SetupRegistry.log","w");
  getcwd(dir,sizeof(dir)-1);
  dir2slash(dir,dirslash);

  if( getRegistry("Path",buf) == 0)
  {
    printf("OLD PATH=%s\n",buf);
    if(fp != NULL) fprintf(fp,"OLD PATH=%s\n",buf);
    //printf("registered HKEY_CURRENT_USER\\Environment:\n");
    printf("registered HKEY_LOCAL_MACHINE\\System\\CurrentControlSet\\Control\\Session Manager\\Environment:\n");
    if(strstr(buf,mypath) == NULL)
    {
      strcpy(param,buf);
      if(param[strlen(param)-1] != ';') strcat(param,";");
      strcat(param,mypath);
      putRegistry("Path", REG_EXPAND_SZ, param);
      printf("new PATH=%s\n",param);
      if(fp != NULL) fprintf(fp,"PATH=%s\n",param);
    }
    else
    {
      printf("unchanged PATH=%s\n",buf);
      if(fp != NULL) fprintf(fp,"PATH=%s\n",buf);
    }
  }
  else
  {
    printf("could not append %s to PATH environment variable\n",mypath);
    printf("please set it manually in controlCenter->System->Environment\n");
    if(fp != NULL)
    {
      fprintf(fp,"could not append %s to PATH environment variable\n",mypath);
      fprintf(fp,"please set it manually in controlCenter->System->Environment\n");
    }
  }

#ifdef USE_VTK
  sprintf(param,"%s/Tcl/lib %s/vtk40/Wrapping/Tcl",dirslash,dirslash);
  putRegistry("TCLLIBPATH", REG_SZ, param);
  printf("TCLLIBPATH=%s\n",param);
  if(fp != NULL) fprintf(fp,"TCLLIBPATH=%s\n",param);
#endif

#ifdef USE_VTK
  sprintf(param,"%s\\Tcl\\lib\\tcl8.3",dir);
  putRegistry("TCL_LIBRARY", REG_SZ, param);
  printf("TCL_LIBRARY=%s\n",param);
  if(fp != NULL) fprintf(fp,"TCL_LIBRARY=%s\n",param);
#endif

#ifdef USE_VTK
  sprintf(param,"%s\\Tcl\\lib\\tk8.3",dir);
  putRegistry("TK_LIBRARY", REG_SZ, param);
  printf("TK_LIBRARY=%s\n",param);
  if(fp != NULL) fprintf(fp,"TK_LIBRARY=%s\n",param);
#endif

  sprintf(param,"%s",dir);
  putRegistry("PVBDIR", REG_SZ, param);
  printf("PVBDIR=%s\n",param);
  if(fp != NULL) fprintf(fp,"PVBDIR=%s\n",param);

#ifdef USE_VTK
  sprintf(param,"%s\\tcl",dir);
  putRegistry("TCLDIR", REG_SZ, param);
  printf("TCLDIR=%s\n",param);
  if(fp != NULL) fprintf(fp,"TCLDIR=%s\n",param);
#endif

#ifdef USE_VTK
  sprintf(param,"%s\\vtk40",dir);
  putRegistry("VTKDIR", REG_SZ, param);
  printf("VTKDIR=%s\n",param);
  if(fp != NULL) fprintf(fp,"VTKDIR=%s\n",param);
#endif

  RegCloseKey(environment);
  RegCloseKey(sessionmanager);
  RegCloseKey(control);
  RegCloseKey(currentcontrolset);
  RegCloseKey(keysystem);

  // define URL Protocol pv://
  printf("[HKEY_CLASSES_ROOT]/pv and subentries\n");
  if(fp != NULL) fprintf(fp,"[HKEY_CLASSES_ROOT]/pv and subentries\n");
  get_pv_environment("pv");
  putpvRegistry(NULL, REG_SZ, "URL: pv Protocol");
  putpvRegistry("URL Protocol", REG_SZ, "");
  get_pv_shell_environment();
  get_pv_shell_open_environment();
  get_pv_shell_open_command_environment();
  strcpy(buf,"\"");
  strcat(buf,dir);
  strcat(buf,"\\win\\bin\\pvbrowser.exe\" \"%1\"");
  putpv_shell_open_commandRegistry(NULL, REG_SZ, buf);
  RegCloseKey(pv_shell_open_command_environment);
  RegCloseKey(pv_shell_open_environment);
  RegCloseKey(pv_shell_environment);
  RegCloseKey(pv_environment);

  // define URL Protocol pvssh://
  printf("[HKEY_CLASSES_ROOT]/pvssh and subentries\n");
  if(fp != NULL) fprintf(fp,"[HKEY_CLASSES_ROOT]/pvssh and subentries\n");
  get_pv_environment("pvssh");
  putpvRegistry(NULL, REG_SZ, "URL: pvssh Protocol");
  putpvRegistry("URL Protocol", REG_SZ, "");
  get_pv_shell_environment();
  get_pv_shell_open_environment();
  get_pv_shell_open_command_environment();
  strcpy(buf,"\"");
  strcat(buf,dir);
  strcat(buf,"\\win\\bin\\pvbrowser.exe\" \"%1\"");
  putpv_shell_open_commandRegistry(NULL, REG_SZ, buf);
  RegCloseKey(pv_shell_open_command_environment);
  RegCloseKey(pv_shell_open_environment);
  RegCloseKey(pv_shell_environment);
  RegCloseKey(pv_environment);

  if(fp != NULL) fclose(fp);
  //printf("\n");
  //printf("Important before running ProcessView.exe:: !!!\n");
  //printf("Logout and Login again (so that changed environment is effective)\n");

  //getch();
  return 0;
}
