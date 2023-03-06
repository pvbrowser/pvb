/********************************************************************************
** Form generated from reading ui file 'dlgmybrowser.ui'
**
** Created: Do Mai 8 07:36:18 2008
**      by: Qt User Interface Compiler version 4.3.1
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef DLGMYBROWSER_UI_H
#define DLGMYBROWSER_UI_H
//#ifndef NO_WEBKIT

#include <QWebEngineView>
#include <QtCore/QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

class pvQWebView : public QWebEngineView
{
public:
  pvQWebView(QWidget *parent);
  ~pvQWebView();
  virtual bool event(QEvent *e);
protected:
  virtual QWebEngineView *createWindow(QWebEnginePage::WebWindowType type);
private:
  virtual void keyPressEvent(QKeyEvent *event);
  virtual void mousePressEvent(QMouseEvent *event);
  QPoint pressPos;
  float factor;
};

class Ui_MyBrowser
{
public:
    QHBoxLayout *hboxLayout;
    QVBoxLayout *vboxLayout;
    QGroupBox *groupBoxNavigation;
    QPushButton *pushButtonBack;
    QPushButton *pushButtonHome;
    QPushButton *pushButtonForward;
    QPushButton *pushButtonReload;
    QLineEdit   *lineEditPattern;
    QPushButton *pushButtonFind;
//#ifdef USE_WEBKIT
    pvQWebView *browser;
//#else
//    QLabel   *browser;
//#endif

    void setupUi(QWidget *MyBrowser)
    {
    if (MyBrowser->objectName().isEmpty())
        MyBrowser->setObjectName(QString::fromUtf8("MyBrowser"));
    MyBrowser->resize(502, 397);
    hboxLayout = new QHBoxLayout(MyBrowser);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    vboxLayout = new QVBoxLayout();
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    groupBoxNavigation = new QGroupBox(MyBrowser);
    groupBoxNavigation->setObjectName(QString::fromUtf8("groupBoxNavigation"));
    groupBoxNavigation->setMinimumSize(QSize(0, 28));
    groupBoxNavigation->setMaximumSize(QSize(16777215, 28));
    pushButtonBack = new QPushButton(groupBoxNavigation);
    pushButtonBack->setObjectName(QString::fromUtf8("pushButtonBack"));
    pushButtonBack->setGeometry(QRect(10, 3, 75, 26));
    pushButtonBack->setIcon(QIcon(QString::fromUtf8(":/images/back.png")));
    pushButtonHome = new QPushButton(groupBoxNavigation);
    pushButtonHome->setObjectName(QString::fromUtf8("pushButtonHome"));
    pushButtonHome->setGeometry(QRect(90, 3, 75, 26));
    pushButtonHome->setIcon(QIcon(QString::fromUtf8(":/images/gohome.png")));
    pushButtonForward = new QPushButton(groupBoxNavigation);
    pushButtonForward->setObjectName(QString::fromUtf8("pushButtonForward"));
    pushButtonForward->setGeometry(QRect(170, 3, 75, 26));
    pushButtonForward->setIcon(QIcon(QString::fromUtf8(":/images/reconnect.png")));
    pushButtonReload = new QPushButton(groupBoxNavigation);
    pushButtonReload->setObjectName(QString::fromUtf8("pushButtonReload"));
    pushButtonReload->setGeometry(QRect(250, 3, 75, 26));
    pushButtonReload->setIcon(QIcon(QString::fromUtf8(":/images/reload.png")));
    lineEditPattern = new QLineEdit(groupBoxNavigation);
    lineEditPattern->setObjectName(QString::fromUtf8("lineEditPattern"));
    lineEditPattern->setGeometry(QRect(330, 3, 75, 26));
    pushButtonFind = new QPushButton(groupBoxNavigation);
    pushButtonFind->setObjectName(QString::fromUtf8("pushButtonFind"));
    pushButtonFind->setGeometry(QRect(410, 3, 75, 26));
    pushButtonFind->setIcon(QIcon(QString::fromUtf8(":/images/find.png")));

    vboxLayout->addWidget(groupBoxNavigation);

//#ifdef USE_WEBKIT
    browser = new pvQWebView(MyBrowser);
    browser->setObjectName(QString::fromUtf8("browser"));
//#else
//    browser = new QLabel(MyBrowser);
//    browser->setObjectName(QString::fromUtf8("browser"));
//    browser->setAlignment(Qt::AlignCenter);
//#endif

    vboxLayout->addWidget(browser);


    hboxLayout->addLayout(vboxLayout);


    retranslateUi(MyBrowser);

    QMetaObject::connectSlotsByName(MyBrowser);
    } // setupUi

    void retranslateUi(QWidget *MyBrowser)
    {
    MyBrowser->setWindowTitle(QObject::tr("Form"));
    groupBoxNavigation->setTitle(QObject::tr(""));
    pushButtonBack->setToolTip(QObject::tr("Back"));
    pushButtonBack->setText(QObject::tr("Back"));
    pushButtonHome->setToolTip(QObject::tr("Home"));
    pushButtonHome->setText(QObject::tr("Home"));
    pushButtonForward->setToolTip(QObject::tr("Forward"));
    pushButtonForward->setText(QObject::tr("Forward"));
    pushButtonReload->setText(QObject::tr("Reload"));
    pushButtonFind->setText(QObject::tr("Find"));
//#ifndef USE_WEBKIT
//    browser->setText(QObject::tr("KHTML/WebKit is only available on<br>QT_VERSION >= 4.4.0"));
//#endif
    Q_UNUSED(MyBrowser);
    } // retranslateUi

};

namespace Ui {
    class MyBrowser: public Ui_MyBrowser {};
} // namespace Ui

//#endif
#endif // DLGMYBROWSER_UI_H
