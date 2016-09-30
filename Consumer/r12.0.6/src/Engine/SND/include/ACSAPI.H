// Copyright 1998 Symantec, SABU
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/acsapi.h_v   1.3   08 Dec 1998 12:34:00   DCHI  $
//
// Description:
//  ACSAPI header file
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/acsapi.h_v  $
// 
//    Rev 1.3   08 Dec 1998 12:34:00   DCHI
// Added last accessed page optimization fields to A97_OBJ_ACCESS_T.
// 
//    Rev 1.2   29 Apr 1998 15:57:38   DCHI
// Added Access 2.0 API
// 
//    Rev 1.1   20 Apr 1998 13:48:46   DCHI
// Modifications for handling of compacted/repaired databases.
// 
//    Rev 1.0   17 Apr 1998 13:40:42   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _ACSAPI_H_

#define _ACSAPI_H_

#define A97_PAGE_SIZE               (0x800)

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

BOOL ACSMemoryAlloc
(
    LPVOID      lpvRootCookie,
    DWORD       dwNumBytes,
    LPVOID FAR *lplpvBuffer
);

BOOL ACSMemoryFree
(
    LPVOID      lpvRootCookie,
    LPVOID      lpvBuffer
);

#define ACS_SEEK_SET    0
#define ACS_SEEK_CUR    1
#define ACS_SEEK_END    2

BOOL ACSFileSeek
(
    LPVOID      lpvRootCookie,
    LPVOID      lpvFile,
    long        lOffset,
    int         nWhence,
    LPLONG      lplNewOffset
);

BOOL ACSFileRead
(
    LPVOID      lpvRootCookie,
    LPVOID      lpvFile,
    LPVOID      lpvBuffer,
    DWORD       dwNumBytes,
    LPDWORD     lpdwBytesRead
);


BOOL ACSFileWrite
(
    LPVOID      lpvRootCookie,
    LPVOID      lpvFile,
    LPVOID      lpvBuffer,
    DWORD       dwNumBytes,
    LPDWORD     lpdwBytesWritten
);

BOOL ACSFileTruncate
(
    LPVOID      lpvRootCookie,
    LPVOID      lpvFile
);

#ifdef SYM_NLM
BOOL ACSProgress
(
    LPVOID      lpvRootCookie
);
#endif


//////////////////////////////////////////////////////////////////////
// Access 97
//////////////////////////////////////////////////////////////////////

#define A97_OBJECTS_MASTER_PAGE         0x02
#define A97_ACES_MASTER_PAGE            0x03
#define A97_QUERIES_MASTER_PAGE         0x04
#define A97_RELATIONSHIPS_MASTER_PAGE   0x05

#define A97_MAX_ITEM_NAME_LEN           64

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD) || defined(SYM_400)
#include "pshpack1.h"
#elif defined(__MACINTOSH__) || defined(SYM_AIX) 
#pragma options align=packed
#else
#pragma pack(1)
#endif

#define A97_P01_MAX_ITEMS           ((0x800 - 0x0A) / sizeof(WORD))

typedef struct tagA97_P01
{
    BYTE        abyType[2];                         // 0x00
    WORD        wIndexSize;                         // 0x02
    BYTE        abySubType[4];                      // 0x04
    WORD        wItemCount;                         // 0x08

    // WORD        awItemOffsets[wItemCount];
    WORD        awItemOffsets[A97_P01_MAX_ITEMS];   // 0x0A
} A97_P01_T, FAR *LPA97_P01;

#define A97_P02_DATA_SIZE           (0x7D5)

typedef struct tagA97_P02
{
    BYTE        abyType[2];                         // 0x00
    BYTE        abyVC[2];                           // 0x02
    BYTE        abySubType[4];                      // 0x04
    DWORD       dwTableSize;                        // 0x08
    DWORD       dwUnknown0;                         // 0x0C
    DWORD       dwUnknown1;                         // 0x10
    BYTE        byUnknown;                          // 0x14
    WORD        wCount0;                            // 0x15
    WORD        wCount1;                            // 0x17
    WORD        wCount2;                            // 0x19
    DWORD       dwCount3;                           // 0x1B
    DWORD       dwCount4;                           // 0x1F
    DWORD       dwValue0;                           // 0x23
    DWORD       dwValue1;                           // 0x27

    BYTE        abyData[A97_P02_DATA_SIZE];         // 0x2B
    // DWORD    adwArray0[dwCount4][2];
    // BYTE     abyArray1[wCount0][0x12];
    // BYTE     abypsStr0[wCount0];
    // BYTE     abyArray2[dwCount4][0x27];
    // BYTE     abyArray3[dwCount3][0x14];
    // BYTE     abypsStr1[dwCount3];
    // BYTE     abyEnd[2] = { 0xFF, 0xFF };
} A97_P02_T, FAR *LPA97_P02;

#define A97_P04_TABLE_SIZE          (0xE1)
#define A97_P04_DATA_SIZE           (0x708)

typedef struct tagA97_P04
{
    BYTE        abyType[2];                         // 0x00
    WORD        wEmptySpace;                        // 0x02
    BYTE        abySubType[4];                      // 0x04
    DWORD       dwPrevPage;                         // 0x08
    DWORD       dwNextPage;                         // 0x0C
    DWORD       dwBranchPage;                       // 0x10
    BYTE        byCommonPrefixLen;                  // 0x14
    BYTE        abyUnknown[2];                      // 0x15
    BYTE        abyTable[A97_P04_TABLE_SIZE];       // 0x17
    BYTE        abyData[A97_P04_DATA_SIZE];         // 0xF8
} A97_P04_T, FAR *LPA97_P04;

typedef struct tagA97OBJECTID_OBJ_INFO
{
    BYTE    byInfoSize;      // 00: Size of object info
    BYTE    byObjectOffset0; // 01: Object size and page
    BYTE    byObjectOffset1; // 02: Object size and page
    BYTE    abyUnknown0[6];  // 03:
    BYTE    byDescOffset;    // 09: Description size and page
    BYTE    byNameEndOffset; // 0A: Object name end offset
    BYTE    byNameBegOffset; // 0B: Object name begin offset
    BYTE    abyUnknown1[4];  // 0C:
} A97OBJECTID_OBJ_INFO_T, FAR *LPA97OBJECTID_OBJ_INFO;

typedef struct tagA97MODULE_OBJ_INFO
{
    BYTE    byInfoSize;      // 00: Size of object info
    BYTE    abyUnknown0[2];  // 01:
    union
    {
        struct
        {
            BYTE    byObjectOffset;  // 03: Object size and page
            BYTE    abyUnknown1[3];  // 04:
            BYTE    byNameEndOffset; // 07: Object name end offset
            BYTE    byNameBegOffset; // 08: Object name begin offset
            BYTE    abyUnknown2[2];  // 09:
        } a;

        struct
        {
            BYTE    byTypeOffset;    // 03:
            BYTE    byNameEndOffset; // 04: Object name end offset
            BYTE    byNameBegOffset; // 05: Object name begin offset
            BYTE    byObjectOffset;  // 06: Object size and page
            BYTE    abyUnknown1[4];  // 07:
        } b;
    } i;
} A97MODULE_OBJ_INFO_T, FAR *LPA97MODULE_OBJ_INFO;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD) || defined(SYM_400)
#include "poppack.h"
#elif defined(__MACINTOSH__) || defined(SYM_AIX) 
#pragma options align=reset
#else
#pragma pack()
#endif

typedef struct tagA97_T
{
    // User root cookie

    LPVOID                  lpvRootCookie;

    // Handle to open file

    LPVOID                  lpvFile;

    // Current page buffer

    BYTE                    abyPage[A97_PAGE_SIZE];

    // Temporary page buffer

    BYTE                    abyTempPage[A97_PAGE_SIZE];
} A97_T, FAR *LPA97, FAR * FAR *LPLPA97;

#define A97_OBJ_MAX_CACHED_PAGES        4

typedef struct tagA97_OBJ_ACCESS
{
    LPA97           lpstA97;

    DWORD           dwSize;

    int             nNumCachedPages;
    WORD            awPageSize[A97_OBJ_MAX_CACHED_PAGES];
    DWORD           adwPageOffset[A97_OBJ_MAX_CACHED_PAGES];

    // Last accessed page

    WORD            wLastPageSize;
    DWORD           dwLastPageRelOffset;
    DWORD           dwLastPageOffset;
    int             nLastNextIndex;
    DWORD           dwLastNextPage;

    int             nNextIndex;
    DWORD           dwNextPage;
} A97_OBJ_ACCESS_T, FAR *LPA97_OBJ_ACCESS;

////////////////////////////////////////////////////////////
// Root creation and destruction

BOOL A97Create
(
    LPLPA97             lplpstA97,
    LPVOID              lpvRootCookie,
    LPVOID              lpvFile
);

BOOL A97Destroy
(
    LPA97               lpstA97
);

////////////////////////////////////////////////////////////
// Index

extern BYTE gabypsA97Name[5];
extern BYTE gabypsA97ParentId[9];

BOOL A97P02GetFirstIndexPage
(
    LPA97               lpstA97,
    DWORD               dwMasterPage,
    LPBYTE              lpabypsFieldName,
    LPDWORD             lpdwPage
);

// Index character set translation

extern BYTE gabyA97XlatASCIItoA97[0x80];

void A97XlatASCIItoA97
(
    LPBYTE              lpabyBuf,
    int                 nBufLen
);

extern BYTE gabyA97XlatA97toASCII[0x80];

void A97XlatA97toASCII
(
    LPBYTE              lpabyBuf,
    int                 nBufLen
);

// Index callback function prototypes

typedef BOOL (*LPFNA97PrefixFlags)
(
    LPBYTE              lpabyKeyValue,
    int                 nPrefixLen,
    LPINT               lpnPrefixFlags
);

typedef BOOL (*LPFNA97KeyValueLen)
(
    LPBYTE              lpabyKeyValue,
    int                 nPrefixFlags,
    int                 nMaxOffset,
    LPINT               lpnLen
);

// Object ID index callback functions

BOOL A97ObjectIDPrefixFlags
(
    LPBYTE              lpabyKeyValue,
    int                 nPrefixLen,
    LPINT               lpnPrefixFlags
);

BOOL A97ObjectIDKeyValueLen
(
    LPBYTE              lpabyKeyValue,
    int                 nPrefixFlags,
    int                 nMaxOffset,
    LPINT               lpnLen
);

// Indexing

BOOL A97P04FindIndexEntry
(
    LPA97_P04           lpstPage,
    LPFNA97PrefixFlags  lpfnPrefixFlags,
    LPFNA97KeyValueLen  lpfnKeyValueLen,
    int                 nKeyValueLen,
    LPBYTE              lpabyKeyValue,
    LPDWORD             lpdwPage,
    LPINT               lpnIndex,
    LPINT               lpnResult
);

BOOL A97P04FindIndexedKeyValue
(
    LPA97               lpstA97,
    DWORD               dwPage,
    LPFNA97PrefixFlags  lpfnPrefixFlags,
    LPFNA97KeyValueLen  lpfnKeyValueLen,
    int                 nKeyValueLen,
    LPBYTE              lpabyKeyValue,
    LPDWORD             lpdwPage,
    LPINT               lpnIndex,
    LPBOOL              lpbFound
);

BOOL A97GetFirstIndexPagePrefix
(
    LPA97               lpstA97,
    DWORD               dwPage,
    LPFNA97PrefixFlags  lpfnPrefixFlags,
    LPFNA97KeyValueLen  lpfnKeyValueLen,
    int                 nKeyValueLen,
    LPBYTE              lpabyKeyValue,
    LPDWORD             lpdwPage
);

typedef BOOL (*LPFNA97ItemMatch)
(
    LPVOID              lpvItemListCookie,
    LPBYTE              lpabyPrefix,
    int                 nPrefixLen,
    LPBYTE              lpabyItem,
    int                 nItemLen
);

BOOL A97P04GetItemList
(
    LPA97_P04           lpstPage,
    LPFNA97PrefixFlags  lpfnPrefixFlags,
    LPFNA97KeyValueLen  lpfnKeyValueLen,
    LPFNA97ItemMatch    lpfnItemMatch,
    LPVOID              lpvItemListCookie,
    LPDWORD             lpdwPage,
    LPDWORD             lpadwItems,
    LPDWORD             lpdwNextPage
);

// Object deletion

BOOL A97P04DeleteEntry
(
    LPA97               lpstA97,
    DWORD               dwPage,
    LPFNA97PrefixFlags  lpfnPrefixFlags,
    LPFNA97KeyValueLen  lpfnKeyValueLen,
    int                 nKeyValueLen,
    LPBYTE              lpabyKeyValue
);

BOOL A97P01DeleteItemAtIndex
(
    LPA97               lpstA97,
    DWORD               dwPage,
    int                 nItemIndex
);

// Object enumeration callbacks

typedef BOOL (*LPFNA97P04ItemMatch)
(
    LPVOID              lpvItemListCookie,
    LPBYTE              lpabyPrefix,
    int                 nPrefixLen,
    LPBYTE              lpabyItem,
    int                 nItemLen
);

typedef BOOL (*LPFNA97ObjectParse)
(
    LPBYTE              lpabyObject,
    int                 nObjectSize,
    LPBYTE FAR *        lplpabypsszName,
    LPDWORD             lpdwSize,
    LPDWORD             lpdwPage,
    LPINT               lpnIndex
);

typedef BOOL (*LPFNA97ObjectEnumCB)
(
    LPA97               lpstA97,
    LPVOID              lpvObjectEnumCookie,
    LPBYTE              lpabypsszObjectName,
    DWORD               dwSize,
    DWORD               dwPage,
    int                 nIndex
);

// Macro enumeration

BOOL A97P04MacroItemMatch
(
    LPVOID              lpvItemListCookie,
    LPBYTE              lpabyPrefix,
    int                 nPrefixLen,
    LPBYTE              lpabyItem,
    int                 nItemLen
);

BOOL A97EnumMacros
(
    LPA97               lpstA97,
    LPFNA97ObjectEnumCB lpfnEnumCB,
    LPVOID              lpvEnumCBCookie
);

// VBA module enumeration

BOOL A97GetVBAMasterPage
(
    LPA97               lpstA97,
    BOOL                b95,
    LPDWORD             lpdwPage
);

BOOL A97VBAModulePrefixFlags
(
    LPBYTE              lpabyKeyValue,
    int                 nPrefixLen,
    LPINT               lpnPrefixFlags
);

BOOL A97VBAModuleKeyValueLen
(
    LPBYTE              lpabyKeyValue,
    int                 nPrefixFlags,
    int                 nMaxOffset,
    LPINT               lpnLen
);

BOOL A97P04VBAModuleItemMatch
(
    LPVOID              lpvItemListCookie,
    LPBYTE              lpabyPrefix,
    int                 nPrefixLen,
    LPBYTE              lpabyItem,
    int                 nItemLen
);

BOOL A97EnumVBAModules
(
    LPA97               lpstA97,
    BOOL                b95,
    LPFNA97ObjectEnumCB lpfnEnumCB,
    LPVOID              lpvEnumCBCookie
);

// Object access

BOOL A97ObjAccessInit
(
    LPA97               lpstA97,
    DWORD               dwSize,
    DWORD               dwPage,
    int                 nIndex,
    LPA97_OBJ_ACCESS    lpstObj
);

BOOL A97ObjAccessRead
(
    LPA97_OBJ_ACCESS    lpstObj,
    DWORD               dwOffset,
    LPVOID              lpvBuffer,
    DWORD               dwNumBytes,
    LPDWORD             lpdwBytesRead
);

// Compressed VBA access

#define A97LZNT_MAX_COPY_DISPLACEMENT   4096
#define A97LZNT_BUF_SIZE                64

typedef struct tagA97LZNT_BUF
{
    LPA97_OBJ_ACCESS    lpstObj;

    BYTE            abyBuf[A97LZNT_BUF_SIZE];
    int             nNumBytesInBuf;
    int             nNextByteI;

    DWORD           dwNextReadOffset;
    DWORD           dwEndReadOffsetPlus1;
} A97LZNT_BUF_T, FAR *LPA97LZNT_BUF;

typedef struct tagA97LZNT
{
    DWORD           dwFirstChunkOffset;

    DWORD           dwSize;

    BOOL            bCompressed;
    BOOL            bChunkIsCompressed;

    BYTE            abyUncompressed[A97LZNT_MAX_COPY_DISPLACEMENT];

    // stLZNTBuf buffers the compressed data from the stream

    A97LZNT_BUF_T   stLZNTBuf;

    // i is the index of the byte in the uncompressed buffer
    //  with the smallest offset in the stream

    int             i;

    // dwOffsetAtI is the offset in the stream of the byte
    //  at index i

    DWORD           dwOffsetAtI;

    // lpbyNextDst points to the next location in abyUncompressed
    //  into which to continue reading bytes from the stream

    LPBYTE          lpbyNextDst;

    // nCopyLen will be non-zero if the last copy operation
    //  was partially completed and nCopyDisplacement will hold the
    //  corresponding copy displacement

    int             nCopyLen;
    int             nCopyDisplacement;

    // byFlag is the value of the last flag byte read

    BYTE            byFlag;

    // The current bit of the flags variable

    int             nFlagBit;

    // nFormat is the current assumed format of the copy descriptor

    int             nFormat;

    // nChunkBytesLeft holds the number of bytes left in the
    //  current chunk

    int             nChunkBytesLeft;

    // dwNumUncompressedChunkBytes holds the number of bytes
    //  decompressed so far from the current chunk

    DWORD           dwNumUncompressedChunkBytes;

} A97LZNT_T, FAR *LPA97LZNT, FAR * FAR *LPLPA97LZNT;

BOOL A97LZNTModuleInit
(
    LPA97_OBJ_ACCESS    lpstObj,
    DWORD               dwOffset,
    LPA97LZNT           lpstLZNT
);

int A97LZNTGetBytes
(
    LPA97LZNT           lpstLZNT,
    DWORD               dwOffset,
    int                 nNumBytes,
    LPBYTE              lpbyDst
);

////////////////////////////////////////////////////////////
// Macro actions

#define A97MACROAddMenu                 0x01
#define A97MACROApplyFilter             0x03
#define A97MACROBeep                    0x04
#define A97MACROCancelEvent             0x05
#define A97MACROClose                   0x06
#define A97MACROCopyObject              0x07
#define A97MACRODeleteObject            0x34
#define A97MACROEcho                    0x09
#define A97MACROFindNext                0x0B
#define A97MACROFindRecord              0x0C
#define A97MACROGoToControl             0x0D
#define A97MACROGoToPage                0x0E
#define A97MACROGoToRecord              0x0F
#define A97MACROHourglass               0x11
#define A97MACROMaximize                0x13
#define A97MACROMinimize                0x14
#define A97MACROMoveSize                0x15
#define A97MACROMsgBox                  0x16
#define A97MACROOpenForm                0x17
#define A97MACROOpenModule              0x35
#define A97MACROOpenQuery               0x18
#define A97MACROOpenReport              0x2E
#define A97MACROOpenTable               0x19
#define A97MACROOutputTo                0x33
#define A97MACROPrintOut                0x1A
#define A97MACROQuit                    0x1B
#define A97MACRORename                  0x1E
#define A97MACRORepaintObject           0x1D
#define A97MACRORequery                 0x1C
#define A97MACRORestore                 0x1F
#define A97MACRORunApp                  0x20
#define A97MACRORunCode                 0x21
#define A97MACRORunCommand              0x3A
#define A97MACRORunMacro                0x22
#define A97MACRORunSQL                  0x23
#define A97MACROSave                    0x38
#define A97MACROSelectObject            0x25
#define A97MACROSendKeys                0x26
#define A97MACROSendObject              0x36
#define A97MACROSetMenuItem             0x39
#define A97MACROSetValue                0x28
#define A97MACROSetWarnings             0x29
#define A97MACROShowAllRecords          0x2B
#define A97MACROShowToolbar             0x37
#define A97MACROStopAllMacros           0x2C
#define A97MACROStopMacro               0x2D
#define A97MACROTransferDatabase        0x2F
#define A97MACROTransferSpreadsheet     0x30
#define A97MACROTransferText            0x31

#define A97MACRO_MAX_ARGS               10

#define A97MACRO_HDR_ARG_COUNT_OFFSET   0x1C

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD) || defined(SYM_400)
#include "pshpack1.h"
#elif defined(__MACINTOSH__) || defined(SYM_AIX) 
#pragma options align=packed
#else
#pragma pack(1)
#endif

typedef struct tagA97MACRO_ACTION
{
    BYTE    abyUnknown0[2];                     // 0x00
    BYTE    byAction;                           // 0x02
    BYTE    abyUnknown1[5];                     // 0x03
    WORD    wCommentOffset;                     // 0x08
    WORD    wConditionOffset;                   // 0x0A
    WORD    wMacroNameOffset;                   // 0x0C
    WORD    awArgOffsets[A97MACRO_MAX_ARGS];    // 0x0E
    WORD    wArgSize;                           // 0x22
    // BYTE abyArgs[wArgSize];                  // 0x24
} A97MACRO_ACTION_T, FAR *LPA97MACRO_ACTION;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD) || defined(SYM_400)
#include "poppack.h"
#elif defined(__MACINTOSH__) || defined(SYM_AIX) 
#pragma options align=reset
#else
#pragma pack()
#endif


//////////////////////////////////////////////////////////////////////
// Access 2.0
//////////////////////////////////////////////////////////////////////

#define A2_PAGE_SIZE                (0x800)

#define A2_OBJECTS_MASTER_PAGE          0x03

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD) || defined(SYM_400)
#include "pshpack1.h"
#elif defined(__MACINTOSH__) || defined(SYM_AIX) 
#pragma options align=packed
#else
#pragma pack(1)
#endif

#define A2_P06_MAX_ITEMS        ((A2_PAGE_SIZE - 0x14) / sizeof(WORD))

typedef struct tagA2_P06
{
    BYTE        abyType[2];                         // 0x00
    BYTE        abyUnknown0[6];                     // 0x02
    WORD        wItemCount;                         // 0x08
    WORD        wIndexSize;                         // 0x0A
    DWORD       dwNextPage;                         // 0x0C
    DWORD       dwPrevPage;                         // 0x10

    // WORD        awItemOffsets[wItemCount];
    WORD        awItemOffsets[A2_P06_MAX_ITEMS];    // 0x14
} A2_P06_T, FAR *LPA2_P06;

#define A2_P04_MAX_ITEMS        ((A2_PAGE_SIZE - 0x18) / sizeof(WORD))

typedef struct tagA2_P04
{
    BYTE        abyType[2];                         // 0x00
    BYTE        abyUnknown0[6];                     // 0x02
    WORD        wUnknown1;                          // 0x08
    WORD        wItemCount;                         // 0x0A
    WORD        wBranchPageOffset;                  // 0x0C
    WORD        wUnknown3;                          // 0x0E
    DWORD       dwNextPage;                         // 0x10
    DWORD       dwPrevPage;                         // 0x14

    // WORD        awItemOffsets[wItemCount];
    WORD        awItemOffsets[A2_P04_MAX_ITEMS];    // 0x18
} A2_P04_T, FAR *LPA2_P04;

typedef struct tagA2_P05
{
    BYTE        abyType[2];                         // 0x00
    BYTE        abyUnknown0[6];                     // 0x02
    WORD        wUnknown1;                          // 0x08
    WORD        wUnknown2;                          // 0x0A
    DWORD       dwFirstPage;                        // 0x0C
    DWORD       dwLastPage;                         // 0x10
    DWORD       dwUnknown3;                         // 0x14
    DWORD       dwUnknown4;                         // 0x18
    DWORD       dwUnknown5;                         // 0x1C
    DWORD       dwPageCount;                        // 0x20
    DWORD       dwTotalItemCount;                   // 0x24
    DWORD       dwUnknown6;                         // 0x28
    DWORD       dwFirstIndexPage;                   // 0x2C
    DWORD       dwUnknown8;                         // 0x30
    DWORD       dwActualItemCount;                  // 0x34
    DWORD       dwUnknown9;                         // 0x38
    DWORD       dwUnknown10;                        // 0x3C
    BYTE        abyData[A2_PAGE_SIZE - 0x40];       // 0x40
} A2_P05_T, FAR *LPA2_P05;

typedef struct tagA2OBJ_INFO
{
    BYTE        byInfoSize;      // 00:
    BYTE        abyUnknown0[2];  // 02:
    BYTE        byNameEndOffset; // 04: Object name end offset
    BYTE        byNameBegOffset; // 05: Object name begin offset
    BYTE        abyUnknown2[3];  // 06:
} A2OBJ_INFO_T, FAR *LPA2OBJ_INFO;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD) || defined(SYM_400)
#include "poppack.h"
#elif defined(__MACINTOSH__) || defined(SYM_AIX) 
#pragma options align=reset
#else
#pragma pack()
#endif

typedef struct tagA2_T
{
    // User root cookie

    LPVOID                  lpvRootCookie;

    // Handle to open file

    LPVOID                  lpvFile;

    // Current page buffer

    BYTE                    abyPage[A2_PAGE_SIZE];

    // Temporary page buffer

    BYTE                    abyTempPage[A2_PAGE_SIZE];

} A2_T, FAR *LPA2, FAR * FAR *LPLPA2;

#define A2_OBJ_MAX_CACHED_PAGES        4

typedef struct tagA2_OBJ_ACCESS
{
    LPA2            lpstA2;

    DWORD           dwSize;

    int             nNumCachedPages;
    WORD            awPageSize[A2_OBJ_MAX_CACHED_PAGES];
    DWORD           adwPageOffset[A2_OBJ_MAX_CACHED_PAGES];

    int             nNextIndex;
    DWORD           dwNextPage;
} A2_OBJ_ACCESS_T, FAR *LPA2_OBJ_ACCESS;

////////////////////////////////////////////////////////////
// Macro actions

#define A2MACRO                         0x00
#define A2MACROAddMenu                  0x01
#define A2MACROApplyFilter              0x03
#define A2MACROBeep                     0x04
#define A2MACROCancelEvent              0x05
#define A2MACROClose                    0x06
#define A2MACROCopyObject               0x07
#define A2MACRODeleteObject             0x34
#define A2MACRODoMenuItem               0x08
#define A2MACROEcho                     0x09
#define A2MACROFindNext                 0x0B
#define A2MACROFindRecord               0x0C
#define A2MACROGoToControl              0x0D
#define A2MACROGoToPage                 0x0E
#define A2MACROGoToRecord               0x0F
#define A2MACROHourglass                0x11
#define A2MACROMaximize                 0x13
#define A2MACROMinimize                 0x14
#define A2MACROMoveSize                 0x15
#define A2MACROMsgBox                   0x16
#define A2MACROOpenForm                 0x17
#define A2MACROOpenModule               0x35
#define A2MACROOpenQuery                0x18
#define A2MACROOpenReport               0x2E
#define A2MACROOpenTable                0x19
#define A2MACROOutputTo                 0x33
#define A2MACROPrint                    0x1A
#define A2MACROQuit                     0x1B
#define A2MACRORename                   0x1E
#define A2MACRORepaintObject            0x1D
#define A2MACRORequery                  0x1C
#define A2MACRORestore                  0x1F
#define A2MACRORunApp                   0x20
#define A2MACRORunCode                  0x21
#define A2MACRORunMacro                 0x22
#define A2MACRORunSQL                   0x23
#define A2MACROSelectObject             0x25
#define A2MACROSendKeys                 0x26
#define A2MACROSendObject               0x36
#define A2MACROSetValue                 0x28
#define A2MACROSetWarnings              0x29
#define A2MACROShowAllRecords           0x2B
#define A2MACROShowToolbar              0x37
#define A2MACROStopAllMacros            0x2C
#define A2MACROStopMacro                0x2D
#define A2MACROTransferDatabase         0x2F
#define A2MACROTransferSpreadsheet      0x30
#define A2MACROTransferText             0x31

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD) || defined(SYM_400)
#include "pshpack1.h"
#elif defined(__MACINTOSH__) || defined(SYM_AIX) 
#pragma options align=packed
#else
#pragma pack(1)
#endif

typedef struct tagA2MACRO_ACTION
{
    WORD    wItemSize;                          // 0x00
    BYTE    abyUnknown0[2];                     // 0x02
    BYTE    byAction;                           // 0x04
    BYTE    byUnknown1;                         // 0x05
    WORD    wLineNumber;                        // 0x06
    BYTE    abyUnknown2[2];                     // 0x08
    // BYTE abyArgs[byArgSize];                 // 0x24
} A2MACRO_ACTION_T, FAR *LPA2MACRO_ACTION;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD) || defined(SYM_400)
#include "poppack.h"
#elif defined(__MACINTOSH__) || defined(SYM_AIX) 
#pragma options align=reset
#else
#pragma pack()
#endif

// Object enumeration callbacks

typedef BOOL (*LPFNA2ItemMatch)
(
    LPVOID              lpvItemListCookie,
    LPBYTE              lpabyPrefixValue,
    int                 nPrefixLen,
    LPBYTE              lpabyItem,
    int                 nItemLen
);

typedef BOOL (*LPFNA2ObjectParse)
(
    LPBYTE              lpabyObject,
    int                 nObjectSize,
    LPBYTE FAR *        lplpabypsszName,
    LPDWORD             lpdwSize,
    LPDWORD             lpdwPage,
    LPINT               lpnIndex
);

typedef BOOL (*LPFNA2ObjectEnumCB)
(
    LPA2                lpstA2,
    LPVOID              lpvObjectEnumCookie,
    LPBYTE              lpabypsszObjectName,
    DWORD               dwSize,
    DWORD               dwPage,
    int                 nIndex
);

BOOL A2Create
(
    LPLPA2              lplpstA2,
    LPVOID              lpvRootCookie,
    LPVOID              lpvFile
);

BOOL A2Destroy
(
    LPA2               lpstA2
);

BOOL A2P05GetFirstIndexPage
(
    LPA2                lpstA2,
    DWORD               dwMasterPage,
    LPDWORD             lpdwPage
);

BOOL A2P04FindIndexEntry
(
    LPA2_P04            lpstPage,
    BOOL                bWeirdIndex,
    int                 nKeyValueLen,
    LPBYTE              lpabyKeyValue,
    LPDWORD             lpdwPage,
    LPINT               lpnIndex,
    LPINT               lpnResult
);

BOOL A2P03CheckForWeirdIndex
(
    LPA2                lpstA2,
    LPA2_P04            lpstPage,
    LPBOOL              lpbWeirdIndex
);

BOOL A2P04FindIndexedKeyValue
(
    LPA2                lpstA2,
    DWORD               dwPage,
    int                 nKeyValueLen,
    LPBYTE              lpabyKeyValue,
    LPDWORD             lpdwPage,
    LPINT               lpnIndex,
    LPBOOL              lpbFound
);

BOOL A2GetFirstIndexPagePrefix
(
    LPA2                lpstA2,
    DWORD               dwPage,
    int                 nKeyValueLen,
    LPBYTE              lpabyKeyValue,
    LPDWORD             lpdwPage
);

BOOL A2P04GetItemList
(
    LPA2_P04            lpstPage,
    LPFNA2ItemMatch     lpfnItemMatch,
    LPBYTE              lpabyPrefixValue,
    int                 nPrefixLen,
    LPVOID              lpvItemListCookie,
    LPDWORD             lpdwPage,
    LPDWORD             lpadwItems,
    LPDWORD             lpdwNextPage
);

BOOL A2EnumItems
(
    LPA2                lpstA2,
    DWORD               dwPage,
    LPFNA2ItemMatch     lpfnItemMatch,
    LPBYTE              lpabyPrefixValue,
    int                 nPrefixLen,
    LPFNA2ObjectParse   lpfnObjectParse,
    LPFNA2ObjectEnumCB  lpfnEnumCB,
    LPVOID              lpvEnumCBCookie
);

BOOL A2P04GenericItemMatch
(
    LPVOID              lpvItemListCookie,
    LPBYTE              lpabyPrefixValue,
    int                 nPrefixLen,
    LPBYTE              lpabyItem,
    int                 nItemLen
);

BOOL A2ModuleObjectParse
(
    LPBYTE              lpabyObject,
    int                 nObjectSize,
    LPBYTE FAR *        lplpabypsszName,
    LPDWORD             lpdwSize,
    LPDWORD             lpdwPage,
    LPINT               lpnIndex
);

BOOL A2GetVBModulePrefixID
(
    LPA2                lpstA2,
    DWORD               dwPage,
    LPBYTE              lpabyPrefix
);

BOOL A2EnumVBModules
(
    LPA2                lpstA2,
    LPFNA2ObjectEnumCB  lpfnEnumCB,
    LPVOID              lpvEnumCBCookie
);

BOOL A2P05GetFirstDataPage
(
    LPA2                lpstA2,
    DWORD               dwMasterPage,
    LPDWORD             lpdwPage
);

BOOL A2GetFirstMacrosDataPage
(
    LPA2                lpstA2,
    LPDWORD             lpdwPage
);

BOOL A2ObjAccessInit
(
    LPA2                lpstA2,
    DWORD               dwSize,
    DWORD               dwPage,
    int                 nIndex,
    LPA2_OBJ_ACCESS     lpstObj
);

BOOL A2ObjAccessRead
(
    LPA2_OBJ_ACCESS     lpstObj,
    DWORD               dwOffset,
    LPVOID              lpvBuffer,
    DWORD               dwNumBytes,
    LPDWORD             lpdwBytesRead
);

////////////////////////////////////////////////////////////
// Macro line info

#define A2MACROE_NUM_PARAMS     10

#define A2MACRO_MAX_INFO_ITEMS  14

#define A2MACRO_MAINMACRONAME   0
#define A2MACRO_MACRONAME       1
#define A2MACRO_CONDITION       2
#define A2MACRO_COMMENT         3
#define A2MACRO_PARAM1          4
#define A2MACRO_PARAM2          5
#define A2MACRO_PARAM3          6
#define A2MACRO_PARAM4          7
#define A2MACRO_PARAM5          8
#define A2MACRO_PARAM6          9
#define A2MACRO_PARAM7          10
#define A2MACRO_PARAM8          11
#define A2MACRO_PARAM9          12
#define A2MACRO_PARAM10         13

typedef struct tagA2MACRO_LINE_INFO
{
    int         anOffsets[A2MACRO_MAX_INFO_ITEMS];
    int         anSizes[A2MACRO_MAX_INFO_ITEMS];
} A2MACRO_LINE_INFO_T, FAR *LPA2MACRO_LINE_INFO;

typedef struct tagA2MACROE
{
    BYTE        byInfoSize;
    BYTE        byMainMacroNameOffset;
    BYTE        byMacroNameOffset;
    BYTE        byConditionOffset;
    BYTE        byCommentOffset;
    BYTE        abyParamOffsets[A2MACROE_NUM_PARAMS];
    BYTE        byOffsetsSize;
    BYTE        byUnknown;
} A2MACROE_T, FAR *LPA2MACROE;

BOOL A2GetMacroLineInfo
(
    LPBYTE              lpabyMacroLine,
    int                 nSize,
    LPA2MACRO_LINE_INFO lpstInfo
);

DWORD A2CRCMacroLine
(
    DWORD               dwCRC,
    LPBYTE              lpabyMacroLine,
    LPA2MACRO_LINE_INFO lpstInfo,
    LPDWORD             lpdwCRCSize
);

#endif // #ifndef _ACSAPI_H_

