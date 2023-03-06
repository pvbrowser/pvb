/***************************************************************************
                          MyWidgets.h  -  description
                             -------------------
    begin                : Mon Dec 11 2000
    copyright            : (C) 2000 by R. Lehrig
                         : M. Bangieff
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef _MY_WIDGETS_H_
#define _MY_WIDGETS_H_

#include <QMouseEvent>
#include <qwidget.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qslider.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qtabwidget.h>
#include <qtoolbox.h>
#include <qspinbox.h>
#include <qdial.h>
#include <qpixmap.h>
#include <qdialog.h>
#include <qprogressbar.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qtablewidget.h>
#include <QTimeEdit>
#include <QDateEdit>
#include <QDateTimeEdit>
#include <QTextEdit>
#include <QTextBrowser>
//v5diff #ifndef NO_WEBKIT
//v5diff #include <QWebView>
//v5diff #endif
#include <QListWidget>
#include <QTreeWidget>
#include <QDockWidget>
// --- QWT ---
#ifndef NO_QWT
#include "qwtwidgets.h"
#endif

void mySetForegroundColor(QWidget *w, int type, int r, int g, int b);
void mySetBackgroundColor(QWidget *w, int type, int r, int g, int b);

class Interpreter;

class MyDialog : public QDialog
{
    Q_OBJECT
public:
    MyDialog(Interpreter *interpreter, int *sock, int ident, QWidget * parent=0, const char *name=0, bool modal=true);
    ~MyDialog();
private:
    int *s,id;
    void done(int result);
    Interpreter *interpreter;
};

class MyQWidget : public QWidget
{
    Q_OBJECT
public:
    MyQWidget(int *sock, int ident, QWidget * parent, const char * name=0 );
    ~MyQWidget();
    int sendJpeg2clipboard();

public slots:

protected:
    //virtual void mousePressEvent(QMouseEvent *e);

private:
    int *s,id;
};

class MyLabel : public QLabel
{
    Q_OBJECT
public:
    MyLabel(int *sock, int ident, QWidget * parent, const char * name=0 );
    ~MyLabel();
    int row, col;

private:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    int *s,id;
};

class MyQPushButton : public QPushButton
{
    Q_OBJECT
public:
    MyQPushButton(int *sock, int ident, QWidget * parent, const char * name=0 );
    ~MyQPushButton();
    int row, col;
 
public slots:
    void slotClicked();
    void slotPressed();
    void slotReleased();

private:
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    int *s,id;
};

class MyLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    MyLineEdit(int *sock, int ident, QWidget * parent, const char * name=0 );
    ~MyLineEdit();

public slots:
    void slotTextChanged(const QString &txt);
    void slotReturnPressed();

private:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    int *s,id;
};

class MyComboBox : public QComboBox
{
    Q_OBJECT
public:
    MyComboBox(int *sock, int ident, QWidget * parent, const char * name=0 );
    ~MyComboBox();
    void removeItemByName(QString name);
    int row, col;

public slots:
    void slotActivated(const QString &txt);

private:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    int *s,id;
};

class MySlider : public QSlider
{
    Q_OBJECT
public:
    MySlider(int *sock, int ident, int minValue, int maxValue, int pageStep, int value, Qt::Orientation, QWidget * parent, const char * name=0 );
    ~MySlider();

public slots:
    void slotValueChanged(int value);

private:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    int *s,id;
};

class MyCheckBox : public QCheckBox
{
    Q_OBJECT
public:
    MyCheckBox(int *sock, int ident, QWidget * parent, const char * name=0 );
    ~MyCheckBox();
    int row, col;

public slots:
    void slotClicked();

private:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    int *s,id;
};

class MyRadioButton : public QRadioButton
{
    Q_OBJECT
public:
    MyRadioButton(int *sock, int ident, QWidget * parent, const char * name=0 );
    ~MyRadioButton();

public slots:
    void slotToggled(bool on);

private:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    int *s,id;
};

class MyButtonGroup : public QGroupBox
{
    Q_OBJECT
public:
    MyButtonGroup(int *sock, int ident, int columns, Qt::Orientation o, QString title, QWidget * parent=0, const char * name=0 );
    ~MyButtonGroup();

private:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    int *s,id;
};

class MyFrame : public QFrame
{
    Q_OBJECT
public:
    MyFrame(int *sock, int ident, int shape, int shadow, int line_width, int margin,
            QWidget * parent=0, const char * name=0);
    ~MyFrame();

private:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    int *s,id;
};

class MyQTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    MyQTabWidget(int *sock, int ident, QWidget * parent=0, const char *name=0);
    ~MyQTabWidget();

public slots:
    void slotCurrentChanged(int index);
    void enableTabBar(int state);

private:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    int *s,id;
};

class MyQToolBox : public QToolBox
{
    Q_OBJECT
public:
    MyQToolBox(int *sock, int ident, QWidget * parent=0, const char * name=0);
    ~MyQToolBox();

public slots:
    void slotCurrentChanged(int index);

private:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    int *s,id;
};

class MyGroupBox : public QGroupBox
{
    Q_OBJECT
public:
    MyGroupBox(int *sock, int ident, int columns, Qt::Orientation o, QString title, QWidget * parent=0, const char * name=0 );
    ~MyGroupBox();

private:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    int *s,id;
};

class MyListBox : public QListWidget
{
    Q_OBJECT
public:
    MyListBox(int *sock, int ident, QWidget * parent=0, const char * name=0);
    ~MyListBox();
    virtual void insertItem(QPixmap &pixmap, const QString &text, int index=-1);
    virtual void changeItem(QPixmap &pixmap, const QString &text, int index);
    virtual void removeItem(int index);
    virtual void removeItemByName(QString name);
    virtual void clear();

public slots:
    void slotClicked(QListWidgetItem *item);
    void slotCurrentChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void slotSendSelected();

private:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    int *s,id;
};

class MyTable : public QTableWidget
{
    Q_OBJECT
public:
    MyTable(int *sock, int ident, int numRows, int numColumns, QWidget *parent=0, const char *name=0);
    ~MyTable();
    void setEditable(int editable);
    void setTableButton(int row, int col, QString text);
    void setTableCheckBox(int row, int col, int state, QString text);
    void setTableComboBox(int row, int col, int editable, const char *text);
    void setTableLabel(int row, int col, QString text);
    void copyToClipboard(int title);
    void saveTextfile(const char *filename=NULL);
    void clear();
    int wrap,read_only,updates,autoresize,is_editable;

public slots:
    void slotRowClicked(int section);
    void slotColClicked(int section);
    void slotClicked( int row, int col);
    void slotCurrentChanged( int row, int col, int oldrow, int oldcol);
    void slotValueChanged(int row, int col);
    void slotActivated(QModelIndex index);
private:
    void mousePressEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    int *s,id,button;
    QString cellbuf;
};

class MySpinBox : public QSpinBox
{
    Q_OBJECT
public:
    MySpinBox(int *sock, int ident, int minValue, int maxValue, int step=1, QWidget *parent=0, const char *name=0);
    ~MySpinBox();
    // virtual void setValue(int)

public slots:
   void slotValueChanged(int value);

private:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    int *s,id;
};

class MyDial : public QDial
{

    Q_OBJECT
public:
    MyDial(int *sock, int ident, int minValue, int maxValue, int pageStep, int value, QWidget *parent=0, const char *name=0);
    ~MyDial();
    virtual void setValue(int);

public slots:
   void slotValueChanged(int value);

private:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    int *s,id;
};

class MyProgressBar : public QProgressBar
{
    Q_OBJECT
public:
    MyProgressBar(int *sock, int ident, int totalSteps, Qt::Orientation, QWidget *parent=0, const char *name=0);
    ~MyProgressBar();

private:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    int *s,id;
};

class MyMultiLineEdit : public QTextEdit
{
    Q_OBJECT
public:
    MyMultiLineEdit(int *sock, int ident, int editable, int maxLines, QWidget * parent=0, const char * name=0);
    ~MyMultiLineEdit();
    virtual void setText(const QString &text);
    int maxlines,num_lines;

public slots:
    void slotSendToClipboard();

private:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    int *s,id;
};

class MyListViewItem;

#define MAX_TREE_RECURSION 80

class MyListView : public QTreeWidget
{
    Q_OBJECT
public:
    MyListView(int *sock, int ident, QWidget * parent=0, const char * name=0);
    ~MyListView();
    void setListViewText(const char *path, int column, QString &text);
    void setListViewPixmap(const char *path, int column, QPixmap &pixmap);
    int  deleteListViewItem(const char *path, MyListViewItem *item);
    void doSendSelected(MyListViewItem *item);
    int  ensureVisible(const char *path, MyListViewItem *item);
    int  setItemOpen(const char *path, int open, MyListViewItem *item);
    void setSelected(int mode, const char *path);
    MyListViewItem *firstChild(MyListViewItem *parent);
    MyListViewItem *nextSibling(MyListViewItem *sibling, QTreeWidgetItem *parent);
    void addColumn(QString text, int size);
    void removeAllColumns();
    void setMultiSelection(int mode);
    void setSorting(int col, int mode);
    int hasStandardPopupMenu;

public slots:
    void slotClicked(QTreeWidgetItem *item, int column);
    void slotSendSelected();
    void slotRightButtonPressed(QTreeWidgetItem *, int i);
    void slotCustomContextMenuRequested(const QPoint &pos);

private:
    virtual void mousePressEvent(QMouseEvent * event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    void insertItem(MyListViewItem *item, MyListViewItem *parent, int num_slash);
    void closeTree(MyListViewItem *lvi, int mode);
    void nameVersionSetListViewText(const char *path, int column, QString &text, MyListViewItem *item, const char *relpath, int num_slash);
    void nameVersionSetListViewPixmap(const char *path, int column, QPixmap &pixmap, MyListViewItem *item, const char *relpath, int num_slash);
    void standardPopupMenu();
    int *s,id;
    int ichild[MAX_TREE_RECURSION],recursion,icol;
    int colwidth[20];
};

class MyListViewItem : public QTreeWidgetItem
{
public:
  QString path;
  MyListViewItem ( MyListView *parent) :
    QTreeWidgetItem(parent) {};
  MyListViewItem ( MyListViewItem *parent) :
    QTreeWidgetItem (parent) {};
  virtual ~MyListViewItem() {};
};

class MyIconView : public QListWidget //Q3IconView
{
    Q_OBJECT
public:
    MyIconView(int *sock, int ident, QWidget * parent=0, const char * name=0);
    ~MyIconView();
    virtual void setIconViewItem(QString &text, QPixmap &pixmap);
    virtual void deleteIconViewItem(QString &text);

public slots:
    void slotClicked(QListWidgetItem *);

private:
    virtual void mousePressEvent(QMouseEvent * event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    int *s,id;
};

class MyQDateEdit : public QDateEdit
{
    Q_OBJECT
public:
    MyQDateEdit(int *sock, int ident, QWidget *parent=0, const char *name=0);
    ~MyQDateEdit();
    int pvsVersion;

public slots:
   void slotValueChanged(const QDate &date);

private:
    virtual void mousePressEvent(QMouseEvent * event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    int *s,id;
};

class MyQTimeEdit : public QTimeEdit
{
    Q_OBJECT
public:
    MyQTimeEdit(int *sock, int ident, QWidget *parent=0, const char *name=0);
    ~MyQTimeEdit();
    int pvsVersion;

public slots:
   void slotValueChanged(const QTime &time);

private:
    virtual void mousePressEvent(QMouseEvent * event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    int *s,id;
};

class MyQDateTimeEdit : public QDateTimeEdit
{
    Q_OBJECT
public:
    MyQDateTimeEdit(int *sock, int ident, QWidget *parent=0, const char *name=0);
    ~MyQDateTimeEdit();
    int pvsVersion;

public slots:
   void slotValueChanged(const QDateTime &date_time);

private:
    virtual void mousePressEvent(QMouseEvent * event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    int *s,id;
};

class MyQDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    MyQDockWidget(QString &title, int *sock, int ident, int dock_id, QWidget *parent=0, const char *name=0);
    ~MyQDockWidget();
    int w, h;
    int x_original, y_original, w_original, h_original;

public slots:
   void slotTopLevelChanged(bool toplevel);

private:
    int *s,id,dock_id;
};

#ifdef PVB_FOOTPRINT_BASIC
#include "MyTextBrowser_without_www.h"
#else
#ifdef USE_GOOGLE_WEBKIT_FORK
#include "MyTextBrowser_with_www.h"
#else
#include "MyTextBrowser_v4.h"
#endif
#endif

#endif
