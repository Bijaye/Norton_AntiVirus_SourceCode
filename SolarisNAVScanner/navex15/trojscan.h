#ifndef _TROJSCAN_H

#define _TROJSCAN_H

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

#define TROJAN_SECTION_ID               710

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
} TROJAN_HEADER_T, FAR *LPTROJAN_HEADER;

typedef struct
{
    DWORD               dwDOSHeaderCRC;
    DWORD               dwWinHeaderCRC;
    DWORD               dwFileSize;
    DWORD               dwFileCRC;
    WORD                wVirusID;
    WORD                wFiller;
} TROJAN_DATA_T, FAR *LPTROJAN_DATA;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "poppack.h"
#elif defined(__MACINTOSH__)
#pragma options align=reset
#else
#pragma pack()
#endif

#define MAX_TROJAN_POOLS                64
#define MAX_ENTRIES_PER_POOL            2048

typedef struct
{
    LPTROJAN_DATA       alpstPool[MAX_TROJAN_POOLS];
    DWORD               dwSigsPerPool;
    DWORD               dwNumSigs;
    int                 nNumPools;
} TROJAN_INFO_T, FAR *LPTROJAN_INFO;

// prototypes

#ifndef TROJPARSE

BOOL ObtainInformation
(
    LPCALLBACKREV1              lpCallBack,
    HFILE                       hFile,
    DWORD                       dwFlags,
    LPBYTE                      lpbyHeader,
    LPBYTE                      lpbyWorkBuffer,
    LPTROJAN_DATA               lpstFillMe
);

EXTSTATUS ScanForTrojans
(
    LPCALLBACKREV1              lpCallBack,
    HFILE                       hFile,
    LPBYTE                      lpbyInfectionBuffer,
    LPBYTE                      lpbyWorkBuffer,
    LPWORD                      lpwVID
);


EXTSTATUS FreeTrojanData
(
    LPCALLBACKREV1                          lpstCallBack
);

EXTSTATUS LoadTrojanData
(
    LPCALLBACKREV1                          lpstCallBack,
    LPTSTR                                  lpszNAVEXDataDir
);

#ifdef SYM_WIN16
void LoadTrojanDataW16
(
    LPVOID              lpvExtra
);
#endif // SYM_WIN16

#ifdef SYM_DOSX
void LoadTrojanDataDX
(
    LPTSTR              lpszProgramPath
);
#endif // SYM_DOSX

#if defined(SYM_DOSX) || defined(SYM_WIN16)
void FreeTrojanDataNoCB
(
    void
);
#endif // #if defined(SYM_DOSX) || defined(SYM_WIN16)

#endif // #ifndef TROJPARSE

#endif // #ifndef _TROJSCAN_H
