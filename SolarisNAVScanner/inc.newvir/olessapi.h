//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/olessapi.h_v   1.16   07 Jan 1999 16:03:26   DCHI  $
//
// Description:
//  Core OLE structured storage access implementation header file.
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/olessapi.h_v  $
// 
//    Rev 1.16   07 Jan 1999 16:03:26   DCHI
// Added prototypes for SSDocSumInfoRemDigiSig() and SSGetDocSumInfoStream().
// 
//    Rev 1.15   24 Jul 1998 16:34:46   DCHI
// Added prototype for SSGetStreamIndexCB().
// 
//    Rev 1.14   21 Jul 1998 11:13:02   DCHI
// Modified prototype of SSGetDirEntry().
// 
//    Rev 1.13   21 Jul 1998 11:10:10   DCHI
// Added SSGetDirEntry() prototype.
// 
//    Rev 1.12   24 Jun 1998 11:09:04   DCHI
// Added prototypes for SSEndianDWORD() and SSEndianWORD().
// 
//    Rev 1.11   26 Mar 1998 18:34:50   DCHI
// Added SSClearSummaryInfo() prototype.
// 
//    Rev 1.10   26 Mar 1998 11:22:08   DCHI
// Added function prototype for SSGetNamedSiblingID().
// 
//    Rev 1.9   24 Nov 1997 17:28:56   DCHI
// Added SSGetParentStorage() prototype.
// 
//    Rev 1.8   14 Oct 1997 17:34:34   DCHI
// Added SSToLower() and SSToUpper() function prototypes.
// 
//    Rev 1.7   25 Jun 1997 17:04:26   DCHI
// Added SSProgress() prototype for SYM_NLM.
// 
//    Rev 1.6   11 Jun 1997 13:59:24   DCHI
// Added prototype for SSOpenStreamByNameCB().
// 
//    Rev 1.5   09 Jun 1997 14:09:14   DCHI
// Reworked pack pragmas to work on UNIX gcc which doesn't replace tokens
// within pragmas.
// 
//    Rev 1.4   30 May 1997 16:29:02   DCHI
// Added #define of SS_ENUM_CB_STATUS_ERROR.
// 
//    Rev 1.3   08 May 1997 12:36:24   DCHI
// Corrected #define of SS_MINI_SECTOR_OFFSET_MASK from 0x2F to 0x3F.
// 
//    Rev 1.2   05 May 1997 13:55:56   DCHI
// Added prototype for SSFileTruncate().
// 
//    Rev 1.1   25 Apr 1997 12:34:56   DCHI
// Added SSWStrNCmp().
// 
//    Rev 1.0   23 Apr 1997 15:10:36   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _OLESSAPI_H_

#define _OLESSAPI_H_

#include "ssbstype.h"

//////////////////////////////////////////////////////////////////////
// Default return type
//////////////////////////////////////////////////////////////////////

typedef int SS_STATUS;

#define SS_STATUS_OK        0
#define SS_STATUS_ERROR     1


//////////////////////////////////////////////////////////////////////
// Endianization definitions
//////////////////////////////////////////////////////////////////////

#if !defined(WENDIAN) || !defined(DWENDIAN)

    #ifdef  BIG_ENDIAN

        #define WENDIAN(w)   ( (WORD)((WORD)(w) >> 8) | (WORD)((WORD)(w) << 8) )
        #define DWENDIAN(dw) ( (DWORD)((DWORD)(dw) >> 24) | \
                               (DWORD)((DWORD)dw << 24)  | \
                               (DWORD)(((DWORD)(dw) >> 8) & 0xFF00U) | \
                               (DWORD)(((DWORD)(dw) << 8) & 0xFF0000UL) )

    #else   // if LITTLE_ENDIAN (default)

        #define WENDIAN(w)   (w)
        #define DWENDIAN(dw) (dw)

    #endif

#endif // #if !defined(WENDIAN) || !defined(DWENDIAN)


//////////////////////////////////////////////////////////////////////
// Application specific callback implementation
//////////////////////////////////////////////////////////////////////

SS_STATUS SSMemoryAlloc
(
    LPVOID      lpvRootCookie,
    DWORD       dwNumBytes,
    LPLPVOID    lplpvBuffer
);

SS_STATUS SSMemoryFree
(
    LPVOID      lpvRootCookie,
    LPVOID      lpvBuffer
);

#define SS_SEEK_SET     0
#define SS_SEEK_CUR     1
#define SS_SEEK_END     2

SS_STATUS SSFileSeek
(
    LPVOID      lpvRootCookie,
    LPVOID      lpvFile,
    long        lOffset,
    int         nWhence,
    LPLONG      lplNewOffset
);

SS_STATUS SSFileRead
(
    LPVOID      lpvRootCookie,
    LPVOID      lpvFile,
    LPVOID      lpvBuffer,
    DWORD       dwNumBytes,
    LPDWORD     lpdwBytesRead
);


SS_STATUS SSFileWrite
(
    LPVOID      lpvRootCookie,
    LPVOID      lpvFile,
    LPVOID      lpvBuffer,
    DWORD       dwNumBytes,
    LPDWORD     lpdwBytesWritten
);

SS_STATUS SSFileTruncate
(
    LPVOID      lpvRootCookie,
    LPVOID      lpvFile
);

#ifdef SYM_NLM
SS_STATUS SSProgress
(
    LPVOID      lpvRootCookie
);
#endif


//////////////////////////////////////////////////////////////////////
// OLE structured storage structures
//////////////////////////////////////////////////////////////////////

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "pshpack1.h"
#elif defined(__MACINTOSH__)
#pragma options align=packed
#else
#pragma pack(1)
#endif

// Direct FAT count

#define SS_CDFAT            109

// File header structure

typedef struct tagSS_HDR
{
    BYTE        abySig[8];              // 0x00: Signature
    DWORD       dwUnknown0[4];          // 0x08

    WORD        wMinorVersion;          // 0x18
    WORD        wDLLVersion;            // 0x1A
    WORD        wByteOrder;             // 0x1C

    WORD        wSectorShift;           // 0x1E
    WORD        wMiniSectorShift;       // 0x20

    WORD        wReserved;              // 0x22
    DWORD       dwReserved1;            // 0x24
    DWORD       dwReserved2;            // 0x28

    DWORD       dwFATSectorCount;       // 0x2C
    DWORD       dwDirStartSector;       // 0x30

    DWORD       dwSignature;            // 0x34

    // Size below which mini stream is used

    DWORD       dwMiniSectorCutoff;     // 0x38

    // Mini FAT info

    DWORD       dwMiniFATStartSector;   // 0x3C
    DWORD       dwMiniFATSectorCount;   // 0x40

    // Double indirect FAT info

    DWORD       dwDIFATStartSector;     // 0x44
    DWORD       dwDIFATSectorCount;     // 0x48

    // Array of sectors containing direct FATs

    DWORD       dwFATSector[SS_CDFAT];  // 0x4C
} SS_HDR_T, FAR *LPSS_HDR;

// Directory entry structure

#define SS_MAX_NAME_LEN     0x20

#define STGTY_STORAGE       1
#define STGTY_STREAM        2
#define STGTY_LOCKBYTES     3
#define STGTY_PROPERTY      4
#define STGTY_ROOT          5

typedef struct tagSS_DIR_ENTRY
{
    WORD    uszName[SS_MAX_NAME_LEN];   // 0x00

    WORD    wPadding;                   // 0x40

    BYTE    byMSE;                      // 0x42
    BYTE    byBFlags;                   // 0x43

    DWORD   dwSIDLeftSib;               // 0x44
    DWORD   dwSIDRightSib;              // 0x48

    DWORD   dwSIDChild;                 // 0x4C
    DWORD   dwCLSID[4];                 // 0x50
    DWORD   dwUserFlags;                // 0x60
    DWORD   dwTimes[4];                 // 0x64

    // If Root Entry, then this is the starting block of the mini stream

    DWORD   dwStartSector;              // 0x74

    DWORD   dwStreamLen;                // 0x78

    DWORD   dwUnknown2;                 // 0x7C

} SS_DIR_ENTRY_T, FAR *LPSS_DIR_ENTRY, FAR * FAR *LPLPSS_DIR_ENTRY;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "poppack.h"
#elif defined(__MACINTOSH__)
#pragma options align=reset
#else
#pragma pack()
#endif


//////////////////////////////////////////////////////////////////////
// Stream access definitions and structure
//////////////////////////////////////////////////////////////////////

#define SS_STREAM_FLAG_NO_BAT_CACHE     0x00000000
#define SS_STREAM_FLAG_DEF_BAT_CACHE    0x00000001

#define SS_STREAM_FLAG_MINI_STREAM      0x00000001

typedef struct tagSS_ROOT FAR *LPSS_ROOT;

typedef struct tagSS_STREAM
{
    LPSS_ROOT   lpstRoot;

    // Flags

    DWORD       dwFlags;

    // Stream ID

    DWORD       dwSID;

    // Starting sector and length of stream

    DWORD       dwStartSector;
    DWORD       dwStreamLen;

    // Cache seek forward info for stream

    DWORD       dwStreamBlk;    // Last block read/written
    DWORD       dwLink;         // Sector of last block read/written

    // BAT cache

    LPBYTE      lpbyCache;
} SS_STREAM_T, FAR *LPSS_STREAM, FAR * FAR *LPLPSS_STREAM;


//////////////////////////////////////////////////////////////////////
// Root OLE structured storage access definitions and structure
//////////////////////////////////////////////////////////////////////

#define SS_ROOT_FLAG_DIR_CACHE_MASK     0x0000000F
#define SS_ROOT_FLAG_DIR_CACHE_DEF      0x00000000
#define SS_ROOT_FLAG_DIR_CACHE_1        0x00000001
#define SS_ROOT_FLAG_DIR_CACHE_2        0x00000002
#define SS_ROOT_FLAG_DIR_CACHE_3        0x00000003
#define SS_ROOT_FLAG_DIR_CACHE_4        0x00000004
#define SS_ROOT_FLAG_DIR_CACHE_5        0x00000005
#define SS_ROOT_FLAG_DIR_CACHE_6        0x00000006
#define SS_ROOT_FLAG_DIR_CACHE_7        0x00000007
#define SS_ROOT_FLAG_DIR_CACHE_8        0x00000008
#define SS_ROOT_FLAG_DIR_CACHE_9        0x00000009
#define SS_ROOT_FLAG_DIR_CACHE_A        0x0000000A
#define SS_ROOT_FLAG_DIR_CACHE_B        0x0000000B
#define SS_ROOT_FLAG_DIR_CACHE_C        0x0000000C
#define SS_ROOT_FLAG_DIR_CACHE_D        0x0000000D
#define SS_ROOT_FLAG_DIR_CACHE_E        0x0000000E
#define SS_ROOT_FLAG_DIR_CACHE_F        0x0000000F

#define SS_ROOT_FLAG_FAT_CACHE_MASK     0x000000F0
#define SS_ROOT_FLAG_FAT_CACHE_DEF      0x00000000
#define SS_ROOT_FLAG_FAT_CACHE_1        0x00000010
#define SS_ROOT_FLAG_FAT_CACHE_2        0x00000020
#define SS_ROOT_FLAG_FAT_CACHE_3        0x00000030
#define SS_ROOT_FLAG_FAT_CACHE_4        0x00000040
#define SS_ROOT_FLAG_FAT_CACHE_5        0x00000050
#define SS_ROOT_FLAG_FAT_CACHE_6        0x00000060
#define SS_ROOT_FLAG_FAT_CACHE_7        0x00000070
#define SS_ROOT_FLAG_FAT_CACHE_8        0x00000080
#define SS_ROOT_FLAG_FAT_CACHE_9        0x00000090
#define SS_ROOT_FLAG_FAT_CACHE_A        0x000000A0
#define SS_ROOT_FLAG_FAT_CACHE_B        0x000000B0
#define SS_ROOT_FLAG_FAT_CACHE_C        0x000000C0
#define SS_ROOT_FLAG_FAT_CACHE_D        0x000000D0
#define SS_ROOT_FLAG_FAT_CACHE_E        0x000000E0
#define SS_ROOT_FLAG_FAT_CACHE_F        0x000000F0

#define SS_SECTOR_SIZE                  512
#define SS_MINI_SECTOR_SIZE             64
#define SS_DIR_ENTRIES_PER_SECTOR       4
#define SS_DIR_ENTRIES_SECTOR_SHIFT     2
#define SS_DIR_ENTRIES_ENTRY_MASK       3
#define SS_MAX_MINI_SECTOR_STREAM_LEN   4096
#define SS_SECTOR_SHIFT                 9
#define SS_SECTOR_OFFSET_MASK           0x1FF
#define SS_MINI_SECTOR_SHIFT            6
#define SS_MINI_SECTOR_OFFSET_MASK      0x3F

#define SS_LINKS_PER_FAT_SECTOR         128
#define SS_FAT_SECTOR_LINK_SHIFT        7
#define SS_LINKS_PER_DIFAT_SECTOR       127

#define SS_FAT_MAXREGSECT               0xFFFFFFFB
#define SS_FAT_DIFSECT                  0xFFFFFFFC
#define SS_FAT_FATSECT                  0xFFFFFFFD
#define SS_FAT_ENDOFCHAIN               0xFFFFFFFE
#define SS_FAT_FREESECT                 0xFFFFFFFF

#define SS_MINI_FAT_BAT_CACHE_SIZE      256
#define SS_MINI_SECTOR_BAT_CACHE_SIZE   256
#define SS_DIR_BAT_CACHE_SIZE           256

#define SS_DEF_STREAM_BAT_CACHE_SIZE    512

typedef struct tagSS_ROOT
{
    // User root cookie

    LPVOID                  lpvRootCookie;

    // Handle to open file

    LPVOID                  lpvFile;

    // Extra memory

    DWORD                   dwExtraMemorySize;
    LPVOID                  lpvExtraMemory;

    // Header of structured storage file

    SS_HDR_T                stHdr;

    // Cached directory blocks

    DWORD                   dwNumDirCacheSlots;
    DWORD                   dwDirCacheCurLRU;
    LPDWORD                 lpdwDirCacheSlotLRU;
    LPDWORD                 lpdwDirCacheSlotBlock;
    LPBYTE                  lpbyDirCacheBlocks;

    // Cached FAT blocks

    DWORD                   dwNumFATCacheSlots;
    DWORD                   dwFATCacheIsMiniTypeBitArray;
    DWORD                   dwFATCacheCurLRU;
    LPDWORD                 lpdwFATCacheSlotLRU;
    LPDWORD                 lpdwFATCacheSlotBlock;
    LPBYTE                  lpbyFATCacheBlocks;

    // Streams

    SS_STREAM_T             stMiniFATStream;
    SS_STREAM_T             stMiniSectorStream;
    SS_STREAM_T             stDirStream;

} SS_ROOT_T, FAR *LPSS_ROOT, FAR * FAR*LPLPSS_ROOT;


//////////////////////////////////////////////////////////////////////
// OLE structured storage access function prototypes
//////////////////////////////////////////////////////////////////////

SS_STATUS SSCreateRoot
(
    LPLPSS_ROOT         lplpstRoot,
    LPVOID              lpvRootCookie,
    LPVOID              lpvFile,
    DWORD               dwFlags
);

SS_STATUS SSDestroyRoot
(
    LPSS_ROOT           lpstRoot
);

SS_STATUS SSAllocStreamStruct
(
    LPSS_ROOT           lpstRoot,
    LPLPSS_STREAM       lplpstStream,
    DWORD               dwFlags
);

SS_STATUS SSFreeStreamStruct
(
    LPSS_STREAM         lpstStream
);

SS_STATUS SSOpenStreamAtIndex
(
    LPSS_STREAM         lpstStream,
    DWORD               dwID
);

SS_STATUS SSSeekRead
(
    LPSS_STREAM         lpstStream,
    DWORD               dwOffset,
    LPVOID              lpvBuffer,
    DWORD               dwBytesToRead,
    LPDWORD             lpdwBytesRead
);

SS_STATUS SSSeekWrite
(
    LPSS_STREAM         lpstStream,
    DWORD               dwOffset,
    LPVOID              lpvBuffer,
    DWORD               dwBytesToWrite,
    LPDWORD             lpdwBytesWritten
);

SS_STATUS SSWriteZeroes
(
    LPSS_STREAM         lpstStream,
    DWORD               dwStartOffset,
    DWORD               dwNumBytes
);

SS_STATUS SSCopyBytes
(
    LPSS_STREAM         lpstStream,
    DWORD               dwSrcOffset,
    DWORD               dwDstOffset,
    DWORD               dwNumBytes
);

DWORD SSStreamLen
(
    LPSS_STREAM     lpstStream
);

DWORD SSStreamID
(
    LPSS_STREAM     lpstStream
);

SS_STATUS SSGetDirEntry
(
    LPSS_ROOT           lpstRoot,
    DWORD               dwID,
    LPLPSS_DIR_ENTRY    lplpstDirEntry
);

// Enumeration callback function prototype

#define SS_ENUM_CB_STATUS_CONTINUE      0
#define SS_ENUM_CB_STATUS_OPEN          1
#define SS_ENUM_CB_STATUS_RETURN        2
#define SS_ENUM_CB_STATUS_ERROR         3

typedef int (*LPFNSS_ENUM_CB)
    (
        LPSS_DIR_ENTRY      lpstEntry,  // Ptr to the entry
        DWORD               dwIndex,    // The entry's index in the directory
        LPVOID              lpvCookie
    );

SS_STATUS SSEnumDirEntriesCB
(
    LPSS_ROOT           lpstRoot,
    LPFNSS_ENUM_CB      lpfnEnumCB,
    LPVOID              lpvCookie,
    LPDWORD             lpdwDirEntryNum,
    LPSS_STREAM         lpstStream
);

typedef struct tagSS_ENUM_SIBS
{
    DWORD           dwMaxWaitingSibs;
    DWORD           dwNumWaitingSibs;
    LPDWORD         lpdwSibs;
} SS_ENUM_SIBS_T, FAR *LPSS_ENUM_SIBS, FAR * FAR *LPLPSS_ENUM_SIBS;

#define SS_DEF_MAX_WAITING_SIBS     32

SS_STATUS SSAllocEnumSibsStruct
(
    LPSS_ROOT           lpstRoot,
    LPLPSS_ENUM_SIBS    lplpstEnumSibs,
    DWORD               dwMaxWaitingSibs
);

SS_STATUS SSFreeEnumSibsStruct
(
    LPSS_ROOT           lpstRoot,
    LPSS_ENUM_SIBS      lpstEnumSibs
);

void SSInitEnumSibsStruct
(
    LPSS_ENUM_SIBS      lpstEnumSibs,
    DWORD               dwRootID
);

SS_STATUS SSEnumSiblingEntriesCB
(
    LPSS_ROOT           lpstRoot,
    LPFNSS_ENUM_CB      lpfnEnumCB,
    LPVOID              lpvCookie,
    LPSS_ENUM_SIBS      lpstEnumSibs,
    LPSS_STREAM         lpstStream
);

SS_STATUS SSGetChildOfStorage
(
    LPSS_ROOT           lpstRoot,
    DWORD               dwStorageID,
    LPDWORD             lpdwChildID
);

SS_STATUS SSGetParentStorage
(
    LPSS_ROOT           lpstRoot,
    DWORD               dwEntryID,
    LPDWORD             lpdwParentID,
    LPDWORD             lpdwChildID
);

SS_STATUS SSUnlinkEntry
(
    LPSS_ROOT           lpstRoot,
    DWORD               dwParentStorageID,
    DWORD               dwUnlinkEntryID
);

SS_STATUS SSSetStreamLen
(
    LPSS_ROOT           lpstRoot,
    DWORD               dwEntryID,
    DWORD               dwNewLen
);

int SSWStrNCmp
(
    LPWORD              lpwStrA,
    LPWORD              lpwStrB,
    WORD                wMaxLen
);

int SSOpenStreamByNameCB
(
    LPSS_DIR_ENTRY      lpstEntry,
    DWORD               dwIndex,
    LPVOID              lpvCookie
);

BYTE SSToLower
(
    BYTE                byChar
);

BYTE SSToUpper
(
    BYTE                byChar
);

BOOL SSGetNamedSiblingID
(
    LPSS_ROOT           lpstRoot,
    DWORD               dwDocStreamID,
    LPBYTE              lpabywszSiblingName,
    LPDWORD             lpdwSiblingID
);

BOOL SSClearSummaryInfo
(
    LPSS_STREAM         lpstStream
);

BOOL SSDocSumInfoRemDigiSig
(
    LPSS_STREAM         lpstStream,
    LPBOOL              lpbFound
);

BOOL SSGetDocSumInfoStream
(
    LPSS_ROOT           lpstRoot,
    DWORD               dwDocStreamID,
    LPSS_STREAM         lpstStream
);

DWORD SSEndianDWORD
(
    LPBYTE      lpabyDWORD,
    BOOL        bLitEnd
);

WORD SSEndianWORD
(
    LPBYTE      lpabyWORD,
    BOOL        bLitEnd
);

BOOL SSGetStreamIndexCB
(
    LPSS_ROOT           lpstRoot,
    LPFNSS_ENUM_CB      lpfnEnumCB,
    DWORD               dwChildEntry,
    LPBYTE              lpabypsName,
    LPDWORD             lpdwIndex
);

#endif // #ifndef _OLESSAPI_H_

