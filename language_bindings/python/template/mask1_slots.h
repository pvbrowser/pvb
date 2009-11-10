//###############################################################
//# mask1_slots.h for ProcessViewServer created: Mo Dez 17 06:49:22 2007
//# please fill out these slots
//# here you find all possible events
//# Yours: Lehrig Software Engineering
//###############################################################

// todo: uncomment me if you want to use this data acquisiton
// also uncomment this classes in main.cpp and pvapp.h
// also remember to uncomment rllib in the project file
//extern rlModbusClient     modbus;
//extern rlSiemensTCPClient siemensTCP;
//extern rlPPIClient        ppi;

typedef struct // (todo: define your data structure here)
{
  char this_python[32];
}
DATA;

static int slotInit(PARAM *p, DATA *d)
{
  if(p == NULL || d == NULL) return -1;
  //memset(d,0,sizeof(DATA));
  strcpy(d->this_python, __FILE__); // __FILE__ = "maskX_slots.h" < 15 characters
  pvPy_SetInstance((pvPyCleanupStruct *) p->app_data, d->this_python);

  PyMethod("slotInit");
  PyBuildValue("(i)", p->s);
  PyReturn
}

static int slotNullEvent(PARAM *p, DATA *d)
{
  if(p == NULL || d == NULL) return -1;
  PyMethod("slotNullEvent");
  PyBuildValue("(i)", p->s);
  PyReturn
}

static int slotButtonEvent(PARAM *p, int id, DATA *d)
{
  if(p == NULL || id == 0 || d == NULL) return -1;
  PyMethod("slotButtonEvent");
  PyBuildValue("(ii)", p->s, id);
  PyReturn
}

static int slotButtonPressedEvent(PARAM *p, int id, DATA *d)
{
  if(p == NULL || id == 0 || d == NULL) return -1;
  PyMethod("slotButtonPressedEvent");
  PyBuildValue("(ii)", p->s, id);
  PyReturn
}

static int slotButtonReleasedEvent(PARAM *p, int id, DATA *d)
{
  if(p == NULL || id == 0 || d == NULL) return -1;
  PyMethod("slotButtonReleasedEvent");
  PyBuildValue("(ii)", p->s, id);
  PyReturn
}

static int slotTextEvent(PARAM *p, int id, DATA *d, const char *text)
{
  if(p == NULL || id == 0 || d == NULL || text == NULL) return -1;
  PyMethod("slotTextEvent");
  PyBuildValue("(ii)", p->s, id);
  PyReturn
}

static int slotSliderEvent(PARAM *p, int id, DATA *d, int val)
{
  if(p == NULL || id == 0 || d == NULL || val < -1000) return -1;
  PyMethod("slotSliderEvent");
  PyBuildValue("(iii)", p->s, id, val);
  PyReturn
}

static int slotCheckboxEvent(PARAM *p, int id, DATA *d, const char *text)
{
  if(p == NULL || id == 0 || d == NULL || text == NULL) return -1;
  PyMethod("slotCheckboxEvent");
  PyBuildValue("(iis)", p->s, id, text);
  PyReturn
}

static int slotRadioButtonEvent(PARAM *p, int id, DATA *d, const char *text)
{
  if(p == NULL || id == 0 || d == NULL || text == NULL) return -1;
  PyMethod("slotRadioButtonEvent");
  PyBuildValue("(iis)", p->s, id, text);
  PyReturn
}

static int slotGlInitializeEvent(PARAM *p, int id, DATA *d)
{
  if(p == NULL || id == 0 || d == NULL) return -1;
  PyMethod("slotGlInitializeEvent");
  PyBuildValue("(ii)", p->s, id);
  PyReturn
}

static int slotGlPaintEvent(PARAM *p, int id, DATA *d)
{
  if(p == NULL || id == 0 || d == NULL) return -1;
  PyMethod("slotGlPaintEvent");
  PyBuildValue("(ii)", p->s, id);
  PyReturn
}

static int slotGlResizeEvent(PARAM *p, int id, DATA *d, int width, int height)
{
  if(p == NULL || id == 0 || d == NULL || width < 0 || height < 0) return -1;
  PyMethod("slotGlResizeEvent");
  PyBuildValue("(iiii)", p->s, id, width, height);
  PyReturn
}

static int slotGlIdleEvent(PARAM *p, int id, DATA *d)
{
  if(p == NULL || id == 0 || d == NULL) return -1;
  PyMethod("slotGlIdleEvent");
  PyBuildValue("(ii)", p->s, id);
  PyReturn
}

static int slotTabEvent(PARAM *p, int id, DATA *d, int val)
{
  if(p == NULL || id == 0 || d == NULL || val < -1000) return -1;
  PyMethod("slotTabEvent");
  PyBuildValue("(iii)", p->s, id, val);
  PyReturn
}

static int slotTableTextEvent(PARAM *p, int id, DATA *d, int x, int y, const char *text)
{
  if(p == NULL || id == 0 || d == NULL || x < -1000 || y < -1000 || text == NULL) return -1;
  PyMethod("slotTableTextEvent");
  PyBuildValue("(iiiis)", p->s, id, x, y, text);
  PyReturn
}

static int slotTableClickedEvent(PARAM *p, int id, DATA *d, int x, int y, int button)
{
  if(p == NULL || id == 0 || d == NULL || x < -1000 || y < -1000 || button < 0) return -1;
  PyMethod("slotTableClickedEvent");
  PyBuildValue("(iiiii)", p->s, id, x, y, button);
  PyReturn
}

static int slotSelectionEvent(PARAM *p, int id, DATA *d, int val, const char *text)
{
  if(p == NULL || id == 0 || d == NULL || val < -1000 || text == NULL) return -1;
  PyMethod("slotSelectionEvent");
  PyBuildValue("(iiis)", p->s, id, val, text);
  PyReturn
}

static int slotClipboardEvent(PARAM *p, int id, DATA *d, int val)
{
  if(p == NULL || id == 0 || d == NULL || val < -1000) return -1;
  PyMethod("slotClipboardEvent");
  PyBuildValue("(iiis)", p->s, id, val, p->clipboard);
  PyReturn
}

static int slotRightMouseEvent(PARAM *p, int id, DATA *d, const char *text)
{
  if(p == NULL || id == 0 || d == NULL || text == NULL) return -1;
  //pvPopupMenu(p,-1,"Menu1,Menu2,,Menu3");
  PyMethod("slotRightMouseEvent");
  PyBuildValue("(iis)", p->s, id, text);
  PyReturn
}

static int slotKeyboardEvent(PARAM *p, int id, DATA *d, int val, int modifier)
{
  if(p == NULL || id == 0 || d == NULL || val < -1000 || modifier < -1000) return -1;
  PyMethod("slotKeyboardEvent");
  PyBuildValue("(iiii)", p->s, id, val, modifier);
  PyReturn
}

static int slotMouseMovedEvent(PARAM *p, int id, DATA *d, float x, float y)
{
  if(p == NULL || id == 0 || d == NULL || x < -1000 || y < -1000) return -1;
  PyMethod("slotMouseMovedEvent");
  PyBuildValue("(iiff)", p->s, id, x, y);
  PyReturn
}

static int slotMousePressedEvent(PARAM *p, int id, DATA *d, float x, float y)
{
  if(p == NULL || id == 0 || d == NULL || x < -1000 || y < -1000) return -1;
  PyMethod("slotMousePressedEvent");
  PyBuildValue("(iiff)", p->s, id, x, y);
  PyReturn
}

static int slotMouseReleasedEvent(PARAM *p, int id, DATA *d, float x, float y)
{
  if(p == NULL || id == 0 || d == NULL || x < -1000 || y < -1000) return -1;
  PyMethod("slotMouseReleasedEvent");
  PyBuildValue("(iiff)", p->s, id, x, y);
  PyReturn
}

static int slotMouseOverEvent(PARAM *p, int id, DATA *d, int enter)
{
  if(p == NULL || id == 0 || d == NULL || enter < -1000) return -1;
  PyMethod("slotMouseOverEvent");
  PyBuildValue("(iii)", p->s, id, enter);
  PyReturn
}

static int slotUserEvent(PARAM *p, int id, DATA *d, const char *text)
{
  if(p == NULL || id == 0 || d == NULL || text == NULL) return -1;
  PyMethod("slotUserEvent");
  PyBuildValue("(iis)", p->s, id, text);
  PyReturn
}
