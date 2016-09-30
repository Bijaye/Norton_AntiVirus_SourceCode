// Copyright 1998 Symantec, SABU
//************************************************************************
//
// $Header:   S:/NAVEX/VCS/a2scn.h_v   1.0   11 May 1998 17:45:28   DCHI  $
//
// Description:
//  Contains Access 2.0 macro scan function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/a2scn.h_v  $
// 
//    Rev 1.0   11 May 1998 17:45:28   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _A2SCN_H_

#define _A2SCN_H_

EXTSTATUS EXTScanFileAccess2
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    HFILE               hFile,              // Handle to file to scan
    LPBYTE              lpbyWorkBuffer,     // Work buffer >= 2048 bytes
    LPWORD              lpwVID              // Virus ID storage on hit
);

#endif // #ifndef _A2SCN_H_

