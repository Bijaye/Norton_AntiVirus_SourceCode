// Copyright 1998 Symantec, SABU
//************************************************************************
//
// $Header:   S:/NAVEX/VCS/a97scn.h_v   1.0   17 Apr 1998 13:42:48   DCHI  $
//
// Description:
//  Contains Access 95/97 macro scan function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/a97scn.h_v  $
// 
//    Rev 1.0   17 Apr 1998 13:42:48   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _A97SCN_H_

#define _A97SCN_H_

EXTSTATUS EXTScanFileAccess97
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    HFILE               hFile,              // Handle to file to scan
    LPBYTE              lpbyWorkBuffer,     // Work buffer >= 2048 bytes
    LPWORD              lpwVID              // Virus ID storage on hit
);

#endif // #ifndef _A97SCN_H_

