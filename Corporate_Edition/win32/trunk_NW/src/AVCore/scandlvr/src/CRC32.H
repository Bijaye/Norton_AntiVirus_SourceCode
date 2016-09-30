// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/crc32.h_v   1.3   17 Nov 1998 11:25:44   DCHI  $
//
// Description:
//  CRC32 header file.
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/crc32.h_v  $
// 
//    Rev 1.3   17 Nov 1998 11:25:44   DCHI
// Added #define ADLER_CRC_SEED and AdlerCRC32() prototype.
// 
//    Rev 1.2   16 Oct 1997 13:17:40   DCHI
// Added CRC32Compute().
// 
//    Rev 1.1   13 Jun 1997 11:02:18   DCHI
// Modification so that CRC32Init() uses CRC32_SEED constant.
// 
//    Rev 1.0   10 Jun 1997 13:24:44   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _CRC32_H_

#define _CRC32_H_

extern DWORD gadwCRC32Table[256];

#define CRC32_SEED          0xFFFFFFFF

#define CRC32Init(dwCRC)    (dwCRC = CRC32_SEED)

#define CRC32Continue(dwCRC,by) \
    (dwCRC = ((dwCRC >> 8) & 0x00FFFFFFL) ^ \
            gadwCRC32Table[(dwCRC ^ (by)) & 0x000000FFL])

DWORD CRC32
(
    DWORD       dwNumBytesToCRC,
    LPBYTE      lpbyBytesToCRC
);

DWORD CRC32I
(
    DWORD       dwNumBytesToCRC,
    LPBYTE      lpbyBytesToCRC
);

DWORD CRC32Compute
(
    DWORD       dwNumBytesToCRC,
    LPBYTE      lpbyBytesToCRC,
    DWORD       dwStartCRC
);

#define ADLER_CRC_SEED  1

DWORD AdlerCRC32
(
    DWORD       dwStartCRC,
    LPBYTE      lpabyBuf,
    DWORD       dwByteCount
);

#endif // #ifndef _CRC32_H_

