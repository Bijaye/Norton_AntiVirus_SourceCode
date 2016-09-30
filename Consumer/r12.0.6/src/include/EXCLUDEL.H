// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/excludel.h_v   1.1   10 Jul 1997 21:06:24   DALLEE  $
//
// Description:
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/excludel.h_v  $
// 
//    Rev 1.1   10 Jul 1997 21:06:24   DALLEE
// Set packing to 1-byte for exclude structures since they're all written
// to disk.
//************************************************************************

#ifndef EXCLUDE_LFN
#define EXCLUDE_LFN

                                        // Pack to 1-byte since all these
                                        // structures go to disk.
#ifdef SYM_WIN32
    #pragma pack(push)
    #pragma pack(1)
#endif

typedef WORD    EXCLUDELACTION;

#define EXCLUDELACTION_DONTSAVEFREEMEM  1
#define EXCLUDELACTION_SAVEDONTFREEMEM  2
#define EXCLUDELACTION_SAVEFREEMEM      3


/* 
    ---------------------------------------------------------------
    File header information.
    ---------------------------------------------------------------
*/

typedef struct tagEXCLUDELHEADER
{
    WORD    wSize;
    DWORD   dwVersion;
    WORD    wNumRecords;
    char    szSignature[15];
    BYTE    byReserved [ 64-2-4-2-15 ];

} EXCLUDELHEADER, FAR * LPEXCLUDELHEADER;

#define NAV_W95_VERSION     1
#define NAV_W95_SIGNATURE   "EXCLUDEL.DAT"
#define ENTRIES_PER_BLOCK   10

/* 
    ---------------------------------------------------------------
    These are the IDs that should be used with the LEXCLUDE.wFlags
    field.
    ---------------------------------------------------------------
*/

#define LEXCLUDE_VIRUS_FOUND    ((WORD)0x0001)
#define LEXCLUDE_FILE_CHANGE    ((WORD)0x0002)
#define LEXCLUDE_FILE_WATCH     ((WORD)0x0004)
#define LEXCLUDE_LOW_FORMAT     ((WORD)0x0008)
#define LEXCLUDE_WR_HBOOT       ((WORD)0x0010)
#define LEXCLUDE_WR_FBOOT       ((WORD)0x0020)
#define LEXCLUDE_WR_PROGLIST    ((WORD)0x0040)
#define LEXCLUDE_RO_CHANGE      ((WORD)0x0080)
#define LEXCLUDE_INOC_CHANGE    ((WORD)0x0100)


/* 
   ---------------------------------------------------------------------
   RLH - There are now TWO LEXCLUDE structures:
      - LEXCLUDE_DAT is used to store LFN exlusions on disk (always OEM)
      - LEXCLUDE is used for the in-memory copy and is in the native
        format (TCHAR).
   ---------------------------------------------------------------------
*/

typedef struct tagLEXCLUDE_DAT
{
    char    szItem [SYM_MAX_PATH + 1];
    BYTE    bSubDirectory;
    WORD    wFlags;

} LEXCLUDE_DAT, FAR * LPLEXCLUDE_DAT;


typedef struct tagLEXCLUDE
{
    TCHAR   szItem [SYM_MAX_PATH + 1];
    BYTE    bSubDirectory;
    WORD    wFlags;

} LEXCLUDE, FAR * LPLEXCLUDE;


/* 
    ---------------------------------------------------------------
    The EXCLUDELSTRUCT contains information to manage the life-cycle
    of the EXCLUDEL.DAT file.
    ---------------------------------------------------------------
*/

typedef struct tagEXCLUDELSTRUCT
{
    WORD        wNumEntries;
    LPLEXCLUDE  lpEntries;
    BOOL        bIsDirty;
    DWORD       dwEntriesSize;
    TCHAR       szFilePath [ SYM_MAX_PATH + 1 ];

} EXCLUDELSTRUCT, FAR * LPEXCLUDELSTRUCT, FAR * HLEXCLUDE;

/* 
    ---------------------------------------------------------------
    Possible return codes from the ExcludeLxxx() functions.
    ---------------------------------------------------------------
*/

typedef enum STATUSEXCLUDE
{
    STATUSEXCLUDE_OK = 0,
    STATUSEXCLUDE_ERR,
    STATUSEXCLUDE_DUPLICATE

} STATUSEXCLUDE;

/* 
    ---------------------------------------------------------------
    Function prototypes for all the functions that deal with long
    filename exclusions and that are visible to an application.
    ---------------------------------------------------------------
*/

HLEXCLUDE WINAPI ExcludeLOpen (         // To open/create and excludel.dat file
    LPCTSTR     lpExcludeFile,          // [in] name of file to open/create.
    BOOL        bCreate                 // [in] flag to create file
    );

STATUSEXCLUDE WINAPI ExcludeLClose (    // Closes a file opened ExcludeLOpen()
    HLEXCLUDE       hLExclude,          // [in] handle to file to close.
    EXCLUDELACTION  Action              // [in] flag indicating to free memory
    );

WORD WINAPI ExcludeLGetCount (          // Obtain number of entries in file
    HLEXCLUDE   hLExclude               // [in] handle to file to query
    );

STATUSEXCLUDE WINAPI ExcludeLAdd (      // Adds an entry to the file
    HLEXCLUDE   hLExclude,              // [in] handle to the file to add
    LPLEXCLUDE  lpItem                  // [in] pointer to item to add
    );

STATUSEXCLUDE WINAPI ExcludeLGet (      // Obtain an item from the file
    HLEXCLUDE   hLExclude,              // [in] handle to file
    WORD        wIndex,                 // [in] record number to get
    LPLEXCLUDE  lpItem                  // [out] info placed here
    );

STATUSEXCLUDE WINAPI ExcludeLSet (      // Set an item in the file
    HLEXCLUDE   hLExclude,              // [in] handle to file
    WORD        wIndex,                 // [in] record number to get
    LPLEXCLUDE  lpItem                  // [in] info to set
    );

STATUSEXCLUDE WINAPI ExcludeLDelete (   // Deletes an item from the file
    HLEXCLUDE   hLExclude,              // [in] handle to the file
    WORD        wIndex                  // [in] record number to delete
    );

HLEXCLUDE WINAPI ExcludeLCreateCopy (   // Create a copy of
    HLEXCLUDE   hLExclude               // [in] this handle
    );

STATUSEXCLUDE WINAPI ExcludeLKillCopy ( // Kill a previously created copy
    HLEXCLUDE   hLExclude               // [in] this handle
    );

STATUSEXCLUDE WINAPI ExcludeLObjectsIdentical ( 
    HLEXCLUDE   hThis,                  // [in] this handle
    HLEXCLUDE   hThat                   // [in] this handle
    );

                                        // Restore default packing.
#ifdef SYM_WIN32
    #pragma pack( pop )
#endif

#endif // #ifndef EXCLUDE_LFN
