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

extern OPT opt;
extern QStringList tablist;
dlgeditlayout *editlayout = NULL;

Designer::Designer(const char *mask)
{
  root = new MyRootWidget(NULL);
  editlayout = new dlgeditlayout;
  root->setObjectName("0");
  filename = mask;
  imask = 0;
  sscanf(mask,"mask%d", &imask);
  xclick = yclick = 0;
  if(opt.arg_debug) printf("Designer::Designer: before getWidgetsFromMask(%s)\n",mask);
  if(opt.arg_mask_to_generate == -1) getWidgetsFromMask(mask, root);
  if(opt.arg_debug) printf("Designer::Designer: after getWidgetsFromMask(%s)\n",mask);
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
      generateMask(filename.toAscii(), root); // generate C/C++
      // add additional language here
      if(opt.script == PV_PYTHON)
      {
         if(opt.arg_debug) printf("generate python imask=%d\n", imask);
         generatePython(imask, root);
      } 
      if(opt.script == PV_PERL)
      {
         if(opt.arg_debug) printf("generate perl imask=%d\n", imask);
         generatePerl(imask, root);
      } 
      if(opt.script == PV_PHP)
      {
         if(opt.arg_debug) printf("generate php imask=%d\n", imask);
         generatePHP(imask, root);
      } 
      if(opt.script == PV_TCL)
      {
         if(opt.arg_debug) printf("generate tcl imask=%d\n", imask);
         generateTcl(imask, root);
      } 
    }
  }
  delete editlayout;
  editlayout = NULL;
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
  opt.ctrlPressed = 0;
  QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
}

MyRootWidget::~MyRootWidget()
{
  releaseMouse();
  mainWindow->releaseKeyboard();
  QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
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
  int xs = scroll->width();
  int ys = scroll->height();
  if(opt.arg_debug > 0) printf("DoubleClickEvent\n");
  if((x<0 || y<0 || x>xs || y>ys) && grabbed == 1)
  {
    releaseMouse();
    mainWindow->releaseKeyboard();
    grabbed = 2;
    return;
  }
  if(y >= 0 && grabbed == 2)
  {
    grabbed = 1;
    grabMouse();
    mainWindow->grabKeyboard();
  }
  else // call insertdialog
  {
    QWidget *p = this;
    if(parentLevel == 0)
    {
      if(lastChild != NULL)
      {
        lastChild->setAutoFillBackground(false);
        lastChild->setPalette(savedPalette);
      }
      releaseMouse();
      mainWindow->releaseKeyboard();
      QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
      insert.run();
      QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
      if(insert.ret == 1)
      {
        int xw,yw;
        QPoint parent0 = p->mapToGlobal(QPoint(0,0));
        QPoint thisxy  = mapToGlobal(QPoint(x,y));
        xw = thisxy.x() - parent0.x();
        yw = thisxy.y() - parent0.y();
        if(opt.arg_debug) printf("Insert Widget: pos(%d,%d)\n",xw,yw);
        insert.newWidget(this,p,xw,yw);
        modified = 1;
      }
    }
    grabMouse();
    mainWindow->grabKeyboard();
    grabbed = 1;
    QApplication::setOverrideCursor(QCursor(Qt::SizeAllCursor));
  }
}

void MyRootWidget::MoveKey(int key)
{
  int xNew, yNew;
  int wNew, hNew;
  QWidget *child = lastChild;
  if(child == NULL) return;
  if(grabbed == 0)  return;

  if(opt.ctrlPressed)
  {
    if     (key == Qt::Key_Left)
    {
      wNew = wChild0 - opt.xGrid;
      hNew = hChild0;
    }
    else if(key == Qt::Key_Right)
    {
      wNew = wChild0 + opt.xGrid;
      hNew = hChild0;
    }
    else if(key == Qt::Key_Up)
    {
      wNew = wChild0;
      hNew = hChild0 - opt.yGrid;
    }
    else if(key == Qt::Key_Down)
    {
      wNew = wChild0;
      hNew = hChild0 + opt.yGrid;
    }
    else
    {
      return;
    }
    wNew = (wNew/opt.xGrid)*opt.xGrid;
    hNew = (hNew/opt.yGrid)*opt.yGrid;
    if(wNew < opt.xGrid) wNew = opt.xGrid;
    if(hNew < opt.yGrid) hNew = opt.yGrid;
    if(wNew>=0 && hNew>=0)
    {
      myResize(child,wNew,hNew);
      wChild0 = wNew;
      hChild0 = hNew;
      modified = 1;
    }  
  }
  else
  {
    if     (key == Qt::Key_Left)
    {
      xNew = xChild0 - opt.xGrid;
      yNew = yChild0;
    }
    else if(key == Qt::Key_Right)
    {
      xNew = xChild0 + opt.xGrid;
      yNew = yChild0;
    }
    else if(key == Qt::Key_Up)
    {
      xNew = xChild0;
      yNew = yChild0 - opt.yGrid;
    }
    else if(key == Qt::Key_Down)
    {
      xNew = xChild0;
      yNew = yChild0 + opt.yGrid;
    }
    else
    {
      return;
    }
    xNew = (xNew/opt.xGrid)*opt.xGrid;
    yNew = (yNew/opt.yGrid)*opt.yGrid;
    if(xNew>=0 && yNew>=0 &&
       xNew<((QWidget *)child->parent())->width() && yNew<((QWidget *)child->parent())->height())
    {
      myMove(child,xNew,yNew);
      xChild0 = xNew;
      yChild0 = yNew;
      modified = 1;
    }
  }
}

void MyRootWidget::mouseMoveEvent(QMouseEvent *event)
{
  int x = event->x();
  int y = event->y();
  int xs = scroll->width();
  int ys = scroll->height();
  int xx = -1;
  int yy = -1;
  int ww = -1;
  int hh = -1;
  if(opt.arg_debug > 0) printf("mouseMoveEvent x=%d y=%d\n",x,y);
  if((x<0 || y<0 || x>xs || y>ys) && grabbed == 1)
  {
    QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
    releaseMouse();
    mainWindow->releaseKeyboard();
    grabbed = 2;
    return;
  }
  if(y >= 0 && grabbed == 2)
  {
    grabbed = 1;
    grabMouse();
    mainWindow->grabKeyboard();
    QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
  }
  QWidget *child = clickedChild;
  if(child != NULL && grabbed)
  {
    if(opt.arg_debug > 0) printf("child->statusTip=%s\n",(const char *) child->statusTip().toAscii());
    if(buttonDown && inMiddle==1)
    {
      int xNew, yNew;
      xNew = xChild0 + x - xOld;
      yNew = yChild0 + y - yOld;
      xNew = (xNew/opt.xGrid)*opt.xGrid;
      yNew = (yNew/opt.yGrid)*opt.yGrid;
      xx = xNew;
      yy = yNew;
      if(xNew>=0 && yNew>=0 &&
         xNew<((QWidget *)child->parent())->width() && yNew<((QWidget *)child->parent())->height())
      {
        myMove(child,xNew,yNew);
      }
      else if(reparentDone == 0) // reparent
      {
        QWidget *root = scroll->widget();
        if(root != NULL)
        {
          QPoint childPos0  = child->mapToGlobal(QPoint(0,0));
          QPoint parentPos0 = root->mapToGlobal(QPoint(0,0));
          child->hide();
          child->setParent(root);
          xNew = childPos0.x() - parentPos0.x();
          yNew = childPos0.y() - parentPos0.y();
          xNew = (xNew/opt.xGrid)*opt.xGrid;
          yNew = (yNew/opt.yGrid)*opt.yGrid;
          myMove(child,xNew,yNew);
          child->show();
          reparentDone = 1;
        }
      }
      modified = 1;
    }
    if(buttonDown && inMiddle==0)
    {
      int wNew, hNew;
      wNew = wChild0 + x - xOld;
      hNew = hChild0 + y - yOld;
      wNew = (wNew/opt.xGrid)*opt.xGrid;
      hNew = (hNew/opt.yGrid)*opt.yGrid;
      if(wNew < opt.xGrid) wNew = opt.xGrid;
      if(hNew < opt.yGrid) hNew = opt.yGrid;
      ww = wNew;
      hh = hNew;
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
    if(opt.arg_debug > 0) printf("x0=%d y0=%d x=%d y=%d w=%d h=%d\n",x0,y0,x,y,w,h);
    if((x-x0)>(w-5) && (y-y0)>(h-5))
    {
      QApplication::setOverrideCursor(QCursor(Qt::SizeFDiagCursor));
      inMiddle = 0;
    }
    else if(child != NULL &&
            child->statusTip().startsWith("TQ") &&
           !child->statusTip().startsWith("TQWidget"))
    {
      QApplication::setOverrideCursor(QCursor(Qt::SizeAllCursor));
      inMiddle = 1;
    }
    else
    {
      QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
      inMiddle = 1;
    }
  }
  if(child != NULL && statusBar != NULL)
  {
    QString pos;
    pos.sprintf(" x=%d y=%d",xx,yy);
    QString dim;
    dim.sprintf(" width=%d height=%d",ww,hh);
    QString message = child->statusTip();
    message += " objectName=\"";
    message += child->objectName();
    message += "\" toolTip=\"";
    message += child->toolTip();
    message += "\" whatsThis=\"";
    message += child->whatsThis();
    message += "\"";
    if(xx != -1 && yy != -1) message += pos;
    if(ww != -1 && hh != -1) message += dim;
    statusBar->showMessage(message);
  }
  else if(statusBar != NULL)
  {
    statusBar->showMessage("over background");
  }
}

void MyRootWidget::mousePressEvent(QMouseEvent *event)
{
  int x = event->x();
  int y = event->y();
  int xs = scroll->width();
  int ys = scroll->height();
  if(opt.arg_debug > 0) printf("mousePressEvent x=%d y=%d\n",x,y);
  reparentDone = 0;
  if((x<0 || y<0 || x>xs || y>ys) && grabbed == 1)
  {
    releaseMouse();
    mainWindow->releaseKeyboard();
    grabbed = 2;
    return;
  }
  if(y >= 0 && grabbed == 2)
  {
    grabbed = 1;
    grabMouse();
    mainWindow->grabKeyboard();
  }
  QWidget *child = getChild(x,y);
  if(event->button() == Qt::LeftButton)
  {
    if(opt.arg_debug > 0) printf("mousePressEvent LeftButton parentLevel=%d ctrlPressed=%d\n",parentLevel,opt.ctrlPressed);
    if(lastChild != NULL && opt.ctrlPressed ==  0 && tabbing == 0 && copying == 0)
    {
      if(opt.arg_debug > 0) printf("mousePressEvent LeftButton reset background\n");
      if(!lastChild->statusTip().contains("TQImage:")) lastChild->setAutoFillBackground(false);
      if( lastChild->statusTip().contains("TQLabel:")) lastChild->setAutoFillBackground(true);
      if( lastChild->statusTip().contains("TQFrame:")) lastChild->setAutoFillBackground(true);
      if( lastChild->statusTip().contains("TQRadio:")) lastChild->setAutoFillBackground(true);
      if( lastChild->statusTip().contains("TQCheck:")) lastChild->setAutoFillBackground(true);
      if(opt.arg_debug > 0) printf("mousePressEvent LeftButton Verzweifelung1\n");
      lastChild->setPalette(savedPalette);
      if(opt.arg_debug > 0) printf("mousePressEvent LeftButton Verzweifelung2\n");
      lastChild = NULL;
    }
    if(opt.arg_debug > 0) printf("mousePressEvent LeftButton murx1\n");
    if(child != NULL && opt.ctrlPressed==0 && tabbing == 0 && copying == 0) // set highlight
    {
      if(opt.arg_debug > 0) printf("mousePressEvent LeftButton set background\n");
      if(!child->statusTip().contains("TQWidget:"))
      {
        if(opt.arg_debug > 0) printf("mousePressEvent LeftButton set background2\n");
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
    }
    if(opt.arg_debug > 0) printf("mousePressEvent LeftButton murx2\n");
    xOld = x;
    yOld = y;
    buttonDown = 1;
    if(opt.ctrlPressed==1 && tabbing == 0 && copying == 0) // insert last selected widget
    {
      QWidget *p = child;
      if(p == NULL && x>0 && y>0) p = this;
      if(p != NULL && parentLevel == 0)
      {
        if(opt.arg_debug > 0) printf("mousePressEvent LeftButton newWidget\n");
        int xw,yw;
        QPoint parent0 = p->mapToGlobal(QPoint(0,0));
        QPoint thisxy  = mapToGlobal(QPoint(x,y));
        xw = thisxy.x() - parent0.x();
        yw = thisxy.y() - parent0.y();
        if(opt.arg_debug) printf("Insert Widget: pos(%d,%d)\n",xw,yw);
        insert.newWidget(this,p,xw,yw);
        modified = 1;
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
        QString tabCommand = "  pvTabOrder(p,";
        tabCommand.append(lastTab);
        tabCommand.append(",");
        tabCommand.append(newTab);
        tabCommand.append(");\n");
        tablist.append(tabCommand);
        if(lastTabChild != NULL && child != NULL) QWidget::setTabOrder(lastTabChild,child);
        lastTab = newTab;
        lastTabChild = child;
      }
      if(child->statusTip().startsWith("TQTabWidget:"))
      {
        releaseMouse();
        mainWindow->releaseKeyboard();
        QMessageBox::information(0,"pvdevelop: define new TabOrder.",
                     "TabWidget added to TabOrder\n",
                     QMessageBox::Ok);
        grabMouse();
        mainWindow->grabKeyboard();
      }
      else if(child->statusTip().startsWith("TQToolBox:"))
      {
        releaseMouse();
        mainWindow->releaseKeyboard();
        QMessageBox::information(0,"pvdevelop: define new TabOrder.",
                     "TollBox added to TabOrder\n",
                     QMessageBox::Ok);
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
    if(opt.arg_debug > 0) printf("mousePressEvent LeftButton done\n");
  }
  else if(event->button() == Qt::RightButton)
  {
    QMenu popupMenu;
    QAction *ret;
    if(grabbed == 1 && tabbing == 0 && copying == 0) popupMenu.addAction("Properties");
    if(grabbed == 1 && tabbing == 0 && copying == 0) popupMenu.addAction("Insert Widget");
    if(grabbed == 1 && tabbing == 0 && copying == 0) popupMenu.addAction("Delete Widget");
    if(child != NULL && child->statusTip().startsWith("TQTabWidget:") &&
       tabbing == 0 && copying == 0)
                                                     popupMenu.addAction("Add Tab");
    if(child != NULL && child->statusTip().startsWith("TQToolBox:") &&
       tabbing == 0 && copying == 0)
                                                     popupMenu.addAction("Add Item");
    if(grabbed == 1 && tabbing == 0 && copying == 0) popupMenu.addSeparator();
    if(grabbed == 1 && tabbing == 0 && copying == 0) popupMenu.addAction("copy attributes");
    if(grabbed == 1 && tabbing == 0 && copying == 0) popupMenu.addAction("define new TabOrder");
    if(grabbed == 1 && tabbing == 0 && copying == 0) popupMenu.addAction("edit layout");
    if(grabbed == 1 && tabbing == 0 && copying == 0) popupMenu.addSeparator();
    if(grabbed == 1 && tabbing == 1)                 popupMenu.addAction("end define TabOrder");
    if(grabbed == 1 && copying == 1)                 popupMenu.addAction("end copy attributes");
    if((grabbed == 1 && tabbing == 1) || copying == 1) popupMenu.addSeparator();
    if(grabbed == 0) popupMenu.addAction("grabMouse");
    if(grabbed == 1) popupMenu.addAction("releaseMouse");
    QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
    ret = popupMenu.exec(QCursor::pos());
    if(ret != NULL)
    {
      if(ret->text() == "Properties" && child != NULL &&
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
          opt.ctrlPressed = 0;
        }
        grabMouse();
        mainWindow->grabKeyboard();
        grabbed = 1;
        QApplication::setOverrideCursor(QCursor(Qt::SizeAllCursor));
        modified = 1;
      }
      else if(ret->text() == "Insert Widget")
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
          insert.run();
          if(insert.ret == 1)
          {
            int xw,yw;
            QPoint parent0 = p->mapToGlobal(QPoint(0,0));
            QPoint thisxy  = mapToGlobal(QPoint(x,y));
            xw = thisxy.x() - parent0.x();
            yw = thisxy.y() - parent0.y();
            if(opt.arg_debug) printf("Insert Widget: pos(%d,%d)\n",xw,yw);
            insert.newWidget(this,p,xw,yw);
            modified = 1;
          }
        }
        grabMouse();
        mainWindow->grabKeyboard();
        grabbed = 1;
        QApplication::setOverrideCursor(QCursor(Qt::SizeAllCursor));
      }
      else if(ret->text() == "Delete Widget")
      {
        if(lastChild != NULL)
        {
          lastChild->setAutoFillBackground(false);
          lastChild->setPalette(savedPalette);
        }
        lastChild = clickedChild = NULL;
        if(child != NULL) delete child;
        grabMouse();
        mainWindow->grabKeyboard();
        grabbed = 1;
        modified = 1;
        QApplication::setOverrideCursor(QCursor(Qt::SizeAllCursor));
      }
      else if(ret->text() == "Add Tab")
      {
        if(child != NULL)
        {
          bool ok;
          releaseMouse();
          mainWindow->releaseKeyboard();
          QString text = QInputDialog::getText(this, "pvdevelop: Add Tab",
                                                     "Title of tab", QLineEdit::Normal,
                                                     "", &ok);
          if(ok && !text.isEmpty())
          {
            QWidget *tab = new QWidget();
            tab->setStatusTip("TQWidget:");
            insert.setDefaultObjectName(this,tab);
            ((MyQTabWidget *)child)->addTab(tab, text);
          }
          grabMouse();
          mainWindow->grabKeyboard();
          grabbed = 1;
        }
      }
      else if(ret->text() == "Add Item")
      {
        if(child != NULL)
        {
          QString text;
          bool ok;
          releaseMouse();
          mainWindow->releaseKeyboard();
          text = QInputDialog::getText(this, "pvdevelop: Add Item",
                                                     "Title of Item", QLineEdit::Normal,
                                                     "", &ok);
          if(ok && !text.isEmpty())
          {
            QWidget *item = new QWidget();
            item->setStatusTip("TQWidget:");
            insert.setDefaultObjectName(this,item);
            ((MyQToolBox *)child)->addItem(item, text);
          }
          grabMouse();
          mainWindow->grabKeyboard();
          grabbed = 1;
        }
      }
      else if(ret->text() == "grabMouse")
      {
        grabMouse();
        mainWindow->grabKeyboard();
        grabbed = 1;
        QApplication::setOverrideCursor(QCursor(Qt::SizeAllCursor));
      }
      else if(ret->text() == "releaseMouse")
      {
        if(lastChild != NULL)
        {
          lastChild->setAutoFillBackground(false);
          lastChild->setPalette(savedPalette);
        }
        releaseMouse();
        mainWindow->releaseKeyboard();
        grabbed = 0;
        QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
      }
      else if(ret->text() == "define new TabOrder")
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
        grabMouse();
        mainWindow->grabKeyboard();
      }
      else if(ret->text() == "end define TabOrder")
      {
        tabbing = 0;
        showWidgets(this);
      }
      else if(ret->text() == "copy attributes")
      {
        copying = 1;
        releaseMouse();
        mainWindow->releaseKeyboard();
        QMessageBox::information(0,"pvdevelop: copy attributes.",
                     "You should have selected a widget before this.\n"
                     "Now click on the widgets you want to copy to.\n"
                     "When you are finished with copying click right mouse button and end copying.\n",
                     QMessageBox::Ok);
        grabMouse();
        mainWindow->grabKeyboard();
      }
      else if(ret->text() == "end copy attributes")
      {
        copying = 0;
      }
      else if(ret->text() == "edit layout")
      {
        releaseMouse();
        mainWindow->releaseKeyboard();
        QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
        editlayout->exec();
        modified = 1;
        grabMouse();
        mainWindow->grabKeyboard();
      }
      else
      {
        printf("unknown popup text=%s\n",(const char *) ret->text().toAscii());
      }
    }
  }
}

void MyRootWidget::mouseReleaseEvent(QMouseEvent *event)
{
  int x = event->x();
  int y = event->y();
  int xs = scroll->width();
  int ys = scroll->height();
  if(opt.arg_debug > 0) printf("mouseReleaseEvent x=%d y=%d\n",x,y);
  if((x<0 || y<0 || x>xs || y>ys) && grabbed == 1)
  {
    releaseMouse();
    mainWindow->releaseKeyboard();
    grabbed = 2;
    return;
  }
  if(y >= 0 && grabbed == 2)
  {
    grabbed = 1;
    grabMouse();
    mainWindow->grabKeyboard();
  }
  if(clickedChild != NULL)
  {
    clickedChild->hide();
    QWidget *item = childAt(x,y);
    if(item != NULL)
    {
      int xNew, yNew;
      QPoint parentPos0 = item->mapToGlobal(QPoint(0,0));
      QPoint childPos0  = clickedChild->mapToGlobal(QPoint(0,0));
      item->hide();
      clickedChild->setParent(item);
      //printf("parentPos0(%d,%d) childPos(%d,%d)\n",parentPos0.x(),parentPos0.y(),childPos0.x(),childPos0.y());
      xNew = childPos0.x() - parentPos0.x();
      yNew = childPos0.y() - parentPos0.y();
      xNew = (xNew/opt.xGrid)*opt.xGrid;
      yNew = (yNew/opt.yGrid)*opt.yGrid;
      if(xNew>=0 && yNew>=0) myMove(clickedChild,xNew,yNew);
      item->show();
      modified = 1;
    }
    clickedChild->show();
    modified = 1;
  }
  buttonDown = 0;
  clickedChild = NULL;
  if(opt.arg_debug > 0) printf("mouseReleaseEvent end\n");
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
    QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
    releaseMouse();
    mainWindow->releaseKeyboard();
    int ret = dlg.run();
    grabMouse();
    mainWindow->grabKeyboard();
    grabbed = 1;
    QApplication::setOverrideCursor(QCursor(Qt::SizeAllCursor));
    if(ret == 1) modified = 1;
  }
}

void MyRootWidget::GrabMouse()
{
  grabMouse();
  mainWindow->grabKeyboard();
  grabbed = 1;
  QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
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
  QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
}

void MyRootWidget::EditLayout()
{
  releaseMouse();
  mainWindow->releaseKeyboard();
  QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
  editlayout->exec();
  modified = 1;
  GrabMouse();
}
