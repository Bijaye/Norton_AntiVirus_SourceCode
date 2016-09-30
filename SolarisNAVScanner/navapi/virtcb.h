// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVAPI/VCS/virtcb.h_v   1.2   15 Sep 1998 14:13:42   MKEATIN  $
//
// Description:
//  Header for NAVAPI's global engine callbacks.
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/NAVAPI/VCS/virtcb.h_v  $
// 
//    Rev 1.2   15 Sep 1998 14:13:42   MKEATIN
// Changed bExecutable to lpstFileExt.
//
//    Rev 1.1   04 Aug 1998 15:57:16   MKEATIN
// Now defining DOS_LOADDS as LOADDS for DOS and Win16.
//
//    Rev 1.0   11 Jun 1998 17:38:42   DALLEE
// Initial revision.
//************************************************************************

#ifndef __VIRTCB_H
#define __VIRTCB_H

#include "navapi.h"

// Function Prototypes

VOID SetStaticCallbackData
(
    LPCTSTR     szDefsPath,
    WORD        wHeuristicLevel,
    LPNAVFILEIO lpFileCallbacks
);

LPNAVFILEIO GetFileIoCallbacks();
LPCTSTR GetDefsPath();


// Function for encoding pointer to virtual file ID's in string form which
// can be passed to AVAPI and later decoded by virtual file callbacks.
// See notes in VIRTCB.CPP.
// Internal use only - lpszEncodedId should be SYM_MAX_PATH.

void EncodeVirtFileId ( LPTSTR lpszEncodedId, LPVOID lpvFileId, LPTSTR lpstFileExt );

// Lowest level of NAVAPI's file callbacks.
// The following VirtFileXXX() routines switch between standard platform IO and
// client provided IO functions.

#if defined(SYM_DOS) || defined(SYM_WIN16)
  #define DOS_LOADDS    LOADDS
#else
  #define DOS_LOADDS
#endif

HFILE DOS_LOADDS WINAPI VirtFileOpen ( LPTSTR  lpszFilename, UINT uMode );
HFILE DOS_LOADDS WINAPI VirtFileCreate ( LPTSTR  lpszFilename, UINT uFlags );
HFILE DOS_LOADDS WINAPI VirtGetTempFile ( LPTSTR lpszFileName, LPVOID lpvCookie );
HFILE DOS_LOADDS WINAPI VirtFileClose ( HFILE hFile );
UINT DOS_LOADDS WINAPI VirtFileRead ( HFILE hFile, LPVOID lpBuffer, UINT uBytes );
UINT DOS_LOADDS WINAPI VirtFileWrite ( HFILE hFile, LPVOID lpBuffer, UINT uBytes );
DWORD DOS_LOADDS WINAPI VirtFileSeek ( HFILE hFile, LONG lOffset, int nFrom );
DWORD DOS_LOADDS WINAPI VirtFileLength ( HFILE hFile );
BOOL DOS_LOADDS WINAPI VirtFileGetDateTime ( HFILE hFile, UINT uType, LPWORD lpwDate, LPWORD lpwTime );
BOOL DOS_LOADDS WINAPI VirtFileSetDateTime ( HFILE hFile, UINT uType, WORD wDate, WORD wTime );
BOOL DOS_LOADDS WINAPI VirtFileDelete ( LPTSTR lpszFilename );
UINT DOS_LOADDS WINAPI VirtFileGetAttr ( LPTSTR lpszFilename, LPUINT lpuAttr );
UINT DOS_LOADDS WINAPI VirtFileSetAttr ( LPTSTR lpszFilename, UINT uAttr );

#endif // __VIRTCB_H
