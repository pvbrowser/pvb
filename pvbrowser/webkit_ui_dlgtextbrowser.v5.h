/********************************************************************************
** Form generated from reading ui file 'dlgtextbrowser.ui'
**
** Created: Thu Aug 13 13:49:03 2009
**      by: Qt User Interface Compiler version 4.4.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_DLGTEXTBROWSER_H
#define UI_DLGTEXTBROWSER_H

#include <QtCore/QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QDialog>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QSpacerItem>
#ifdef USE_ANDROID
#define FOOTPRINT_OHNE
#include <QTextBrowser>
#else

#ifdef PVB_FOOTPRINT_BASIC 
#undef  FOOTPRINT_OHNE
#define FOOTPRINT_OHNE
#include <QTextBrowser>
#else
#include <QWebEngineView>
#endif

#endif

#include <QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_DialogTextBrowser
{
public:
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout;
    QPushButton *pushButtonBack;
    QPushButton *pushButtonHome;
    QLineEdit   *lineEditPattern;
    QPushButton *pushButtonFind;
    QSpacerItem *spacerItem;
    QPushButton *pushButtonClose;
#ifdef FOOTPRINT_OHNE
    QTextBrowser   *textBrowser;
#else    
    QWebEngineView *textBrowser;
#endif    

    void setupUi(QDialog *DialogTextBrowser)
    {
    if (DialogTextBrowser->objectName().isEmpty())
        DialogTextBrowser->setObjectName(QString::fromUtf8("DialogTextBrowser"));
    DialogTextBrowser->resize(641, 563);
    vboxLayout = new QVBoxLayout(DialogTextBrowser);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    hboxLayout = new QHBoxLayout();
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    pushButtonBack = new QPushButton(DialogTextBrowser);
    pushButtonBack->setObjectName(QString::fromUtf8("pushButtonBack"));
    const QIcon icon = QIcon(QString::fromUtf8(":/images/back.png"));
    pushButtonBack->setIcon(icon);

    hboxLayout->addWidget(pushButtonBack);

    pushButtonHome = new QPushButton(DialogTextBrowser);
    pushButtonHome->setObjectName(QString::fromUtf8("pushButtonHome"));
    const QIcon icon1 = QIcon(QString::fromUtf8(":/images/gohome.png"));
    pushButtonHome->setIcon(icon1);

    hboxLayout->addWidget(pushButtonHome);

    lineEditPattern = new QLineEdit(DialogTextBrowser);
    lineEditPattern->setObjectName(QString::fromUtf8("lineEditPattern"));
    
    hboxLayout->addWidget(lineEditPattern);

    pushButtonFind = new QPushButton(DialogTextBrowser);
    pushButtonFind->setObjectName(QString::fromUtf8("pushButtonFind"));
    const QIcon icon2 = QIcon(QString::fromUtf8(":/images/find.png"));
    pushButtonFind->setIcon(icon2);

    hboxLayout->addWidget(pushButtonFind);

    spacerItem = new QSpacerItem(211, 26, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem);

    pushButtonClose = new QPushButton(DialogTextBrowser);
    pushButtonClose->setObjectName(QString::fromUtf8("pushButtonClose"));

    hboxLayout->addWidget(pushButtonClose);


    vboxLayout->addLayout(hboxLayout);

#ifdef FOOTPRINT_OHNE
    textBrowser = new QTextBrowser(DialogTextBrowser);
#else    
    textBrowser = new QWebEngineView(DialogTextBrowser);
#endif
    textBrowser->setObjectName(QString::fromUtf8("textBrowser"));

    vboxLayout->addWidget(textBrowser);


    retranslateUi(DialogTextBrowser);
    QObject::connect(pushButtonClose, SIGNAL(clicked()), DialogTextBrowser, SLOT(accept()));

    QMetaObject::connectSlotsByName(DialogTextBrowser);
    } // setupUi

    void retranslateUi(QDialog *DialogTextBrowser)
    {
    DialogTextBrowser->setWindowTitle(QObject::tr("pvdevelop Manual"));
    pushButtonBack->setText(QObject::tr("&Back"));
    pushButtonHome->setText(QObject::tr("&Home"));
    pushButtonFind->setText(QObject::tr("&Find"));
    pushButtonClose->setText(QObject::tr("&Close"));
    Q_UNUSED(DialogTextBrowser);
    } // retranslateUi

};

namespace Ui {
    class DialogTextBrowser: public Ui_DialogTextBrowser {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGTEXTBROWSER_H
