// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/SDFILTER/VCS/filthash.h_v   1.0   06 Apr 1998 11:55:38   CNACHEN  $
//
// Description:
//  Filter hash code
//
// See Also:
//
//************************************************************************
// $Log:   S:/SDFILTER/VCS/filthash.h_v  $
// 
//    Rev 1.0   06 Apr 1998 11:55:38   CNACHEN
// Initial revision.
// 
//************************************************************************


#ifndef _FILTHASH_H

#define _FILTHASH_H

#define DOS_HEADER_MATCH_LEN            64
#define HEADER_OFFSET_IN_NAVEX_BUFFER   256
#define WIN_HEADER_MATCH_LEN            512

#define CHECKSUM_OFFSET_IN_HEADER1      0x12
#define CHECKSUM_OFFSET_IN_HEADER2      0x13

#define WINDOWS_PROGRAM_OVERLAY_VALUE   0x40

#define BIT_FILL_DOS_HEADER             0x00000001UL
#define BIT_FILL_WIN_HEADER             0x00000002UL
#define BIT_FILL_FILE_SIZE              0x00000004UL
#define BIT_FILL_FILE_CRC               0x00000008UL

#define FILTER_SECTION_ID               6900

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "pshpack1.h"
#elif defined(__MACINTOSH__)
#pragma options align=packed
#else
#pragma pack(1)
#endif

typedef struct
{
    DWORD           dwVersion;
    DWORD           dwNumSignatures;
    DWORD           dwNumPools;
    DWORD           dwPoolSize;
} FILTER_HEADER_T, FAR *LPFILTER_HEADER;

typedef struct
{
    DWORD               dwDOSHeaderCRC;
    DWORD               dwWinHeaderCRC;
    DWORD               dwFileSize;
    DWORD               dwFileCRC;
    WORD                wVirusID;
    WORD                wFiller;
} FILTER_DATA_T, FAR *LPFILTER_DATA;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "poppack.h"
#elif defined(__MACINTOSH__)
#pragma options align=reset
#else
#pragma pack()
#endif

// prototypes

BOOL ComputeFileCRC
(
    LPCALLBACKREV1          lpCallBack,
    HFILE                   hStream,
    LPDWORD                 lpdwCRC
);

EXTSTATUS GetPESectionEntry
(
    LPCALLBACKREV1                  lpCallBack,
    HFILE                           hFile,
    DWORD                           dwWinHeaderOffset,
    DWORD                           dwSecNum,
    NAVEX_IMAGE_SECTION_HEADER FAR *lpstSection
);

BOOL ObtainInformationPE
(
    LPCALLBACKREV1              lpCallBack,
    HFILE                       hFile,
    DWORD                       dwWinHeaderOffset,
    DWORD                       dwFlags,
    LPBYTE                      lpbyWorkBuffer,
    LPFILTER_DATA               lpstFillMe
);

BOOL ObtainInformation
(
    LPCALLBACKREV1              lpCallBack,
    HFILE                       hFile,
    DWORD                       dwFlags,
    LPBYTE                      lpbyHeader,
    LPBYTE                      lpbyWorkBuffer,
    LPFILTER_DATA               lpstFillMe
);

#endif // #ifndef _FILTHASH_H
