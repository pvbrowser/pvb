/***************************************************************************
                          rlssl.cpp  -  description
                             -------------------
    begin                : Thu Oct 16 2014
    copyright            : (C) 2014 by R. Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#include "rlsocket.h"

typedef struct
{
  SSL     *sslStream;
  SSL_CTX *sslContext;
}rlssl_struct;

// inspired by inkscape socket.cpp
static void cryptoLockCallback(int mode, int type, const char *file, int line)
{
  static int modes[CRYPTO_NUM_LOCKS]; /* = {0, 0, ... } */
  if(type < 0 || type >= CRYPTO_NUM_LOCKS)
  {
    printf("cryptoLockCallback::type out of bounds\n");
    return;
  }

  int rw = mode & (CRYPTO_READ|CRYPTO_WRITE);
  if(!((rw == CRYPTO_READ) || (rw == CRYPTO_WRITE)))
  {
    printf("cryptoLockCallback::invalid mode\n");
    return;
  }
  
  if(mode & CRYPTO_LOCK)
  {
    if(modes[type])
    {
      errstr = "already locked";
            /* must not happen in a single-threaded program
             * (would deadlock)
             */
      goto err;
    }
    modes[type] = rw;
  }
  else if (mode & CRYPTO_UNLOCK)
  {
    if(!modes[type])
    {
       errstr = "not locked";
       goto err;
    }
    if(modes[type] != rw)
    {
       errstr = (rw == CRYPTO_READ) ?
       "CRYPTO_r_unlock on write lock" :
       "CRYPTO_w_unlock on read lock";
    }
    modes[type] = 0;
  }
}

int rlssl_init(rlssl_struct *rlssl)
{
  rlssl->sslStream  = NULL;
  rlssl->sslContext = NULL;
  CRYPTO_set_locking_callback(cryptoLockCallback);
  CRYPTO_set_id_callback(cryptoIdCallback);
  SSL_library_init();
  SSL_load_error_strings();
  return 0;
}

int rlssl_starttls(rlssl_struct *rlssl)
{
  rlssl->sslStream  = NULL;
  rlssl->sslContext = NULL;
  //SSL_METHOD *meth = SSLv23_method();
  //SSL_METHOD *meth = SSLv3_client_method();
  SSL_METHOD *meth = TLSv1_client_method();
  rlssl->sslContext = SSL_CTX_new(meth);
  //SSL_CTX_set_info_callback(rlssl->sslContext, infoCallback);

  /* Connect the SSL socket */
  rlssl->sslStream  = SSL_new(rlssl->sslContext);
  SSL_set_fd(rlssl->sslStream, sock);
  if(SSL_connect(rlssl->sslStream)<=0)
  {
    //fprintf(stderr, "SSL connect error\n");
    //disconnect();
    return -1;
  }
  return 0;
}

int rlssl_disconnect(rlssl_struct *rlssl)
{
  int ret = SSL_shutdown(rlssl->sslStream);
  if(ret <= 0) ret = -1; // error
  SSL_free(rlssl->sslStream);
  if(rlssl->sslContext)
  {
    SSL_CTX_free(rlssl->sslContext);
  }
  rlssl->sslStream  = NULL;
  rlssl->sslContext = NULL;
  return ret;
}

int rlssl_available(rlssl_struct *rlssl)
{
  int count = SSL_pending(rlssl->sslStream);
  return count;
}

int rlssl_write(rlssl_struct *rlssl, int ch)
{
  unsigned char c = (unsigned char) ch;
  return SSL_write(rlssl->sslStream, &c, 1);
}

int rlssl_write(rlssl_struct *rlssl, const char *text)
{
  return SSL_write(rlssl->sslStream, text, strlen(text));
}

int rlssl_read(rlssl_struct *rlssl)
{
  unsigned char ch;
  int ret = SSL_read(rlssl->sslStream, &ch, 1);
  if(ret <= 0)
  {
    printf("rlssl_read: ERROR SSL_read() ret=%d SSL_get_error=%d\n", ret, SSL_get_error(rlssl->sslStream,ret));
    return ret - 1;
  }
  return (int) ch;
}
