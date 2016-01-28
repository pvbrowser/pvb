////////////////////////////////////////////////////////////////////////////
//
// show_mask1 for ProcessViewServer created: Do Nov 20 07:46:31 2008
//
////////////////////////////////////////////////////////////////////////////
#include "pvapp.h"

// _begin_of_generated_area_ (do not edit -> use ui2pvc) -------------------

// our mask contains the following objects
enum {
  ID_MAIN_WIDGET = 0,
  OpenGL1,
  group1,
  sliderPos,
  sliderScale,
  btCenter,
  btLeft,
  btRight,
  btUp,
  btDown,
  obj1,
  btBack,
  obj2,
  layout1,
  ID_END_OF_WIDGETS
};

  static const char *toolTip[] = {
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  ""};

  static const char *whatsThis[] = {
  "",
  "",
  "",
  "",
  "",
  "1center.png",
  "1leftarrow.png",
  "1rightarrow.png",
  "1uparrow.png",
  "1downarrow.png",
  "1downarrow.png",
  "",
  "",
  ""};

  static const int widgetType[ID_END_OF_WIDGETS+1] = {
  0,
  TQGl,
  TQGroupBox,
  TQSlider,
  TQSlider,
  TQImage,
  TQImage,
  TQImage,
  TQImage,
  TQImage,
  TQLabel,
  TQPushButton,
  TQLabel,
  -1 };

static int generated_defineMask(PARAM *p)
{
  int w,h,depth;

  if(p == NULL) return 1;
  w = h = depth = strcmp(toolTip[0],whatsThis[0]);
  if(widgetType[0] == -1) return 1;
  if(w==h) depth=0; // fool the compiler
  pvStartDefinition(p,ID_END_OF_WIDGETS);

  printf("generated_defineMaks 1\n");
  pvQGL(p,OpenGL1,0);
  printf("generated_defineMaks 2\n");
  pvSetGeometry(p,OpenGL1,105,0,1024,768);
  printf("generated_defineMaks 3\n");
  pvGlBegin(p,OpenGL1);
  printf("generated_defineMaks 4\n");
  initializeGL(p);    // (todo: write your gl initialization routine) see example
  printf("generated_defineMaks 5\n");
  resizeGL(p,1024,768);  // (todo: write your resize routine) see example
  printf("generated_defineMaks 6\n");
  pvGlEnd(p);
  printf("generated_defineMaks 7\n");

  pvQGroupBox(p,group1,0,-1,HORIZONTAL,"w2d");
  pvSetGeometry(p,group1,0,0,100,515);
  pvSetFont(p,group1,"Sans Serif",10,0,0,0,0);
  pvSetMaxSize(p,group1,100,99999);

  pvQSlider(p,sliderPos,group1,0,100,1,10,Horizontal);
  pvSetGeometry(p,sliderPos,10,145,80,30);
  pvSetFont(p,sliderPos,"Sans Serif",10,0,0,0,0);

  pvQSlider(p,sliderScale,group1,0,100,1,31,Vertical);
  pvSetGeometry(p,sliderScale,70,55,25,75);
  pvSetFont(p,sliderScale,"Sans Serif",10,0,0,0,0);

  pvDownloadFile(p,"1center.png");
  pvQImage(p,btCenter,group1,"1center.png",&w,&h,&depth);
  pvSetGeometry(p,btCenter,25,85,25,25);
  pvSetFont(p,btCenter,"Sans Serif",10,0,0,0,0);

  pvDownloadFile(p,"1leftarrow.png");
  pvQImage(p,btLeft,group1,"1leftarrow.png",&w,&h,&depth);
  pvSetGeometry(p,btLeft,5,85,25,25);
  pvSetFont(p,btLeft,"Sans Serif",10,0,0,0,0);

  pvDownloadFile(p,"1rightarrow.png");
  pvQImage(p,btRight,group1,"1rightarrow.png",&w,&h,&depth);
  pvSetGeometry(p,btRight,45,85,25,25);
  pvSetFont(p,btRight,"Sans Serif",10,0,0,0,0);

  pvDownloadFile(p,"1uparrow.png");
  pvQImage(p,btUp,group1,"1uparrow.png",&w,&h,&depth);
  pvSetGeometry(p,btUp,25,65,25,25);
  pvSetFont(p,btUp,"Sans Serif",10,0,0,0,0);

  pvDownloadFile(p,"1downarrow.png");
  pvQImage(p,btDown,group1,"1downarrow.png",&w,&h,&depth);
  pvSetGeometry(p,btDown,25,105,25,25);
  pvSetFont(p,btDown,"Sans Serif",10,0,0,0,0);

  pvQLabel(p,obj1,group1);
  pvSetGeometry(p,obj1,80,45,15,15);
  pvSetText(p,obj1,"+");
  pvSetFont(p,obj1,"Sans Serif",10,0,0,0,0);

  pvQPushButton(p,btBack,group1);
  pvSetGeometry(p,btBack,5,25,75,30);
  pvSetText(p,btBack,"Back");
  pvSetFont(p,btBack,"Sans Serif",10,0,0,0,0);

  pvQLabel(p,obj2,group1);
  pvSetGeometry(p,obj2,80,135,15,15);
  pvSetText(p,obj2,"-");
  pvSetFont(p,obj2,"Sans Serif",10,0,0,0,0);

  pvQLayoutHbox(p,ID_MAIN_WIDGET,-1);

  pvQLayoutVbox(p,layout1,-1);

  pvAddWidgetOrLayout(p,ID_MAIN_WIDGET,layout1,-1,-1);
  pvAddWidgetOrLayout(p,ID_MAIN_WIDGET,OpenGL1,-1,-1);
  pvAddWidgetOrLayout(p,layout1,group1,-1,-1);

  printf("generated_defineMaks x\n");
  pvEndDefinition(p);
  printf("generated_defineMaks end\n");
  return 0;
}

// _end_of_generated_area_ (do not edit -> use ui2pvc) ---------------------

#include "mask11_slots.h"

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
  return 0;
}

static int readData(DATA *d) // from shared memory, database or something else
{
  if(d == NULL) return 1;
  // (todo: add your code here)
  return 0;
}

int show_mask11(PARAM *p)
{
  DATA d;
  char event[MAX_EVENT_LENGTH];
  char text[MAX_EVENT_LENGTH];
  char str1[MAX_EVENT_LENGTH];
  int  i,w,h,val,x,y,button,ret;
  float xval, yval;

  defineMask(p);
  //rlSetDebugPrintf(1);
  if((ret=slotInit(p,&d)) != 0) return ret;
  readData(&d); // from shared memory, database or something else
  showData(p,&d);
  pvClearMessageQueue(p);
  while(1)
  {
    pvPollEvent(p,event);
    switch(pvParseEvent(event, &i, text))
    {
      case NULL_EVENT:
        readData(&d); // from shared memory, database or something else
        showData(p,&d);
        if((ret=slotNullEvent(p,&d)) != 0) return ret;
        break;
      case BUTTON_EVENT:
        if(trace) printf("BUTTON_EVENT id=%d\n",i);
        if((ret=slotButtonEvent(p,i,&d)) != 0) return ret;
        break;
      case BUTTON_PRESSED_EVENT:
        if(trace) printf("BUTTON_PRESSED_EVENT id=%d\n",i);
        if((ret=slotButtonPressedEvent(p,i,&d)) != 0) return ret;
        break;
      case BUTTON_RELEASED_EVENT:
        if(trace) printf("BUTTON_RELEASED_EVENT id=%d\n",i);
        if((ret=slotButtonReleasedEvent(p,i,&d)) != 0) return ret;
        break;
      case TEXT_EVENT:
        if(trace) printf("TEXT_EVENT id=%d %s\n",i,text);
        if((ret=slotTextEvent(p,i,&d,text)) != 0) return ret;
        break;
      case SLIDER_EVENT:
        sscanf(text,"(%d)",&val);
        if(trace) printf("SLIDER_EVENT val=%d\n",val);
        if((ret=slotSliderEvent(p,i,&d,val)) != 0) return ret;
        break;
      case CHECKBOX_EVENT:
        if(trace) printf("CHECKBOX_EVENT id=%d %s\n",i,text);
        if((ret=slotCheckboxEvent(p,i,&d,text)) != 0) return ret;
        break;
      case RADIOBUTTON_EVENT:
        if(trace) printf("RADIOBUTTON_EVENT id=%d %s\n",i,text);
        if((ret=slotRadioButtonEvent(p,i,&d,text)) != 0) return ret;
        break;
      case GL_INITIALIZE_EVENT:
        if(trace) printf("you have to call initializeGL()\n");
        if((ret=slotGlInitializeEvent(p,i,&d)) != 0) return ret;
        break;
      case GL_PAINT_EVENT:
        if(trace) printf("you have to call paintGL()\n");
        if((ret=slotGlPaintEvent(p,i,&d)) != 0) return ret;
        break;
      case GL_RESIZE_EVENT:
        sscanf(text,"(%d,%d)",&w,&h);
        if(trace) printf("you have to call resizeGL(w,h)\n");
        if((ret=slotGlResizeEvent(p,i,&d,w,h)) != 0) return ret;
        break;
      case GL_IDLE_EVENT:
        if((ret=slotGlIdleEvent(p,i,&d)) != 0) return ret;
        break;
      case TAB_EVENT:
        sscanf(text,"(%d)",&val);
        if(trace) printf("TAB_EVENT(%d,page=%d)\n",i,val);
        if((ret=slotTabEvent(p,i,&d,val)) != 0) return ret;
        break;
      case TABLE_TEXT_EVENT:
        sscanf(text,"(%d,%d,",&x,&y);
        pvGetText(text,str1);
        if(trace) printf("TABLE_TEXT_EVENT(%d,%d,\"%s\")\n",x,y,str1);
        if((ret=slotTableTextEvent(p,i,&d,x,y,str1)) != 0) return ret;
        break;
      case TABLE_CLICKED_EVENT:
        sscanf(text,"(%d,%d,%d)",&x,&y,&button);
        if(trace) printf("TABLE_CLICKED_EVENT(%d,%d,button=%d)\n",x,y,button);
        if((ret=slotTableClickedEvent(p,i,&d,x,y,button)) != 0) return ret;
        break;
      case SELECTION_EVENT:
        sscanf(text,"(%d,",&val);
        pvGetText(text,str1);
        if(trace) printf("SELECTION_EVENT(column=%d,\"%s\")\n",val,str1);
        if((ret=slotSelectionEvent(p,i,&d,val,str1)) != 0) return ret;
        break;
      case CLIPBOARD_EVENT:
        sscanf(text,"(%d",&val);
        if(trace) printf("CLIPBOARD_EVENT(id=%d)\n",val);
        if(trace) printf("clipboard = \n%s\n",p->clipboard);
        if((ret=slotClipboardEvent(p,i,&d,val)) != 0) return ret;
        break;
      case RIGHT_MOUSE_EVENT:
        if(trace) printf("RIGHT_MOUSE_EVENT id=%d text=%s\n",i,text);
        if((ret=slotRightMouseEvent(p,i,&d,text)) != 0) return ret;
        break;
      case KEYBOARD_EVENT:
        sscanf(text,"(%d",&val);
        if(trace) printf("KEYBOARD_EVENT modifier=%d key=%d\n",i,val);
        if((ret=slotKeyboardEvent(p,i,&d,val,i)) != 0) return ret;
        break;
      case PLOT_MOUSE_MOVED_EVENT:
        sscanf(text,"(%f,%f)",&xval,&yval);
        if(trace) printf("PLOT_MOUSE_MOVE %f %f\n",xval,yval);
        if((ret=slotMouseMovedEvent(p,i,&d,xval,yval)) != 0) return ret;
        break;
      case PLOT_MOUSE_PRESSED_EVENT:
        sscanf(text,"(%f,%f)",&xval,&yval);
        if(trace) printf("PLOT_MOUSE_PRESSED %f %f\n",xval,yval);
        if((ret=slotMousePressedEvent(p,i,&d,xval,yval)) != 0) return ret;
        break;
      case PLOT_MOUSE_RELEASED_EVENT:
        sscanf(text,"(%f,%f)",&xval,&yval);
        if(trace) printf("PLOT_MOUSE_RELEASED %f %f\n",xval,yval);
        if((ret=slotMouseReleasedEvent(p,i,&d,xval,yval)) != 0) return ret;
        break;
      case MOUSE_OVER_EVENT:
        sscanf(text,"%d",&val);
        if(trace) printf("MOUSE_OVER_EVENT %d\n",val);
        if((ret=slotMouseOverEvent(p,i,&d,val)) != 0) return ret;
        break;
      case USER_EVENT:
        if(trace) printf("USER_EVENT id=%d %s\n",i,text);
        if((ret=slotUserEvent(p,i,&d,text)) != 0) return ret;
        break;
      default:
        if(trace) printf("UNKNOWN_EVENT id=%d %s\n",i,text);
        break;
    }
  }
}
