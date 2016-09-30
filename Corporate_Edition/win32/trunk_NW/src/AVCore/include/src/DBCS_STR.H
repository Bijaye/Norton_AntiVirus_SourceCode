// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
/* Copyright 1992 - 2003, 2005 Symantec Corporation. All rights reserved.     */
/************************************************************************
 *                                                                      *
 * $Header:   S:/INCLUDE/VCS/dbcs_str.h_v   1.2   16 Oct 1996 19:14:06   DALLEE  $ *
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/INCLUDE/VCS/dbcs_str.h_v  $ *
// 
//    Rev 1.2   16 Oct 1996 19:14:06   DALLEE
// Added DBCS functions for strncpy, stricmp, strnicmp, and strstr.
// 
//    Rev 1.1   06 May 1996 22:08:34   RSTANEV
// Didn't like unicode...
//
//    Rev 1.0   26 Jan 1996 20:17:32   RJACKSO
// Initial revision.
//
//    Rev 1.1   20 Jan 1995 09:49:52   THUNTER
// Added support for SYM_MAC platform.
//
//    Rev 1.0   08 Nov 1994 17:21:30   BRAD
// Initial revision.
 ************************************************************************/

#ifdef SYM_MAC
	#pragma once
#endif

#ifndef INCL_DBCS_STR
#define INCL_DBCS_STR

#ifdef __cplusplus
extern "C" {
#endif

#if defined(UNICODE)

#define SYMstrchr   STRCHR
#define SYMstrrchr  STRRCHR
#define SYMstrpbrk  STRPBRK
#define SYMstrrev   STRREV
#define SYMstrncpy  STRNCPY
#define SYMstricmp  STRICMP
#define SYMstrnicmp STRNICMP
#define SYMstrstr   STRSTR

#else

LPSTR SYM_EXPORT WINAPI SYMstrchr(LPCSTR lpszStr, int nChar);
LPSTR SYM_EXPORT WINAPI SYMstrrchr(LPCSTR lpszStr, int nChar);
LPSTR SYM_EXPORT WINAPI SYMstrpbrk(LPCSTR lpszStr1, LPCSTR lpszStr2);
LPSTR SYM_EXPORT WINAPI SYMstrrev(LPSTR lpszStr);
LPSTR SYM_EXPORT WINAPI SYMstrncpy(LPSTR lpszDest, LPCSTR lpszSrc, size_t uCount);
int   SYM_EXPORT WINAPI SYMstricmp(LPCSTR lpszStr1, LPCSTR lpszStr2);
int   SYM_EXPORT WINAPI SYMstrnicmp(LPCSTR lpszStr1, LPCSTR lpszStr2, size_t uCount);
LPSTR SYM_EXPORT WINAPI SYMstrstr(LPCSTR lpszStr1, LPCSTR lpszStr2);

#endif

#ifdef __cplusplus                    // end prototypes
}
#endif

#endif

