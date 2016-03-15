//###############################################################
//# mask3_slots.h for ProcessViewServer created: Do Nov 9 15:53:54 2006
//# please fill out these slots
//# here you find all possible events
//# Yours: Lehrig Software Engineering
//###############################################################
//PLOT1

// todo: uncomment me if you want to use this data aquisiton
// also uncomment this classes in main.cpp and pvapp.h
// also remember to uncomment rllib in the project file
//extern rlModbusClient     modbus;
//extern rlSiemensTCPClient siemensTCP;
//extern rlPPIClient        ppi;

typedef struct // (todo: define your data structure here)
{
  int pause, display;
  int step1, step2;
  float phi1, phi2;
  double x1[100], sin_x[100];
  double x2[100], cos_x[100];
  int tab;
  int modalInput;
}
DATA;

static int drawGraphic1(PARAM *p, int id, DATA *d)
{
  int fontsize;
  int i;
  float x1[100], sin_x[100];
  float x2[100], cos_x[100];

  for(i=0;i<100;i++)
  {
    x1[i] = (float) d->x1[i]; sin_x[i]= (float) d->sin_x[i];
    x2[i] = (float) d->x2[i]; cos_x[i]= (float) d->cos_x[i];
  }

  fontsize = 12;

  gBeginDraw    (p, id);

  gSetColor     (p, BLACK);
  gSetFont      (p, TIMES, fontsize, Normal, 0);
  gBoxWithText  (p, 50, 50, 1050, 550, fontsize, "x/radiant", "sin(x), cos(x)", NULL);
  gXAxis        (p, 0, 1.0f, 2.0f*PI, 1);
  gYAxis        (p, -1.5f, 0.5f, 1.5f, 1);

  gSetStyle     (p, 2);
  gXGrid        (p);
  gYGrid        (p);

  gSetWidth     (p, 3);
  gSetStyle     (p, 1);
  gSetColor     (p, RED);
  gLine         (p, x1, sin_x, 100);
  gSetColor     (p, GREEN);
  gLine         (p, x2, cos_x, 100);

  fontsize = 18;
  gSetColor     (p, BLUE);
  gSetFont      (p, TIMES, fontsize, Bold, 0);
  gText         (p, 50, 50-fontsize*2, "This is a Diagram", ALIGN_LEFT);

  gEndDraw      (p);
  return 0;
}

static int slotInit(PARAM *p, DATA *d)
{
  if(p == NULL || d == NULL) return -1;
  memset(d,0,sizeof(DATA));
  pvResize(p,0,1280,1024);
  pvPrintf(p,ID_TAB,"Plot");
  pvSetPixmap(p,back,"back.png");
  pvSetChecked(p,radioButton1,1);

  d->step1=1;
  d->step2=1;
  d->tab=0;
  pvDisplayFloat(p,lCDNumber1,1);
  pvSetValue(p,progressBar1,1);

  // qwt plot begin ---------------------------------------------
  qpwSetCanvasBackground(p,qwtPlot1,239,239,239);
  qpwEnableAxis(p,qwtPlot1,yLeft);
  qpwEnableAxis(p,qwtPlot1,xBottom);
  qpwSetTitle(p,qwtPlot1,"Trigonometric");

  qpwEnableOutline(p,qwtPlot1,1);
  qpwSetOutlinePen(p,qwtPlot1,GREEN);

  // legend
  qpwSetAutoLegend(p,qwtPlot1,1);
  qpwEnableLegend(p,qwtPlot1,1);
  qpwSetLegendPos(p,qwtPlot1,BottomLegend);
  qpwSetLegendFrameStyle(p,qwtPlot1,Box|Sunken);

  // axes
  qpwSetAxisTitle(p,qwtPlot1,xBottom, "Alpha");
  // qpwSetAxisScaleDraw(p,qwtPlot1,xBottom, "hh:mm:ss");
  qpwSetAxisTitle(p,qwtPlot1,yLeft, "f(Alpha)");

  // curves
  qpwInsertCurve(p,qwtPlot1, 0, "Sinus(Alpha)");
  qpwSetCurvePen(p,qwtPlot1, 0, BLUE, 3, DashDotLine);
  qpwSetCurveYAxis(p,qwtPlot1, 0, yLeft);

  qpwInsertCurve(p, qwtPlot1, 1, "Cosinus(Alpha)");
  qpwSetCurvePen(p, qwtPlot1, 1, GREEN, 3, DashDotLine);
  qpwSetCurveYAxis(p, qwtPlot1, 1, yLeft);
  // qwt plot end --------------------------------------------------
  return 0;
}

static int slotNullEvent(PARAM *p, DATA *d)
{
  if(p == NULL || d == NULL) return -1;
  if(d->pause) return 0;
  switch(d->display)
  {
    case 0:
      memset(d->cos_x, 0, sizeof(d->cos_x));
      break;
    case 1:
      memset(d->sin_x, 0, sizeof(d->sin_x));
      break;
    case 2:
      break;
    default:
      break;
  }

  // draw qwt_plot graphic 
  if(d->tab == 0)
  {
    qpwSetCurveData(p, qwtPlot1, 0, 100, d->x1, d->sin_x);
    qpwSetCurveData(p, qwtPlot1, 1, 100, d->x2, d->cos_x);
    qpwReplot(p,qwtPlot1);
  }

  // draw graphic with gDraw routines
  if(d->tab == 1) drawGraphic1(p, qDraw1, d);

  // animate some data
  d->phi1 += d->step1/10.0f;
  d->phi2 += d->step2/10.0f;
  if(d->phi1 > (1000.0f*2.0f*PI)) d->phi1 = 0.0f;
  if(d->phi2 > (1000.0f*2.0f*PI)) d->phi2 = 0.0f;
  for(int i=0; i<100; i++)
  {
    d->x1[i]=(((float) i) * 2.0f * PI) / 100.0f;
    d->sin_x[i]=sinf((float) d->x1[i]+d->phi1);
    d->x2[i]=(((float) i) * 2.0f * PI) / 100.0f;
    d->cos_x[i]=cosf((float) d->x2[i]+d->phi2);
  }
  return 0;
}

static int slotButtonEvent(PARAM *p, int id, DATA *d)
{
  if(p == NULL || id == 0 || d == NULL) return -1;
  if(id == back) return WELLCOME;
  if(id == buttonModalDialog)
  {
    pvRunModalDialog(p,330,100,show_mask4,&d->modalInput,NULL,(showDataCast)slotNullEvent,d);
    pvPrintf(p, multiLine1, "Input = %d\n", d->modalInput);
  }
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
  return 0;
}

static int slotSliderEvent(PARAM *p, int id, DATA *d, int val)
{
  if(p == NULL || id == 0 || d == NULL || val < -1000) return -1;
  switch(id)
  {
    case slider1:
      d->step1=val;
      pvDisplayFloat(p, lCDNumber1, (float) val);
      break;
    case qwtWheel1:
      d->step2=val;
      pvSetValue(p, progressBar1, val);
      break;
  }
  return 0;
}

static int slotCheckboxEvent(PARAM *p, int id, DATA *d, const char *text)
{
  if(p == NULL || id == 0 || d == NULL || text == NULL) return -1;
  if(id == checkBox1)
  {
    if(strcmp(text,"(1)") == 0) d->pause = 1;
    else                        d->pause = 0;
    pvPrintf(p, multiLine1, "Pause = %s\n", (d->pause?"TRUE":"FALSE"));
  }
  return 0;
}

static int slotRadioButtonEvent(PARAM *p, int id, DATA *d, const char *text)
{
  if(p == NULL || id == 0 || d == NULL || text == NULL) return -1;
  switch(id)
  {
    case radioButton1:
      if(strcmp(text, "(0)")) 
      {
        pvPrintf(p, multiLine1, "Dispay->Sin\n");
        d->display=0;
      }
      break;
    case radioButton2:
      if(strcmp(text, "(0)")) 
      {
        pvPrintf(p, multiLine1, "Dispay->Cos\n");
        d->display=1;
      }
      break;
    case radioButton3:
      if(strcmp(text, "(0)")) 
      {
        pvPrintf(p, multiLine1, "Dispay->Both\n");
        d->display=2;
      }
      break;
    default:
      d->display=2;
      break;
  }
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
  d->tab = val;
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
