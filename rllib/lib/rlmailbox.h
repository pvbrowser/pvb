/***************************************************************************
                          rlmailbox.h  -  description
                             -------------------
    begin                : Tue Jan 02 2001
    copyright            : (C) 2001 by R. Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#ifndef _RL_MAILBOX_H_
#define _RL_MAILBOX_H_

#include "rldefine.h"

/*! <pre>
class for a mailbox.

A mailbox is for communication between processes on the same computer.
It works like a fifo.
Many processes can write to 1 mailbox.
Only 1 reader is allowed.
</pre> */
class rlMailbox
{
public:
  enum MailboxEnum
  {
    /// return codes for write()
    MAILBOX_ERROR            = -1,
    MAILBOX_FULL             = -2,
    /// wait parameter for read()
    WAIT                     = 1,
    NOWAIT                   = 0,
    /// maximum mailbox length
    MAX_MAILBOX              = 256*256,
    /// status:
    OK                       = 2,
    COULD_NOT_CREATE_MAILBOX = 3,
    COULD_NOT_GET_KEY        = 4,
    COULD_NOT_GET_CHAN_ID    = 5
  };

  /*! <pre>
  construct a named mailbox
  </pre> */
  rlMailbox(const char *name);

  /*! <pre>
  destruct the mailbox
  </pre> */
  virtual ~rlMailbox();

  /*! <pre>
  write buf with length len to mailbox
  return: bytes written
          MAILBOX_ERROR
          MAILBOX_FULL
  </pre> */
  int write(const void *buf, int len);

  /*! <pre>
  similar to printf
  return: bytes written
          MAILBOX_ERROR
          MAILBOX_FULL
  </pre> */
  int printf(const char *format, ...);

  /*! <pre>
  read buf from mailbox, maxlen=Maximal length of buf
  return: number of bytes read
  wait = 0 no wait
  wait = 1 wait for message
  </pre> */
  int read(void *buf, int maxlen, int wait=WAIT);

  /*! <pre>
  set the size of buffer for "const char *read(int wait)"
  </pre> */
  int setReadBufferSize(int size);

  /*! <pre>
  read buffer from mailbox
  return: buffer | ""
  wait = 0 no wait
  wait = 1 wait for message
  </pre> */
  const char *read(int wait=WAIT);

  /*! <pre>
  write message to mailbox
  return: bytes written
          MAILBOX_ERROR
          MAILBOX_FULL
  </pre> */
  int write(const char *message);

  /*! <pre>
  read all messages from mailbox, clear them
  </pre> */
  void clear();

  /*! <pre>
  should be: OK
  can be:    COULD_NOT_CREATE_MAILBOX
             COULD_NOT_GET_CHAN_ID
  </pre> */
  int status;

  /*! <pre>
  Name of mailbox
  </pre> */
  char *name;

private:
  int chanid;
  int buffer_size;
  char *buffer;
};

#endif
