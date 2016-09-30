// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/include/VCS/exclapi.h_v   1.0   06 Feb 1997 20:51:56   RFULLER  $
//
// Description:
//      Exclusions API header.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/include/VCS/exclapi.h_v  $
// 
//    Rev 1.0   06 Feb 1997 20:51:56   RFULLER
// Initial revision
// 
//    Rev 1.0   30 Jan 1996 15:47:00   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 10:11:22   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Sep 1994 10:48:52   DALLEE
// Initial revision.
//************************************************************************

#ifndef __EXCLAPI_H
#define __EXCLAPI_H

#include "callback.h"


#define EXCL_VERSION    0x0001

                                        // Exclude API status returns.
typedef enum tagESTATUS
    {
    ES_OK = 0,
    ES_MEMALLOC,
    ES_FILE_ERROR,
    ES_INVALID_ARG,
    ES_OUT_OF_SYNC,
    ES_OVERRUN,
    ES_WRONG_VERSION,
    ES_NOT_LOCKED,
    ES_DATA_CORRUPTED,
    ES_NO_DATABASE
    } ESTATUS;


#define EXCL_DB_SIG     (WORD)0xEEDB

                                        // Information saved in exclusion
                                        // header.
typedef struct tagEXCLHEADERINFO
    {
    WORD        wSigniture;             // Must be EXCL_DB_SIG.
    WORD        wVersion;               // EXCL_VERSION on creation.
    WORD        wSizeHeader;            // sizeof(EXCLHEADER).
    DWORD       dwNumEntries;           // Number of entries in database.
    } EXCLHEADERINFO;

                                        // Actual header written to database.
                                        // 256 bytes total: EXCLHEADERINFO
                                        // followed by zero's.
typedef struct tagEXCLHEADER
    {
    EXCLHEADERINFO  Info;
    BYTE            Unused [256 - sizeof(EXCLHEADERINFO)];
    } EXCLHEADER;


// Exclude API string defines.
// These are queried w/ EXCLGetString().  The first four are predefined.
// Applications are free to use as many extra strings as they wish starting
// with index number ESZ_APP_STRING_1.  The present implementation will
// use a single fixed buffer for all string data.  Calling EXCLSetString()
// with too long a string, will fail and return ES_OVERRUN.

typedef enum tagESTRING
    {
    ESZ_VOLUME = 0,                     // Volume portion of exclusion spec.
    ESZ_DIRECTORY,                      // ...and so on...
    ESZ_FILENAME,
    ESZ_FILETYPE,
    ESZ_APP_STRING_1,                   // First ID for app. defined strings.
    } ESTRING;


// Exclude API dword defines.
// Applications are restricted to the following number of DWORD fields.
// The DWORDs are accessed with EXCLSet/GetDword() using indices
// 0 to (EXCL_NUM_DWORDS - 1).

#define EXCL_NUM_DWORDS     5

                                        // Container to hold HEXCLUDE's.
typedef struct tagEXCLUDEBOX
    {
    struct tagEXCLUDECONTEXTTYPE FAR *hEContext;
    DWORD           dwCount;
    DWORD           dwElementsAllocated;

    struct tagEXCLUDEMEMORYINFOTYPE FAR * FAR *lpHandles;
    } EXCLUDEBOX, NEAR *NPEXCLUDEBOX, *PEXCLUDEBOX, FAR *LPEXCLUDEBOX;


// Prototype for application defined match function.
// lpszFileSpec and dwCookie are both provided by the application when
// calling either EXCLIsExcluded() or EXCLFindMatches().
// The actual function should probably be declared with SYM_EXPORT, or
// LOADDS as appropriate.

typedef BOOL (WINAPI *EXCLUDEMATCHFUNC)(
        struct  tagEXCLUDEMEMORYINFOTYPE FAR *hExclude,
        LPCSTR  lpszFileSpec,
        DWORD   dwCookie );


// Internal Flags for EXCLUDECONTEXTTYPE (HECONTEXT)

#define EXCL_DB_LOCKED      0x0001      // Database is locked for modifications.

// Context for a single exclude database.  An HECONTEXT is returned from
// the init call and is used to reference an exclusions database.

typedef struct tagEXCLUDECONTEXTTYPE
    {
    char                szDataBase [SYM_MAX_PATH + 1];

    CALLBACKREV1        stCallBack;
    EXCLUDEMATCHFUNC    lpfnMatchFunc;

    HFILE               hFile;          // Handle from FileOpen callback.

    WORD                wFlags;

    struct tagEXCLUDEBOX    rHandleBox;
    } EXCLUDECONTEXTTYPE;

typedef EXCLUDECONTEXTTYPE FAR *    HECONTEXT;
typedef HECONTEXT FAR *             LPHECONTEXT;


//------------------------------------------------------------------------
// HEXCLUDE - "handle" to a single exclude specification.
//
// Strings are stored in lpBuffer in the following format:
//      lpBuffer [0]        'E' safety check. (EXCL_STRING_START_CHAR)
//      lpBuffer [1..2]     WORD STRLEN(String0)
//      lpBuffer [3..n]     String0 with EOS.
//       (repeat)
//      Slack space filled with '\0'.
//
// Applications should query the DWORDS and Strings with
//  EXCLGetDword() and EXCLGetString().
//&? Must have indicator for unwritten blank for ReleaseEntry()...
//  or some method to deal with this.
//------------------------------------------------------------------------

#define EXCL_SIZE_STRING_BUFFER     (2 * SYM_MAX_PATH)
#define EXCL_STRING_START_CHAR      '\xEE'

// Internal flags for EXCLUDEMEMORYINFOTYPE wFlags field.
                                        // This is a "scratch" entry
#define EXCL_SCRATCH_ENTRY      0x0001  // from EXCLCreateBlank().


                                        // Structure saved to database.
typedef struct tagEXCLUDESAVEINFOTYPE
    {
    WORD        wNumDwords;             // Number of DWORDS.
    WORD        wNumStrings;            // Number of strings defined.

    WORD        wType;                  // Application defined type.

                                        // Application defines DWORD usage.
    DWORD       aDwords [EXCL_NUM_DWORDS];

                                        // List of strings attached
                                        // to this exclusion.
    BYTE        aStrings [EXCL_SIZE_STRING_BUFFER];
    } EXCLUDESAVEINFOTYPE;

                                        // Exclude structure used in memory.
typedef struct tagEXCLUDEMEMORYINFOTYPE
    {
    HECONTEXT           hEContext;      // Exclude context info.
    DWORD               dwDBIndex;      // Index in disk database.
    WORD                wFlags;         // Internal flags.
    UINT                uLockCount;     // Number of times handle retrieved
                                        // by application.

    EXCLUDESAVEINFOTYPE Info;           // Actual exclusion info from database.
    } EXCLUDEMEMORYINFOTYPE;

typedef EXCLUDEMEMORYINFOTYPE FAR * HEXCLUDE;
typedef HEXCLUDE FAR *              LPHEXCLUDE;

// This is the structure used to hold all matches found with a call
// to EXCLFindMatches().  The application will receive an HMATCHBOX
// which it can query with EXCLGetNumMatches() and EXCLGetMatch().

typedef struct tagMATCHBOXTYPE
    {
    HECONTEXT   hEContext;

    EXCLUDEBOX  rHandleBox;
    } MATCHBOXTYPE;

typedef MATCHBOXTYPE FAR *      HMATCHBOX;
typedef HMATCHBOX FAR *         LPHMATCHBOX;


//------------------------------------------------------------------------
// PROTOTYPES
//------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif


// Open an exclusions database.

ESTATUS SYM_EXPORT WINAPI EXCLDBInit (
    UINT              uRevision,
    LPCALLBACKREV1    lpCallBack,
    LPCSTR            lpszDataBase,
    WORD              wFlags,
    EXCLUDEMATCHFUNC  lpfnMatchFunc,
    LPHECONTEXT       lphEContext );

// Flags for EXCLDBInit().

#define EXCLINIT_CREATE         0x0001  // Create database if missing.


// Close exclusions database.  (Nice comment, eh?)

ESTATUS SYM_EXPORT WINAPI EXCLDBClose (
    HECONTEXT   hEContext );


// Returns the number of exclusions present in the database.

DWORD SYM_EXPORT WINAPI EXCLGetNumEntries (
    HECONTEXT   hEContext );


// Returns a handle to an exclusion specified by index.

ESTATUS SYM_EXPORT WINAPI EXCLGetEntry (
    HECONTEXT   hEContext,
    DWORD       dwEntry,                // Zero-based entry index.
    LPHEXCLUDE  lphExclude );           // [out] Place exclusion handle here.


// Frees an HEXCLUDE obtained from EXCLGetEntry(), EXCLGetMatch(), or
// EXCLCreateBlank().

ESTATUS SYM_EXPORT WINAPI EXCLReleaseEntry (
    HEXCLUDE    hExclude );


// Used to query TRUE/FALSE whether a file is excluded.
// Flags for type of exclusion, etc., should be passed in an application
// defined info dwCookie expected by the app defined match callback.
// This function will return TRUE after the first match found, or FALSE
// for no match.

BOOL SYM_EXPORT WINAPI EXCLIsExcluded (
    HECONTEXT   hEContext,
    LPCSTR      lpszFileSpec,           //&? Throw in dwAppCookie?
    DWORD       dwCookie );


// Used to retrieve handles to all exclusion records which a given file
// matches.  The HMATCHBOX returned can be queried to return HEXCLUDE's
// for each matching exclusion record.
// On success, EXCLReleaseMatchBox() must be called to free the
// memory associated with the HMATCHBOX returned.

ESTATUS SYM_EXPORT WINAPI EXCLFindMatches (
    HECONTEXT       hEContext,
    LPCSTR          lpszFileSpec,       //&? Put in query struct w/ Cookie?
    DWORD           dwCookie,
    LPHMATCHBOX     lphMatchBox );      // [out] Handle to list of matching
                                        // exclusion records.

// Use to free memory associated with HMATCHBOX returned from EXCLFindMatches().

ESTATUS SYM_EXPORT WINAPI EXCLReleaseMatchBox (
    HMATCHBOX       hMatchBox );


// Gets the number of HEXCLUDE's in a HMATCHBOX from EXCLFindMatches().

DWORD SYM_EXPORT WINAPI EXCLGetNumMatches (
    HMATCHBOX       hMatchBox );


// Gets the indexed HEXCLUDE from an HMATCHBOX.

ESTATUS SYM_EXPORT WINAPI EXCLGetMatch (
    HMATCHBOX       hMatchBox,          // Handle from EXCLFindMatches().
    DWORD           dwEntry,            // Zero-based entry index.
    LPHEXCLUDE      lphExclude );       // [out] Exclusion handle here.


// Gets the application specified wType from an exclusion record.

ESTATUS SYM_EXPORT WINAPI EXCLGetType (
    HEXCLUDE    hExclude,               // Exclusion item.
    LPWORD      lpwType );              // [out] Place type of exclusion here.


// When database is locked for modifications, this changes the type of
// an HEXCLUDE.

ESTATUS SYM_EXPORT WINAPI EXCLSetType (
    HEXCLUDE    hExclude,               // Exclusion item.
    WORD        wType );                // Type of exclusion.


// This retrieves the specified DWORD from an exclusion record.

ESTATUS SYM_EXPORT WINAPI EXCLGetDword (
    HEXCLUDE    hExclude,               // Exclusion item.
    UINT        uIndex,                 // Zero-based index of DWORD to get.
    LPDWORD     lpDword );              // [out] Place DWORD value here.


// When database is locked for modifications, this sets the indexed DWORD
// to the specified value.

ESTATUS SYM_EXPORT WINAPI EXCLSetDword (
    HEXCLUDE    hExclude,               // Exclusion item.
    UINT        uIndex,                 // Zero-based index of DWORD to set.
    DWORD       dwValue );              // Value to save.


// Used to retrieve the indexed string into the buffer provided.
// If the buffer is too small, (uBufferSize - 1) characters + EOS are written
// to lpBuffer, and ES_OVERRUN is returned.

ESTATUS SYM_EXPORT WINAPI EXCLGetString (
    HEXCLUDE    hExclude,               // Exclusion item.
    UINT        uIndex,                 // Zero-based index of String to get.
    LPSTR       lpBuffer,               // [out] Buffer to recieve string.
    UINT        uBufferSize );          // Size of buffer.


// When database is locked for modifications, this sets the indexed string
// to the specified value.  If the total length of all strings stored would
// be greater than the fixed buffer size, ES_OVERRUN is returned.

ESTATUS SYM_EXPORT WINAPI EXCLSetString (
    HEXCLUDE    hExclude,               // Exclusion item.
    UINT        uIndex,                 // Zero-based index of String to set.
    LPCSTR      lpszString );           // String to save.


// This call should be used to exclusively lock the database for modifications.
// This will probably return ES_OUT_OF_SYNC if the database has changed since
// EXCLDBInit().  If the EXCL_RESYNC flag is set, the in memory copy of
// exclusions will be resynced, and ES_OK will be returned.
//&? Icky implementation, eh?  Do we also need a distinct resync function??

ESTATUS SYM_EXPORT WINAPI EXCLDBLock (  // Lock DB for modifications.
    HECONTEXT   hEContext,              // Exclude context.
    WORD        wFlags );               // Set EXCL_RESYNC to resync w/o error.

// Flags for EXCLDBLock.

#define EXCL_RESYNC     0x0001


// Call once done making modifications.
//&? Queries here for out of sync?  If EXCLSetXXX() called, but no
// calls to EXCLWrite()??

ESTATUS SYM_EXPORT WINAPI EXCLDBUnlock (// Unlock DB after modifications.
    HECONTEXT   hEContext );            // Exclude context.


// Writes a modified exclusion record to the database.

ESTATUS SYM_EXPORT WINAPI EXCLWrite (
    HEXCLUDE    hExclude );             // Exclusion item to save after
                                        // modifications w/ EXCLSetDword/String.


// Deletes an exclusion record from the database.

ESTATUS SYM_EXPORT WINAPI EXCLDelete (
    HEXCLUDE    hExclude );


// Creates a new blank exclusion.  Set fields with EXCLSetXXX() then save
// with EXCLWrite().

ESTATUS SYM_EXPORT WINAPI EXCLCreateBlank (
    HECONTEXT   hEContext,
    LPHEXCLUDE  lphExclude );           // [out] Put new exclusion handle here.


//************************************************************************
// EXCLUDE ENGINE INTERNAL USE ONLY.
//************************************************************************

#define EXCLFileOpen        hEContext->stCallBack.FileOpen
#define EXCLFileClose       hEContext->stCallBack.FileClose
#define EXCLFileRead        hEContext->stCallBack.FileRead
#define EXCLFileWrite       hEContext->stCallBack.FileWrite
#define EXCLFileSeek        hEContext->stCallBack.FileSeek
#define EXCLFileSize        hEContext->stCallBack.FileSize
#define EXCLFileCreate      hEContext->stCallBack.FileCreate
#define EXCLFileDelete      hEContext->stCallBack.FileDelete

#define EXCLTempMemoryAlloc hEContext->stCallBack.TempMemoryAlloc
#define EXCLPermMemoryAlloc hEContext->stCallBack.PermMemoryAlloc

#define EXCLTempMemoryFree  hEContext->stCallBack.TempMemoryFree
#define EXCLPermMemoryFree  hEContext->stCallBack.PermMemoryFree


// EXCLUDEBOX functions

VOID WINAPI EXCLBoxInit (HECONTEXT hEContext, LPEXCLUDEBOX lpBox);
VOID WINAPI EXCLBoxClear (LPEXCLUDEBOX lpBox);
ESTATUS WINAPI EXCLBoxAdd (LPEXCLUDEBOX lpBox, HEXCLUDE hExclude);
DWORD WINAPI EXCLBoxGetCount (LPEXCLUDEBOX lpBox);
ESTATUS WINAPI EXCLBoxGetExclude (LPEXCLUDEBOX  lpBox,
                                  DWORD         dwIndex,
                                  LPHEXCLUDE    lphExclude);

#ifdef __cplusplus
}
#endif

#endif  // __EXCLAPI_H
