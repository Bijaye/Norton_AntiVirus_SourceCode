////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/o97api.h_v   1.23   08 Dec 1998 12:44:06   DCHI  $
//
// Description:
//  Header file for Office 97 document access API
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/o97api.h_v  $
//
//    Rev 1.26   14 Feb 2001 16:23:38   ELAI
// Modified O97GetLineBinaryOffsets() to support another argument 
// used in the macrtools project
// 
//    Rev 1.25   01 Feb 2001 16:23:38   ELAI
// Changed size of Externally defined gabyLZNTBlankThisDocumentSrc[]
//
//    Rev 1.24   16 Jan 2001 12:44:06   ELAI
// added function prototypes
//
//    Rev 1.23   08 Dec 1998 12:44:06   DCHI
// Added prototypes for O97MemICmp() and O97ModReplaceVBASrc().
// 
//    Rev 1.22   19 Oct 1998 16:21:36   DCHI
// Reverted back to revision 1.20.
// 
//    Rev 1.21   16 Oct 1998 18:40:32   DCHI
// Added prototypes for O97MemICmp(), O97ModReplaceVBASrc(), and
// gabyLZNTBlankThisDocumentSrc[].
// 
//    Rev 1.20   04 Aug 1998 13:42:44   DCHI
// - Consolidated abyFlags[3] and byTab fields of O97_LINE_TABLE_LINE_T
//   as single field abyFlags[4].
// - Added nVersion and bLittleEndian fields to O97_MODULE_T.
// 
//    Rev 1.19   13 Jul 1998 11:05:58   DCHI
// Added Office 9 module support.
// 
//    Rev 1.18   02 Jul 1998 12:32:30   DCHI
// Added XL97FindBoundSheet() prototype.
// 
//    Rev 1.17   29 Jun 1998 11:03:50   DCHI
// Added prototypes for XL97WorkbookIsEncrypted()
// and XL97EntryHasEncryptedWBSib().
// 
//    Rev 1.16   25 Jun 1998 15:17:08   DCHI
// Added #define O97_OP_CALL_IDENT_DIM 0x41.
// 
//    Rev 1.15   24 Jun 1998 10:56:42   DCHI
// - Added prototypes for:
//     O97Get_VBA_PROJECTEndian()
//     O97GetOpSkip()
//     O97ModuleCRCFast()
//     O97ModAccInit()
//     O97ModAccGetLineInfo()
//     O97ModAccGetDataPtr()
// - Added lpbLittleEndian parameter to O97GetIdentifierTableOffset().
// - Added opcode definitions.
// 
//    Rev 1.14   04 May 1998 11:54:26   DCHI
// Added XL97WriteBlankSheet().
// 
//    Rev 1.13   02 Apr 1998 10:43:42   DCHI
// Added O97XLWorkbookVBtoVHWS() prototype.
// 
//    Rev 1.12   26 Mar 1998 18:33:28   DCHI
// Added W97FIB_T structure definition and W97ClearDocument() prototype.
// 
//    Rev 1.11   12 Mar 1998 10:42:36   DCHI
// Various additional functionality for new Excel engine.
// 
//    Rev 1.10   10 Mar 1998 11:40:32   RELNITI
// ADD one more state into O97_CRC_STATE: OPTION_EXPLICIT
// 
//    Rev 1.9   11 Feb 1998 16:19:34   DCHI
// Added XL97CRC32Expr() for CRCing Excel 4.0 macro expressions.
// 
//    Rev 1.8   26 Nov 1997 15:07:16   DCHI
// Added O97VBAFindAssociatedStreams() and O97VBACheckMacrosDeletable().
// 
//    Rev 1.7   26 Sep 1997 12:31:52   DCHI
// Added prototype for O97XLWorkbookVeryHideVBSheets().
// 
//    Rev 1.6   28 Aug 1997 14:52:04   DCHI
// Added VBA5_GENERIC support to O97_SCAN_T structure.
// 
//    Rev 1.5   18 Aug 1997 14:59:48   DCHI
// Added dwByteCount field to O97_CRC_STATE_T structure.
// 
//    Rev 1.4   14 Aug 1997 18:14:54   DCHI
// 
//    Rev 1.3   11 Aug 1997 14:22:30   DCHI
// Added module and module binary access functions.
// 
//    Rev 1.2   13 Jun 1997 12:46:52   DCHI
// Added CRC function.
// 
//    Rev 1.1   08 May 1997 12:56:06   DCHI
// Added HasAutoText() and ChangeToDocument() functions.
// 
//    Rev 1.0   05 May 1997 14:26:14   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _O97API_H_

#define _O97API_H_

#include "olessapi.h"

#define LZNT_MAX_COPY_DISPLACEMENT  4096
#define LZNT_BUF_SIZE               64

typedef struct tagLZNT_BUF
{
    LPSS_STREAM     lpstStream;

    BYTE            abyBuf[LZNT_BUF_SIZE];
    int             nNumBytesInBuf;
    int             nNextByteI;

    DWORD           dwNextReadOffset;
    DWORD           dwEndReadOffsetPlus1;
} LZNT_BUF_T, FAR *LPLZNT_BUF;

typedef struct tagLZNT
{
    DWORD           dwFirstChunkOffset;

    DWORD           dwSize;

    BOOL            bCompressed;
    BOOL            bChunkIsCompressed;

    BYTE            abyUncompressed[LZNT_MAX_COPY_DISPLACEMENT];

    // stLZNTBuf buffers the compressed data from the stream

    LZNT_BUF_T      stLZNTBuf;

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

} LZNT_T, FAR *LPLZNT, FAR * FAR *LPLPLZNT;


// Function prototypes

BOOL LZNTAllocStruct
(
    LPVOID          lpvRootCookie,
    LPLPLZNT        lplpstLZNT
);

BOOL LZNTFreeStruct
(
    LPVOID          lpvRootCookie,
    LPLZNT          lpstLZNT
);

int LZNTGetBytes
(
    LPLZNT          lpstLZNT,
    DWORD           dwOffset,
    int             nNumBytes,
    LPBYTE          lpbyDst
);

BOOL LZNTCopyCompress
(
    LPLZNT          lpstLZNT,   // Stream to compress to
    DWORD           dwOffset,   // Offset in file of bytes to compress
    DWORD           dwNumBytes, // Number of bytes to copy/compress
    LPDWORD         lpdwLen     // Ptr to DWORD for compressed stream len
);

BOOL LZNTCompressBufWrite
(
    LPLZNT          lpstLZNT,
    DWORD           dwOffset,
    LPBYTE          lpabyBuf,
    DWORD           dwNumBytes,
    LPDWORD         lpdwEndOffset
);

BOOL VBA5LZNTInit
(
    LPSS_STREAM     lpstStream,
    LPLZNT          lpstLZNT
);

BOOL VBA5LZNTModuleInit
(
    LPSS_STREAM     lpstStream,
    LPLZNT          lpstLZNT
);

BOOL VBA5LZNTModuleInit2
(
    LPSS_STREAM     lpstStream,
    LPLZNT          lpstLZNT,
    LPDWORD         lpdwEndOffset
);

#define VBA5_WD8                0
#define VBA5_XL97               1
#define VBA5_GENERIC            2

typedef struct tagO97_SCAN
{
    WORD    wType;

    union
    {
        // Word 8.0

        struct
        {
            DWORD   dwParentStorageEntry;
            DWORD   dwWordDocumentEntry;
            DWORD   dw1TableEntry;
            DWORD   dw0TableEntry;
            DWORD   dwMacrosEntry;
            DWORD   dwMacrosChildEntry;
        } stWD8;

        // Excel 97

        struct
        {
            DWORD   dwParentStorageEntry;
            DWORD   dwWorkbookEntry;
            DWORD   dw_VBA_PROJECT_CUREntry;
            DWORD   dw_VBA_PROJECT_CURChildEntry;
        } stXL97;

        // Generic

        struct
        {
            DWORD   dwVBAParentEntry;
            DWORD   dwVBAParentChildEntry;
            LPVOID  alpvExtra[4];
        } stGeneric;
    } u;

    // VBA 5

    DWORD   dwVBAEntry;
    DWORD   dwVBAChildEntry;
    DWORD   dwPROJECTEntry;
    DWORD   dwPROJECTwmEntry;
    DWORD   dw_VBA_PROJECTEntry;
    DWORD   dwdirEntry;
} O97_SCAN_T, FAR *LPO97_SCAN;

BOOL O97OpenVBAStorage
(
    LPSS_ROOT           lpstRoot,
    LPO97_SCAN          lpstO97Scan
);

int O97OpenVBA5ModuleCB
(
    LPSS_DIR_ENTRY      lpstEntry,
    DWORD               dwIndex,
    LPVOID              lpvCookie
);

int O97OpenVBA5ModuleULCB
(
    LPSS_DIR_ENTRY      lpstEntry,
    DWORD               dwIndex,
    LPVOID              lpvCookie
);

int O97EnumStorageEntriesCB
(
    LPSS_DIR_ENTRY      lpstEntry,  // Ptr to the entry
    DWORD               dwIndex,    // The entry's index in the directory
    LPVOID              lpvCookie
);

BOOL O97FindVBAStreams
(
    LPSS_ROOT           lpstRoot,
    LPSS_ENUM_SIBS      lpstEnumSibs,
    LPO97_SCAN          lpstO97Scan
);

BOOL O97GetModuleCount
(
    LPSS_ROOT           lpstRoot,
    DWORD               dwVBAChildEntry,
    LPDWORD             lpdwCount
);

BOOL O97GetModuleNameAtIndex
(
    LPSS_ROOT           lpstRoot,
    DWORD               dwVBAChildEntry,
    DWORD               dwIndex,
    LPBYTE              lpbyName
);

BOOL O97MemICmp
(
    LPBYTE      lpbyMem0,
    LPBYTE      lpbyMem1,
    int         n
);

BOOL O97LZNTStreamAttributeOnly
(
    LPLZNT              lpstLZNT
);

BOOL O97CountNonEmptyModules
(
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    LPLZNT              lpstLZNT,
    DWORD               dwVBAChildEntry,    // Entry number of VBA child
    LPWORD              lpwModuleCount      // Ptr to WORD for count
);

BOOL O97UpdateForRecompilation
(
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    LPO97_SCAN          lpstO97Scan
);

#define VBA_PROJ_VER_OFFICE2001MAC      0x0062
#define VBA_PROJ_VER_OFFICE98MAC        0x0060
#define VBA_PROJ_SYSTYPE_MAC            0x000e

BOOL O97getVBA_projVersionPlatform
(
    LPSS_STREAM         lpstStream,
    LPO97_SCAN          lpstO97Scan,        // VBA5 project stream info
    LPBYTE              lpbyWorkBuffer,      // Work buffer >= 512 bytes
    LPWORD              lpwVersionNumber,
    LPWORD              lpwSystemType
);

BOOL O97DeleteModule
(
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    LPLZNT              lpstLZNT,
    LPBYTE              abyModuleName,
    LPO97_SCAN          lpstO97Scan,
    LPBYTE              lpbyWorkBuffer
);

BOOL O97OpenModuleStreamByName
(
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    DWORD               dwVBAChildEntry,
    LPBYTE              lpbyName
);

BOOL O97ModuleGetDesc
(
    LPSS_STREAM     lpstStream,
    LPLZNT          lpstLZNT,
    LPBYTE          lpbyMacroName,
    LPBYTE          lpbyDesc,
    int             nDescBufSize
);

BOOL O97VBAFindAssociatedStreams
(
    LPSS_ROOT           lpstRoot,
    LPO97_SCAN          lpstO97Scan
);

int O97WDCheckMacrosDeletableCB
(
    LPSS_DIR_ENTRY      lpstEntry,  // Ptr to the entry
    DWORD               dwIndex,    // The entry's index in the directory
    LPVOID              lpvCookie
);

BOOL O97VBACheckMacrosDeletable
(
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    LPO97_SCAN          lpstO97Scan
);

BOOL O97PROJECTRemoveDigiSig
(
    LPSS_STREAM         lpstStream,
    LPO97_SCAN          lpstO97Scan,
    LPBYTE              lpbyWorkBuffer
);


/////////////////////////////////////////////////////////////////
// VBA module binary code access
/////////////////////////////////////////////////////////////////

// This should be packed when moved into o97api.h

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD) || defined(SYM_400)
#include "pshpack1.h"
#elif defined(__MACINTOSH__) || defined(SYM_AIX) 
#pragma options align=packed
#else
#pragma pack(1)
#endif

typedef struct tagO97_LINE_TABLE_LINE
{
    BYTE        abyFlags[4];
    WORD        wSize;
    WORD        wFlags2;
    DWORD       dwOffset;
} O97_LINE_TABLE_LINE_T, FAR *LPO97_LINE_TABLE_LINE;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD) || defined(SYM_400)
#include "poppack.h"
#elif defined(__MACINTOSH__) || defined(SYM_AIX) 
#pragma options align=reset
#else
#pragma pack()
#endif

#define O97_LINE_FLAG_FUNCTION          0x0001
#define O97_LINE_FLAG_SUB               0x0002
#define O97_LINE_FLAG_END_FUNC_SUB      0x0004
#define O97_LINE_FLAG_STATEMENT         0x0008
#define O97_LINE_FLAG_DECLARE           0x0010
#define O97_LINE_FLAG_CONST             0x0020
#define O97_LINE_FLAG_COMMENT           0x0040

typedef struct tagO97_LINE_ENTRY
{
    WORD            wType;
    WORD            wSize;
    DWORD           dwOffset;
} O97_LINE_ENTRY_T, FAR *LPO97_LINE_ENTRY;

/////////////////////////////////////////////////////////////////
// Cached access to module line table entries

#define O97_LINE_TABLE_NUM_BLOCKS       8
#define O97_LINE_TABLE_BLOCK_SIZE       16

// Block number mask

#define O97_LINE_TABLE_BLOCK_NUM_MASK   0xFFFFFFF0

// Line in block (LIB) mask

#define O97_LINE_TABLE_LIB_MASK         0x0000000F

// Cache line number (CLN) mask and shift

#define O97_LINE_TABLE_CLN_MASK         0x00000070
#define O97_LINE_TABLE_CLN_SHIFT        4

typedef struct tagO97_LINE_TABLE
{
    DWORD               dwOffset;
    DWORD               dwNumLines;
    DWORD               dwNumBlocks;
    DWORD               dwBlockSize;
    DWORD               adwBlockNum[O97_LINE_TABLE_NUM_BLOCKS];
    O97_LINE_ENTRY_T    abyBlocks[O97_LINE_TABLE_NUM_BLOCKS]
                                 [O97_LINE_TABLE_BLOCK_SIZE];
} O97_LINE_TABLE_T, FAR *LPO97_LINE_TABLE;

/////////////////////////////////////////////////////////////////
// Cached access to compiled module binary

#define O97_MOD_BIN_NUM_BLOCKS          8
#define O97_MOD_BIN_BLOCK_SIZE          256

// Block number mask

#define O97_MOD_BIN_BLOCK_NUM_MASK      0xFFFFFF00

// Byte in block (BIB) mask

#define O97_MOD_BIN_BIB_MASK            0x000000FF

// Cache line number (CLN) mask and shift

#define O97_MOD_BIN_CLN_MASK            0x00000700
#define O97_MOD_BIN_CLN_SHIFT           8

typedef struct tagO97_MOD_BIN
{
    DWORD               dwOffset;
    DWORD               dwNumBlocks;
    DWORD               dwBlockSize;
    DWORD               adwBlockNum[O97_MOD_BIN_NUM_BLOCKS];
    BYTE                abyBlocks[O97_MOD_BIN_NUM_BLOCKS]
                                 [O97_MOD_BIN_BLOCK_SIZE];
} O97_MOD_BIN_T, FAR *LPO97_MOD_BIN;

typedef struct tagO97_TYPE_TABLE
{
    DWORD               dwOffset;
    DWORD               dwSize;
} O97_TYPE_TABLE_T, FAR *LPO97_TYPE_TABLE;

typedef struct tagO97_MODULE
{
    LPSS_STREAM         lpstStream;

    int                 nVersion;
    BOOL                bLittleEndian;

    O97_TYPE_TABLE_T    stTypeTable;
    O97_LINE_TABLE_T    stLineTable;
    O97_MOD_BIN_T       stBinary;
} O97_MODULE_T, FAR *LPO97_MODULE;

typedef LPO97_MODULE FAR *LPLPO97_MODULE;

BOOL O97Get_VBA_PROJECTEndian
(
    LPSS_STREAM         lpstStream,
    LPINT               lpnVersion,
    LPBOOL              lpbLitEnd
);

#define O97_VERSION_O97         0
#define O97_VERSION_O98         1

BOOL O97GetModuleVersionEndian
(
    LPSS_STREAM         lpstStream,
    LPINT               lpnVersion,
    LPBOOL              lpbLitEnd
);

BOOL O97GetIdentifierTableOffset
(
    LPSS_STREAM     lpstStream,
    LPDWORD         lpdwOffset,
    LPDWORD         lpdwNumIdents
);

BOOL O97GetTypeTableOffset
(
    LPSS_STREAM         lpstStream,
    LPDWORD             lpdwOffset,
    LPDWORD             lpdwSize
);

BOOL O97GetLineBinaryOffsets
(
    LPSS_STREAM         lpstStream,
    LPDWORD             lpdwLineTableOffset,
    LPDWORD             lpdwNumLines,
    LPDWORD             lpdwBinaryOffset,
    LPBOOL              lpbPossiblyStealthVirus
);

BOOL O97CreateModuleAccessStruct
(
    LPVOID              lpvRootCookie,
    LPLPO97_MODULE      lplpstModule
);

BOOL O97DestroyModuleAccessStruct
(
    LPVOID              lpvRootCookie,
    LPO97_MODULE        lpstModule
);

BOOL O97InitModuleAccess
(
    LPSS_STREAM         lpstStream,
    LPO97_MODULE        lpstModule
);

BOOL O97GetLineEntry
(
    LPO97_MODULE        lpstModule,
    DWORD               dwLine,
    LPO97_LINE_ENTRY    lpstEntry
);

BOOL O97GetModuleBinary
(
    LPO97_MODULE        lpstModule,
    DWORD               dwOffset,
    LPBYTE              lpbyBuf,
    DWORD               dwNumBytes
);

BOOL O97GetModuleWORD
(
    LPO97_MODULE        lpstModule,
    DWORD               dwOffset,
    LPWORD              lpwValue
);

BOOL O97GetModuleDWORD
(
    LPO97_MODULE        lpstModule,
    DWORD               dwOffset,
    LPDWORD             lpdwValue
);

BOOL O97TypeTableGetRecordIdent
(
    LPO97_MODULE        lpstModule,
    DWORD               dwOffset,
    LPWORD              lpwIdent
);

BOOL O97TypeTableGetRoutineInfo
(
    LPO97_MODULE        lpstModule,
    DWORD               dwOffset,
    LPBYTE              lpabyFlagBytes,
    LPWORD              lpwIdent,
    LPBYTE              lpbyNumParams,
    LPDWORD             lpdwFirstParamOffset
);

BOOL O97TypeTableGetParamInfo
(
    LPO97_MODULE        lpstModule,
    DWORD               dwOffset,
    LPWORD              lpwIdent,
    LPDWORD             lpdwNextParamOffset
);

// Module binary CRC

typedef enum tagEO97B_CRC_STATE
{
    eO97B_CRC_STATE_OPCODE_LO,  // Get low byte of opcode
    eO97B_CRC_STATE_OPCODE_HI,  // Get high byte of opcode
    eO97B_CRC_STATE_OP_LEN_LO,  // Get low byte of operand length
    eO97B_CRC_STATE_OP_LEN_HI,  // Get high byte of operand length
    eO97B_CRC_STATE_CRC_N,      // CRC n bytes
    eO97B_CRC_STATE_REV_CRC_N,  // CRC n bytes
    eO97B_CRC_STATE_WORDOP_LO,  // Get low byte of WORD operand
    eO97B_CRC_STATE_WORDOP_HI   // Get high byte of WORD operand
} EO97B_CRC_STATE_T, FAR *LPEO97B_CRC_STATE;

#define O97_OP_MASK                 0x03FF

#define O97_OP_PLUS                 0x0B
#define O97_OP_AMPERSAND            0x11
#define O97_OP_IDENT                0x20
#define O97_OP_X__IDENT             0x21
#define O97_OP_IDENT_DIM            0x24
#define O97_OP_X__IDENT_DIM         0x25
#define O97_OP_ASSIGN_IDENT         0x27
#define O97_OP_ASSIGN_X_IDENT       0x28
#define O97_OP_ASSIGN_X_IDENT_DIM   0x2C
#define O97_OP_SET_IDENT            0x2E
#define O97_OP__IDENT               0x35
#define O97_OP__IDENT_DIM           0x37
#define O97_OP_ASSIGN__IDENT        0x39
#define O97_OP_ASSIGN__IDENT_DIM    0x3B
#define O97_OP_CALL_IDENT_DIM       0x41
#define O97_OP_CALL_X__IDENT        0x42
#define O97_OP_CALL__IDENT          0x43
#define O97_OP_BOS_COLON            0x45
#define O97_OP_DEBUG                0x58
#define O97_OP_END_WITH             0x6E
#define O97_OP_FUNC_DEFN            0x8F
#define O97_OP_LINE_CONTINUATION    0x9E
#define O97_OP_CURRENCY             0xA1
#define O97_OP_DATE                 0xA2
#define O97_OP_WORD_VALUE           0xA4
#define O97_OP_DWORD_VALUE          0xA5
#define O97_OP_HEX_WORD_VALUE       0xA6
#define O97_OP_HEX_DWORD_VALUE      0xA7
#define O97_OP_OCT_WORD_VALUE       0xA9
#define O97_OP_OCT_DWORD_VALUE      0xAA
#define O97_OP_SINGLE_FLOAT_VALUE   0xAB
#define O97_OP_DOUBLE_FLOAT_VALUE   0xAC
#define O97_OP_LIT_STR              0xAE
#define O97_OP_ASSIGN_NAMED_PARAM   0xC9
#define O97_OP_PRINT_ITEM_NL        0xCE
#define O97_OP_PRINT_OBJ            0xD1
#define O97_OP_QUOTED_COMMENT       0xD8
#define O97_OP_REM_COMMENT          0xDC
#define O97_OP_VAR_DEFN             0xEA
#define O97_OP_WITH                 0xED

#define O97_OP_MAX      0xFC

extern BYTE FAR gabyO97OpSkip[O97_OP_MAX+1];

int O97GetOpSkip
(
    BOOL        bLittleEndian,
    LPBYTE      lpabyOp,
    int         nNumOpBytes
);

#define O98_OP_MASK                 0x03FF

#define O98_OP_PLUS                 0x0B
#define O98_OP_AMPERSAND            0x11
#define O98_OP_IDENT                0x20
#define O98_OP_X__IDENT             0x21
#define O98_OP_IDENT_DIM            0x24
#define O98_OP_X__IDENT_DIM         0x25
#define O98_OP_ASSIGN_IDENT         0x27
#define O98_OP_ASSIGN_X_IDENT       0x28
#define O98_OP_ASSIGN_X_IDENT_DIM   0x2C
#define O98_OP_SET_IDENT            0x2E
#define O98_OP__IDENT               0x35
#define O98_OP__IDENT_DIM           0x37
#define O98_OP_ASSIGN__IDENT        0x39
#define O98_OP_ASSIGN__IDENT_DIM    0x3B
#define O98_OP_CALL_IDENT_DIM       0x41
#define O98_OP_CALL_X__IDENT        0x42
#define O98_OP_CALL__IDENT          0x43
#define O98_OP_BOS_COLON            0x46
#define O98_OP_DEBUG                0x5B
#define O98_OP_END_WITH             0x71
#define O98_OP_FUNC_DEFN            0x96
#define O98_OP_LINE_CONTINUATION    0xA6
#define O98_OP_CURRENCY             0xA9
#define O98_OP_DATE                 0xAA
#define O98_OP_WORD_VALUE           0xAC
#define O98_OP_DWORD_VALUE          0xAD
#define O98_OP_HEX_WORD_VALUE       0xAE
#define O98_OP_HEX_DWORD_VALUE      0xAF
#define O98_OP_OCT_WORD_VALUE       0xB1
#define O98_OP_OCT_DWORD_VALUE      0xB2
#define O98_OP_SINGLE_FLOAT_VALUE   0xB3
#define O98_OP_DOUBLE_FLOAT_VALUE   0xB4
#define O98_OP_LIT_STR              0xB6
#define O98_OP_ASSIGN_NAMED_PARAM   0xD1
#define O98_OP_PRINT_ITEM_NL        0xD6
#define O98_OP_PRINT_OBJ            0xD9
#define O98_OP_QUOTED_COMMENT       0xE0
#define O98_OP_REM_COMMENT          0xE4
#define O98_OP_VAR_DEFN             0xF2
#define O98_OP_WITH                 0xF5

#define O98_OP_MAX                  0x104

extern BYTE FAR gabyO98OpSkip[O98_OP_MAX+1];

int O98GetOpSkip
(
    BOOL        bLittleEndian,
    LPBYTE      lpabyOp,
    int         nNumOpBytes
);

typedef BOOL (*LPFNO97_CRC_FAST_CB)
    (
        LPVOID              lpvCookie,
        LPEO97B_CRC_STATE   lpeState,
        int                 nVersion,
        BOOL                bLittleEndian,
        LPBYTE              lpabyBuf,
        int                 nCount
    );

BOOL O97ModuleCRCFast
(
    LPSS_STREAM         lpstStream,
    DWORD               dwTableOffset,
    DWORD               dwTableSize,
    DWORD               dwModuleOffset,
    LPBYTE              lpabyBuf,
    int                 nBufSize,
    LPFNO97_CRC_FAST_CB lpfnCB,
    LPVOID              lpvCBCookie,
    LPDWORD             lpdwCRC,
    LPDWORD             lpdwCRCByteCount
);

// Module access

typedef struct tagO97_MOD_ACC
{
    LPSS_STREAM             lpstStream;

    DWORD                   dwTableOffset;
    DWORD                   dwTableSize;
    DWORD                   dwDataOffset;
    DWORD                   dwDataSize;

    int                     nVersion;
    BOOL                    bLittleEndian;

    DWORD                   dwMaxLineEntries;
    LPO97_LINE_TABLE_LINE   lpastLines;

    DWORD                   dwLineStart;
    DWORD                   dwLineEnd;

    DWORD                   dwLineCenter;

    DWORD                   dwMaxDataSize;
    LPBYTE                  lpabyData;

    DWORD                   dwDataStart;
    DWORD                   dwDataEnd;

    DWORD                   dwDataCenter;
} O97_MOD_ACC_T, FAR *LPO97_MOD_ACC;

BOOL O97ModAccInit
(
    LPO97_MOD_ACC           lpstMod,
    LPSS_STREAM             lpstStream,
    LPBYTE                  lpabyBuf,
    DWORD                   dwBufSize
);

BOOL O97ModAccGetLineInfo
(
    LPO97_MOD_ACC           lpstMod,
    DWORD                   dwLine,
    LPDWORD                 lpdwOffset,
    LPWORD                  lpwSize
);

LPBYTE O97ModAccGetDataPtr
(
    LPO97_MOD_ACC           lpstMod,
    DWORD                   dwOffset,
    WORD                    wSize,
    LPWORD                  lpwPtrSize
);

extern BYTE FAR gabyModAccXOpO98toO97[O98_OP_MAX+1];

BOOL O97ModAccGetOpcode
(
    LPO97_MOD_ACC       lpstMod,
    DWORD               dwOffset,
    LPBYTE              lpabyOpcode
);

BOOL O97ModAccGetWORD
(
    LPO97_MOD_ACC       lpstMod,
    DWORD               dwOffset,
    LPWORD              lpwValue
);

BOOL O97ModAccGetDWORD
(
    LPO97_MOD_ACC       lpstMod,
    DWORD               dwOffset,
    LPDWORD             lpdwValue
);

BOOL O97ModReplaceVBASrc
(
    LPSS_STREAM         lpstStream,
    LPLZNT              lpstLZNT,
    LPBYTE              lpabySrc,
    DWORD               dwSrcSize,
    LPBOOL              lpbNoRoom
);

BOOL O97ModVBASrcHasVB_Base
(
    LPSS_STREAM         lpstStream,
    LPLZNT              lpstLZNT,
    LPBYTE              lpabyBuf,
    int                 nBufSize,
    LPINT               lpnAttributeSize
);

BOOL O97ModReplaceVB_BaseSrc
(
    LPSS_STREAM         lpstStream,
    LPLZNT              lpstLZNT,
    LPBYTE              lpabyWorkBuf,
    LPBOOL              lpbReplaced
);

BOOL O97ModVBASrcGetVB_Name
(
    LPLZNT              lpstLZNT,
    LPBYTE              lpabyBuf,
    int                 nBufSize,
    LPBYTE              lpabyszName,
    int                 nMaxNameSize
);

BOOL O97VBAStorageIsEmpty
(
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    DWORD               dwVBAChildEntry,
    LPLZNT              lpstLZNT,
    LPSS_ENUM_SIBS      lpstSibs,
    LPBOOL              lpbIsEmpty
);

BOOL O97ModGetRoutineTypeInfo
(
    LPSS_STREAM         lpstStream,
    BOOL                bLittleEndian,
    DWORD               dwOffset,
    LPBYTE              lpabyFlagBytes,
    LPWORD              lpwIdent,
    LPBYTE              lpbyNumParams,
    LPDWORD             lpdwFirstParamOffset
);

BOOL O97ModGetParamInfo
(
    LPSS_STREAM         lpstStream,
    BOOL                bLittleEndian,
    DWORD               dwOffset,
    LPWORD              lpwIdent,
    LPDWORD             lpdwNextParamOffset
);

// Module subroutine ID retrieval

#define O97MOD_GETSUBS_BUF_SIZE         2048
#define O97MOD_GETSUBS_BUF_MAX_COUNT    32

typedef struct tagO97MOD_GETSUBS
{
    WORD                wCount;     // Count of subs found
    DWORD               dwNextLine; // Next line to examine

    // IDs of found subs

    WORD                awSubIDs[O97MOD_GETSUBS_BUF_MAX_COUNT];

    // For module binary access

    O97_MOD_ACC_T       stMod;
    BYTE                abyBuf[O97MOD_GETSUBS_BUF_SIZE];

    // For type table info

    DWORD               dwTypeTableOffset;
    DWORD               dwTypeTableSize;
} O97MOD_GETSUBS_T, FAR *LPO97MOD_GETSUBS;

BOOL O97ModGetSubsInit
(
    LPSS_STREAM             lpstStream,
    LPO97MOD_GETSUBS        lpstSubs
);

BOOL O97ModGetSubs
(
    LPO97MOD_GETSUBS        lpstSubs,
    DWORD                   dwFlags
);

// ID set lookup

typedef struct tagO97_ID_LOOKUP
{
    LPSS_STREAM             lpstStream;     // _VBA_PROJECT stream
    BOOL                    bLittleEndian;  // Endian state
    DWORD                   dwTableOffset;  // Offset of table
    DWORD                   dwNumIdents;    // Number of identifiers
} O97_ID_LOOKUP_T, FAR *LPO97_ID_LOOKUP;

BOOL O97IDLookupInit
(
    LPSS_STREAM             lpstStream,
    LPO97_ID_LOOKUP         lpstLookup
);

BOOL O97IDLookupSet
(
    LPO97_ID_LOOKUP         lpstLookup,
    LPWORD                  lpawIDs,
    WORD                    wCount,
    LPBYTE                  lpabyapszNames,
    DWORD                   dwNameBufSize,
    LPLPBYTE                lpalpapszNames,
    LPWORD                  lpwCountFilled
);

// Module opcode and param parsing

typedef struct tagO97_MOD_ACC_PARSE
{
    LPO97_MOD_ACC       lpstMod;
    DWORD               dwLineOffset;

    DWORD               dwOffset;
    BYTE                abyOpcode[2];

    WORD                wArg0;
    WORD                wArg1;

    DWORD               dwArg;
} O97_MOD_ACC_PARSE_T, FAR *LPO97_MOD_ACC_PARSE;

BOOL O97ModAccParseOpcode
(
    LPO97_MOD_ACC_PARSE lpstParse
);

// _VBA_PROJECT identifiers

BOOL VBAInitKnownIdent
(
    LPSS_STREAM     lpstStream,
    LPLPBYTE        lplpabyKnownIdentSet,
    int             nNumKnownIdents,
    LPWORD          lpawKnownIdentID,
    LPWORD          lpawIdentEnum
);

WORD VBAGetKnownIdentEnum
(
    int             nNumKnownIdents,
    LPWORD          lpawKnownIdentID,
    LPWORD          lpawIdentEnum,
    WORD            wID
);


/////////////////////////////////////////////////////////////////
// Module CRC
/////////////////////////////////////////////////////////////////

typedef enum tagEO97_CRC_STATE
{
    eO97_CRC_STATE_BOL,             // Beginning of line
    eO97_CRC_STATE_ATTRIBUTE,       // Checking for Attribute
    eO97_CRC_STATE_SPACES,          // Checking for all spaces
    eO97_CRC_STATE_NORMAL,          // Normal CRC
    eO97_CRC_STATE_SKIP_LINE,       // Skip the line
    eO97_CRC_STATE_LINECONTINUE,    // Checking for line continuation
    eO97_CRC_STATE_OPTIONEXPLICIT   // Checking for OPTION EXPLICIT
} EO97_CRC_STATE_T, FAR *LPEO97_CRC_STATE;;

extern BYTE FAR gabyO97attribute[10];

typedef struct tagO97_CRC_STATE
{
    EO97_CRC_STATE_T    eState;
    DWORD               dwSubState;
    DWORD               dwCRC;
    DWORD               dwByteCount;
} O97_CRC_STATE_T, FAR *LPO97_CRC_STATE;

void O97ModuleSourceCRCInit
(
    LPO97_CRC_STATE     lpstState
);

void O97ModuleSourceCRCContinue
(
    LPO97_CRC_STATE     lpstState,
    BYTE                byCurByte
);

BOOL O97ModuleSourceCRC
(
    LPSS_STREAM         lpstStream,
    LPLZNT              lpstLZNT,
    LPDWORD             lpdwCRC,
    LPDWORD             lpdwByteCount
);

void O97ModuleSourceCRCContinue2
(
    LPO97_CRC_STATE     lpstState,
    BYTE                byCurByte
);

BOOL O97ModuleSourceCRC2
(
    LPSS_STREAM         lpstStream,
    LPLZNT              lpstLZNT,
    LPDWORD             lpdwCRC,
    LPDWORD             lpdwByteCount
);

BOOL O97ModuleSourceCRC3
(
    LPSS_STREAM         lpstStream,
    LPLZNT              lpstLZNT,
    DWORD               dwLZNTOffset,
    LPDWORD             lpdwCRC,
    LPDWORD             lpdwByteCount
);


/////////////////////////////////////////////////////////////////
// Word specific
/////////////////////////////////////////////////////////////////

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD) || defined(SYM_400)
#include "pshpack1.h"
#elif defined(__MACINTOSH__) || defined(SYM_AIX) 
#pragma options align=packed
#else
#pragma pack(1)
#endif

#define W97FIB_FLAGS0_DOT               0x01
#define W97FIB_FLAGS0_GLSY              0x02
#define W97FIB_FLAGS0_COMPLEX           0x04
#define W97FIB_FLAGS0_HAS_PIC           0x08
#define W97FIB_FLAGS0_QUICK_SAVES_MASK  0xF0

#define W97FIB_FLAGS1_ENCRYPTED         0x01
#define W97FIB_FLAGS1_WHICH_TBL_STREAM  0x02
#define W97FIB_FLAGS1_READ_ONLY_REC     0x04
#define W97FIB_FLAGS1_WRITE_RESERVATION 0x08
#define W97FIB_FLAGS1_EXT_CHAR          0x10
#define W97FIB_FLAGS1_LOAD_OVERRIDE     0x20
#define W97FIB_FLAGS1_FAR_EAST          0x40
#define W97FIB_FLAGS1_CRYPTO            0x80

#define W97FIB_FLAGSMAC_MAC             0x01
#define W97FIB_FLAGSMAC_EMPTY_SPECIAL   0x02
#define W97FIB_FLAGSMAC_LOAD_OVERRIDE_P 0x04
#define W97FIB_FLAGSMAC_FUTURE_SAVED_UD 0x08
#define W97FIB_FLAGSMAC_WORD97_SAVED    0x10

typedef struct tagW97FIB
{
    WORD        wIdent;                 //   0   0: Magic number
    WORD        wFIB;                   //   2   2: FIB version
    WORD        wProduct;               //   4   4: Product version
    WORD        wLID;                   //   6   6: Language stamp
    WORD        wPN;                    //   8   8:
    BYTE        byFlags0;               //  10   A:
    BYTE        byFlags1;               //  11   B:
    WORD        wFIBBack;               //  12   C:
    DWORD       dwKey;                  //  14   E: File encrypted key
    BYTE        byEnvr;                 //  18  12: Creation environment
    BYTE        byFlagsMac;             //  19  13:
    WORD        wChSe;                  //  20  14: Default ext text char set
    WORD        wChSeTables;            //  22  16: Default ext data char set
    DWORD       dwFCMin;                //  24  18: First char file offset
    DWORD       dwFCMac;                //  28  1C: Last char file offset
    WORD        wCSW;                   //  32  20: Count of fields in "shorts"
    WORD        wMagicCreated;          //  34  22: File creator
    WORD        wMagicRevised;          //  36  24: File's last modifier
    WORD        wMagicCreatedPrivate;   //  38  26: Private data
    WORD        wMagicRevisedPrivate;   //  40  28: Private data
    WORD        wPNFbpChpFirst_W6;      //  42  2A: Not used
    WORD        wPNChpFirst_W6;         //  44  2C: Not used
    WORD        wCPNBteChp_W6;          //  46  2E: Not used
    WORD        wPNFbpPapFirst_W6;      //  48  30: Not used
    WORD        wPNPapFirst_W6;         //  50  32: Not used
    WORD        wCPNBtePap_W6;          //  52  34: Not used
    WORD        wPNFbpLvcFirst_W6;      //  54  36: Not used
    WORD        wPNLvcFirst_W6;         //  56  38: Not used
    WORD        wCPNBteLvc_W6;          //  58  3A: Not used
    WORD        wLIDFE;                 //  60  3C: Far East language ID
    WORD        wCLW;                   //  62  3E: Count of fields in "longs"
    DWORD       dwCBMac;                //  64  40: Last byte file offset
    DWORD       dwProductCreated;       //  68  44: Build date of creator
    DWORD       dwProductRevised;       //  72  48: Build date of modifier
    DWORD       dwCCPText;              //  76  4C: Length of main doc text
    DWORD       dwCCPFtn;               //  80  50: Length of footnote text
    DWORD       dwCCPHdd;               //  84  54: Length of header text
    DWORD       dwCCPMcr;               //  88  58: Length of macro text
    DWORD       dwCCPAtn;               //  92  5C: Length of annotation text
    DWORD       dwCCPEdn;               //  96  60: Length of endnote text
    DWORD       dwCCPTxbx;              // 100  64: Length of textbox text
    DWORD       dwCCPHdrTxbx;           // 104  68: Length of hdr textbox text
    DWORD       dwPNFbpChpFirst;        // 108  6C: First plcfbte block
    DWORD       dwPNChpFirst;           // 112  70: First page w/CHPX FKP info
    DWORD       dwCPNBteChp;            // 116  74: CHPX FKP count
    DWORD       dwPNFbpPapFirst;        // 120  78: First plcfbte block
    DWORD       dwPNPapFirst;           // 124  7C: First page w/PAPX FKP info
    DWORD       dwCPNBtePap;            // 128  80: PAPX FKP count
    DWORD       dwPNFbpLvcFirst;        // 132  84: First plcfbte block
    DWORD       dwPNLvcFirst;           // 136  88: First page w/LVC FKP info
    DWORD       dwCPNBteLvc;            // 140  8C: LVC FKP count
    DWORD       dwFCIslandFirst;        // 144  90:
    DWORD       dwFCIslandLim;          // 148  94:
    WORD        wCFCLCB;                // 152  98: Count of FC/LCB pairs.
    DWORD       dwFCStshfOrig;          // 154  9A: Orig STSH offset
    DWORD       dwLCBStshfOrig;         // 158  9E: Orig STSH size
    DWORD       dwFCStshf;              // 162  A2: STSH offset
    DWORD       dwLCBStshf;             // 166  A6: STSH size
    DWORD       dwFCPlcffndRef;         // 170  AA: Footnote ref PLC offset
    DWORD       dwLCBPlcffndRef;        // 174  AE: Footnote ref PLC size
    DWORD       dwFCPlcffndTxt;         // 178  B2: Footnote text PLC offset
    DWORD       dwLCBPlcffndTxt;        // 182  B6: Footnote text PLC size
    DWORD       dwFCPlcfandRef;         // 186  BA: Annotation ref PLC offset
    DWORD       dwLCBPlcfandRef;        // 190  BE: Annotation ref PLC size
    DWORD       dwFCPlcfandTxt;         // 194  C2: Annotation text PLC offset
    DWORD       dwLCBPlcfandTxt;        // 198  C6: Annotation text PLC size
    DWORD       dwFCPlcfsed;            // 202  CA: Section desc PLC offset
    DWORD       dwLCBPlcfsed;           // 206  CE: Section desc PLC size
    DWORD       dwFCPlcpad;             // 210  D2: No longer used
    DWORD       dwLCBPlcpad;            // 214  D6: No longer used
    DWORD       dwFCPlcfphe;            // 218  DA: Paragraph height PLC offset
    DWORD       dwLCBPlcfphe;           // 222  DE: Paragraph height PLC size
    DWORD       dwFCSttbfglsy;          // 226  E2: Glossary str table offset
    DWORD       dwLCBSttbfglsy;         // 230  E6: Glossary str table size
    DWORD       dwFCPlcfglsy;           // 234  EA: Glossary PLC offset
    DWORD       dwLCBPlcfglsy;          // 238  EE: Glossary PLC size
    DWORD       dwFCPlcfhdd;            // 242  F2: Header PLC offset
    DWORD       dwLCBPlcfhdd;           // 246  F6: Header PLC size
    DWORD       dwFCPlcfbteChpx;        // 250  FA: Char prop bin table offset
    DWORD       dwLCBPlcfbteChpx;       // 254  FE: Char prop bin table size
    DWORD       dwFCPlcfbtePapx;        // 258 102: Para prop bin table offset
    DWORD       dwLCBPlcfbtePapx;       // 262 106: Para prop bin table size
    DWORD       dwFCPlcfsea;            // 266 10A: Private PLC offset
    DWORD       dwLCBPlcfsea;           // 270 10E: Private PLC size
    DWORD       dwFCSttbfffn;           // 274 112: Font info table offset
    DWORD       dwLCBSttbfffn;          // 278 116: Font info table size
    DWORD       dwFCPlcffldMom;         // 282 11A: Main doc field PLC offset
    DWORD       dwLCBPlcffldMom;        // 286 11E: Main doc field PLC size
    DWORD       dwFCPlcffldHdr;         // 290 122: Header field PLC offset
    DWORD       dwLCBPlcffldHdr;        // 294 126: Header field PLC size
    DWORD       dwFCPlcffldFtn;         // 298 12A: Footnote field PLC offset
    DWORD       dwLCBPlcffldFtn;        // 302 12E: Footnote field PLC size
    DWORD       dwFCPlcffldAtn;         // 306 132: Annotation field PLC offset
    DWORD       dwLCBPlcffldAtn;        // 310 136: Annotation field PLC size
    DWORD       dwFCPlcffldMcr;         // 314 13A: No longer used
    DWORD       dwLCBPlcffldMcr;        // 318 13E: No longer used
    DWORD       dwFCSttbfbkmk;          // 322 142: Bookmark name table offset
    DWORD       dwLCBSttbfbkmk;         // 326 146: Bookmark name table size
    DWORD       dwFCPlcfbkf;            // 330 14A: Bookmark begin PLC offset
    DWORD       dwLCBPlcfbkf;           // 334 14E: Bookmark begin PLC size
    DWORD       dwFCPlcfbkl;            // 338 152: Bookmark end PLC offset
    DWORD       dwLCBPlcfbkl;           // 342 156: Bookmark end PLC size
    DWORD       dwFCCmds;               // 346 15A: Macro commands offset
    DWORD       dwLCBCmds;              // 350 15E: Macro commands size
    DWORD       dwFCPlcmcr;             // 354 162: No longer used
    DWORD       dwLCBPlcmcr;            // 358 166: No longer used
    DWORD       dwFCSttbfmcr;           // 362 16A: No longer used
    DWORD       dwLCBSttbfmcr;          // 366 16E: No longer used
    DWORD       dwFCPrDrvr;             // 370 172: Printer driver info offset
    DWORD       dwLCBPrDrvr;            // 374 176: Printer driver info size
    DWORD       dwFCPrEnvPort;          // 378 17A: Portrait print env offset
    DWORD       dwLCBPrEnvPort;         // 382 17E: Portrait print env size
    DWORD       dwFCPrEnvLand;          // 386 182: Landscape print env offset
    DWORD       dwLCBPrEnvLand;         // 390 186: Landscape print env size
    DWORD       dwFCWss;                // 394 18A: Window save state offset
    DWORD       dwLCBWss;               // 398 18E: Window save state size
    DWORD       dwFCDop;                // 402 192: Document property offset
    DWORD       dwLCBDop;               // 406 196: Document property size
    DWORD       dwFCSttbfAssoc;         // 410 19A: Assoc str table offset
    DWORD       dwLCBSttbfAssoc;        // 414 19E: Assoc str table size
    DWORD       dwFCClx;                // 418 1A2: Complex file info offset
    DWORD       dwLCBClx;               // 422 1A6: Complex file info size
    DWORD       dwFCPlcfpgdFtn;         // 426 1AA: Not used
    DWORD       dwLCBPlcfpgdFtn;        // 430 1AE: Not used
    DWORD       dwFCAutosaveSource;     // 434 1B2: Orig file name offset
    DWORD       dwLCBAutosaveSource;    // 438 1B6: Orig file name size
    DWORD       dwFCGrpXstAtnOwners;    // 442 1BA: Annotation owners offset
    DWORD       dwLCBGrpXstAtnOwners;   // 446 1BE: Annotation owners size
    DWORD       dwFCSttbfAtnbkmk;       // 450 1C2: Annotation bkmk name offset
    DWORD       dwLCBSttbfAtnbkmk;      // 454 1C6: Annotation bkmk name size
    DWORD       dwFCPlcdoaMom;          // 458 1CA: No longer used
    DWORD       dwLCBPlcdoaMom;         // 462 1CE: No longer used
    DWORD       dwFCPlcdoaHdr;          // 466 1D2: No longer used
    DWORD       dwLCBPlcdoaHdr;         // 470 1D6: No longer used
    DWORD       dwFCPlcspaMom;          // 474 1DA: Main doc FSPA PLC offset
    DWORD       dwLCBPlcspaMom;         // 478 1DE: Main doc FSPA PLC size
    DWORD       dwFCPlcspaHdr;          // 482 1E2: Header FSPA PLC offset
    DWORD       dwLCBPlcspaHdr;         // 486 1E6: Header FSPA PLC size
    DWORD       dwFCPlcfAtnbkf;         // 490 1EA: Annotation BKF PLC offset
    DWORD       dwLCBPlcfAtnbkf;        // 494 1EE: Annotation BKF PLC size
    DWORD       dwFCPlcfAtnbkl;         // 498 1F2: Annotation BKL PLC offset
    DWORD       dwLCBPlcfAtnbkl;        // 502 1F6: Annotation BKL PLC size
    DWORD       dwFCPms;                // 506 1FA: Print merge state offset
    DWORD       dwLCBPms;               // 510 1FE: Print merge state size
    DWORD       dwFCFormFldSttbs;       // 514 202: Form field table offset
    DWORD       dwLCBFormFldSttbs;      // 518 206: Form field table size
    DWORD       dwFCPlcfendRef;         // 522 20A: Endnote ref PLC offset
    DWORD       dwLCBPlcfendRef;        // 526 20E: Endnote ref PLC size
    DWORD       dwFCPlcfendTxt;         // 530 212: Endnote text PLC offset
    DWORD       dwLCBPlcfendTxt;        // 534 216: Endnote text PLC size
    DWORD       dwFCPlcffldEdn;         // 538 21A: Endnote field PLC offset
    DWORD       dwLCBPlcffldEdn;        // 542 21E: Endnote field PLC size
    DWORD       dwFCPlcfpgdEdn;         // 546 222: Not used
    DWORD       dwLCBPlcfpgdEdn;        // 550 226: Not used
    DWORD       dwFCDggInfo;            // 554 22A: Art object table offset
    DWORD       dwLCBDggInfo;           // 558 22E: Art object table size
    DWORD       dwFCSttbfRMark;         // 562 232: Author abbrev table offset
    DWORD       dwLCBSttbfRMark;        // 566 236: Author abbrev table size
    DWORD       dwFCSttbCaption;        // 570 23A: Caption titles table offset
    DWORD       dwLCBSttbCaption;       // 574 23E: Caption titles table size
    DWORD       dwFCSttbAutoCaption;    // 578 242: Auto caption table offset
    DWORD       dwLCBSttbAutoCaption;   // 582 246: Auto caption table size
    DWORD       dwFCPlcfwkb;            // 586 24A: Contrib doc PLC offset
    DWORD       dwLCBPlcfwkb;           // 590 24E: Contrib doc PLC size
    DWORD       dwFCPlcfspl;            // 594 252: Spell check PLC offset
    DWORD       dwLCBPlcfspl;           // 598 256: Spell check PLC size
    DWORD       dwFCPlcftxbxTxt;        // 602 25A: Textbox text PLC offset
    DWORD       dwLCBPlcftxbxTxt;       // 606 25E: Textbox text PLC size
    DWORD       dwFCPlcffldTxbx;        // 610 262: Textbox field PLC offset
    DWORD       dwLCBPlcffldTxbx;       // 614 266: Textbox field PLC size
    DWORD       dwFCPlcfhdrtxbxTxt;     // 618 26A: Header textbox PLC offset
    DWORD       dwLCBPlcfhdrtxbxTxt;    // 622 26E: Header textbox PLC size
    DWORD       dwFCPlcffldHdrTxbx;     // 626 272: Hdr txtbox field PLC offset
    DWORD       dwLCBPlcffldHdrTxbx;    // 630 276: Hdr txtbox field PLC size
    DWORD       dwFCStwUser;            // 634 27A: Macro user storage offset
    DWORD       dwLCBStwUser;           // 638 27E: Macro user storage size
    DWORD       dwFCSttbttmbd;          // 642 282: Embedded TTF table offset
    DWORD       dwcbSttbttmbd;          // 646 286: Embedded TTF table size
    DWORD       dwFCUnused;             // 650 28A:
    DWORD       dwLCBUnused;            // 654 28E:
    DWORD       dwFCPgdMother;          // 658 292: Main text PGD PLC offset
    DWORD       dwLCBPgdMother;         // 662 296: Main text PGD PLC size
    DWORD       dwFCBkdMother;          // 666 29A: Main text BKD PLC offset
    DWORD       dwLCBBkdMother;         // 670 29E: Main text BKD PLC size
    DWORD       dwFCPgdFtn;             // 674 2A2: Footnote PGD PLC offset
    DWORD       dwLCBPgdFtn;            // 678 2A6: Footnote PGD PLC size
    DWORD       dwFCBkdFtn;             // 682 2AA: Footnote BKD PLC offset
    DWORD       dwLCBBkdFtn;            // 686 2AE: Footnote BKD PLC size
    DWORD       dwFCPgdEdn;             // 690 2B2: Endnote PGD PLC offset
    DWORD       dwLCBPgdEdn;            // 694 2B6: Endnote PGD PLC size
    DWORD       dwFCBkdEdn;             // 698 2BA: Endnote BKD PLC offset
    DWORD       dwLCBBkdEdn;            // 702 2BE: Endnote BKD PLC size
    DWORD       dwFCSttbfIntlFld;       // 706 2C2: Field keyword table offset
    DWORD       dwLCBSttbfIntlFld;      // 710 2C6: Field keyword table size
    DWORD       dwFCRouteSlip;          // 714 2CA: Mailer route slip offset
    DWORD       dwLCBRouteSlip;         // 718 2CE: Mailer route slip size
    DWORD       dwFCSttbSavedBy;        // 722 2D2: Saved by users table offset
    DWORD       dwLCBSttbSavedBy;       // 726 2D6: Saved by users table size
    DWORD       dwFCSttbFnm;            // 730 2DA: Filename ref table offset
    DWORD       dwLCBSttbFnm;           // 734 2DE: Filename ref table size
    DWORD       dwFCPlcfLst;            // 738 2E2: List format PLC offset
    DWORD       dwLCBPlcfLst;           // 742 2E6: List format PLC size
    DWORD       dwFCPlfLfo;             // 746 2EA: Lst fmt override PLC offset
    DWORD       dwLCBPlfLfo;            // 750 2EE: Lst fmt override PLC size
    DWORD       dwFCPlcftxbxBkd;        // 754 2F2: Mn doc txbx BKD PLC offset
    DWORD       dwLCBPlcftxbxBkd;       // 758 2F6: Mn doc txbx BKD PLC size
    DWORD       dwFCPlcftxbxHdrBkd;     // 762 2FA: Header txbx BKD PLC offset
    DWORD       dwLCBPlcftxbxHdrBkd;    // 766 2FE: Header txbx BKD PLC size
    DWORD       dwFCDocUndo;            // 770 302: Main strm undo data offset
    DWORD       dwLCBDocUndo;           // 774 306:
    DWORD       dwFCRgbuse;             // 778 30A: Main strm undocumented
    DWORD       dwLCBRgbuse;            // 782 30E:
    DWORD       dwFCUsp;                // 786 312: Main strm undocumented
    DWORD       dwLCBUsp;               // 790 316:
    DWORD       dwFCUskf;               // 794 31A: Table strm undocumented
    DWORD       dwLCBUskf;              // 798 31E:
    DWORD       dwFCPlcupcRgbuse;       // 802 322: Table strm undocumented
    DWORD       dwLCBPlcupcRgbuse;      // 806 326:
    DWORD       dwFCPlcupcUsp;          // 810 32A: Table strm undocumented
    DWORD       dwLCBPlcupcUsp;         // 814 32E:
    DWORD       dwFCSttbGlsyStyle;      // 818 332: Glossary style table offset
    DWORD       dwLCBSttbGlsyStyle;     // 822 336: Glossary style table size
    DWORD       dwFCPlgosl;             // 826 33A: Grammar options offset
    DWORD       dwLCBPlgosl;            // 830 33E: Grammar options size
    DWORD       dwFCPlcocx;             // 834 342: OCX table offset
    DWORD       dwLCBPlcocx;            // 838 346: OCX table size
    DWORD       dwFCPlcfbteLvc;         // 842 34A: Char prop bin table offset
    DWORD       dwLCBPlcfbteLvc;        // 846 34E: Char prop bin table size
    DWORD       dwLowDateTime;          // 850 352:
    DWORD       dwHighDateTime;         // 854 356:
    DWORD       dwFCPlcflvc;            // 858 35A: LVC PLC offset
    DWORD       dwLCBPlcflvc;           // 862 35E: LVC PLC size
    DWORD       dwFCPlcasumy;           // 866 362: Autosummary PLC offset
    DWORD       dwLCBPlcasumy;          // 870 366: Autosummary PLC size
    DWORD       dwFCPlcfgram;           // 874 36A: Grammar state PLC offset
    DWORD       dwLCBPlcfgram;          // 878 36E: Grammar state PLC size
    DWORD       dwFCSttbListNames;      // 882 372: List name table offset
    DWORD       dwLCBSttbListNames;     // 886 376: List name table size
    DWORD       dwFCSttbfUssr;          // 890 37A: Table strm undocumented
    DWORD       dwLCBSttbfUssr;         // 894 37E:
} W97FIB_T, FAR *LPW97FIB;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD) || defined(SYM_400)
#include "poppack.h"
#elif defined(__MACINTOSH__) || defined(SYM_AIX) 
#pragma options align=reset
#else
#pragma pack()
#endif

BOOL O97WDFindAssociatedStreams
(
    LPSS_ROOT           lpstRoot,
    LPO97_SCAN          lpstO97Scan
);

extern BYTE FAR gabyszThisDocument[13];
extern BYTE FAR gabyLZNTBlankThisDocumentSrc[0xA8];

BOOL O97WDDeleteModule
(
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    LPLZNT              lpstLZNT,
    LPBYTE              abyModuleName,      // Zero terminated module name
    LPO97_SCAN          lpstO97Scan,        // VBA5 project stream info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 0.75K bytes
);

BOOL O97WDCheckMacrosDeletable
(
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    LPLZNT              lpstLZNT,
    LPO97_SCAN          lpstO97Scan,        // VBA5 project stream info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 0.5K bytes
);

BOOL O97WDHasAutoText
(
    LPSS_STREAM         lpstStream,
    LPBOOL              lpbHasAutoText
);

BOOL O97WDChangeToDocument
(
    LPSS_STREAM         lpstStream
);

BOOL W97VBADeleteModule
(
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    LPLZNT              lpstLZNT,
    LPBYTE              lpabyModuleName,
    LPO97_SCAN          lpstO97Scan,
    LPBYTE              lpabyWorkBuffer
);

BOOL W97DeleteModule
(
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    LPLZNT              lpstLZNT,
    LPBYTE              lpabyModuleName,
    LPO97_SCAN          lpstO97Scan,
    LPBYTE              lpabyWorkBuffer,
    LPBOOL              lpbResync
);

BOOL W97VBAStorageIsEmpty
(
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    DWORD               dwVBAChildEntry,
    LPLZNT              lpstLZNT,
    LPSS_ENUM_SIBS      lpstSibs,
    LPBOOL              lpbIsEmpty
);

// Text removal

BOOL W97ClearDocument
(
    LPSS_STREAM         lpstStream,
    LPSS_STREAM         lpstTableStream,
    LPW97FIB            lpstFIB,
    DWORD               dwFIBOffset
);

// Template Data Table (TDT)

BOOL W97TDTDeleteModule
(
    BOOL                bFile,
    LPVOID              lpvRootCookie,
    LPVOID              lpvFile,
    LPBYTE              lpabyModuleName,
    LPBYTE              lpabyWorkBuffer
);

BOOL W97TDTPackMacroRecs
(
    BOOL                bFile,
    LPVOID              lpvRootCookie,
    LPVOID              lpvFile,
    LPBYTE              lpabyWorkBuffer
);

BOOL W97TDTHasNoMoreMacros
(
    BOOL                bFile,
    LPVOID              lpvRootCookie,
    LPVOID              lpvFile,
    LPBYTE              lpabyWorkBuffer,
    LPBOOL              lpbNoMoreMacros
);

BOOL W97TDTDeleteRec0x07
(
    BOOL                bFile,
    LPVOID              lpvRootCookie,
    LPVOID              lpvFile,
    LPBYTE              lpabyWorkBuffer,
    LPBOOL              lpbHasRec0x07
);


// Digital signatures

BOOL W97RemoveDigiSig
(
    LPSS_STREAM         lpstStream,
    LPO97_SCAN          lpstO97Scan,
    LPBOOL              lpbFound
);


/////////////////////////////////////////////////////////////////
// Excel specific
/////////////////////////////////////////////////////////////////

extern BYTE FAR gabyszThisWorkbook[13];

BOOL O97XLFindAssociatedStreams
(
    LPSS_ROOT           lpstRoot,
    LPO97_SCAN          lpstO97Scan
);

BOOL O97XLDeleteModule
(
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    LPLZNT              lpstLZNT,
    LPBYTE              abyModuleName,
    LPO97_SCAN          lpstO97Scan,
    LPBYTE              lpbyWorkBuffer
);

BOOL O97XLCheckMacrosDeletable
(
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    LPLZNT              lpstLZNT,
    LPO97_SCAN          lpstO97Scan         // VBA5 project stream info
);

BOOL O97XLWorkbookRemoveVBP
(
    LPSS_STREAM         lpstStream,
    DWORD               dwWorkbookEntryNum
);

BOOL O97XLWorkbookVeryHideVBSheets
(
    LPSS_STREAM         lpstStream,
    DWORD               dwWorkbookEntryNum
);

extern BYTE FAR gabyXL97FormulaTokenSkip[];
extern BYTE FAR gabyXL97FormulaExtTokenSkip[];
extern BYTE FAR gabyXL97FormulaOpTokenSkip[];
extern BYTE FAR gabyXL97FormulaOpTokenChange[];

DWORD XL97CRC32Expr
(
    LPBYTE              lpabyExpr,
    DWORD               dwExprSize,
    DWORD               dwCRC,
    LPDWORD             lpdwCRCByteCount
);

BOOL XL97UnicodeIsStrI
(
    LPSS_STREAM             lpstStream,
    WORD                    wCharCnt,
    DWORD                   dwUnicodeStrOffset,
    LPBYTE                  lpbypsStr,
    LPBOOL                  lpbEqual
);

BOOL XL97FindBoundSheet
(
    LPSS_STREAM             lpstStream,
    LPBYTE                  lpbypsName,
    BYTE                    bySheetType,
    LPDWORD                 lpdwSheetIndex,
    LPDWORD                 lpdwBoundSheetOffset
);

BOOL XL97RemoveGlobalNames
(
    LPSS_STREAM             lpstStream,
    LPBYTE                  lpbypsName
);

BOOL XL97AssignRndSheetName
(
    LPSS_STREAM             lpstStream,
    DWORD                   dwBoundSheetOffset
);

BOOL XL97EraseSheet
(
    LPSS_STREAM             lpstStream,
    DWORD                   dwOffset,
    WORD                    wSheetIndex
);

BOOL XL97ReadUnicodeStr
(
    LPSS_STREAM             lpstStream,
    DWORD                   dwOffset,
    BOOL                    bReadSingleByte,
    LPBYTE                  lpbyDst,
    WORD                    wSize,
    LPBOOL                  lpbDoubleByte
);

BOOL O97XLWorkbookVBtoVHWS
(
    LPSS_STREAM         lpstStream,
    DWORD               dwWorkbookEntryNum,
    LPO97_SCAN          lpstO97Scan
);

BOOL XL97WriteBlankSheet
(
    LPSS_STREAM             lpstStream,
    DWORD                   dwBOFOffset
);

BOOL XL97WorkbookIsEncrypted
(
    LPSS_STREAM             lpstStream,
    LPBOOL                  lpbIsEncrypted
);

BOOL XL97EntryHasEncryptedWBSib
(
    DWORD                   dwEntry,
    LPSS_STREAM             lpstStream,
    LPBOOL                  lpbResult
);

#endif // #ifndef _O97API_H_






