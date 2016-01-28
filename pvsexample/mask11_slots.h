//###############################################################
//# mask1_slots.h for ProcessViewServer created: Do Nov 20 07:46:31 2008
//# please fill out these slots
//# here you find all possible events
//# Yours: Lehrig Software Engineering
//###############################################################
// Autocad DWF2OpenGL

// todo: uncomment me if you want to use this data aquisiton
// also uncomment this classes in main.cpp and pvapp.h
// also remember to uncomment rllib in the project file
//extern rlModbusClient     modbus;
//extern rlSiemensTCPClient siemensTCP;
//extern rlPPIClient        ppi;

// constants for OpenGL scene
static const float    scale0 = 3.0f;
static const GLfloat  mat_specular[] = {1.0,1.0,1.0,1.0};
static const GLfloat  mat_shininess[] = {50.0};
static const GLfloat  light_position[] = {1.0,1.0,1.0,1.0};
static const GLfloat  white_light[] = {1.0,1.0,1.0,1.0};

// OpenGL variables
typedef struct
{
  GLdouble frustSize;
  GLdouble frustNear;
  GLdouble frustFar;
  GLfloat  scale;
  GLfloat  xRot;
  GLfloat  yRot;
  GLfloat  zRot;
  GLuint   listarray[100];
  int      num_listarray;
  GLfloat  pos;
  GLfloat  posAll;
  GLfloat  posVertAll;
  GLfloat  posAllOld;
  GLfloat  posVertAllOld;
  GLfloat  X0, Y0;
  int      height, width;
  int      mouseFirstPressed;
  glFont   proportional, fixed;
}GL;

typedef struct // (todo: define your data structure here)
{
  GL gl;
}
DATA;

int initializeGL(PARAM *p)
{
  if(p == NULL) return -1;
  glClearColor(0.9,0.9,0.9,0.0);      // Let OpenGL clear color
  glEnable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glEnable(GL_TEXTURE_2D);
  return 0;
}

int resizeGL(PARAM *p, int width, int height)
{
  if(p == NULL) return -1;
  glViewport(0, 0, (GLint) width, (GLint) height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  return 0;
}

static int paintGL(PARAM *p, DATA *d)
{
  if(p == NULL || d == NULL) return -1;
  pvGlBegin(p,OpenGL1);

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glFrustum(-d->gl.frustSize, d->gl.frustSize, -d->gl.frustSize, d->gl.frustSize, d->gl.frustNear, d->gl.frustFar);
  glTranslatef( 0.0, 0.0, -3.5f );
  double aspect =  (double) d->gl.width / (double) d->gl.height;
  glScalef( d->gl.scale, d->gl.scale*aspect, d->gl.scale );
  glTranslatef( d->gl.posAll, d->gl.posVertAll, 0.0 );
  for(int i=1; i< d->gl.num_listarray; i++) glCallList(d->gl.listarray[i]);
  glTranslatef( d->gl.pos, 0.0, 0.0 );
  glCallList(d->gl.listarray[0]);
  pvGlEnd(p);
  pvGlUpdate(p,OpenGL1);
  return 0;
}

static int slotInit(PARAM *p, DATA *d)
{
  if(p == NULL || d == NULL) return -1;
  printf("slotInit 1\n");
  // init DATA d
  memset(d,0,sizeof(DATA));
  d->gl.frustSize = 0.5;
  d->gl.frustNear = scale0;
  d->gl.frustFar  = 200.0;
  d->gl.scale = 1.0f;
  d->gl.xRot  = 0.0f;
  d->gl.yRot  = 0.0f;
  d->gl.zRot  = 0.0f;
  d->gl.num_listarray = 0;
  d->gl.pos           = 0.0f;
  d->gl.posAll        = 0.0f;
  d->gl.posVertAll    = 0.0f;
  d->gl.posAllOld     = 0.0f;
  d->gl.posVertAllOld = 0.0f;
  d->gl.X0 = d->gl.Y0 = 0.0f;
  d->gl.height = d->gl.width = 1;
  d->gl.mouseFirstPressed = 0;  

  // set sliders
  printf("slotInit 2\n");
  pvSetValue(p,sliderPos,50);
  printf("slotInit 2.1\n");

  // load OpenGL graphic
  printf("slotInit 3\n");
  pvGlBegin(p,OpenGL1);
  printf("slotInit 3.1\n");
  d->gl.proportional.read("gl/proportional.glfont"); // load proportional font
  printf("slotInit 3.2\n");
  d->gl.fixed.read("gl/fixed.glfont");               // load fixed font
  printf("slotInit 3.3\n");
  d->gl.proportional.setZoom(0.9f);
  printf("slotInit 3.4\n");
  d->gl.fixed.setZoom(0.9f);
  printf("slotInit 3.5\n");
  d->gl.num_listarray = pvSendOpenGL(p,"gl/scene.gl.cpp",d->gl.listarray,100,&d->gl.proportional,&d->gl.fixed);
  printf("slotInit 3.6\n");
  pvGlEnd(p);

  printf("slotInit 4\n");
  paintGL(p,d);

  // Download Graphics
  printf("slotInit 5\n");
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

  // set images 4 buttons
  pvSetPixmap(p,btBack,"back.png");

  printf("slotInit end\n");
  return 0;
}

static int slotNullEvent(PARAM *p, DATA *d)
{
  if(p == NULL || d == NULL) return -1;
  //paintGL(p,d);
  return 0;
}

static int slotButtonEvent(PARAM *p, int id, DATA *d)
{
  if(p == NULL || id == 0 || d == NULL) return -1;

  if(id == btBack)
  {
    return WELLCOME; // call mask 1
  }
  if(id == btCenter)
  {
    d->gl.posAll = 0.0f;
    d->gl.posVertAll = 0.0f;
    paintGL(p,d);
    pvSetImage(p,id,"1center2.png");
  }
  if(id == btLeft)
  {
    d->gl.posAll -= 0.1f;
    paintGL(p,d);
    pvSetImage(p,id,"1leftarrow2.png");
  }
  if(id == btRight)
  {
    d->gl.posAll += 0.1f;
    paintGL(p,d);
    pvSetImage(p,id,"1rightarrow2.png");
  }
  if(id == btUp)
  {
    d->gl.posVertAll -= 0.1f;
    paintGL(p,d);
    pvSetImage(p,id,"1uparrow2.png");
  }
  if(id == btDown)
  {
    d->gl.posVertAll += 0.1f;
    paintGL(p,d);
    pvSetImage(p,id,"1downarrow2.png");
  }

  return 0;
}

static int slotButtonPressedEvent(PARAM *p, int id, DATA *d)
{
  if(p == NULL || id == 0 || d == NULL) return -1;
  if(id == OpenGL1) d->gl.mouseFirstPressed = 1;  

  if(id == btCenter)
  {
    pvSetImage(p,id,"1center3.png");
  }
  if(id == btLeft)
  {
    pvSetImage(p,id,"1leftarrow3.png");
  }
  if(id == btRight)
  {
    pvSetImage(p,id,"1rightarrow3.png");
  }
  if(id == btUp)
  {
    pvSetImage(p,id,"1uparrow3.png");
  }
  if(id == btDown)
  {
    pvSetImage(p,id,"1downarrow3.png");
  }
  return 0;
}

static int slotButtonReleasedEvent(PARAM *p, int id, DATA *d)
{
  if(p == NULL || id == 0 || d == NULL) return -1;
  if(id == OpenGL1)
  {
    d->gl.mouseFirstPressed = 0;  
    d->gl.posAllOld     = d->gl.posAll;
    d->gl.posVertAllOld = d->gl.posVertAll;
  }

  if(id == btCenter)
  {
    pvSetImage(p,id,"1center.png");
  }
  if(id == btLeft)
  {
    pvSetImage(p,id,"1leftarrow.png");
  }
  if(id == btRight)
  {
    pvSetImage(p,id,"1rightarrow.png");
  }
  if(id == btUp)
  {
    pvSetImage(p,id,"1uparrow.png");
  }
  if(id == btDown)
  {
    pvSetImage(p,id,"1downarrow.png");
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

  if(id == sliderPos)
  {
    d->gl.pos = val/100.0f - 0.5f;
    paintGL(p,d);
  }
  if(id == sliderScale)
  {
    d->gl.scale = 0.5f + 8.0f*(val/100.0f);
    paintGL(p,d);
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
  //initializeGL(p);
  return 0;
}

static int slotGlPaintEvent(PARAM *p, int id, DATA *d)
{
  if(p == NULL || id == 0 || d == NULL) return -1;
  if(id == OpenGL1) paintGL(p,d);
  return 0;
}

static int slotGlResizeEvent(PARAM *p, int id, DATA *d, int width, int height)
{
  if(p == NULL || id == 0 || d == NULL || width < 0 || height < 0) return -1;
  if(id == OpenGL1)
  {
    d->gl.width  = width;
    d->gl.height = height;
    pvGlBegin(p,id);
    resizeGL(p,width,height);
    pvGlEnd(p);
    //pvGlUpdate(p,id);
    paintGL(p,d);
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
  if(id == OpenGL1)
  {
    if(d->gl.mouseFirstPressed == 1)
    {
      d->gl.mouseFirstPressed = 0;
      d->gl.X0 = x;
      d->gl.Y0 = y;
    }
    d->gl.posAll     = d->gl.posAllOld     + ((x - d->gl.X0)/1000.0f);
    d->gl.posVertAll = d->gl.posVertAllOld - ((y - d->gl.Y0)/1000.0f);
    paintGL(p,d);
  }
  return 0;
}

static int slotMousePressedEvent(PARAM *p, int id, DATA *d, float x, float y)
{
  if(p == NULL || id == 0 || d == NULL || x < -1000 || y < -1000) return -1;
  if(id == OpenGL1)
  {
    d->gl.X0 = x;
    d->gl.Y0 = y;
  }
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

  if(id == OpenGL1)
  {
    if(enter) pvSetMouseShape(p, OpenHandCursor);
    else      pvSetMouseShape(p, ArrowCursor);
  }
  return 0;
}

static int slotUserEvent(PARAM *p, int id, DATA *d, const char *text)
{
  if(p == NULL || id == 0 || d == NULL || text == NULL) return -1;
  return 0;
}
