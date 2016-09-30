// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/include/VCS/cmbstr.h_v   1.0   06 Feb 1997 20:51:12   RFULLER  $
//
// Description:
//      Typedefs and defines for NAVW string handling.
//      (and some defines so DOS can share some of the windows code).
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/include/VCS/cmbstr.h_v  $
// 
//    Rev 1.0   06 Feb 1997 20:51:12   RFULLER
// Initial revision
// 
//    Rev 1.0   30 Jan 1996 15:47:18   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 10:11:42   BARRY
// Initial revision.
// 
//    Rev 1.0   31 Oct 1994 12:48:08   MFALLEN
// Initial revision.
// 
//    Rev 1.4   23 Jul 1993 03:39:58   DALLEE
// Changed DOS macro String_Get to a function call in DOSMISC.C
// 
//    Rev 1.3   06 Jul 1993 09:16:26   MFALLEN
// Removed DisplayMessage()
// 
//    Rev 1.2   31 May 1993 17:46:26   DALLEE
// #define for String_Get() under DOS.
//************************************************************************

#ifndef _MSTRING
#define _MSTRING

#define SIZE_STRING     256

typedef struct tagM_STRING
    {
    UINT      hCmb;
    HINSTANCE hInstance;
    HGLOBAL   hString;
    LPSTR     lpString;
    } M_STRING;


#ifdef __cplusplus
extern "C" {
#endif

// *************************************** //
// Function prototypes for string routines //
// *************************************** //

#ifdef SYM_WIN
  BOOL WINAPI String_Init  (HINSTANCE, LPSTR);
  VOID WINAPI String_End   (VOID);
  UINT WINAPI String_Get   (UINT, LPSTR, UINT);
  LPSTR WINAPI String_Ptr(UINT);
#else
                                        // If DOS, just ignore the function
                                        // call.
  #define String_Init(x,y)            (TRUE)
  #define String_Ptr(num)             (num)
  UINT WINAPI String_Get(LPSTR lpDest, LPSTR lpSource, UINT nNum);
#endif

#ifdef __cplusplus
}
#endif

#endif

