/* <St> *******************************************************************

  cif_types.h

  -------------------------------------------------------------------------
  CREATETED     : D. Tsaava, Hilscher GmbH
  DATE          : 18.07.2000
  PROJEKT       : CIF device driver
  =========================================================================

  DISCRIPTION

    Includefile for CIF device driver, DPM layout                 .

  =========================================================================

  CHANGES

  version name      date        Discription
                 March 2001
    		    Juli  2004     Redesigned for the 2.6 Kernel
  		                   Copyright changed to GNU Lesser GPL
-------------------------------------------------------------------------

  V2.601

  ======================== Copyright =====================================

  Copyright (C) 2004  Hilscher GmbH

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
   
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA

  =========================================================================
  ******************************************************************** <En> */
#ifndef CIF_TYPES_H
# define CIF_TYPES_H

/* Macros for debugging */
#undef DBG_PRN              /* undef it, just in case */
#ifdef CIF_DEBUG
# define CIF_PRN(function, lineno,fmt,args...) printk(fmt,function,lineno,##args)
# define DBG_PRN(fmt,args...) CIF_PRN((__FUNCTION__),(__LINE__),KERN_INFO __FILE__"::%s(L%.4d): "fmt,##args)
#else
#  define DBG_PRN(fmt, args...) /* not debugging: nothing */
#endif

#ifndef TRUE
# define TRUE  1
#endif

#ifndef FALSE
# define FALSE 0
#endif

#ifndef BOOL
# define BOOL  int
#endif

#endif /* CIF_TYPES_H */
