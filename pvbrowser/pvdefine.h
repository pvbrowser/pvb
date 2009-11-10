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

#endif

