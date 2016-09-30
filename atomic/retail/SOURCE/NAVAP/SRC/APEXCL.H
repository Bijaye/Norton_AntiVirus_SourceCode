// Copyright 1996 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/NAVAP/VCS/APEXCL.H_v   1.2   17 Feb 1998 22:04:52   RStanev  $
//
// Description:
//      This file prototypes NAVAP's exclusion APIs.  Some of these APIs
//      may be exported to other NAVAP modules.
//
// Contains:
//      NAVAPInitExclusionAPI()
//      NAVAPCloseExclusionAPI()
//      NAVAPIsFileExcluded()
//      NAVAPExcludeFile()
//
// See Also:
//      apexcl.cpp
//
// Note:
//
//***************************************************************************
// $Log:   S:/NAVAP/VCS/APEXCL.H_v  $
// 
//    Rev 1.2   17 Feb 1998 22:04:52   RStanev
// Updated copyright.
//
//    Rev 1.1   15 May 1997 02:50:32   RStanev
// Minor changes.
//
//    Rev 1.0   09 Apr 1997 12:21:28   RStanev
// Initial revision.
//
//***************************************************************************

#ifndef _APEXCL_H_
#define _APEXCL_H_

//---------------------------------------------------------------------------
//
// Prototypes of exported functions.
//
//---------------------------------------------------------------------------
HEXCLUDEN NAVAPInitExclusionAPI (
    PTSTR pszExcludeDBLocation
    );

BOOL NAVAPCloseExclusionAPI (
    HEXCLUDEN hExcludeN
    );

BOOL NAVAPIsFileExcluded (
    HEVENTPACKET hEventPacket,
    PTSTR        pszFileName,
    DWORD        dwFlags
    );

BOOL NAVAPExcludeFile (
    HEVENTPACKET hEventPacket,
    PTSTR        pszFileName,
    DWORD        dwFlags
    );

#endif // #ifndef _APEXCL_H_
