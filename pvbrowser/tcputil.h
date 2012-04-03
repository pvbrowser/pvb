/****************************************************************************
**
** Copyright (C) 2000-2006 Lehrig Software Engineering.
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
//extern "C" {
int tcp_init();
int tcp_free();
int tcp_con(const char *adr, int port);
int tcp_rec(int *s, char *msg, int len);
int tcp_rec_binary(int *s, char *msg, int len);
int tcp_send(int *s, const char *msg, int len);
int tcp_close(int *s);
int tcp_sleep(int milliseconds);
//}
