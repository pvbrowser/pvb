////////////////////////////////////////////////////////////////////////////
//
// show_qwt for ProcessViewServer created: Thu Jun 19 12:10:55 2003
//
////////////////////////////////////////////////////////////////////////////
//#define USE_DOM

#ifdef USE_DOM
#include "qdom.h"
#include "qfile.h"
#endif
#include "pvapp.h"
#include <math.h>
#define PI 3.141592654f

typedef struct // (todo: define your data structure here)
{
}
DATA;

// _begin_of_generated_area_ (do not edit -> use ui2pvc) -------------------

// our mask contains the following objects
enum {
  ID_MAIN_WIDGET = 0,
  pushButtonBack,
  tabWidget2,
  tab1,
  wtPlot1,
  tab2,
  wtCounter1,
  wtWheel1,
  wtCompass1,
  wtDial1,
  wtThermo1,
  wtScale1,
  wtKnob1,
  wtSlider1,
  tab3,
  dateEdit1,
  timeEdit1,
  dateTimeEdit1,
  frame3,
  tab4,
  drawSVG,
  Tab5,
  MNGlabel,
  textLabel1_2,
  textLabel1_3,
  lineEdit1,
  textLabel1,
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
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "ajpg.jpg",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  ""};

static int generated_defineMask(PARAM *p)
{
  int w,h,depth;

  if(p == NULL) return 1;
  w = h = depth = 0;
  if(w==h) depth=0; // fool the compiler
  if(strcmp(toolTip[0],whatsThis[0]) == 0) depth = 0; // you may use this variables from Qt Designer for your purpose
  pvStartDefinition(p,ID_END_OF_WIDGETS);


  pvQPushButton(p,pushButtonBack,0);
  pvSetGeometry(p,pushButtonBack,10,540,60,31);
  pvSetText(p,pushButtonBack,"Back");

  pvQTabWidget(p,tabWidget2,0);
  pvSetGeometry(p,tabWidget2,10,10,801,521);

  pvQWidget(p,tab1,tabWidget2);
  pvAddTab(p,tabWidget2,tab1,"QwtPlot");

  pvQwtPlotWidget(p,wtPlot1,tab1,5,2);
  pvSetGeometry(p,wtPlot1,0,10,781,471);
  qpwSetCanvasBackground(p,wtPlot1,33,67,238);
  qpwEnableAxis(p,wtPlot1,yLeft);
  qpwEnableAxis(p,wtPlot1,xBottom);

  pvQWidget(p,tab2,tabWidget2);
  pvAddTab(p,tabWidget2,tab2,"QwtWidgets");

  pvQwtCounter(p,wtCounter1,tab2);
  pvSetGeometry(p,wtCounter1,409,50,161,51);
  qwtCounterSetValue(p,wtCounter1,2);

  pvQwtWheel(p,wtWheel1,tab2);
  pvSetGeometry(p,wtWheel1,589,41,200,60);

  pvQwtCompass(p,wtCompass1,tab2);
  pvSetGeometry(p,wtCompass1,229,190,161,161);

  pvQwtDial(p,wtDial1,tab2);
  pvSetGeometry(p,wtDial1,429,190,161,161);

  pvQwtThermo(p,wtThermo1,tab2);
  pvSetGeometry(p,wtThermo1,150,10,60,160);

  pvQwtScale(p,wtScale1,tab2,0);
  pvSetGeometry(p,wtScale1,9,10,91,151);
//qwtScaleSetPosition(p,wtScale1,ScaleLeft);
  qwtScaleSetPosition(p,wtScale1,ScaleBottom);

  pvQwtKnob(p,wtKnob1,tab2);
  pvSetGeometry(p,wtKnob1,239,10,151,161);
  //qwtKnobSetScale(p,wtKnob1,0,15,2,0);

  pvQwtSlider(p,wtSlider1,tab2);
  pvSetGeometry(p,wtSlider1,0,190,200,50);
  qwtSliderSetScalePos(p,wtSlider1,SliderLeft);
  qwtSliderSetValue(p,wtSlider1,3);

  pvQWidget(p,tab3,tabWidget2);
  pvAddTab(p,tabWidget2,tab3,"Pixmap");

  pvQDateEdit(p,dateEdit1,tab3);
  pvSetGeometry(p,dateEdit1,299,10,161,51);

  pvQTimeEdit(p,timeEdit1,tab3);
  pvSetGeometry(p,timeEdit1,300,70,161,41);

  pvQDateTimeEdit(p,dateTimeEdit1,tab3);
  pvSetGeometry(p,dateTimeEdit1,299,120,210,51);

  pvDownloadFile(p,"ajpg.jpg");
  pvQImage(p,frame3,tab3,"ajpg.jpg",&w,&h,&depth);
  pvSetGeometry(p,frame3,10,10,256,256);

  pvQWidget(p,tab4,tabWidget2);
  pvAddTab(p,tabWidget2,tab4,"SVG");

  pvQDraw(p,drawSVG,tab4);
  pvSetGeometry(p,drawSVG,10,10,750,471);

  pvQWidget(p,Tab5,tabWidget2);
  pvAddTab(p,tabWidget2,Tab5,"Animated MNG");

  pvQLabel(p,MNGlabel,Tab5);
  pvSetGeometry(p,MNGlabel,28,25,531,401);
  pvSetText(p,MNGlabel,"textLabel1");

  pvQLabel(p,textLabel1_2,Tab5);
  pvSetGeometry(p,textLabel1_2,600,70,131,31);
  pvSetText(p,textLabel1_2,"center");
  pvSetAlignment(p,textLabel1_2,AlignCenter);

  pvQLabel(p,textLabel1_3,Tab5);
  pvSetGeometry(p,textLabel1_3,600,120,131,31);
  pvSetText(p,textLabel1_3,"right");
  pvSetAlignment(p,textLabel1_3,AlignVCenter|AlignRight);

  pvQLineEdit(p,lineEdit1,Tab5);
  pvSetGeometry(p,lineEdit1,588,185,151,41);
  pvSetText(p,lineEdit1,"");
  pvSetAlignment(p,lineEdit1,AlignRight);

  pvQLabel(p,textLabel1,Tab5);
  pvSetGeometry(p,textLabel1,598,16,140,40);
  pvSetText(p,textLabel1,"left");
  pvSetAlignment(p,textLabel1,AlignVCenter|AlignLeft);

  pvEndDefinition(p);
  return 0;
}

// _end_of_generated_area_ (do not edit -> use ui2pvc) ---------------------

static int defineMask(PARAM *p)
{
  int i;                    // x    i
  float x;
  double xa[100],ya[100];  // 2PI  100
  static float phi = 0.0f;

  if(p == NULL) return 1;
  generated_defineMask(p);

  phi += 0.1f;
  if(phi > (1000.0f*2.0f*PI)) phi = 0.0f;

  for(i=0; i<100; i++)
  {
    x = (((float) i) * 2.0f * PI) / 100.0f;
    xa[i] = x;
    ya[i] = sin(x+phi);
  }

  // outline
  qpwEnableOutline(p,wtPlot1,1);
  qpwSetOutlinePen(p,wtPlot1,GREEN);

  // legend
  qpwSetAutoLegend(p,wtPlot1,1);
  qpwEnableLegend(p,wtPlot1,1);
  qpwSetLegendPos(p,wtPlot1,0);
  qpwSetLegendFrameStyle(p,wtPlot1,Box|Sunken);

  // axes
  qpwSetAxisTitle(p,wtPlot1,xBottom, "Normalized Frequency");
  qpwSetAxisTitle(p,wtPlot1,yLeft, "Amplitude");

  // curves
  qpwInsertCurve(p,wtPlot1,0,"Sinus");
  qpwSetCurvePen(p,wtPlot1,0,YELLOW,3,DashDotLine);
  qpwSetCurveYAxis(p,wtPlot1,0,yLeft);
  qpwSetCurveData(p,wtPlot1,0,100,xa,ya);

  for(i=0; i<100; i++)
  {
    x = (((float) i) * 2.0f * PI) / 100.0f;
    xa[i] = x;
    ya[i] = 2.0 * sin(x+phi);
  }
  qpwInsertCurve(p,wtPlot1,1,"2*Sinus");
  qpwSetCurvePen(p,wtPlot1,1,RED,3,DashDotLine);
  qpwSetCurveYAxis(p,wtPlot1,1,yLeft);
  qpwSetCurveData(p,wtPlot1,1,100,xa,ya);
  //qpwSetCurveSymbol(p,wtPlot1,0,MarkerDiamond,RED,BLUE,10,10);
  qpwReplot(p,wtPlot1);
  //qpwRemoveCurve(p,wtPlot1,1);
  //qpwReplot(p,wtPlot1);

  // Thermo wtThermo1
  qwtThermoSetScale(p, wtThermo1, 0.0f, 50.0f, 10.0f, 0);
  qwtThermoSetFillColor(p, wtThermo1, BLUE);
  qwtThermoSetAlarmColor(p, wtThermo1, RED);
  qwtThermoSetAlarmLevel(p, wtThermo1, 40.0f);
  qwtThermoSetAlarmEnabled(p, wtThermo1, 1);
  qwtThermoSetPipeWidth(p, wtThermo1, 20);
  qwtThermoSetRange(p, wtThermo1, 0.0f, 50.0f);
  qwtThermoSetValue(p, wtThermo1, 45.0f);

  qwtThermoSetOrientation(p, wtThermo1, HORIZONTAL, ThermoTop);

  // Compass wtCompass1
#ifdef unix
  qwtCompassSetSimpleCompassRose(p, wtCompass1, 4, 2);
  qwtCompassSetNeedle(p, wtCompass1, QwtCompassNeedle4, RED, BLUE, YELLOW);
#endif

  qwtDialSetScale(p, wtDial1, 0, 60, 2);

  pvSetDateOrder(p, dateEdit1, MDY);
  pvSetMinDate(p, dateEdit1, 2011, 1,  1);
  pvSetMaxDate(p, dateEdit1, 2011, 12, 31);
  pvSetDate   (p, dateEdit1, 2011, 11, 11);

  pvSetMinTime(p, timeEdit1, 0,  0 );
  pvSetMaxTime(p, timeEdit1, 23, 59);
  pvSetTime   (p, timeEdit1, 11, 11, 11, 11);

  pvSetDate   (p, dateTimeEdit1, 2011, 11, 11);
  pvSetMinDate(p, dateTimeEdit1, 2011, 1,  1);
  pvSetMaxDate(p, dateTimeEdit1, 2011, 12, 31);
  pvSetTime   (p, dateTimeEdit1, 11, 11, 11, 11);
  pvSetMinTime(p, dateTimeEdit1, 0,  0 );
  pvSetMaxTime(p, dateTimeEdit1, 23, 59);

  pvSetBackgroundColor(p,drawSVG, 180, 180, 180);
  pvDownloadFile(p,"test.svg");
  gBeginDraw  (p,drawSVG);
#ifdef USE_DOM  
  QDomDocument svgdoc;
  QFile svgfile("test.svg");
  if(svgfile.open(IO_ReadOnly))
  {
    svgdoc.setContent(&svgfile);
    svgfile.close();
    gSocketPlaySVG(p,svgdoc.toString());
  }
#else  
  gPlaySVG    (p,"test.svg");
#endif  
  gEndDraw    (p);

  pvDownloadFile(p,"pnglogo-grr-anim-lc.mng");
  pvSetStyle(p,MNGlabel,Box,Raised,3,0);   
  pvSetMovie(p,MNGlabel,0,"pnglogo-grr-anim-lc.mng");
  pvMovieControl(p,MNGlabel,-2); // restart
  
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

int show_qwt(PARAM *p)
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
        break;
      case BUTTON_EVENT:
        if(i == pushButtonBack) return 1;
        printf("BUTTON_EVENT id=%d\n",i);
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
        printf("SLIDER_EVENT val=%s\n",text);
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
