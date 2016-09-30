////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/SSBSTYPE.H_v   1.8   06 Nov 1998 16:30:30   rpulint  $
//
// Description:
//  Structured storage base type definitions.
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/SSBSTYPE.H_v  $
// 
//    Rev 1.8   06 Nov 1998 16:30:30   rpulint
// Added changes for OS/2 Platform
// 
//    Rev 1.7   03 Sep 1997 17:20:42   DCHI
// Added #ifndef for __MACINTOSH__.
// 
//    Rev 1.6   09 Jun 1997 14:47:10   DCHI
// Redefinition of some base types cause problems under SYM_NLM.
// 
//    Rev 1.5   09 Jun 1997 14:40:08   DCHI
// Removed typedef of LPTSTR.
// 
//    Rev 1.4   09 Jun 1997 14:10:08   DCHI
// Reworked pack pragmas to work on UNIX gcc which doesn't replace tokens
// within pragmas.
// 
//    Rev 1.3   06 Jun 1997 17:37:14   CFORMUL
// Added PACK defines.
// 
//    Rev 1.2   28 Apr 1997 18:39:32   DCHI
// Added #include of platform.h.
// 
//    Rev 1.1   28 Apr 1997 18:09:28   DCHI
// Modified #define of FAR.
// 
//    Rev 1.0   23 Apr 1997 15:10:40   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _SSBSTYPE_H_

#define _SSBSTYPE_H_

#include <stdio.h>
#include <stdlib.h>

//#include "platform.h"

#ifndef __MACINTOSH__

#ifndef FAR
    #if defined(SYM_UNIX) || defined(__MACINTOSH__) || defined(BORLAND)
        #define FAR
    #endif
#endif

// Base types

// PR1023
//#ifndef SYM_NLM
#if !defined(SYM_NLM) && !defined(SYM_OS2) && \
    !defined(SYM_AIX) && !defined(SYM_SOL) && \
    !defined(SYM_400) && !defined(SYM_LIN)
typedef int             BOOL;
typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned long   DWORD;
typedef signed char     SBYTE;
typedef signed short    SWORD;
typedef signed long     SDWORD;
typedef unsigned int    UINT;
typedef long            LONG;
typedef WORD FAR        *LPWORD;
typedef int FAR         *LPINT;
typedef char FAR        *LPSTR;
typedef BYTE FAR        *LPBYTE;
typedef void FAR        *LPVOID;
typedef DWORD FAR       *LPDWORD;
typedef BOOL FAR        *LPBOOL;
typedef long FAR        *LPLONG;
typedef LPSTR FAR       *LPLPSTR;
typedef void FAR        *LPVOID;
#endif

typedef LPBYTE FAR      *LPLPBYTE;
typedef LPVOID FAR      *LPLPVOID;
typedef LPDWORD FAR     *LPLPDWORD;

#define FALSE			0
#define TRUE			1

#endif // __MACINTOSH__

#endif

