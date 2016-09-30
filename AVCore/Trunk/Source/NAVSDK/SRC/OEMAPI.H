// Copyright 1995, 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// Description:
//
//      This file defines structures and functions used internally
//      within NAVOEM. Not for public consumption.
//      
//************************************************************************
// $Log:   S:/NAVSDK/VCS/oemapi.h_v  $
// 
//    Rev 1.4   19 Mar 1998 11:45:20   DALLEE
// Added SynchronizeScanContext().
// Helper func for copying scan context to per thread data.
// Allows sharing of scan handles among multiple threads.
// 
//    Rev 1.3   18 Dec 1997 13:24:46   DALLEE
// Added memory progress callback.
// 
//    Rev 1.2   18 Dec 1997 02:08:38   DALLEE
// Klugery to combat buggy NAVENG. If the config callback is not implemented
// and there is no .INF engine config file, then heuristic scanning is
// disabled (the engine gets an empty extension list).
// Added engine config NAVSDK.INF (szNavexInf) path to per thread data.
// In callback, if this doesn't exist, manually set scan level to 3.
// 
//    Rev 1.1   04 Dec 1997 16:23:52   DALLEE
// Added routines to store OEM context structure per scan instance in
// thread local storage.
// Added definition, AP enable/disable, and manage virus defs information
// to per thread OEM context structure.
// 
//    Rev 1.0   02 Dec 1997 14:24:04   DALLEE
// Initial revision.
//************************************************************************

#ifndef __OEMAPI_H
#define __OEMAPI_H


//***************************************************************************
// Defines
//***************************************************************************

#define OEM_SIGNATURE   0xDEAF00ED

//***************************************************************************
// Structures
//***************************************************************************

typedef struct
{
    DWORD           dwOEMSignature;
    HVCONTEXT       hContext;
    VSTATUS         vStatus;
    VIRLIBFUNCFLAGS FuncEnable;
    VIRUSINFO       VirInfo;
    WORD            wHeuristicScanningLevel;
    BOOL            bDisableNAVAPBySession;
    BOOL            bManageVirusDefs;
    PROGRESSFUNC    lpfMemProgress;
    LPVOID          lpvMemUserData;
    DWORD           dwMemProgressMax;
    BOOL            bScanningMem;
    TCHAR           szAppID [ VIRLIB_MAX_APP_ID_SIZE + 1 ];
    TCHAR           szVirusDefDir [ VIRLIB_MAX_PATH_SIZE + 1 ];
    TCHAR           szNavexInf [ VIRLIB_MAX_PATH_SIZE + 1 ];

}   OEMVIRUSCONTEXT, FAR *LPOEMCONTEXT;


//***************************************************************************
// Function Prototypes
//***************************************************************************

VOID WINAPI AutoProtectOn ( VOID );

VOID WINAPI AutoProtectOff ( VOID );

BOOL WINAPI CheckForNewVxDMethod (
    LPHINSTANCE       lphInst );

BOOL WINAPI ValidOEMContext (
    HVIRUSSCANNER       hScannerHandle,
    LPOEMCONTEXT FAR *  lpOEMContext );

BOOL WINAPI AllocScanContextStorage ( VOID );
BOOL WINAPI FreeScanContextStorage ( VOID );
LPOEMCONTEXT WINAPI GetScanContext ( VOID );
BOOL WINAPI SetScanContext ( LPOEMCONTEXT );

BOOL WINAPI SynchronizeScanContext ( LPOEMCONTEXT );

#endif // __OEMAPI_H

