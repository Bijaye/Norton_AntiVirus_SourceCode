//************************************************************************
//
// $Header:   S:/NAVEX/VCS/copy.h_v   1.1   12 Oct 1998 13:38:08   DCHI  $
//
// Description:
//  Contains heuristic macro virus detection source prototypes.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/copy.h_v  $
// 
//    Rev 1.1   12 Oct 1998 13:38:08   DCHI
// - Added CopyIsEmpty().
// - Added the following copy flags:
//     COPY_FLAG_EXPORT_FROM_GLOBAL
//     COPY_FLAG_IMPORT_TO_LOCAL
//     COPY_FLAG_EXPORT_FROM_LOCAL
//     COPY_FLAG_IMPORT_TO_GLOBAL
// - Added the prototypes for CopyLogExport() and CopyLogImport().
// 
//    Rev 1.0   09 Jul 1997 16:14:40   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _COPY_H_

#define _COPY_H_

typedef struct tagMACRO_PAIR
{
    WORD    wFlags;
    WORD    wIdxLocal;
    WORD    wIdxGlobal;
} MACRO_PAIR_T, FAR *LPMACRO_PAIR;

#define COPY_NAME_BUF_SIZE  2048

#define COPY_MAX_NAMES      128
#define COPY_MAX_PAIRS      64

#define COPY_FLAG_NO_FLAGS              0x0000
#define COPY_FLAG_GLOBAL_TO_LOCAL       0x0001
#define COPY_FLAG_LOCAL_TO_GLOBAL       0x0002
#define COPY_FLAG_EXPORT_FROM_GLOBAL    0x0004
#define COPY_FLAG_IMPORT_TO_LOCAL       0x0008
#define COPY_FLAG_EXPORT_FROM_LOCAL     0x0010
#define COPY_FLAG_IMPORT_TO_GLOBAL      0x0020

typedef struct tagCOPY
{
    WORD            wNameBufUsed;
    BYTE            abyNameBuf[COPY_NAME_BUF_SIZE];

    WORD            wNumNames;
    WORD            wNameOffsets[COPY_MAX_NAMES];

    WORD            wNumMacroPairs;
    MACRO_PAIR_T    astMacroPairs[COPY_MAX_PAIRS];
} COPY_T, FAR *LPCOPY;

LPCOPY CopyCreate
(
    LPVOID      lpvRootCookie
);

BOOL CopyDestroy
(
    LPVOID      lpvRootCookie,
    LPCOPY      lpstCopy
);

BOOL CopyInit
(
    LPCOPY      lpstCopy
);

BOOL CopyLogCopy
(
    LPCOPY      lpstCopy,
    LPBYTE      lpbySrc,
    LPBYTE      lpbyDst
);

BOOL CopyLogExport
(
    LPCOPY      lpstCopy,
    WORD        wFlags,
    LPBYTE      lpabypsFileName,
    LPBYTE      lpabypsModuleName
);

BOOL CopyLogImport
(
    LPCOPY      lpstCopy,
    WORD        wFlags,
    LPBYTE      lpabypsFileName
);

void CopyPrint
(
    LPCOPY          lpstCopy
);

BOOL CopyIsViral
(
    LPCOPY          lpstCopy
);

BOOL CopyIsPartOfViralSet
(
    LPCOPY      lpstCopy,
    LPBYTE      lpbyMacroName
);

BOOL CopyIsEmpty
(
    LPCOPY      lpstCopy
);

#endif // #ifndef _COPY_H_


