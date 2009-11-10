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

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

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
#ifdef USE_WEBKIT
    QWebView *browser;
#else
    QLabel   *browser;
#endif

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
    groupBoxNavigation->setMinimumSize(QSize(0, 61));
    groupBoxNavigation->setMaximumSize(QSize(16777215, 61));
    pushButtonBack = new QPushButton(groupBoxNavigation);
    pushButtonBack->setObjectName(QString::fromUtf8("pushButtonBack"));
    pushButtonBack->setGeometry(QRect(10, 20, 75, 26));
    pushButtonBack->setIcon(QIcon(QString::fromUtf8(":/images/back.png")));
    pushButtonHome = new QPushButton(groupBoxNavigation);
    pushButtonHome->setObjectName(QString::fromUtf8("pushButtonHome"));
    pushButtonHome->setGeometry(QRect(90, 20, 75, 26));
    pushButtonHome->setIcon(QIcon(QString::fromUtf8(":/images/gohome.png")));
    pushButtonForward = new QPushButton(groupBoxNavigation);
    pushButtonForward->setObjectName(QString::fromUtf8("pushButtonForward"));
    pushButtonForward->setGeometry(QRect(170, 20, 75, 26));
    pushButtonForward->setIcon(QIcon(QString::fromUtf8(":/images/reconnect.png")));
    pushButtonReload = new QPushButton(groupBoxNavigation);
    pushButtonReload->setObjectName(QString::fromUtf8("pushButtonReload"));
    pushButtonReload->setGeometry(QRect(250, 20, 75, 26));
    pushButtonReload->setIcon(QIcon(QString::fromUtf8(":/images/reload.png")));

    vboxLayout->addWidget(groupBoxNavigation);

#ifdef USE_WEBKIT
    browser = new QWebView(MyBrowser);
    browser->setObjectName(QString::fromUtf8("browser"));
#else
    browser = new QLabel(MyBrowser);
    browser->setObjectName(QString::fromUtf8("browser"));
    browser->setAlignment(Qt::AlignCenter);
#endif

    vboxLayout->addWidget(browser);


    hboxLayout->addLayout(vboxLayout);


    retranslateUi(MyBrowser);

    QMetaObject::connectSlotsByName(MyBrowser);
    } // setupUi

    void retranslateUi(QWidget *MyBrowser)
    {
    MyBrowser->setWindowTitle(QApplication::translate("MyBrowser", "Form", 0, QApplication::UnicodeUTF8));
    groupBoxNavigation->setTitle(QApplication::translate("MyBrowser", "Navigation", 0, QApplication::UnicodeUTF8));
    pushButtonBack->setToolTip(QApplication::translate("MyBrowser", "Back", 0, QApplication::UnicodeUTF8));
    pushButtonBack->setText(QApplication::translate("MyBrowser", "Back", 0, QApplication::UnicodeUTF8));
    pushButtonHome->setToolTip(QApplication::translate("MyBrowser", "Home", 0, QApplication::UnicodeUTF8));
    pushButtonHome->setText(QApplication::translate("MyBrowser", "Home", 0, QApplication::UnicodeUTF8));
    pushButtonForward->setToolTip(QApplication::translate("MyBrowser", "Forward", 0, QApplication::UnicodeUTF8));
    pushButtonForward->setText(QApplication::translate("MyBrowser", "Forward", 0, QApplication::UnicodeUTF8));
    pushButtonReload->setText(QApplication::translate("MyBrowser", "Reload", 0, QApplication::UnicodeUTF8));
#ifndef USE_WEBKIT
    browser->setText(QApplication::translate("MyBrowser", "KHTML/WebKit is only available on<br>QT_VERSION >= 4.4.0", 0, QApplication::UnicodeUTF8));
#endif
    Q_UNUSED(MyBrowser);
    } // retranslateUi

};

namespace Ui {
    class MyBrowser: public Ui_MyBrowser {};
} // namespace Ui

#endif // DLGMYBROWSER_UI_H
