/****************************************************************************
**
** Copyright (C) 2000-2008 Lehrig Software Engineering.
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
#ifndef PVB_DESIGNER_PLUGIN_H
#define PVB_DESIGNER_PLUGIN_H

#include <qglobal.h>
//#include <QDesignerCustomWidgetInterface>
#include <QtUiPlugin/QDesignerCustomWidgetInterface>
#include <QDesignerTaskMenuExtension>
#include <QExtensionFactory>

namespace PvbDesignerPlugin
{

class CustomWidgetInterface: public QObject, 
    public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    CustomWidgetInterface(QObject *parent);

    virtual bool isContainer() const;
    virtual bool isInitialized() const;
    virtual QIcon icon() const;
    virtual QString codeTemplate() const;
    virtual QString domXml() const;
    virtual QString group() const;
    virtual QString includeFile() const;
    virtual QString name() const;
    virtual QString toolTip() const;
    virtual QString whatsThis() const;
    virtual void initialize(QDesignerFormEditorInterface *);

protected:
    QString d_name; 
    QString d_include; 
    QString d_toolTip; 
    QString d_whatsThis; 
    QString d_domXml; 
    QString d_codeTemplate;
    QIcon d_icon;

private:
    bool d_isInitialized;
};

class CustomWidgetCollectionInterface: public QObject,
    public QDesignerCustomWidgetCollectionInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QDesignerCustomWidgetCollectionInterface" )
#endif

public:
    CustomWidgetCollectionInterface(QObject *parent = NULL);

    virtual QList<QDesignerCustomWidgetInterface*> customWidgets() const;

private:
    QList<QDesignerCustomWidgetInterface*> d_plugins;
};

class DrawInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    DrawInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};

class ImageInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    ImageInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};

class OpenglInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    OpenglInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};

class VtkInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    VtkInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};

class IconviewInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    IconviewInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};

class PvbCustomWidgetDumInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    PvbCustomWidgetDumInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};

class TaskMenuFactory: public QExtensionFactory
{
    Q_OBJECT

public:
    TaskMenuFactory(QExtensionManager *parent = 0);

protected:
    QObject *createExtension(QObject *object,
        const QString &iid, QObject *parent) const;
};

class TaskMenuExtension: public QObject,
            public QDesignerTaskMenuExtension
{
    Q_OBJECT
    Q_INTERFACES(QDesignerTaskMenuExtension)

public:
    TaskMenuExtension(QWidget *widget, QObject *parent);

    QAction *preferredEditAction() const;
    QList<QAction *> taskActions() const;

private slots:
    void editAttributes();

private:
    QAction *d_editAction;
    QWidget *d_widget;
};

};
#endif
