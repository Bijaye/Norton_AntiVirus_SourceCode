// Copyright 1998 Symantec, SABU
//************************************************************************
//
// $Header:   S:/NAVEX/VCS/MSX.H_v   1.0   08 Dec 1998 12:53:08   DCHI  $
//
// Description:
//  MSX external function prototypes.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/MSX.H_v  $
// 
//    Rev 1.0   08 Dec 1998 12:53:08   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _MSX_H_

#define _MSX_H_

BOOL MSXFileIsExcluded
(
    LPCALLBACKREV2      lpstCallBack,
    HFILE               hFile
);

BOOL MSXAddFile
(
    LPCALLBACKREV2      lpstCallBack,
    HFILE               hFile
);

BOOL MSXStartUp
(
    LPCALLBACKREV2      lpstCallBack,
    LPTSTR              lpszDataDir
);

BOOL MSXShutDown
(
    LPCALLBACKREV2      lpstCallBack
);

#endif // #ifndef _MSX_H_

