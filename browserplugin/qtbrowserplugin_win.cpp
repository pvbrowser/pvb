/****************************************************************************
**
** Copyright (C) 2003-2008 Trolltech ASA. All rights reserved.
**
** This file is part of a Qt Solutions component.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** Trolltech sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#include <QtGui>

#include "qtbrowserplugin.h"
#include "qtbrowserplugin_p.h"

#include <windows.h>
#include "qtnpapi.h"

static HHOOK hhook = 0;
static bool ownsqapp = false;
Q_GUI_EXPORT int qt_translateKeyCode(int);

LRESULT CALLBACK FilterProc( int nCode, WPARAM wParam, LPARAM lParam )
{
    if (qApp)
	qApp->sendPostedEvents(0, -1);

    if (nCode < 0 || !(wParam & PM_REMOVE))
        return CallNextHookEx(hhook, nCode, wParam, lParam);

    MSG *msg = (MSG*)lParam;
    bool processed = false;

    // (some) support for key-sequences via QAction and QShortcut
    if(msg->message == WM_KEYDOWN || msg->message == WM_SYSKEYDOWN) {
        QWidget *focusWidget = QWidget::find(msg->hwnd);
        if (focusWidget) {
            int key = msg->wParam;
            if (!(key >= 'A' && key <= 'Z') && !(key >= '0' && key <= '9'))
                key = qt_translateKeyCode(msg->wParam);

            Qt::KeyboardModifiers modifiers = 0;
            int modifierKey = 0;
            if (GetKeyState(VK_SHIFT) < 0) {
                modifierKey |= Qt::SHIFT;
                modifiers |= Qt::ShiftModifier;
            }
            if (GetKeyState(VK_CONTROL) < 0) {
                modifierKey |= Qt::CTRL;
                modifiers |= Qt::ControlModifier;
            }
            if (GetKeyState(VK_MENU) < 0) {
                modifierKey |= Qt::ALT;
                modifiers |= Qt::AltModifier;
            }
            QKeyEvent override(QEvent::ShortcutOverride, key, modifiers);
            override.ignore();
            QApplication::sendEvent(focusWidget, &override);
            processed = override.isAccepted();

            QKeySequence shortcutKey(modifierKey + key);
            if (!processed) {
                QList<QAction*> actions = qFindChildren<QAction*>(focusWidget->window());
                for (int i = 0; i < actions.count() && !processed; ++i) {
                    QAction *action = actions.at(i);
                    if (!action->isEnabled() || action->shortcut() != shortcutKey)
                        continue;
                    QShortcutEvent event(shortcutKey, 0);
                    processed = QApplication::sendEvent(action, &event);
                }
            }
            if (!processed) {
                QList<QShortcut*> shortcuts = qFindChildren<QShortcut*>(focusWidget->window());
                for (int i = 0; i < shortcuts.count() && !processed; ++i) {
                    QShortcut *shortcut = shortcuts.at(i);
                    if (!shortcut->isEnabled() || shortcut->key() != shortcutKey)
                        continue;
                    QShortcutEvent event(shortcutKey, shortcut->id());
                    processed = QApplication::sendEvent(shortcut, &event);
                }
            }
        }
    }

    return CallNextHookEx(hhook, nCode, wParam, lParam);
}

extern "C" bool qtns_event(QtNPInstance *, NPEvent *)
{
    return false;
}

extern Q_CORE_EXPORT void qWinMsgHandler(QtMsgType t, const char* str);

extern "C" void qtns_initialize(QtNPInstance*)
{
    if (!qApp) {
        qInstallMsgHandler(qWinMsgHandler);
        ownsqapp = true;
	static int argc=0;
	static char **argv={ 0 };
	(void)new QApplication(argc, argv);

        QT_WA({
	    hhook = SetWindowsHookExW( WH_GETMESSAGE, FilterProc, 0, GetCurrentThreadId() );
        }, {
	    hhook = SetWindowsHookExA( WH_GETMESSAGE, FilterProc, 0, GetCurrentThreadId() );
        });
    }
}

extern "C" void qtns_destroy(QtNPInstance *)
{
}

extern "C" void qtns_shutdown()
{
    if (!ownsqapp)
        return;

    // check if qApp still runs widgets (in other DLLs)
    QWidgetList widgets = qApp->allWidgets();
    int count = widgets.count();
    for (int w = 0; w < widgets.count(); ++w) {
        // ignore all Qt generated widgets
        QWidget *widget = widgets.at(w);
        if (widget->windowFlags() & Qt::Desktop)
            count--;
    }
    if (count) // qApp still used
        return;

    delete qApp;
    ownsqapp = false;
    if ( hhook )
        UnhookWindowsHookEx( hhook );
    hhook = 0;
}

extern "C" void qtns_embed(QtNPInstance *This)
{
    Q_ASSERT(qobject_cast<QWidget*>(This->qt.object));

    LONG oldLong = GetWindowLong(This->window, GWL_STYLE);
    ::SetWindowLong(This->window, GWL_STYLE, oldLong | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
    ::SetWindowLong(This->qt.widget->winId(), GWL_STYLE, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
    ::SetParent(This->qt.widget->winId(), This->window);
}

extern "C" void qtns_setGeometry(QtNPInstance *This, const QRect &rect, const QRect &)
{
    Q_ASSERT(qobject_cast<QWidget*>(This->qt.object));

    This->qt.widget->setGeometry(QRect(0, 0, rect.width(), rect.height()));
}

/*
extern "C" void qtns_print(QtNPInstance * This, NPPrint *printInfo)
{
    NPWindow* printWindow = &(printInfo->print.embedPrint.window);
    void* platformPrint = printInfo->print.embedPrint.platformPrint;
    // #### Nothing yet.
}
*/
