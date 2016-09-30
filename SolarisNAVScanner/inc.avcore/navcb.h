// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/navcb.h_v   1.3   18 Jun 1997 16:34:20   MKEATIN  $
//
// Description:
//  Header for NAV's global engine callback structer.
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/navcb.h_v  $
// 
//    Rev 1.3   18 Jun 1997 16:34:20   MKEATIN
// Fixed parameters for NAVSetDefsPath().
// 
//    Rev 1.2   18 Jun 1997 16:29:02   MKEATIN
// Added prototype for NAVSetDefsPath().
// 
//    Rev 1.1   17 Jun 1997 15:35:02   MKEATIN
// Don't extern gstNAVCallBacks in the building file.
// 
//    Rev 1.0   17 Jun 1997 14:51:56   MKEATIN
// Initial revision.
//************************************************************************

#ifndef _NAVCB_H

#define _NAVCB_H

#include "avtypes.h"
#include "avcb.h"
#include "callbk15.h"


// Function Prototypes

VOID SYM_EXPORT WINAPI NAVSetDefsPath(LPCTSTR szPath);

typedef struct
{
    LPGENERALCALLBACKS      lpGeneralCallBacks;
    LPPROGRESSCALLBACKS     lpProgressCallBacks;
    LPDATAFILECALLBACKS     lpDataFileCallBacks;
    LPIPCCALLBACKS          lpIPCCallBacks;
    LPMEMORYCALLBACKS       lpMemoryCallBacks;
    LPCALLBACKREV2          lpNAVEXCallBacks;
} AVGLOBALCALLBACKS, FAR *LPAVGLOBALCALLBACKS;

#ifdef SYM_WIN32

  #if !defined(BUILDING_CALLBACK)
    __declspec(dllimport) AVGLOBALCALLBACKS gstNAVCallBacks; 
  #endif

#else 

  #if !defined(BUILDING_CALLBACK)
    EXTERN AVGLOBALCALLBACKS gstNAVCallBacks; 
  #endif

#endif // SYM_WIN32


#endif // _NAVCB_H
