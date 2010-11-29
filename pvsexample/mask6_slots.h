//###############################################################
//# mask6_slots.h for ProcessViewServer created: Fr Nov 10 11:54:40 2006
//# please fill out these slots
//# here you find all possible events
//# Yours: Lehrig Software Engineering
//###############################################################
//SVG1

// todo: uncomment me if you want to use this data aquisiton
// also uncomment this classes in main.cpp and pvapp.h
// also remember to uncomment rllib in the project file
//extern rlModbusClient     modbus;
//extern rlSiemensTCPClient siemensTCP;
//extern rlPPIClient        ppi;

typedef struct // (todo: define your data structure here)
{
  rlSvgAnimator svgAnimator;
}
DATA;

static int drawSVG1(PARAM *p, int id, DATA *d)
{
  if(d == NULL) return -1;
  if(d->svgAnimator.isModified == 0) return 0;
  gBeginDraw(p,id);
  d->svgAnimator.writeSocket();
  gEndDraw(p);
  return 0;
}

static int slotInit(PARAM *p, DATA *d)
{
  if(p == NULL || d == NULL) return -1;
  //memset(d,0,sizeof(DATA));
  pvResize(p,0,1280,1024);
  pvPrintf(p,ID_TAB,"SVG1");
  pvSetPixmap(p,back,"back.png");
  d->svgAnimator.setSocket(&p->s);
  d->svgAnimator.setId(svg1);
  d->svgAnimator.read("test1.svg");
  drawSVG1(p,svg1,d);
  pvRequestGeometry(p,svg1);
  pvRequestParentWidgetId(p,svg1);
  return 0;
}

static int slotNullEvent(PARAM *p, DATA *d)
{
  if(p == NULL || d == NULL) return -1;
  return 0;
}

static int slotButtonEvent(PARAM *p, int id, DATA *d)
{
  if(p == NULL || id == 0 || d == NULL) return -1;
  if(id == back) return WELLCOME;
  //rlSetDebugPrintf(1);
  if     (id == buttonShowCircle)
  {
    d->svgAnimator.show("PV.circle1", 1);
    //testing client commands
    //pvClientCommand(p,"pdf","test.pdf");
    //pvClientCommand(p,"html","index.html");
    //pvClientCommand(p,"txt","index.html");
    //pvClientCommand(p,"csv","motor.csv");
  }
  else if(id == buttonHideCircle)
  {
    d->svgAnimator.show("PV.circle1", 0);
  }
  else if(id == buttonHello)
  {
    d->svgAnimator.svgTextPrintf("text1", "Hello");
    pvRequestSvgBoundsOnElement(p,svg1, "PV.circle1");
    pvRequestSvgMatrixForElement(p,svg1, "PV.circle1");
  }
  else if(id == buttonWorld)
  {
    d->svgAnimator.svgTextPrintf("text1", "World");
  }
  else if(id == buttonShowMonitor)
  {
    d->svgAnimator.show("pv.monitor1", 1);
  }
  else if(id == buttonHideMonitor)
  {
    d->svgAnimator.show("pv.monitor1", 0);
  }
  else if(id == buttonMonitorPos1)
  {
    //d->svgAnimator.svgPrintf("pv.monitor1", "transform=","translate(300.5974,286.613)");
    d->svgAnimator.svgPrintf("pv.monitor1", "transform=","translate(100.0,0.0)");
  }
  else if(id == buttonMonitorPos2)
  {
    //d->svgAnimator.svgPrintf("pv.monitor1", "transform=","translate(200.5974,286.613)");
    d->svgAnimator.svgPrintf("pv.monitor1", "transform=","translate(0.0,0.0)");
  }
  else if(id == buttonAnimate)
  {
    for(int i=0; i<100; i++)
    {
      //d->svgAnimator.svgPrintf("pv.monitor1", "transform=","translate(300.5974,286.613)");
      d->svgAnimator.svgPrintf("pv.monitor1", "transform=","translate(100.0,0.0)");
      drawSVG1(p,svg1,d);
      //d->svgAnimator.svgPrintf("pv.monitor1", "transform=","translate(200.5974,286.613)");
      d->svgAnimator.svgPrintf("pv.monitor1", "transform=","translate(0.0,0.0)");
      drawSVG1(p,svg1,d);
    }
  }  
  drawSVG1(p,svg1,d);
  return 0;
}

static int slotButtonPressedEvent(PARAM *p, int id, DATA *d)
{
  if(p == NULL || id == 0 || d == NULL) return -1;
  return 0;
}

static int slotButtonReleasedEvent(PARAM *p, int id, DATA *d)
{
  if(p == NULL || id == 0 || d == NULL) return -1;
  return 0;
}

static int slotTextEvent(PARAM *p, int id, DATA *d, const char *text)
{
  if(p == NULL || id == 0 || d == NULL || text == NULL) return -1;
  float x,y,w,h;
  float m11,m12,m21,m22,det,dx,dy;
  switch(textEventType(text))
  {
    case PLAIN_TEXT_EVENT:
      printf("plain\n");
      break;
    case WIDGET_GEOMETRY:
      int X,Y,W,H;
      getGeometry(text,&X,&Y,&W,&H);
      printf("geometry(%d)=%d,%d,%d,%d\n",id,X,Y,W,H);
      break;
    case PARENT_WIDGET_ID:
      int PID;
      getParentWidgetId(text,&PID);
      printf("parent(%d)=%d\n",id,PID);
      break;
    case SVG_LEFT_BUTTON_PRESSED:
      printf("left pressed\n");
      printf("objectname=%s\n",svgObjectName(text));
      break;
    case SVG_MIDDLE_BUTTON_PRESSED:
      printf("middle pressed\n");
      printf("objectname=%s\n",svgObjectName(text));
      break;
    case SVG_RIGHT_BUTTON_PRESSED:
      printf("right pressed\n");
      printf("objectname=%s\n",svgObjectName(text));
      break;
    case SVG_LEFT_BUTTON_RELEASED:
      printf("left released\n");
      printf("objectname=%s\n",svgObjectName(text));
      break;
    case SVG_MIDDLE_BUTTON_RELEASED:
      printf("middle released\n");
      printf("objectname=%s\n",svgObjectName(text));
      break;
    case SVG_RIGHT_BUTTON_RELEASED:
      printf("right released\n");
      break;
    case SVG_BOUNDS_ON_ELEMENT:
      getSvgBoundsOnElement(text, &x, &y, &w, &h);
      printf("bounds object=%s xywh=%f,%f,%f,%f\n",svgObjectName(text),x,y,w,h);
      break;
    case SVG_MATRIX_FOR_ELEMENT:
      getSvgMatrixForElement(text, &m11, &m12, &m21, &m22, &det, &dx, &dy);
      printf("matrix object=%s m=%f,%f,%f,%f det=%f dx=%f dy=%f\n",svgObjectName(text),
                               m11,m12,m21,m22,det,dx,dy);
      break;
    default:
      printf("default\n");
      break;
  }
  return 0;
}

static int slotSliderEvent(PARAM *p, int id, DATA *d, int val)
{
  if(p == NULL || id == 0 || d == NULL || val < -1000) return -1;
  return 0;
}

static int slotCheckboxEvent(PARAM *p, int id, DATA *d, const char *text)
{
  if(p == NULL || id == 0 || d == NULL || text == NULL) return -1;
  return 0;
}

static int slotRadioButtonEvent(PARAM *p, int id, DATA *d, const char *text)
{
  if(p == NULL || id == 0 || d == NULL || text == NULL) return -1;
  return 0;
}

static int slotGlInitializeEvent(PARAM *p, int id, DATA *d)
{
  if(p == NULL || id == 0 || d == NULL) return -1;
  return 0;
}

static int slotGlPaintEvent(PARAM *p, int id, DATA *d)
{
  if(p == NULL || id == 0 || d == NULL) return -1;
  return 0;
}

static int slotGlResizeEvent(PARAM *p, int id, DATA *d, int width, int height)
{
  if(p == NULL || id == 0 || d == NULL || width < 0 || height < 0) return -1;
  printf("slotGlResizeEvent(%d,%d)\n",width,height);
  return 0;
}

static int slotGlIdleEvent(PARAM *p, int id, DATA *d)
{
  if(p == NULL || id == 0 || d == NULL) return -1;
  return 0;
}

static int slotTabEvent(PARAM *p, int id, DATA *d, int val)
{
  if(p == NULL || id == 0 || d == NULL || val < -1000) return -1;
  return 0;
}

static int slotTableTextEvent(PARAM *p, int id, DATA *d, int x, int y, const char *text)
{
  if(p == NULL || id == 0 || d == NULL || x < -1000 || y < -1000 || text == NULL) return -1;
  return 0;
}

static int slotTableClickedEvent(PARAM *p, int id, DATA *d, int x, int y, int button)
{
  if(p == NULL || id == 0 || d == NULL || x < -1000 || y < -1000 || button < 0) return -1;
  return 0;
}

static int slotSelectionEvent(PARAM *p, int id, DATA *d, int val, const char *text)
{
  if(p == NULL || id == 0 || d == NULL || val < -1000 || text == NULL) return -1;
  return 0;
}

static int slotClipboardEvent(PARAM *p, int id, DATA *d, int val)
{
  if(p == NULL || id == 0 || d == NULL || val < -1000) return -1;
  return 0;
}

static int slotRightMouseEvent(PARAM *p, int id, DATA *d, const char *text)
{
  if(p == NULL || id == 0 || d == NULL || text == NULL) return -1;
  //pvPopupMenu(p,-1,"Menu1,Menu2,,Menu3");
  return 0;
}

static int slotKeyboardEvent(PARAM *p, int id, DATA *d, int val, int modifier)
{
  if(p == NULL || id == 0 || d == NULL || val < -1000 || modifier < -1000) return -1;
  return 0;
}

static int slotMouseMovedEvent(PARAM *p, int id, DATA *d, float x, float y)
{
  if(p == NULL || id == 0 || d == NULL || x < -1000 || y < -1000) return -1;
  return 0;
}

static int slotMousePressedEvent(PARAM *p, int id, DATA *d, float x, float y)
{
  if(p == NULL || id == 0 || d == NULL || x < -1000 || y < -1000) return -1;
  return 0;
}

static int slotMouseReleasedEvent(PARAM *p, int id, DATA *d, float x, float y)
{
  if(p == NULL || id == 0 || d == NULL || x < -1000 || y < -1000) return -1;
  return 0;
}

static int slotMouseOverEvent(PARAM *p, int id, DATA *d, int enter)
{
  if(p == NULL || id == 0 || d == NULL || enter < -1000) return -1;
  return 0;
}

static int slotUserEvent(PARAM *p, int id, DATA *d, const char *text)
{
  if(p == NULL || id == 0 || d == NULL || text == NULL) return -1;
  return 0;
}
