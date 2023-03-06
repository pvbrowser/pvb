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
#include "../pvbrowser/pvdefine.h"
#include <QtGui>
#include "opt.h"
#include "MyWidgets.h"
#include "designer.h"
#include "widgetgenerator.h"
#include "dlgproperty.h"
#include "dlginsertwidget.h"
#include "dlgeditlayout.h"
#include "interpreter.h"

extern OPT_DEVELOP opt_develop;
extern QStringList tablist;
dlgeditlayout *editlayout = NULL;

#define Properties          "Properties (Alt+Click)"
#define Insert_Widget       "Insert Widget (Shift+Click)"
#define Delete_Widget       "Delete Widget (Del | Backspace)"
#define Add_Tab             "Add Tab"
#define Add_Item            "Add Item"
#define Copy_attributes     "copy attributes"
#define Define_new_TabOrder "define new TabOrder"
#define Edit_layout         "edit layout (L)"
#define End_define_TabOrder "end define TabOrder"
#define End_copy_attributes "end copy attributes"
#define DGrabMouse          "grabMouse (G)"
#define DReleaseMouse       "releaseMouse (R)"

Designer::Designer(const char *mask)
{
  root = new MyRootWidget(NULL);
  editlayout = new dlgeditlayout;
  root->setObjectName("0");
  filename = mask;
  imask = 0;
  sscanf(mask,"mask%d", &imask);
  xclick = yclick = 0;
  if(opt_develop.arg_debug) printf("Designer::Designer: before getWidgetsFromMask(%s)\n",mask);
  if(opt_develop.arg_mask_to_generate == -1) 
  {
    if(opt_develop.script == PV_LUA) 
    {
      //rlmurx-was-here filename.sprintf("mask%d.lua", imask); 
      filename = QString::asprintf("mask%d.lua", imask); 
      getWidgetsFromMask(filename.toUtf8(), root);
    }
    else
    {
      getWidgetsFromMask(mask, root);
    }
  }  
  if(opt_develop.arg_debug) printf("Designer::Designer: after getWidgetsFromMask(%s)\n",mask);
}

Designer::~Designer()
{
  int ret;

  if(root->modified == 1 || root->modified == -1)
  {
    QString header  = "pvdevelop";
    ret = QMessageBox::No;
    if(root->modified == 1)
    {
      QString message = "The design has been modified.\nDo you want to save your changes?";
      ret = QMessageBox::warning(root,header,message,QMessageBox::Yes,QMessageBox::No);
    }
    else
    {
      ret = QMessageBox::Yes;
    }
    if(ret == QMessageBox::Yes)
    { 
      generateMask(filename.toUtf8(), root); // generate C/C++
      // add additional language here
      if(opt_develop.script == PV_PYTHON)
      {
         if(opt_develop.arg_debug) printf("generate python imask=%d\n", imask);
         generatePython(imask, root);
      } 
      if(opt_develop.script == PV_PERL)
      {
         if(opt_develop.arg_debug) printf("generate perl imask=%d\n", imask);
         generatePerl(imask, root);
      } 
      if(opt_develop.script == PV_PHP)
      {
         if(opt_develop.arg_debug) printf("generate php imask=%d\n", imask);
         generatePHP(imask, root);
      } 
      if(opt_develop.script == PV_TCL)
      {
         if(opt_develop.arg_debug) printf("generate tcl imask=%d\n", imask);
         generateTcl(imask, root);
      } 
    }
  }
  delete editlayout;
  editlayout = NULL;
  QApplication::restoreOverrideCursor();
  QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
}

MyRootWidget::MyRootWidget(MyRootWidget *parent)
             :QWidget(parent)
{
  scroll = NULL;
  xOld = yOld = xChild0 = yChild0 = wChild0 = hChild0 = buttonDown = tabbing = copying = inMiddle = parentLevel = 0;
  modified = 0;
  reparentDone = 0;
  parentLevel0 = parentLevel1 = parentLevel2 = NULL;
  clickedChild = lastChild = NULL;
  statusBar = NULL;
  lastChild = NULL;
  lastTabChild = NULL;
  setMouseTracking(true);
  //setFocusPolicy(Qt::NoFocus);
  grabbed = 2;
  opt_develop.altPressed = 0;
  opt_develop.ctrlPressed = 0;
  opt_develop.shiftPressed = 0;
  insert.myrootwidget = this;
  setCursor(Qt::ArrowCursor);
}

MyRootWidget::~MyRootWidget()
{
  releaseMouse();
  mainWindow->releaseKeyboard();
  setCursor(Qt::ArrowCursor);
}

void MyRootWidget::setScroll(QScrollArea *_scroll)
{
  scroll = _scroll;
}

QWidget *MyRootWidget::getChild(int x, int y)
{
  QWidget *child;
  parentLevel = 0;
  child = childAt(x,y);
  parentLevel0 = child;
  if(child != NULL)
  {
    QString st = child->statusTip();
    if(st.isEmpty())
    {
      parentLevel = 1;
      child = (QWidget *) child->parent();
      parentLevel1 = child;
      st = child->statusTip();
    }
    if(st.isEmpty())
    {
      parentLevel = 2;
      child = (QWidget *) child->parent();
      parentLevel2 = child;
      st = child->statusTip();
    }
  }
  return child;
}

void MyRootWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
  int x = event->x();
  int y = event->y();
  if(opt_develop.arg_debug > 0) printf("DoubleClickEvent\n");
  //#####################################
  grabbed = 0;
  releaseMouse();
  mainWindow->releaseKeyboard();
  setCursor(Qt::ArrowCursor);
  //#####################################
  QWidget *p = this;
  if(parentLevel == 0)
  {
    if(lastChild != NULL)
    {
      lastChild->setAutoFillBackground(false);
      lastChild->setPalette(savedPalette);
    }
    insert.myrootwidget = this;
    insert.run();
    if(insert.ret == 1)
    {
      int xw,yw;
      QPoint parent0 = p->mapToGlobal(QPoint(0,0));
      QPoint thisxy  = mapToGlobal(QPoint(x,y));
      xw = thisxy.x() - parent0.x();
      yw = thisxy.y() - parent0.y();
      if(opt_develop.arg_debug) printf("Insert Widget: pos(%d,%d)\n",xw,yw);
      insert.newWidget(this,p,xw,yw);
      modified = 1;
      stackClear();
    }
  }
  grabbed = 1;
  grabMouse();
  mainWindow->grabKeyboard();
  setCursor(Qt::CrossCursor);
}

void MyRootWidget::MoveKey(int key)
{
  int xNew, yNew;
  int wNew, hNew;
  QWidget *child = lastChild;
  if(child == NULL) return;
  if(grabbed == 0)  return;

  push(child);
  if(opt_develop.ctrlPressed)
  {
    if     (key == Qt::Key_Left)
    {
      wNew = child->width() - opt_develop.xGrid;
      hNew = child->height();
    }
    else if(key == Qt::Key_Right)
    {
      wNew = child->width() + opt_develop.xGrid;
      hNew = child->height();
    }
    else if(key == Qt::Key_Up)
    {
      wNew = child->width();
      hNew = child->height() - opt_develop.yGrid;
    }
    else if(key == Qt::Key_Down)
    {
      wNew = child->width();
      hNew = child->height() + opt_develop.yGrid;
    }
    else
    {
      return;
    }
    wNew = (wNew/opt_develop.xGrid)*opt_develop.xGrid;
    hNew = (hNew/opt_develop.yGrid)*opt_develop.yGrid;
    if(wNew < opt_develop.xGrid) wNew = opt_develop.xGrid;
    if(hNew < opt_develop.yGrid) hNew = opt_develop.yGrid;
    if(wNew>=0 && hNew>=0)
    {
      myResize(child,wNew,hNew);
      modified = 1;
    }  
  }
  else
  {
    if     (key == Qt::Key_Left)
    {
      xNew = child->x() - opt_develop.xGrid;
      yNew = child->y();
    }
    else if(key == Qt::Key_Right)
    {
      xNew = child->x() + opt_develop.xGrid;
      yNew = child->y();
    }
    else if(key == Qt::Key_Up)
    {
      xNew = child->x();
      yNew = child->y() - opt_develop.yGrid;
    }
    else if(key == Qt::Key_Down)
    {
      xNew = child->x();
      yNew = child->y() + opt_develop.yGrid;
    }
    else
    {
      return;
    }
    xNew = (xNew/opt_develop.xGrid)*opt_develop.xGrid;
    yNew = (yNew/opt_develop.yGrid)*opt_develop.yGrid;
    if(xNew>=0 && yNew>=0 &&
       xNew<((QWidget *)child->parent())->width() && yNew<((QWidget *)child->parent())->height())
    {
      myMove(child,xNew,yNew);
      modified = 1;
    }
  }
  printStatusMessage(child);
}

void MyRootWidget::mouseMoveEvent(QMouseEvent *event)
{
  int x = event->x();
  int y = event->y();
  int gx = event->globalX();
  int gy = event->globalY();
  if(opt_develop.arg_debug > 0) printf("mouseMoveEvent x=%d y=%d\n",x,y);
  //#####################################
  if(aboveDesignArea(x,y,gx,gy))
  {
    grabbed = 1;
    grabMouse();
    mainWindow->grabKeyboard();
    if(currentCursor == Qt::ArrowCursor) setCursor(Qt::CrossCursor);
  }
  else
  {
    grabbed = 0;
    releaseMouse();
    mainWindow->releaseKeyboard();
    setCursor(Qt::ArrowCursor);
    return;
  }
  //#####################################
  QWidget *child = clickedChild;
  if(child != NULL && grabbed)
  {
    if(opt_develop.arg_debug > 0) printf("child->statusTip=%s\n",(const char *) child->statusTip().toUtf8());
    if(buttonDown && inMiddle==1)
    {
      int xNew, yNew;
      xNew = xChild0 + x - xOld;
      yNew = yChild0 + y - yOld;
      xNew = (xNew/opt_develop.xGrid)*opt_develop.xGrid;
      yNew = (yNew/opt_develop.yGrid)*opt_develop.yGrid;
      if(xNew>=0 && yNew>=0 &&
         xNew<((QWidget *)child->parent())->width() && yNew<((QWidget *)child->parent())->height())
      {
        push(child);
        myMove(child,xNew,yNew);
      }
      else if(reparentDone == 0) // reparent
      {
        QWidget *root = scroll->widget();
        if(root != NULL)
        {
          push(child);
          int dx = child->x();
          int dy = child->y();
          QPoint childPos0  = child->mapToGlobal(QPoint(0,0));
          QPoint parentPos0 = root->mapToGlobal(QPoint(0,0));
          child->hide();
          child->setParent(root);
          xNew = childPos0.x() - parentPos0.x();
          yNew = childPos0.y() - parentPos0.y();
          xNew = (xNew/opt_develop.xGrid)*opt_develop.xGrid + dx;
          yNew = (yNew/opt_develop.yGrid)*opt_develop.yGrid + dy;
          myMove(child,xNew,yNew);
          child->show();
          reparentDone = 1;
          xChild0 = child->x() - child->width()/2;
          yChild0 = child->y() - child->height()/2;
          wChild0 = child->width();
          hChild0 = child->height();
          xOld = xNew;
          yOld = yNew;
        }
      }
      modified = 1;
    }
    if(buttonDown && inMiddle==0)
    {
      int wNew, hNew;
      wNew = wChild0 + x - xOld;
      hNew = hChild0 + y - yOld;
      wNew = (wNew/opt_develop.xGrid)*opt_develop.xGrid;
      hNew = (hNew/opt_develop.yGrid)*opt_develop.yGrid;
      if(wNew < opt_develop.xGrid) wNew = opt_develop.xGrid;
      if(hNew < opt_develop.yGrid) hNew = opt_develop.yGrid;
      push(child);
      if(wNew>=0 && hNew>=0) myResize(child,wNew,hNew);
      modified = 1;
    }
  }
  else if(child == NULL && grabbed)
  {
    int x0,y0,w,h;
    child = getChild(x,y);
    if(child == NULL)
    {
      x0 = 0;
      y0 = 0;
      w  = width();
      h  = height();
    }
    else
    {
      QPoint childPos0 = child->mapToGlobal(QPoint(0,0));
      QPoint thisPos0  = mapToGlobal(QPoint(0,0));
      x0 = childPos0.x() - thisPos0.x();
      y0 = childPos0.y() - thisPos0.y();
      w  = child->width();
      h  = child->height();
    }
    if(opt_develop.arg_debug > 0) printf("x0=%d y0=%d x=%d y=%d w=%d h=%d\n",x0,y0,x,y,w,h);
    if((x-x0)>(w-5) && (y-y0)>(h-5))
    {
      setCursor(Qt::SizeFDiagCursor);
      inMiddle = 0;
    }
    else if(child != NULL &&
            child->statusTip().startsWith("TQ") &&
           !child->statusTip().startsWith("TQWidget"))
    {
      setCursor(Qt::SizeAllCursor);
      inMiddle = 1;
    }
    else
    {
      setCursor(Qt::CrossCursor);
      inMiddle = 1;
    }
  }
  printStatusMessage(child);
}

void MyRootWidget::printStatusMessage(QWidget *child)
{
  if(child != NULL && statusBar != NULL)
  {
    QString pos;
    //rlmurx-was-here pos.sprintf(" x=%d y=%d",child->x(),child->y());
    pos = QString::asprintf(" x=%d y=%d",child->x(),child->y());
    QString dim;
    //rlmurx-was-here dim.sprintf(" width=%d height=%d",child->width(),child->height());
    dim = QString::asprintf(" width=%d height=%d",child->width(),child->height());
    QString message = child->statusTip();
    message += " objectName=\"";
    message += child->objectName();
    message += "\" toolTip=\"";
    message += child->toolTip();
    message += "\" whatsThis=\"";
    message += child->whatsThis();
    message += "\"";
    message += pos;
    message += dim;
    statusBar->showMessage(message);
  }
  else if(statusBar != NULL)
  {
    statusBar->showMessage("over background");
  }
}

void MyRootWidget::setCursor(Qt::CursorShape cursor)
{

#ifdef PVWIN32
  // damn windows resets cursor otherwise
  QApplication::processEvents();
  QApplication::restoreOverrideCursor();
  QApplication::setOverrideCursor(QCursor(cursor));
#endif

#ifdef PVUNIX
  if(cursor != currentCursor)
  {
    QApplication::restoreOverrideCursor();
    QApplication::setOverrideCursor(QCursor(cursor));
  }  
  currentCursor = cursor;
#endif  

}

int MyRootWidget::aboveDesignArea(int x, int y, int gx, int gy)
{
  if(x < 0) return 0;
  if(y < 0) return 0;
  if(isActiveWindow())
  {
    int dx = 0;
    int dy = 0;
    QWidget *vp = scroll->viewport();
    if(vp->width()  < scroll->width())  dx = scroll->verticalScrollBar()->width();
    if(vp->height() < scroll->height()) dy = scroll->horizontalScrollBar()->height();
    QPoint gm = mainWindow->mapToGlobal(QPoint(0,0));
    if(gx < gm.x()) return 0;
    if(gy < gm.y()) return 0;
    if(gx > gm.x() + mainWindow->width()                                      - dx) return 0;
    if(gy > gm.y() + mainWindow->height() - mainWindow->statusBar()->height() - dy) return 0;
    return 1;
  }
  return 0;
}

void MyRootWidget::perhapsDeselectOldWidget()
{
  if(lastChild != NULL && tabbing == 0 && copying == 0)
  {
    if(opt_develop.arg_debug > 0) printf("deselect old widget\n");
    if(!lastChild->statusTip().contains("TQImage:")) lastChild->setAutoFillBackground(false);
    if( lastChild->statusTip().contains("TQLabel:")) lastChild->setAutoFillBackground(true);
    if( lastChild->statusTip().contains("TQFrame:")) lastChild->setAutoFillBackground(true);
    if( lastChild->statusTip().contains("TQRadio:")) lastChild->setAutoFillBackground(true);
    if( lastChild->statusTip().contains("TQCheck:")) lastChild->setAutoFillBackground(true);
    lastChild->setPalette(savedPalette);
    lastChild = NULL;
  }
}
void MyRootWidget::selectWidget(QWidget *child)
{
  if(opt_develop.arg_debug > 0) printf("mousePressEvent LeftButton set background2\n");
  if(child == NULL) return;
  savedPalette = child->palette();
  savedStatusTip = child->statusTip();
  child->setAutoFillBackground(true);
  child->setPalette(QPalette(QColor(0,0,0)));
  xChild0 = child->x();
  yChild0 = child->y();
  wChild0 = child->width();
  hChild0 = child->height();
  clickedChild = child;
  lastChild = child;
}

void MyRootWidget::mousePressEvent(QMouseEvent *event)
{
  int x = event->x();
  int y = event->y();
  int gx = event->globalX();
  int gy = event->globalY();
  if(opt_develop.arg_debug > 0) printf("mousePressEvent x=%d y=%d\n",x,y);
  reparentDone = 0;
  //#####################################
  if(aboveDesignArea(x,y,gx,gy))
  {
    grabbed = 1;
    grabMouse();
    mainWindow->grabKeyboard();
  }
  else
  {
    grabbed = 0;
    releaseMouse();
    mainWindow->releaseKeyboard();
    return;
  }
  //#####################################
  QWidget *child = getChild(x,y);
  if(opt_develop.altPressed)
  {
    if(child != NULL)
    {
      if(lastChild != NULL)
      {
        lastChild->setAutoFillBackground(false);
        if( lastChild->statusTip().contains("TQLabel:")) lastChild->setAutoFillBackground(true);
        if( lastChild->statusTip().contains("TQFrame:")) lastChild->setAutoFillBackground(true);
        if( lastChild->statusTip().contains("TQRadio:")) lastChild->setAutoFillBackground(true);
        if( lastChild->statusTip().contains("TQCheck:")) lastChild->setAutoFillBackground(true);
        lastChild->setPalette(savedPalette);
      }
      dlgProperty dlg(child);
      releaseMouse();
      mainWindow->releaseKeyboard();
      setCursor(Qt::ArrowCursor);
      dlg.run();
      grabbed = 1;
      grabMouse();
      mainWindow->grabKeyboard();
      modified = 1;
    }
    opt_develop.altPressed = 0;
    return;
  }
  if(opt_develop.shiftPressed)
  {
    mouseDoubleClickEvent(event); // show insert dialog
    opt_develop.shiftPressed = 0;
    return;
  }
  
  if(event->button() == Qt::LeftButton)
  {
    if(opt_develop.arg_debug > 0) printf("mousePressEvent LeftButton parentLevel=%d ctrlPressed=%d\n",parentLevel,opt_develop.ctrlPressed);
    perhapsDeselectOldWidget();
    if(child != NULL && opt_develop.ctrlPressed==0 && tabbing == 0 && copying == 0) // set highlight
    {
      if(opt_develop.arg_debug > 0) printf("mousePressEvent LeftButton set background\n");
      if(!child->statusTip().contains("TQWidget:"))
      {
        selectWidget(child);
      }
    }
    if(opt_develop.arg_debug > 0) printf("mousePressEvent LeftButton murx2\n");
    xOld = x;
    yOld = y;
    buttonDown = 1;
    if(opt_develop.ctrlPressed==1 && tabbing == 0 && copying == 0) // insert last selected widget
    {
      QWidget *p = child;
      if(p == NULL && x>0 && y>0) p = this;
      if(p != NULL && parentLevel == 0)
      {
        if(opt_develop.arg_debug > 0) printf("mousePressEvent LeftButton newWidget\n");
        int xw,yw;
        QPoint parent0 = p->mapToGlobal(QPoint(0,0));
        QPoint thisxy  = mapToGlobal(QPoint(x,y));
        xw = thisxy.x() - parent0.x();
        yw = thisxy.y() - parent0.y();
        if(opt_develop.arg_debug) printf("Insert Widget: pos(%d,%d)\n",xw,yw);
        grabbed = 0;
        releaseMouse();
        mainWindow->releaseKeyboard();
        setCursor(Qt::ArrowCursor);
        insert.myrootwidget = this;
        QWidget *wnew = insert.newWidget(this,p,xw,yw);
        setCursor(Qt::CrossCursor);
        modified = 1;
        grabbed = 1;
        grabMouse();
        mainWindow->grabKeyboard();
        stackClear();
        selectWidget(wnew);
      }
    }
    if(child != NULL && grabbed == 1 && tabbing == 1 && copying == 0
       && !child->statusTip().startsWith("TQWidget:"))
    {
      if(lastTab.isEmpty())
      {
        lastTab = child->objectName();
        lastTabChild = child;
      }
      else
      {
        QString newTab = child->objectName();
        QString tabCommand;
        if(opt_develop.script == PV_LUA) tabCommand = "  pv.pvTabOrder(p,";
        else                     tabCommand = "  pvTabOrder(p,";
        tabCommand.append(lastTab);
        tabCommand.append(",");
        tabCommand.append(newTab);
        if(opt_develop.script == PV_LUA) tabCommand.append(")\n");
        else                     tabCommand.append(");\n");
        tablist.append(tabCommand);
        if(lastTabChild != NULL && child != NULL) QWidget::setTabOrder(lastTabChild,child);
        lastTab = newTab;
        lastTabChild = child;
      }
      if(child->statusTip().startsWith("TQTabWidget:"))
      {
        releaseMouse();
        releaseKeyboard();
        mainWindow->releaseKeyboard();
        QMessageBox::information(0,"pvdevelop: define new TabOrder.",
                     "TabWidget added to TabOrder\n",
                     QMessageBox::Ok);
        grabbed = 1;
        grabMouse();
        mainWindow->grabKeyboard();
      }
      else if(child->statusTip().startsWith("TQToolBox:"))
      {
        releaseMouse();
        releaseKeyboard();
        mainWindow->releaseKeyboard();
        QMessageBox::information(0,"pvdevelop: define new TabOrder.",
                     "TollBox added to TabOrder\n",
                     QMessageBox::Ok);
        grabbed = 1;
        grabMouse();
        mainWindow->grabKeyboard();
      }
      else
      {
        child->hide();
      }
    }
    if(tabbing == 0 && copying == 1)
    {
      copyAttributes(lastChild,child);
    }
    if(opt_develop.arg_debug > 0) printf("mousePressEvent LeftButton done\n");
  }
  else if(event->button() == Qt::RightButton)
  {
    QMenu popupMenu;
    QAction *ret;
    if(grabbed == 1 && tabbing == 0 && copying == 0) popupMenu.addAction(Properties);
    if(grabbed == 1 && tabbing == 0 && copying == 0) popupMenu.addAction(Insert_Widget);
    if(grabbed == 1 && tabbing == 0 && copying == 0) popupMenu.addAction(Delete_Widget);
    if(child != NULL && child->statusTip().startsWith("TQTabWidget:") &&
       tabbing == 0 && copying == 0)
                                                     popupMenu.addAction(Add_Tab);
    if(child != NULL && child->statusTip().startsWith("TQToolBox:") &&
       tabbing == 0 && copying == 0)
                                                     popupMenu.addAction(Add_Item);
    if(grabbed == 1 && tabbing == 0 && copying == 0) popupMenu.addSeparator();
    if(grabbed == 1 && tabbing == 0 && copying == 0) popupMenu.addAction(Copy_attributes);
    if(grabbed == 1 && tabbing == 0 && copying == 0) popupMenu.addAction(Define_new_TabOrder);
    if(grabbed == 1 && tabbing == 0 && copying == 0) popupMenu.addAction(Edit_layout);
    if(grabbed == 1 && tabbing == 0 && copying == 0) popupMenu.addSeparator();
    if(grabbed == 1 && tabbing == 1)                 popupMenu.addAction(End_define_TabOrder);
    if(grabbed == 1 && copying == 1)                 popupMenu.addAction(End_copy_attributes);
    if((grabbed == 1 && tabbing == 1) || copying == 1) popupMenu.addSeparator();
    if(grabbed == 0) popupMenu.addAction(DGrabMouse);
    if(grabbed == 1) popupMenu.addAction(DReleaseMouse);
    setCursor(Qt::ArrowCursor);
    ret = popupMenu.exec(QCursor::pos());
    stackClear();
    if(ret == NULL)
    {
      grabbed = 1;
      grabMouse();
      mainWindow->grabKeyboard();
      setCursor(Qt::CrossCursor);
      return;
    }
    else
    {
      if(ret->text() == Properties && child != NULL &&
         !child->statusTip().startsWith("TQWidget:"))
      {
        if(child != NULL)
        {
          if(lastChild != NULL)
          {
            lastChild->setAutoFillBackground(false);
            if( lastChild->statusTip().contains("TQLabel:")) lastChild->setAutoFillBackground(true);
            if( lastChild->statusTip().contains("TQFrame:")) lastChild->setAutoFillBackground(true);
            if( lastChild->statusTip().contains("TQRadio:")) lastChild->setAutoFillBackground(true);
            if( lastChild->statusTip().contains("TQCheck:")) lastChild->setAutoFillBackground(true);
            lastChild->setPalette(savedPalette);
          }
          dlgProperty dlg(child);
          releaseMouse();
          mainWindow->releaseKeyboard();
          dlg.run();
          opt_develop.ctrlPressed = 0;
        }
        modified = 1;
      }
      else if(ret->text() == Properties)
      {
      }
      else if(ret->text() == Insert_Widget)
      {
        QWidget *p = child;
        if(p == NULL && x>0 && y>0) p = this;
        if(p != NULL && parentLevel == 0)
        {
          if(lastChild != NULL)
          {
            lastChild->setAutoFillBackground(false);
            lastChild->setPalette(savedPalette);
          }
          releaseMouse();
          mainWindow->releaseKeyboard();
          setCursor(Qt::ArrowCursor);
          insert.myrootwidget = this;
          insert.run();
          if(insert.ret == 1)
          {
            int xw,yw;
            QPoint parent0 = p->mapToGlobal(QPoint(0,0));
            QPoint thisxy  = mapToGlobal(QPoint(x,y));
            xw = thisxy.x() - parent0.x();
            yw = thisxy.y() - parent0.y();
            if(opt_develop.arg_debug) printf("Insert Widget: pos(%d,%d)\n",xw,yw);
            QWidget *wnew = insert.newWidget(this,p,xw,yw);
            modified = 1;
            stackClear();
            perhapsDeselectOldWidget();
            selectWidget(wnew);
          }
          setCursor(Qt::CrossCursor);
        }
      }
      else if(ret->text() == Delete_Widget)
      {
        if(lastChild != NULL)
        {
          lastChild->setAutoFillBackground(false);
          lastChild->setPalette(savedPalette);
        }
        lastChild = clickedChild = NULL;
        if(child != NULL) delete child;
        modified = 1;
        stackClear();
      }
      else if(ret->text() == Add_Tab)
      {
        if(child != NULL)
        {
          bool ok;
          releaseMouse();
          mainWindow->releaseKeyboard();
          setCursor(Qt::ArrowCursor);
          QString text = QInputDialog::getText(this, "pvdevelop: Add Tab",
                                                     "Title of tab", QLineEdit::Normal,
                                                     "", &ok);
          if(ok && !text.isEmpty())
          {
            QWidget *tab = new QWidget();
            tab->setStatusTip("TQWidget:");
            insert.myrootwidget = this;
            insert.setDefaultObjectName(this,tab);
            ((MyQTabWidget *)child)->addTab(tab, text);
            stackClear();
          }
          setCursor(Qt::CrossCursor);
        }
      }
      else if(ret->text() == Add_Item)
      {
        if(child != NULL)
        {
          QString text;
          bool ok;
          releaseMouse();
          mainWindow->releaseKeyboard();
          setCursor(Qt::ArrowCursor);
          text = QInputDialog::getText(this, "pvdevelop: Add Item",
                                                     "Title of Item", QLineEdit::Normal,
                                                     "", &ok);
          if(ok && !text.isEmpty())
          {
            QWidget *item = new QWidget();
            item->setStatusTip("TQWidget:");
            insert.myrootwidget = this;
            insert.setDefaultObjectName(this,item);
            ((MyQToolBox *)child)->addItem(item, text);
            stackClear();
          }
          setCursor(Qt::CrossCursor);
        }
      }
      else if(ret->text() == DGrabMouse)
      {
        grabMouse();
        mainWindow->grabKeyboard();
        setCursor(Qt::SizeAllCursor);
      }
      else if(ret->text() == DReleaseMouse)
      {
        if(lastChild != NULL)
        {
          lastChild->setAutoFillBackground(false);
          lastChild->setPalette(savedPalette);
        }
        releaseMouse();
        mainWindow->releaseKeyboard();
        grabbed = 0;
        setCursor(Qt::ArrowCursor);
        return;
      }
      else if(ret->text() == Define_new_TabOrder)
      {
        tabbing = modified = 1;
        tablist.clear();
        lastTab = "";
        lastTabChild = NULL;
        releaseMouse();
        mainWindow->releaseKeyboard();
        QMessageBox::information(0,"pvdevelop: define new TabOrder.",
                     "Click on the widgets in the order you want to define\n"
                     "The widgets will behidden.\n"
                     "When you are finished with definition click right mouse button and end definition.\n"
                     "In order to switch TabWidget: release mouse and grab it again.\n"
                     "In order to switch ToolBox: release mouse and grab it again.",
                     QMessageBox::Ok);
        stackClear();
      }
      else if(ret->text() == End_define_TabOrder)
      {
        tabbing = 0;
        showWidgets(this);
        stackClear();
      }
      else if(ret->text() == Copy_attributes)
      {
        copying = 1;
        releaseMouse();
        mainWindow->releaseKeyboard();
        QMessageBox::information(0,"pvdevelop: copy attributes.",
                     "You should have selected a widget before this.\n"
                     "Now click on the widgets you want to copy to.\n"
                     "When you are finished with copying click right mouse button and end copying.\n",
                     QMessageBox::Ok);
        stackClear();
      }
      else if(ret->text() == End_copy_attributes)
      {
        copying = 0;
      }
      else if(ret->text() == Edit_layout)
      {
        releaseMouse();
        mainWindow->releaseKeyboard();
        setCursor(Qt::ArrowCursor);
        editlayout->exec();
        modified = 1;
        stackClear();
      }
      else
      {
        printf("unknown popup text=%s\n",(const char *) ret->text().toUtf8());
      }
      grabbed = 1;
      grabMouse();
      mainWindow->grabKeyboard();
      setCursor(Qt::ArrowCursor);
    }
  }
}

void MyRootWidget::mouseReleaseEvent(QMouseEvent *event)
{
  int x = event->x();
  int y = event->y();
  int gx = event->globalX();
  int gy = event->globalY();
  if(opt_develop.arg_debug > 0) printf("mouseReleaseEvent x=%d y=%d\n",x,y);
  //#####################################
  if(aboveDesignArea(x,y,gx,gy))
  {
    grabbed = 1;
    grabMouse();
    mainWindow->grabKeyboard();
  }
  else
  {
    grabbed = 0;
    releaseMouse();
    mainWindow->releaseKeyboard();
    return;
  }
  //#####################################
  if(clickedChild != NULL)
  {
    clickedChild->hide();
    QWidget *item = childAt(x,y);
    if(item != NULL)
    {
      int xNew, yNew;
      push(clickedChild);
      QPoint parentPos0 = item->mapToGlobal(QPoint(0,0));
      QPoint childPos0  = clickedChild->mapToGlobal(QPoint(0,0));
      item->hide();
      clickedChild->setParent(item);
      xNew = childPos0.x() - parentPos0.x();
      yNew = childPos0.y() - parentPos0.y();
      xNew = (xNew/opt_develop.xGrid)*opt_develop.xGrid;
      yNew = (yNew/opt_develop.yGrid)*opt_develop.yGrid;
      if(xNew>=0 && yNew>=0) myMove(clickedChild,xNew,yNew);
      item->show();
      modified = 1;
    }
    clickedChild->show();
    modified = 1;
  }
  buttonDown = 0;
  clickedChild = NULL;
  if(opt_develop.arg_debug > 0) printf("mouseReleaseEvent end\n");
}

void MyRootWidget::showWidgets(QObject *item)
{
  QObject *subitem;
  QList<QObject *> itemlist;
  itemlist.clear();
  itemlist = item->children();
  for(int i=0; i<itemlist.size(); i++)
  {
    subitem = itemlist.at(i);
    if(subitem->isWidgetType() &&
      ((QWidget *) subitem)->statusTip().startsWith("TQ") &&
      !((QWidget *) subitem)->statusTip().startsWith("TQWidget:"))
    {
      ((QWidget *)subitem)->show();
    }
    showWidgets(subitem);
  }
}

void MyRootWidget::restoreColors(QWidget *item)
{
  QColor color;
  QString st = item->statusTip();
  st.remove("background:");
  st.remove("foreground:");
  int r,g,b;
  //item->unsetPalette();
  QPalette palette;
  item->setPalette(palette);
  if(savedStatusTip.contains("TQPushButton:") ||
     savedStatusTip.contains("TQRadio:")      ||
     savedStatusTip.contains("TQCheck:")      )
  {
    color = savedPalette.color(QPalette::ButtonText);
    r = color.red();
    g = color.green();
    b = color.blue();
    if(savedStatusTip.contains("foreground:"))
    {
      mySetForegroundColor(item,TQPushButton,r,g,b);
      st.append("foreground:");
    }
    color = savedPalette.color(QPalette::Button);
    r = color.red();
    g = color.green();
    b = color.blue();
    if(savedStatusTip.contains("background:"))
    {
      if(savedStatusTip.contains("TQPushButton:")) mySetBackgroundColor(item,TQPushButton,r,g,b);
      if(savedStatusTip.contains("TQRadio:"))      mySetBackgroundColor(item,TQRadio,r,g,b);
      if(savedStatusTip.contains("TQCheck:"))      mySetBackgroundColor(item,TQCheck,r,g,b);
      st.append("background:");
    }
  }
  else
  {
    color = savedPalette.color(QPalette::WindowText);
    r = color.red();
    g = color.green();
    b = color.blue();
    if(savedStatusTip.contains("foreground:"))
    {
      mySetForegroundColor(item,-1,r,g,b);
      st.append("foreground:");
    }
    color = savedPalette.color(QPalette::Window);
    r = color.red();
    g = color.green();
    b = color.blue();
    if(savedStatusTip.contains("background:"))
    {
      mySetBackgroundColor(item,-1,r,g,b);
      st.append("background:");
    }
  }
  item->setStatusTip(st);
}

void MyRootWidget::copyAttributes(QWidget *source, QWidget *destination)
{
  if(source == NULL || destination == NULL) return;
  //destination->unsetPalette();
  QPalette palette;
  destination->setPalette(palette);
  if(savedStatusTip.contains("background:") || savedStatusTip.contains("foreground:"))
  {
    restoreColors(destination);
  }
  if(savedStatusTip.contains("TQLabel:") && destination->statusTip().contains("TQLabel:"))
  {
    MyLabel *Source = (MyLabel *) source;
    MyLabel *Destination = (MyLabel *) destination;
    Destination->setFrameShape(Source->frameShape());
    Destination->setFrameShadow(Source->frameShadow());
    Destination->setLineWidth(Source->lineWidth());
    Destination->setMargin(Source->margin());
  }
  if(savedStatusTip.contains("TQFrame:") && destination->statusTip().contains("TQFrame:"))
  {
    MyFrame *Source = (MyFrame *) source;
    MyLabel *Destination = (MyLabel *) destination;
    Destination->setFrameShape(Source->frameShape());
    Destination->setFrameShadow(Source->frameShadow());
    Destination->setLineWidth(Source->lineWidth());
    //Destination->setMargin(Source->margin());
  }
}

void MyRootWidget::deleteLastChild()
{
  if(lastChild != NULL)
  {
    delete lastChild;
  }
  lastChild = clickedChild = NULL;
}

void MyRootWidget::showProperties()
{
  if(lastChild != NULL)
  {
    lastChild->setAutoFillBackground(false);
    lastChild->setPalette(savedPalette);
    dlgProperty dlg(lastChild);
    setCursor(Qt::ArrowCursor);
    releaseMouse();
    mainWindow->releaseKeyboard();
    int ret = dlg.run();
    if(ret == 1) modified = 1;
  }
}

void MyRootWidget::GrabMouse()
{
  grabMouse();
  mainWindow->grabKeyboard();
  grabbed = 1;
  setCursor(Qt::CrossCursor);
}

void MyRootWidget::ReleaseMouse()
{
  if(lastChild != NULL)
  {
    lastChild->setAutoFillBackground(false);
    lastChild->setPalette(savedPalette);
  }
  releaseMouse();
  mainWindow->releaseKeyboard();
  grabbed = 0;
  setCursor(Qt::ArrowCursor);
}

void MyRootWidget::EditLayout()
{
  releaseMouse();
  mainWindow->releaseKeyboard();
  setCursor(Qt::ArrowCursor);
  editlayout->exec();
  modified = 1;
  GrabMouse();
}

void MyRootWidget::push(QWidget *current)
{
  OldState state;
  if(current == NULL) return;

  state.OldX       = current->x();
  state.OldY       = current->y();
  state.OldWidth   = current->width();
  state.OldHeight  = current->height();
  state.OldParent  = (QWidget *) current->parent();
  state.OldCurrent = (QWidget *) current;
  stack.push(state);
}

QWidget *MyRootWidget::pop(QWidget *current)
{
  // current may be NULL
  OldState state;
  if(stack.isEmpty())
  {
    qApp->beep();
    return current;
  }
  state = stack.pop();
  if(current == state.OldCurrent)
  {
    if(current->parent() != state.OldParent)
    {
      state.OldParent->hide();
      current->setParent(state.OldParent);
      state.OldParent->show();
    }  
  }
  else
  {
    current = state.OldCurrent;
    if(current->parent() != state.OldParent)
    {
      state.OldParent->hide();
      current->setParent(state.OldParent);
      state.OldParent->show();
    }  
  }
  current->setGeometry(state.OldX, state.OldY, state.OldWidth, state.OldHeight);
  printStatusMessage(current);
  return current;
}

void MyRootWidget::stackClear()
{
  stack.clear();
}

