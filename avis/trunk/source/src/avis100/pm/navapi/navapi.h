// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// External header for NAVAPI scanning and repair interface.
//
//************************************************************************

#if !defined(__NAVAPI_H)
#define __NAVAPI_H


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
//typedef void            VOID;
typedef unsigned int    UINT;

//typedef unsigned short WORD;
typedef unsigned long       DWORD;
typedef int             BOOL;
typedef unsigned short  *LPWORD;
typedef int             *LPINT;
typedef long            LONG;
typedef char            *LPSTR;
typedef char            *LPTSTR;
typedef BYTE            *LPBYTE;
typedef void            *LPVOID;
typedef DWORD           *LPDWORD;
typedef BOOL            *LPBOOL;
typedef long            *LPLONG;
typedef LPSTR           *LPLPSTR;
typedef DWORD NAVSTATUS;

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

        NAV_VI_ITEM_NAME,               // the item name (filename or otherwise)
        NAV_VI_DETECT_DATE,
        NAV_VI_DETECT_TIME,
        NAV_VI_PHYSICAL_DRIVE_NUM,      // get physical drive # of scanned item
        NAV_VI_PARTITION_NUM,           // get partition num of scanned item
        NAV_VI_PARTITION_TYPE,          // get partition type of scanned item

        NAV_VI_VIRUS_ID                 // obtain virus's ID
        };
typedef enum enumNAVVirusInfo NAVVIRINFO;


typedef struct tagVIRUSDBINFO
{
    DWORD   dwVersion;
    unsigned short    wDay, wMonth, wYear;
} VIRUSDBINFO, * LPVIRUSDBINFO;


// Client file date time type defines

#define NAV_TIME_LASTWRITE      0
#define NAV_TIME_LASTACCESS     4
#define NAV_TIME_CREATION       6


// Client provided file callbacks

struct tagNAVFILEIO {
    DWORD (NAVCALLBACK FAR *Open)   ( LPVOID lpvFileInfo, DWORD dwOpenMode );
    BOOL  (NAVCALLBACK FAR *Close)  ( DWORD  dwHandle );
    DWORD (NAVCALLBACK FAR *Seek)   ( DWORD  dwHandle, LONG lOffset, int nFrom );
    UINT  (NAVCALLBACK FAR *Read)   ( DWORD  dwHandle, LPVOID lpvBuffer, UINT uBytesToRead );
    UINT  (NAVCALLBACK FAR *Write)  ( DWORD  dwHandle, LPVOID lpvBuffer, UINT uBytesToWrite );
    BOOL  (NAVCALLBACK FAR *Delete) ( LPVOID lpvFileInfo );
    BOOL  (NAVCALLBACK FAR *GetDateTime) ( DWORD dwHandle, UINT uType, LPWORD lpwDate, LPWORD lpwTime );
    BOOL  (NAVCALLBACK FAR *SetDateTime) ( DWORD dwHandle, UINT uType, unsigned short wDate, unsigned short wTime );
    };

typedef struct tagNAVFILEIO NAVFILEIO, FAR *LPNAVFILEIO;


#ifdef __cplusplus
extern "C" {
#endif

// init and close API's

HNAVENGINE NAVCALLAPI NAVEngineInit
(
    LPTSTR       lpszDefsPath,          // [in] path to DEFSS.
    LPTSTR       lpszNavexInfFile,      // [in] path & filename of NAVEX15.INF or NULL
    LPNAVFILEIO  lpFileCallbacks,       // [in] FileCallbacks or NULL
    unsigned short         wHeuristicLevel,       // [in] 0, 1, 2, or 3 (off, low, med, high)
    unsigned short         wFlags,                // [in] flags to specify options - or zero
    NAVSTATUS *  lpStatus               // [out] status code
);

NAVSTATUS NAVCALLAPI NAVEngineClose
(
    HNAVENGINE hNAVEngine               // [in] a valid NAV engine handle
);

// scanning API's

NAVSTATUS NAVCALLAPI NAVScanFile
(
    HNAVENGINE   hNAVEngine,            // [in] NAV engine handle
    LPVOID       lpFileId,              // [in] pointer to client-defined file ID.
    LPTSTR       lpstFileExt,           // [in] The file's extention (COM, EXE, DOT)
    BOOL         bPreserveLastAccess,   // [in] if TRUE, preserves last access date
    HNAVVIRUS*   lphVirus               // [out] receives handle if infected.
);

NAVSTATUS NAVCALLAPI NAVScanBoot
(
    HNAVENGINE   hNAVEngine,            // [in] a valid NAV engine handle
    char         cDriveLetter,          // [in] logical drive letter (A, C, etc.)
    HNAVVIRUS*   lphVirus               // [out] receives handle if infected.
);

NAVSTATUS NAVCALLAPI NAVScanMasterBoot
(
    HNAVENGINE   hNAVEngine,            // [in] a valid NAV engine handle
    UINT         uPhysDriveNum,         // [in] physical drive number (0x80, 0x81, etc.)
    HNAVVIRUS*   lphVirus               // [out] receives handle if infected.
);

NAVSTATUS NAVCALLAPI NAVScanMemory
(
    HNAVENGINE   hNAVEngine,            // [in] a valid NAV engine handle
    HNAVVIRUS*   lphVirus               // [out] receives handle if infected.
);


// repairing API's

NAVSTATUS NAVCALLAPI NAVRepairFile
(
    HNAVENGINE   hNAVEngine,            // [in] NAV engine handle
    LPVOID       lpFileId,              // [in] pointer to client-defined file ID.
    LPTSTR       lpstFileExt            // [in] The file's extention (COM, EXE, DOT)
);

NAVSTATUS NAVCALLAPI NAVDeleteFile
(
    HNAVENGINE   hNAVEngine,            // [in] NAV engine handle
    LPVOID       lpFileId,              // [in] pointer to client-defined file ID.
    LPTSTR       lpstFileExt            // [in] The file's extention (COM, EXE, DOT)
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
    LPTSTR       lpszResultBuffer,      // [out] Information requested.
    LPDWORD      lpdwBufferSize         // [in/out]  Size of the return buffer, and characters returned.
);


// Virus definition information API's.

NAVSTATUS NAVCALLAPI NAVGetVirusDefCount
(
    HNAVENGINE   hNAVEngine,            // [in]  Handle to an open NAV engine.
    DWORD*       lpdwVirusCount         // [out] Number of viruses in definition set.
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
    LPTSTR       lpszResultBuffer,      // [out] Information requested.
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
    DWORD*            lpdwVirusDefCount // [out] Number of virus definitions in this table.
);

DWORD NAVCALLAPI NAVGetVirusDefTableInfo
(
    HNAVVIRUSDEFTABLE hNAVVirusDefTable,// [in]  Handle to an open Virus Definition List.
    DWORD             dwIndex,          // [in]  Index of item to get.
    NAVVIRINFO        enumVirInfo,      // [in]  Item to get info on. (see NAVVIRINFO definition)
    LPTSTR            lpszResultBuffer, // [out] Information requested.
    LPDWORD           lpdwBufferSize    // [in/out]  Size of the return buffer, and characters returned.
);

NAVSTATUS NAVCALLAPI NAVGetNavapiVersionString
(
    LPTSTR lpszVersionBuffer,           // [out] buffer to copy string into
    UINT   uBufferSize                  // [in]  size of lpszVersionBuffer
);

NAVSTATUS NAVCALLAPI NAVGetVirusDBInfo
(
    HNAVENGINE    hNAVEngine,           // [in]  initialized NAV Engine handle.
    LPVIRUSDBINFO lpVirusDBInfo         // [out] database information structure.
);

#ifdef __cplusplus
}
#endif


#endif // if !defined(__NAVAPI_H)
