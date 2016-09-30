// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1995 - 2003, 2005 Symantec Corporation. All rights reserved.
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/SEARCH.H_v   1.2   06 Sep 1996 14:04:26   CNACHEN  $
//
// Description:
//
//  This file contains constants and data structures for the PAM string
//  searching module.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/SEARCH.H_v  $
// 
//    Rev 1.2   06 Sep 1996 14:04:26   CNACHEN
// Updated header to use proper #pragma pack() for NTK/WIN32.
// 
//    Rev 1.2   06 Sep 1996 14:01:10   CNACHEN
// Added proper #pragma pack() commands for NTK/WIN32 platforms.
// 
//    Rev 1.1   14 Mar 1996 10:08:08   DCHI
// Brought CLUSTER_SIZE down to a safe level below 64K.
// 
//    Rev 1.0   01 Feb 1996 10:17:40   CNACHEN
// Initial revision.
// 
//    Rev 1.6   23 Oct 1995 12:51:34   CNACHEN
// Added #pragma pack(1) directives around all structure definitions
// 
//    Rev 1.5   19 Oct 1995 18:39:16   CNACHEN
// Initial revision... With comment headers... :)
//************************************************************************


#define NUM_TOP_SETS    16              /* # of bytes in starting sig set */
#define BIT_TABLE_SIZE  1024            /* # of bytes in the bit table for */
										/* each sig set */
#define BIT_TABLE_SHIFT 3               /* shift right next WORD by 3 bits */
										/* to obtain bit index # */
#define MAX_SIG_PTRS    6144
#define SIG_LEN         24

#define ENCRYPT_START   3

#define NOT_FOUND       0xFFFFU

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(push,1)
#else
#pragma pack(1)
#endif


typedef struct
{
    BYTE    sig[SIG_LEN];
    WORD    length;
    DWORD   virus_index;
} sig_type;

#define CLUSTER_SIZE    (0xF000U / sizeof(sig_type))

typedef struct
{
    BYTE        bit_table[BIT_TABLE_SIZE];
    WORD        low_index;
    WORD        high_index;
} table_type;

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(pop)
#else
#pragma pack()
#endif

