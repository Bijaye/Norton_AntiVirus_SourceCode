/////////////////////////////////////////////////////////////////////////////
//
// navapinw.cpp
// NAV API Library NLM (NAVAPI.NLM) main source file
// Copyright 1998 by Symantec Corporation.  All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////
//
// $Header:   S:/NAVAPI/VCS/navapinw.cpv   1.10   02 Dec 1998 19:44:36   MKEATIN  $
// $Log:   S:/NAVAPI/VCS/navapinw.cpv  $
// 
//    Rev 1.10   02 Dec 1998 19:44:36   MKEATIN
// reverted back to revision 1.8
// 
//    Rev 1.8   15 Sep 1998 18:32:30   MKEATIN
// Fixed some prototypes.
//
//    Rev 1.7   15 Sep 1998 18:19:32   MKEATIN
// Changed bExecutable to lpstFileExt.
//
//    Rev 1.6   02 Sep 1998 12:59:26   dhertel
// Backed out last change, allowing multiple threads into repair once more.
//
//    Rev 1.5   01 Sep 1998 11:53:40   dhertel
// Allow only one thread into NAVRepairFile() at a time since there are
// unresolved multithread bugs in the repair code (NLM only).
//
//    Rev 1.4   13 Aug 1998 16:59:50   DHERTEL
// Changed the name of NAVLIB.NLM to NAVAPI.NLM, and renamed NAVLIB.* source
// files to NAVAPI (where possible) or NAVAPINW (where necessary)
//
//    Rev 1.3   11 Aug 1998 20:23:58   DHERTEL
// #ifdef'ed out the Netware 3.11 support
//
//    Rev 1.2   06 Aug 1998 21:05:26   DHERTEL
// Added helper NLMs to allow 3.11 support
//
//    Rev 1.1   31 Jul 1998 19:09:32   DHERTEL
// Brought virus info code online for NLM platform.
//
//    Rev 1.0   30 Jul 1998 21:53:16   DHERTEL
// Initial revision.
//
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Headers

#include <signal.h>
#include <process.h>
#include <conio.h>

#include "avapi_l.h"
#include "navcb_l.h"
#include "navapi.h"


/////////////////////////////////////////////////////////////////////////////
// Prototypes
//

int main(int argc, char* argv[]);
void SignalHandler(int nSig);

// These are in nlmstuff.cpp:

int LoadHelperNLM();
void SaveStartPath(char *argv[]);


// This section is cut & pasted from navapi.h, but the names
// have been changed.  Between this and navapi.h, we have
// both the original form of these prototypes and the NLM_
// versions.


HNAVENGINE NAVCALLAPI NLM_NAVEngineInit
(
    LPTSTR       lpszDefsPath,          // [in] path to DEFSS.
    LPTSTR       lpszNavexInfFile,      // [in] path & filename of NAVEX15.INF or NULL
    LPNAVFILEIO  lpFileCallbacks,       // [in] FileCallbacks or NULL
    WORD         wHeuristicLevel,       // [in] 0, 1, 2, or 3 (off, low, med, high)
    WORD         wFlags,                // [in] flags to specify options - or zero
    NAVSTATUS *  lpStatus               // [out] status code
);

NAVSTATUS NAVCALLAPI NLM_NAVEngineClose
(
    HNAVENGINE hNAVEngine               // [in] a valid NAV engine handle
);

// scanning API's

NAVSTATUS NAVCALLAPI NLM_NAVScanFile
(
    HNAVENGINE   hNAVEngine,            // [in] NAV engine handle
    LPVOID       lpFileId,              // [in] pointer to client-defined file ID.
    LPTSTR       lpstFileExt,           // [in] The file's extention (COM, EXE, DOT)
    BOOL         bPreserveLastAccess,   // [in] if TRUE, preserves last access date
    HNAVVIRUS*   lphVirus               // [out] receives handle if infected.
);

NAVSTATUS NAVCALLAPI NLM_NAVScanBoot
(
    HNAVENGINE   hNAVEngine,            // [in] a valid NAV engine handle
    char         cDriveLetter,          // [in] logical drive letter (A, C, etc.)
    HNAVVIRUS*   lphVirus               // [out] receives handle if infected.
);

NAVSTATUS NAVCALLAPI NLM_NAVScanMasterBoot
(
    HNAVENGINE   hNAVEngine,            // [in] a valid NAV engine handle
    UINT         uPhysDriveNum,         // [in] physical drive number (0x80, 0x81, etc.)
    HNAVVIRUS*   lphVirus               // [out] receives handle if infected.
);

NAVSTATUS NAVCALLAPI NLM_NAVScanMemory
(
    HNAVENGINE   hNAVEngine,            // [in] a valid NAV engine handle
    HNAVVIRUS*   lphVirus               // [out] receives handle if infected.
);


// repairing API's

NAVSTATUS NAVCALLAPI NLM_NAVRepairFile
(
    HNAVENGINE   hNAVEngine,            // [in] NAV engine handle
    LPVOID       lpFileId,              // [in] pointer to client-defined file ID.
    LPTSTR       lpstFileExt            // [in] The file's extention (COM, EXE, DOT)
);

NAVSTATUS NAVCALLAPI NLM_NAVDeleteFile
(
    HNAVENGINE   hNAVEngine,            // [in] NAV engine handle
    LPVOID       lpFileId,              // [in] pointer to client-defined file ID.
    LPTSTR       lpstFileExt            // [in] The file's extention (COM, EXE, DOT)
);

NAVSTATUS NAVCALLAPI NLM_NAVRepairBoot
(
    HNAVENGINE   hNAVEngine,            // [in] a valid NAV engine handle
    char         cDriveLetter           // [in] logical drive letter (A, C, etc.)
);

NAVSTATUS NAVCALLAPI NLM_NAVRepairMasterBoot
(
    HNAVENGINE   hNAVEngine,            // [in] a valid NAV engine handle
    UINT         uPhysDriveNum          // [in] physical drive number (0x80, 0x81, etc.)
);


// misc API's

NAVSTATUS NAVCALLAPI NLM_NAVFreeVirusHandle
(
    HNAVVIRUS    hVirus                 // [in] a valid HNAVVIRUS
);


// Virus information API's

DWORD NAVCALLAPI NLM_NAVGetVirusInfo
(
    HNAVVIRUS    hNAVVirus,             // [in]  Handle to an open virus.
    NAVVIRINFO   enumVirInfo,           // [in]  Item to get info on. (see NAVVIRINFO definition)
    LPTSTR       lpszResultBuffer,      // [out] Information requested.
    LPDWORD      lpdwBufferSize         // [in/out]  Size of the return buffer, and characters returned.
);


// Virus definition information API's.

NAVSTATUS NAVCALLAPI NLM_NAVGetVirusDefCount
(
    HNAVENGINE   hNAVEngine,            // [in]  Handle to an open NAV engine.
    DWORD*       lpdwVirusCount         // [out] Number of viruses in definition set.
);


// Individual virus definition information API's.

HNAVVIRUSDEF NAVCALLAPI NLM_NAVLoadVirusDef
(
    HNAVENGINE   hNAVEngine,            // [in] Handle to an open NAV engine.
    DWORD        dwDefIndex             // [in] Index of definition to load from the database.
);

VOID NAVCALLAPI NLM_NAVReleaseVirusDef
(
    HNAVVIRUSDEF hNAVVirusDef           // [in] Handle to an open NAV Virus Definition.
);

DWORD NAVCALLAPI NLM_NAVGetVirusDefInfo
(
    HNAVVIRUSDEF hNAVVirusDef,          // [in]  Handle to an open Virus Definition.
    NAVVIRINFO   enumVirInfo,           // [in]  Item to get info on. (see NAVVIRINFO definition)
    LPTSTR       lpszResultBuffer,      // [out] Information requested.
    LPDWORD      lpdwBufferSize         // [in/out]  Size of the return buffer, and characters returned.
);


// Table type virus definition information API's.

HNAVVIRUSDEFTABLE NAVCALLAPI NLM_NAVLoadVirusDefTable
(
    HNAVENGINE        hNAVEngine        // [in] Handle to an open NAV Engine.
);

VOID NAVCALLAPI NLM_NAVReleaseVirusDefTable
(
    HNAVVIRUSDEFTABLE hNAVVirusDefTable // [in] Handle to an allocated virus definition table.
);

NAVSTATUS NAVCALLAPI NLM_NAVGetVirusDefTableCount
(
    HNAVVIRUSDEFTABLE hNAVVirusDefTable,// [in]  Handle to an open Virus Definition List.
    DWORD*            lpdwVirusDefCount // [out] Number of virus definitions in this table.
);

DWORD NAVCALLAPI NLM_NAVGetVirusDefTableInfo
(
    HNAVVIRUSDEFTABLE hNAVVirusDefTable,// [in]  Handle to an open Virus Definition List.
    DWORD             dwIndex,          // [in]  Index of item to get.
    NAVVIRINFO        enumVirInfo,      // [in]  Item to get info on. (see NAVVIRINFO definition)
    LPTSTR            lpszResultBuffer, // [out] Information requested.
    LPDWORD           lpdwBufferSize    // [in/out]  Size of the return buffer, and characters returned.
);

NAVSTATUS NAVCALLAPI NLM_NAVGetNavapiVersionString
(
    LPTSTR lpszVersionBuffer,           // [out] buffer to copy string into
    UINT   uBufferSize                  // [in]  size of lpszVersionBuffer
);

NAVSTATUS NAVCALLAPI NLM_NAVGetVirusDBInfo
(
    HNAVENGINE    hNAVEngine,           // [in]  initialized NAV Engine handle.
    LPVIRUSDBINFO lpVirusDBInfo         // [out] database information structure.
);


/////////////////////////////////////////////////////////////////////////////
// Global data

int gnThreadCount = 0;
int gnHandleCount = 0;
int gbInitialized = FALSE;


/////////////////////////////////////////////////////////////////////////////
// main()

int main(int argc, char* argv[])
{
    ++gnThreadCount;
    SaveStartPath(argv);
#if defined(SUPPORT311)
    LoadHelperNLM();
#endif
    signal(SIGTERM, SignalHandler);
    gbInitialized = TRUE;
    --gnThreadCount;
    ExitThread(TSR_THREAD, 0);
    return 0;
}


/////////////////////////////////////////////////////////////////////////////
// SignalHandler()

void SignalHandler(int nSig)
{
    switch (nSig)
    {
    case SIGTERM:
        while (gnThreadCount != 0 && gnHandleCount != 0)
            ThreadSwitch();
        break;
    }
}


/////////////////////////////////////////////////////////////////////////////
// NAV API Wrappers
//
// For each NLM_NAV___ function in the NLM version of NAVAPI.LIB,
// we have a NAV___ function that wraps it and keeps track of
// this NLM's active thread count and the number of outstanding handles.

HNAVENGINE NAVCALLAPI NAVEngineInit
(
    LPTSTR       lpszDefsPath,          // [in] path to DEFSS.
    LPTSTR       lpszNavexInfFile,      // [in] path & filename of NAVEX15.INF or NULL
    LPNAVFILEIO  lpFileCallbacks,       // [in] FileCallbacks or NULL
    WORD         wHeuristicLevel,       // [in] 0, 1, 2, or 3 (off, low, med, high)
    WORD         wFlags,                // [in] flags to specify options - or zero
    NAVSTATUS *  lpStatus               // [out] status code
)
{
    HNAVENGINE hEngine;

    ++gnThreadCount;

    while (!gbInitialized)
        ThreadSwitch();
    hEngine = NLM_NAVEngineInit(lpszDefsPath, lpszNavexInfFile, lpFileCallbacks, wHeuristicLevel, wFlags, lpStatus);
    if (hEngine != NULL)
        ++gnHandleCount;
    --gnThreadCount;
    return hEngine;
}


NAVSTATUS NAVCALLAPI NAVEngineClose
(
    HNAVENGINE   hNAVEngine             // [in] a valid NAV engine handle
)
{
    NAVSTATUS status;

    ++gnThreadCount;
    status = NLM_NAVEngineClose(hNAVEngine);
    if (hNAVEngine != NULL)
        --gnHandleCount;
    --gnThreadCount;
    return status;
}


NAVSTATUS NAVCALLAPI NAVScanFile
(
    HNAVENGINE   hNAVEngine,            // [in] NAV engine handle
    LPVOID       lpFileId,              // [in] pointer to client-defined file ID.
    LPTSTR       lpstFileExt,           // [in] The file's extention (COM, EXE, DOT)
    BOOL         bPreserveLastAccess,   // [in] if TRUE, preserves last access date
    HNAVVIRUS*   lphVirus               // [out] receives handle if infected.
)
{
    NAVSTATUS status;

    ++gnThreadCount;
    status = NLM_NAVScanFile(hNAVEngine, lpFileId, lpstFileExt, bPreserveLastAccess, lphVirus);
    if (*lphVirus != NULL)
        ++gnHandleCount;
    --gnThreadCount;
    return status;
}


NAVSTATUS NAVCALLAPI NAVScanBoot
(
    HNAVENGINE   hNAVEngine,            // [in] a valid NAV engine handle
    char         cDriveLetter,          // [in] logical drive letter (A, C, etc.)
    HNAVVIRUS*   lphVirus               // [out] receives handle if infected.
)
{
    return NAV_ERROR;
}


NAVSTATUS NAVCALLAPI NAVScanMasterBoot
(
    HNAVENGINE   hNAVEngine,            // [in] a valid NAV engine handle
    UINT         uPhysDriveNum,         // [in] physical drive number (0x80, 0x81, etc.)
    HNAVVIRUS*   lphVirus               // [out] receives handle if infected.
)
{
    return NAV_ERROR;
}


NAVSTATUS NAVCALLAPI NAVScanMemory
(
    HNAVENGINE   hNAVEngine,            // [in] a valid NAV engine handle
    HNAVVIRUS*   lphVirus               // [out] receives handle if infected.
)
{
    NAVSTATUS status;

    ++gnThreadCount;
    status = NLM_NAVScanMemory(hNAVEngine, lphVirus);
    if (*lphVirus != NULL)
        ++gnHandleCount;
    --gnThreadCount;
    return status;
}


NAVSTATUS NAVCALLAPI NAVRepairFile
(
    HNAVENGINE   hNAVEngine,            // [in] NAV engine handle
    LPVOID       lpFileId,              // [in] pointer to client-defined file ID.
    LPTSTR       lpstFileExt            // [in] The file's extention (COM, EXE, DOT)
)
{
    NAVSTATUS status;

    // NLM_NAVRepairFile() will sometimes fail or crash
    // when entered by multiple threads (using Cascade dumb virus),
    // so gnRepairCount is used to allow only one thread at a time to repair.

    ++gnThreadCount;
    status = NLM_NAVRepairFile(hNAVEngine, lpFileId, lpstFileExt);
    --gnThreadCount;
    return status;
}


NAVSTATUS NAVCALLAPI NAVDeleteFile
(
    HNAVENGINE   hNAVEngine,            // [in] NAV engine handle
    LPVOID       lpFileId,              // [in] pointer to client-defined file ID.
    LPTSTR       lpstFileExt            // [in] The file's extention (COM, EXE, DOT)
)
{
    NAVSTATUS status;

    ++gnThreadCount;
    status = NLM_NAVDeleteFile(hNAVEngine, lpFileId, lpstFileExt);
    --gnThreadCount;
    return status;
}


NAVSTATUS NAVCALLAPI NAVRepairBoot
(
    HNAVENGINE   hNAVEngine,            // [in] a valid NAV engine handle
    char         cDriveLetter           // [in] logical drive letter (A, C, etc.)
)
{
    return NAV_ERROR;
}


NAVSTATUS NAVCALLAPI NAVRepairMasterBoot
(
    HNAVENGINE   hNAVEngine,            // [in] a valid NAV engine handle
    UINT         uPhysDriveNum          // [in] physical drive number (0x80, 0x81, etc.)
)
{
    return NAV_ERROR;
}


NAVSTATUS NAVCALLAPI NAVFreeVirusHandle
(
    HNAVVIRUS    hVirus                 // [in] a valid HNAVVIRUS
)
{
    NAVSTATUS status;

    ++gnThreadCount;
    status = NLM_NAVFreeVirusHandle(hVirus);
    if (hVirus != NULL)
        --gnHandleCount;
    --gnThreadCount;
    return status;
}


DWORD NAVCALLAPI NAVGetVirusInfo
(
    HNAVVIRUS    hNAVVirus,             // [in]  Handle to an open virus.
    NAVVIRINFO   enumVirInfo,           // [in]  Item to get info on. (see NAVVIRINFO definition)
    LPTSTR       lpszResultBuffer,      // [out] Information requested.
    LPDWORD      lpdwBufferSize         // [in/out]  Size of the return buffer, and characters returned.
)
{
    NAVSTATUS status;

    ++gnThreadCount;
    status = NLM_NAVGetVirusInfo(hNAVVirus, enumVirInfo, lpszResultBuffer, lpdwBufferSize);
    --gnThreadCount;
    return status;
}


NAVSTATUS NAVCALLAPI NAVGetVirusDefCount
(
    HNAVENGINE   hNAVEngine,            // [in]  Handle to an open NAV engine.
    DWORD*       lpdwVirusCount         // [out] Number of viruses in definition set.
)
{
    NAVSTATUS status;

    ++gnThreadCount;
    status = NLM_NAVGetVirusDefCount(hNAVEngine, lpdwVirusCount);
    --gnThreadCount;
    return status;
}


HNAVVIRUSDEF NAVCALLAPI NAVLoadVirusDef
(
    HNAVENGINE   hNAVEngine,            // [in] Handle to an open NAV engine.
    DWORD        dwDefIndex             // [in] Index of definition to load from the database.
)
{
    HNAVVIRUSDEF hNAVVirusDef;

    ++gnThreadCount;
    hNAVVirusDef = NLM_NAVLoadVirusDef(hNAVEngine, dwDefIndex);
    if (hNAVVirusDef != NULL)
        ++gnHandleCount;
    --gnThreadCount;
    return hNAVVirusDef;
}


VOID NAVCALLAPI NAVReleaseVirusDef
(
    HNAVVIRUSDEF hNAVVirusDef           // [in] Handle to an open NAV Virus Definition.
)
{
    ++gnThreadCount;
    NLM_NAVReleaseVirusDef(hNAVVirusDef);
    if (hNAVVirusDef != NULL)
        --gnHandleCount;
    --gnThreadCount;
}


DWORD NAVCALLAPI NAVGetVirusDefInfo
(
    HNAVVIRUSDEF hNAVVirusDef,          // [in]  Handle to an open Virus Definition.
    NAVVIRINFO   enumVirInfo,           // [in]  Item to get info on. (see NAVVIRINFO definition)
    LPTSTR       lpszResultBuffer,      // [out] Information requested.
    LPDWORD      lpdwBufferSize         // [in/out]  Size of the return buffer, and characters returned.
)
{
    DWORD dwResult;

    ++gnThreadCount;
    dwResult = NLM_NAVGetVirusDefInfo(hNAVVirusDef, enumVirInfo, lpszResultBuffer, lpdwBufferSize);
    --gnThreadCount;
    return dwResult;
}


HNAVVIRUSDEFTABLE NAVCALLAPI NAVLoadVirusDefTable
(
    HNAVENGINE        hNAVEngine        // [in] Handle to an open NAV Engine.
)
{
    HNAVVIRUSDEFTABLE hNAVVirusDefTable;

    ++gnThreadCount;
    hNAVVirusDefTable = NLM_NAVLoadVirusDefTable(hNAVEngine);
    if (hNAVVirusDefTable != NULL)
        ++gnHandleCount;
    --gnThreadCount;
    return hNAVVirusDefTable;
}


VOID NAVCALLAPI NAVReleaseVirusDefTable
(
    HNAVVIRUSDEFTABLE hNAVVirusDefTable // [in] Handle to an allocated virus definition table.
)
{
    ++gnThreadCount;
    NLM_NAVReleaseVirusDefTable(hNAVVirusDefTable);
    if (hNAVVirusDefTable != NULL)
        --gnHandleCount;
    --gnThreadCount;
}


NAVSTATUS NAVCALLAPI NAVGetVirusDefTableCount
(
    HNAVVIRUSDEFTABLE hNAVVirusDefTable,// [in]  Handle to an open Virus Definition List.
    DWORD*            lpdwVirusDefCount // [out] Number of virus definitions in this table.
)
{
    NAVSTATUS status;

    ++gnThreadCount;
    status = NLM_NAVGetVirusDefTableCount(hNAVVirusDefTable, lpdwVirusDefCount);
    --gnThreadCount;
    return status;
}


DWORD NAVCALLAPI NAVGetVirusDefTableInfo
(
    HNAVVIRUSDEFTABLE hNAVVirusDefTable,// [in]  Handle to an open Virus Definition List.
    DWORD             dwIndex,          // [in]  Index of item to get.
    NAVVIRINFO        enumVirInfo,      // [in]  Item to get info on. (see NAVVIRINFO definition)
    LPTSTR            lpszResultBuffer, // [out] Information requested.
    LPDWORD           lpdwBufferSize    // [in/out]  Size of the return buffer, and characters returned.
)
{
    DWORD dwResult;

    ++gnThreadCount;
    dwResult = NLM_NAVGetVirusDefTableInfo(hNAVVirusDefTable, dwIndex, enumVirInfo, lpszResultBuffer, lpdwBufferSize);
    --gnThreadCount;
    return dwResult;
}


NAVSTATUS NAVCALLAPI NAVGetNavapiVersionString
(
    LPTSTR lpszVersionBuffer,           // [out] buffer to copy string into
    UINT   uBufferSize                  // [in]  size of lpszVersionBuffer
)
{
    NAVSTATUS status;

    ++gnThreadCount;
    status = NLM_NAVGetNavapiVersionString(lpszVersionBuffer, uBufferSize);
    --gnThreadCount;
    return status;
}


NAVSTATUS NAVCALLAPI NAVGetVirusDBInfo
(
    HNAVENGINE    hNAVEngine,           // [in]  initialized NAV Engine handle.
    LPVIRUSDBINFO lpVirusDBInfo         // [out] database information structure.
)
{
    NAVSTATUS status;

    ++gnThreadCount;
    status = NLM_NAVGetVirusDBInfo(hNAVEngine, lpVirusDBInfo);
    --gnThreadCount;
    return status;
}



