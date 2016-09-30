// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:\include\vcs\inocapi.h_v   1.8   30 Oct 1994 12:00:42   JMILLARD  $
//
// Description: INOC API prototypes, handle definitions, & #defines
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:\include\vcs\inocapi.h_v  $
// 
//    Rev 1.8   30 Oct 1994 12:00:42   JMILLARD
// 
// fix spelling of unlock covers
// 
//    Rev 1.7   13 Sep 1994 16:54:32   CAREY
// now includes callback.h
// 
//    Rev 1.6   13 Sep 1994 15:59:56   CAREY
// added II_BOOL_UNINOCULATE_FAILED flag
// 
//    Rev 1.5   12 Sep 1994 15:52:32   CAREY
// 
//    Rev 1.4   07 Sep 1994 12:09:20   CAREY
// 
//    Rev 1.3   03 Sep 1994 16:04:40   CAREY
// 
//    Rev 1.2   02 Sep 1994 18:20:48   CAREY
// 
//    Rev 1.1   02 Sep 1994 18:05:10   CAREY
// 
//    Rev 1.0   02 Sep 1994 17:24:36   CAREY
// Initial revision.
//************************************************************************


#ifndef INOC_INCLUDE

#define INOC_INCLUDE

#include "callback.h"
#include "hashdb.h"
#include "fprot.h"

///////////////////////////////////////////////////////////////////////////////
//
// Please take a look at this prelim report and append any changes or comments
// which you have...
//                                              Carey
//
//
// TSR considerations
//-------------------
// 1. The TSR must access some list of drive/unc -> database path & name
//    mappings from a NAV configuration file in order to determine which
//    database to use for a given file.
//
// 2. The TSR will also have to support any file/record locking mechanisms
//    which the INOCAPI uses.
//
// General Considerations
//-------------------------
//
// 1. Each open context will require roughly 32+K of memory for quick lookup
//    items unless the INOC_NO_QUICK_LOOK_UP flag is specified.
//
// 2. Each open context will require one perpetually opened file handle unless
//    the INOC_OPEN_ON_ACCESS flag is specified. (not keeping the files open
//    will significantly slow things down...)
//
// 3. The user of the library must keep track of which files belong to which
//    database (context).
//
// 4. LOCKING:
//      a. lock database at 32MB (or some ungodly number). A process will
//         block (for a finite amount of time) on any database
//         UPDATE if this location is locked and return an error.
//      b. remember to re-load quick lookup information if the database
//         changes during scan. (not really necessary)
//      c. current database insertion routines need to be reordered such that
//         the chaining information of the parent record is updated after the
//         new record has been added so we have no dangling pointers...
//      d. need to open the database in the proper sharing mode (done yet?)
//
// 5. NAV must contain the same sort of drive/unc -> database path & name
//    mappings as the TSR (which are accessable to the TSR).  These should be
//    user configurable and stored in an easily accessable file in the TSR/NAV
//    startup directory.
//
// 6. Need to deal with windows and chicago file formats!  Are we concerned
//    with performing repairs on these files?  If so, some research will need
//    to be done.
//
// 7. Need to XOR the size of the file with the CRC in quick lookup.
//
// Initial time estimates (don't hold me to these just yet...):
//
//      1 working week to complete the C api.
//      1 working week to modify the necessary assembly routines and test
//      ? working weeks to udpate the TSR...
//
// 8. Need to standardize the callbacks for AV, INOC and INTEG engines
//
///////////////////////////////////////////////////////////////////////////////

// current revision value

#define         INOC_REVISION                           1

#define         WORK_BUFFER1_SIZE                       1024
#define         WORK_BUFFER2_SIZE                       256

#define         STORE_HEADER_SIZE                       35
#define         HEADER_START_OFFSET                     256
#define         RELOCATION_OFFSET                       0x18
#define         EXE_HEADER_OFFSET_OFFSET                0x3C
#define         EXTENDED_EXE_FORMAT_WORD                0x40

#define         WINDOWS_NE_FORMAT_WORD                  0x454EU
#define         WINDOWS_PE_FORMAT_WORD                  0x4550U
#define         OS2_LX_FORMAT_WORD                      0x584CU
#define         VXD_LE_FORMAT_WORD                      0x454CU

// wFlags flags for InocDBInit

#define         INOC_INIT_HANDLE_NO_ACCESS              0x0001
#define         INOC_INIT_CREATE                        0x0002
#define         INOC_INIT_SLOW_INOC                     0x0004
#define         INOC_INIT_OPEN_ON_ACCESS                0x0008

// INOCINFO

enum enumINOCINFO
{
        II_FILE_NAME,
        II_INDEX_NAME,
        II_BOOL_FILE_JUST_DELETED,
        II_BOOL_FILE_JUST_REPAIRED,
        II_BOOL_FILE_CHANGED,
        II_BOOL_NOT_INOCULATED,
        II_BOOL_NO_ACCESS,
        II_BOOL_FILE_JUST_UNINOCULATED,
        II_BOOL_REPAIR_FAILED,
        II_BOOL_DELETE_FAILED,
        II_DETECT_DATE,
        II_DETECT_TIME,
        II_BOOL_INOCULATE_FAILED,
        II_BOOL_UNINOCULATE_FAILED,
        II_BOOL_FILE_JUST_INOCULATED,
        II_MAX_SCAN_LEVEL,
        II_CUR_SCAN_LEVEL,
        II_CUR_REPAIR_LEVEL,
        II_BOOL_INOCULATION_LEVEL_ERROR
};


typedef enum enumINOCINFO INOCINFO;


#define     IIT_FILE_DELETED                    0x0001
#define     IIT_FILE_REPAIRED                   0x0002
#define     IIT_FILE_CHANGED                    0x0004
#define     IIT_NOT_INOCULATED                  0x0008
#define     IIT_NO_ACCESS                       0x0010
#define     IIT_REPAIR_FAILED                   0x0020
#define     IIT_DELETE_FAILED                   0x0040
#define     IIT_INOCULATE_FAILED                0x0080  // failed during current session
#define     IIT_PROTECT_INFO_INVALID            0x0100
#define     IIT_FILE_INOCULATED                 0x0200  // inoc'd during current session
#define     IIT_FILE_UNINOCULATED               0x0400
#define     IIT_UNINOCULATE_FAILED              0x0800
#define     IIT_INOCULATION_LEVEL_ERROR         0x1000

#define     IIT_ACTIONS (IIT_FILE_DELETED | IIT_FILE_REPAIRED |         \
                         IIT_REPAIR_FAILED | IIT_DELETE_FAILED |        \
                         IIT_INOCULATE_FAILED | IIT_FILE_INOCULATED |   \
                         IIT_FILE_UNINOCULATED | IIT_UNINOCULATE_FAILED)



typedef WORD    ISTATUS;


#define IS_OK                   ((ISTATUS)0)
#define IS_MEMALLOC             ((ISTATUS)1)
#define IS_FILE_ERROR           ((ISTATUS)2)
#define IS_CRIT_ERROR           ((ISTATUS)3)
#define IS_CANT_REPAIR          ((ISTATUS)4)
#define IS_INVALID_ARG          ((ISTATUS)5)
#define IS_CANT_DELETE          ((ISTATUS)6)
#define IS_CANT_INOCULATE       ((ISTATUS)7)
#define IS_CANT_UNINOCULATE     ((ISTATUS)8)
#define IS_WRONG_VERSION        ((ISTATUS)9)
#define IS_LOCK_ERROR           ((ISTATUS)10)
#define IS_INVALID_DATABASE     ((ISTATUS)11)
#define IS_USER_ABORT           ((ISTATUS)12)
#define IS_NO_ACCESS            ((ISTATUS)13)
#define IS_INOC_LEVEL_ERROR     ((ISTATUS)14)
#define IS_DELETED_RECORD       ((ISTATUS)15)

// size info

#define MAX_INDEX_SIZE          32767L


// scan/inoc levels

#define SECURITY_NONE           0
#define SECURITY_NORMAL         1
#define SECURITY_EXTENDED       2
#define SECURITY_FULL_CRC       3
#define SECURITY_USER_CRC       4           // not supported
#define SECURITY_MAX_LEVEL      5

// inoc commands

#define INOC_REPAIR_ITEM        0
#define INOC_GET_INFO           1
#define INOC_GET_QUICK_INFO     2


///////////////////////////////////////////////////////////////////////////////
// Data structures
///////////////////////////////////////////////////////////////////////////////

typedef struct
{
    CALLBACKREV1                stCallBack;
    char                        szInocDataBase[SYM_MAX_PATH];
    WORD                        wFlags;
    
    UINT                        uSizeIndex;
    DBC4_HEADER                 stDataBaseInfo;
    LPWORD                      lpQuickBuffer;
    LPBYTE                      lpBucketBuffer;
    LPBYTE                      lpWorkBuffer1;
    LPBYTE                      lpWorkBuffer2;
} INOCCONTEXTTYPE;

typedef     INOCCONTEXTTYPE     FAR *HICONTEXT;

#define     sINOCCONTEXTTYPE    sizeof(INOCCONTEXTTYPE)


typedef struct
{
    HICONTEXT                   hContext;       // * to parent context
    char                        szFileName[SYM_MAX_PATH]; // filename of item we scanned
    FILE_PROTECT                stProtectInfo;  // FIL4_PROTECT data
    WORD                        wFlags;         // flags...
    UINT                        uDate;          // date of detection
    UINT                        uTime;          // time of detection
    BYTE                        byMaxScanLevel; // maximum level of security
    BYTE                        byCurScanLevel; // level at which file was scanned
} INOCINFOTYPE;

typedef     INOCINFOTYPE        FAR *HINOC;
typedef     HINOC               FAR *LPHINOC;

#define     sINOCINFOTYPE       sizeof(INOCINFOTYPE)


#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
// prototypes
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Initialize an inoculation context.  This context should be used for an
// entire drive.  I.E. all files which will be looked up in the specified
// database will be from the same drive.  The location of the database does
// not need to be where the inoculated files reside.  If the user were to scan
// drive C, Q, and M the application would probably have to initialize 3
// different HICONTEXT's.
///////////////////////////////////////////////////////////////////////////////

ISTATUS         SYM_EXPORT  WINAPI          INOCDBInit
(
        UINT                    uRevisionNum,   // [in] revision # of structure
        LPCALLBACKREV1          lpCallBack,     // [in] filled callback struct
        LPSTR                   lpszDataBase,   // [in] database name
        WORD                    wFlags,         // [in] flags
        DWORD                   dwNumFiles,     // [in] # of files on device
        HICONTEXT               *lphContext     // [out] valid INOC context
);


///////////////////////////////////////////////////////////////////////////////
// This is used to perform an inoculation scan on a specified file.  The user
// must pass in the associated HICONTEXT for the drive where the file is stored
// on.  If the specified item has changed then a non-NULL handle is returned.
// If the specified item cannot be accessed and the
// INOC_INIT_NO_ACCESS_NO_HANDLE flag was not set during initialization, then
// a handle will be returned.
///////////////////////////////////////////////////////////////////////////////
ISTATUS         SYM_EXPORT      WINAPI          INOCScanFile
(
        HICONTEXT               hContext,       // [in] valid context
        LPSTR                   lpszFileName,   // [in] filename to scan
        LPSTR                   lpszIndexName,  // [in] indexname to scan
        BYTE                    byScanLevel,    // [in] level of scanning
        LPHINOC                 lphInoc,        // [out] place ptr to hinoc here
        DWORD                   dwCookie
);



///////////////////////////////////////////////////////////////////////////////
// Close a context.
///////////////////////////////////////////////////////////////////////////////


ISTATUS         SYM_EXPORT      WINAPI          INOCDBClose
(
        HICONTEXT               hContext        // [in] valid context
);


///////////////////////////////////////////////////////////////////////////////
// Obtain inoculation information about an HINOC handle.  See INOCINFO enum...
///////////////////////////////////////////////////////////////////////////////

BOOL            SYM_EXPORT      WINAPI          INOCInfo
(
        HINOC                   hInoc,          // [in] valid inoc handle
        INOCINFO                info,           // [in] what info to retrieve
        LPSTR                   lpszResult      // [out] !BOOL results go here
);


///////////////////////////////////////////////////////////////////////////////
// Free an HINOC handle.
///////////////////////////////////////////////////////////////////////////////

ISTATUS         SYM_EXPORT      WINAPI          INOCReleaseHandle
(
        HINOC                   hInoc           // [in] valid inoc handle
);


///////////////////////////////////////////////////////////////////////////////
// Attempt to repair an inoculated file.
///////////////////////////////////////////////////////////////////////////////


ISTATUS         SYM_EXPORT      WINAPI          INOCRepairFile
(
        HINOC                   hInoc,          // [in/out] valid inoc handle
        DWORD                   dwCookie
);


///////////////////////////////////////////////////////////////////////////////
// Attempt to delete an inoculated file.
///////////////////////////////////////////////////////////////////////////////

ISTATUS         SYM_EXPORT      WINAPI          INOCDeleteFile
(
        HINOC                   hInoc           // [in/out] valid inoc handle
);


///////////////////////////////////////////////////////////////////////////////
// Inoculate a new file!
///////////////////////////////////////////////////////////////////////////////
ISTATUS         SYM_EXPORT      WINAPI          INOCInoculateFile
(
        HICONTEXT               hContext,       // [in] valid context
        LPSTR                   lpszFileName,   // [in] filename to inoc (null)
        LPSTR                   lpszIndexName,  // [in] indexname to scan
        HINOC                   hInoc,          // [in] inoc handle to inoc (null)
        BYTE                    byScanLevel,    // [in] inoculation level info
        BYTE                    byRepairLevel,  // [in] what amt of info to save
                                                // currently not implemented
        DWORD                   dwCookie
);

///////////////////////////////////////////////////////////////////////////////
// Uninoculate an inoculated file!
///////////////////////////////////////////////////////////////////////////////

ISTATUS         SYM_EXPORT      WINAPI          INOCUninoculateFile
(
        HICONTEXT               hContext,       // [in] valid context
        LPSTR                   lpszIndexName,  // [in] indexname to scan
        HINOC                   hInoc           // [in] valid inoc handle or NULL
);


ISTATUS         SYM_EXPORT      WINAPI          INOCGoSlowInoc
(
    HICONTEXT                   hContext        // [in] valid context
);

ISTATUS         SYM_EXPORT      WINAPI          INOCGoFastInoc
(
    HICONTEXT                   hContext        // [in] valid context
);

ISTATUS     SYM_EXPORT      WINAPI          INOCDBRebuild
(
    HICONTEXT                   hContext,
    DWORD                       dwNewNumFiles,
    DWORD                       dwCookie
);

ISTATUS     SYM_EXPORT      WINAPI          INOCDBPurge
(
    HICONTEXT                   hContext,
    BOOL                        (*IsItemValid)(LPSTR lpszIndexName,
                                               DWORD dwCookie),
    DWORD                       dwCookie
);

ISTATUS     SYM_EXPORT      WINAPI          INOCOpenOnAccess(
    HICONTEXT                   hContext
);

ISTATUS     SYM_EXPORT      WINAPI          INOCOpenAlways
(
    HICONTEXT                   hContext
);


ISTATUS SYM_EXPORT      WINAPI          INOCDBTotalRecords
(
    HICONTEXT                   hContext,
    LPDWORD                     lpdwNumEntries
);

ISTATUS SYM_EXPORT      WINAPI          INOCDBGetRecord
(
    HICONTEXT                   hContext,
    DWORD                       dwRecordNum,
    LPHINOC                     lphInoc
);


#ifdef __cplusplus
}
#endif

///////////////////////////////////////////////////////////////////////////////
// internal support routine prototypes
///////////////////////////////////////////////////////////////////////////////

DBSTATUS LOCAL INOCHashOpen
(
    HICONTEXT hContext,
    BOOL      bCreate
);

DBSTATUS LOCAL INOCHashClose
(
    HICONTEXT hContext
);

BOOL WINAPI INOCCompare (LPVOID lpEntry1,LPVOID lpEntry2);
WORD WINAPI INOCHash (LPVOID lpEntry);
WORD WINAPI INOCQuick(LPVOID lpProt);
BYTE WINAPI INOCBucket(LPVOID lpProt);

HINOC LOCAL SetHINOCFields
(
    HICONTEXT           hContext,
    FILE_PROTECT        FAR *lpFileProtect,
    WORD                wFlags,
    UINT                uDate,
    UINT                uTime,
    LPSTR               lpszFileName,
    BYTE                byMaxScanLevel,
    BYTE                byCurScanLevel
);

ISTATUS WINAPI INOCProtect (WORD               wAction,
                            BYTE               byInocLevel,
                            LPSTR              lpszFileName,
                            LPSTR              lpszIndexName,
                            LPFILE_PROTECT     lpFileProtect,
                            LPBYTE             lpWorkBuffer1,
                            LPBYTE             lpWorkBuffer2,
                            DWORD              dwCookie
                           );

BOOL        INOCInfoChanged(LPFILE_PROTECT  lpOrig,
                            LPFILE_PROTECT  lpCur,
                            BYTE            byScanLevel);

#define     INOCFileOpen         hContext->stCallBack.FileOpen
#define     INOCFileClose        hContext->stCallBack.FileClose
#define     INOCFileRead         hContext->stCallBack.FileRead
#define     INOCFileWrite        hContext->stCallBack.FileWrite
#define     INOCFileSeek         hContext->stCallBack.FileSeek
#define     INOCFileSize         hContext->stCallBack.FileSize
#define     INOCFileGetDateTime  hContext->stCallBack.FileGetDateTime
#define     INOCFileSetDateTime  hContext->stCallBack.FileSetDateTime
#define     INOCFileDelete       hContext->stCallBack.FileDelete
#define     INOCFileGetAttr      hContext->stCallBack.FileGetAttr
#define     INOCFileSetAttr      hContext->stCallBack.FileSetAttr
#define     INOCFileCreate       hContext->stCallBack.FileCreate

#define     INOCFileLock         hContext->stCallBack.FileLock
#define     INOCFileUnlock       hContext->stCallBack.FileUnlock
#define     INOCProgress         hContext->stCallBack.Progress

#define     INOCSDiskReadPhysical  hContext->stCallBack.SDiskReadPhysical
#define     INOCSDiskWritePhysical hContext->stCallBack.SDiskWritePhysical

#define     INOCTempMemoryAlloc   hContext->stCallBack.TempMemoryAlloc
#define     INOCPermMemoryAlloc   hContext->stCallBack.PermMemoryAlloc

#define     INOCTempMemoryFree    hContext->stCallBack.TempMemoryFree
#define     INOCPermMemoryFree    hContext->stCallBack.PermMemoryFree

#define     gINOCFileOpen         lpgCallBack->FileOpen
#define     gINOCFileClose        lpgCallBack->FileClose
#define     gINOCFileRead         lpgCallBack->FileRead
#define     gINOCFileWrite        lpgCallBack->FileWrite
#define     gINOCFileSeek         lpgCallBack->FileSeek
#define     gINOCFileSize         lpgCallBack->FileSize
#define     gINOCFileGetDateTime  lpgCallBack->FileGetDateTime
#define     gINOCFileSetDateTime  lpgCallBack->FileSetDateTime
#define     gINOCFileDelete       lpgCallBack->FileDelete
#define     gINOCFileGetAttr      lpgCallBack->FileGetAttr
#define     gINOCFileSetAttr      lpgCallBack->FileSetAttr
#define     gINOCFileCreate       lpgCallBack->FileCreate

#define     gINOCFileLock         lpgCallBack->FileLock
#define     gINOCFileUnlock       lpgCallBack->FileUnlock
#define     gINOCProgress         lpgCallBack->Progress

#define     gINOCSDiskReadPhysical  lpgCallBack->SDiskReadPhysical
#define     gINOCSDiskWritePhysical lpgCallBack->SDiskWritePhysical

#define     gINOCTempMemoryAlloc   lpgCallBack->TempMemoryAlloc
#define     gINOCPermMemoryAlloc   lpgCallBack->PermMemoryAlloc

#define     gINOCTempMemoryFree    lpgCallBack->TempMemoryFree
#define     gINOCPermMemoryFree    lpgCallBack->PermMemoryFree

#endif
