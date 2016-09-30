// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// External header for NAVAPI scanning and repair interface.
//
//************************************************************************

#if !defined(__NAVAPI_H) || defined(NAVAPI_PROTOTYPES_ONLY)
#ifndef __NAVAPI_H
#define __NAVAPI_H
#endif

// Define this symbol when you only want the function prototypes visible.
// This is used internally ... please ignore.
#if !defined(NAVAPI_PROTOTYPES_ONLY)

// Define compiler specific structure packing. This ensures that both the 
// NAVAPI and the calling applications have the same structure packing for
// our structures.
//      Win32           8 byte packing
//      NLM             8 byte packing
//      Win16, DOS      1 byte packing

//      <others>        <error>
#if defined(WIN32) || defined (SYM_WIN32)
    #pragma pack(push, 8)
    #define NAVAPI_POP_PRAGMA	pack(pop)

#elif defined(NLM) || defined (SYM_NLM)
    #pragma pack(__push,8)
    #define NAVAPI_POP_PRAGMA	pack(__pop)

#elif defined(WIN16) || defined(SYM_WIN16) || defined(SYM_DOS) || defined(DOS)
    #pragma pack(1)
#else
    // If you recieve this message, contact Symantec AV Core group
    // for an updated header file for your platform. Note that if your
    // compiler does not support "push"ing the packing size, you must
    // specify a packing size of 1 for at least this header. This is 
    // because NAVAPI will use a packing of 1.
    #error No packing order defined for this platform / compiler.
#endif

// forward declarations

typedef struct tag_VCONTEXTTYPE     FAR *HNAVENGINE;
typedef struct tag_HVIRUS           FAR *HNAVVIRUS;
typedef struct tag_HVIRUSDEF        FAR *HNAVVIRUSDEF;
typedef struct tag_HVIRUSDEFTABLE   FAR *HNAVVIRUSDEFTABLE;


// Calling conventions

#ifdef WIN32
    #ifdef _NAVAPIDLL_
        #define NAVCALLAPI __declspec(dllexport) WINAPI
    #else
        #define NAVCALLAPI __declspec(dllimport) WINAPI
    #endif
    #define NAVCALLBACK
#elif _WINDOWS
    #ifdef _NAVAPIDLL_
        #define NAVCALLAPI WINAPI _export
    #else
        #define NAVCALLAPI WINAPI
    #endif
    #define NAVCALLBACK WINAPI _loadds
#else
    #define NAVCALLAPI
    #define NAVCALLBACK
#endif


/////////////////////////////////////////////////////////////////////////////
// Defines and enumerations.


// Return Status

typedef DWORD   NAVSTATUS;
typedef LPDWORD LPNAVSTATUS;

#define NAV_OK                      0
#define NAV_ERROR                   1
#define NAV_INSUFFICIENT_BUFFER     2
#define NAV_INVALID_ARG             3
#define NAV_MEMORY_ERROR            4
#define NAV_NO_ACCESS               5
#define NAV_CANT_REPAIR             6
#define NAV_CANT_DELETE             7
#define NAV_ENGINE_IN_USE           8
#define NAV_VXD_INIT_FAILURE        9
#define NAV_DEFINITIONS_ERROR       10

// Virus info enumerations

enum    enumNAVVirusInfo   {

        NAV_VI_VIRUS_NAME,              // obtain virus's name
        NAV_VI_VIRUS_ALIAS,             // obtain virus's alias list
        NAV_VI_VIRUS_INFO,              // obtain virus information
        NAV_VI_VIRUS_SIZE,              // obtain virus's size

        NAV_VI_BOOL_FIRST = 0x20,       // (Internal) must always be the first NAV_VI_BOOL_* value.
        NAV_VI_BOOL_INFECT_FILES,       // does this virus infect FILES in general? (MAC too)
        NAV_VI_BOOL_INFECT_EXE,         // does this virus infect EXE's?
        NAV_VI_BOOL_INFECT_COM,         // does this virus infect COM's?
        NAV_VI_BOOL_INFECT_COMCOM,      // does this virus infect COMMAND.COM?
        NAV_VI_BOOL_INFECT_SYS,         // does this virus infect SYS's?
        NAV_VI_BOOL_INFECT_MBR,         // does this virus infect MBR's?
        NAV_VI_BOOL_INFECT_FLOPPY_BOOT, // does this virus infect floppies?
        NAV_VI_BOOL_INFECT_HD_BOOT,     // does this virus infect HD bootsecs?

        NAV_VI_BOOL_INFECT_WILD,        // is this virus in the wild?

        NAV_VI_BOOL_MEMORY_RES,         // can this virus go memory resident?
        NAV_VI_BOOL_SIZE_STEALTH,       // does this virus size stealth?
        NAV_VI_BOOL_FULL_STEALTH,       // does this virus full stealth?
        NAV_VI_BOOL_TRIGGER,            // does this virus have a trigger?
        NAV_VI_BOOL_ENCRYPTING,         // is this virus encrypting?
        NAV_VI_BOOL_POLYMORPHIC,        // is this virus polymorphic?
        NAV_VI_BOOL_MULTIPART,          // is this virus multipartite?
        NAV_VI_BOOL_MAC_VIRUS,          // macintosh virus
        NAV_VI_BOOL_MACRO_VIRUS,        // macro virus
        NAV_VI_BOOL_WINDOWS_VIRUS,      // windows virus
        NAV_VI_BOOL_AGENT_VIRUS,        // agent virus
        NAV_VI_BOOL_SUBMITABLE,         // is this item something that SARC wants a copy of?
        NAV_VI_BOOL_HEURISTIC,          // detected with Bloodhound heuristics?


        ///////////////////////////////////////////////////////////////////////
        // item specific queries
        ///////////////////////////////////////////////////////////////////////

        NAV_VI_BOOL_REPAIRED,           // has this specific item been repaired?
        NAV_VI_BOOL_REPAIR_FAILED,      //
        NAV_VI_BOOL_REPAIRABLE,         // is this specific item repairable?
        NAV_VI_BOOL_DELETABLE,          // is this virus deletable?
        NAV_VI_BOOL_NO_ACCESS,          // check if we have access to this item
        NAV_VI_BOOL_DISEMBODIED,        // is the item disembodied (mem/boo)
        NAV_VI_BOOL_DELETED,            // has this file been deleted yet?
        NAV_VI_BOOL_DELETE_FAILED,      //
        NAV_VI_BOOL_DEF_DELETED,        // def been deleted from VIRSCAN.DAT?
        NAV_VI_BOOL_INFO_VALID,         // is this information valid?
        NAV_VI_BOOL_MEMORY_DETECT,      // is this item a memory detection?
        NAV_VI_BOOL_FILE_ITEM,          // is this item a file detection?
        NAV_VI_BOOL_BOOT_ITEM,          // is this item a boot detection?
        NAV_VI_BOOL_PART_ITEM,          // is this item a part detection?
        NAV_VI_BOOL_LAST,               // (Internal) must always be the last NAV_VI_BOOL_* value.

        NAV_VI_NEXT = 0xFF,             // (Internal) Ensure we have room to add more NAV_VI_BOOL_* values.

        NAV_VI_ITEM_NAME,               // the item name (filename or otherwise)
        NAV_VI_DETECT_DATE_TIME,        // standard date/time (time_t) when the item was detected.
        NAV_VI_PHYSICAL_DRIVE_NUM,      // get physical drive # of BR or MBR item
        NAV_VI_PARTITION_NUM,           // get partition num of BR item
        NAV_VI_PARTITION_TYPE,          // get partition type of BR item

        NAV_VI_VIRUS_ID                 // obtain virus's ID
        };
typedef enum enumNAVVirusInfo NAVVIRINFO;


typedef struct tagNAVVIRUSDBINFO
{
    DWORD   dwVersion;
    WORD    wDay, wMonth, wYear;
} NAVVIRUSDBINFO, FAR *LPNAVVIRUSDBINFO;

typedef struct tagNAVVIRUSDBINFOEX
{
    DWORD   dwSize;                     // sizeof (NAVVIRUSDBINFOEX). Caller initializes.
    DWORD   dwVersion;                  // definition's version number
    WORD    wDay, wMonth, wYear;        // definition's creation date
    DWORD   dwSequence;                 // definition's sequence number
} NAVVIRUSDBINFOEX, FAR *LPNAVVIRUSDBINFOEX;

// Client file date time type defines

#define NAV_TIME_LASTWRITE      0
#define NAV_TIME_LASTACCESS     4
#define NAV_TIME_CREATION       6


// Client provided file callbacks

struct tagNAVFILEIO {
    BOOL  (NAVCALLBACK FAR *Open)   ( LPVOID lpvFileInfo, DWORD dwOpenMode, LPVOID FAR *lplpvHandle );
    BOOL  (NAVCALLBACK FAR *Close)  ( LPVOID lpvHandle );
    DWORD (NAVCALLBACK FAR *Seek)   ( LPVOID lpvHandle, LONG lOffset, int nFrom );
    UINT  (NAVCALLBACK FAR *Read)   ( LPVOID lpvHandle, LPVOID lpvBuffer, UINT uBytesToRead );
    UINT  (NAVCALLBACK FAR *Write)  ( LPVOID lpvHandle, LPVOID lpvBuffer, UINT uBytesToWrite );
    BOOL  (NAVCALLBACK FAR *Delete) ( LPVOID lpvFileInfo );
    BOOL  (NAVCALLBACK FAR *GetDateTime) ( LPVOID lpvHandle, UINT uType, LPWORD lpwDate, LPWORD lpwTime );
    BOOL  (NAVCALLBACK FAR *SetDateTime) ( LPVOID lpvHandle, UINT uType, WORD wDate, WORD wTime );
    };

typedef struct tagNAVFILEIO NAVFILEIO, FAR *LPNAVFILEIO;

// Remove our structure packing. This ensures that follow on header files
// do not get our packing when they really want the default.
#ifdef NAVAPI_POP_PRAGMA
    #pragma NAVAPI_POP_PRAGMA
    #undef NAVAPI_POP_PRAGMA
#endif

#endif // if !defined(NAVAPI_PROTOTYPES_ONLY)


//------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------


#ifdef __cplusplus
extern "C" {
#endif

// init and close API's

HNAVENGINE NAVCALLAPI NAVEngineInit
(
    LPCSTR       lpszDefsPath,          // [in] path to DEFSS.
    LPCSTR       lpszNavexInfFile,      // [in] path & filename of NAVEX15.INF or NULL
    LPNAVFILEIO  lpFileCallbacks,       // [in] FileCallbacks or NULL
    WORD         wHeuristicLevel,       // [in] 0, 1, 2, or 3 (off, low, med, high)
    WORD         wFlags,                // [in] flags to specify options - or zero
    LPNAVSTATUS  lpStatus               // [out] status code
);

NAVSTATUS NAVCALLAPI NAVEngineClose
(
    HNAVENGINE hNAVEngine               // [in] a valid NAV engine handle
);

// scanning API's

NAVSTATUS NAVCALLAPI NAVScanFile
(
    HNAVENGINE     hNAVEngine,          // [in] NAV engine handle
    LPVOID         lpFileId,            // [in] pointer to client-defined file ID.
    LPCSTR         lpstFileExt,         // [in] The file's extention (COM, EXE, DOT)
    BOOL           bPreserveLastAccess, // [in] if TRUE, preserves last access date
    HNAVVIRUS FAR *lphVirus             // [out] receives handle if infected.
);

NAVSTATUS NAVCALLAPI NAVScanBoot
(
    HNAVENGINE     hNAVEngine,          // [in] a valid NAV engine handle
    char           cDriveLetter,        // [in] logical drive letter (A, C, etc.)
    HNAVVIRUS FAR *lphVirus             // [out] receives handle if infected.
);

NAVSTATUS NAVCALLAPI NAVScanMasterBoot
(
    HNAVENGINE     hNAVEngine,          // [in] a valid NAV engine handle
    UINT           uPhysDriveNum,       // [in] physical drive number (0x80, 0x81, etc.)
    HNAVVIRUS FAR *lphVirus             // [out] receives handle if infected.
);

NAVSTATUS NAVCALLAPI NAVScanMemory
(
    HNAVENGINE     hNAVEngine,          // [in] a valid NAV engine handle
    HNAVVIRUS FAR *lphVirus             // [out] receives handle if infected.
);


// repairing API's

NAVSTATUS NAVCALLAPI NAVRepairFile
(
    HNAVENGINE   hNAVEngine,            // [in] NAV engine handle
    LPVOID       lpFileId,              // [in] pointer to client-defined file ID.
    LPCSTR       lpstFileExt            // [in] The file's extention (COM, EXE, DOT)
);

NAVSTATUS NAVCALLAPI NAVDeleteFile
(
    HNAVENGINE   hNAVEngine,            // [in] NAV engine handle
    LPVOID       lpFileId,              // [in] pointer to client-defined file ID.
    LPCSTR       lpstFileExt            // [in] The file's extention (COM, EXE, DOT)
);

NAVSTATUS NAVCALLAPI NAVRepairBoot
(
    HNAVENGINE   hNAVEngine,            // [in] a valid NAV engine handle
    char         cDriveLetter           // [in] logical drive letter (A, C, etc.)
);

NAVSTATUS NAVCALLAPI NAVRepairMasterBoot
(
    HNAVENGINE   hNAVEngine,            // [in] a valid NAV engine handle
    UINT         uPhysDriveNum          // [in] physical drive number (0x80, 0x81, etc.)
);


// misc API's

NAVSTATUS NAVCALLAPI NAVFreeVirusHandle
(
    HNAVVIRUS    hVirus                 // [in] a valid HNAVVIRUS
);


// Virus information API's

DWORD NAVCALLAPI NAVGetVirusInfo
(
    HNAVVIRUS    hNAVVirus,             // [in]  Handle to an open virus.
    NAVVIRINFO   enumVirInfo,           // [in]  Item to get info on. (see NAVVIRINFO definition)
    LPSTR        lpszResultBuffer,      // [out] Information requested.
    LPDWORD      lpdwBufferSize         // [in/out]  Size of the return buffer, and characters returned.
);

#ifdef SYM_WIN32

NAVSTATUS NAVCALLAPI NAVIsInfectableFile
(
    HNAVENGINE   hNAVEngine,            // [in] NAV engine handle
    LPVOID       lpFileId,              // [in] pointer to client-defined file ID.
    LPCSTR       lpszFileExt,           // [in] The file's extention (COM, EXE, DOT)
    BOOL         bPreserveLastAccess,   // [in] if TRUE, preserves last access date
    LPBOOL       lpbInfectable          // [out] TRUE if file has the potential to be infected. FALSE otherwise.
);

#endif   // #ifdef SYM_WIN32

// Virus definition information API's.

NAVSTATUS NAVCALLAPI NAVGetVirusDefCount
(
    HNAVENGINE   hNAVEngine,            // [in]  Handle to an open NAV engine.
    LPDWORD      lpdwVirusCount         // [out] Number of viruses in definition set.
);

NAVSTATUS NAVCALLAPI NAVGetVirusDefMaxCount
(
    HNAVENGINE   hNAVEngine,            // [in]  Handle to an open NAV engine.
    LPDWORD      lpdwVirusMaxCount      // [out] Maximum number of viruses possible for any definition set.
);

NAVSTATUS NAVCALLAPI NAVGetVirusDefDetectCount
(
    HNAVENGINE   hNAVEngine,            // [in]  Handle to an open NAV engine.
    LPDWORD	     lpdwVirusCount         // [out] number of viruses the definition set detects.
);

// Individual virus definition information API's.

HNAVVIRUSDEF NAVCALLAPI NAVLoadVirusDef
(
    HNAVENGINE   hNAVEngine,            // [in] Handle to an open NAV engine.
    DWORD        dwVirusID              // [in] ID of virus to load from the database.
);

VOID NAVCALLAPI NAVReleaseVirusDef
(
    HNAVVIRUSDEF hNAVVirusDef           // [in] Handle to an open NAV Virus Definition.
);

DWORD NAVCALLAPI NAVGetVirusDefInfo
(
    HNAVVIRUSDEF hNAVVirusDef,          // [in]  Handle to an open Virus Definition.
    NAVVIRINFO   enumVirInfo,           // [in]  Item to get info on. (see NAVVIRINFO definition)
    LPSTR        lpszResultBuffer,      // [out] Information requested.
    LPDWORD      lpdwBufferSize         // [in/out]  Size of the return buffer, and characters returned.
);


// Table type virus definition information API's.

HNAVVIRUSDEFTABLE NAVCALLAPI NAVLoadVirusDefTable
(
    HNAVENGINE        hNAVEngine        // [in] Handle to an open NAV Engine.
);

VOID NAVCALLAPI NAVReleaseVirusDefTable
(
    HNAVVIRUSDEFTABLE hNAVVirusDefTable // [in] Handle to an allocated virus definition table.
);

NAVSTATUS NAVCALLAPI NAVGetVirusDefTableCount
(
    HNAVVIRUSDEFTABLE hNAVVirusDefTable,// [in]  Handle to an open Virus Definition List.
    LPDWORD           lpdwVirusDefCount // [out] Number of virus definitions in this table.
);

DWORD NAVCALLAPI NAVGetVirusDefTableInfo
(
    HNAVVIRUSDEFTABLE hNAVVirusDefTable,// [in]  Handle to an open Virus Definition List.
    DWORD             dwIndex,          // [in]  Index of item to get.
    NAVVIRINFO        enumVirInfo,      // [in]  Item to get info on. (see NAVVIRINFO definition)
    LPSTR             lpszResultBuffer, // [out] Information requested.
    LPDWORD           lpdwBufferSize    // [in/out]  Size of the return buffer, and characters returned.
);

NAVSTATUS NAVCALLAPI NAVGetNavapiVersionString
(
    LPSTR  lpszVersionBuffer,           // [out] buffer to copy string into
    UINT   uBufferSize                  // [in]  size of lpszVersionBuffer
);

// Obsolete function. Call NAVGetVirusDBInfoEx instead.
NAVSTATUS NAVCALLAPI NAVGetVirusDBInfo
(
    HNAVENGINE       hNAVEngine,        // [in]  initialized NAV Engine handle.
    LPNAVVIRUSDBINFO lpVirusDBInfo      // [out] database information structure.
);

NAVSTATUS NAVCALLAPI NAVGetVirusDBInfoEx
(
    HNAVENGINE         hNAVEngine,      // [in]  initialized NAV Engine handle.
    LPNAVVIRUSDBINFOEX lpVirusDBInfo    // [out] database information structure.
);

#ifdef __cplusplus
}
#endif


#endif // if !defined(__NAVAPI_H) || defined(NAVAPI_PROTOTYPES_ONLY)
