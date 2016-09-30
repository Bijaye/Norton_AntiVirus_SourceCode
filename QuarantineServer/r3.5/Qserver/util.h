/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/


/*------------------------------------------------------------------------
   UTIL.H
   
   Utility routines 
   
   Written by Jim Hill
------------------------------------------------------------------------*/



#ifndef _UTIL_H_DEFINED
   #define  _UTIL_H_DEFINED

#ifdef __cplusplus               // Define as "C" linkage
extern "C"
{
#endif



#define MAX_PRINTSTRING     2048
#define	NOERR				   0

///////#include "util.h"

//BYTE *GetTimeString(BYTE *lpszTimeString);
void fPrintString(LPCSTR lpszFmt, ...);
void fDebugString(LPCSTR lpszFmt, ...);
void fWidePrintString(LPCSTR lpszFmt, ...);
LPTSTR RemoveMultipleCharacters(LPTSTR str,LPCTSTR chars);





#ifdef __cplusplus                                  
}
#endif

#endif