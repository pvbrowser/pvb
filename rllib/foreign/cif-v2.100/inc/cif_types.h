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
  -------------------------------------------------------------------------

  V2.000

  ======================== Copyright =====================================
  Complete package is copyrighted by Hilscher GmbH and is licensed through 
  the GNU General Public License. 
  You should have received a copy of the GNU Library General Public
  License along with this package; if not, please refer to www.gnu.org
  ========================================================================
  ******************************************************************** <En> */
#ifndef CIF_TYPES_H
# define CIF_TYPES_H

/* Macros to help debugging */
#undef DBG_PRN              /* undef it, just in case */
#ifdef CIF_DEBUG

# define CIF_PRN(lineno,fmt,args...) printk(fmt,lineno,##args)
# define DBG_PRN(fmt,args...) CIF_PRN((__LINE__),KERN_INFO __FILE__"::"__FUNCTION__"(L%.4d): "fmt,##args)
//#  define DBG_PRN(fmt, args...) printk( KERN_DEBUG "cif: " fmt, ## args)
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
