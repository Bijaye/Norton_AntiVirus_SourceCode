// Copyright 1992-1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/VIRLIST/VCS/V_VIRLIB.CPv   1.14   12 Sep 1997 10:08:46   jmillar  $ 
//
// Description:                                                   
//      
//
// Contains (as exports):
//
//************************************************************************
// $Log:   S:/VIRLIST/VCS/V_VIRLIB.CPv  $
// 
//    Rev 1.14   12 Sep 1997 10:08:46   jmillar
// port Roswell change from QAKG
// 
//    Rev 1.13   18 Jun 1997 20:21:44   JBRENNA
// Added call to NAVSetDefsPath. This sets up the definition path for our NAV
// engine callbacks.
// 
//    Rev 1.12   17 Jun 1997 19:06:30   MKEATIN
// VLScanInit() now takes a &gstNAVCallBacks as it's second parameter.
// 
//    Rev 1.11   20 May 1997 18:57:24   MKEATIN
// We're now linking to an avenged api.
// 
//    Rev 1.10   01 Mar 1996 12:01:22   JWORDEN
// No change.
// 
//    Rev 1.9   20 Feb 1996 15:42:00   JWORDEN
// Make Init static in InitVirusLibrary (). This will avoid the invalid
// function pointer caused by attempting to reference a structure that
// was defined on the stack.
// 
//    Rev 1.8   05 Feb 1996 12:29:00   GDZIECI
// Fixed MySDiskReadPhysical() and MySDiskWritePhysical() prototypes.
// 
//    Rev 1.7   16 Jan 1996 17:04:36   JWORDEN
// Port Quake 6 to trunk
// 
//    Rev 1.2.1.3   13 Mar 1995 08:59:44   KEITH
// Added a header. Actually, just testing the checkin process.
// 

#include "platform.h"
#include <stdlib.h>
#include "xapi.h"
#include "file.h"
#include "disk.h"
#include "virusdbf.h"
#include "navcb.h"


HVCONTEXT SYM_EXPORT WINAPI InitVirusLibrary ( // Initialize virus library
    LPSTR   lpDatFile,                  // [in] path to DAT file
    LPSTR   lpInfFile )                 // [in] path to INF file
{
    auto    BOOL            bSuccess = FALSE;

    HVCONTEXT hContext = VLScanInit(1, &gstNAVCallBacks, 0);

    if (hContext)
    {
        char szDir[SYM_MAX_PATH];
        
        STRCPY (szDir, lpDatFile);
        NameStripFile (szDir);
        NAVSetDefsPath (szDir);

        if (VLRegisterVirusDB(hContext, lpDatFile, lpInfFile, NULL) != VS_OK)
        {
            VLScanClose (hContext);
            hContext = NULL;
        }
    }

    return (hContext);
}

// --------------------------------------------------------------------------
// InitVirusList() 
// Initialize virus list with no intent to scan.
// --------------------------------------------------------------------------

HVCONTEXT SYM_EXPORT WINAPI InitVirusList ( // Initialize virus list
    LPSTR   lpDatFile,                  // [in] path to DAT file
    LPSTR   lpInfFile )                 // [in] path to INF file
{
    auto    BOOL            bSuccess = FALSE;

    HVCONTEXT hContext = VLScanInit(1, &gstNAVCallBacks, 0);

    if (hContext)
    {                                   // This is all we need from the
                                        // VLRegisterVirusDB() when we won't
                                        // do any scanning.
        STRCPY(hContext->szVirScanDataFile, lpDatFile);
        STRCPY(hContext->szVirScanInfoFile, lpInfFile);
    }

    return (hContext);
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

VOID SYM_EXPORT WINAPI UnloadVirusLibrary ( // Unload virus library
    HVCONTEXT hContext )                    // [in] our nav pointer
{
    if (hContext)
        VLScanClose(hContext);
}


