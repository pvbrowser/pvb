//###############################################################
//# mask2_slots.h for ProcessViewServer created: Mi Nov 8 15:40:04 2006
//# please fill out these slots
//# here you find all possible events
//# Yours: Lehrig Software Engineering
//###############################################################
//CHAT1

// todo: uncomment me if you want to use this data aquisiton
// also uncomment this classes in main.cpp and pvapp.h
// also remember to uncomment rllib in the project file
//extern rlModbusClient     modbus;
//extern rlSiemensTCPClient siemensTCP;
//extern rlPPIClient        ppi;

rlEventLogServer eventLogServer(LOGFILE);
extern int num_threads;

typedef struct // (todo: define your data structure here)
{
  rlTime   time;
  int      num, num_threads;
  char     alias[80];
}
DATA;

static int slotInit(PARAM *p, DATA *d)
{
  if(p == NULL || d == NULL) return -1;
  //memset(d,0,sizeof(DATA));
  pvPrintf(p,ID_TAB,"Chat");
  d->num = -1;
  d->num_threads = num_threads;
  strcpy(d->alias,"unknown");
  pvSetText(p,lineEditAlias,"unknown");
  pvSetEditable(p,textEditOutput,0);
  pvPrintf(p,textLabelNumThreads,"Num Users = %d",num_threads);
  pvSetFontColor(p,textEditOutput,0,255,0);
  pvSetPixmap(p,back,"back.png");
  return 0;
}

static int slotNullEvent(PARAM *p, DATA *d)
{
  if(p == NULL || d == NULL) return -1;
  const char *cptr;
  char buf[rlMAX_EVENT];

  if(num_threads != d->num_threads)
  {
    pvPrintf(p,textLabelNumThreads,"Num Users = %d",num_threads);
    d->num_threads = num_threads;
  }

  while(1)
  {
    cptr = eventLogServer.getEvent(buf,&d->num);
    if(cptr == NULL) break;
    if(strncmp(cptr,"alias=",6) == 0 && strstr(cptr,"::") != NULL)
    {
      pvBeep(p);
      if(strncmp(&cptr[6],d->alias,strlen(d->alias)) == 0)
      {
        pvSetFontColor(p,textEditOutput,0,0,255); // our message
      }
      else
      {
        pvSetFontColor(p,textEditOutput,255,0,0);
      }
      strcpy(buf,"From:"); buf[5] = ':';
      pvPrintf(p,textEditOutput,"%s",buf);
      pvSetFontColor(p,textEditOutput,0,255,0); // message from other user
      pvBeep(p);
    }
    else
    {
      pvPrintf(p,textEditOutput,"%s",cptr);
    }
  }
  return 0;
}

static int slotButtonEvent(PARAM *p, int id, DATA *d)
{
  if(p == NULL || id == 0 || d == NULL) return -1;
  if(id == back) return WELLCOME;
  if(id == pushButtonSend)
  {
    pvText(p,textEditInput); // request text
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
  if(id == lineEditAlias)
  {
    if(strlen(text) < (int) (sizeof(d->alias)-1))
    {
      strcpy(d->alias,text);
    }
    else printf("text too long:%s\n",text);
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
  if(id == textEditInput)
  {
    char buf[1024];
    char *cptr,*cptr_start;

    d->time.getLocalTime();
    sprintf(buf,"alias=%s::%s\n",d->alias,d->time.getTimeString());
    eventLogServer.putEvent(buf);
    cptr = p->clipboard;
    while(cptr != NULL)
    {
      cptr_start = cptr;
      cptr = strchr(cptr_start,'\n');
      if(cptr != NULL)
      {
        *cptr = '\0';
        cptr++;
      }
      if(strlen(cptr_start) > (int) (sizeof(buf)-1))
      {
        eventLogServer.putEvent("this line has been tooooo long\n");
      }
      else
      {
        strcpy(buf,cptr_start); strcat(buf,"\n");
        eventLogServer.putEvent(buf);
      }
    }
    pvClear(p,textEditInput);
  }
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
  if(modifier == 1 && val == ESC_KEY) pvText(p,textEditInput); // request text
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
