//************************************************************************
//
// $Header:   S:/NAVEX/VCS/navexent.cpv   1.21   09 Dec 1998 17:45:42   DCHI  $
//
// Description:
//      Contains NAVEX EXTStartUp and EXTShutDown functions
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/navexent.cpv  $
// 
//    Rev 1.21   09 Dec 1998 17:45:42   DCHI
// Added #include "gdefines.h"
// 
//    Rev 1.20   08 Dec 1998 13:10:34   DCHI
// Changes for MSX.
// 
//    Rev 1.19   19 Nov 1998 03:35:56   ECHIEN
// Carey changes
// 
//    Rev 1.18   12 Oct 1998 17:36:12   relniti
// define SYM_WIN
// 
//    Rev 1.17   12 Oct 1998 10:59:28   CNACHEN
// Added support for AOL pws heuristics.
// 
// 
//    Rev 1.16   09 Sep 1998 12:45:30   DCHI
// Changes for password-protected Excel 97 document repair.
// 
//    Rev 1.15   02 Jul 1998 18:20:24   DCHI
// Implemented NOMACRO env var checking for building without macro engine.
// 
//    Rev 1.14   14 May 1998 12:32:36   CNACHEN
// Trojan scanner no longer checks for error on load so we work off of
// resuce disks.
// 
// 
//    Rev 1.13   13 May 1998 12:59:04   CNACHEN
// changed to use new trojan+data file scanner
// 
//    Rev 1.12   05 Nov 1997 12:02:16   DCHI
// Moved InitMacroHeurEnableState() to mcrheuen.cpp.
// 
//    Rev 1.11   29 Oct 1997 09:18:46   DCHI
// Added NAVNLM section check for disabling macro heuristics.
// 
//    Rev 1.10   25 Sep 1997 19:08:50   DDREW
// Removed call to LibeExitFunction (SYM_NLM only)
// 
//    Rev 1.9   11 Aug 1997 17:38:14   DCHI
// Commented out unused variables.
// 
//    Rev 1.8   11 Aug 1997 17:35:48   DCHI
// Disablement of macroheuristics only through NAVEXINI file now.
// 
//    Rev 1.7   07 Aug 1997 19:16:22   DCHI
// Removed int 3 and put _T() around "heur".
// 
//    Rev 1.6   07 Aug 1997 18:37:26   DCHI
// Added initialization of macro heuristics enablement.
// 
//    Rev 1.5   07 Aug 1997 09:43:46   DDREW
// A few minor NLM fixes
// 
//    Rev 1.4   09 Jul 1997 17:01:02   DCHI
// Added support for datafile loading.
// 
//    Rev 1.3   30 Jun 1997 12:16:46   CNACHEN
// Added usage count to NAVEX 1.5 startup/shutdown
// 
//    Rev 1.2   04 Jun 1997 12:10:24   CNACHEN
// Added support for engine15 invocation
// 
//    Rev 1.1   08 May 1997 13:00:18   MKEATIN
// Use n30type.h instead of ctsn.h
// 
//    Rev 1.0   02 May 1997 13:28:54   CNACHEN
// Initial revision.
// 
//************************************************************************

#include "gdefines.h"

#include "platform.h"
#include "n30type.h"
#include "callbk15.h"
#include "navex15.h"
#include "ldmcrsig.h"
#include "nvx15inf.h"

#include "trojscan.h"
#include "winconst.h"
#include "aolpw.h"

#include "inifile.h"
#include "datadir.h"

#ifdef MSX
#include "msx.h"
#endif // #ifdef MSX

#ifdef SYM_WIN32
#define APP_STRING "NAVW32"
#elif defined(SYM_DOSX)
#define APP_STRING "NAVDX"
#elif defined(SYM_NLM)
#define APP_STRING "NAVNLM"
#elif defined(SYM_VXD)
#define APP_STRING "NAVVXD"
#elif defined(SYM_NTK)
#define APP_STRING "NAVNTK"
#elif defined(SYM_OS2)
#define APP_STRING "NAVOS2"
#elif defined(SYM_WIN)
#define APP_STRING "NAVWIN"
#elif defined(SYM_UNIX)
#define APP_STRING "NAVUNIX"
#endif


/////////////////////////////////////////////////////////////////////////////
// global data for NAVEX15 starts
/////////////////////////////////////////////////////////////////////////////

#ifdef SYM_NLM
VOID  LibraryExitFunction(VOID);
#endif

PEXPORT15_TABLE_TYPE        glpstEngineExportTable;
int                         gnUsageCount = 0;


/////////////////////////////////////////////////////////////////////////////
// global data for NAVEX15 ends
/////////////////////////////////////////////////////////////////////////////


int GetNAVEX15INFInt
(
    LPCALLBACKREV2                          lpstCallBacks,
    LPTSTR                                  lpszNAVEXINIFile,
    LPSTR                                   lpszSection,
    LPSTR                                   lpszKey,
    int                                     nDefault
)
{
    HFILE                                   hFile;
    int                                     nValue;

    hFile = lpstCallBacks->FileOpen(lpszNAVEXINIFile,0);
    if (hFile == (HFILE)-1)
    {
        return(nDefault);
    }

    nValue = nDefault;

    GetProfileInt(lpstCallBacks,
                  hFile,
                  lpszSection,
                  lpszKey,
                  nDefault,
                  &nValue);

    lpstCallBacks->FileClose(hFile);

    return(nValue);
}



// EXTStartUp
//  lpstCallBacks       Callbacks provided by cleint
//  wClientVersion      Version number of client for compatability checking
//  lpstChain           Pointer to the ENGINE1.5 export table, to be passed
//                      in to the NAVEX1.5 EXTStartUp function.
//  lpszNAVEXDataDir    Path, including trailing backslash to data file dir.
//  lpszNAVEXINIFile    Full path and filename to NAVEX1.5 .ini file.
//  lpwServerVersion    Pointer to a WORD to contain the NAVEX/ENGINE1.5 version #
//
// Description:
//  This should only be called for the NAVEX1.5 module; it will automatically
//  call the ENGINE1.5 EXTStartUp if lpstChain points to its export table.  This
//  function loads all data files and sets up all global data for NAVEX scanning.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_MEM_ERROR           if memory error
//                        EXTSTATUS_FILE_ERROR          if file error
//                        EXTSTATUS_INIT_ERROR          general init. error
//

EXTSTATUS FAR WINAPI NLOADDS EXTStartUp
(
    LPCALLBACKREV2                          lpstCallBacks,
    WORD                                    wClientVersion,
    struct tag_EXPORT15_TABLE FAR *         lpstChain,
    LPTSTR                                  lpszNAVEXDataDir,
    LPTSTR                                  lpszNAVEXINIFile,
    LPWORD                                  lpwServerVersion
)
{
    EXTSTATUS                               status;
    WORD                                    wServer;
    BOOL                                    bNAVENGError = FALSE;
    TCHAR                                   szInfFileName[2 * SYM_MAX_PATH];

    if (gnUsageCount > 0)
    {
        gnUsageCount++;
        return(EXTSTATUS_OK);
    }

    gnUsageCount++;

    // Form the pathname for NAVEX15.INF if the given one
    //  is null or empty

    if (lpszNAVEXINIFile == NULL || lpszNAVEXINIFile[0] == 0)
    {
        DataDirAppendName(lpszNAVEXDataDir,
#if defined(SYM_UNIX)
                          _T("navex15.inf"),
#else
                          _T("NAVEX15.INF"),
#endif
                          szInfFileName);

        lpszNAVEXINIFile = szInfFileName;
    }

    glpstEngineExportTable = lpstChain;

    if (lpstChain != NULL)
    {
        status = lpstChain->EXTStartUp(lpstCallBacks,
                                       0x00000001,     // NAVEX15 client, ver 1
                                       NULL,
                                       lpszNAVEXDataDir,
                                       lpszNAVEXINIFile,
                                       &wServer);

        if (status != EXTSTATUS_OK)
            bNAVENGError = TRUE;
    }
    else
        status = EXTSTATUS_OK;

#ifndef NOMACRO
    // Check the INF file

    NAVEX15InfCheck(lpstCallBacks,lpszNAVEXINIFile);

    // Load macro scanning data

    if (status == EXTSTATUS_OK)
    {
        if (LoadMacroSigs(lpstCallBacks,
                          lpszNAVEXDataDir) == FALSE)
            status = EXTSTATUS_INIT_ERROR;
    }

#ifdef MSX
    // Load exclusion data

    MSXStartUp(lpstCallBacks,lpszNAVEXDataDir);
#endif // #ifdef MSX

#endif // #ifndef NOMACRO

    if (status == EXTSTATUS_OK)
    {
        // load but don't fret if there's an error - we'll free everything
        // up later... this is to take care of rescue disks that don't have
        // virscan7.dat on them...

        LoadTrojanData(lpstCallBacks,lpszNAVEXDataDir);

        // load AOL stuff!

        if (GetNAVEX15INFInt(lpstCallBacks,
                             lpszNAVEXINIFile,
                             APP_STRING,
                             "aolpws",
                             TRUE) == TRUE)
        {
            // load that data!

            LoadAOLData(lpstCallBacks,lpszNAVEXDataDir);
        }
        else
        {
            ZeroAOLData();
        }
    }

    if (status != EXTSTATUS_OK && bNAVENGError == FALSE)
        glpstEngineExportTable->EXTShutDown(lpstCallBacks);

    return ( status );
}


// EXTShutDown
//
//  lpstCallBacks       Callbacks provided by cleint
//
// Description:
//
//  Call this function only for NAVEX1.5 module.  Do not call for ENGINE1.5.
//  This func. frees all global scanning data allocated by NAVEX/ENGINE.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_MEM_ERROR           if memory error
//                        EXTSTATUS_FILE_ERROR          if file error
//                        EXTSTATUS_INIT_ERROR          general init. error
//

EXTSTATUS FAR WINAPI NLOADDS EXTShutDown
(
    LPCALLBACKREV2                          lpstCallBacks
)
{
    EXTSTATUS                               status, tempStatus;

    gnUsageCount--;

    if (gnUsageCount > 0)
        return(EXTSTATUS_OK);

    status = EXTSTATUS_OK;

    // Free trojan scanning data!

    FreeTrojanData(lpstCallBacks);

    // Free AOL scanning data; if we didn't load, no worries...

    FreeAOLData(lpstCallBacks);

#ifndef NOMACRO

#ifdef MSX
    // Unload exclusion data

    MSXShutDown(lpstCallBacks);
#endif // #ifdef MSX

    // Unload macro scanning data

    if (UnloadMacroSigs(lpstCallBacks) == FALSE)
        status = EXTSTATUS_INIT_ERROR;

#endif // #ifndef NOMACRO

    if (glpstEngineExportTable != NULL)
    {
        tempStatus = glpstEngineExportTable->EXTShutDown(lpstCallBacks);
        if (tempStatus != EXTSTATUS_OK)
            status = tempStatus;
    }

    return ( status );
}




