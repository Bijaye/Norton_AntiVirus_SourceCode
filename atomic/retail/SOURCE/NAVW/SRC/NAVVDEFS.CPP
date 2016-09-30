// Copyright 1997 Symantec Corporation
//************************************************************************
// $Header:   S:/NAVW/VCS/navvdefs.cpv   1.3   13 Aug 1997 14:48:04   MKEATIN  $
//
// Description:
//   This source file contains the Virus Definition Init and DeInit
//   functionality. These routines encapsulate the CDefUtils management
//   for the NAVW AppId.
//
//************************************************************************
// $Log:   S:/NAVW/VCS/navvdefs.cpv  $
// 
//    Rev 1.3   13 Aug 1997 14:48:04   MKEATIN
// VirusScanInit() now takes an extra parameter - the full path to NAVEX15.INF.
// 
//    Rev 1.2   18 Jun 1997 15:58:12   MKEATIN
// Call InitCallBacks() before calling VirusScanInit().
// 
//    Rev 1.1   17 Jun 1997 15:14:34   MKEATIN
// VirusScanInit() now has an LPAVGLOBALCALLBACKS parameter.
// 
//    Rev 1.0   04 Jun 1997 15:17:00   JBRENNA
// Initial revision.
// 
//************************************************************************

// Ensure that we define and initialize the strings defined in NavAppIDs.h
#define NAVAPPIDS_BUILDING

#include "platform.h"
#include "xapi.h"
#include "DefUtils.h"
#include "VirScan.h"
#include "NavVDefs.h"
#include "NavAppIDs.h"
#include "navutil.h"

//************************************************************************
// Static variables ******************************************************
//************************************************************************

static CDefUtils s_DefUtils;
static BOOL      s_bInitialized = FALSE;

//************************************************************************
// Static functions ******************************************************
//************************************************************************

//////////////////////////////////////////////////////////////////////////
// Description:
//   Initialize s_DefUtils with the NAVW Application Id. This only needs
//   to happen once.
//
// Returns:
//   NOERR      success.
//   ERR        failure.
//////////////////////////////////////////////////////////////////////////
static UINT DefUtilsInit ()
{
    auto   LPCSTR  szAppId;
    static BOOL bInited = FALSE;

    if (!bInited)
    {
        bInited = TRUE;

        // Determine the application ID for CDefUtils.
        if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
            szAppId = g_szNavAppIdNavwNt;
        else
            szAppId = g_szNavAppIdNavw95;

        // Set our AppId
        if (!s_DefUtils.InitWindowsApp (szAppId))
            return ERR;
    }

    return NOERR;
}

//************************************************************************
// Extern functions ******************************************************
//************************************************************************

//////////////////////////////////////////////////////////////////////////
// Description:
//   Initialize VirScan and our local CDefUtils class.
//
// Returns:
//   NOERR      success.
//   ERR        failure.
//////////////////////////////////////////////////////////////////////////
UINT NavVDefsVirusScanInit ()
{
    auto   BOOL    bDefDirChanged;
    auto   char    szDefDir[_MAX_PATH+1];
    auto   char    szNavexInfFile[_MAX_PATH+1];

    extern HINSTANCE ghInst;

    // Ensure that multiple calls to Init without a DeInit go through
    // the DeInit code. This ensures that we do not have any of the
    // VirScan[1234].DAT files open when we call CDefUtils::UseNewestDefs().
    if (s_bInitialized)
    {
        if (!NavVDefsVirusScanDeInit())
            return ERR;
    }
    s_bInitialized = TRUE;

    // Initialize the AppID into s_DefUtils.
    if (DefUtilsInit() == ERR)
        return ERR;

    // Start using the newest definitions on the machine.
    if (!s_DefUtils.UseNewestDefs (szDefDir, sizeof (szDefDir), &bDefDirChanged))
        return ERR;

    // Tell VirScan the definitions directory to use.
    VirusScanSetPath (szDefDir);

    // Override application specific callbacks
    InitCallBacks (&gstNAVCallBacks, 0);

    // Get the full path for NAVEX15.INF
    GetNavexInfFile(ghInst, szNavexInfFile, sizeof(szNavexInfFile));

    return VirusScanInit (&gstNAVCallBacks, szNavexInfFile);
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   DeInitialize VirScan and our local CDefUtils class.
//
// Returns:
//   TRUE   success.
//   FALSE  failure.
//////////////////////////////////////////////////////////////////////////
BOOL NavVDefsVirusScanDeInit ()
{
    // If the Init function was not called, there is nothing to do
    // in the DeInit function.
    if (!s_bInitialized)
        return TRUE;

    s_bInitialized = FALSE;

    return VirusScanEnd ();
}
