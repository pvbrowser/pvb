/****************************************************************************
**
** Copyright (C) 2000-2006 Lehrig Software Engineering.
**
** This file is part of the pvbrowser project.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#ifndef PYTHON_H
#define PYTHON_H

static int generateWidgetEnumPython(FILE *fout, QWidget *root)
{
  QString item;
  QWidget *widget;
  int i;

  theroot = root;
  fprintf(fout,"  # our mask contains the following objects\n");
  fprintf(fout,"  ID_MAIN_WIDGET = 0\n");

  // loop over widgets
  for(i=0; i<strlist.size(); i++)
  {
    item = strlist.at(i);
    widget = findChild(item.toUtf8()); // root->findChild<QWidget *>(item);
    if(widget != NULL)
    {
      fprintf(fout,"  %s = %d\n",(const char *) widget->objectName().toUtf8(), i+1);
    }
    else
    {
      printf("WARNING generateWidgetEnumPython:findChild=%s not found\n",(const char *) item.toUtf8());
    }
  }

  fprintf(fout,"  ID_END_OF_WIDGETS = %d\n", i+1);
  fprintf(fout,"\n");
  return 0;
}

static int generateToolTipPython(FILE *fout, QWidget *root)
{
  QString item,qbuf;
  QWidget *widget;

  theroot = root;
  fprintf(fout,"  toolTip = [\n");
  fprintf(fout,"    '',\n");

  // loop over widgets
  for(int i=0; i<strlist.size(); i++)
  {
    item = strlist.at(i);
    widget = findChild(item.toUtf8()); //root->findChild<QWidget *>(item);
    if(widget != NULL)
    {
      qbuf = widget->toolTip();
      fprintf(fout,"    '%s',\n",quote(qbuf));
    }
    else
    {
      printf("WARNING generateToolTipPython:findChild=%s not found\n",(const char *) item.toUtf8());
    }
  }

  fprintf(fout,"    '' ]\n");
  fprintf(fout,"\n");

  return 0;
}

static int generateWhatsThisPython(FILE *fout, QWidget *root)
{
  QString item,qbuf;
  QWidget *widget;

  theroot = root;
  fprintf(fout,"  whatsThis = [\n");
  fprintf(fout,"    '',\n");

  // loop over widgets
  for(int i=0; i<strlist.size(); i++)
  {
    item = strlist.at(i);
    widget = findChild(item.toUtf8()); //root->findChild<QWidget *>(item);
    if(widget != NULL)
    {
      qbuf = widget->whatsThis();
      fprintf(fout,"    '%s',\n",quote(qbuf));
    }
    else
    {
      printf("WARNING generateWhatsThisPython:findChild=%s not found\n",(const char *) item.toUtf8());
    }
  }

  fprintf(fout,"    '' ]\n");
  fprintf(fout,"\n");

  return 0;
}

static int generateWidgetTypePython(FILE *fout, QWidget *root)
{
  QString item,qbuf;
  QWidget *widget;
  char    buf[1024],*cptr;

  theroot = root;
  fprintf(fout,"  widgetType = [\n");
  fprintf(fout,"    '',\n");

  // loop over widgets
  for(int i=0; i<strlist.size(); i++)
  {
    item = strlist.at(i);
    widget = findChild(item.toUtf8()); //root->findChild<QWidget *>(item);
    if(widget != NULL)
    {
      qbuf = widget->statusTip(); // parse statusTip
      strcpy(buf,qbuf.toUtf8());
      cptr = strstr(buf,":");
      if(cptr != NULL) *cptr = '\0';
      if(strncmp(buf,"TQ",2) != 0) strcpy(buf,"0");
      fprintf(fout,"    '%s',\n",buf);
    }
    else
    {
      printf("WARNING generateWidgetTypePython:findChild=%s not found\n",(const char *) item.toUtf8());
    }
  }

  fprintf(fout,"    '' ]\n");
  fprintf(fout,"\n");

  return 0;
}

static int generateGeneratedAreaPython(FILE *fout, QWidget *root, int imask)
{
  strlist.clear();
  iitem = 1;

  fprintf(fout,"### begin of generated area, do not edit ##############\n");
  fprintf(fout,"import pv, rllib\n");
  fprintf(fout,"\n");
  fprintf(fout,"class mask%d:\n", imask);
  fprintf(fout,"\n");
  fprintf(fout,"  p = pv.PARAM()\n");

  getStrList(root);
  generateWidgetEnumPython(fout, root);
  generateToolTipPython(fout, root);
  generateWhatsThisPython(fout, root);
  generateWidgetTypePython(fout,root);

  fprintf(fout,"  ### end of generated area, do not edit ##############\n");
  strlist.clear();
  return 0;
}

int generatePython(int imask, QWidget *root)
{
  if(imask < 1)    return -1;
  if(root == NULL) return -1;

  FILE *fout;
  FileLines *fl;
  int found_begin, found_end, done_end, done;
  char filename[80];

  sprintf(filename,"mask%d.py",imask);
  if(loadFile(filename) != 0) return -1;
  fout = fopen(filename,"w");
  if(fout == NULL)
  {
    unloadFile();
    printf("ERROR: generatePython could not write %s\n",filename);
    return -1;
  }
  if(opt_develop.arg_debug) printf("generatePython %s\n",filename);

  found_begin = found_end = done_end = done = 0;
  fl = &file_lines;
  fl = fl->next;
  while(fl != NULL)
  {
    if(strstr(fl->line,"### begin of generated area, do not edit ###") != NULL) found_begin = 1;
    if(strstr(fl->line,"### end of generated area, do not edit ###")   != NULL) found_end = 1;
    if(found_begin == 0)
    {
      fprintf(fout,"%s",fl->line);
    }
    else if(found_begin == 1 && found_end == 0)
    {
      if(done==0) generateGeneratedAreaPython(fout, root, imask);
      done = 1;
    }
    else if(found_begin == 1 && found_end == 1)
    {
      if(done_end==1) fprintf(fout,"%s",fl->line);
      done_end = 1;
    }
    fl = fl->next;
  }

  fclose(fout);
  unloadFile();
  if(opt_develop.arg_debug) printf("generatePython end\n");
  return 0;
}

#endif
