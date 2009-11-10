////////////////////////////////////////////////////////////////////////////
//
// show_maskvtk for ProcessViewServer created: Thu Jul 18 09:21:46 2002
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
  PushButtonBack,
  TabWidget2,
  tab1,
  VtkTclWidget1,
  Slider2,
  Slider3,
  Slider1,
  tab2,
  VtkTclWidgetXY,
  Slider4,
  Slider5,
  Slider6,
  ID_END_OF_WIDGETS
};

static int generated_defineMask(PARAM *p)
{
  int w,h,depth;

  if(p == NULL) return 1;
  w = h = depth = 0;
  if(w==h) depth=0; // fool the compiler
  pvStartDefinition(p,ID_END_OF_WIDGETS);


  pvQPushButton(p,PushButtonBack,0);
  pvSetGeometry(p,PushButtonBack,10,720,70,30);
  pvSetText(p,PushButtonBack,"Back");

  pvQTabWidget(p,TabWidget2,0);
  pvSetGeometry(p,TabWidget2,10,10,911,700);

  pvQWidget(p,tab1,TabWidget2);
  pvAddTab(p,TabWidget2,tab1,"Tab 1");

  pvQVtkTclWidget(p,VtkTclWidget1,tab1);
  pvSetGeometry(p,VtkTclWidget1,8,4,740,660);

  pvQSlider(p,Slider2,tab1,0,99,10,50,Vertical);
  pvSetGeometry(p,Slider2,798,4,31,230);

  pvQSlider(p,Slider3,tab1,0,99,10,0,Vertical);
  pvSetGeometry(p,Slider3,838,4,31,231);

  pvQSlider(p,Slider1,tab1,0,99,10,50,Vertical);
  pvSetGeometry(p,Slider1,758,4,31,231);

  pvQWidget(p,tab2,TabWidget2);
  pvAddTab(p,TabWidget2,tab2,"Tab 2");

  pvQVtkTclWidget(p,VtkTclWidgetXY,tab2);
  pvSetGeometry(p,VtkTclWidgetXY,9,9,770,651);

  pvQSlider(p,Slider4,tab2,0,99,10,50,Vertical);
  pvSetGeometry(p,Slider4,790,10,31,231);

  pvQSlider(p,Slider5,tab2,0,99,10,50,Vertical);
  pvSetGeometry(p,Slider5,830,10,31,231);

  pvQSlider(p,Slider6,tab2,0,99,10,50,Vertical);
  pvSetGeometry(p,Slider6,869,9,31,231);

  pvEndDefinition(p);
  return 0;
}

// _end_of_generated_area_ (do not edit -> use ui2pvc) ---------------------

static int defineMask(PARAM *p)
{
  generated_defineMask(p);

  pvDownloadFile(p,"data1.vtk");
  pvDownloadFile(p,"combq.bin");
  pvDownloadFile(p,"combxyz.bin");

  pvVtkTclScript(p,VtkTclWidget1,"surface.tcl");
  pvVtkTclPrintf(p,VtkTclWidget1,"dataActor RotateX %d",0);
  pvVtkTclPrintf(p,VtkTclWidget1,"dataActor RotateY %d",0);
  pvVtkTclPrintf(p,VtkTclWidget1,"renWin Render");
  pvVtkTclPrintf(p,VtkTclWidget1,"reader Modified");
  pvVtkTclPrintf(p,VtkTclWidget1,"reader Update");
  pvVtkTclPrintf(p,VtkTclWidget1,"renWin Render");

  pvVtkTclScript(p,VtkTclWidgetXY,"xyplot.tcl");
  pvVtkTclPrintf(p,VtkTclWidgetXY,"pl3d Modified");
  pvVtkTclPrintf(p,VtkTclWidgetXY,"pl3d Update");
  //pvVtkTclPrintf(p,VtkTclWidgetXY,"renderer2 Render");
  
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

int show_maskvtk(PARAM *p)
{
  DATA d;
  char event[MAX_EVENT_LENGTH];
  char text[MAX_EVENT_LENGTH];
  char str1[MAX_EVENT_LENGTH];
  int  i,w,h,val,x,y,button;
  int  xangle,yangle;
  int  xangle2,yangle2;
  int  roll,elevation;

  xangle = yangle = 0;
  xangle2 = yangle2 = 0;
  roll = elevation = 50;
  defineMask(p);
  memset(&d,0,sizeof(DATA));
  readData(&d); // from shared memory, datab or something else
  showData(p,&d);
  while(1)
  {
    pvPollEvent(p,event);
    switch(pvParseEvent(event, &i, text))
    {
      case NULL_EVENT:
        readData(&d); // from shared memory, database or something else
        showData(p,&d);
        break;
      case BUTTON_EVENT:
        printf("BUTTON_EVENT id=%d\n",i);
        if(i == PushButtonBack) return 1;
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
        if(i == Slider1)
        {
          int delta;
          delta = (val-50)*3 - xangle;
          xangle += delta;
          pvVtkTclScript(p,VtkTclWidget1,"update_surface.tcl");
          pvVtkTclPrintf(p,VtkTclWidget1,"dataActor RotateX %d",delta);
          pvVtkTclPrintf(p,VtkTclWidget1,"renWin Render");
        }
        if(i == Slider2)
        {
          int delta;
          delta = (val-50)*3 - yangle;
          yangle += delta;
          pvVtkTclScript(p,VtkTclWidget1,"update_surface.tcl");
          pvVtkTclPrintf(p,VtkTclWidget1,"dataActor RotateY %d",delta);
          pvVtkTclPrintf(p,VtkTclWidget1,"renWin Render");
        }
        if(i == Slider3)
        {
          float zoom = (float) val;
          zoom += 1.0;
          zoom /= 50.0;
          pvVtkTclPrintf(p,VtkTclWidget1,"$cam1 Zoom %f",zoom);
          pvVtkTclPrintf(p,VtkTclWidget1,"renWin Render");
        }
        if(i == Slider4)
        {
          float angle;

          if(val > elevation) angle = 3.0f;
          else                angle = -3.0f;
          elevation = val;
          pvVtkTclPrintf(p,VtkTclWidgetXY,"$cam1 Elevation %f",angle);
          pvVtkTclPrintf(p,VtkTclWidgetXY,"renWin Render");
        }
        if(i == Slider5)
        {
          float angle;

          if(val > roll) angle = 9.0f;
          else           angle = -9.0f;
          roll = val;
          pvVtkTclPrintf(p,VtkTclWidgetXY,"$cam1 Roll %f",angle);
          pvVtkTclPrintf(p,VtkTclWidgetXY,"renWin Render");
        }
        if(i == Slider6)
        {
          float zoom;

          zoom = ((float)(val + 50)) / 100.0f;
          pvVtkTclPrintf(p,VtkTclWidgetXY,"$cam1 Zoom %f",zoom);
          pvVtkTclPrintf(p,VtkTclWidgetXY,"renWin Render");
        }
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
      default:
        printf("UNKNOWN_EVENT id=%d %s\n",i,text);
        break;
    }
  }
}


