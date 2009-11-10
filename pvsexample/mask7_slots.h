//###############################################################
//# mask7_slots.h for ProcessViewServer created: Fr Nov 10 16:33:16 2006
//# please fill out these slots
//# here you find all possible events
//# Yours: Lehrig Software Engineering
//###############################################################
//VTK1

// todo: uncomment me if you want to use this data aquisiton
// also uncomment this classes in main.cpp and pvapp.h
// also remember to uncomment rllib in the project file
//extern rlModbusClient     modbus;
//extern rlSiemensTCPClient siemensTCP;
//extern rlPPIClient        ppi;

typedef struct // (todo: define your data structure here)
{
  float phi;
}
DATA;

static int animateData(PARAM *p, int id, DATA *d)
{
  if(p == NULL || id == 0 || d == NULL) return -1;
  char filename[80];
  double val,xval,yval;
  FILE *fout;

  sprintf(filename,"%sdata1.vtk",p->file_prefix);
  fout = fopen(filename,"w");  
  if(fout == NULL)
  {
    printf("could not open %s\n",filename);
    return -1;
  }
  // write measured values ##############################################################################################################
  fprintf(fout,"# vtk DataFile Version 2.0\n");
  fprintf(fout,"2D scalar data\n");
  fprintf(fout,"ASCII\n");
  fprintf(fout,"\n");
  fprintf(fout,"DATASET STRUCTURED_POINTS\n");
  fprintf(fout,"DIMENSIONS    20   20    1\n");
  fprintf(fout,"ORIGIN   -10.000 -10.000   0.000\n");
  fprintf(fout,"SPACING    1.000   1.000   1.000\n");
  fprintf(fout,"\n");
  fprintf(fout,"POINT_DATA     400\n");
  fprintf(fout,"SCALARS scalars float\n");
  fprintf(fout,"LOOKUP_TABLE default\n");
  fprintf(fout," 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0\n");
  fprintf(fout," 0.0 1.5 1.5 1.5 1.5 1.5 1.5 1.5 1.5 0.0 0.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 0.0\n");
  fprintf(fout," 0.0 1.5 3.0 3.0 3.0 3.0 3.0 3.0 1.5 0.0 0.0 1.0 2.0 2.0 2.0 2.0 2.0 2.0 1.0 0.0\n");
  fprintf(fout," 0.0 1.5 3.0 4.5 4.5 4.5 4.5 3.0 1.5 0.0 0.0 1.0 2.0 3.0 3.0 3.0 3.0 2.0 1.0 0.0\n");
  fprintf(fout," 0.0 1.5 3.0 4.5 6.0 6.0 4.5 3.0 1.5 0.0 0.0 1.0 2.0 3.0 4.0 4.0 3.0 2.0 1.0 0.0\n");
  fprintf(fout," 0.0 1.5 3.0 4.5 6.0 6.0 4.5 3.0 1.5 0.0 0.0 1.0 2.0 3.0 4.0 4.0 3.0 2.0 1.0 0.0\n");
  fprintf(fout," 0.0 1.5 3.0 4.5 4.5 4.5 4.5 3.0 1.5 0.0 0.0 1.0 2.0 3.0 3.0 3.0 3.0 2.0 1.0 0.0\n");
  fprintf(fout," 0.0 1.5 3.0 3.0 3.0 3.0 3.0 3.0 1.5 0.0 0.0 1.0 2.0 2.0 2.0 2.0 2.0 2.0 1.0 0.0\n");
  fprintf(fout," 0.0 1.5 1.5 1.5 1.5 1.5 1.5 1.5 1.5 0.0 0.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 0.0\n");
  //fprintf(fout," 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0\n");
  //fprintf(fout," 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0\n");
  //fprintf(fout," 0.0 2.0 2.0 2.0 2.0 2.0 2.0 2.0 2.0 0.0 0.0 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.0\n");
  //fprintf(fout," 0.0 2.0 4.0 4.0 4.0 4.0 4.0 4.0 2.0 0.0 0.0 0.5 1.0 1.0 1.0 1.0 1.0 1.0 0.5 0.0\n");
  //fprintf(fout," 0.0 2.0 4.0 6.0 6.0 6.0 6.0 4.0 2.0 0.0 0.0 0.5 1.0 1.5 1.5 1.5 1.5 1.0 0.5 0.0\n");
  //fprintf(fout," 0.0 2.0 4.0 6.0 8.0 8.0 6.0 4.0 2.0 0.0 0.0 0.5 1.0 1.5 2.0 2.0 1.5 1.0 0.5 0.0\n");
  //fprintf(fout," 0.0 2.0 4.0 6.0 8.0 8.0 6.0 4.0 2.0 0.0 0.0 0.5 1.0 1.5 2.0 2.0 1.5 1.0 0.5 0.0\n");
  //fprintf(fout," 0.0 2.0 4.0 6.0 6.0 6.0 6.0 4.0 2.0 0.0 0.0 0.5 1.0 1.5 1.5 1.5 1.5 1.0 0.5 0.0\n");
  //fprintf(fout," 0.0 2.0 4.0 4.0 4.0 4.0 4.0 4.0 2.0 0.0 0.0 0.5 1.0 1.0 1.0 1.0 1.0 1.0 0.5 0.0\n");
  //fprintf(fout," 0.0 2.0 2.0 2.0 2.0 2.0 2.0 2.0 2.0 0.0 0.0 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.0\n");
  //fprintf(fout," 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0\n");
  for(int y=0; y<10; y++)
  {
    for(int x=0; x<20; x++)
    {
      xval = ((PI*x)/20.0f) - (PI/2.0f) + d->phi;
      yval = ((PI*y)/10.0f) - (PI/2.0f);
      val = 6.0f * sin(xval)*cos(yval);
      fprintf(fout," %8.3f",(float)val);                        
    }
    fprintf(fout,"\n");
  }

  fprintf(fout,"\n");
  fprintf(fout,"\n");
  fprintf(fout,"\n");
  fclose(fout);

  pvDownloadFileAs(p,filename,"data1.vtk");

  d->phi += 0.1f;
  return 0;
}

static int vtkUpdate(PARAM *p)
{
  pvVtkTclPrintf(p,vtk,"reader Modified");
  pvVtkTclPrintf(p,vtk,"reader Update");
  pvVtkTclPrintf(p,vtk,"renWin Render");
  return 0;
}

static int slotInit(PARAM *p, DATA *d)
{
  if(p == NULL || d == NULL) return -1;
  //memset(d,0,sizeof(DATA));
  pvPrintf(p,ID_TAB,"VTK");
  pvSetPixmap(p,back,"back.png");

  pvDownloadFile(p,"index.html");
  pvSetSource(p,textBrowser,"index.html");

  d->phi = 0.0f;
  animateData(p,vtk,d);
  pvVtkTclScript(p,vtk,"surface.tcl");
  return 0;
}

static int slotNullEvent(PARAM *p, DATA *d)
{
  if(p == NULL || d == NULL) return -1;
  animateData(p,vtk,d);
  vtkUpdate(p);
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
