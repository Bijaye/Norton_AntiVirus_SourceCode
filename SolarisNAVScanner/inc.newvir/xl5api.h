//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/xl5api.h_v   1.12   22 Sep 1998 13:47:28   DCHI  $
//
// Description:
//  Header file for Excel 5.0/7.0/95 document access API
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/xl5api.h_v  $
// 
//    Rev 1.12   22 Sep 1998 13:47:28   DCHI
// Added XL5DecryptData() prototype.
// 
//    Rev 1.11   25 Jun 1998 15:16:20   DCHI
// Added type table structure definitions and XL5_OP_CALL_IDENT_DIM #define.
// 
//    Rev 1.10   24 Jun 1998 11:03:50   DCHI
// - Added additional opcode definitions.
// - Added prototypes for:
//     XL5GetOpSkip()
//     XL5ModAccInit()
//     XL5ModAccGetLineInfo()
//     XL5ModAccGetDataPtr()
// 
//    Rev 1.9   09 Jun 1998 11:31:06   DCHI
// Added CRC_STATE enums and OP #defines.
// 
//    Rev 1.8   02 Jun 1998 13:23:24   DCHI
// Added prototype for XL5ModuleCRCFast().
// 
//    Rev 1.7   04 May 1998 11:53:54   DCHI
// Added XL5WriteBlankSheet().
// 
//    Rev 1.6   07 Apr 1998 15:29:26   DCHI
// Added module type table and identifier retrieval functions.
// 
//    Rev 1.5   03 Apr 1998 10:32:20   DCHI
// Added additional XL5_OP constants.
// 
//    Rev 1.4   12 Mar 1998 10:42:38   DCHI
// Various additional functionality for new Excel engine.
// 
//    Rev 1.3   11 Feb 1998 16:16:44   DCHI
// Added line entry structures/definitions, VB identifier table function,
// and CRC functions for 4.0 macro sheets and VB module streams.
// 
//    Rev 1.2   05 Dec 1997 13:11:12   DCHI
// Added #include "xlrec.h", definition of XL5ENCKEY_T structure,
// XL5FindKey() prototype, and XL5CalculatePassword() prototype.
// 
//    Rev 1.1   12 Nov 1997 18:18:20   DCHI
// Added module/stream name association function prototypes.
// 
//    Rev 1.0   02 Oct 1997 17:30:46   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _XL5API_H_

#define _XL5API_H_

#include "olessapi.h"
#include "xlrec.h"

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "pshpack1.h"
#elif defined(__MACINTOSH__)
#pragma options align=packed
#else
#pragma pack(1)
#endif

typedef struct tagXL5_LINE_ENTRY
{
    DWORD       dwUnknown;
    BYTE        byNumPhysLines; // # physical lines in this logical line
    BYTE        byUnknown;
    WORD        wSize;          // # opcode bytes in the line
    WORD        wUnknown;
    WORD        wOffset;        // Offset of opcodes
} XL5_LINE_ENTRY_T, FAR *LPXL5_LINE_ENTRY;

typedef struct tagXL5_TYPE_INFO
{
    BYTE    byFlags0;       // 00:
    BYTE    byFlags1;       // 01:
    WORD    wNext;          // 02:
    WORD    wID;            // 04:
    WORD    wUnknown06;     // 06:
    WORD    wUnknown08;     // 08:
    WORD    wUnknown0A;     // 0A:
    WORD    wUnknown0C;     // 0C:
    WORD    wFirstParam;    // 0E:
    WORD    wParamCounts;   // 10:
    BYTE    byType0;        // 12:
    BYTE    byType1;        // 13:
    WORD    wUnknown14;     // 14:
    WORD    wUnknown16;     // 16:
    WORD    wUnknown18;     // 18:
    WORD    wDeclareOffset; // 1A:
    WORD    wUnknown1C;     // 1C:
    WORD    wUnknown1E;     // 1E:
} XL5_TYPE_INFO_T, FAR *LPXL5_TYPE_INFO;

typedef struct tagXL5_PARAM_TYPE_INFO
{
    BYTE    byFlags0;       // 00:
    BYTE    byFlags1;       // 01:
    WORD    wUnknown02;     // 02:
    WORD    wID;            // 04:
    WORD    wUnknown06;     // 06:
    BYTE    byType0;        // 08:
    BYTE    byType1;        // 09:
    BYTE    byType2;        // 0A:
    BYTE    byType3;        // 0B:
    WORD    wNextParam;     // 0C:
    WORD    wUnknown0E;     // 0E:
} XL5_PARAM_TYPE_INFO_T, FAR *LPXL5_PARAM_TYPE_INFO;

typedef struct tagXL5_DECLARE_INFO
{
    WORD    wUnknown00;     // 00:
    WORD    wLibID;         // 02:
    WORD    wNameToUse;     // 04:
    WORD    wNext;          // 06:
    BYTE    byType0;        // 08:
    BYTE    byType1;        // 09:
    WORD    wUnknown0A;     // 0A:
    WORD    wUnknown0C;     // 0C:
    WORD    wUnknown0E;     // 0E:
} XL5_DECLARE_INFO_T, FAR *LPXL5_DECLARE_INFO;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "poppack.h"
#elif defined(__MACINTOSH__)
#pragma options align=reset
#else
#pragma pack()
#endif

BOOL XL5GetMacroOffsetSize
(
    LPSS_STREAM     lpstStream,
    LPDWORD         lpdwMacroOffset,
    LPDWORD         lpdwMacroSize
);

BOOL XL5GetMacroLineInfo
(
    LPSS_STREAM     lpstStream,
    LPDWORD         lpdwTableOffset,
    LPDWORD         lpdwTableSize,
    LPDWORD         lpdwMacroOffset,
    LPDWORD         lpdwMacroSize
);

BOOL XL5GetTypeTableInfo
(
    LPSS_STREAM     lpstStream,
    LPDWORD         lpdwDeclareTableOffset,
    LPDWORD         lpdwDeclareTableSize,
    LPDWORD         lpdwTypeTableOffset,
    LPDWORD         lpdwTypeTableSize
);


/////////////////////////////////////////////////////////////////
// dir stream access

BOOL XL5GetIdentInfo
(
    LPSS_STREAM     lpstStream,
    LPWORD          lpwNumModules,
    LPDWORD         lpdwModuleIndexOffset,
    LPDWORD         lpdwTableSize,
    LPDWORD         lpdwTableOffset,
    LPDWORD         lpdwDataSize,
    LPDWORD         lpdwDataOffset
);

#define XL5_DIR_NAME_BUF_SIZE   256

typedef struct tagXL5_DIR_INFO
{
    // dir stream

    LPSS_STREAM lpstStream;

    // Number of module records and offset of first module record

    WORD        wNumModules;
    DWORD       dwFirstModuleIndexOffset;

    WORD        wFirstCachedModuleName;
    WORD        wNumCachedModuleNames;
    DWORD       dwNextModuleIndexOffset;

    BYTE        abyModuleNameBuf[XL5_DIR_NAME_BUF_SIZE];

    // Offset of string record section and length of section

    DWORD       dwStringRecordOffset;
    DWORD       dwStringRecordLen;

    // Offset of string section and length of section

    DWORD       dwStringOffset;
    DWORD       dwStringLen;
} XL5_DIR_INFO_T, FAR *LPXL5_DIR_INFO;

BOOL XL5DirInfoInit
(
    LPSS_STREAM     lpstStream,
    LPXL5_DIR_INFO  lpstDir
);

BOOL XL5DirInfoModuleName
(
    LPXL5_DIR_INFO  lpstDir,
    LPBYTE          lppsStreamName,
    LPBYTE          lppsModuleName
);

typedef struct tagXL5_DIR_ID
{
    // dir stream

    LPSS_STREAM         lpstStream;

    // Table size and offset

    DWORD               dwTableSize;
    DWORD               dwTableOffset;

    // Data size and offset

    DWORD               dwDataSize;
    DWORD               dwDataOffset;
} XL5_DIR_ID_T, FAR *LPXL5_DIR_ID;

BOOL XL5DirIDInit
(
    LPSS_STREAM         lpstStream,
    LPXL5_DIR_ID        lpstDir
);

BOOL XL5GetIdentifier
(
    LPXL5_DIR_ID        lpstDir,
    WORD                wID,
    LPSTR               lpszID

);


/////////////////////////////////////////////////////////////////
// Encrypted file access

#define XL5_ENC_KEY_LEN     16

typedef struct tagXL5ENCKEY
{
    BOOL            bEncrypted;

    // Offset of FILEPASS record

    DWORD           dwFilePassOffset;

    // Probability of success from 0 to 16 (16 == 100%)

    int             nSuccessProbability;

    // Aligned on (Record Offset + Record Length) % 16

    BYTE            abyKey[XL5_ENC_KEY_LEN];
} XL5ENCKEY_T, FAR *LPXL5ENCKEY;

BOOL XL5FindKey
(
    LPSS_STREAM         lpstStream,
    LPXL5ENCKEY         lpstKey
);

BOOL XL5CalculatePassword
(
    LPSS_STREAM         lpstStream,
    LPXL5ENCKEY         lpstKey,
    LPBYTE              lpabyPassword
);

BOOL XL5EncryptedRead
(
    LPSS_STREAM         lpstStream,
    DWORD               dwRecOffset,
    XL_REC_HDR_T        stRecHdr,
    LPXL5ENCKEY         lpstKey,
    WORD                wDataOffset,
    LPBYTE              lpbyData,
    WORD                wNumBytes
);

BOOL XL5EncryptedWrite
(
    LPSS_STREAM         lpstStream,
    DWORD               dwRecOffset,
    XL_REC_HDR_T        stRecHdr,
    LPXL5ENCKEY         lpstKey,
    WORD                wDataOffset,
    LPBYTE              lpbyData,
    WORD                wNumBytes
);

BOOL XL5EncryptedWriteRepeat
(
    LPSS_STREAM         lpstStream,
    DWORD               dwRecOffset,
    XL_REC_HDR_T        stRecHdr,
    LPXL5ENCKEY         lpstKey,
    WORD                wDataOffset,
    BYTE                byValue,
    WORD                wNumBytes
);

BOOL XL5EncryptWriteRecords
(
    LPSS_STREAM             lpstStream,
    LPXL5ENCKEY             lpstKey,
    DWORD                   dwOffset,
    LPBYTE                  lpabyRecordData,
    long                    lRecordDataSize
);

void XL5DecryptData
(
    DWORD               dwRecOffset,
    XL_REC_HDR_T        stRecHdr,
    LPXL5ENCKEY         lpstKey,
    WORD                wDataOffset,
    LPBYTE              lpabyData,
    WORD                wNumBytes
);


/////////////////////////////////////////////////////////////////
// Excel 4.0 parsed expression CRC

extern BYTE FAR gabyXL5FormulaTokenSkip[];
extern BYTE FAR gabyXL5FormulaOpTokenSkip[];
extern BYTE FAR gabyXL5FormulaOpTokenChange[];

DWORD XL5CRC32Expr
(
    LPBYTE              lpabyExpr,
    DWORD               dwExprSize,
    DWORD               dwCRC,
    LPDWORD             lpdwCRCByteCount
);


/////////////////////////////////////////////////////////////////
// Excel 5.0/95 VB module stream CRC

typedef enum tagEXL5_CRC_STATE
{
    eXL5_CRC_STATE_OPCODE_LO,   // Get low byte of opcode
    eXL5_CRC_STATE_OPCODE_HI,   // Get high byte of opcode
    eXL5_CRC_STATE_OP_LEN_LO,   // Get low byte of operand length
    eXL5_CRC_STATE_OP_LEN_HI,   // Get high byte of operand length
    eXL5_CRC_STATE_CRC_N,       // CRC n bytes
    eXL5_CRC_STATE_WORDOP_LO,   // Get low byte of WORD operand
    eXL5_CRC_STATE_WORDOP_HI,   // Get high byte of WORD operand
} EXL5_CRC_STATE_T, FAR *LPEXL5_CRC_STATE;

#define XL5_OP_MASK                 0x03FF

#define XL5_OP_PLUS                 0x0B
#define XL5_OP_AMPERSAND            0x11
#define XL5_OP_IDENT                0x20
#define XL5_OP_X__IDENT             0x21
#define XL5_OP_IDENT_DIM            0x24
#define XL5_OP_X__IDENT_DIM         0x25
#define XL5_OP_ASSIGN_IDENT         0x27
#define XL5_OP_ASSIGN_X_IDENT       0x28
#define XL5_OP_ASSIGN_X_IDENT_DIM   0x2C
#define XL5_OP_SET_IDENT            0x2E
#define XL5_OP__IDENT               0x35
#define XL5_OP__IDENT_DIM           0x37
#define XL5_OP_ASSIGN__IDENT        0x39
#define XL5_OP_ASSIGN__IDENT_DIM    0x3B
#define XL5_OP_CALL_IDENT_DIM       0x41
#define XL5_OP_CALL_X__IDENT        0x42
#define XL5_OP_CALL__IDENT          0x43
#define XL5_OP_BOS_COLON            0x44
#define XL5_OP_DEBUG                0x57
#define XL5_OP_END_WITH             0x6D
#define XL5_OP_FUNC_DEFN            0x8E
#define XL5_OP_LINE_CONTINUATION    0x9D
#define XL5_OP_CURRENCY             0xA0
#define XL5_OP_DATE                 0xA1
#define XL5_OP_WORD_VALUE           0xA3
#define XL5_OP_DWORD_VALUE          0xA4
#define XL5_OP_HEX_WORD_VALUE       0xA5
#define XL5_OP_HEX_DWORD_VALUE      0xA6
#define XL5_OP_OCT_WORD_VALUE       0xA8
#define XL5_OP_OCT_DWORD_VALUE      0xA9
#define XL5_OP_SINGLE_FLOAT_VALUE   0xAA
#define XL5_OP_DOUBLE_FLOAT_VALUE   0xAB
#define XL5_OP_LIT_STR              0xAD
#define XL5_OP_ASSIGN_NAMED_PARAM   0xC8
#define XL5_OP_PRINT_ITEM_NL        0xCD
#define XL5_OP_PRINT_OBJ            0xD0
#define XL5_OP_QUOTED_COMMENT       0xD7
#define XL5_OP_REM_COMMENT          0xDB
#define XL5_OP_VAR_DEFN             0xE9
#define XL5_OP_WITH                 0xEC

#define XL5_OP_MAX                  0xEF

extern BYTE FAR gabyXL5OpSkip[XL5_OP_MAX+1];

int XL5GetOpSkip
(
    LPBYTE      lpabyOp,
    int         nNumOpBytes
);

typedef BOOL (*LPFNXL5_CRC_FAST_CB)
    (
        LPVOID              lpvCookie,
        LPEXL5_CRC_STATE    lpeState,
        LPBYTE              lpabyBuf,
        int                 nCount
    );

BOOL XL5ModuleCRCFast
(
    LPSS_STREAM         lpstStream,
    DWORD               dwTableOffset,
    DWORD               dwTableSize,
    DWORD               dwModuleOffset,
    DWORD               dwModuleSize,
    LPBYTE              lpabyBuf,
    int                 nBufSize,
    LPFNXL5_CRC_FAST_CB lpfnCB,
    LPVOID              lpvCBCookie,
    LPDWORD             lpdwCRC,
    LPDWORD             lpdwCRCByteCount
);

BOOL XL5ModuleCRC
(
    LPSS_STREAM         lpstStream,
    DWORD               dwTableOffset,
    DWORD               dwTableSize,
    DWORD               dwModuleOffset,
    DWORD               dwModuleSize,
    LPDWORD             lpdwCRC,
    LPDWORD             lpdwCRCByteCount
);


/////////////////////////////////////////////////////////////////
// Excel 5.0/95 Book stream modification

BOOL XLSetStandardSheetFlags
(
    LPSS_STREAM             lpstStream,
    LPXL5ENCKEY             lpstKey,
    DWORD                   dwOffset
);

BOOL XLMakeWindowInfoValid
(
    LPSS_STREAM             lpstStream,
    LPXL5ENCKEY             lpstKey
);

BOOL XLRemoveUnusedVBProj
(
    LPSS_STREAM             lpstStream,
    LPXL5ENCKEY             lpstKey
);

BOOL XLEraseNameRecords
(
    LPSS_STREAM             lpstStream,
    LPXL5ENCKEY             lpstKey,
    WORD                    wIndex
);

BOOL XLBlankAllCellValues
(
    LPSS_STREAM             lpstStream,
    LPXL5ENCKEY             lpstKey,
    DWORD                   dwOffset
);

BOOL XLSetSheetStateType
(
    LPSS_STREAM             lpstStream,
    LPXL5ENCKEY             lpstKey,
    DWORD                   dwOffset,
    BYTE                    byHiddenState,
    BYTE                    bySheetType
);

BOOL XL5AssignRndSheetName
(
    LPSS_STREAM             lpstStream,
    LPXL5ENCKEY             lpstKey,
    DWORD                   dwBoundSheetOffset
);

BOOL XL5EraseSheet
(
    LPSS_STREAM             lpstStream,
    LPXL5ENCKEY             lpstKey,
    DWORD                   dwOffset,
    WORD                    wSheetIndex
);

/////////////////////////////////////////////////////////////////
// Module stream opening

int XL5OpenHexNamedStreamCB
(
    LPSS_DIR_ENTRY      lpstEntry,  // Ptr to the entry
    DWORD               dwIndex,    // The entry's index in the directory
    LPVOID              lpvCookie
);

/////////////////////////////////////////////////////////////////
// Miscellaneous

BOOL XL5GetVBABookStream
(
    LPSS_ROOT               lpstRoot,
    DWORD                   dw_VBA_PROJECTID,
    LPSS_STREAM             lpstStream
);

BOOL XL5FindBoundSheet
(
    LPSS_STREAM             lpstStream,
    LPXL5ENCKEY             lpstKey,
    LPBYTE                  lpbypsName,
    BYTE                    bySheetType,
    LPDWORD                 lpdwSheetIndex,
    LPDWORD                 lpdwBoundSheetOffset,
    LPXL7_REC_BOUNDSHEET    lpstBoundSheet
);

BOOL XL5BlankOutVBModuleStream
(
    LPSS_STREAM     lpstStream
);

BOOL XLFindFirstGlobalRec
(
    LPSS_STREAM             lpstStream,
    WORD                    wType,
    LPDWORD                 lpdwOffset,
    LPXL_REC_HDR            lpstRecHdr
);

BOOL XL5WriteBlankSheet
(
    LPSS_STREAM             lpstStream,
    LPXL5ENCKEY             lpstKey,
    DWORD                   dwBOFOffset
);


/////////////////////////////////////////////////////////////////
// Module access

typedef struct tagXL5_MOD_ACC
{
    LPSS_STREAM         lpstStream;

    DWORD               dwTableOffset;
    DWORD               dwTableSize;
    DWORD               dwDataOffset;
    DWORD               dwDataSize;

    DWORD               dwMaxLineEntries;
    LPXL5_LINE_ENTRY    lpastLines;

    DWORD               dwLineStart;
    DWORD               dwLineEnd;

    DWORD               dwLineCenter;

    DWORD               dwMaxDataSize;
    LPBYTE              lpabyData;

    DWORD               dwDataStart;
    DWORD               dwDataEnd;

    DWORD               dwDataCenter;
} XL5_MOD_ACC_T, FAR *LPXL5_MOD_ACC;

BOOL XL5ModAccInit
(
    LPXL5_MOD_ACC       lpstMod,
    LPSS_STREAM         lpstStream,
    LPBYTE              lpabyBuf,
    DWORD               dwBufSize
);

BOOL XL5ModAccGetLineInfo
(
    LPXL5_MOD_ACC       lpstMod,
    DWORD               dwLine,
    LPWORD              lpwOffset,
    LPWORD              lpwSize
);

LPBYTE XL5ModAccGetDataPtr
(
    LPXL5_MOD_ACC       lpstMod,
    WORD                wOffset,
    WORD                wSize,
    LPWORD              lpwPtrSize
);

#endif // #ifndef _XL5API_H_

