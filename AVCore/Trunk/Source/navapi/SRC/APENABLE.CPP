// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVAPI/VCS/apenable.cpv   1.0   22 Jun 1998 22:27:58   DALLEE  $
//
// Description:
//      Routines for enabling and disabling NAVAP.
//
// Contents:
//      NavapCommInit()
//      NavapCommDeinit()
//      NavapProtectProcess()
//      NavapUnprotectProcess()
//
// See Also:
//************************************************************************
// $Log:   S:/NAVAPI/VCS/apenable.cpv  $
// 
//    Rev 1.0   22 Jun 1998 22:27:58   DALLEE
// Initial revision.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "drvcomm.h"


//************************************************************************
// Defines from APCOMM.H and NAVAPCOM.H
//************************************************************************

#define NAVAP_VXD_COMM_NAME         _T("\\\\.\\NAVAP")
#define NAVAP_COMMUNICATION_NAME    _T("NAVAP")

#define szNAVAPGetEnabledDisabled   _T("_gretz1@4")
#define szNAVAPSetEnabledDisabled   _T("_gretz2@4")
#define szNAVAPProtectProcess       _T("_gretz3@0")
#define szNAVAPUnprotectProcess     _T("_gretz4@0")

enum tagVXD_ID_COMMANDS
    {
    ID_COMMAND_NO_COMMAND,
    ID_COMMAND_GET_VXD_VERSION,
    ID_COMMAND_GET_ENABLE_DISABLE,
    ID_COMMAND_GET_CMD_FROM_VXD,
    ID_COMMAND_AGENT_STARTED,
    ID_COMMAND_AGENT_TERMINATING,
    ID_COMMAND_VXD_AUTOLOADED,
    ID_COMMAND_ENABLE_VXD,
    ID_COMMAND_DISABLE_VXD,
    ID_COMMAND_UNLOAD_VXD,
    ID_COMMAND_UNLOAD_VXD_ON_SYSSHUTDOWN,
    ID_COMMAND_SHUTDOWN_IN_PROGRESS,
    ID_COMMAND_SHUTDOWN_ABORTED,
    ID_COMMAND_START_ASYNC_COMMANDS,
    ID_COMMAND_END_ASYNC_COMMANDS,
    ID_COMMAND_ADD_PROCESS_TO_PEL,
    ID_COMMAND_DELETE_PROCESS_FROM_PEL,
    ID_COMMAND_NAVAP_LOAD_CONFIG
    };

//---------------------------------------------------------------------------
//
// Define commands to be sent by NAVAP to user-mode applications, and
// commands to be sent by user-mode applications to NAVAP.  Both
// communication directions should share the DIOCTL_SYM_USER?? constants.
//
//---------------------------------------------------------------------------
#define NAVAP_COMM_GETVERSION        DIOCTL_SYM_USER00  // app   -> NAVAP
#define NAVAP_COMM_GETSTATUS         DIOCTL_SYM_USER01  // app   -> NAVAP
#define NAVAP_COMM_SETSTATUS         DIOCTL_SYM_USER02  // app   -> NAVAP
#define NAVAP_COMM_CONTROLLEDINIT    DIOCTL_SYM_USER03  // app   -> NAVAP
#define NAVAP_COMM_CONTROLLEDDEINIT  DIOCTL_SYM_USER04  // app   -> NAVAP
#define NAVAP_COMM_RELOADCONFIG      DIOCTL_SYM_USER05  // app   -> NAVAP
#define NAVAP_COMM_LOADAVAPI         DIOCTL_SYM_USER06  // app   -> NAVAP
#define NAVAP_COMM_UNLOADAVAPI       DIOCTL_SYM_USER07  // app   -> NAVAP
#define NAVAP_COMM_UNPROTECTPROCESS  DIOCTL_SYM_USER08  // app   -> NAVAP
#define NAVAP_COMM_PROTECTPROCESS    DIOCTL_SYM_USER09  // app   -> NAVAP
#define NAVAP_COMM_INFO              DIOCTL_SYM_USER10  // NAVAP -> app
#define NAVAP_COMM_SCANMOUNTEDDEVICE DIOCTL_SYM_USER11  // NAVAP -> app


//************************************************************************
// Global data
//************************************************************************

static BOOL         s_bUseNavapDll;

static HINSTANCE    s_hInstNavapDll;

typedef BOOL (WINAPI *PFNPROTECTPROCESS)();

static PFNPROTECTPROCESS s_lpfnProtectProcess;
static PFNPROTECTPROCESS s_lpfnUnprotectProcess;

static const TCHAR  s_szNAVAPDLL [] = _T("NAVAP32.DLL");
static const TCHAR  s_szNAVINSTALLKEY [] = _T("SOFTWARE\\Symantec\\InstalledApps");
static const TCHAR  s_szNAVNT [] = _T("NAVNT");
static const TCHAR  s_szNAV95 [] = _T("NAV95");


//************************************************************************
// Code
//************************************************************************

//*************************************************************************
// NavapCommInit()
//
// BOOL NavapCommInit ( )
//
// This routine checks for the standard new method for communicating with
// NAVAP (through the NAVAP32.DLL routines).
// If the DLL is present, the protect and unprotect functions from it 
// are loaded.
// After a successful result, call NavapCommDeinit() to unload the functions
// and DLL.
//
// Note: within NAVAPI, this should be called from NAVEngineInit().
// NavapCommDeinit() should be called from NAVEngineClose().
//
// Returns:
//      TRUE if new/standard AP communication method present and initialized.
//      FALSE otherwise.
//*************************************************************************
// 6/22/98 DALLEE, created.
//*************************************************************************

BOOL NavapCommInit ()
{
    auto    HKEY        hNavInstallKey;
    auto    LRESULT     lResult;

    auto    TCHAR       szNavPath [ SYM_MAX_PATH ];
    auto    DWORD       dwPathSize;
    auto    DWORD       dwType;

    auto    UINT        uOldErrorMode = SEM_FAILCRITICALERRORS;


    // Default to using old-style direct DeviceIoControl calls.

    s_bUseNavapDll = FALSE;


    // See if new preferred method is present:

    // Find installed version of NAV.

    lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                            s_szNAVINSTALLKEY,
                            0,
                            KEY_QUERY_VALUE,
                            &hNavInstallKey );

    if ( ERROR_SUCCESS == lResult )
    {
        dwPathSize = sizeof( szNavPath );

        lResult = RegQueryValueEx( hNavInstallKey,
                                   SYM_SYSTEM_WIN_NT == SystemGetWindowsType() ? s_szNAVNT : s_szNAV95,
                                   NULL,
                                   &dwType,
                                   (LPBYTE) szNavPath,
                                   &dwPathSize );

        RegCloseKey( hNavInstallKey );

        if ( ( ERROR_SUCCESS == lResult ) &&
             ( REG_SZ == dwType ) )
        {
            // Got NAV directory. Now load NAVAP32.DLL

            NameAppendFile( szNavPath, s_szNAVAPDLL );

            uOldErrorMode = SetErrorMode( SEM_FAILCRITICALERRORS );

            s_hInstNavapDll = LoadLibrary( szNavPath );

            if ( SEM_FAILCRITICALERRORS != uOldErrorMode )
                SetErrorMode( uOldErrorMode );

            if ( NULL != s_hInstNavapDll )
            {
                // Load protect/unprotect functions.

                s_lpfnProtectProcess   = (PFNPROTECTPROCESS) GetProcAddress( s_hInstNavapDll, szNAVAPProtectProcess );
                s_lpfnUnprotectProcess = (PFNPROTECTPROCESS) GetProcAddress( s_hInstNavapDll, szNAVAPUnprotectProcess );

                if ( s_lpfnProtectProcess && s_lpfnUnprotectProcess )
                {
                    s_bUseNavapDll = TRUE;
                }
                else
                {
                    s_lpfnProtectProcess = NULL;
                    s_lpfnProtectProcess = NULL;

                    FreeLibrary( s_hInstNavapDll );
                    s_hInstNavapDll = NULL;
                }
            }
        }
    }

    return ( TRUE );
} // NavapCommInit()


//*************************************************************************
// NavapCommDeinit()
//
// BOOL NavapCommDeinit ( )
//
// This routine frees any libraries or functions loaded during
// NavapCommInit().
//
// Note: within NAVAPI, this should be called from NAVEngineClose().
//
// Returns:
//      TRUE on success.
//      FALSE otherwise.
//*************************************************************************
// 6/22/98 DALLEE, created.
//*************************************************************************

BOOL NavapCommDeinit ()
{
    // If new preferred DLL was being used for NAVAP communication,
    // free library and clean up.

    if ( NULL != s_hInstNavapDll )
    {
        SYM_ASSERT( s_bUseNavapDll );
        SYM_ASSERT( s_lpfnProtectProcess );
        SYM_ASSERT( s_lpfnUnprotectProcess );

        s_bUseNavapDll = FALSE;

        s_lpfnProtectProcess = NULL;
        s_lpfnProtectProcess = NULL;

        FreeLibrary( s_hInstNavapDll );
        s_hInstNavapDll = NULL;
    }

    return ( TRUE );
} // NavapCommDeinit()


//*************************************************************************
// NavapProtectProcess()
//
// VOID NavapProtectProcess ( )
//
// This routine turns on NAVAP protection for the current thread (Win95)
// or process (WinNT) after protection has been disabled by a call
// to NavapUnprotectProcess().
//
// This routine should only be called after a call to NavapCommInit(),
// and before the matching call to NavapCommDeinit().
//
// NOTE: NAVAP keeps a count of protect/unprotect calls.
// For example, after three consecutive calls to NavapUnprotectProcess(),
// NavapProtectProcess() must be called three times to re-enable protection.
//
// Returns:
//      Nothing.
//*************************************************************************
// 6/22/98 DALLEE, created.
//*************************************************************************

VOID NavapProtectProcess()
{
    // First check for preferred communication method.

    if ( s_bUseNavapDll )
    {
        SYM_ASSERT( s_lpfnProtectProcess );

        s_lpfnProtectProcess();
    }
    else
    {
        // Use old-style communication.

        if ( SYM_SYSTEM_WIN_NT == SystemGetWindowsType() )
        {
            // Use driver comm to unprotect on WinNT.

            auto    DriverComm    commNAVAP;

            if ( commNAVAP.Open( NAVAP_COMMUNICATION_NAME ) )
            {
                commNAVAP.Send( NAVAP_COMM_PROTECTPROCESS );
            }
        }
        else
        {
            // Use DeviceIoControl for Win95

            auto    HANDLE  hNAVAP;
                                            // Open up a communications channel
                                            // with the Vxd

            hNAVAP = CreateFile( NAVAP_VXD_COMM_NAME, 0, 0, 0, OPEN_EXISTING, 0, 0 );

            if ( INVALID_HANDLE_VALUE != hNAVAP )
            {
                DeviceIoControl( hNAVAP, ID_COMMAND_DELETE_PROCESS_FROM_PEL, NULL, 0, NULL, 0, NULL, NULL );

                CloseHandle( hNAVAP );
            }
        }
    }
} // NavapProtectProcess()


//*************************************************************************
// NavapUnprotectProcess()
//
// VOID NavapUnprotectProcess ( )
//
// This routine turns off NAVAP protection for the current thread (Win95)
// or process (WinNT).
//
// This routine should only be called after a call to NavapCommInit(),
// and before the matching call to NavapCommDeinit().
//
// NOTE: NAVAP keeps a count of protect/unprotect calls.
// For example, after three consecutive calls to NavapUnprotectProcess(),
// NavapProtectProcess() must be called three times to re-enable protection.
//
// Returns:
//      Nothing.
//*************************************************************************
// 6/22/98 DALLEE, created.
//*************************************************************************

VOID NavapUnprotectProcess()
{
    // First check for preferred communication method.

    if ( s_bUseNavapDll )
    {
        SYM_ASSERT( s_lpfnUnprotectProcess );

        s_lpfnUnprotectProcess();
    }
    else
    {
        // Use old-style communication.

        if ( SYM_SYSTEM_WIN_NT == SystemGetWindowsType() )
        {
            // Use driver comm to unprotect on WinNT.

            auto    DriverComm    commNAVAP;

            if ( commNAVAP.Open( NAVAP_COMMUNICATION_NAME ) )
            {
                commNAVAP.Send( NAVAP_COMM_UNPROTECTPROCESS );
            }
        }
        else
        {
            // Use DeviceIoControl for Win95

            auto    HANDLE  hNAVAP;

            hNAVAP = CreateFile( NAVAP_VXD_COMM_NAME, 0, 0, 0, OPEN_EXISTING, 0, 0 );

            if ( INVALID_HANDLE_VALUE != hNAVAP )
            {
                DeviceIoControl( hNAVAP, ID_COMMAND_ADD_PROCESS_TO_PEL, NULL, 0, NULL, 0, NULL, NULL );

                CloseHandle( hNAVAP );
            }
        }
    }
} // NavapUnprotectProcess()





