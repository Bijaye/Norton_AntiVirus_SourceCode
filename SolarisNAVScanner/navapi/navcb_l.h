// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVAPI/VCS/navcb_l.h_v   1.1   27 May 1998 17:41:52   MKEATIN  $
//
// Description:
//  Header for NAV's global engine callback structer.
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/NAVAPI/VCS/navcb_l.h_v  $
// 
//    Rev 1.1   27 May 1998 17:41:52   MKEATIN
// changed NAVSetDefsPath to NAVSetStaticData
// 
//    Rev 1.0   22 May 1998 20:29:52   MKEATIN
// Initial revision.
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

VOID SYM_EXPORT WINAPI NAVSetStaticData
(
    LPCTSTR szDefsPath,
    WORD wHeuristicLevel
);

typedef struct
{
    LPGENERALCALLBACKS      lpGeneralCallBacks;
    LPPROGRESSCALLBACKS     lpProgressCallBacks;
    LPDATAFILECALLBACKS     lpDataFileCallBacks;
    LPIPCCALLBACKS          lpIPCCallBacks;
    LPMEMORYCALLBACKS       lpMemoryCallBacks;
    LPCALLBACKREV2          lpNAVEXCallBacks;
} AVGLOBALCALLBACKS, FAR *LPAVGLOBALCALLBACKS;

#endif // _NAVCB_H
