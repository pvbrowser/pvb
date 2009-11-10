////////////////////////////////////////////////////////////////////////////
//
// show_modal1 for ProcessViewServer created: Sun Feb 2 12:25:45 2003
//
////////////////////////////////////////////////////////////////////////////
#include "pvapp.h"

typedef struct // (todo: define your data structure here)
{
}
DATA;

// _begin_of_generated_area_ (do not edit -> use ui2pvc) -------------------

// our mask contains the following objects
enum {
  ID_MAIN_WIDGET = 0,
  PushButton1,
  TextLabel1,
  ID_END_OF_WIDGETS
};

static int generated_defineMask(PARAM *p)
{
  int w,h,depth;

  if(p == NULL) return 1;
  w = h = depth = 0;
  if(w==h) depth=0; // fool the compiler
  pvStartDefinition(p,ID_END_OF_WIDGETS);


  pvQPushButton(p,PushButton1,0);
  pvSetGeometry(p,PushButton1,90,120,141,71);
  pvSetText(p,PushButton1,"PushButton1");

  pvQLabel(p,TextLabel1,0);
  pvSetGeometry(p,TextLabel1,50,20,240,93);
  pvSetText(p,TextLabel1,"This is a modal<br>Dialog Box");
  pvSetFont(p,TextLabel1,"Times New Roman",24,0,0,0,0);

  pvEndDefinition(p);
  return 0;
}

// _end_of_generated_area_ (do not edit -> use ui2pvc) ---------------------

static int defineMask(PARAM *p)
{
  if(p == NULL) return 1;
  generated_defineMask(p);
  // (todo: add your code here)
  return 0;
}

static int showData(PARAM *p, DATA *d)
{
  if(p == NULL) return 1;
  if(d == NULL) return 1;
  // (todo: add your code here)
  return 0;
}

static int readData(DATA *d) // from shared memory, database or something else
{
  if(d == NULL) return 1;
  // (todo: add your code here)
  return 0;
}

int show_modal1(PARAM *p)
{
  DATA d;
  char event[MAX_EVENT_LENGTH];
  char text[MAX_EVENT_LENGTH];
  char str1[MAX_EVENT_LENGTH];
  int  i,w,h,val,x,y,button;

  defineMask(p);
  memset(&d,0,sizeof(DATA));
  readData(&d); // from shared memory, database or something else
  showData(p,&d);
  while(1)
  {
    pvPollEvent(p,event);
    switch(pvParseEvent(event, &i, text))
    {
      case NULL_EVENT:
        readData(&d); // from shared memory, database or something else
        showData(p,&d);
        pvUpdateBaseWindow(p);
        break;
      case BUTTON_EVENT:
        printf("BUTTON_EVENT id=%d\n",i);
        if(i == 0) return -1;   // The modal dialog box has been closed
        if(i == PushButton1) pvTerminateModalDialog(p);
        break;
      case BUTTON_PRESSED_EVENT:
        printf("BUTTON_PRESSED_EVENT id=%d\n",i);
        break;
      case BUTTON_RELEASED_EVENT:
        printf("BUTTON_RELEASED_EVENT id=%d\n",i);
        break;
      case TEXT_EVENT:
        printf("TEXT_EVENT id=%d %s\n",i,text);
        break;
      case SLIDER_EVENT:
        sscanf(text,"(%d)",&val);
        printf("SLIDER_EVENT val=%d\n",val);
        break;
      case CHECKBOX_EVENT:
        printf("CHECKBOX_EVENT id=%d %s\n",i,text);
        break;
      case RADIOBUTTON_EVENT:
        printf("RADIOBUTTON_EVENT id=%d %s\n",i,text);
        break;
      case GL_INITIALIZE_EVENT:
        printf("you have to call initializeGL()\n");
        break;
      case GL_PAINT_EVENT:
        printf("you have to call paintGL()\n");
        break;
      case GL_RESIZE_EVENT:
        sscanf(text,"(%d,%d)",&w,&h);
        printf("you have to call resizeGL(w,h)\n");
        break;
      case GL_IDLE_EVENT:
        break;
      case TAB_EVENT:
        sscanf(text,"(%d)",&val);
        printf("TAB_EVENT(%d,page=%d)\n",i,val);
        break;
      case TABLE_TEXT_EVENT:
        sscanf(text,"(%d,%d,",&x,&y);
        pvGetText(text,str1);
        printf("TABLE_TEXT_EVENT(%d,%d,\"%s\")\n",x,y,str1);
        break;
      case TABLE_CLICKED_EVENT:
        sscanf(text,"(%d,%d,%d)",&x,&y,&button);
        printf("TABLE_CLICKED_EVENT(%d,%d,button=%d)\n",x,y,button);
        break;
      case SELECTION_EVENT:
        sscanf(text,"(%d,",&val);
        pvGetText(text,str1);
        printf("SELECTION_EVENT(column=%d,\"%s\")\n",val,str1);
        break;
      case CLIPBOARD_EVENT:
        sscanf(text,"(%d",&val);
        printf("CLIPBOARD_EVENT(id=%d)\n",val);
        printf("clipboard = \n%s\n",p->clipboard);
        break;
      case RIGHT_MOUSE_EVENT:
        printf("RIGHT_MOUSE_EVENT id=%d\n",i);
        //pvPopupMenu(p,-1,"Menu1,Menu2,,Menu3");
        break;
      case KEYBOARD_EVENT:
        sscanf(text,"(%d",&val);
        printf("KEYBOARD_EVENT modifier=%d key=%d\n",i,val);
        break;
      default:
        printf("UNKNOWN_EVENT id=%d %s\n",i,text);
        break;
    }
  }
}
