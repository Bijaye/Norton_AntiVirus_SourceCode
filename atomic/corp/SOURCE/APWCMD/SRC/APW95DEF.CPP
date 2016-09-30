// Copyright 1996-1997 Symantec Corporation
//////////////////////////////////////////////////////////////////////////
// $Header:   S:/APWCMD/VCS/apw95def.cpv   1.9   01 Jul 1998 14:34:44   CEATON  $
//
// Description:
//    This source file contains routines for watching the Definition files
//    when we discover that the definition files have changed, we send
//    a re-load engine message to the device driver.
//
//    Also watches options files for changes and sends reload config message.
//
//////////////////////////////////////////////////////////////////////////
// $Log:   S:/APWCMD/VCS/apw95def.cpv  $
// 
//    Rev 1.9   01 Jul 1998 14:34:44   CEATON
// Modified to support file notifications on Win32 platforms.
// 
// 
//    Rev 1.8   05 Jun 1998 14:08:26   RStanev
// Cosmetic change.
//
//    Rev 1.7   28 May 1998 07:00:22   RStanev
// Reloading defs if the heuristic settings in navopts change.
//
//    Rev 1.6   21 Aug 1997 00:17:58   DALLEE
// Use new m_szConfigPath instead of GetModuleName() for constructing filenames.
//
//    Rev 1.5   18 Aug 1997 13:40:14   DALLEE
// Big bug -- close file handles opened for options date checks.
//
//    Rev 1.4   13 Aug 1997 12:39:56   DALLEE
// Use definition monitor thread to also watch changes to options files.
// Added CApw95OptionsTime to monitor time/date changes to options.
//
//    Rev 1.3   12 Aug 1997 11:55:50   JBRENNA
// Add call to SvcUpdateDefDateInRegistry() when ApwCmd is Init-ing. This
// ensures that the registry is updated when we startup as well as when
// we change to new definitions.
//
//    Rev 1.2   27 Jun 1997 19:30:50   DALLEE
// Call new function SvcReloadApEngine() instead of SvcReloadApOptions().
// Options used to do both options and virus engine -- split into two
// functions now.
//
//    Rev 1.1   09 Jun 1997 16:28:48   JBRENNA
// Up the time between definition checks to 2 minuntes. It was mistakenly
// checked in to check every 20 seconds (by me).
//
//    Rev 1.0   06 Jun 1997 17:29:42   JBRENNA
// Initial revision.
//
//////////////////////////////////////////////////////////////////////////

#include "platform.h"
#include "drvcomm.h"
#include "apcomm.h"
#include "linksrv.h"
#include "apscomm.h"
#include "DefUtils.h"
#include "DefUtlEx.h"
#include "process.h"

#include "ApwUtil.h"
#include "ApwCmd.h"
#include "Apw95Cmd.h"

//************************************************************************
// Constants
//************************************************************************
#define         MAX_THREAD_STACK_SIZE           (10240)  // Max Stack size

static LPSTR SZ_VIRDEF_KEY = "SOFTWARE\\Symantec\\Norton AntiVirus\\Virus Defs\\LastUpdate";
static LPSTR SZ_VIRDEF_VALUENAME ="SystemTime";

//************************************************************************
// External data
//************************************************************************

#ifdef __cplusplus
extern "C" {
#endif

extern HINSTANCE    g_hInstance;

#ifdef __cplusplus
}
#endif

/////////////////////////////////////////////////////////////////////
// Functions

/////////////////////////////////////////////////////////////////////
// DefsSyncAppIds()
//
// This function synchronizes the AppId2 such that it is using the
// same definitions as AppId1. By moving the AppId2's usage, potentially
// the AppId2's old definitions will be removed from the system.
//
// Parameters:
//    none
//
// Returns:
//    BOOL - TRUE  - AppIds are sync-ed
//         - FALSE - AppIds are not sync-ed
//
/////////////////////////////////////////////////////////////////////
// 06/05/97 - JBRENNA - Function Created
/////////////////////////////////////////////////////////////////////
BOOL CApw95Cmd::SvcDefsSyncAppIds()
{
    char     szAppId1Path[_MAX_PATH];
    BOOL     bReturn = TRUE;

    bReturn = m_DefUtilsAppId1.GetCurrentDefs (szAppId1Path, sizeof (szAppId1Path));

    if (bReturn)
        bReturn = m_DefUtilsAppId2.SetCurrentDefs (szAppId1Path);

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// DefsNeedReLoading()
//
// This function determines if we need to tell the Auto-Protect to
// re-load the virus definitions.
//
// Parameters:
//    none
//
// Returns:
//    BOOL - TRUE  - Defs need updating
//         - FALSE - Defs haven't changed.
//
/////////////////////////////////////////////////////////////////////
// 06/05/97 - JBRENNA - Changed to use CDefUtils for def management.
/////////////////////////////////////////////////////////////////////
BOOL CApw95Cmd::SvcDefsNeedReLoading()
{
    BOOL bReturn = FALSE;

#ifdef SYM_WIN32
    if ( !m_DefUtilsAppId1.IsNewDefsAvailable() )
        return( FALSE );
#endif

    if (!m_DefUtilsAppId1.UseNewestDefs (&bReturn))
        bReturn = FALSE;

    return bReturn;
}


/////////////////////////////////////////////////////////////////////
// UpdateDefDateInRegistry()
//
// Purpose:
//    This uses Hawking (CDefUtils) to set the definition date in
//    the registry. This allows non-NAV applications to determine the
//    virus definitions date.
//
// Parameters:
//    none.
//
// Returns:
//    none.
//
/////////////////////////////////////////////////////////////////////
// 01/24/97 - DSACKIN - Function Created.
// 06/06/97 - JBRENNA - ported from NAVAPSVC
// 08/12/97 - JBRENNA - Added the functionality to actually put the
//                      date into the registry ... using Hawking.
/////////////////////////////////////////////////////////////////////
void CApw95Cmd::SvcUpdateDefDateInRegistry()
{
    WORD        wYear;
    WORD        wMonth;
    WORD        wDay;
    DWORD       dwVersion;
    SYSTEMTIME  SysTime;
    LONG        lStatus;
    HKEY        hKey;
    DWORD       dwDisposition;

    memset (&SysTime, 0, sizeof (SysTime));

                                        // Get the definition date.
    m_DefUtilsAppId1.GetCurrentDefsDate (&wYear, &wMonth, &wDay, &dwVersion);



                                        // Create, if it doesn't already
                                        // exist, a registry entry for
                                        // keeping track of the date of the
                                        // virus defs in use by NAV.
    lStatus = RegCreateKeyEx (HKEY_LOCAL_MACHINE, SZ_VIRDEF_KEY, 0, "",
                              REG_OPTION_NON_VOLATILE,
                              KEY_ALL_ACCESS, NULL, &hKey,
                              &dwDisposition);

    if (lStatus == ERROR_SUCCESS)
    {
        SysTime.wMonth = wMonth;
        SysTime.wDay   = wDay;
        SysTime.wYear  = wYear;

                                        // Store that date in the registry.
        lStatus = RegSetValueEx (hKey, SZ_VIRDEF_VALUENAME, 0, REG_BINARY,
                                 (BYTE *)&SysTime, sizeof (SysTime));

        RegCloseKey (hKey);
    }
}


/////////////////////////////////////////////////////////////////////
// DefinitionMonitor()
//
// This function monitors the definitions to see when we need to
// update them.
// Also watches for changes to options files.
//
// Parameters:
//    none
//
// Returns:
//    BOOL - TRUE  - Success
//         - FALSE - Failure
//
/////////////////////////////////////////////////////////////////////
// 08/19/96 - DSACKIN - Function Created
// 06/06/97 - JBRENNA - ported from NAVAPSVC project
/////////////////////////////////////////////////////////////////////

void CApw95Cmd::SvcDefinitionMonitor()
{
                                        // Check for new definitions every
                                        // 120 times the TIMEOUT occurs. As
                                        // MONITOR_WAIT_MS is set to 1 second,
                                        // This will check about every 2
                                        // minutes.
    const DWORD dwDefsChkMod = 120;
    auto  DWORD dwLoopCnt    = 0;

                                        // Now that we have the events,
                                        // its time to start monitoring
    while (!m_bDefMonitorExit)
    {
        if (0 == (dwLoopCnt % dwDefsChkMod))
        {
            auto BOOL bDefsReloaded;

            if (bDefsReloaded = SvcDefsNeedReLoading())
            {
                                        // Reload the virus definitions
                SvcReloadApEngine();

                SvcUpdateDefDateInRegistry();

                                        // Set AppId2 to the current AppId1
                                        // directory setting.
                SvcDefsSyncAppIds();
            }

            if ( SvcOptionsNeedReLoading() )
            {
                auto BOOL bEnableHeuristicScanSave = m_bEnableHeuristicScan;
                auto UINT uHeuristicLevelSave = m_uHeuristicLevel;

                SvcReloadApConfig();
                SvcLoadSvcOptions();

                m_OptionsTime.Update();

                if ( !bDefsReloaded &&
                     ( bEnableHeuristicScanSave != m_bEnableHeuristicScan ||
                       uHeuristicLevelSave != m_uHeuristicLevel ) )
                    {
                                        // Reload the virus definitions
                    SvcReloadApEngine();
                    }
            }
        }
        ++dwLoopCnt;

        Sleep (1000L /* 1 second */);
    }

#ifdef SYM_WIN32
    m_DefUtilsAppId1.StopNewDefsDetection();
#endif
}


/////////////////////////////////////////////////////////////////////
// DefinitionMonitorProc()
//
// This function is a passthrough to our DefinitionMonitor function
//
// Parameters:
//    pObject  - Pointer to our object.
//
// Returns:
//    Doesn't return.  We call endthread to exit this function.
//
/////////////////////////////////////////////////////////////////////
// 08/19/96 - DSACKIN - Function Created
// 06/06/97 - JBRENNA - ported from NAVAPSVC project
/////////////////////////////////////////////////////////////////////
static void DefinitionMonitorProc(void *pObject)
{
    CApw95Cmd *pThis;

    if (NULL != pObject)
    {
                                        // Get our this pointer
        pThis = (CApw95Cmd *) pObject;

                                        // And then call our monitor
        pThis->SvcDefinitionMonitor();
    }

    _endthread();                       // Don't just return!!!
}


/////////////////////////////////////////////////////////////////////
// StartDefinitionMonitor()
//
// This function does a beginthread on our DefinitionMonitor.
//
// Parameters:
//    none
//
// Returns:
//    BOOL - TRUE  - Success
//         - FALSE - Faulire
//
/////////////////////////////////////////////////////////////////////
// 08/19/96 - DSACKIN - Function Created
// 06/06/97 - JBRENNA - ported from NAVAPSVC
/////////////////////////////////////////////////////////////////////
BOOL CApw95Cmd::SvcStartDefinitionMonitor()
{
    auto    HANDLE  hThread;
    auto    BOOL    bReturn = TRUE;

    // Get initial value for options files times.

    m_OptionsTime.Init( m_szConfigPath );
    m_OptionsTime.Update();

    // Start definiton / options monitor thread.

#ifdef SYM_WIN32
    m_DefUtilsAppId1.StartNewDefsDetection();
#endif

    hThread = (HANDLE)_beginthread (
                  DefinitionMonitorProc,// start Addr
                  MAX_THREAD_STACK_SIZE,// stack size
                  (LPVOID) this);       // Let it know what object!

    if ((HANDLE)INVALID_HANDLE_VALUE == hThread)
    {
#ifdef SYM_WIN32
        m_DefUtilsAppId1.StopNewDefsDetection();
#endif

        bReturn = FALSE;
    }

    return ( bReturn );
}


//************************************************************************
// CApw95OptionsDate
//************************************************************************

//************************************************************************
// CApw95Cmd::SvcOptionsNeedReLoading()
//
// Checks whether options have changed.
//
// Parameters:
//
// Returns:
//
//************************************************************************
// 08/12/1997 DALLEE, created.
//************************************************************************

BOOL CApw95Cmd::SvcOptionsNeedReLoading()
{
    auto    CApw95OptionsTime   xTimeNow;

    // Quick initialization of xTimeNow.

    xTimeNow = m_OptionsTime;

    // Get current date/time of options files.

    xTimeNow.Update();

    // Compare versus last saved times.

    return ( xTimeNow != m_OptionsTime );
} // Capw95Cmd::SvcOptionsNeedReLoading()


const LPCTSTR CApw95OptionsTime::m_alpszOptionsFiles [] = { _T("NAVOPTS.DAT"),
                                                            _T("NAVSTART.DAT"),
                                                            _T("EXCLUDE.DAT"),
                                                            _T("EXCLUDEL.DAT"),
                                                            _T("NAVALERT.DAT"),
                                                            _T("LONGMSG.TXT") };

//************************************************************************
// CApw95OptionsTime::CApw95OptionsTime()
//
// Constructor for options time object.
//
// Parameters:
//
// Returns:
//
//************************************************************************
// 08/12/1997 DALLEE, created.
//************************************************************************

CApw95OptionsTime::CApw95OptionsTime ()
{
    memset ( m_aOptTimes, 0, sizeof( m_aOptTimes ) );
} // CApw95OptionsTime::CApw95OptionsTime()


//************************************************************************
// CApw95OptionsTime::~CApw95OptionsTime()
//
// Destructor for options date object.
//
// Parameters:
//
// Returns:
//
//************************************************************************
// 08/12/1997 DALLEE, created.
//************************************************************************

CApw95OptionsTime::~CApw95OptionsTime ()
{
} // CApw95OptionsTime::~CApw95OptionsTime()


//************************************************************************
// CApw95OptionsTime::Init()
//
// Set the location of the options files to check.
//
// Parameters:
//  LPCTSTR lpszOptionsDir
//
// Returns:
//  TRUE on success.
//  FALSE otherwise.
//************************************************************************
// 08/12/1997 DALLEE, created.
//************************************************************************

CApw95OptionsTime::Init ( LPCTSTR lpszOptionsDir )
{
    auto    int     i;
    auto    BOOL    bSuccess;

    SYM_ASSERT( lpszOptionsDir );

    if ( _tcslen( lpszOptionsDir ) + MAX_OPTIONS_FILE_LEN + 2 > MAX_PATH )
    {
        SYM_ASSERT( FALSE );
        bSuccess = FALSE;
        goto BailOut;
    }

    for ( i = 0; i < NUM_OPTIONS_FILES; i++ )
    {
        _tcscpy( m_aOptTimes[i].szFileName, lpszOptionsDir );
        ApwNameCat( m_aOptTimes[i].szFileName, m_alpszOptionsFiles[i] );

        memset( &m_aOptTimes[i].ftFileTime, 0, sizeof( FILETIME ) );
    }

    bSuccess = TRUE;

BailOut:
    return ( bSuccess );
} // CApw95OptionsTime::Init()


//************************************************************************
// CApw95OptionsTime::Update()
//
// Get the current last modified time of the options files.
//
// Parameters:
//
// Returns:
//  TRUE on success.
//  FALSE otherwise.
//************************************************************************
// 08/12/1997 DALLEE, created.
//************************************************************************

CApw95OptionsTime::Update ()
{
    auto    BOOL    bSuccess;
    auto    HANDLE  hFile;
    auto    int     i;

    SYM_ASSERT( m_aOptTimes[0].szFileName[0] );

    // Initialize locals.

    bSuccess = FALSE;

    // Get file time for each options file.

    for ( i = 0; i < NUM_OPTIONS_FILES; i++ )
    {
        memset( &m_aOptTimes[i].ftFileTime, 0, sizeof( FILETIME ) );

        hFile = CreateFile( m_aOptTimes[i].szFileName,
                            0,
                            FILE_SHARE_READ | FILE_SHARE_WRITE,
                            NULL,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL );

        if ( INVALID_HANDLE_VALUE != hFile )
        {
            if ( GetFileTime( hFile, NULL, NULL, &m_aOptTimes[i].ftFileTime ) )
            {
                // Some files ( LONGMSG.TXT ) are optional, just return TRUE
                // if we got at least one file time.

                bSuccess = TRUE;
            }

            CloseHandle( hFile );
        }
    }

    return ( bSuccess );
} // CApw95OptionsTime::Update()


//************************************************************************
// CApw95OptionsTime::operator==()
//
// Compares two sets of options for differences in last modified times.
//
// Parameters:
//
// Returns:
//  TRUE    if last modified times are the same.
//  FALSE   otherwise.
//************************************************************************
// 08/12/1997 DALLEE, created.
//************************************************************************

BOOL CApw95OptionsTime::operator== ( const CApw95OptionsTime &xTime )
{
    auto    int     i;
    auto    BOOL    bEqual;

    SYM_ASSERT( m_aOptTimes[0].szFileName[0] );
    SYM_ASSERT( m_aOptTimes[0].szFileName[0] );

    // Initialize locals.

    bEqual = TRUE;

    // Check all options file times.

    for ( i = 0; i < NUM_OPTIONS_FILES; i++ )
    {
        if ( memcmp( &m_aOptTimes[i].ftFileTime,
                     &xTime.m_aOptTimes[i].ftFileTime,
                     sizeof( FILETIME ) ) )
        {
            bEqual = FALSE;
            break;
        }
    }

    return ( bEqual );
} // CApw95OptionsTime::operator==()


//************************************************************************
// CApw95OptionsTime::operator!=()
//
// Compares two sets of options for differences in last modified times.
//
// Parameters:
//
// Returns:
//  TRUE    if last modified times are different.
//  FALSE   otherwise.
//************************************************************************
// 08/12/1997 DALLEE, created.
//************************************************************************

BOOL CApw95OptionsTime::operator!= ( const CApw95OptionsTime &xTime )
{
    return( !operator==( xTime ) );
} // CApw95OptionsTime::operator!=()


//************************************************************************
// CApw95OptionsTime::operator=()
//
// Assignment for CApw95OptionsTime.
//
// Parameters:
//
// Returns:
//************************************************************************
// 08/12/1997 DALLEE, created.
//************************************************************************

CApw95OptionsTime & CApw95OptionsTime::operator= ( const CApw95OptionsTime &xTime )
{
    memcpy( m_aOptTimes, xTime.m_aOptTimes, sizeof( m_aOptTimes ) );

    return ( *this );
} // CApw95OptionsTime::operator=()

