/***************************************************************************
 *                                                                         *
 *                              (C) IBM Corp.                              *
 *                                                                         *
 * File:          CR_ALL.H                                                 *
 *                                                                         *
 * Description:   INTeL 8086/88 interpreter.                               *
 *                                                                         *
 *                Include file for all CodeRunner related sources.         *
 *                                                                         *
 *      Must be compiled with /dBIG_ENDIAN for big-endian machines.        *
 *      Must be compiled with /dAIX        for AIX.                        *
 *      Must be compiled with /dDOS        for DOS.                        *
 *      Must be compiled with /dOS2_16     for 16-bit OS2.                 *
 *      Must be compiled with /dOS2_32     for 32-bit OS2.                 *
 *      Must be compiled with /dWNT_32     for 32-bit Windows NT.          *
 *      Must be compiled with /dW95_32     for 32-bit Windows NT.          *
 *                                                                         *
 ***************************************************************************/


#ifndef  _CR_ALL_H_
#define  _CR_ALL_H_


/* Safe measure, so that makefile will have a definition for operating
 * system. If none is defined - following text line will produce error.
 */
#ifndef  AIX
   #ifndef  DOS
      #ifndef  OS2_16
         #ifndef  OS2_32
            #ifndef WNT_32
               #ifndef W95_32
                  #error ERROR: AIX or DOS or OS2_16 or OS2_32 or WNT_32 or W95_32 must be defined.
               #endif
            #endif
         #endif
      #endif
   #endif
#endif

#if S_H
#include "mymalloc.h"
#endif

/* if CR_DEBUG is defined - all CodeRunner related modules will produce
 * some debug output (in form of "printf"-s and/or similar.)
 */
/*#define CR_DEBUG*/
/*#define CR_DBGTF */

#ifdef CR_DEBUG
#include <stdio.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

/* All local header files to be included for CodeRunner.
 */
#ifdef   GE_MACHINE
   #include "dirlevel.dir"

   #ifdef   DIR_LEVEL_0
      #if defined(OS2_16DB)
         #include "dbmalloc.h"
      #endif
      #include "cr_defs.h"    /* <- This case to be used in GE machine.    */
      #include "cr_exts.h"
      #include "ge_local.pro"
   #endif

   #ifdef   DIR_LEVEL_1
      #if defined(OS2_16DB)
         #include "..\dbmalloc.h"
      #endif
      #include "..\cr_defs.h"
      #include "..\cr_exts.h"
      #include "..\ge_local.pro"
   #endif

#else                         /* <- This case to be used in IBM.           */
   #if defined(OS2_16DB)
      #include "dbmalloc.h"
   #endif
   #include "cr_defs.h"
   #include "cr_exts.h"
#endif

//riad//
#include <stdio.h>
#include <time.h>
#endif   /* _CR_ALL_H_  */


