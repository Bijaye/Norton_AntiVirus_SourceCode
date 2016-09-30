// Copyright 1999 Symantec Corporation
//************************************************************************
//
// $Header:   $
//
// Description:
//  Header file for document stripping API
//
//************************************************************************
// $Log:   $
// 
//************************************************************************

#ifndef _STRIPDOC_H_

#define _STRIPDOC_H_

#ifdef WIN32

#ifdef _STRIPDOC_CPP_
#define StripDocImportExport __declspec(dllexport)
#else
#define StripDocImportExport __declspec(dllimport)
#endif

#else

#define StripDocImportExport

#endif // #ifdef SYM_WIN32

////////////////////////////////////////////////////////////
// Memory allocation/file IO/progress callbacks
//
//  Progress may be null if not needed

typedef enum tagESTRIP_DOC_SEEK
{
    eStripDocSeekSet        = 0,
    eStripDocSeekCur        = 1,
    eStripDocSeekEnd        = 2
} ESTRIP_DOC_SEEK_T;

typedef enum tagESTRIP_DOC_OPEN
{
    eStripDocOpenRead       = 0,
    eStripDocOpenWrite      = 1,
    eStripDocOpenReadWrite  = 2
} ESTRIP_DOC_OPEN_T;

typedef struct tagSTRIP_DOC_CB
{
    BOOL (*MemoryAlloc)
    (
        LPVOID              lpvExtraParam,
        DWORD               dwNumBytes,
        LPLPVOID            lplpvBuffer
    );

    BOOL (*MemoryFree)
    (
        LPVOID              lpvExtraParam,
        LPVOID              lpvBuffer
    );

    BOOL (*FileOpen)
    (
        LPVOID              lpvExtraParam,
        LPVOID              lpvFileName,
        ESTRIP_DOC_OPEN_T   eOpenMode,
        LPLPVOID            lplpvFile
    );

    BOOL (*FileClose)
    (
        LPVOID              lpvExtraParam,
        LPVOID              lpvFile
    );

    BOOL (*FileSeek)
    (
        LPVOID              lpvExtraParam,
        LPVOID              lpvFile,
        long                lOffset,
        ESTRIP_DOC_SEEK_T   eWhence,
        LPDWORD             lpdwNewOffset
    );

    BOOL (*FileRead)
    (
        LPVOID              lpvExtraParam,
        LPVOID              lpvFile,
        LPVOID              lpvBuffer,
        DWORD               dwNumBytes,
        LPDWORD             lpdwBytesRead
    );

    BOOL (*FileWrite)
    (
        LPVOID              lpvExtraParam,
        LPVOID              lpvFile,
        LPVOID              lpvBuffer,
        DWORD               dwNumBytes,
        LPDWORD             lpdwBytesWritten
    );

    BOOL (*FileTruncate)
    (
        LPVOID              lpvExtraParam,
        LPVOID              lpvFile
    );

    BOOL (*Progress)
    (
        LPVOID              lpvExtraParam,
        int                 nPercent
    );
} STRIP_DOC_CB_T, FAR *LPSTRIP_DOC_CB;

////////////////////////////////////////////////////////////
// Strip a document

typedef enum tagESTRIP_DOC_ERR
{
    eStripDocErrNone                = 0,
    eStripDocErrDetermineFileType   = 1,
    eStripDocErrNotOLEFile          = 2,
    eStripDocErrGeneral             = 3,
    eStripDocErrCreateCopy          = 4,
    eStripDocErrOpenOutputFile      = 5,
    eStripDocErrCloseOutputFile     = 6,
    eStripDocErrMalloc              = 7,
    eStripDocErrPasswordEncrypted   = 8,
    eStripDocErrStripExcel95        = 9,
    eStripDocErrStripExcel97        = 10,
    eStripDocErrStripWord95         = 11,
    eStripDocErrStripWord97         = 12,
    eStripDocErrProgressFalse       = 13
} ESTRIP_DOC_ERR_T;

StripDocImportExport ESTRIP_DOC_ERR_T StripDocumentCopy
(
    LPSTRIP_DOC_CB      lpstCallbacks,
    LPVOID              lpvExtraParam,
    LPVOID              lpvInputFileName,
    LPVOID              lpvOutputFileName
);

StripDocImportExport ESTRIP_DOC_ERR_T StripDocument
(
    LPSTRIP_DOC_CB      lpstCallbacks,
    LPVOID              lpvExtraParam,
    LPVOID              lpvFile
);

#endif // #ifndef _STRIPDOC_H_

