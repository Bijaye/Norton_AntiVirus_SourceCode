// Copyright 1998 Symantec, SABU
//************************************************************************
//
// $Header:   S:/ENGINE15/VCS/strscan.h_v   1.1   11 Aug 1998 17:33:42   CNACHEN  $
//
// Description:
//  IBM String scanning data file
//
//************************************************************************
// $Log:   S:/ENGINE15/VCS/strscan.h_v  $
// 
//    Rev 1.1   11 Aug 1998 17:33:42   CNACHEN
// updated to support ibm string scanning at level 2 heuristics.
// 
//    Rev 1.0   05 Jun 1998 15:17:22   CNACHEN
// Initial revision.
// 
//************************************************************************

#ifndef _STRSCAN_H_

#define _STRSCAN_H_

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "pshpack1.h"
#elif defined(__MACINTOSH__)
#pragma options align=packed
#else
#pragma pack(1)
#endif

typedef struct
{
    WORD                    wPoolNum;
    WORD                    wOffsetInPool;
} BUCKET_T, FAR *LPBUCKET;

#define TWO_BYTE_HASH_TABLE_SIZE_MAX    16380

typedef struct
{
    DWORD                   dwType;
    DWORD                   dwVersion;
    WORD                    wNumPools;
} MAIN_STRING_SCAN_HEADER_T, FAR *LPMAIN_STRING_SCAN_HEADER;

typedef struct
{
    WORD                    wHashTableSize;
    WORD                    wHashTableMask;
} HASH_STRING_SCAN_HEADER_T, FAR *LPHASH_STRING_SCAN_HEADER;

typedef struct
{
    DWORD                   dwType;
    DWORD                   dwVersion;
    DWORD                   dwTotalSize;
    WORD                    wNumSigs;
} MAIN_OFFSET_SCAN_HEADER_T, FAR *LPMAIN_OFFSET_SCAN_HEADER;

typedef struct
{
    WORD                    wHashTableSize;
    WORD                    wHashTableMask;
    LPBUCKET                lpstHash;
    WORD                    wNumPools;
    LPBYTE FAR *            lplpbyPoolPtrs; // up to 16K pool pointers
    LPBYTE                  lpbyOffsetSigBuffer;
    WORD                    wNumOffsetSigs;
} SEARCH_INFO_T, FAR *LPSEARCH_INFO;

typedef struct
{
    WORD exesig;            /* MZ */
    WORD len_mod_512;       /* length of file % 512 */
    WORD page_size;         /* size of file in 512 byte pages */
    WORD reloc_table_items; /* # of relocation table items */
    WORD header_size;       /* size of header in paragraphs */
    WORD min_par_above;     /* min # of pars needed above program */
    WORD max_par_above;     /* max " */
    WORD ss;
    WORD sp;
    WORD checksum;
    WORD ip;
    WORD cs;
    WORD first_rel_offset;  /* offset of first relocation table */
} IBMEXEHEADER_T, FAR *LPIBMEXEHEADER;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "poppack.h"
#elif defined(__MACINTOSH__)
#pragma options align=reset
#else
#pragma pack()
#endif


#define STRING_SCAN_NO_STRING           0
#define STRING_SCAN_FOUND_STRING        1

#define STRING_LOAD_OK                  0
#define STRING_LOAD_FILE_NOT_FOUND      1
#define STRING_LOAD_GENERAL_ERROR       2

#define SIG_LENGTH_END_OF_SIGS          0xFF
#define SIG_LENGTH_NEXT_POOL            0xFE

#define SIG_VALUE_SLIDE                 0xFD
#define SIG_VALUE_LITERAL               0xFE
#define SIG_VALUE_WILDCARD              0xFF

#define INVALID_POOL_NUM                0xFFFFU

#define ENTRYPOINT_SIG_OFFSET           0xFFFFU

#define IBM_STRING_SCAN_SECTION         1500
#define IBM_OFFSET_SCAN_SECTION         1501

#define IBM_FILE_BASE                   1500
#define IBM_BOOT_BASE                   1510

#define IBM_STRING_INCREMENT            0
#define IBM_OFFSET_INCREMENT            1

#define STRING_DATA_FILE_TYPE_VALUE     0
#define STRING_DATA_FILE_VERSION_VALUE  0

#define MIN_EXE_FILE_LENGTH             0x1A        // enuf for header

#define EXE_ADDRESS_MASK                0xFFFFFLU   // < 1MB

#define TOF_SCAN_SIZE                   16384
#define EOF_SCAN_SIZE                   8192
#define EP_SANDWICH_SIZE                8192

#define WIMPY_TOF_SCAN_SIZE             4096
#define WIMPY_EOF_SCAN_SIZE             4096
#define WIMPY_EP_SANDWICH_SIZE          4096

#define IBM_STRING_FILE_VID             0x4a09
#define IBM_STRING_BOOT_VID             0x4a0a

// protos

int LoadAllStringData
(
    LPCALLBACKREV1          lpstCallBack,
    LPTSTR                  lpszStringDataFile,
    DWORD                   dwBase,
    LPSEARCH_INFO           lpstSearchInfo
);

void FreeAllStringData
(
    LPCALLBACKREV1          lpstCallBack,
    LPSEARCH_INFO           lpstSearchInfo
);

int IBMStringScanFile
(
    LPCALLBACKREV1          lpstCallBack,
    LPSEARCH_INFO           lpstSearchInfo,
    HFILE                   hFile,
    LPBYTE                  lpbyDecryptedBuffer,
    int                     nDecryptedBufferLen,
    BOOL                    bUseWimpyMode
);

int IBMStringScanBoot
(
    LPCALLBACKREV1          lpstCallBack,
    LPSEARCH_INFO           lpstBootInfo,
    LPSEARCH_INFO           lpstFileInfo,
    LPBYTE                  lpbyBOOBuffer           // 512
);

#endif // #ifndef _STRSCAN_H_



