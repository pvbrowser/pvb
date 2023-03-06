// Example SVG

/*********************************************************
/                                                        /
/  01.03.2007 by S. Lehrig                               /
/                                                        /
*********************************************************/

typedef struct // (todo: define your data structure here)
{
  rlSvgAnimator svgAnimator;
  int origWidth, origHeight, width, height, svgWidth, svgHeight, moveValue, zoomValue;
  float svgScale, svgX0, svgY0, svgWindowWidth, svgWindowHeight, mouseX0, mouseY0, mouseX1, mouseY1;
  bool naviHidden, popupOn, handOn, zoomOn, mousePressed, zoomActive, specialMouseCursor, popup;
  POPUP_DATA *popup_data;
}
DATA;

extern SIMULATION_DATA simulation_data;

//--------------------------------------------------------------------------------------------------
// User functions start

static int drawSVG1(PARAM *p, int id, DATA *d)
{
  if(d == NULL) return -1;
  if(d->svgAnimator.isModified == 0) return 0;
  gBeginDraw(p,id);
  d->svgAnimator.writeSocket();
  gEndDraw(p);
  return 0;
}

static int zoomCenter(PARAM *p, DATA *d, float newScale)
{
  if(d == NULL || p == NULL) return -1;

  float oldX0, oldY0;

  if (newScale > 1000) newScale = 1000.0f;

  oldX0 = (d->svgX0 - ((d->svgWindowWidth) * (1.0f - d->svgScale)) / 2.0f) / d->svgScale;
  oldY0 = (d->svgY0 - ((d->svgWindowHeight) * (1.0f - d->svgScale)) / 2.0f) / d->svgScale;

  d->svgScale  = newScale;

  d->svgX0 = oldX0*d->svgScale + ((d->svgWindowWidth) * (1.0f - d->svgScale)) / 2.0f;
  d->svgY0 = oldY0*d->svgScale + ((d->svgWindowHeight) * (1.0f - d->svgScale)) / 2.0f;

  return 0;
}

static int zoomRect(PARAM *p, DATA *d)
{
  float newScale, scale1, scale2, rectWidth, rectHeight;

  rectWidth  = d->mouseX1-d->mouseX0;
  rectHeight = d->mouseY1-d->mouseY0;
  d->svgX0   = d->svgX0 + d->svgWindowWidth/2  - (d->mouseX0 + rectWidth/2);
  d->svgY0   = d->svgY0 + d->svgWindowHeight/2 - (d->mouseY0 + rectHeight/2);
  rectWidth  = rectWidth  / d->svgScale;
  rectHeight = rectHeight / d->svgScale;
  scale1     = d->svgWindowWidth  / rectWidth;
  scale2     = d->svgWindowHeight / rectHeight;

  if (scale1 < scale2) newScale = scale1;
  else                 newScale = scale2;
  zoomCenter(p, d, newScale);

  return 0;
}

static int setMouseProperties(PARAM *p, DATA *d)
{
  if(d == NULL || p == NULL) return -1;

  if (d->specialMouseCursor)
  {
    if      (d->handOn)
    {
      if   (d->mousePressed) pvSetMouseShape(p, ClosedHandCursor);
      else                   pvSetMouseShape(p, OpenHandCursor);
      pvSetSelector(p, svgMain, 0);
    }
    else if (d->zoomOn)
    {
      pvSetMouseShape(p, CrossCursor);
      pvSetSelector(p, svgMain, 1);
    }
    else if (d->popupOn)
    {
      pvSetMouseShape(p, CrossCursor);
      pvSetSelector(p, svgMain, 1);
    }
  }
  else
  {
    pvSetMouseShape(p, ArrowCursor);
    pvSetSelector(p, svgMain, 0);
  }

  return 0;
}

// User functions end
//--------------------------------------------------------------------------------------------------

static int slotInit(PARAM *p, DATA *d)
{
  if(p == NULL || d == NULL) return -1;

  pvPrintf(p,ID_TAB,"SVG2");
  // start values
  d->origWidth          = d->width  = 640;
  d->origHeight         = d->height = 480;
  d->svgX0              = 0.0f;
  d->svgY0              = 0.0f;
  d->svgWidth           = 210;
  d->svgHeight          = 297;
  d->svgScale           = 1.0f;
  d->svgWindowWidth     = 640.0f;
  d->svgWindowHeight    = 480.0f;
  d->naviHidden         = false;
  d->popupOn            = false;
  d->handOn             = true;
  d->zoomOn             = false;
  d->specialMouseCursor = false;
  d->mousePressed       = false;
  d->mouseX0            = 0.0f;
  d->mouseY0            = 0.0f;
  d->mouseX1            = 0.0f;
  d->mouseY1            = 0.0f;
  d->moveValue          = 1;
  d->zoomActive         = false;
  d->zoomValue          = 50;
  d->popup_data         = (POPUP_DATA*) p->user;

  // load SVG
  d->svgAnimator.setSocket(&p->s);
  d->svgAnimator.setId(svgMain);
  d->svgAnimator.read("Example.svg");

  // set special popup properties
  if (d->popup_data->popup)
  {
    //d->origWidth  = d->width  = (int) (d->popup_data->x1 - d->popup_data->x0);
    //d->origHeight = d->height = (int) (d->popup_data->y1 - d->popup_data->y0);
    d->mouseX0    = d->popup_data->x0;
    d->mouseY0    = d->popup_data->y0;
    d->mouseX1    = d->popup_data->x1;
    d->mouseY1    = d->popup_data->y1;
    d->svgX0      = d->popup_data->svgx0;
    d->svgY0      = d->popup_data->svgy0;
    d->svgScale   = d->popup_data->scale;
    
    //pvResize(p, ID_MAINWINDOW, d->origWidth, d->origHeight);
    pvHide(p, ID_EDITBAR);
    pvHide(p, ID_TOOLBAR);
    pvHide(p, ID_STATUSBAR);
    pvHide(p, btBack);
    
    zoomRect(p, d);
    d->svgAnimator.setMatrix("main", d->svgScale, 0.0f, d->svgX0, d->svgY0, 0.0f, 0.0f);    
  }

  // svg-field stack to window
  pvSetZoomX(p, svgMain, -1.0f);
  pvSetZoomY(p, svgMain, -1.0f);

  // set mouse-properties
  setMouseProperties(p, d);

  // hide show button 4 navi
  pvHide(p, btShow); 

  // Download Graphics
  pvDownloadFile(p,"1uparrow2.png");
  pvDownloadFile(p,"1uparrow3.png");
  pvDownloadFile(p,"1leftarrow2.png");
  pvDownloadFile(p,"1leftarrow3.png");
  pvDownloadFile(p,"1rightarrow2.png");
  pvDownloadFile(p,"1rightarrow3.png");
  pvDownloadFile(p,"1downarrow2.png");
  pvDownloadFile(p,"1downarrow3.png");
  pvDownloadFile(p,"1center2.png");
  pvDownloadFile(p,"1center3.png");
  pvDownloadFile(p,"hand.png");
  pvDownloadFile(p,"hand2.png");
  pvDownloadFile(p,"hand3.png");
  pvDownloadFile(p,"zoom.png");
  pvDownloadFile(p,"zoom2.png");
  pvDownloadFile(p,"zoom3.png"); 
  pvDownloadFile(p,"popup.png");
  pvDownloadFile(p,"popup2.png"); 
  pvDownloadFile(p,"popup3.png");

  // set images 4 buttons
  pvSetPixmap(p,btBack,"back.png");
  pvSetImage(p,btHand,"hand3.png");
  pvSetImage(p,btZoom,"zoom.png");
  pvSetImage(p,btPopup,"popup.png");

  //d->svgAnimator.setMatrix(const char *object, float sx, float alpha, float x0, float y0, float cx, float cy)
  //d->svgAnimator.setMatrix("pv.lifter", 0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

  // draw svg
  drawSVG1(p,svgMain,d);

  return 0;
}

static int slotNullEvent(PARAM *p, DATA *d)
{
  if(p == NULL || d == NULL) return -1;
 
  d->zoomActive = true;
  if (d->zoomActive && d->zoomValue != 50)
  {
    float newScale;

    newScale = d->svgScale * (1.0f + (d->zoomValue - 50.0f)/200.0f);
    zoomCenter(p, d, newScale);

    d->svgAnimator.setMatrix("main", d->svgScale, 0.0f, d->svgX0, d->svgY0, 0.0f, 0.0f);
    drawSVG1(p,svgMain,d);
  }

  return 0;
}

static int slotButtonEvent(PARAM *p, int id, DATA *d)
{
  if(p == NULL || id == 0 || d == NULL) return -1;

  if(id == btBack)
  {
    return WELLCOME; // call mask 1
  }

  if(id == btHide)
  {
    // hide navi
    d->naviHidden = !(d->naviHidden);
    
    if (d->naviHidden)
    {
      pvHide(p, gbNavi);
      pvShow(p, btShow);
    }
  }

  if(id == btShow)
  {
    // show navi
    d->naviHidden = !(d->naviHidden);
    
    if (!(d->naviHidden))
    {
      pvHide(p, btShow);
      pvShow(p, gbNavi);
    }
  }

 if(id == btUp)
  {
    //d->svgAnimator.setMatrix(const char *object, float sx, float alpha, float x0, float y0, float cx, float cy)    
    d->svgY0 = d->svgY0 + (float) d->moveValue;
    d->svgAnimator.setMatrix("main", d->svgScale, 0.0f, d->svgX0, d->svgY0, 0.0f, 0.0f);
    drawSVG1(p,svgMain,d);
    pvSetImage(p,id,"1uparrow2.png");
  }
  
  if(id == btLeft)
  {
    //d->svgAnimator.setMatrix(const char *object, float sx, float alpha, float x0, float y0, float cx, float cy)
    d->svgX0 = d->svgX0 + (float) d->moveValue;
    d->svgAnimator.setMatrix("main", d->svgScale, 0.0f, d->svgX0, d->svgY0, 0.0f, 0.0f);
    drawSVG1(p,svgMain,d);
    pvSetImage(p,id,"1leftarrow2.png");
  }

  if(id == btRight)
  {
    //d->svgAnimator.setMatrix(const char *object, float sx, float alpha, float x0, float y0, float cx, float cy)
    d->svgX0 = d->svgX0 - (float) d->moveValue;
    d->svgAnimator.setMatrix("main", d->svgScale, 0.0f, d->svgX0, d->svgY0, 0.0f, 0.0f);
    drawSVG1(p,svgMain,d);
    pvSetImage(p,id,"1rightarrow2.png");
  }

  if(id == btDown)
  {
    //d->svgAnimator.setMatrix(const char *object, float sx, float alpha, float x0, float y0, float cx, float cy)
    d->svgY0 = d->svgY0 - (float) d->moveValue;
    d->svgAnimator.setMatrix("main", d->svgScale, 0.0f, d->svgX0, d->svgY0, 0.0f, 0.0f);
    drawSVG1(p,svgMain,d);
    pvSetImage(p,id,"1downarrow2.png");
  }
  
  if(id == btCenter)
  {
    d->svgX0    = 0.0f;
    d->svgY0    = 0.0f;
    d->svgScale = 1.0f;
    d->svgAnimator.setMatrix("main", d->svgScale, 0.0f, d->svgX0, d->svgY0, 0.0f, 0.0f);
    drawSVG1(p,svgMain,d);
    pvSetImage(p,id,"1center2.png");
  }
  
  if(id == btHand && !d->handOn)
  {
    d->handOn  = true;
    d->zoomOn  = false;
    d->popupOn = false;
    pvSetImage(p,btHand,"hand3.png");
    pvSetImage(p,btZoom,"zoom.png");
    pvSetImage(p,btPopup,"popup.png");
  }

  if(id == btZoom && !d->zoomOn)
  {
    d->handOn  = false;
    d->zoomOn  = true;
    d->popupOn = false;
    pvSetImage(p,btHand,"hand.png");
    pvSetImage(p,btZoom,"zoom3.png");
    pvSetImage(p,btPopup,"popup.png");
  }
  
  if(id == btPopup && !d->popupOn)
  {
    d->handOn  = false;
    d->zoomOn  = false;
    d->popupOn = true;
    pvSetImage(p,btHand,"hand.png");
    pvSetImage(p,btZoom,"zoom.png");
    pvSetImage(p,btPopup,"popup3.png");
  }
  else if(id == btAnimate)
  {
    // animate roll1
    // animate lifter
    float cx, cy;
    float scale;

    cx = 312.784f + 85.224f/2.0f;
    cy = 1052.36f - 702.914f - 85.224f/2.0f;
    scale = -162.0f/100.0f;
 
    for(int i=0; i<100; i++)
    {
      // setMatrix(const char *object, float sx, float alpha, float x0, float y0, float cx, float cy)
      d->svgAnimator.setMatrix("pv.roll1", 1.0, (i+1)*(8*PI/100), 0.0, 0.0, cx, cy);
      d->svgAnimator.setMatrix("pv.lifter", 1.0, 0.0, 0.0, scale*i, 0.0, 0.0);
      drawSVG1(p,svgMain,d);
    }
  }

  return 0;
}

static int slotButtonPressedEvent(PARAM *p, int id, DATA *d)
{
  if(p == NULL || id == 0 || d == NULL) return -1;

  if(id == slZoom)
  {
    d->zoomActive = true;
  }
  
//printf("x0: %f\n", d->svgX0);
//printf("y0: %f\n", d->svgY0);
//printf("scale: %f\n", d->svgScale);
  if(id == btUp)
  {
    pvSetImage(p,id,"1uparrow3.png");
  }
  
  if(id == btLeft)
  {
    pvSetImage(p,id,"1leftarrow3.png");
  }
  
  if(id == btRight)
  {
    pvSetImage(p,id,"1rightarrow3.png");
  }
  
  if(id == btDown)
  {
    pvSetImage(p,id,"1downarrow3.png");
  }
  
  if(id == btCenter)
  {
    pvSetImage(p,id,"1center3.png");
  }
  
  if(id == btHand && !d->handOn)
  {
    pvSetImage(p,id,"hand3.png");
  }
  
  if(id == btZoom && !d->zoomOn)
  {
    pvSetImage(p,id,"zoom3.png");
  }
  
  if(id == btPopup && !d->popupOn)
  {
    pvSetImage(p,id,"popup3.png");
  }

  return 0;
}

static int slotButtonReleasedEvent(PARAM *p, int id, DATA *d)
{
  if(p == NULL || id == 0 || d == NULL) return -1;

  if (id == slZoom)
  {
    d->zoomActive = false;
    pvSetValue(p, slZoom, 50);
  }

  if(id == btUp)
  {
    pvSetImage(p,id,"1uparrow.png");
  }
  
  if(id == btLeft)
  {
    pvSetImage(p,id,"1leftarrow.png");
  }
  
  if(id == btRight)
  {
    pvSetImage(p,id,"1rightarrow.png");
  }
  
  if(id == btDown)
  {
    pvSetImage(p,id,"1downarrow.png");
  }
  
  if(id == btCenter)
  {
    pvSetImage(p,id,"1center.png");
  }
  
  if(id == btHand && !d->handOn)
  {
    pvSetImage(p,id,"hand.png");
  }
  
  if(id == btZoom && !d->zoomOn)
  {
    pvSetImage(p,id,"zoom.png");
  }
  
  if(id == btPopup && !d->popupOn)
  {
    pvSetImage(p,id,"popup.png");
  }
  
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

  if(id == slZoom)
  {
    d->zoomValue = val;

    if (d->zoomActive && d->zoomValue != 50)
    {
      float newScale;

      newScale = d->svgScale * (1.0f + (d->zoomValue - 50.0f)/200.0f);
      zoomCenter(p, d, newScale);

      d->svgAnimator.setMatrix("main", d->svgScale, 0.0f, d->svgX0, d->svgY0, 0.0f, 0.0f);
      drawSVG1(p,svgMain,d);
    }
  }

  if(id == slLifter)
  {
    // animate lifter
    float scale;

    // setMatrix(DATA *d, const char *object, float sx, float alpha, float x0, float y0, float cx, float cy)
    scale = -162.0f/100.0f; 
    d->svgAnimator.setMatrix("pv.lifter", 1.0, 0.0, 0.0, scale*val, 0.0, 0.0);
    drawSVG1(p,svgMain,d);
  }

  if(id == sbMoveVal)
  {
    d->moveValue = val*10;
  }
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

  if (id == svgMain)
  {
    d->svgWindowWidth = (float) width;
    d->svgWindowHeight = (float) height;
  }

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

  if (id == svgMain && d->handOn && d->mousePressed &&1==2)
  {
    d->svgX0 = d->svgX0 + (((x*d->svgWindowWidth) / 100.0f) - d->mouseX0);
    d->svgY0 = d->svgY0 + (((y*d->svgWindowHeight) / 100.0f) - d->mouseY0);
    d->mouseX0 = x;
    d->mouseY0 = y;

    d->svgAnimator.setMatrix("main", d->svgScale, 0.0f, d->svgX0, d->svgY0, 0.0f, 0.0f);
    drawSVG1(p,svgMain,d);
  }

  if (id == svgMain && d->popupOn && d->mousePressed)
  {
    d->mouseX1 = x;
    d->mouseY1 = y;
    drawSVG1(p,svgMain,d);
  } 
  return 0;
}

static int slotMousePressedEvent(PARAM *p, int id, DATA *d, float x, float y)
{
  if(p == NULL || id == 0 || d == NULL || x < -1000 || y < -1000) return -1;

  if(id == btUp)
  {
    pvSetImage(p,id,"1uparrow3.png");
  }
  if(id == btLeft)
  {
    pvSetImage(p,id,"1leftarrow3.png");
  }
  if(id == btRight)
  {
    pvSetImage(p,id,"1rightarrow3.png");
  }
  if(id == btDown)
  {
    pvSetImage(p,id,"1downarrow3.png");
  }
  if(id == btCenter)
  {
    pvSetImage(p,id,"1center3.png");
  }

  if (id == svgMain)
  {
    d->mousePressed = true;
    d->mouseX0 = x;
    d->mouseY0 = y;
    setMouseProperties(p, d);
  }
  
  return 0;
}

static int slotMouseReleasedEvent(PARAM *p, int id, DATA *d, float x, float y)
{
  if(p == NULL || id == 0 || d == NULL || x < -1000 || y < -1000) return -1;

  if (id == svgMain && d->handOn && d->mousePressed)
  {
    d->svgX0 = d->svgX0 + (x - d->mouseX0);
    d->svgY0 = d->svgY0 + (y - d->mouseY0);

    d->svgAnimator.setMatrix("main", d->svgScale, 0.0f, d->svgX0, d->svgY0, 0.0f, 0.0f);
    drawSVG1(p,svgMain,d);

    d->mousePressed  = false;
    setMouseProperties(p, d);
  }

  if (id == svgMain && d->zoomOn && d->mousePressed)
  {
    d->mouseX1 = x;
    d->mouseY1 = y;
    zoomRect(p, d);

    d->svgAnimator.setMatrix("main", d->svgScale, 0.0f, d->svgX0, d->svgY0, 0.0f, 0.0f);
    drawSVG1(p,svgMain,d);

    d->mousePressed  = false;
  }

  if (id == svgMain && d->popupOn && d->mousePressed)
  {
    char buff[3500], url[1024], *cptr;

    d->mouseX1 = x;
    d->mouseY1 = y;
   
    printf("popup p->url=%s\n", p->url);
    printf("popup p->initial_mask=%s\n", p->initial_mask);
    if(strlen(p->url) >= sizeof(url)) return 0;
    strcpy(url, p->url);
    cptr = strstr(url,"://");
    if(cptr != NULL)
    {
      cptr += 3;
      cptr = strchr(cptr, '/');
      if(cptr != NULL) *cptr = '\0';
    }
    cptr = strchr(url,'?');
    if(cptr != NULL) *cptr = '\0';
    sprintf(buff, "+%s/mask10?popup=true?x0=%f?y0=%f?x1=%f?y1=%f?scale=%f?svgx0=%f?svgy0=%f", 
      url, d->mouseX0, d->mouseY0, d->mouseX1, d->mouseY1, d->svgScale, d->svgX0, d->svgY0);
    // sprintf(buff, "+pv://localhost:5050/mask10?popup=true?x0=%f?y0=%f?x1=%f?y1=%f?scale=%f?svgx0=%f?svgy0=%f", 
    //   d->mouseX0, d->mouseY0, d->mouseX1, d->mouseY1, d->svgScale, d->svgX0, d->svgY0);
    pvHyperlink(p,buff);

    d->mousePressed  = false;
  }

  return 0;
}

static int slotMouseOverEvent(PARAM *p, int id, DATA *d, int enter)
{
  if(p == NULL || id == 0 || d == NULL || enter < -1000) return -1;
  
  if(id == svgMain)
  {
    if (enter) 
    {
      d->specialMouseCursor = true;
      setMouseProperties(p, d);
    }
    else 
    {
      d->specialMouseCursor = false;
      setMouseProperties(p, d);
    }
  }

  if(id == gbNavi || id == gbExample)
  {
    if (enter) 
    {
      d->specialMouseCursor = false;
      setMouseProperties(p, d);
    }
    else 
    {
      d->specialMouseCursor = true;
      setMouseProperties(p, d);
    }
  }

  if(id == btUp)
  {
    if (enter) pvSetImage(p,id,"1uparrow2.png");
    else       pvSetImage(p,id,"1uparrow.png");
  }
  if(id == btLeft)
  {
    if (enter) pvSetImage(p,id,"1leftarrow2.png");
    else       pvSetImage(p,id,"1leftarrow.png");
  }
  if(id == btRight)
  {
    if (enter) pvSetImage(p,id,"1rightarrow2.png");
    else       pvSetImage(p,id,"1rightarrow.png");
  }
  if(id == btDown)
  {
    if (enter) pvSetImage(p,id,"1downarrow2.png");
    else       pvSetImage(p,id,"1downarrow.png");
  }
  if(id == btCenter)
  {
    if (enter) pvSetImage(p,id,"1center2.png");
    else       pvSetImage(p,id,"1center.png");
  }
  if(id == btHand && !d->handOn)
  {
    if (enter) pvSetImage(p,id,"hand2.png");
    else       pvSetImage(p,id,"hand.png");
  }
  if(id == btZoom && !d->zoomOn)
  {
    if (enter) pvSetImage(p,id,"zoom2.png");
    else       pvSetImage(p,id,"zoom.png");
  }
  if(id == btPopup && !d->popupOn)
  {
    if (enter) pvSetImage(p,id,"popup2.png");
    else       pvSetImage(p,id,"popup.png");
  }

  return 0;
}

static int slotUserEvent(PARAM *p, int id, DATA *d, const char *text)
{
  if(p == NULL || id == 0 || d == NULL || text == NULL) return -1;
  return 0;
}
