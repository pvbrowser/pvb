////////////////////////////////////////////////////////////////////////////
//
// show_periodic for ProcessViewServer created: Tue Mar 9 11:09:00 2002
//
////////////////////////////////////////////////////////////////////////////
#include "pvapp.h"
#include <math.h>
#ifdef unix
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

typedef struct // (todo: define your data structure here)
{
  float x[100];
  float y[100];
  int   n;
  int   tab;
}
DATA;

#define PI 3.141592654f

// _begin_of_generated_area_ (do not edit -> use ui2pvc) -------------------

// our mask contains the following objects
enum {
  ID_MAIN_WIDGET = 0,
  PushButtonBack,
  TabWidget2,
  tab1,
  TextLabel2,
  ListBox1,
  LCDNumber1,
  ProgressBar1,
  SpinBox1,
  Dial1,
  Line1,
  Line2,
  Table1,
  GroupBox1,
  PushButton4,
  PushButton5,
  PushButton6,
  PushButton7,
  pushButtonSave,
  PushButton1,
  PushButton2,
  TextLabel1,
  tab2,
  ButtonGroup1,
  RadioButton3,
  RadioButton2,
  RadioButton1,
  ButtonGroup2,
  CheckBox3,
  CheckBox2,
  CheckBox1,
  PushButtonRequestMultiLineEdit,
  MultiLineEdit1,
  tab3,
  TextLabel3,
  ComboBox1,
  TextLabel4,
  TextLabel5,
  ComboBox2,
  Slider1,
  TextLabel6,
  Slider2,
  PushButtonBackward,
  PushButtonForward,
  TextBrowser1,
  PushButtonHome,
  LineEdit1,
  tab4,
  Draw1,
  TextLabel2_2,
  TextLabel1_2,
  Frame3,
  Image1,
  PixmapLabel1,
  tab5,
  IconView1,
  PushButtonRequestListView,
  ListView1,
  tab6,
  VtkTclWidget1,
  VtkSlider1,
  VtkSlider2,
  TabPage,
  draw2,
  ID_END_OF_WIDGETS
};

  static const char *toolTip[] = {
  "",
  "back",
  "tab",
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
  "button1",
  "button2",
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
  "back what",
  "tab what",
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
  "button1 what",
  "button2 what",
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

static int generated_defineMask(PARAM *p)
{
  int w,h,depth;

  if(p == NULL) return 1;
  w = h = depth = strcmp(toolTip[0],whatsThis[0]);
  if(w==h) depth=0; // fool the compiler
  pvStartDefinition(p,ID_END_OF_WIDGETS);


  pvQPushButton(p,PushButtonBack,0);
  pvSetGeometry(p,PushButtonBack,10,480,111,40);
  pvSetText(p,PushButtonBack,"Back");
  pvToolTip(p,PushButtonBack,"back");

  pvQTabWidget(p,TabWidget2,0);
  pvSetGeometry(p,TabWidget2,10,10,680,460);
  pvToolTip(p,TabWidget2,"tab");

  pvQWidget(p,tab1,TabWidget2);
  pvAddTab(p,TabWidget2,tab1,"Labels + Buttons + More");

  pvQLabel(p,TextLabel2,tab1);
  pvSetGeometry(p,TextLabel2,128,55,101,31);
  pvSetText(p,TextLabel2,"TextLabel2");

  pvQListBox(p,ListBox1,tab1);
  pvSetGeometry(p,ListBox1,400,10,140,140);

  pvQLCDNumber(p,LCDNumber1,tab1,2,Outline,Dec);
  pvSetGeometry(p,LCDNumber1,550,170,111,41);

  pvQProgressBar(p,ProgressBar1,tab1,50);
  pvSetGeometry(p,ProgressBar1,548,265,121,31);

  pvQSpinBox(p,SpinBox1,tab1,3,112,1);
  pvSetGeometry(p,SpinBox1,550,10,111,30);

  pvQDial(p,Dial1,tab1,2,101,13,3);
  pvSetGeometry(p,Dial1,548,45,121,111);

  pvQFrame(p,Line1,tab1,HLine,Plain,3,1);
  pvSetGeometry(p,Line1,548,225,121,20);
  pvSetStyle(p,Line1,HLine,-1,-1,-1);
  pvSetStyle(p,Line1,-1,Plain,-1,-1);
  pvSetStyle(p,Line1,-1,-1,3,-1);
  pvSetStyle(p,Line1,-1,-1,-1,1);

  pvQFrame(p,Line2,tab1,HLine,Plain,3,1);
  pvSetGeometry(p,Line2,548,305,121,20);
  pvSetStyle(p,Line2,HLine,-1,-1,-1);
  pvSetStyle(p,Line2,-1,Plain,-1,-1);
  pvSetStyle(p,Line2,-1,-1,3,-1);
  pvSetStyle(p,Line2,-1,-1,-1,1);

  pvQTable(p,Table1,tab1,11,11);
  pvSetGeometry(p,Table1,8,165,531,261);

  pvQGroupBox(p,GroupBox1,tab1,-1,HORIZONTAL,"GroupBox1");
  pvSetGeometry(p,GroupBox1,238,5,151,150);
  pvSetPaletteForegroundColor(p,GroupBox1,0,0,0);
  pvSetPaletteBackgroundColor(p,GroupBox1,10,21,220);

  pvQPushButton(p,PushButton4,GroupBox1);
  pvSetGeometry(p,PushButton4,10,20,130,30);
  pvSetText(p,PushButton4,"PushButton4");

  pvQPushButton(p,PushButton5,GroupBox1);
  pvSetGeometry(p,PushButton5,10,50,130,30);
  pvSetText(p,PushButton5,"PushButton5");

  pvQPushButton(p,PushButton6,GroupBox1);
  pvSetGeometry(p,PushButton6,10,80,130,31);
  pvSetText(p,PushButton6,"PushButton6");

  pvQPushButton(p,PushButton7,GroupBox1);
  pvSetGeometry(p,PushButton7,10,110,130,30);
  pvSetText(p,PushButton7,"PushButton7");

  pvQPushButton(p,pushButtonSave,tab1);
  pvSetGeometry(p,pushButtonSave,9,128,101,31);
  pvSetText(p,pushButtonSave,"Save Table");

  pvQPushButton(p,PushButton1,tab1);
  pvSetGeometry(p,PushButton1,8,15,101,31);
  pvSetText(p,PushButton1,"Button1");
  pvSetFont(p,PushButton1,"Bookman l",18,0,0,0,0);
  pvToolTip(p,PushButton1,"button1");

  pvQPushButton(p,PushButton2,tab1);
  pvSetGeometry(p,PushButton2,10,50,101,31);
  pvSetText(p,PushButton2,"PushButton2");
  pvSetPaletteForegroundColor(p,PushButton2,255,0,0);
  pvSetPaletteBackgroundColor(p,PushButton2,0,85,255);
  pvToolTip(p,PushButton2,"button2");

  pvQLabel(p,TextLabel1,tab1);
  pvSetGeometry(p,TextLabel1,128,15,101,31);
  pvSetText(p,TextLabel1,"TextLabel1");
  pvSetStyle(p,TextLabel1,PopupPanel,-1,-1,-1);
  pvSetStyle(p,TextLabel1,-1,Raised,-1,-1);

  pvQWidget(p,tab2,TabWidget2);
  pvAddTab(p,TabWidget2,tab2,"Radio + Check + Edit");

  pvQButtonGroup(p,ButtonGroup1,tab2,-1,HORIZONTAL,"ButtonGroup1");
  pvSetGeometry(p,ButtonGroup1,20,20,151,140);

  pvQRadioButton(p,RadioButton3,ButtonGroup1);
  pvSetGeometry(p,RadioButton3,10,100,131,31);
  pvSetText(p,RadioButton3,"RadioButton3");

  pvQRadioButton(p,RadioButton2,ButtonGroup1);
  pvSetGeometry(p,RadioButton2,10,60,131,31);
  pvSetText(p,RadioButton2,"RadioButton2");

  pvQRadioButton(p,RadioButton1,ButtonGroup1);
  pvSetGeometry(p,RadioButton1,10,20,131,31);
  pvSetText(p,RadioButton1,"RadioButton1");
  pvSetChecked(p,RadioButton1,1);

  pvQButtonGroup(p,ButtonGroup2,tab2,-1,HORIZONTAL,"ButtonGroup2");
  pvSetGeometry(p,ButtonGroup2,20,170,150,140);

  pvQCheckBox(p,CheckBox3,ButtonGroup2);
  pvSetGeometry(p,CheckBox3,10,100,121,31);
  pvSetText(p,CheckBox3,"CheckBox3");

  pvQCheckBox(p,CheckBox2,ButtonGroup2);
  pvSetGeometry(p,CheckBox2,10,60,121,31);
  pvSetText(p,CheckBox2,"CheckBox2");

  pvQCheckBox(p,CheckBox1,ButtonGroup2);
  pvSetGeometry(p,CheckBox1,10,20,121,31);
  pvSetText(p,CheckBox1,"CheckBox1");
  pvSetChecked(p,CheckBox1,1);

  pvQPushButton(p,PushButtonRequestMultiLineEdit,tab2);
  pvSetGeometry(p,PushButtonRequestMultiLineEdit,18,315,151,31);
  pvSetText(p,PushButtonRequestMultiLineEdit,"Request MultiLineEdit");

  pvQMultiLineEdit(p,MultiLineEdit1,tab2,0,30);
  pvSetGeometry(p,MultiLineEdit1,180,10,491,411);

  pvQWidget(p,tab3,TabWidget2);
  pvAddTab(p,TabWidget2,tab3,"Edit + Combo + Slider + Browser");

  pvQLabel(p,TextLabel3,tab3);
  pvSetGeometry(p,TextLabel3,168,15,101,31);
  pvSetText(p,TextLabel3,"Edit");

  pvQComboBox(p,ComboBox1,tab3,0,AtTop);
  pvSetGeometry(p,ComboBox1,18,55,141,31);

  pvQLabel(p,TextLabel4,tab3);
  pvSetGeometry(p,TextLabel4,168,55,101,31);
  pvSetText(p,TextLabel4,"Combo");

  pvQLabel(p,TextLabel5,tab3);
  pvSetGeometry(p,TextLabel5,168,95,101,31);
  pvSetText(p,TextLabel5,"Combo editable");

  pvQComboBox(p,ComboBox2,tab3,1,AtTop);
  pvSetGeometry(p,ComboBox2,18,95,141,31);

  pvQSlider(p,Slider1,tab3,0,99,10,0,Horizontal);
  pvSetGeometry(p,Slider1,18,145,141,31);

  pvQLabel(p,TextLabel6,tab3);
  pvSetGeometry(p,TextLabel6,168,145,101,31);
  pvSetText(p,TextLabel6,"Slider");

  pvQSlider(p,Slider2,tab3,0,99,10,0,Vertical);
  pvSetGeometry(p,Slider2,18,185,31,141);

  pvQPushButton(p,PushButtonBackward,tab3);
  pvSetGeometry(p,PushButtonBackward,330,10,50,20);
  pvSetText(p,PushButtonBackward,"<-");

  pvQPushButton(p,PushButtonForward,tab3);
  pvSetGeometry(p,PushButtonForward,380,10,50,20);
  pvSetText(p,PushButtonForward,"->");

  pvQTextBrowser(p,TextBrowser1,tab3);
  pvSetGeometry(p,TextBrowser1,278,36,381,380);

  pvQPushButton(p,PushButtonHome,tab3);
  pvSetGeometry(p,PushButtonHome,280,10,50,20);
  pvSetText(p,PushButtonHome,"Home");

  pvQLineEdit(p,LineEdit1,tab3);
  pvSetGeometry(p,LineEdit1,18,15,141,31);
  pvSetText(p,LineEdit1,"");
  pvSetEchoMode(p,LineEdit1,2);

  pvQWidget(p,tab4,TabWidget2);
  pvAddTab(p,TabWidget2,tab4,"Image + Draw + Frames");

  pvQDraw(p,Draw1,tab4);
  pvSetGeometry(p,Draw1,298,5,371,280);

  pvQLabel(p,TextLabel2_2,tab4);
  pvSetGeometry(p,TextLabel2_2,20,300,300,31);
  pvSetText(p,TextLabel2_2,"a JPG Image (you can also use it as a Webcam)");

  pvQLabel(p,TextLabel1_2,tab4);
  pvSetGeometry(p,TextLabel1_2,390,300,151,31);
  pvSetText(p,TextLabel1_2,"a DrawImage");

  pvQFrame(p,Frame3,tab4,StyledPanel,Raised,5,0);
  pvSetGeometry(p,Frame3,8,5,278,278);
  pvSetStyle(p,Frame3,StyledPanel,-1,-1,-1);
  pvSetStyle(p,Frame3,-1,Raised,-1,-1);
  pvSetStyle(p,Frame3,-1,-1,5,-1);
  pvSetStyle(p,Frame3,-1,-1,-1,0);

  pvDownloadFile(p,"ajpg.jpg");
  pvQImage(p,Image1,Frame3,"ajpg.jpg",&w,&h,&depth);
  pvSetGeometry(p,Image1,10,10,256,256);

  pvQLabel(p,PixmapLabel1,tab4);
  pvSetGeometry(p,PixmapLabel1,610,370,32,32);
  pvSetText(p,PixmapLabel1,"");

  pvQWidget(p,tab5,TabWidget2);
  pvAddTab(p,TabWidget2,tab5,"ListView + IconView");

  pvQIconView(p,IconView1,tab5);
  pvSetGeometry(p,IconView1,338,15,331,401);

  pvQPushButton(p,PushButtonRequestListView,tab5);
  pvSetGeometry(p,PushButtonRequestListView,10,380,161,31);
  pvSetText(p,PushButtonRequestListView,"Request Selection");

  pvQListView(p,ListView1,tab5);
  pvSetGeometry(p,ListView1,8,16,321,360);
  pvSetMultiSelection(p,ListView1,1);

  pvQWidget(p,tab6,TabWidget2);
  pvAddTab(p,TabWidget2,tab6,"VTK");

  pvQVtkTclWidget(p,VtkTclWidget1,tab6);
  pvSetGeometry(p,VtkTclWidget1,8,4,531,421);

  pvQSlider(p,VtkSlider1,tab6,0,99,10,50,Vertical);
  pvSetGeometry(p,VtkSlider1,548,4,31,141);

  pvQSlider(p,VtkSlider2,tab6,0,99,10,50,Horizontal);
  pvSetGeometry(p,VtkSlider2,548,154,121,31);

  pvQWidget(p,TabPage,TabWidget2);
  pvAddTab(p,TabWidget2,TabPage,"SVG");

  pvQDraw(p,draw2,TabPage);
  pvSetGeometry(p,draw2,8,7,581,401);


  pvEndDefinition(p);
  pvTabOrder(p,TabWidget2,PushButton1);
  pvTabOrder(p,PushButton1,PushButton2);
  pvTabOrder(p,PushButton2,pushButtonSave);
  pvTabOrder(p,pushButtonSave,PushButton4);
  pvTabOrder(p,PushButton4,PushButton5);
  pvTabOrder(p,PushButton5,PushButton6);
  pvTabOrder(p,PushButton6,PushButton7);
  pvTabOrder(p,PushButton7,ListBox1);
  pvTabOrder(p,ListBox1,SpinBox1);
  pvTabOrder(p,SpinBox1,Dial1);
  pvTabOrder(p,Dial1,Table1);
  pvTabOrder(p,Table1,PushButtonBack);
  pvTabOrder(p,PushButtonBack,RadioButton1);
  pvTabOrder(p,RadioButton1,CheckBox3);
  pvTabOrder(p,CheckBox3,CheckBox2);
  pvTabOrder(p,CheckBox2,CheckBox1);
  pvTabOrder(p,CheckBox1,PushButtonRequestMultiLineEdit);
  pvTabOrder(p,PushButtonRequestMultiLineEdit,ComboBox1);
  pvTabOrder(p,ComboBox1,ComboBox2);
  pvTabOrder(p,ComboBox2,Slider1);
  pvTabOrder(p,Slider1,Slider2);
  pvTabOrder(p,Slider2,PushButtonBackward);
  pvTabOrder(p,PushButtonBackward,PushButtonForward);
  pvTabOrder(p,PushButtonForward,TextBrowser1);
  pvTabOrder(p,TextBrowser1,PushButtonHome);
  pvTabOrder(p,PushButtonHome,LineEdit1);
  pvTabOrder(p,LineEdit1,IconView1);
  pvTabOrder(p,IconView1,PushButtonRequestListView);
  pvTabOrder(p,PushButtonRequestListView,ListView1);
  pvTabOrder(p,ListView1,VtkSlider1);
  pvTabOrder(p,VtkSlider1,VtkSlider2);
  return 0;
}

// _end_of_generated_area_ (do not edit -> use ui2pvc) ---------------------

static int defineMask(PARAM *p)
{
  float fval;

  generated_defineMask(p);

  pvSetText(p,ComboBox1,"choice1");
  pvSetText(p,ComboBox1,"choice2");
  pvSetText(p,ComboBox1,"choice3");
  pvSetText(p,ComboBox1,"choice4");
  pvSetText(p,ComboBox1,"choice5");
  pvSetText(p,ComboBox1,"choice6");
  pvInsertItem(p,ComboBox1,-1,"ok.bmp","a bmp");

  pvSetText(p,ComboBox2,"choice1");
  pvSetText(p,ComboBox2,"choice2");
  pvSetText(p,ComboBox2,"choice3");
  pvSetText(p,ComboBox2,"choice4");
  pvSetText(p,ComboBox2,"choice5");
  pvSetText(p,ComboBox2,"choice6");

  pvInsertItem(p,ListBox1,-1,"error.bmp","item1");
  pvInsertItem(p,ListBox1,-1,"ok.bmp","item2");
  pvInsertItem(p,ListBox1,-1,NULL,"item3");
  pvInsertItem(p,ListBox1,-1,NULL,"item4");
  pvInsertItem(p,ListBox1,-1,NULL,"item5");
  pvInsertItem(p,ListBox1,-1,NULL,"item6");
  pvInsertItem(p,ListBox1,-1,NULL,"item7");
  pvInsertItem(p,ListBox1,-1,NULL,"item8");
  pvInsertItem(p,ListBox1,-1,NULL,"item9");
  pvInsertItem(p,ListBox1,-1,NULL,"item10");


  pvInsertItem(p,ListBox1,-1,NULL,"item11");
  pvInsertItem(p,ListBox1,-1,NULL,"item12");
  pvInsertItem(p,ListBox1,-1,NULL,"item13");
  pvInsertItem(p,ListBox1,-1,NULL,"item14");
  pvInsertItem(p,ListBox1,-1,NULL,"item15");
  pvSetMultiSelection(p,ListBox1,1);
  pvRemoveItemByName(p,ListBox1,"item14");

  pvSetValue(p,ProgressBar1,20);

  pvDisplayNum(p,LCDNumber1,86);

  pvSetColumnWidth(p,Table1,1,100);
  pvSetColumnWidth(p,Table1,2,100);
  pvSetColumnWidth(p,Table1,3,100);
  pvSetColumnWidth(p,Table1,4,100);
  pvSetColumnWidth(p,Table1,5,100);
  pvSetWordWrap(p,Table1,1);
  fval = 1.0f;
  pvTablePrintf(p,Table1,0,0,"color(255,0,0)%5.2f",fval); fval += 0.5f;
  pvTablePrintf(p,Table1,0,1,"color(0,255,0)%5.2f",fval); fval += 0.5f;
  pvTablePrintf(p,Table1,0,2,"color(0,0,255)%5.2f",fval); fval += 0.5f;
  pvTablePrintf(p,Table1,0,3,"%5.2f",fval); fval += 0.5f;
  pvTablePrintf(p,Table1,0,4,"one two three four five six seven eight nine ten");
  pvSetTablePixmap(p,Table1,1,0,"error.bmp");
  pvSetTablePixmap(p,Table1,1,1,"ok.bmp");
  pvTablePrintf(p,Table1,1,0,"error.bmp");
  pvTablePrintf(p,Table1,1,1,"ok.bmp");
  pvSetTableCheckBox(p,Table1,2,0,1,"CheckBox");
  pvSetTableComboBox(p,Table1,2,1,1,"choice1,choice2,choice3");
  pvTablePrintf(p,Table1,0,-1,"hello");
  pvTablePrintf(p,Table1,-1,0,"world");
  pvSetColumnWidth(p,Table1,-1,40);
  pvTableSetHeaderResizeEnabled(p,Table1,1,0,1);  
  pvTableSetEnabled(p,Table1,0,0,1);

  pvSetPixmap(p,PushButton2,"ok.bmp");

  pvDownloadFile(p,"index.html");

  pvDownloadFile(p,"screenshots.html");
  pvDownloadFile(p,"doc.html");
  pvDownloadFile(p,"download.html");
  pvDownloadFile(p,"license.html");
  pvDownloadFile(p,"sflogo-hammer1.jpg");
  pvSetSource(p,TextBrowser1,"index.html");

#ifdef unix
  if(1) // test big text
  {
    char *buf, line[1024];;
    FILE *fin;
    int fsize;
    struct stat statbuf;
    fin = fopen("download.html","r");
    if(fin == NULL)
    {
      printf("could not open download.html\n");
    }
    else
    {
      lstat("download.html", &statbuf);
      fsize = (int) statbuf.st_size;
      printf("fsize=%d\n", fsize);
      buf = new char[fsize + 1];
      buf[0] = '\0';
      while(fgets(line,sizeof(line)-1,fin) != NULL)
      {
        strcat(buf,line);
      }
      pvSetText(p,TextBrowser1,buf);
      delete [] buf;
      fclose(fin);
    }
  }
#endif

  pvSetMultiSelection(p,ListView1,1);
  pvAddColumn(p,ListView1,"colum1",-1);
  pvAddColumn(p,ListView1,"colum2",-1);
  pvAddColumn(p,ListView1,"colum3",-1);
  pvSetListViewText(p,ListView1,"/0"     ,0,"text0");
  pvSetListViewText(p,ListView1,"/1"     ,0,"text1");
  pvSetListViewText(p,ListView1,"/1/0"   ,0,"subtext0");
  pvSetListViewText(p,ListView1,"/1/1"   ,0,"subtext1");
  pvSetListViewText(p,ListView1,"/1/1/0" ,0,"subsubtext1");
  pvSetListViewText(p,ListView1,"/2"     ,0,"text2");
  pvSetListViewText(p,ListView1,"/3"     ,0,"text3");
  pvSetListViewPixmap(p,ListView1,"/0","ok.bmp",0);
  pvSetListViewPixmap(p,ListView1,"/1","error.bmp",0);
  pvSetListViewText(p,ListView1,"/0"     ,2,"a");
  pvSetListViewText(p,ListView1,"/1"     ,2,"b");
  pvSetListViewText(p,ListView1,"/1/0"   ,2,"c");
  pvSetListViewText(p,ListView1,"/1/1"   ,2,"d");
  pvSetListViewText(p,ListView1,"/1/1/0" ,2,"e");
  pvSetListViewText(p,ListView1,"/2"     ,2,"f");
  pvSetListViewText(p,ListView1,"/3"     ,2,"g");

  pvRemoveListViewItem(p,ListView1,"/3");
  //for(int i=0; i<600; i++)
  //{
  //  char buf[80];
  //  sprintf(buf,"/a%d",i);
  //  pvSetListViewText(p,ListView1,buf,0,buf);
  //}
  pvListViewEnsureVisible(p,ListView1,"/1/1/0");
  //pvListViewSetOpen(p,ListView1,"/1/1",1);
  //pvListViewSetOpen(p,ListView1,"/1",0);
  pvSetSorting(p,ListView1,0,1);
  //pvSetListViewText(p,ListView1,"/hallo/welt"     ,2,"hallo welt");

  pvSetIconViewItem(p,IconView1,"ok.bmp", "ok");
  pvSetIconViewItem(p,IconView1,"error.bmp", "error1");
  pvSetIconViewItem(p,IconView1,"error.bmp", "error2");
  pvRemoveIconViewItem(p,IconView1,"error1");

/*
  pvVtkTclPrintf(p,VtkTclWidget1,"vtkConeSource cone");
  pvVtkTclPrintf(p,VtkTclWidget1,"  cone SetHeight 3.0");
  pvVtkTclPrintf(p,VtkTclWidget1,"  cone SetRadius 1.0");
  pvVtkTclPrintf(p,VtkTclWidget1,"  cone SetResolution 10");
  pvVtkTclPrintf(p,VtkTclWidget1,"vtkPolyDataMapper coneMapper");
  pvVtkTclPrintf(p,VtkTclWidget1,"  coneMapper SetInput [cone GetOutput]");
  pvVtkTclPrintf(p,VtkTclWidget1,"vtkActor coneActor");
  pvVtkTclPrintf(p,VtkTclWidget1,"  coneActor SetMapper coneMapper");
  pvVtkTclPrintf(p,VtkTclWidget1,"renderer AddActor coneActor");
  pvVtkTclPrintf(p,VtkTclWidget1,"  renderer SetBackground 0.2 0.4 0.3");
*/

  pvVtkTclScript(p,VtkTclWidget1,"expCos.tcl");

  // (todo: add your code here)
  return 0;
}

static int drawGraphic(PARAM *p, int id, DATA *d)
{
//int i;
int x,y,w,h,fontsize,oldfontsize;
float xf[2],yf[2];

  xf[0] = 0.0f;

  yf[0] = -1.5f;
  xf[1] = 2.0f*PI;
  yf[1] =  1.5f;

  x = 80;
  y = 50;
  w = 270;
  h = 180;
  fontsize = 10;

  gBeginDraw    (p,id);
  gComment      (p,"This is a comment in the metafile");
  gSetColor     (p,BLACK);
  gSetFont      (p,TIMES,fontsize,Normal,0);
  gBoxWithText  (p,x,y,w,h,fontsize,"phi/rad","Achsenbeschriftung: sin(phi)",NULL);
  gXAxis        (p,0,1.0f,2.0f*PI,1);
  gYAxis        (p,-1.5f,0.5f,1.5f,1);
  //gSetStyle     (p,1);
  gSetStyle     (p,DotLine);
  gXGrid        (p);
  gYGrid        (p);
  //gSetStyle     (p,0);
  gSetStyle     (p,SolidLine);
  gSetColor     (p,RED);
  gLine         (p,d->x,d->y,d->n);
  //gSetStyle     (p,1);
  gSetStyle     (p,DashDotLine);
  gSetColor     (p,BLUE);
  gLine         (p,xf,yf,2);
  gRightYAxis   (p,0.0f,0.5f,2.0f,1);
  gText         (p,x+w/2,y-fontsize*2,"Date: 09-MAR-2002",ALIGN_CENTER);
  oldfontsize = fontsize;
  fontsize = 20;
  gSetFont      (p,TIMES,fontsize,Bold,1);
  gText         (p,x+w/2,y-oldfontsize*4,"This is a Diagram",ALIGN_CENTER);
  gTextInAxis   (p,3.0f,0.0f,"Text at (3.0f,0.0f)",ALIGN_CENTER);
  gEndDraw      (p);
  return 0;
}

static int showData(PARAM *p, DATA *d)
{
  static int val1 = 0;

  if(d->tab == 3) drawGraphic(p, Draw1, d);
  if(d->tab == 4)
  {
    pvListViewPrintf(p,ListView1,"/1/1/0",1,"val1=%d",val1);
    val1++;
    if(val1 > 100) val1 = 0;
  }
  // (todo: add your code here)
  return 0;
}

static int readData(DATA *d) // from shared memory, database or something else
{
  int i;                  // x    i
  float x;                // 2PI  100
  static float phi = 0.0f;

  phi += 0.1f;
  if(phi > (1000.0f*2.0f*PI)) phi = 0.0f;

  for(i=0; i<100; i++)
  {
    x = (((float) i) * 2.0f * PI) / 100.0f;
    d->x[i] = x;
    d->y[i] = (float) sin(x+phi);
  }
  d->n = 100;
  return 0;
}

static int showSvg(PARAM *p)
{
  pvDownloadFile(p,"test.svg");
  gBeginDraw  (p,draw2);
  gPlaySVG    (p,"test.svg");
  gEndDraw    (p);
  return 0;
}

int show_periodic(PARAM *p)
{
  DATA d;
  char event[MAX_EVENT_LENGTH];
  char text[MAX_EVENT_LENGTH];
  char str1[MAX_EVENT_LENGTH];
  int  i,w,h,val,x,y,button;
  int  xangle,yangle;

  xangle = yangle = 0;
  defineMask(p);
  memset(&d,0,sizeof(DATA));
  readData(&d); // from shared memory, database or something else
  showData(p,&d);
  showSvg(p);

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
        if(i == PushButton1   ) pvInputDialog(p,ID_END_OF_WIDGETS+1,"Please\ngive me data:", "wastebasket");
        if(i == PushButton4   ) pvFileDialog (p,ID_END_OF_WIDGETS+2,FileOpenDialog);
        if(i == PushButton5   ) pvFileDialog (p,ID_END_OF_WIDGETS+3,FileSaveDialog);
        if(i == PushButton6   ) pvFileDialog (p,ID_END_OF_WIDGETS+4,FindDirectoryDialog);
        if(i == PushButton7   ) pvMessageBox (p,ID_END_OF_WIDGETS+5,BoxInformation,"this is a MessageBox\nthis is line2",1,2,3);
        if(i == PushButtonHome    ) pvMoveContent(p,TextBrowser1,Home);
        if(i == PushButtonForward ) pvMoveContent(p,TextBrowser1,Backward);
        if(i == PushButtonBackward) pvMoveContent(p,TextBrowser1,Forward);
        if(i == PushButtonRequestListView) pvSelection(p,ListView1);
        if(i == PushButton2)               pvSelection(p,ListBox1);
        if(i == PushButtonRequestMultiLineEdit) pvText(p,MultiLineEdit1);
        if(i == pushButtonSave) pvSave(p,Table1);
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
        if(i == VtkSlider1)
        {
          int delta;
          //pvVtkTclPrintf(p,VtkTclWidget1,"coneActor RotateX %d",val);
          delta = (val-50)*3 - xangle;
          xangle += delta;
          pvVtkTclPrintf(p,VtkTclWidget1,"carpet RotateX %d",delta);
          pvVtkTclPrintf(p,VtkTclWidget1,"renWin Render");
        }
        if(i == VtkSlider2)
        {
          int delta;
          delta = (val-50)*3 - yangle;
          yangle += delta;
          //pvVtkTclPrintf(p,VtkTclWidget1,"coneActor RotateY %d",val);
          pvVtkTclPrintf(p,VtkTclWidget1,"carpet RotateY %d",delta);
          pvVtkTclPrintf(p,VtkTclWidget1,"renWin Render");
        }
        break;
      case CHECKBOX_EVENT:
        printf("CHECKBOX_EVENT id=%d %s\n",i,text);
        pvSetFont(p,MultiLineEdit1,"courier",12,1,1,1,0);
        pvSetFontColor(p,MultiLineEdit1,RED);
        if(i == CheckBox1) pvPrintf(p,MultiLineEdit1,"CheckBox1=%s",text);
        if(i == CheckBox2) pvPrintf(p,MultiLineEdit1,"CheckBox2=%s",text);
        if(i == CheckBox3) pvPrintf(p,MultiLineEdit1,"CheckBox3=%s",text);
        break;
      case RADIOBUTTON_EVENT:
        printf("RADIOBUTTON_EVENT id=%d %s\n",i,text);
        pvSetFont(p,MultiLineEdit1,"arial",10,0,0,0,0);
        pvSetFontColor(p,MultiLineEdit1,GREEN);
        if(i == RadioButton1) pvPrintf(p,MultiLineEdit1,"RadioButton1=%s",text);
        if(i == RadioButton2) pvPrintf(p,MultiLineEdit1,"RadioButton2=%s",text);
        if(i == RadioButton3) pvPrintf(p,MultiLineEdit1,"RadioButton3=%s",text);
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
        d.tab = val;
        printf("TAB_EVENT(%d,page=%d)\n",i,val);
        break;
      case TABLE_TEXT_EVENT:
        printf("%s\n",text);
        sscanf(text,"(%d,%d,",&x,&y);
        pvGetText(text,str1);
        printf("TABLE_TEXT_EVENT(%d,%d,\"%s\")\n",x,y,str1);
        break;
      case TABLE_CLICKED_EVENT:
        printf("%s\n",text);
        sscanf(text,"(%d,%d,%d)",&x,&y,&button);
        printf("TABLE_CLICKED_EVENT(%d,%d,button=%d)\n",x,y,button);
        break;
      case SELECTION_EVENT:
        sscanf(text,"(%d,",&val);
        pvGetText(text,str1);
        printf("SELECTION_EVENT(column=%d,\"%s\")\n",val,str1);
        break;
      case RIGHT_MOUSE_EVENT:
        printf("RIGHT_MOUSE_EVENT id=%d text=%s\n",i,text);
      case CLIPBOARD_EVENT:
        sscanf(text,"(%d,",&val);
        printf("CLIPBOARD_EVENT(id=%d)\n",val);
        printf("clipboard = \n%s\n",p->clipboard);
        break;
      default:
        printf("UNKNOWN_EVENT id=%d %s\n",i,text);
        break;
    }

  }
}

