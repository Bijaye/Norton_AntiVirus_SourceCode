//************************************************************************
// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/navnlmex.h_v   1.0   24 Jul 1997 13:42:30   DDREW  $
//
// Description: These functions are to be found in the NAVNLM area. 
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/navnlmex.h_v  $
// 
//    Rev 1.0   24 Jul 1997 13:42:30   DDREW
// Initial revision.
// 
//    Rev 1.0   24 Jul 1997 13:25:14   DDREW
// Initial revision.
//************************************************************************

// CBSTATUS in nothing more that an unsigned int

HFILE SYM_EXPORT WINAPI NV_FileOpen (
    LPSTR   lpName, 
    UINT    uOpenMode );

UINT SYM_EXPORT WINAPI NV_FileGetDateTime (
    HFILE   hHandle,
    UINT    uType,
    LPWORD  lpwDate,
    LPWORD  lpwTime );

UINT SYM_EXPORT WINAPI NV_FileSetDateTime (
    HFILE   hHandle,
    UINT    uType,
    WORD    wDate,
    WORD    wTime );

UINT SYM_EXPORT WINAPI NV_ProgressInit
(
    LPWSTR       lpwstrObjectName,
    DWORD        dwLevel,
    DWORD        dwProgressValueMax,
    LPCOOKIETRAY lpCookieTray,
    LPLPVOID     lplpvData
);

UINT SYM_EXPORT WINAPI NV_ProgressUpdate
(
    LPVOID       lpvData,
    DWORD        dwProgressValue
);

UINT SYM_EXPORT WINAPI NV_ProgressClose
(
    LPVOID       lpvData
);
