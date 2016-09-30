// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/HEUR/VCS/crc32.h_v   1.0   23 May 1997 18:14:32   CNACHEN  $
//
// Description:
//  Contains prototypes for functions exported by AVENGE.
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/HEUR/VCS/crc32.h_v  $
// 
//    Rev 1.0   23 May 1997 18:14:32   CNACHEN
// Initial revision.
// 
//************************************************************************

#ifndef _CRC32_H

#define _CRC32_H

DWORD ComputeCRC32
(
    LPBYTE      lpbyData,
    int         nLen
);

#endif
