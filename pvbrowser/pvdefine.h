#ifndef _PV_DEFINE_H_
#define _PV_DEFINE_H_

// define unix if not already defined
#ifdef __unix__
#ifndef unix
#define unix
#endif
#endif
#ifdef unix
#define PVUNIX
#endif

// define WIN
#ifdef _WIN32
#define PVWIN32
#endif

// Symbian
#ifdef USE_SYMBIAN
#include <stdapis/sys/select.h>
#include <stdapis/locale.h>
#include <stdapis/arpa/inet.h>
#ifndef PVUNIX
#define PVUNIX
#endif
#endif

#include <QtCore>
#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif

#endif

