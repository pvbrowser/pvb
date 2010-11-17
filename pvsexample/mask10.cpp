////////////////////////////////////////////////////////////////////////////
//
// show_mask2 for ProcessViewServer created: Mo Mrz 12 10:21:34 2007
//
////////////////////////////////////////////////////////////////////////////
#include "pvapp.h"

// _begin_of_generated_area_ (do not edit -> use ui2pvc) -------------------

// our mask contains the following objects
enum {
  ID_MAIN_WIDGET = 0,
  svgMain,
  btShow,
  gbNavi,
  btBack,
  obj4,
  obj5,
  slZoom,
  lbMask,
  btHide,
  obj6,
  sbMoveVal,
  btHand,
  btZoom,
  btPopup,
  btUp,
  btLeft,
  btRight,
  btDown,
  btCenter,
  gbExample,
  slLifter,
  btAnimate,
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
  "Example.svg",
  "downarrow.png",
  "",
  "",
  "",
  "",
  "",
  "",
  "rightarrow.png",
  "",
  "",
  "hand.png",
  "zoom.png",
  "popup.png",
  "1uparrow.png",
  "1leftarrow.png",
  "1rightarrow.png",
  "1downarrow.png",
  "1center.png",
  "",
  "",
  "",
  ""};

  static const int widgetType[ID_END_OF_WIDGETS+1] = {
  0,
  TQDraw,
  TQImage,
  TQGroupBox,
  TQPushButton,
  TQLabel,
  TQLabel,
  TQSlider,
  TQLabel,
  TQImage,
  TQLabel,
  TQSpinBox,
  TQImage,
  TQImage,
  TQImage,
  TQImage,
  TQImage,
  TQImage,
  TQImage,
  TQImage,
  TQGroupBox,
  TQSlider,
  TQPushButton,
  -1 };

static int generated_defineMask(PARAM *p)
{
  int w,h,depth;

  if(p == NULL) return 1;
  w = h = depth = strcmp(toolTip[0],whatsThis[0]);
  if(widgetType[0] == -1) return 1;
  if(w==h) depth=0; // fool the compiler
  pvStartDefinition(p,ID_END_OF_WIDGETS);

  pvQDraw(p,svgMain,0);
  pvSetGeometry(p,svgMain,5,5,640,480);

  pvDownloadFile(p,"downarrow.png");
  pvQImage(p,btShow,svgMain,"downarrow.png",&w,&h,&depth);
  pvSetGeometry(p,btShow,15,5,15,15);

  pvQGroupBox(p,gbNavi,svgMain,-1,HORIZONTAL,"      Navigation");
  pvSetGeometry(p,gbNavi,5,5,115,210);

  pvQPushButton(p,btBack,gbNavi);
  pvSetGeometry(p,btBack,5,20,100,25);
  pvSetText(p,btBack,"Back");

  pvQLabel(p,obj4,gbNavi);
  pvSetGeometry(p,obj4,90,95,15,15);
  pvSetText(p,obj4,"+");

  pvQLabel(p,obj5,gbNavi);
  pvSetGeometry(p,obj5,92,185,15,15);
  pvSetText(p,obj5,"-");

  pvQSlider(p,slZoom,gbNavi,0,100,1,50,Vertical);
  pvSetGeometry(p,slZoom,85,110,20,75);

  pvQLabel(p,lbMask,gbNavi);
  pvSetGeometry(p,lbMask,5,45,100,20);
  pvSetText(p,lbMask,"Example.svg");

  pvDownloadFile(p,"rightarrow.png");
  pvQImage(p,btHide,gbNavi,"rightarrow.png",&w,&h,&depth);
  pvSetGeometry(p,btHide,10,0,15,15);

  pvQLabel(p,obj6,gbNavi);
  pvSetGeometry(p,obj6,5,75,65,20);
  pvSetText(p,obj6,"Move-Value");

  pvQSpinBox(p,sbMoveVal,gbNavi,1,100,1);
  pvSetGeometry(p,sbMoveVal,70,75,40,20);

  pvDownloadFile(p,"hand.png");
  pvQImage(p,btHand,gbNavi,"hand.png",&w,&h,&depth);
  pvSetGeometry(p,btHand,15,185,16,16);

  pvDownloadFile(p,"zoom.png");
  pvQImage(p,btZoom,gbNavi,"zoom.png",&w,&h,&depth);
  pvSetGeometry(p,btZoom,34,185,16,16);

  pvDownloadFile(p,"popup.png");
  pvQImage(p,btPopup,gbNavi,"popup.png",&w,&h,&depth);
  pvSetGeometry(p,btPopup,54,185,16,16);

  pvDownloadFile(p,"1uparrow.png");
  pvQImage(p,btUp,gbNavi,"1uparrow.png",&w,&h,&depth);
  pvSetGeometry(p,btUp,30,105,22,22);

  pvDownloadFile(p,"1leftarrow.png");
  pvQImage(p,btLeft,gbNavi,"1leftarrow.png",&w,&h,&depth);
  pvSetGeometry(p,btLeft,5,130,22,22);

  pvDownloadFile(p,"1rightarrow.png");
  pvQImage(p,btRight,gbNavi,"1rightarrow.png",&w,&h,&depth);
  pvSetGeometry(p,btRight,55,130,22,22);

  pvDownloadFile(p,"1downarrow.png");
  pvQImage(p,btDown,gbNavi,"1downarrow.png",&w,&h,&depth);
  pvSetGeometry(p,btDown,30,155,22,22);

  pvDownloadFile(p,"1center.png");
  pvQImage(p,btCenter,gbNavi,"1center.png",&w,&h,&depth);
  pvSetGeometry(p,btCenter,30,130,22,22);

  pvQGroupBox(p,gbExample,svgMain,-1,HORIZONTAL,"Example");
  pvSetGeometry(p,gbExample,5,225,115,95);

  pvQSlider(p,slLifter,gbExample,0,100,1,0,Vertical);
  pvSetGeometry(p,slLifter,5,20,18,69);

  pvQPushButton(p,btAnimate,gbExample);
  pvSetGeometry(p,btAnimate,25,20,85,30);
  pvSetText(p,btAnimate,"Animate!");

  pvQLayoutHbox(p,ID_MAIN_WIDGET,-1);

  pvAddWidgetOrLayout(p,ID_MAIN_WIDGET,svgMain,-1,-1);

  pvEndDefinition(p);
  return 0;
}

// _end_of_generated_area_ (do not edit -> use ui2pvc) ---------------------

#include "mask10_slots.h"

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


int show_mask10(PARAM *p)
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
