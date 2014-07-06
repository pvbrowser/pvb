/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of a Qt Solutions component.
**
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
****************************************************************************/

#include "pvdefine.h"

#include <QtGui>
#include "qtnpapi.h"
#include <qlogging.h>

#include "qtbrowserplugin.h"
#include "qtbrowserplugin_p.h"

#include <windows.h>
// rl qt4->qt5 #include "qtnpapi.h"

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
        QWidget *focusWidget = QWidget::find((unsigned int) msg->hwnd);
        if (focusWidget) {
            int key = msg->wParam;
            if (!(key >= 'A' && key <= 'Z') && !(key >= '0' && key <= '9'))
#if QT_VERSION < 0x050000
                key = qt_translateKeyCode(msg->wParam);
#else
                key = (int) msg->wParam;
#endif

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
            QKeySequence shortcutKey(modifierKey + key);
            if (!shortcutKey.isEmpty()) {
                QKeyEvent override(QEvent::ShortcutOverride, key, modifiers);
                override.ignore();
                QApplication::sendEvent(focusWidget, &override);
                processed = override.isAccepted();

                if (!processed) {
#if QT_VERSION < 0x050000
                    QList<QAction*> actions = qFindChildren<QAction*>(focusWidget->window());
#else
                    QList<QAction*> actions = focusWidget->window()->findChildren<QAction*>();
#endif
                    for (int i = 0; i < actions.count() && !processed; ++i) {
                        QAction *action = actions.at(i);
                        if (!action->isEnabled() || action->shortcut() != shortcutKey)
                            continue;
                        QShortcutEvent event(shortcutKey, 0);
                        processed = QApplication::sendEvent(action, &event);
                    }
                }
                if (!processed) {
#if QT_VERSION < 0x050000
                    QList<QShortcut*> shortcuts = qFindChildren<QShortcut*>(focusWidget->window());
#else
                    QList<QShortcut*> shortcuts = focusWidget->window()->findChildren<QShortcut*>();
#endif
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
    if (!qApp) 
    {
#if QT_VERSION < 0x050000
        qInstallMsgHandler(qWinMsgHandler);
        ownsqapp = true;
	static int argc=0;
	static char **argv={ 0 };
	(void)new QApplication(argc, argv);

        QT_WA({ hhook = SetWindowsHookExW( WH_GETMESSAGE, FilterProc, 0, GetCurrentThreadId() );    }, 
              { hhook = SetWindowsHookExA( WH_GETMESSAGE, FilterProc, 0, GetCurrentThreadId() );    }); 
#else
        ownsqapp = true;
	static int argc=0;
	static char **argv={ 0 };
	(void)new QApplication(argc, argv);
        
  //hhook = SetWindowsHookExW( WH_GETMESSAGE, FilterProc, 0, GetCurrentThreadId() );     
  //hhook = SetWindowsHookExA( WH_GETMESSAGE, FilterProc, 0, GetCurrentThreadId() );     
  hhook   = SetWindowsHookEx(  WH_GETMESSAGE, FilterProc, 0, GetCurrentThreadId() );     
#endif
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
    ::SetWindowLong((HWND) This->qt.widget->winId(), GWL_STYLE, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
    ::SetParent((HWND) This->qt.widget->winId(), This->window);
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
