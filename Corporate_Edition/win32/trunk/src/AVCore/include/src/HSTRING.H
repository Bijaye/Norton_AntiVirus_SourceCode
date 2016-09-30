// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
/* Copyright 1992 - 2003, 2005 Symantec Corporation. All rights reserved.     */
/************************************************************************/
/*                                                                      */
/* $Header:   S:/INCLUDE/VCS/HSTRING.H_v   1.0   26 Jan 1996 20:14:36   RJACKSO  
/*                                                                      */
/* Description:  .h file for Huge String routines.                      */
/*                                                                      */
/* Contains:                                                            */
/*                                                                      */
/* See Also:                                                            */
/*                                                                      */
/************************************************************************/
/* $Log:   S:/INCLUDE/VCS/HSTRING.H_v  $ */
// 
//    Rev 1.0   26 Jan 1996 20:14:36   RJACKSO
// Initial revision.
// 
//    Rev 1.5   26 Apr 1994 16:39:56   BRAD
// Removed PVCS Logs

/*----------------------------------------------------------------------*/
/* TypeDefs                                                             */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function Prototypes                                                  */
/*----------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif


int SYM_EXPORT WINAPI hstrcmp    (HPSTR, HPSTR);
HPSTR SYM_EXPORT WINAPI hstrcpy    (HPSTR, HPSTR);
DWORD SYM_EXPORT WINAPI hstrlen    (HPSTR);
HPSTR SYM_EXPORT WINAPI hstrcat    (HPSTR, HPSTR);
WORD SYM_EXPORT WINAPI hGetWord   (HPBYTE);


#define HSTRCMP(x,y)    hstrcmp  ((HPSTR)(x), (HPSTR)(y))
#define HSTRCPY(x,y)    hstrcpy  ((HPSTR)(x), (HPSTR)(y))
#define HSTRCAT(x,y)    hstrcat  ((HPSTR)(x), (HPSTR)(y))
#define HSTRLEN(x)      hstrlen  ((HPSTR)(x))
#define HGETWORD(x)     hGetWord ((HPBYTE)(x))


#ifdef __cplusplus
}
#endif

