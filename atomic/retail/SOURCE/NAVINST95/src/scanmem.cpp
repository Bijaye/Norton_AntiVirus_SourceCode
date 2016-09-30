/////////////////////////////////////////////////////////////////////////////
//  Copyright (c)1997 - Symantec, Peter Norton Computing Group
/////////////////////////////////////////////////////////////////////////////
//
//  $Header:   S:/NAVINST95/VCS/scanmem.cpv   1.6   14 Aug 1997 12:20:10   jtaylor  $
//
//  NavInst.CPP -   Module that holds mostly install functions for the
//                  InstallShield installation of NAVNT; this is the main
//                  point of entry for the custom DLL.
//
/////////////////////////////////////////////////////////////////////////////
//  $Log:   S:/NAVINST95/VCS/scanmem.cpv  $
// 
//    Rev 1.6   14 Aug 1997 12:20:10   jtaylor
// -- Added one more parameter to the VLRegisterVirusDB.  Added a "" passed
//    as the location of the navex15.inf file.
//
//    Rev 1.5   02 Jul 1997 15:17:06   jtaylor
// -- Changed the size arguments to LoadString calls to the #characters in string.
//
//    Rev 1.4   20 Jun 1997 09:16:12   JBRENNA
// Pass gstNAVCallBacks into VLScanInit. The callbacks now reside in
// N32CALL.DLL. Previously, they were in AVAPI1 directly.
//
//    Rev 1.3   18 Jun 1997 22:00:56   JBRENNA
// Add call to NAVSetDefsPath() before calling VLRegisterVirusDB. This tells
// our AVAPI1 callback functions the definition file location.
//
//    Rev 1.2   08 Jun 1997 01:06:42   jtaylor
// Made ScanMemory return True for no problems or false for unable to scan.
//
//    Rev 1.1   01 Jun 1997 19:58:34   jtaylor
// Added G_hinst declaration.
//
//    Rev 1.0   01 Jun 1997 19:41:30   jtaylor
// Initial revision.
//
//
/////////////////////////////////////////////////////////////////////////////


#ifndef SYM_WIN32
#define SYM_WIN32
#endif

#include <windows.h>
#include <windowsx.h>

#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <winuser.h>

#include "platform.h"
#include "resource.h"
#include "svcctrl.h"
#include "cwstring.h"
#include "navstart.h"
#include "ScanMem.h"

#include "navapcom.h"

#include "avapi.h"
#include "virscan.h"
#include "xapi.h"
#include "disk.h"
#include "navcb.h"

HVCONTEXT    hContext;
char    SZ_STAR_STAR[]  = "*.*";

#ifdef SYM_WIN
    HINSTANCE hInstance;
#endif  // #ifdef SYM_WIN

/////////////////////////////////////////////////////////////////////////////
//  Global Variables
/////////////////////////////////////////////////////////////////////////////

#ifdef  __cplusplus
extern "C"  {
#endif

HINSTANCE       g_hInst;

#ifdef  __cplusplus
    }
#endif

// ==== DllMain ===========================================================
//
//
// ========================================================================
// ========================================================================

BOOL    WINAPI  DllMain(HANDLE  hInst, ULONG  ul_reason_for_call
                                                        , LPVOID  lpReserved)
{
    switch (ul_reason_for_call)
        {
        case  DLL_PROCESS_ATTACH:

            g_hInst = hInst;

            break;

        case  DLL_THREAD_ATTACH:
            break;

        case  DLL_THREAD_DETACH:
            break;

        case  DLL_PROCESS_DETACH:
            break;

        default:
            break;
        }

    return  (TRUE);

}  // end of "DllMain"


// ========================================================================
//  Scan Memory
//
//  This function scans memory for viruses.  If a virus is found then this
//  function WILL shut down the computer!
//
//  This code was adapted from virscan.cpp.
//
//  Input:  strDatFile -- The location of the primary virus definition file
//          strInfFile -- The location of the virus informtion file.
//
//  Output: FALSE  -- There was a problem.
//          TRUE   -- The init/scan/end suceeded and detected no viruses.
//
// ========================================================================
//  Function created: 4/97, JTAYLOR
// ========================================================================
extern "C" DllExport int ScanMemory( char  *strDatFile, char *strInfFile )
{
                                        // This initializes the scan engine!
    UINT uSize = SYM_MAX_PATH;
    char szDefsDir[SYM_MAX_PATH];

    hContext = VLScanInit(1, &gstNAVCallBacks, 0);

                                        // Make sure we have a valid context
    if( hContext == NULL )
        return FALSE;

                                        // Tell our AVAPI1 callbacks the
                                        // location of our definition files.
    STRCPY (szDefsDir, strDatFile);
    NameStripFile (szDefsDir);
    NAVSetDefsPath (szDefsDir);

                                        // Register the Virus Database
    if (VLRegisterVirusDB (hContext,
                           strDatFile,
                           strInfFile,
                           "" ) != VS_OK )
    {
        return FALSE;
    }

                                        // This is the code that scans the mem
    WORD wFlags = VL_SCAN640;

                                        // Convert bScanHighMemory into
                                        // the flags for VLScanMemory
    HVLVIRUS hVirus;

    VSTATUS result = VLScanMemory( hContext, wFlags, &hVirus );

                                        // Check to see if a virus was found
    if( result == VS_OK && hVirus != NULL )
        {
        TCHAR  szTitle[SI_MAXSTR];
        TCHAR  szCaption[SI_MAXSTR];

        LoadString(g_hInst, IDS_NAV_MEMORY_INFECTED, szCaption, SI_MAXSTR);
        LoadString(g_hInst, IDS_NAV_MEMORY_INFECTED_TITLE, szTitle, SI_MAXSTR);

        MessageBox( NULL, szCaption, szTitle, MB_OK );

        VLReleaseVirusHandle( hVirus );
                                        // A virus was found....halt the system.
                                        // TODO: Halt the system.
        ExitWindowsEx (EWX_FORCE | EWX_SHUTDOWN, 0);
        return FALSE;
        }

                                        // This cleans up the scan engine
    VLScanClose(hContext);

    if( result != VS_OK )
        return FALSE;

    return TRUE;
} // end of "ScanMemory"