////////////////////////////////////////////////////////////////////////////
//
// show_mask2 for ProcessViewServer created: Mi Nov 8 15:40:04 2006
//
////////////////////////////////////////////////////////////////////////////
#include "pvapp.h"

// _begin_of_generated_area_ (do not edit -> use ui2pvc) -------------------

// our mask contains the following objects
enum {
  ID_MAIN_WIDGET = 0,
  back,
  textLabelNumThreads,
  textEditOutput,
  textEditInput,
  pushButtonSend,
  lineEditAlias,
  labelChat,
  labelAlias,
  layout1,
  layout2,
  ID_END_OF_WIDGETS
};

// our mask contains the following widget names
  static const char *widgetName[] = {
  "ID_MAIN_WIDGET",
  "back",
  "textLabelNumThreads",
  "textEditOutput",
  "textEditInput",
  "pushButtonSend",
  "lineEditAlias",
  "labelChat",
  "labelAlias",
  "layout1",
  "layout2",
  "ID_END_OF_WIDGETS",
  ""};

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
  ""};

  static const char *whatsThis[] = {
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

  static const int widgetType[ID_END_OF_WIDGETS+1] = {
  0,
  TQPushButton,
  TQLabel,
  TQMultiLineEdit,
  TQMultiLineEdit,
  TQPushButton,
  TQLineEdit,
  TQLabel,
  TQLabel,
  -1 };

static int generated_defineMask(PARAM *p)
{
  int w,h,depth;

  if(p == NULL) return 1;
  if(widgetName[0] == NULL) return 1; // suppress unused warning
  w = h = depth = strcmp(toolTip[0],whatsThis[0]);
  if(widgetType[0] == -1) return 1;
  if(w==h) depth=0; // fool the compiler
  pvStartDefinition(p,ID_END_OF_WIDGETS);

  pvQPushButton(p,back,0);
  pvSetGeometry(p,back,10,10,100,30);
  pvSetText(p,back,pvtr("&Back"));
  pvSetFont(p,back,"Sans Serif",9,0,0,0,0);

  pvQLabel(p,textLabelNumThreads,0);
  pvSetGeometry(p,textLabelNumThreads,125,10,100,30);
  pvSetText(p,textLabelNumThreads,pvtr("numThreads"));
  pvSetFont(p,textLabelNumThreads,"Sans Serif",9,0,0,0,0);

  pvQMultiLineEdit(p,textEditOutput,0,0,-1);
  pvSetGeometry(p,textEditOutput,10,55,635,290);
  pvSetFont(p,textEditOutput,"Courier",9,0,0,0,0);

  pvQMultiLineEdit(p,textEditInput,0,1,-1);
  pvSetGeometry(p,textEditInput,10,355,635,200);
  pvSetFont(p,textEditInput,"Courier",9,0,0,0,0);
  pvSetMaxSize(p,textEditInput,99999,200);

  pvQPushButton(p,pushButtonSend,0);
  pvSetGeometry(p,pushButtonSend,545,570,100,30);
  pvSetText(p,pushButtonSend,pvtr("&Send <ESC>"));
  pvSetFont(p,pushButtonSend,"Sans Serif",9,0,0,0,0);

  pvQLineEdit(p,lineEditAlias,0);
  pvSetGeometry(p,lineEditAlias,120,575,180,30);
  pvSetText(p,lineEditAlias,pvtr("unknown"));
  pvSetFont(p,lineEditAlias,"Sans Serif",9,0,0,0,0);

  pvQLabel(p,labelChat,0);
  pvSetGeometry(p,labelChat,535,15,110,30);
  pvSetText(p,labelChat,pvtr("pvbrowser Chat"));
  pvSetFont(p,labelChat,"Sans Serif",9,1,1,0,0);

  pvQLabel(p,labelAlias,0);
  pvSetGeometry(p,labelAlias,10,575,100,30);
  pvSetText(p,labelAlias,pvtr("Alias:"));
  pvSetFont(p,labelAlias,"Sans Serif",9,0,0,0,0);

  pvQLayoutVbox(p,ID_MAIN_WIDGET,-1);

  pvQLayoutHbox(p,layout1,-1);

  pvQLayoutHbox(p,layout2,-1);

  pvAddWidgetOrLayout(p,layout1,back,-1,-1);
  pvAddWidgetOrLayout(p,layout1,textLabelNumThreads,-1,-1);
  pvAddStretch(p,layout1,1);
  pvAddWidgetOrLayout(p,layout1,labelChat,-1,-1);
  pvAddWidgetOrLayout(p,layout2,labelAlias,-1,-1);
  pvAddWidgetOrLayout(p,layout2,lineEditAlias,-1,-1);
  pvAddStretch(p,layout2,1);
  pvAddWidgetOrLayout(p,layout2,pushButtonSend,-1,-1);
  pvAddWidgetOrLayout(p,ID_MAIN_WIDGET,layout1,-1,-1);
  pvAddWidgetOrLayout(p,ID_MAIN_WIDGET,textEditOutput,-1,-1);
  pvAddWidgetOrLayout(p,ID_MAIN_WIDGET,textEditInput,-1,-1);
  pvAddWidgetOrLayout(p,ID_MAIN_WIDGET,layout2,-1,-1);

  pvEndDefinition(p);
  return 0;
}

// _end_of_generated_area_ (do not edit -> use ui2pvc) ---------------------

#include "mask2_slots.h"

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


int show_mask2(PARAM *p)
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
