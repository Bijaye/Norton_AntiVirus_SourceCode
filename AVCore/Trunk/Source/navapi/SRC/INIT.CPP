// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVAPI/VCS/init.cpv   1.17   20 Aug 1998 16:11:20   MKEATIN  $
//
// Description: Contains NAVAPI's init and close API's
//
//************************************************************************
// $Log:   S:/NAVAPI/VCS/init.cpv  $
//
//    Rev 1.17   20 Aug 1998 16:11:20   MKEATIN
// Checking for additional invalid NULL pointers in NAVEngineInit.
//
//    Rev 1.16   30 Jul 1998 21:52:36   DHERTEL
// Changes for NLM packaging of NAVAPI.
//
//    Rev 1.15   29 Jul 1998 18:34:00   DHERTEL
// DX and NLM changes
//
//    Rev 1.14   29 Jul 1998 16:44:28   MKEATIN
// Added lpStatus parameter to NAVEngineInit so apps can evaluate a NAVSTATUS
// code on init failure.
//
//    Rev 1.13   28 Jul 1998 13:37:28   MKEATIN
// AP enabling/disabling now only happens on WIN32.
//
//    Rev 1.12   22 Jun 1998 22:28:34   DALLEE
// Added calls to enable/disable NAV auto-protect around scanning.
//
//    Rev 1.11   22 Jun 1998 16:59:32   DALLEE
// Added required callbacks for Get/SetDateTime().
//
//    Rev 1.10   19 Jun 1998 17:29:24   DALLEE
// Removed user data parameter - unused and couldn't be referenced
// by client.
//
//    Rev 1.9   19 Jun 1998 17:03:26   MKEATIN
// Don't compile VxDServicesInit on Alpha.
//
//    Rev 1.8   17 Jun 1998 17:46:22   MKEATIN
// Now, returning NAV_xxx error codes and check for valid arguments -
// basically valid hEngine's that is.
//
//    Rev 1.7   11 Jun 1998 17:33:00   DALLEE
// Add specific file callbacks structure.
// Made file callbacks parameter to NAVEngineInit() required.
//
//    Rev 1.6   02 Jun 1998 15:01:52   MKEATIN
// Don't allow NAVEngInit to be called more than once until
// NAVEngineClose is called.
//
//    Rev 1.5   28 May 1998 17:01:14   MKEATIN
// Fixed comments
//
//    Rev 1.4   27 May 1998 19:39:32   MKEATIN
// Changed code to match our new naming conventions
//
//    Rev 1.3   27 May 1998 17:40:50   MKEATIN
// We now call NAVSetStaticData instead of NAVSetDefsPath
//
//    Rev 1.2   27 May 1998 16:50:44   MKEATIN
// Added NAVFreeVirusHandle
//
//    Rev 1.1   27 May 1998 16:29:04   MKEATIN
// Init the VxD only on Win 95
//
//    Rev 1.0   26 May 1998 19:12:26   MKEATIN
// Initial revision.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "avapi_l.h"
#include "nlm_nav.h"
#include "navapi.h"
#include "virtcb.h"

#ifdef SYM_WIN32
  #include "apenable.h"
#endif

// AllocCSSelector is in DOSTOOL.LIB but is not prototyped
// because it is usually not called from external code.
#if defined(SYM_DOSX)
extern "C" WINAPI AllocCSSelector();
#endif

// NAVEngineInit should be called only once for all threads until
// NAVEngineClose is called. The variable below is used to enforce
// this behavior

static BOOL s_bIsInitialized = FALSE;

//************************************************************************
// HNAVENGINE NAVCALLAPI NAVEngineInit
// (
//     LPTSTR          lpszDefsPath,   // [in] path to DEFS.
//     LPTSTR          lpszNavexInfFile,// [in] path & filename of NAVEX15.INF or NULL
//     LPVOID          lpFileCallbacks,// [in] FileCallbacks or NULL
//     WORD            wHeuristicLevel,// [in] 0, 1, 2, or 3 (off, low, med, high)
//     WORD            wFlags          // [in] flags to specify options - or zero
//     NAVSTATUS *     lpStatus        // [out] status code
// )
//
// The routine opens a new engine handle.  Only one engine handle
// should be opened for all threads in a given application.  Use
// NAVEngineClose to close this engine handle.
//
// Returns:
//      A valid HNAVENGINE on success, NULL otherwise
//************************************************************************
// 05/26/98 Created by MKEATIN
//************************************************************************

HNAVENGINE NAVCALLAPI NAVEngineInit
(
    LPCTSTR         lpszDefsPath,   // [in] path to DEFS.
    LPCTSTR         lpszNavexInfFile,// [in] path & filename of NAVEX15.INF or NULL
    LPNAVFILEIO     lpFileCallbacks,// [in] FileCallbacks or NULL
    WORD            wHeuristicLevel,// [in] 1,2, or 3
    WORD            wFlags,         // [in] flags to specify options - or zero
    LPNAVSTATUS     lpStatus        // [out] status code
)
{
    extern AVGLOBALCALLBACKS gstNAVCallBacks;
    HVCONTEXT hContext = NULL;
    TCHAR     szVirscanDatPath[SYM_MAX_PATH+1];
    TCHAR     szVirscanInfPath[SYM_MAX_PATH+1];
    VSTATUS   nStatus;

    // don't allow more than one call to this function until NAVEngineClose is called

    //&? Should really synchronize access to s_bIsInitialized here. -DALLEE 06/11/1998

    if ( s_bIsInitialized )
    {
        *lpStatus = NAV_ENGINE_IN_USE;
        return NULL;
    }

    s_bIsInitialized = TRUE;

    // Validate the input arguments. NULL is allowed for lpFileCallbacks.
    // However, NULL is not allowed in the members of the lpFileCallback
    // structure (when it is specified).
    if ((lpFileCallbacks                &&
         (!lpFileCallbacks->Open        ||
          !lpFileCallbacks->Close       ||
          !lpFileCallbacks->Seek        ||
          !lpFileCallbacks->Read        ||
          !lpFileCallbacks->Write       ||
          !lpFileCallbacks->Delete      ||
          !lpFileCallbacks->GetDateTime ||
          !lpFileCallbacks->SetDateTime)) ||
         !lpszDefsPath                    ||
         !lpStatus )
    {
        s_bIsInitialized = FALSE;
        *lpStatus = NAV_INVALID_ARG;
        return  NULL;
    }

    // Do DOS Extender initializations for Symantec Core libraries.
    // This is normally done by our applications' custom CRT0.asm file,
    // which a third party application won't have.

#if defined(SYM_DOSX)
    AllocCSSelector();
#endif

    // Load NAVAPI.VXD

#if defined(SYM_WIN32) && !defined(_ALPHA_)
    if ((SystemGetWindowsType() == SYM_SYSTEM_WIN_95) &&
        (!VxDServicesInit()))
    {
        *lpStatus = NAV_VXD_INIT_FAILURE;
        s_bIsInitialized = FALSE;
        return NULL;
    }
#endif

    hContext = VLScanInit(1, &gstNAVCallBacks, wFlags);

    if (!hContext)
    {
        s_bIsInitialized = FALSE;
        *lpStatus = NAV_MEMORY_ERROR;
        return NULL;
    }

    // Note that NULL is allowed for lpFileCallbacks.
    SetStaticCallbackData(lpszDefsPath,
                          wHeuristicLevel,
                          lpFileCallbacks);

    // Setup szVirscanDatPath and szVirscanInfPath for VLRegisterVirusDB in AVAPI1

    STRCPY(szVirscanDatPath, lpszDefsPath);
    STRCPY(szVirscanInfPath, lpszDefsPath);
#if defined(SYM_UNIX)
    NameAppendFile(szVirscanDatPath, _T("virscan1.dat"));
    NameAppendFile(szVirscanInfPath, _T("virscan.inf"));
#else
    NameAppendFile(szVirscanDatPath, _T("VIRSCAN1.DAT"));
    NameAppendFile(szVirscanInfPath, _T("VIRSCAN.INF"));
#endif

    nStatus = VLRegisterVirusDB(hContext,
                                szVirscanDatPath,
                                szVirscanInfPath,
                                (LPTSTR)lpszNavexInfFile);

    if (nStatus != VS_OK)
    {
        VLScanClose(hContext);
        s_bIsInitialized = FALSE;

        switch (nStatus)
        {
            case VS_INVALID_ARG:

                *lpStatus = NAV_INVALID_ARG;
                break;

            case VS_MEMALLOC:

                *lpStatus = NAV_MEMORY_ERROR;
                break;

            case VS_FILE_ERROR:

                *lpStatus = NAV_DEFINITIONS_ERROR;
                break;

            default:

                *lpStatus = NAV_ERROR;
                break;
        }

        return NULL;
    }

#ifdef SYM_WIN32
    NavapCommInit();
    NavapUnprotectProcess();
#endif

    *lpStatus = NAV_OK;
    return (HNAVENGINE)hContext;
}


//************************************************************************
// NAVSTATUS NAVCALLAPI NAVEngineClose
// (
//     HNAVENGINE hNAVEngine         // [in] a valid NAV engine handle
// )
//
// This routine closes an engine handle opened with NAVEngineInit().
// Client apps need to ensure that all threads are finished using this
// engine handle before closing it.
//
// Returns:
//      NAVSTATUS error code
//************************************************************************
// 05/26/98 Created by MKEATIN
//************************************************************************

NAVSTATUS NAVCALLAPI NAVEngineClose
(
    HNAVENGINE hNAVEngine         // [in] a valid NAV engine handle
)
{
    VSTATUS nStatus = VLScanClose((HVCONTEXT)hNAVEngine);

    switch (nStatus)
    {
        case VS_INVALID_ARG:

            return NAV_INVALID_ARG;

        case VS_OK:

            s_bIsInitialized = FALSE;

#ifdef SYM_WIN32
            NavapProtectProcess();
            NavapCommDeinit();
#endif

            return NAV_OK;

        default:

            return NAV_ERROR;
    }
}

// I'm adding NAVFreeVirusHandle here to avoid creating a whold new
// source file for just one function.  If more misc functions arise we
// should create a misc.cpp or something similar.

//************************************************************************
// NAVSTATUS NAVCALLAPI NAVFreeVirusHandle
// (
//     HNAVVIRUS hVirus                    // [in] a valid virus handle
// )
//
// This routine frees a virus handle allocated from one of the scanning
// API's.  It should be called when an application no longer needs the
// virus handle.
//
// Returns:
//      NAVSTATUS error code
//************************************************************************
// 05/26/98 Created by MKEATIN
//************************************************************************

NAVSTATUS NAVCALLAPI NAVFreeVirusHandle
(
    HNAVVIRUS hVirus                    // [in] a valid virus handle
)
{
    VSTATUS nStatus = VLReleaseVirusHandle(hVirus);

    switch (nStatus)
    {
        case VS_INVALID_ARG:

            return NAV_INVALID_ARG;

        case VS_OK:

            return NAV_OK;

        default:

            return NAV_ERROR;
    }
}

