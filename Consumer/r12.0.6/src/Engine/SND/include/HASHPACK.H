// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/hashpack.h_v   1.3   13 Apr 1998 18:01:36   CNACHEN  $
//
// Description:
//  Structures for Atomic/Scan and Deliver client-server hash packets
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/hashpack.h_v  $
// 
//    Rev 1.3   13 Apr 1998 18:01:36   CNACHEN
// Added HASH_PACKET_VERSIOn
// 
//    Rev 1.2   13 Apr 1998 16:42:02   CNACHEN
// 
//    Rev 1.1   13 Apr 1998 16:40:40   CNACHEN
// Added #defines for packet types
// 
//    Rev 1.0   13 Apr 1998 15:32:08   CNACHEN
// Initial revision.
// 
//************************************************************************

#ifndef _HASHPACK_H_

#define _HASHPACK_H_

#ifdef UNIX
#pragma pack(1)
#elif defined(SYM_AIX)
#pragma options align=packed
#else
#include "pshpack1.h"
#endif

#define PRE_SUBMISSION_VERSION      1

#define PRE_SUBMISSION_HEADER       1       // dwType

#define MAX_SUBMISSION_PACKETS      16

#define HASH_PACKET_VERSION         1

typedef struct
{
    DWORD           dwType;
    DWORD           dwVersion;
    DWORD           dwSize;
    DWORD           dwTotalNumFiles; // not nesc. sent
    DWORD           dwNumPackets;
} PRE_SUBMISSION_HDR_T, *PPRE_SUBMISSION_HDR;

#define SUBMIT_BUFFER_SIZE  20

#define HASH_PACKET_TYPE            1       // dwType

typedef struct
{
    DWORD           dwType;
    DWORD           dwVersion;
    DWORD           dwSize;
    BYTE            byTrojanData[SUBMIT_BUFFER_SIZE];
} PRE_PACKET_T, *PPRE_PACKET;

#ifdef UNIX
#pragma pack()
#elif defined(SYM_AIX)
#pragma options align=reset
#else
#include "poppack.h"
#endif

#endif // #ifndef _HASHPACK_H_
