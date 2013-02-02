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

#if defined(_MSC_VER) /* MSVC Compiler */
#pragma warning ( disable : 4786 )
#endif

#include <qglobal.h>
#include <qaction.h>
#include <QtPlugin>
#include <QDesignerFormEditorInterface>
#include <QExtensionManager>
#include <QErrorMessage>
#include <QFrame>

#include "pvb_designer_plugin.h"
#include "pvbdummy.h"

using namespace PvbDesignerPlugin;

CustomWidgetInterface::CustomWidgetInterface(QObject *parent): 
    QObject(parent),
    d_isInitialized(false)
{
}

bool CustomWidgetInterface::isContainer() const
{
    return false;
}

bool CustomWidgetInterface::isInitialized() const
{
    return d_isInitialized;
}

QIcon CustomWidgetInterface::icon() const
{
    return d_icon;
}

QString CustomWidgetInterface::codeTemplate() const
{
    return d_codeTemplate;
}

QString CustomWidgetInterface::domXml() const
{
    return d_domXml;
}

QString CustomWidgetInterface::group() const
{
    return "Pvb Widgets";
}

QString CustomWidgetInterface::includeFile() const
{
    return d_include;
}

QString CustomWidgetInterface::name() const
{
    return d_name;
}

QString CustomWidgetInterface::toolTip() const
{
    return d_toolTip;
}

QString CustomWidgetInterface::whatsThis() const
{
    return d_whatsThis;
}

void CustomWidgetInterface::initialize(
    QDesignerFormEditorInterface *formEditor)
{
    if ( d_isInitialized )
        return;

    QExtensionManager *manager = formEditor->extensionManager();
    if ( manager )
    {
        manager->registerExtensions(new TaskMenuFactory(manager),
            Q_TYPEID(QDesignerTaskMenuExtension));
    }

    d_isInitialized = true;
}

DrawInterface::DrawInterface(QObject *parent): 
    CustomWidgetInterface(parent)
{
    d_name = "PvbDraw";
    d_include = "pvbdummy.h";
    d_icon = QPixmap(":/pixmaps/pvbdraw.png");
    d_domXml = 
        "<widget class=\"PvbDraw\" name=\"pvbDraw\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>200</width>\n"
        "   <height>100</height>\n"
        "  </rect>\n"
        " </property>\n"
        "</widget>\n";
}

QWidget *DrawInterface::createWidget(QWidget *parent)
{
    return new PvbDraw(parent);
}

ImageInterface::ImageInterface(QObject *parent): 
    CustomWidgetInterface(parent)
{
    d_name = "PvbImage";
    d_include = "pvbdummy.h";
    d_icon = QPixmap(":/pixmaps/pvbimage.png");
    d_domXml = 
        "<widget class=\"PvbImage\" name=\"pvbImage\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>200</width>\n"
        "   <height>100</height>\n"
        "  </rect>\n"
        " </property>\n"
        "</widget>\n";
}

QWidget *ImageInterface::createWidget(QWidget *parent)
{
    return new PvbImage(parent);
}

VtkInterface::VtkInterface(QObject *parent): 
    CustomWidgetInterface(parent)
{
    d_name = "PvbVtk";
    d_include = "pvbdummy.h";
    d_icon = QPixmap(":/pixmaps/pvbvtk.png");
    d_domXml = 
        "<widget class=\"PvbVtk\" name=\"pvbVtk\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>200</width>\n"
        "   <height>100</height>\n"
        "  </rect>\n"
        " </property>\n"
        "</widget>\n";
}

QWidget *VtkInterface::createWidget(QWidget *parent)
{
    return new PvbVtk(parent);
}

OpenglInterface::OpenglInterface(QObject *parent): 
    CustomWidgetInterface(parent)
{
    d_name = "PvbOpengl";
    d_include = "pvbdummy.h";
    d_icon = QPixmap(":/pixmaps/pvbopengl.png");
    d_domXml = 
        "<widget class=\"PvbOpengl\" name=\"pvbOpengl\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>200</width>\n"
        "   <height>100</height>\n"
        "  </rect>\n"
        " </property>\n"
        "</widget>\n";
}

QWidget *OpenglInterface::createWidget(QWidget *parent)
{
    return new PvbOpengl(parent);
}

IconviewInterface::IconviewInterface(QObject *parent): 
    CustomWidgetInterface(parent)
{
    d_name = "PvbIconview";
    d_include = "pvbdummy.h";
    d_icon = QPixmap(":/pixmaps/pvbiconview.png");
    d_domXml = 
        "<widget class=\"PvbIconview\" name=\"pvbIconview\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>200</width>\n"
        "   <height>100</height>\n"
        "  </rect>\n"
        " </property>\n"
        "</widget>\n";
}

QWidget *IconviewInterface::createWidget(QWidget *parent)
{
    return new PvbIconview(parent);
}

PvbCustomWidgetDumInterface::PvbCustomWidgetDumInterface(QObject *parent): 
    CustomWidgetInterface(parent)
{
    d_name = "PvbCustomWidget";
    d_include = "pvbdummy.h";
    d_icon = QPixmap(":/pixmaps/pvbiconview.png");
    d_domXml = 
        "<widget class=\"PvbCustomWidget\" name=\"pvbCustomWidget\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>200</width>\n"
        "   <height>100</height>\n"
        "  </rect>\n"
        " </property>\n"
        "</widget>\n";
}

QWidget *PvbCustomWidgetDumInterface::createWidget(QWidget *parent)
{
    return new PvbCustomWidget(parent);
}

CustomWidgetCollectionInterface::CustomWidgetCollectionInterface(   
        QObject *parent): 
    QObject(parent)
{
    d_plugins.append(new IconviewInterface(this));
    d_plugins.append(new DrawInterface(this));
    d_plugins.append(new ImageInterface(this));
    d_plugins.append(new VtkInterface(this));
    d_plugins.append(new OpenglInterface(this));
    d_plugins.append(new PvbCustomWidgetDumInterface(this));
}

QList<QDesignerCustomWidgetInterface*> 
    CustomWidgetCollectionInterface::customWidgets(void) const
{
    return d_plugins;
}

TaskMenuFactory::TaskMenuFactory(QExtensionManager *parent): 
    QExtensionFactory(parent)
{
}

QObject *TaskMenuFactory::createExtension(
    QObject *object, const QString &iid, QObject *parent) const
{
    if (iid == Q_TYPEID(QDesignerTaskMenuExtension))
    {
        //if (QDrawWidget *draw = qobject_cast<QDrawWidget*>(object))
        //    return new TaskMenuExtension(draw, parent);
        //...    
    }

    return QExtensionFactory::createExtension(object, iid, parent);
}


TaskMenuExtension::TaskMenuExtension(QWidget *widget, QObject *parent):
    QObject(parent),    
    d_widget(widget)
{
    d_editAction = new QAction(tr("Edit Pvb Attributes ..."), this);
    connect(d_editAction, SIGNAL(triggered()), 
        this, SLOT(editAttributes()));
}

QList<QAction *> TaskMenuExtension::taskActions() const
{
    QList<QAction *> list;
    list.append(d_editAction);
    return list;
}

QAction *TaskMenuExtension::preferredEditAction() const
{
    return d_editAction;
}

void TaskMenuExtension::editAttributes()
{
    static QErrorMessage *errorMessage = NULL;
    if ( errorMessage == NULL )
        errorMessage = new QErrorMessage();
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(PvbPlugin, CustomWidgetCollectionInterface)
#endif

