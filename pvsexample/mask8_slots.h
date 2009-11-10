//###############################################################
//# mask8_slots.h for ProcessViewServer created: Sa Nov 11 07:31:35 2006
//# please fill out these slots
//# here you find all possible events
//# Yours: Lehrig Software Engineering
//###############################################################
//GNUPLOT1

// todo: uncomment me if you want to use this data aquisiton
// also uncomment this classes in main.cpp and pvapp.h
// also remember to uncomment rllib in the project file
//extern rlModbusClient     modbus;
//extern rlSiemensTCPClient siemensTCP;
//extern rlPPIClient        ppi;

typedef struct // (todo: define your data structure here)
{
}
DATA;

static int showGnuplot1(PARAM *p, int id, DATA *d)
{
  FILE *fout;
  char buf[80];
  if(d == NULL) return 1; // you may use d for writing gnuplot.dem

#ifdef _WIN32
  if(1)
  {
    pvPrintf(p,label1,"Gnuplot1 if installed");
    return 0;
  }
#endif

  // write gnuplot file
  sprintf(buf,"%sgnuplot.dem",p->file_prefix); // p->file_prefix makes filenames unique for multiple clients
  fout = fopen(buf,"w");
  if(fout == NULL) return 1;
  fprintf(fout,"# gnuplot test.dem\n");
  fprintf(fout,"set output \"%sgnuplot.png\"\n",p->file_prefix);
  fprintf(fout,"set terminal png\n");
  fprintf(fout,"set xlabel \"x\"\n");
  fprintf(fout,"set ylabel \"y\"\n");
  fprintf(fout,"set key top\n");
  fprintf(fout,"set border 4095\n");
  fprintf(fout,"set xrange [-15:15]\n");
  fprintf(fout,"set yrange [-15:15]\n");
  fprintf(fout,"set zrange [-0.25:1]\n");
  fprintf(fout,"set samples 25\n");
  fprintf(fout,"set isosamples 20\n");
  fprintf(fout,"set title \"Radial sinc function.\"\n");
  fprintf(fout,"splot sin(sqrt(x**2+y**2))/sqrt(x**2+y**2)\n");
  fclose(fout);

  // run gnuplot
  sprintf(buf,"gnuplot %sgnuplot.dem",p->file_prefix);
  rlsystem(buf);

  // send result to pvbrowser
  sprintf(buf,"%sgnuplot.png",p->file_prefix);
  pvDownloadFile(p,buf);
  pvSetImage(p,id,buf);

  // temopary files will be cleaned up at browser exit
  return 0;
}

static int showGnuplot2(PARAM *p, int id, DATA *d)
{
  FILE *fout;
  char buf[80];
  if(d == NULL) return 1; // you may use d for writing gnuplot.dem

#ifdef _WIN32
  if(1)
  {
    pvPrintf(p,label1,"Gnuplot2 if installed");
    return 0;
  }
#endif

  // write gnuplot file
  sprintf(buf,"%sgnuplot.dem",p->file_prefix); // p->file_prefix makes filenames unique for multiple clients
  fout = fopen(buf,"w");
  if(fout == NULL) return 1;
  fprintf(fout,"# gnuplot test.dem\n");
  fprintf(fout,"set output \"%sgnuplot.png\"\n",p->file_prefix);
  fprintf(fout,"set terminal png\n");
  fprintf(fout,"#\n");
  fprintf(fout,"# $Id: pm3dcolors.dem,v 1.2 2003/10/17 15:02:21 mikulik Exp $\n");
  fprintf(fout,"#\n");
  fprintf(fout,"# Test of new color modes for pm3d palettes.\n");
  fprintf(fout,"\n");
  fprintf(fout,"#\n");
  fprintf(fout,"#   Gradient Palettes\n");
  fprintf(fout,"#\n");
  fprintf(fout,"set pm3d; set palette\n");
  fprintf(fout,"set palette color\n");
  fprintf(fout,"set pm3d map\n");
  fprintf(fout,"set cbrange [-10:10]\n");
  fprintf(fout,"set xrange [-10:10]\n");
  fprintf(fout,"set yrange [*:*]\n");
  fprintf(fout,"unset ztics\n");
  fprintf(fout,"unset ytics\n");
  fprintf(fout,"set samples 101\n");
  fprintf(fout,"set isosamples 2\n");
  fprintf(fout,"set xtics 2\n");
  fprintf(fout,"\n");
  fprintf(fout,"set palette model RGB\n");
  fprintf(fout,"\n");
  fprintf(fout,"set palette defined\n");
  fprintf(fout,"set title \"set palette defined\"\n");
  fprintf(fout,"splot x\n");
  fclose(fout);

  // run gnuplot
  sprintf(buf,"gnuplot %sgnuplot.dem",p->file_prefix);
  rlsystem(buf);

  // send result to pvbrowser
  sprintf(buf,"%sgnuplot.png",p->file_prefix);
  pvDownloadFile(p,buf);
  pvSetImage(p,id,buf);

  // temopary files will be cleaned up at browser exit
  return 0;
}

static int slotInit(PARAM *p, DATA *d)
{
  if(p == NULL || d == NULL) return -1;
  //memset(d,0,sizeof(DATA));
  pvPrintf(p,ID_TAB,"Gnuplot");
  pvResize(p,0,1280,1024);
  pvSetPixmap(p,back,"back.png");
  showGnuplot1(p,imageGnuplot1,d);
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
  if(id == buttonPlot1) showGnuplot1(p,imageGnuplot1,d);
  if(id == buttonPlot2) showGnuplot2(p,imageGnuplot1,d);
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
