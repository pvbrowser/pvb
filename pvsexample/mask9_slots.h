//###############################################################
//# mask9_slots.h for ProcessViewServer created: Sa Nov 11 09:35:51 2006
//# please fill out these slots
//# here you find all possible events
//# Yours: Lehrig Software Engineering
//###############################################################
//TREE1

// todo: uncomment me if you want to use this data aquisiton
// also uncomment this classes in main.cpp and pvapp.h
// also remember to uncomment rllib in the project file
//extern rlModbusClient     modbus;
//extern rlSiemensTCPClient siemensTCP;
//extern rlPPIClient        ppi;

typedef struct // (todo: define your data structure here)
{
  int val[100];
}
DATA;

static int createListView(PARAM *p, int id)
{
  int column = 0;
  int topic  = 0;
  int size   = 100;
  pvHide(p,id);
  pvAddColumn(p,id,"tree",size);
  pvAddColumn(p,id,"data",size);
  pvListViewPrintf(p,id,"/common",column,"common");
  pvListViewPrintf(p,id,"/common/topic1",column,"topic%d",topic++);
  pvListViewPrintf(p,id,"/common/topic2",column,"topic%d",topic++);
  pvListViewPrintf(p,id,"/common/topic3",column,"topic%d",topic++);
  pvListViewPrintf(p,id,"/common/topic4",column,"topic%d",topic++);
  pvListViewPrintf(p,id,"/common/topic5",column,"topic%d",topic++);
  pvListViewPrintf(p,id,"/common/topic6",column,"topic%d",topic++);
  topic = 0;
  pvListViewPrintf(p,id,"/temp",column,"temp");
  pvListViewPrintf(p,id,"/temp/topic1",column,"topic%d",topic++);
  pvListViewPrintf(p,id,"/temp/topic2",column,"topic%d",topic++);
  pvListViewPrintf(p,id,"/temp/topic3",column,"topic%d",topic++);
  pvListViewPrintf(p,id,"/temp/topic4",column,"topic%d",topic++);
  pvListViewPrintf(p,id,"/temp/topic5",column,"topic%d",topic++);
  pvListViewPrintf(p,id,"/temp/topic6",column,"topic%d",topic++);

  column = 1;
  topic  = 0;
  pvListViewPrintf(p,id,"/common",column,"data");
  pvListViewPrintf(p,id,"/common/topic1",column,"data%d",topic++);
  pvListViewPrintf(p,id,"/common/topic2",column,"data%d",topic++);
  pvListViewPrintf(p,id,"/common/topic3",column,"data%d",topic++);
  pvListViewPrintf(p,id,"/common/topic4",column,"data%d",topic++);
  pvListViewPrintf(p,id,"/common/topic5",column,"data%d",topic++);
  pvListViewPrintf(p,id,"/common/topic6",column,"data%d",topic++);
  topic = 0;
  pvListViewPrintf(p,id,"/temp",column,"data");
  pvListViewPrintf(p,id,"/temp/topic1",column,"data%d",topic++);
  pvListViewPrintf(p,id,"/temp/topic2",column,"data%d",topic++);
  pvListViewPrintf(p,id,"/temp/topic3",column,"data%d",topic++);
  pvListViewPrintf(p,id,"/temp/topic4",column,"data%d",topic++);
  pvListViewPrintf(p,id,"/temp/topic5",column,"data%d",topic++);
  pvListViewPrintf(p,id,"/temp/topic6",column,"data%d",topic++);

  column = 0;
  pvSetListViewPixmap(p,id,"/common","xchat.png",column);
  pvSetListViewPixmap(p,id,"/temp","xedit.png",column);

  pvListViewEnsureVisible(p,id,"/common/topic1");
  pvListViewEnsureVisible(p,id,"/temp/topic6");

  pvShow(p,id);
  return 0;
}

static int createListBox(PARAM *p, int id)
{
  int index = 0;
  pvInsertItem(p,id,index++,"back.png",           "Back");
  pvInsertItem(p,id,index++,"gnumeric.png",       "Gnumeric");
  pvInsertItem(p,id,index++,"kmplot.png",         "Kmplot");
  pvInsertItem(p,id,index++,"kpl.png",            "Kpl");
  pvInsertItem(p,id,index++,"package_network.png","PackageNetwork");
  pvInsertItem(p,id,index++,"taskbar.png",        "Taskbar");
  pvInsertItem(p,id,index++,"xapp.png",           "Xapp");
  pvInsertItem(p,id,index++,"xchat.png",          "Xchat");
  pvInsertItem(p,id,index++,"xedit.png",          "Xedit");
  return 0;
}

static int slotInit(PARAM *p, DATA *d)
{
  if(p == NULL || d == NULL) return -1;
  memset(d,0,sizeof(DATA));
  pvPrintf(p,ID_TAB,"ListView");
  pvSetPixmap(p,back,"back.png");
  createListView(p,listView1);
  createListBox(p,listBox1);
  for(int i=0; i<100; i++)
  {
    d->val[i] = i;
  }
  return 0;
}

static int slotNullEvent(PARAM *p, DATA *d)
{
  if(p == NULL || d == NULL) return -1;

  int column = 1;
  int topic  = 0;
  int id = listView1;
  pvListViewPrintf(p,id,"/common/topic1",column,"data=%d",d->val[topic++]);
  pvListViewPrintf(p,id,"/common/topic2",column,"data=%d",d->val[topic++]);
  pvListViewPrintf(p,id,"/common/topic3",column,"data=%d",d->val[topic++]);
  pvListViewPrintf(p,id,"/common/topic4",column,"data=%d",d->val[topic++]);
  pvListViewPrintf(p,id,"/common/topic5",column,"data=%d",d->val[topic++]);
  pvListViewPrintf(p,id,"/common/topic6",column,"data=%d",d->val[topic++]);
  topic = 0;
  pvListViewPrintf(p,id,"/temp/topic1",column,"%d",d->val[topic++]);
  pvListViewPrintf(p,id,"/temp/topic2",column,"%d",d->val[topic++]);
  pvListViewPrintf(p,id,"/temp/topic3",column,"%d",d->val[topic++]);
  pvListViewPrintf(p,id,"/temp/topic4",column,"%d",d->val[topic++]);
  pvListViewPrintf(p,id,"/temp/topic5",column,"%d",d->val[topic++]);
  pvListViewPrintf(p,id,"/temp/topic6",column,"%d",d->val[topic++]);
  for(int i=0; i<100; i++)
  {
    d->val[i] += 1;
    if(d->val[i] > 15) d->val[i] = 0;
  }
  return 0;
}

static int slotButtonEvent(PARAM *p, int id, DATA *d)
{
  if(p == NULL || id == 0 || d == NULL) return -1;
  if(id == back) return WELLCOME;
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
