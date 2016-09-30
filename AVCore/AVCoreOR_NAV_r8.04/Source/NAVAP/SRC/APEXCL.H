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

#ifndef _APEXCL_H_
#define _APEXCL_H_

//---------------------------------------------------------------------------
//
// Prototypes of exported functions.
//
//---------------------------------------------------------------------------
HEXCLUDEN NAVAPInitExclusionAPI (
    DWORD dwExclusionItems,
    PVOID pvExclusionItems,
    DWORD dwExclusionLItems,
    PVOID pvExclusionLItems
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
