// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/callfake.h_v   1.3   20 Nov 1996 15:22:42   AOONWAL  $
//
// Description:
//  Header file for callfake
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/callfake.h_v  $
// 
//    Rev 1.3   20 Nov 1996 15:22:42   AOONWAL
// No change.
// 
//    Rev 1.2   22 Oct 1996 12:13:42   AOONWAL
// No change.
// 
//    Rev 1.1   17 Oct 1996 17:57:00   CNACHEN
// Added #defines of READ_ONLY_FILE and READ_WRITE_FILE
// 
//    Rev 1.0   09 Oct 1996 13:08:06   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _CALLFAKE_H_

#define _CALLFAKE_H_

#include <stdio.h>

#if defined(_MSC_VER)
#   define asm __asm
#endif

#ifdef PROTO_PAM_CALLBACKS
    HFILE PAMFileOpen(LPSTR lpName, UINT uOpenMode);
    HFILE PAMFileClose(HFILE hHandle);
    UINT PAMFileRead(HFILE hHandle, LPVOID lpBuffer, UINT uBytes);
    UINT PAMFileWrite(HFILE hHandle, LPVOID lpBuffer, UINT uBytes);
    DWORD PAMFileSeek(HFILE hHandle, LONG dwOffset, int nFlag);
    DWORD PAMFileLength(HFILE hHandle);
    BOOL PAMFileDelete(LPSTR lpFileName);
    HFILE PAMFileCreate(LPSTR lpFileName, UINT uAttr);
    LPVOID PAMMemoryAlloc(DWORD dwSize);
    VOID PAMMemoryFree(LPVOID);
#else
    HFILE DFileOpen(LPSTR lpName, UINT uOpenMode);
    HFILE DFileClose(HFILE hHandle);
    UINT DFileRead(HFILE hHandle, LPVOID lpBuffer, UINT uBytes);
    UINT DFileWrite(HFILE hHandle, LPVOID lpBuffer, UINT uBytes);
    DWORD DFileSeek(HFILE hHandle, LONG dwOffset, int nFlag);
    DWORD DFileLength(HFILE hHandle);
    BOOL DFileDelete(LPSTR lpFileName);
    HFILE DFileCreate(LPSTR lpFileName, UINT uAttr);
#endif

#endif // #ifndef _CALLFAKE_H_

