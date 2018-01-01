// --------------------------------
// rldev: Menu for a lazy developer
// Rainer Lehrig 2017
// --------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>

void handler(int val)
{
  // printf("HANDLER: val=%d\n", val);
}

#define ESC 27

// begin config
//#define EDIT          "joe"
#define EDIT          "vim -X -S ~/shell/vi.ini"
#define LESS          "less"
#define MAKE          "make"
#define BROWSER       "firefox"
#define PVBROWSER     "pvbrowser"
#define PVDEVELOP     "pvdevelop"

#define PVSLIB        "file:/opt/pvb/doc/pvslib/modules.html"
#define RLLIB         "file:/opt/pvb/doc/rllib/classes.html"
#define CREF          "http://en.cppreference.com/w/"
//#define PVB           "http://localhost/pvbrowser/index.php"
#define PVB           "https://pvbrowser.de/pvbrowser/index.php"

#define PROFILE       "pvs.pro"
// end config

static int showMenu()
{
  printf("%c[2J",ESC);   // erase screen
  printf("%c[44m",ESC);  // set BLUE
  printf("%c[1m",ESC);   // set bold text
  printf("%c[H",ESC);    // goto upper left corner of screen 
  printf("rldev: (m)ake b(uild) (new) (pvslib) (rllib) (cref) (pvb)          (q)uit \n");
  printf("edit:  (main) (pvapp) (m<X>)mask (s<X>)slot (pro) (l)oop (h)ttpd   e(x)it \n");
  printf("run:   p<PORTNUM> b<PORTNUM> pvdevelop                           (s)start \n");
  printf("%c[0m",ESC);   // reset all attributes
  return 0;
}

static int showBlankScreen()
{
  printf("%c[2J",ESC);   // erase screen
  printf("%c[0m",ESC);   // reset all attributes
  printf("%c[1;1H",ESC); // goto start
  return 0;
}

int gotoStartPos()
{
  char cwd[256];
  printf("%c[4;1H",ESC); // goto start
  printf("%c[K",ESC);    // erase to end of line
  getcwd(cwd,sizeof(cwd)-1);
  printf("%s\n", cwd);
  printf("> ");
  return 0;
}

static int isEqual(const char *line, const char *start)
{
  if(strcmp(line,start) == 0) return 1;
  return 0;
} 

void startApp(char *input)
{
  int portnum = 5050;
  char line[256],cmd[256+64],*cptr;
  line[0] = cmd[0] = '\0';
  if(strlen(input) > 1 && strlen(input) < 250)
  {
    strcpy(line,&input[1]);
  }
  else
  {
    printf("%c[2J",ESC);   // erase screen
    printf("%c[44m",ESC);  // set BLUE
    printf("%c[1m",ESC);   // set bold text
    printf("%c[H",ESC);    // goto upper left corner of screen
    printf("start: (f)irefox (c)hromium (t)hunderbird                \n"); 
    printf("       (o)kluar (g)imp (gv)iew (oo)ffice                 \n");
    printf("       (p<PORTNUM>)vs  (b<PORTNUM>)pvbrowser pv(d)evelop \n");
    printf("%c[0m",ESC);   // reset all attributes
    if(fgets(line,sizeof(line)-1,stdin) == NULL) return;
    cptr = strstr(line,"\n"); if(cptr!=NULL) *cptr='\0';
  }
  if     (isEqual(line,"f"))  sprintf(cmd,"xterm -e firefox &");
  else if(isEqual(line,"c"))  sprintf(cmd,"xterm -e chromium &");
  else if(isEqual(line,"t"))  sprintf(cmd,"xterm -e thunderbird &");
  else if(isEqual(line,"o"))  sprintf(cmd,"xterm -e okular &");
  else if(isEqual(line,"g"))  sprintf(cmd,"xterm -e gimp &");   
  else if(isEqual(line,"gv")) sprintf(cmd,"xterm -e gwenview &");   
  else if(isEqual(line,"oo")) sprintf(cmd,"xterm -e ooffice &");   
  else if(isEqual(line,"d"))  sprintf(cmd,"xterm -e pvdevelop &");   
  else if(line[0] == 'p')
  {
    if(isdigit(line[1])) sscanf(&line[1],"%d", &portnum);
    sprintf(cmd,"xterm -e ./pvs -port=%d &", portnum);  
  } 
  else if(line[0] == 'b')  
  {
    if(isdigit(line[1])) sscanf(&line[1],"%d", &portnum);
    sprintf(cmd,"xterm -e pvbrowser pv://localhost:%d &", portnum);
  }
  else
  {
    sprintf(cmd,"xterm -e %s &", line);
  }   
  if(cmd[0]!='\0') system(cmd);
}

int main()
{
  int val;
  char line[256],cmd[256],*cptr;
  signal(SIGINT, handler);
  showMenu();
  gotoStartPos();
  while(fgets(line,sizeof(line)-1,stdin) != NULL)
  {
    cptr = strstr(line,"\n"); if(cptr!=NULL) *cptr='\0';
    showMenu();
    gotoStartPos();
    printf("\n");
    cmd[0] = '\0';
    if     (isEqual(line,"main"))   sprintf(cmd,"%s %s", EDIT, "main.cpp");
    else if(isEqual(line,"pvapp"))  sprintf(cmd,"%s %s", EDIT, "pvapp.h");
    else if(isEqual(line,"pro"))    sprintf(cmd,"%s %s", EDIT, PROFILE);
    else if(isEqual(line,"m"))      sprintf(cmd,"%s", MAKE);
    else if(isEqual(line,"b"))      sprintf(cmd,"%s clean && %s", MAKE, MAKE);
    else if(isEqual(line,"new"))    sprintf(cmd,"%s %s > /dev/null 2>&1 &", PVDEVELOP, "-action=writeInitialProject");
    else if(line[0] == 'l')         sprintf(cmd,"%s %s", EDIT, "loop.cpp");
    else if(line[0] == 'h')         sprintf(cmd,"%s %s", EDIT, "httpd.cpp");
    else if(line[0]=='m' && isdigit(line[1]))
    {
      sscanf(line,"m%d",&val);
      sprintf(cmd,"%s mask%d.cpp", EDIT, val);
    } 
    else if(line[0]=='s' && isdigit(line[1]))
    {
      sscanf(line,"s%d",&val);
      sprintf(cmd,"%s mask%d_slots.h", EDIT, val);
    } 
    else if(line[0]=='s')
    {
      startApp(line);
      showMenu();
    } 
    else if(line[0]=='p' && (isdigit(line[1]) || line[1]=='\0'))
    {
      val=5050;
      if(line[1]!='\0') sscanf(line,"p%d",&val);
      sprintf(cmd,"./pvs -http -port=%d", val);
    } 
    else if(line[0]=='b' && (isdigit(line[1]) || line[1]=='\0'))
    {
      val=5050;
      if(line[1]!='\0') sscanf(line,"b%d",&val);
      sprintf(cmd,"%s pv://localhost:%d > /dev/null 2>&1 &", PVBROWSER, val);
    } 
    else if(isEqual(line,"pvslib")) sprintf(cmd,"%s %s > /dev/null 2>&1 &", BROWSER, PVSLIB);
    else if(isEqual(line,"rllib"))  sprintf(cmd,"%s %s > /dev/null 2>&1 &", BROWSER, RLLIB);
    else if(isEqual(line,"cref"))   sprintf(cmd,"%s %s > /dev/null 2>&1 &", BROWSER, CREF);
    else if(isEqual(line,"pvb"))    sprintf(cmd,"%s %s > /dev/null 2>&1 &", BROWSER, PVB);
    else if(isEqual(line,"q"))      break;
    else if(isEqual(line,"x"))      break;
    else if(strncmp(line,"cd ",3) == 0)
    {
      chdir(&line[3]);
    }
    else                            sprintf(cmd,"%s", line);
    if(cmd[0] != '\0') 
    {
      //printf("%s\n", cmd);
      system(cmd);
    }
    if(strncmp(cmd,EDIT,strlen(EDIT))==0) showMenu();
    gotoStartPos();
  }
  showBlankScreen();
  return 0;
}
