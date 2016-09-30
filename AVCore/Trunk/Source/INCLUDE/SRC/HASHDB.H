// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:\include\vcs\hashdb.h_v   1.0   02 Sep 1994 17:24:36   CAREY  $
//
// Description: HASH DB constants & data structures
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:\include\vcs\hashdb.h_v  $
// 
//    Rev 1.0   02 Sep 1994 17:24:36   CAREY
// Initial revision.
//************************************************************************

typedef UINT    DBSTATUS;

#define DBS_OK                  ((DBSTATUS)0)
#define DBS_MEMALLOC            ((DBSTATUS)1)
#define DBS_FILE_ERROR          ((DBSTATUS)2)
#define DBS_CRIT_ERROR          ((DBSTATUS)3)
#define DBS_INVALID_ARG         ((DBSTATUS)4)
#define DBS_LOCK_ERROR          ((DBSTATUS)5)
#define DBS_NOT_FOUND           ((DBSTATUS)6)
#define DBS_SEARCH_DB           ((DBSTATUS)7)
#define DBS_DELETED_RECORD      ((DBSTATUS)8)
#define DBS_INVALID_DATABASE    ((DBSTATUS)9)
#define DBS_SEARCH_QUICK        ((DBSTATUS)10)

#define VERSION_SIGNATURE       0x4E5343FFL
#define VERSION_NUMBER          0x40000000L

#define ZERO_BUFFER_SIZE        128

#define CRITICAL_ERROR          0xFFFFFFFFL

#define MAX_SEARCH_DEPTH        50

typedef struct
{
    DWORD   dwVersionSig;
    DWORD   dwVersionNumber;
    DWORD   dwNextRecordIndex;
    DWORD   dwFreePointer;
    WORD    wElemSize;
    WORD    wTotalElemSize;
    DWORD   dwIndexSize;
    DWORD   dwDataStart;
    DWORD   dwIndexStart;
    DWORD   dwQuickStart;
    DWORD   dwBucketInfoStart;
    BOOL    (WINAPI FAR *CompFunc)(LPVOID, LPVOID);
    WORD    (WINAPI FAR *HashFunc)(LPVOID);
    WORD    (WINAPI FAR *QuickFunc)(LPVOID);
    BYTE    (WINAPI FAR *BucketFunc)(LPVOID);
    UINT    hFile;
    BYTE    bFiller[6-sizeof(UINT)];        // round off size since UINT changes
} DBC4_HEADER;

typedef DBC4_HEADER FAR *LPDBC4_HEADER;

#define sDBC4_HEADER    sizeof(DBC4_HEADER)

//------------------------------------------------------------------------------
// Type of chaining used in hash table from DBC4_HEADER
//------------------------------------------------------------------------------
typedef struct
{
    DWORD   dwNext;
    BYTE    byActive;
} CHAIN_TYPE;

#define sCHAIN_TYPE     sizeof(CHAIN_TYPE)



#define DBFileOpen          lpgCallBack->FileOpen
#define DBFileClose         lpgCallBack->FileClose
#define DBFileRead          lpgCallBack->FileRead
#define DBFileWrite         lpgCallBack->FileWrite
#define DBFileCreate        lpgCallBack->FileCreate
#define DBFileDelete        lpgCallBack->FileDelete
#define DBFileSeek          lpgCallBack->FileSeek
#define DBFileSize          lpgCallBack->FileSize
#define DBFileGetDateTime   lpgCallBack->FileGetDateTime
#define DBFileSetDateTime   lpgCallBack->FileSetDateTime
#define DBFileGetAttr       lpgCallBack->FileGetAttr
#define DBFileSetAttr       lpgCallBack->FileSetAttr
#define DBFileLock          lpgCallBack->FileLock
#define DBFileUnlock        lpgCallBack->FileUnlock

