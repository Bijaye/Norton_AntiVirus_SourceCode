// Copyright 1995 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/NAVXUTIL/VCS/w95stuff.cpv   1.6   06 Oct 1998 16:05:04   tcashin  $
//
// Description:
//      This file contain only Win95 specific functions.
//
// Contains:
//      BackupInfectedFile()
//***************************************************************************
// $Log:   S:/NAVXUTIL/VCS/w95stuff.cpv  $
// 
//    Rev 1.6   06 Oct 1998 16:05:04   tcashin
// Fixed a typo that cause one of the lastscan reg values to not get created.
// 
//    Rev 1.5   23 Sep 1998 14:22:38   cnevare
// Move the include of lastscan.h outside SYM_WIN32.
// 
//    Rev 1.4   18 Sep 1998 11:02:42   tcashin
// Moved the LastScan reg key name strings to lastscan.h in avcore/include.
// 
//    Rev 1.3   17 Sep 1998 14:39:08   tcashin
// Added support for writing all the scan results to the LastScan reg key.
// 
//    Rev 1.2   13 Aug 1997 15:56:00   DBUCHES
// Added support for saving additional LastScan data to the registry
//
//    Rev 1.1   24 Feb 1997 12:48:52   DALLEE
// Port from HYDRA.15:
//            Rev 1.18   26 Sep 1996 14:06:58   PVENKAT
//         Moved constant strings with //##IPE BEGIN ... //##IPE END.
//
//    Rev 1.0   06 Feb 1997 21:05:08   RFULLER
// Initial revision
//
//    Rev 1.8   20 Nov 1996 15:22:20   GWESTER
// ROLLED BACK TO v1.4 to remove dynamic loading of the VxD.
//
//    Rev 1.4   13 Sep 1996 09:58:42   TIVANOV
// fix the oem to ansi conversion for the pif shortcuts
//
//    Rev 1.3   19 Aug 1996 15:49:44   GWESTER
// Also removed #ifdef NAVSCANW from around SZ_LASTSCAN_... reg keys at top.
//
//    Rev 1.2   19 Aug 1996 15:46:12   GWESTER
// For Kirin, removed the #ifdef NAVSCANW around RegistryStampLastScanTime().
//
//    Rev 1.1   13 Feb 1996 12:33:26   GWESTER
// Modifed location of sensor keys under NT (Last Scan and Virus Definitions
// Date).
//
//    Rev 1.0   30 Jan 1996 15:56:20   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:51:22   BARRY
// Initial revision.
//
//    Rev 1.15   27 Jan 1996 18:55:30   GWESTER
// Changed scanner version (NNTWORK.DLL) so that it won't try to write VxD
// keys.
//
//    Rev 1.14   14 Jan 1996 16:58:08   DALLEE
// Changed #ifdef NAVWNT to NAVSCANW.
//
//    Rev 1.13   13 Dec 1995 17:59:00   GWESTER
// In RegistryAddVirusDefDate() and RegistryStampLastScanTime(), disabled
// SYM_ASSERT calls on Registry API calls. If the current user is a Guest,
// the API calls fail, and the SYM_ASSERTs throw an exception. Now, under NT,
// the API calls will fail quietly.
//
//    Rev 1.12   12 Dec 1995 20:40:06   GWESTER
// For NT platform, added RegistryStampLastScanTime(), which will store the
// last time that all of the system's local drives were scanned in the
// NT Registry.
//
//    Rev 1.11   28 Nov 1995 11:34:42   BARRY
// ifndef SYM_VXD for SystemGetWindowsType(). Real fix is to provide VxD wrapper for this fn
//
//    Rev 1.10   27 Nov 1995 20:19:56   DALLEE
// Don't write VxD registry keys under NT.
//
//    Rev 1.9   22 Nov 1995 10:45:06   YVANG
// For NT: Fixed the problem in which the registry key, 'Virus Defs' is created
// under the wrong place, 'Norton AntiVirus'.  It should be placed under 'Norton
// AntiVirus NT'.
//
//    Rev 1.8   16 Jul 1995 19:02:04   MARKL
// Now manipulates the
// HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Run
// registry key.  The Value "Norton Auto-Protect" controls the agent
// being launched across multiple login sessions.
//
//    Rev 1.7   07 Jul 1995 12:16:14   KEITH
// Add an AnsiToOem so that the registry writes the VXD path
// in OEM. This is the only entry where we want this.
//
//    Rev 1.6   23 May 1995 14:30:34   MARKL
// Fixed registry name.
//
//    Rev 1.5   23 May 1995 12:15:34   MARKL
// Updated such that VxD only pulls in the routines needed.
//
//    Rev 1.4   22 May 1995 21:16:56   MARKL
// Added RegistryAddVirusDefDate()
//
//    Rev 1.3   26 Apr 1995 10:27:30   MFALLEN
// Add the short full path to the VxD to the registry and not just the name of the
// VxD.
//
//    Rev 1.2   25 Apr 1995 10:13:28   MFALLEN
// Registry now works fine.
//
//    Rev 1.1   24 Apr 1995 14:54:00   MFALLEN
// Added auto-protect registry functions. Not debugged yet due to build system
// limitations.
//
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "file.h"
#include "navutil.h"
#include "lastscan.h"

#ifdef SYM_WIN32

#include "nonav.h"

extern "C"
    {
    #include <shlobj.h>
    #include <objbase.h>
    }

#endif

// ---------------------------------------------------------------------------
//  Static variables to this function.
// ---------------------------------------------------------------------------
//##IPE BEGIN
static LPSTR SZ_NAVAP_KEY = "System\\CurrentControlSet\\Services\\VxD\\NAVAP";
static LPSTR SZ_NAVAP_VALUENAME = "StaticVxD";
static LPSTR SZ_NAVAP_DATA = "NAVAP.VXD";

static LPSTR SZ_NAVAPW_KEY = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
static LPSTR SZ_NAVAPW_VALUENAME = "Norton Auto-Protect";
static LPSTR SZ_NAVAPW_DATA = "NAVAPW32.EXE /LOADQUIET";

static LPSTR SZ_VIRDEF_KEY = "SOFTWARE\\Symantec\\Norton AntiVirus\\Virus Defs\\LastUpdate";
static LPSTR SZ_VIRDEF_KEY_NT = "SOFTWARE\\Symantec\\Norton AntiVirus\\Virus Defs\\LastUpdate";
static LPSTR SZ_VIRDEF_VALUENAME ="SystemTime";

                                        // Define this also for W95, in
                                        // case we port it back from NT:
static LPSTR SZ_LASTSCAN_KEY           = NAV_LASTSCAN_KEY;
static LPSTR SZ_LASTSCAN_KEY_NT        = NAV_LASTSCAN_KEY_NT;
static LPSTR SZ_LASTSCAN_SYSTEMTIME    = NAV_LASTSCAN_SYSTEMTIME;
static LPSTR SZ_LASTSCAN_DRIVES        = NAV_LASTSCAN_DRIVES;
static LPSTR SZ_LASTSCAN_FILESSCANNED  = NAV_LASTSCAN_FILESSCANNED;
static LPSTR SZ_LASTSCAN_FILESINFECTED = NAV_LASTSCAN_FILESINFECTED;
static LPSTR SZ_LASTSCAN_FILESREPAIRED = NAV_LASTSCAN_FILESREPAIRED;
static LPSTR SZ_LASTSCAN_FILESQUAR     = NAV_LASTSCAN_FILESQUAR;
static LPSTR SZ_LASTSCAN_FILESDELETED  = NAV_LASTSCAN_FILESDELETED;
static LPSTR SZ_LASTSCAN_MBRSCANNED    = NAV_LASTSCAN_MBRSCANNED;
static LPSTR SZ_LASTSCAN_MBRINFECTED   = NAV_LASTSCAN_MBRINFECTED;
static LPSTR SZ_LASTSCAN_MBRREPAIRED   = NAV_LASTSCAN_MBRREPAIRED;
static LPSTR SZ_LASTSCAN_BRSCANNED     = NAV_LASTSCAN_BRSCANNED;
static LPSTR SZ_LASTSCAN_BRINFECTED    = NAV_LASTSCAN_BRINFECTED;
static LPSTR SZ_LASTSCAN_BRREPAIRED    = NAV_LASTSCAN_BRREPAIRED;

static LPSTR SZ_LASTSCAN_SCANTIME      = NAV_LASTSCAN_SCANTIME;
static LPSTR SZ_LASTSCAN_SCANSUMMARY   = NAV_LASTSCAN_SCANSUMMARY;
static LPSTR SZ_LASTSCAN_SCANMEMORY    = NAV_LASTSCAN_SCANMEMORY;
static LPSTR SZ_LASTSCAN_SCANFILES     = NAV_LASTSCAN_SCANFILES;
static LPSTR SZ_LASTSCAN_SCANSETTINGS  = NAV_LASTSCAN_SCANSETTINGS;

static LPSTR SZ_LASTSCAN_UPDATE        = "Update";

//##IPE END

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

#ifdef SYM_WIN32

VOID SYM_EXPORT WINAPI IfSpecialFileGetTrueFile (// If provided file is LNK then get
    LPSTR   lpFile                      // [in][out] true filename.
    )
{
    auto    char    szTargetFile [ SYM_MAX_PATH + 1 ];
    auto    char    szFile       [ SYM_MAX_PATH + 1 ];
    auto    char    szExt        [ SYM_MAX_PATH + 1 ];

    STRCPY ( szFile, lpFile );

    NameReturnExtension ( szFile, szExt );

    if ( !STRICMP ( szExt, "LNK" ) )
        {
        if ( ResolveLnkFile ( szFile, szTargetFile ) == TRUE )
            STRCPY ( lpFile, szTargetFile );
        }
    else if ( !STRICMP ( szExt, "PIF" ) )
        {
        if ( ResolvePifFile ( szFile, szTargetFile ) == TRUE )
            STRCPY ( lpFile, szTargetFile );
        }
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

BOOL SYM_EXPORT WINAPI ResolvePifFile ( // Finds what PIF file pointing to
    LPCSTR  pszPifFile,                 // [in] pointer to PIF file
    LPSTR   pszPath                     // [out] pointer to target file.
    )
{
    typedef struct tagPIFFORMAT
        {
        WORD    wReserved;
        char    byProgramTitle [ 30 ];  // non null terminated
        WORD    wMaxMemToPartition;     // in K
        WORD    wMinMemRequired;        // in K
        char    szProgramName [ 64 ];   // What about LFNs?
        BYTE    byDefaultDriveLetter;
        char    szDefaultDir [ 64 ];
        char    szProgParams [ 64 ];
        BYTE    byInitScreenMode;
        BYTE    byNumTextPages;
        BYTE    byNumFirstSaveInt;
        BYTE    byNumLastSaveInt;
        BYTE    byVirtScreenRows;
        BYTE    byVirtScreenCols;
        BYTE    byInitWindowsRow;
        BYTE    byInitWindowsCol;
        WORD    wSystemMemory;          // in K
        char    szSharedProgName [ 64 ];
        char    szSharedDataFile [ 64 ];
        BYTE    byFlags1;
        BYTE    byFlags2;
        } PIFFORMAT;


    auto    BOOL        bReturn = FALSE;
    auto    HFILE       hFile;
    auto    PIFFORMAT   Pif;

    SYM_ASSERT ( pszPifFile && *pszPifFile );

    hFile = FileOpen ( pszPifFile, READ_ONLY_FILE );

    if ( hFile != HFILE_ERROR )
        {
        UINT uBytes = FileRead ( hFile, &Pif, sizeof(Pif) );

        if ( uBytes == sizeof(Pif) )
            {
                                        // -----------------------------------
                                        // Better safe than sorry
                                        // -----------------------------------

            if ( Pif.szProgramName[1] == ':' &&
                 Pif.szProgramName[2] == '\\' )
                {
                char t[1024];
                NavOemToAnsi(Pif.szProgramName, t);
                if ( NameReturnLongName (
                                        t,
                                        pszPath,
                                        TRUE
                                        ) == ERR )
                    {
                    STRCPY ( pszPath, Pif.szProgramName );
                    }

                bReturn = TRUE;
                }
            }

        FileClose ( hFile );
        }

    return ( bReturn );
}

// ---------------------------------------------------------------------------
// find the destination of a shortcut
//
// in:
//   pszShortcutFile    fully qualified path to shortcut file
//   cchPath            length of return buffer
//
// out:
//   pszPath        return buffer, NULL on failure
//
// returns:
//   standard hres codes
//
//
// assumes:
//   CoInitialize() has been called...
// ---------------------------------------------------------------------------

BOOL SYM_EXPORT WINAPI ResolveLnkFile ( // Finds what LNK file pointing to
    LPCSTR  pszShortcutFile,            // [in] pointer to LNK file
    LPSTR   pszPath                     // [out] pointer to target file.
    )
{
    auto    BOOL            bReturn = FALSE;
    auto    HRESULT         hres;
    auto    IShellLink *    psl;
	auto    char            szGotPath[SYM_MAX_PATH+ 1 ];
	auto    char            szDescription[SYM_MAX_PATH+ 1 ];
	auto    WIN32_FIND_DATA wfd;

                                        // ----------------------------------
                                        // This is to talk with the Win95
                                        // shell. Functions to query LNK
                                        // files require that this is called.
                                        // ----------------------------------
    if ( FAILED(CoInitialize(NULL)) )
        return ( bReturn );

    *pszPath = 0;                       // assume failure

    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                            IID_IShellLink, (void **)&psl);

    if (SUCCEEDED(hres))
        {
        IPersistFile *ppf;

        hres = psl->QueryInterface(IID_IPersistFile, (void **)&ppf);

        if (SUCCEEDED(hres))
            {
            WORD wsz[SYM_MAX_PATH+ 1 ]; /* Buffer for unicode string */

            MultiByteToWideChar ( CP_ACP, 0, pszShortcutFile, -1, wsz,
                                  SYM_MAX_PATH);

            hres = ppf->Load(wsz, STGM_READ);

            if (SUCCEEDED(hres))
                {
                                        // -----------------------------------
                                        // Resolve the link, this may post UI
                                        // to find the link
                                        // -----------------------------------

                hres = psl->Resolve ( NULL, SLR_ANY_MATCH | SLR_NO_UI );

                if ( SUCCEEDED(hres) )
				    {
					strcpy(szGotPath, pszShortcutFile);

                   	hres = psl->GetPath ( szGotPath, SYM_MAX_PATH,
                                         (WIN32_FIND_DATA *)&wfd,
                                          SLGP_SHORTPATH );

				   	if ( SUCCEEDED(hres) )
                        {
                        STRCPY ( pszPath, szGotPath );
	               	    hres = psl->GetDescription(szDescription, SYM_MAX_PATH);
                        bReturn = TRUE;
                        }
				    }
                }

            ppf->Release();
            }
        psl->Release();
        }

                                        // ----------------------------------
                                        // This is to talk with the Win95
                                        // shell. Functions to query LNK
                                        // files require that this is called.
                                        // ----------------------------------
    CoUninitialize();

    return ( bReturn );
}


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

BOOL SYM_EXPORT WINAPI RegistryAddVxD (
    VOID
    )
{
#ifdef NAVSCANW
                                        // The scanner version (NNTWORK.DLL)
                                        // shouldn't add VxD references to
                                        // the Registry (95 or NT).
    return TRUE;
#endif                                  // NAVSCANW

    extern  HINSTANCE   hInstance;
    auto    BOOL        bReturn         =   FALSE;
    auto    HKEY        hNavApKey       =   0;
    auto    DWORD       dwDisposition   =   0;
    auto    char        szFullPath  [ SYM_MAX_PATH + 1 ] = { 0 };
    auto    char        szShortPath [ SYM_MAX_PATH + 1 ] = { 0 };

                                        // Just return success w/ no change
                                        // if Windows NT.
#ifndef SYM_VXD
    if ( SYM_SYSTEM_WIN_NT == SystemGetWindowsType() )
        {
        return (TRUE);
        }
#endif

                                        // Put the VXD in to the
                                        // registry.
    if ( RegCreateKeyEx ( HKEY_LOCAL_MACHINE,
                          SZ_NAVAP_KEY,
                          0,
                          "",
                          REG_OPTION_NON_VOLATILE,
                          KEY_ALL_ACCESS,
                          NULL,
                          &hNavApKey,
                          &dwDisposition
                          ) == ERROR_SUCCESS )
        {
        GetModuleFileName ( hInstance, szFullPath, sizeof(szFullPath) );
        NameStripFile  ( szFullPath );
        NameReturnShortName ( szFullPath, szShortPath );
        NameAppendFile ( szShortPath, SZ_NAVAP_DATA );

                                        // The VXD path must be in OEM
                                        // format for Win95 to load it.
                                        // Other entries stay in ANSI.
        AnsiToOem ( szShortPath, szShortPath );

        if ( RegSetValueEx ( hNavApKey,
                             SZ_NAVAP_VALUENAME,
                             0,
                             REG_SZ,
                             (CONST BYTE *)szShortPath,
                             STRLEN(szShortPath) + 1
                             ) == ERROR_SUCCESS )
            {
            bReturn = TRUE;
            }

        RegCloseKey ( hNavApKey );
        }

    if ( !bReturn )
        return ( bReturn );

                                        // Put the Agent in to the
                                        // registry
    if ( RegCreateKeyEx ( HKEY_LOCAL_MACHINE,
                          SZ_NAVAPW_KEY,
                          0,
                          "",
                          REG_OPTION_NON_VOLATILE,
                          KEY_ALL_ACCESS,
                          NULL,
                          &hNavApKey,
                          &dwDisposition
                          ) == ERROR_SUCCESS )
        {
        NameReturnShortName ( szFullPath, szShortPath );
        NameAppendFile ( szShortPath, SZ_NAVAPW_DATA );

        if ( RegSetValueEx ( hNavApKey,
                             SZ_NAVAPW_VALUENAME,
                             0,
                             REG_SZ,
                             (CONST BYTE *)szShortPath,
                             STRLEN(szShortPath) + 1
                             ) == ERROR_SUCCESS )
            {
            bReturn = TRUE;
            }

        RegCloseKey ( hNavApKey );
        }

                                        // Return to caller
    return ( bReturn );
}


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

BOOL SYM_EXPORT WINAPI RegistryRemoveVxD (
    VOID
    )
{
#ifdef NAVSCANW
                                        // The scanner version (NNTWORK.DLL)
                                        // shouldn't add VxD references to
                                        // the Registry (95 or NT).
    return TRUE;
#endif                                  // NAVSCANW

    auto        LONG    lStatus1;
    auto        BOOL    bReturn         = FALSE;
    auto        HKEY    hNavApKey       = 0;
    auto        DWORD   dwDisposition   = 0;

                                        // Just return success w/ no change
                                        // if Windows NT.
#ifndef SYM_VXD
    if ( SYM_SYSTEM_WIN_NT == SystemGetWindowsType() )
        {
        return (TRUE);
        }
#endif

                                        // Remove the VXD
    lStatus1 = RegDeleteKey ( HKEY_LOCAL_MACHINE, SZ_NAVAP_KEY );

                                        // Now remove the agent.  This is
                                        // a little more complicated since
                                        // this is a shared entry and
                                        // removing the key would prove
                                        // fatal to other applications
                                        // depending on it so we simply
                                        // remove our value.
    if ( RegCreateKeyEx ( HKEY_LOCAL_MACHINE,
                          SZ_NAVAPW_KEY,
                          0,
                          "",
                          REG_OPTION_NON_VOLATILE,
                          KEY_ALL_ACCESS,
                          NULL,
                          &hNavApKey,
                          &dwDisposition
                          ) == ERROR_SUCCESS )
        {
        if ( RegDeleteValue ( hNavApKey, SZ_NAVAPW_VALUENAME ) == ERROR_SUCCESS )
            {
            bReturn = TRUE;
            }
        RegCloseKey ( hNavApKey );
        }

    return ( ( ( lStatus1 == ERROR_SUCCESS ) && bReturn )
             ? TRUE : FALSE );
}


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

BOOL SYM_EXPORT WINAPI RegistryIsVxDStartup (
    VOID
    )
{
    auto    BOOL    bReturn     =   FALSE;
    auto    DWORD   dwType      =   0;
    auto    HKEY    hNavApKey   =   0;
    auto    DWORD   dwDataSize;
    auto    char    szData [ SYM_MAX_PATH + 1 ] = { 0 };
    auto    LONG    lStatus;

    lStatus = RegOpenKeyEx (
                            HKEY_LOCAL_MACHINE,
                            SZ_NAVAP_KEY,
                            0,
                            KEY_ALL_ACCESS,
                            &hNavApKey
                            );

    if ( lStatus == ERROR_SUCCESS )
        {
        dwDataSize  =   sizeof(szData);

        lStatus = RegQueryValueEx (
                            hNavApKey,
                            SZ_NAVAP_VALUENAME,
                            NULL,
                            &dwType,
                            (LPBYTE)szData,
                            &dwDataSize
                            );

        if ( lStatus == ERROR_SUCCESS )
            {
            if ( dwDataSize && *szData )
                {
                bReturn = TRUE;
                }
            }

        lStatus = RegCloseKey ( hNavApKey );

        SYM_ASSERT ( lStatus == ERROR_SUCCESS );
        }

    return ( bReturn );
}

#endif // SYM_WIN32

#if defined(SYM_WIN32) || defined(SYM_VXD)

//***************************************************************************
// RegistryAddVirusDefDate()
//
// Description:
//      This routine updates the registry with the date/time of the current
//      virus defs in use.  This can then be used at a later time by products
//      to figure out if the virus defs are up to date.
//
// Parameters:
//      lpVirContext                    [in] The virus context
//
// Return Value:
//      BOOL    TRUE    Entry Added/Updated
//              FALSE   Didn't work
//
//***************************************************************************
// 05/22/1995 MARK LAWLER Function Created.
//***************************************************************************
BOOL SYM_EXPORT WINAPI RegistryAddVirusDefDate ( // Add date of vir def file
    HVCONTEXT hVirContext)                       // to the registry.
{
    extern  HINSTANCE   hInstance;
    auto    BOOL        bReturn                 = FALSE;
    auto    HKEY        hNavVirDefKey           = 0;
    auto    DWORD       dwDisposition           = 0;
    auto    UINT        uTemp                   = 0;
    auto    SYSTEMTIME  sysTime                 = {0};
    auto    char        szTemp [ 40 ];
    auto    LONG        lStatus;
    auto    LPSTR       lpVirdefKey             = NULL;

#ifndef SYM_VXD
    if (SYM_SYSTEM_WIN_NT == SystemGetWindowsType())
        lpVirdefKey = SZ_VIRDEF_KEY_NT;
    else
#endif
        lpVirdefKey = SZ_VIRDEF_KEY;
                                        // Create, if it doesn't already
                                        // exist, a registry entry for
                                        // keeping track of the date of the
                                        // virus defs in use by NAV.
    lStatus = RegCreateKeyEx (
                                HKEY_LOCAL_MACHINE,
                                lpVirdefKey,
                                0,
                                "",
                                REG_OPTION_NON_VOLATILE,
                                KEY_ALL_ACCESS,
                                NULL,
                                &hNavVirDefKey,
                                &dwDisposition
                                );

    if ( lStatus == ERROR_SUCCESS )
        {
                                        // Get the date of the Virus Defs
                                        // currently in use.  NOTE:  The
                                        // routine ConvertStringToWord()
                                        // lies!!!!  Piece of sh...
        VLGetDBInfo ( hVirContext, DB_MONTH_STAMP, szTemp );
        ConvertStringToWord ( &uTemp, szTemp );
        sysTime.wMonth = uTemp;
        VLGetDBInfo ( hVirContext, DB_DAY_STAMP, szTemp );
        ConvertStringToWord ( &uTemp, szTemp );
        sysTime.wDay = uTemp;
        VLGetDBInfo ( hVirContext, DB_YEAR_STAMP, szTemp );
        ConvertStringToWord ( &uTemp, szTemp );
        sysTime.wYear = uTemp;

                                        // Store that date in the registry.
        lStatus = RegSetValueEx (
                            hNavVirDefKey,
                            SZ_VIRDEF_VALUENAME,
                            0,
                            REG_BINARY,
                            (BYTE *)&sysTime,
                            sizeof (SYSTEMTIME)
                            );

        if ( lStatus == ERROR_SUCCESS )
            bReturn = TRUE;

        lStatus = RegCloseKey ( hNavVirDefKey );
        }


    return ( ( lStatus == ERROR_SUCCESS ) ? bReturn : FALSE );
} // RegistryAddVirusDefDate()

#endif // SYM_WIN32 || SYM_VXD

#ifdef SYM_WIN32

//***************************************************************************
// RegistryStampLastScanTime()
//
// Description:
//      If a scan was successfully completed, and checked all of the system's
//      local drives, this function will update the LastScan timestamp in
//      the Registry. NT System Doctor, Navigator, and other products can
//      use this timestamp to schedule scans of the system.
//
// Parameters:
//      LPSYSTEMTIME                    [in] The System Time
//
// Return Value:
//      BOOL    TRUE    Entry Added/Updated
//              FALSE   Didn't work
//
//***************************************************************************
// 12/12/1995 GWESTER Function Created.
//***************************************************************************
BOOL SYM_EXPORT WINAPI RegistryStampLastScanInfo (
    LPVOID lpvNav,
    LPSTR szDrives,
    LPSYSTEMTIME TimeStamp)
{
                                        // In the future, this function
                                        // might be ported to other
                                        // platforms besides NT, so it contains
                                        // conditional compiler directives.
    auto    BOOL        bReturn             = FALSE;
    auto    HKEY        hNavVirScanKey      = 0;
    auto    DWORD       dwDisposition       = 0;
    auto    LONG        lStatus;
    auto    LPSTR       lpVirScanKey        = NULL;
    auto    LPNONAV     lpNav               = (LPNONAV)lpvNav;
            HANDLE      hLastScanEvent      = NULL;


    SYM_ASSERT ( TimeStamp );

#ifndef SYM_VXD
    if (SYM_SYSTEM_WIN_NT == SystemGetWindowsType())
        lpVirScanKey = SZ_LASTSCAN_KEY_NT;
    else
#endif
        lpVirScanKey = SZ_LASTSCAN_KEY;
                                        // Create, if it doesn't already
                                        // exist, the entry:
    lStatus = RegCreateKeyEx (
                                HKEY_LOCAL_MACHINE,
                                lpVirScanKey,
                                0,
                                "",
                                REG_OPTION_NON_VOLATILE,
                                KEY_ALL_ACCESS,
                                NULL,
                                &hNavVirScanKey,
                                &dwDisposition
                                );

    if ( lStatus == ERROR_SUCCESS )
        {
                                        // Stamp the time and date in
                                        // the Registry:
        lStatus = RegSetValueEx (
                            hNavVirScanKey,
                            SZ_LASTSCAN_SYSTEMTIME,
                            0,
                            REG_BINARY,
                            (BYTE *)TimeStamp,
                            sizeof (SYSTEMTIME)
                            );

        if ( lStatus == ERROR_SUCCESS )
            bReturn = TRUE;

        // If we have a NONAV structure, copy additional data to
        // the registry.
        if ( lpNav != NULL && bReturn)
            {
            DWORD dwData;
                                        // Stamp the drives we scanned
                                        // the Registry:
            lStatus = RegSetValueEx (
                                hNavVirScanKey,
                                SZ_LASTSCAN_DRIVES,
                                0,
                                REG_SZ,
                                (BYTE *)szDrives,
                                lstrlen(szDrives)
                                );

                                        // Stamp the number of files we
                                        // scanned to the Registry:

            dwData = lpNav->Stats.Files.dwScanned;
            lStatus = RegSetValueEx (
                                hNavVirScanKey,
                                SZ_LASTSCAN_FILESSCANNED,
                                0,
                                REG_DWORD,
                                (BYTE *)&dwData,
                                sizeof(DWORD)
                                );
                                        // Stamp the number of infected
                                        // files to the Registry:
            dwData = lpNav->Stats.Files.uInfected;
            lStatus = RegSetValueEx (
                                hNavVirScanKey,
                                SZ_LASTSCAN_FILESINFECTED,
                                0,
                                REG_DWORD,
                                (BYTE *)&dwData,
                                sizeof(DWORD)
                                );
                                        // Stamp the number of repaired
                                        // files to the Registry:
            dwData = lpNav->Stats.Files.uCleaned;
            lStatus = RegSetValueEx (
                                hNavVirScanKey,
                                SZ_LASTSCAN_FILESREPAIRED,
                                0,
                                REG_DWORD,
                                (BYTE *)&dwData,
                                sizeof(DWORD)
                                );
                                        // Stamp the number of deleted
                                        // files to the Registry:
            dwData = lpNav->Stats.Files.uDeleted;
            lStatus = RegSetValueEx (
                                hNavVirScanKey,
                                SZ_LASTSCAN_FILESDELETED,
                                0,
                                REG_DWORD,
                                (BYTE *)&dwData,
                                sizeof(DWORD)
                                );
                                        // Stamp the number of quarantined
                                        // files to the Registry:
            dwData = lpNav->Stats.Files.uQuarantined;
            lStatus = RegSetValueEx (
                                hNavVirScanKey,
                                SZ_LASTSCAN_FILESQUAR,
                                0,
                                REG_DWORD,
                                (BYTE *)&dwData,
                                sizeof(DWORD)
                                );
                                        // Stamp the number of scanned
                                        // MBRs to the Registry:
            dwData = lpNav->Stats.MasterBootRec.uScanned;
            lStatus = RegSetValueEx (
                                hNavVirScanKey,
                                SZ_LASTSCAN_MBRSCANNED,
                                0,
                                REG_DWORD,
                                (BYTE *)&dwData,
                                sizeof(DWORD)
                                );
                                        // Stamp the number of infected
                                        // MBRs to the Registry:
            dwData = lpNav->Stats.MasterBootRec.uInfected;
            lStatus = RegSetValueEx (
                                hNavVirScanKey,
                                SZ_LASTSCAN_MBRINFECTED,
                                0,
                                REG_DWORD,
                                (BYTE *)&dwData,
                                sizeof(DWORD)
                                );
                                        // Stamp the number of repaired
                                        // MBRs to the Registry:
            dwData = lpNav->Stats.MasterBootRec.uCleaned;
            lStatus = RegSetValueEx (
                                hNavVirScanKey,
                                SZ_LASTSCAN_MBRREPAIRED,
                                0,
                                REG_DWORD,
                                (BYTE *)&dwData,
                                sizeof(DWORD)
                                );
                                        // Stamp the number of scanned
                                        // boot records to the Registry:
            dwData = lpNav->Stats.BootRecs.uScanned;
            lStatus = RegSetValueEx (
                                hNavVirScanKey,
                                SZ_LASTSCAN_BRSCANNED,
                                0,
                                REG_DWORD,
                                (BYTE *)&dwData,
                                sizeof(DWORD)
                                );
                                        // Stamp the number of infected
                                        // boot records to the Registry:
            dwData = lpNav->Stats.BootRecs.uInfected;
            lStatus = RegSetValueEx (
                                hNavVirScanKey,
                                SZ_LASTSCAN_BRINFECTED,
                                0,
                                REG_DWORD,
                                (BYTE *)&dwData,
                                sizeof(DWORD)
                                );
                                        // Stamp the number of repaired
                                        // boot records to the Registry:
            dwData = lpNav->Stats.BootRecs.uCleaned;
            lStatus = RegSetValueEx (
                                hNavVirScanKey,
                                SZ_LASTSCAN_BRREPAIRED,
                                0,
                                REG_DWORD,
                                (BYTE *)&dwData,
                                sizeof(DWORD)
                                );
                                        // Stamp the total effective 
                                        // scanning time in the Registry:
            dwData = lpNav->Stats.lTimeScan;
            lStatus = RegSetValueEx (
                                hNavVirScanKey,
                                SZ_LASTSCAN_SCANTIME,
                                0,
                                REG_DWORD,
                                (BYTE *)&dwData,
                                sizeof(DWORD)
                                );
                                        // Stamp the scanning summary
                                        // value in the Registry:
            if ((lpNav->Stats.Files.uInfected && lpNav->Stats.BootRecs.uInfected) ||
               (lpNav->Stats.Files.uInfected && lpNav->Stats.MasterBootRec.uInfected))
	            dwData = LASTSCAN_SUMMARY_INFECTEDBOOT | LASTSCAN_SUMMARY_INFECTEDFILES;
            else if (lpNav->Stats.BootRecs.uInfected || lpNav->Stats.MasterBootRec.uInfected)
	            dwData = LASTSCAN_SUMMARY_INFECTEDBOOT;
            else if (lpNav->Stats.Files.uInfected)
	            dwData = LASTSCAN_SUMMARY_INFECTEDFILES;
            else
	            dwData = LASTSCAN_SUMMARY_NOINFECTEDITEMS;

            lStatus = RegSetValueEx (
                                hNavVirScanKey,
                                SZ_LASTSCAN_SCANSUMMARY,
                                0,
                                REG_DWORD,
                                (BYTE *)&dwData,
                                sizeof(DWORD)
                                );

                                        // Stamp the memory scanning
                                        // results in the Registry:
            if (SYM_SYSTEM_WIN_NT != SystemGetWindowsType())
	        {
	            if (lpNav->Opt.bMemory)
	                dwData = LASTSCAN_MEMSCAN_YES;
	            else
	                dwData = LASTSCAN_MEMSCAN_NO;
	        }
            else
                dwData = LASTSCAN_MEMSCAN_NOTSUPPORTED;

            lStatus = RegSetValueEx (
                                hNavVirScanKey,
                                SZ_LASTSCAN_SCANMEMORY,
                                0,
                                REG_DWORD,
                                (BYTE *)&dwData,
                                sizeof(DWORD)
                                );
                                        // Stamp the file type 
                                        // data in the Registry:
            if (lpNav->Opt.bScanAll)
                dwData = LASTSCAN_FILETYPES_ALL;
            else
                dwData = LASTSCAN_FILETYPES_PROGRAMFILES;

            lStatus = RegSetValueEx (
                                hNavVirScanKey,
                                SZ_LASTSCAN_SCANFILES,
                                0,
                                REG_DWORD,
                                (BYTE *)&dwData,
                                sizeof(DWORD)
                                );
                                        // Stamp the "Scan Settings"
                                        // data (compressed files) in the Registry:
            if (lpNav->Gen.bScanZipFiles)
                dwData = LASTSCAN_SETTINGS_SCANCOMPRESSED;
            else
                dwData = LASTSCAN_SETTINGS_NOSCANCOMPRESSED;

            lStatus = RegSetValueEx (
                                hNavVirScanKey,
                                SZ_LASTSCAN_SCANSETTINGS,
                                0,
                                REG_DWORD,
                                (BYTE *)&dwData,
                                sizeof(DWORD)
                                );

                // Sets a global event to notify other processes that a scan has completed.
                // This function doesn't create the event since we are only in here
                // temporarily.

                hLastScanEvent = OpenEvent ( EVENT_MODIFY_STATE, FALSE, "NAV_LASTSCANUPDATE" );
                if ( hLastScanEvent )
                    SetEvent ( hLastScanEvent );
            }

        lStatus = RegCloseKey ( hNavVirScanKey );
        }

    return ( ( lStatus == ERROR_SUCCESS ) ? bReturn : FALSE );

}                                       // End RegistryStampLastScanTime ()



//***************************************************************************
// RegistryCheckLastScanUpdateKey()
//
// Description:
//      Look to the Update value. This value will be added manually to the
//      as a way for QA to always put scan results in the reqistry.
//
// Parameters:
//      None
//
// Return Value:
//      BOOL    TRUE    Ok to update the registry
//              FALSE   
//
//***************************************************************************
// 9/15/1998 TCASHIN Function Created.
//***************************************************************************
BOOL SYM_EXPORT WINAPI RegistryCheckLastScanUpdateKey (void)
{
    auto    BOOL    bReturn     =   FALSE;
    auto    DWORD   dwType      =   0;
    auto    HKEY    hUpdateKey   =   0;
    auto    DWORD   dwDataSize;
    auto    char    szData [ SYM_MAX_PATH + 1 ] = { 0 };
    auto    LONG    lStatus;
    auto    LPSTR   lpVirScanKey        = NULL;

#ifndef SYM_VXD
    if (SYM_SYSTEM_WIN_NT == SystemGetWindowsType())
        lpVirScanKey = SZ_LASTSCAN_KEY_NT;
    else
#endif
        lpVirScanKey = SZ_LASTSCAN_KEY;

    lStatus = RegOpenKeyEx (
                            HKEY_LOCAL_MACHINE,
                            lpVirScanKey,
                            0,
                            KEY_ALL_ACCESS,
                            &hUpdateKey
                            );

    if ( lStatus == ERROR_SUCCESS )
        {
        dwDataSize  =   sizeof(szData);

        lStatus = RegQueryValueEx (
                            hUpdateKey,
                            SZ_LASTSCAN_UPDATE,
                            NULL,
                            &dwType,
                            (LPBYTE)szData,
                            &dwDataSize
                            );

        if ( lStatus == ERROR_SUCCESS )
            {
            if ( dwDataSize && *szData )
                {
                bReturn = TRUE;
                }
            }

        lStatus = RegCloseKey ( hUpdateKey );
        }

    return ( ( lStatus == ERROR_SUCCESS ) ? bReturn : FALSE );
}                                       // End RegistryCheckLastScanUpdateKey()

#endif // SYM_WIN32
