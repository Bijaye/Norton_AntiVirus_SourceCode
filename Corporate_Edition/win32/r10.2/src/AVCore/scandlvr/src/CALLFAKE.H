// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1996, 2005 Symantec, Peter Norton Product Group. All rights reserved.
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/callfake.h_v   1.15   08 Sep 1997 11:08:20   CNACHEN  $
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
//    Rev 1.15   08 Sep 1997 11:08:20   CNACHEN
// Added prototype for DRelinquishControl
// 
//    Rev 1.14   03 Jun 1997 11:06:32   CNACHEN
// changed lpstrs to lptstrs
// 
//    Rev 1.13   21 Mar 1997 14:45:44   CNACHEN
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

HFILE DFileOpen(LPTSTR lpName, UINT uOpenMode);
HFILE DFileClose(HFILE hHandle);
UINT DFileRead(HFILE hHandle, LPVOID lpBuffer, UINT uBytes);
UINT DFileWrite(HFILE hHandle, LPVOID lpBuffer, UINT uBytes);
DWORD DFileSeek(HFILE hHandle, LONG dwOffset, int nFlag);
DWORD DFileLength(HFILE hHandle);
BOOL DFileDelete(LPTSTR lpFileName);
HFILE DFileCreate(LPTSTR lpFileName, UINT uAttr);
void DRelinquishControl(void);

#define READ_ONLY_FILE      0
#define READ_WRITE_FILE     2

#endif // #ifndef _CALLFAKE_H_

