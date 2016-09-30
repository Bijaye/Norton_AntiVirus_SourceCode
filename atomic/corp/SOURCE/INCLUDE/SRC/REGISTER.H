// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/include/VCS/register.h_v   1.0   06 Feb 1997 20:51:16   RFULLER  $
//
// Description:
//      This file contains the (few) prototypes for Banker's File Monitor
//      Registration routines, and the macro covers to make use of
//      the exclusions code.
//
// Contains:
//
// See Also:
//      EXCLUDE.H
//************************************************************************
// $Log:   S:/include/VCS/register.h_v  $
// 
//    Rev 1.0   06 Feb 1997 20:51:16   RFULLER
// Initial revision
// 
//    Rev 1.0   30 Jan 1996 15:47:22   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 10:11:46   BARRY
// Initial revision.
// 
//    Rev 1.5   31 Oct 1994 12:50:26   MFALLEN
// Starting of fresh from the old NAV 3.0 codebase.
// 
//    Rev 1.2   17 Mar 1994 11:23:50   KEITH
// Bankers Trust - added template for DB_MEMORYINCLUDE
// 
//    Rev 1.1   04 Mar 1994 13:21:54   DALLEE
// Removed #define IS_REGISTERED, use incREG_CHANGE from CTS4.H instead.
// 
//    Rev 1.0   10 Feb 1994 16:17:30   DALLEE
// Initial revision.
//************************************************************************

#ifndef __REGISTER_H
#define __REGISTER_H

#ifndef _EXCLU_STUFF
#include "exclude.h"
#endif

#ifndef _CTS4_H
#include "cts4.h"
#endif


//************************************************************************
// DEFINES
//************************************************************************


//************************************************************************
// TYPEDEFS
//************************************************************************
         
typedef EXCLUDE         REGISTER, FAR *LPREGISTER;
typedef EXCLUDEITEM     REGISTERITEM, FAR *LPREGISTERITEM;


//************************************************************************
// MACROS
//************************************************************************

#define RegisterAdd(Reg,Item)               ExcludeAdd(Reg,Item)
#define RegisterDeleteIndex(Reg,Index)      ExcludeDeleteIndex(Reg,Index)
#define RegisterIs(Reg,File)                ExcludeIs(Reg,File,incREG_CHANGE)
#define RegisterDelete(Reg,Item)            ExcludeDelete(Reg,Item)
#define RegisterEdit(Reg,Old,New)           ExcludeEdit(Reg,Old,New)
#define RegisterInitSpecifyDir(Reg,Dir)     ExcludeInitSpecifyDir(Reg,Dir)
#define RegisterInitSpecifyFile(Reg,File)   ExcludeInitSpecifyFile(Reg,File)
#define RegisterTerminateSpecifyDir(R,D,M)  ExcludeTerminateSpecifyDir(R,D,M)
#define RegisterTerminateSpecifyFile(R,F,M) ExcludeTerminateSpecifyFile(R,F,M)


//************************************************************************
// PROTOTYPES
//************************************************************************

#ifdef __cplusplus
extern "C" {
#endif

VOID    SYM_EXPORT WINAPI GetRegisterBin    (HINSTANCE hInstance,
                                         LPSTR szFile,
                                         UINT uSize);
LPSTR   SYM_EXPORT WINAPI GetRegisterBinFilename    (LPSTR lpszFilename);
STATUS  SYM_EXPORT WINAPI RegisterInit      (LPEXCLUDE lpRegister);
STATUS  SYM_EXPORT WINAPI RegisterTerminate (LPEXCLUDE lpRegister,
                                         BOOL bFreeMemory);
int     WINAPI LOADDS DB_MEMORYINCLUDE  (WORD uBits,
                                         WORD uFlag,
                                         LPSTR lpIncludeDB,
                                         LPSTR lpWork );


#ifdef __cplusplus
}
#endif

#endif  // (#ifndef __REGISTER_H)

