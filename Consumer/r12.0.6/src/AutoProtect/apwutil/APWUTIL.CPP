// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/APWUTIL/VCS/apwutil.cpv   1.9   01 Oct 1998 13:04:50   tcashin  $
//
// Description:
//      Source for NAVAPW utility functions.
//
// Contains:
//      ApwActivate()
//      ApwDlgSet3d()
//      ApwMinimize()
//      ApwUpdateUI()
//      ApwShutdown()
//      ShellTrayPresent()
//      NewShellPresent()
//      ApwMinimize()
//      ApwCenterWindow()
//      ApwNotifyNavw()
//      ApwActivateNavw()
//      ApwGetCurrentNavWindow()
//      ApwGetNavwExePath()
//      GetWindowsPlatform()
//
// See Also:
//************************************************************************
// $Log:   S:/APWUTIL/VCS/apwutil.cpv  $
//
//    Rev 1.9   01 Oct 1998 13:04:50   tcashin
// Put back the check for Atomics navopts version number.
//
//    Rev 1.8   23 Sep 1998 17:25:14   tcashin
// Bumped the navopts version number for Barstow.
//
//    Rev 1.7   31 Jul 1998 10:11:32   tcashin
// Update the NAVOPTS.DAT version number for Atomic.
//
//    Rev 1.6   13 Mar 1998 17:30:34   tcashin
// Added ApwActivateNavw, ApwGetCurrentNavWindow, and ApwGetNavwExePath
// to support AP's launching and interaction with Navw.
//
//    Rev 1.5   11 Aug 1997 17:05:46   DALLEE
// Added magic parameters to APWM_SHUTDOWN message to protect against
// accidentally receiving and this message and shutting down Windows.
// Added ApwUpdateUI() to signal UI to resync dispalyed status of the driver.
//
//    Rev 1.4   27 Jun 1997 19:21:32   DALLEE
// Protect ApwNotifyNavw() against re-entrance.
// Also updated some comments.
//
//    Rev 1.3   20 Jun 1997 11:32:48   DBUCHES
// Bumped NAVOPTS header version.  Now uses NAV_VERSION_97.
//
//    Rev 1.2   11 Jun 1997 12:09:26   DALLEE
// Added GetWindowsPlatform() and ApwNotifyNavw().
//
//    Rev 1.1   05 Jun 1997 19:43:18   DALLEE
// In ApwCenterWindow(), replaced SetWindowPlacement() with SetWindowPos().
// SetWindowPlacement() had strange behaviour when used on main window
// launched minimized and not yet displayed.
//
//    Rev 1.0   02 Jun 1997 02:48:36   DALLEE
// Initial revision.
//************************************************************************

#define STRICT
#include "stdafx.h"
#include "platform.h"
#include <windowsx.h>
#include <tchar.h>
#include <crtdbg.h>

#include "apw.h"
#include "apwutil.h"
#include "apwcfg.h"
#include "resource.h"
#include "SimpleTimer.h"

#define INITIIDS
#include <initguid.h>
#define SIMON_INITGUID
#include <SIMON.h>
#define _INIT_COSVERSIONINFO
#include "SSOsinfo.h"
#include "NAVOptHelperEx.h"

#include "cltLicenseConstants.h"
#include "OptNames.h"
#include "InstOptsNames.h"
#include "NAVInfo.h"
#include "AVRESBranding.h"
#include "APWRES.H"

// PEP Headers
#define _INIT_V2LICENSINGAUTHGUIDS
#define _V2AUTHORIZATION_SIMON
#define INIT_V2AUTHORIZATION_IMPL
#define _DRM_V2AUTHMAP_IMPL
#define _SIMON_PEP_
#define _AV_COMPONENT_CLIENT
#include "ComponentPepHelper.h"
#include "PepMiddleMan.h"

CString _g_csModuleFileName;

#include "ccSettingsInterface.h"
#include "NavUIHelp.h"

#include "ccLib.h"

#include "ccLibDllLink.h"
#include "ccSymDelayLoader.h"


// ccLib delay loader
// This will allow the ccLib dll to delay load at run-time
ccSym::CDelayLoader g_DelayLoader;

#include "ccTrace.h"
#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("apwutil"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

#include "NAVError.h"
#include "ccModuleId.h"
#include "AVccModuleId.h"

#include "GlobalEvents.h"
#include "GlobalEvent.h"    // For CGlobalEvent
#include "SuiteOwnerHelper.h"

SIMON::CSimonModule _SimonModule;


//************************************************************************
// Local prototypes
//************************************************************************

static BOOL CALLBACK ApwDlgSet3dEnum ( HWND hWnd, LPARAM lParam );
static BOOL CALLBACK ApwNavwThrdWinEnum ( HWND hWnd, LPARAM lParam );


//************************************************************************
// Defines
//************************************************************************
#define     SZ_NAVW_WNDCLASS    _T("nav")
#define     SZ_SCAN_WNDCLASS    _T("ScanWindow")
#define     SZ_INTEGRATOR_CMDLINESWITCH  _T("/DAT:")
#define     SZ_INTEGRATOR_PROG           _T("\\NAVUI.NSI")
#define		SZ_NOSYSWORKS				_T("/nosysworks")

//************************************************************************
// Globals
//************************************************************************

#ifdef __cplusplus
extern "C"  {
#endif

HINSTANCE g_hUtilInst;

#ifdef __cplusplus
}
#endif

static  CRITICAL_SECTION    s_CritSecRpcClient;
static ccLib::CCriticalSection g_critTray;
static CSimpleTimer g_timerStartedNAVW;

//Resource loading
#include "..\apwutilres\resource.h"
#include "ccResourceLoader.h"
#define SYMC_RESOURCE_DLL _T("apwutil.loc")
CAtlBaseModule _Module;
::cc::CResourceLoader g_ResLoader(&_Module, SYMC_RESOURCE_DLL);

bool _InitResources()
{
	bool bRet = true;
	if(NULL == g_ResLoader.GetResourceInstance())
	{
		if(g_ResLoader.Initialize())
		{
			_Module.SetResourceInstance(g_ResLoader.GetResourceInstance());
		}
		else
		{
			CCTRACEE(_T("%s - Failed to Initialize resources."), __FUNCTION__);
			bRet = false;
		}
	}

	return bRet;
}

#define INIT_RESOURCES() \
		if( false == _InitResources() )\
	return SYMERR_INVALID_FILE;\

// SymInterface map for ccVerifyTrust static lib
SYM_OBJECT_MAP_BEGIN()
INIT_RESOURCES()
SYM_OBJECT_MAP_END()

CString g_csProductName;

//************************************************************************
// DLL junk
//************************************************************************

//************************************************************************
// DllMain()
//
// Returns:
//      TRUE    on success.
//      FALSE   otherwise.
//************************************************************************
// 05/27/1997 DALLEE, created.
//************************************************************************

BOOL WINAPI DllMain ( HINSTANCE   hInstDLL,
                      DWORD       dwReason,
                      LPVOID      lpvReserved )
{
    switch ( dwReason )
    {
    case DLL_PROCESS_ATTACH:
        // Performance Optimization: see MSDN docs on DisableThreadLibraryCalls() func.
        DisableThreadLibraryCalls((HMODULE)hInstDLL);

        // Save HINSTANCE on process attach.
        g_hUtilInst = hInstDLL;

        // Critical section for RPC communication to NAVW.
        InitializeCriticalSection( &s_CritSecRpcClient );
        break;

    case DLL_PROCESS_DETACH:
        // Critical section for RPC communication to NAVW.

        DeleteCriticalSection( &s_CritSecRpcClient );
        break;

    }

    return ( TRUE );
} // DllMain()


//************************************************************************
// Utiltiy functions
//************************************************************************

//************************************************************************
// ApwDlgSet3d()
//
// APWUTILAPI BOOL WINAPI ApwDlgSet3d (
//      HWND    hWnd );                 // Dialog to give 3d look.
//
// This routine will first check if the new Windows 4.0 GUI is present.
// If not (as in NT 3.51), it will enumerate all child dialog controls
// looking for new style groupboxes and sunken static controls to give them
// a 3d look, then call Ctl3dSubclassDlgEx().
//
// Returns:
//      TRUE    on success.
//      FALSE   otherwise.
//************************************************************************
// 05/23/1997 DALLEE, created.
//************************************************************************

APWUTILAPI BOOL WINAPI ApwDlgSet3d ( HWND hWnd )
{
	_InitResources();

	auto    BOOL    bSuccess;

    if ( NewShellPresent() )
    {
        // New style GUI is present, no need to do anything.

        bSuccess = TRUE;
    }
    else
    {
        // Need to add SS_BLACKFRAME to controls and subclass dialog for 3d.

        bSuccess = EnumChildWindows( hWnd, ApwDlgSet3dEnum, 0 );
    }

    return ( bSuccess );
} // ApwDlgSet3d()

static BOOL CALLBACK ApwDlgSet3dEnum ( HWND hWnd, LPARAM lParam )
{
    const   TCHAR   szStaticClass [] = _T("Static");
    const   TCHAR   szButtonClass [] = _T("Button");

    #define MAX_CLASS   ( max( sizeof(szStaticClass), sizeof(szButtonClass) ) / sizeof(TCHAR) + 1 )

    auto    DWORD   dwStyle;
    auto    TCHAR   szClass [ MAX_CLASS ];

    auto    RECT    rRect;
    auto    POINT   rPoint;
    auto    HWND    hParent;

    dwStyle = GetWindowStyle( hWnd );
    GetClassName( hWnd, szClass, sizeof( szClass ) / sizeof( TCHAR ) );

    // Set SS_BLACKFRAME for groupboxes.

    if ( ( BS_GROUPBOX == ( dwStyle & BS_GROUPBOX ) ) &&
         ( 0 == _tcsicmp( szButtonClass, szClass ) ) )
    {
        dwStyle |= SS_BLACKFRAME;
        SetWindowLong( hWnd, GWL_STYLE, dwStyle );
    }

    // For sunken static controls, need to create another child window with
    // a sunken border. ( Setting SS_BLACKFRAME on a static control would create
    // a sunken frame with no text displayed )

    if ( ( 0 == _tcsicmp( szStaticClass, szClass ) ) &&
         ( SS_SUNKEN == ( dwStyle & SS_SUNKEN ) ) )
    {
        hParent = GetParent( hWnd );

        GetWindowRect( hWnd, &rRect );

        // Klugery: make new frame larger by one unit per side
        // so we don't draw right against the text of the old control.

        if ( rRect.left )
            rRect.left--;
        if ( rRect.top )
            rRect.top--;
        rRect.right++;
        rRect.bottom++;

        rPoint.x = rRect.left;
        rPoint.y = rRect.top;

        ScreenToClient( GetParent( hWnd ), &rPoint );

        CreateWindow( szStaticClass,
                      _T(""),
                      WS_CHILD | WS_VISIBLE | SS_BLACKFRAME,
                      rPoint.x, rPoint.y,
                      rRect.right - rRect.left, rRect.bottom - rRect.top,
                      GetParent( hWnd ),
                      ( HMENU ) -1,
                      GetWindowInstance( hWnd ),
                      NULL );
    }

    return ( TRUE );
} // ApwDlgSet3dEnum()


//************************************************************************
// ShellTrayPresent()
//
// This routine returns whether or not the new shell tray window is
// currently present.
// Note that NAVAPW may start before the shell, so during execution
// the result of this function may change. Do not cache the result.
//
// Returns:
//  TRUE    if tray is currently present.
//  FALSE   otherise.
//************************************************************************
// 05/21/1997 DALLEE, created.
//************************************************************************

APWUTILAPI BOOL WINAPI ShellTrayPresent ()
{
	_InitResources();

	return ( NULL != FindWindow( "Shell_TrayWnd", NULL ) );
} // ShellTrayPresent()


//************************************************************************
// NewShellPresent()
//
// This routine returns whether or not the updated GUI from Windows
// versions >= 4.0 is present.
//
// Returns:
//  TRUE    if new style GUI is present.
//  FALSE   otherise.
//************************************************************************
// 05/22/1997 DALLEE, created.
//************************************************************************

APWUTILAPI BOOL WINAPI NewShellPresent ()
{
	_InitResources();

    auto    OSVERSIONINFO   rVersionInfo;
    auto    BOOL            bNewShell;

    // Initialize locals.

    bNewShell = FALSE;

    memset( &rVersionInfo, 0, sizeof( rVersionInfo ) );
    rVersionInfo.dwOSVersionInfoSize = sizeof( rVersionInfo );

    // See if we have Windows version 4.0 or greater.

    if ( GetVersionEx( &rVersionInfo ) )
    {
        bNewShell = rVersionInfo.dwMajorVersion >= 4;
    }

    return ( bNewShell );
} // NewShellPresent()


//************************************************************************
// GetWindowsPlatform()
//
// APWUTILAPI DWORD WINAPI GetWindowsPlatform ()
//
// This routine returns the Windows platform ID.
//
// Returns:
//  VER_PLATFORM_WIN32s	                Win32s on Windows 3.1.
//  VER_PLATFORM_WIN32_WINDOWS	        Win32 on Windows 95.
//  VER_PLATFORM_WIN32_NT	            Win32 on Windows NT.
//  -1 on error (unexpected).
//************************************************************************
// 06/11/1997 DALLEE, created.
//************************************************************************

APWUTILAPI DWORD WINAPI GetWindowsPlatform ()
{
	_InitResources();

    auto    OSVERSIONINFO   rVersionInfo;
    auto    DWORD           dwPlatform;

    // Initialize locals.

    dwPlatform = ( DWORD ) -1;

    memset( &rVersionInfo, 0, sizeof( rVersionInfo ) );
    rVersionInfo.dwOSVersionInfoSize = sizeof( rVersionInfo );

    // Retrieve platform ID from version info.

    if ( GetVersionEx( &rVersionInfo ) )
    {
        dwPlatform = rVersionInfo.dwPlatformId;
    }

    return ( dwPlatform );
} // GetWindowsPlatform()


//************************************************************************
// ApwActivate()
//
// APWUTILAPI BOOL WINAPI ApwActivate ()
//
// This routine restores and activates the NAVAPW main window.
//
// This restores the current process's main window -- activation of
// previous instances is handled through FindWindow() and
// PostMessage( APWM_ACTIVATE ).
//
// Returns:
//      TRUE on success.
//      FALSE otherwise.
//************************************************************************
// 05/18/1997 DALLEE, created.
//************************************************************************

APWUTILAPI BOOL WINAPI ApwActivate ()
{
	_InitResources();

    auto    CApwConfig  *pConfig;
    auto    HWND        hMainWnd;

    // Get main window.

    pConfig = ApwGetConfig();
    hMainWnd = pConfig->GetMainWnd();

    // Restore window and put in foreground.

    ShowWindow( hMainWnd, SW_RESTORE );

    return ( SetForegroundWindow( hMainWnd ) );
} // ApwActivate()

//************************************************************************
// IsCfgWizFinished()
//
// BOOL IsCfgWizFinished(BOOL& bFinished)
//
// This reads the value "CFGWIZ:Finished" in cfgwiz.dat to determine
// if CfgWiz.exe has been run.
//
// Returns:
//			TRUE if successful reading the value.
//			FALSE otherwise.
//			bFinished = TRUE if CFGWIZ:Finished != 0
//			bFinished = FALSE otherwise.
//
//************************************************************************
APWUTILAPI BOOL WINAPI IsCfgWizFinished(BOOL& bFinished)
{
	_InitResources();

	BOOL bSuccess = FALSE;
	StahlSoft::HRX hrx;
	CNAVOptFileEx CfgWizOpts;

	try
	{
		// Load the CfgWiz.dat.

		CString csCfgwizDat;

		if(SuiteOwnerHelper::GetOwnerCfgWizDat(csCfgwizDat))
		{
			if( !CfgWizOpts.Init(csCfgwizDat, FALSE) )
				hrx << E_FAIL;

			DWORD dwValue = 0;

			if( FAILED(CfgWizOpts.GetValue(InstallToolBox::CFGWIZ_Finished, dwValue, 0)) )
				hrx << E_FAIL;
			bFinished = (dwValue != 0);

			// All good
			bSuccess = TRUE;
		}
	}
	catch(...)
	{
	}

	return bSuccess;
}	// IsCfgWizFinished()

//************************************************************************
// ApwActivateNavw()
//
// APWUTILAPI BOOL WINAPI ApwActivateNavw ()
//
// This launches or restores the NAVW -- activation of previous
// instances is handled through FindWindowEx and GetLastActivePopup()
//
// See warning re: bug in GetLastActivePopup()
//
// Returns:
//      TRUE on success.
//      FALSE otherwise.
//************************************************************************
// 03/10/1998 TCASHIN, created.
// 05/25/1999 GWESTER, modified for compatibility with new integrator main
//            window. When NAV is launched through the tray icon, we will
//            now only launch the SWPLUGIN.NSI using ShellExecuteEx(). We
//            don't care anymore about the previous code that tried to detect
//            that the NAV window was already open; we will simply launch the
//            .NSI and let it handle its own multiple instance resolution. (It
//            doesn't matter if the scan window is open anymore, since
//            scanning is a seperate command-line launched process that the
//            integrator window doesn't wait on.)
// 07/08/1999 GWESTER, modified so that we launch the integrator by calling
//            nmain.exe directly; we no longer depend upon the .NSI file
//            association.
//************************************************************************
static BOOL _g_ReentranceGuardApwActivateNavw = FALSE;
APWUTILAPI BOOL WINAPI ApwActivateNavw ()
{
	_InitResources();

   ccLib::CSingleLock lock ( &g_critTray, INFINITE, FALSE);
    if(_g_ReentranceGuardApwActivateNavw)
    {
        CCTRACEI ( "ApwActivateNavw - ReEntrance on same thread = quiting function.");
        return FALSE;
    }

    class CBOOLToggle
    {
        BOOL* m_pbData;
    public:
        CBOOLToggle(BOOL* pbData)
        {
            m_pbData = pbData;
        }
        ~CBOOLToggle()
        {
            *m_pbData = !(*m_pbData);
        }
    };
    _g_ReentranceGuardApwActivateNavw = TRUE;
    CBOOLToggle smToggle(&_g_ReentranceGuardApwActivateNavw);

    // Don't let the user try to start two Integrators within 5 secs
    // of each other.
    //
    g_timerStartedNAVW.Stop(); // doesn't actually stop, gotta fix the name...
    if (g_timerStartedNAVW.GetElapsedTime () > 0 && g_timerStartedNAVW.GetElapsedTime () < 5000 )
    {
        CCTRACEI ( "ApwActivateNavw - user is spamming double click %d", g_timerStartedNAVW.GetElapsedTime ());
        return FALSE;
    }
    else
        CCTRACEI ("ApwActivateNavw - allowing launch %d", g_timerStartedNAVW.GetElapsedTime ());

    auto    BOOL                bAbort = TRUE;
	auto	BOOL				bCfgwizFinished = TRUE;

    auto    TCHAR               szNavwExePath [SYM_MAX_PATH];
    auto    TCHAR               szParameters [SYM_MAX_PATH];

    auto    SHELLEXECUTEINFO    NavwStartInfo = {0};

    auto    LPCTSTR             szNavKey = _T("Software\\Symantec\\InstalledApps");
    auto    LPCTSTR             szNMainKey = _T("Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\nmain.exe");
    auto    LPCTSTR             lpszExeKey = NULL;
    auto    LPCTSTR             szValue = _T("NAV");

    TCHAR      szValueBuf[SYM_MAX_PATH];
    DWORD      dwValueBufLen = sizeof (szValueBuf);
    TCHAR      szNMainPathBuf[SYM_MAX_PATH];
    DWORD      dwNMainPathBufLen = sizeof (szNMainPathBuf);
    HKEY       hBaseKey;
    DWORD      dwValueType;

    STRCPY (szNavwExePath,"");
    STRCPY (szParameters,"");

    lpszExeKey = szNavKey;

    //  Get the path to the integrator
    if (ERROR_SUCCESS == RegOpenKeyEx (HKEY_LOCAL_MACHINE, szNMainKey, 0, KEY_READ, &hBaseKey))
    {
        if (ERROR_SUCCESS == RegQueryValueEx (hBaseKey, "", 0, &dwValueType, (LPBYTE)szNMainPathBuf, &dwNMainPathBufLen))
        {
            if (dwValueType == REG_SZ && dwValueBufLen )
            {
            bAbort = FALSE;
            }
        }

		RegCloseKey (hBaseKey);
    }

    if ( bAbort )
    {
        CCTRACEE ("ApwActivateNavw - failed getting Integrator path");
        return FALSE; // Will cause nothing to happen
    }

    // Now, szNMainPathBuf contains the full path of the integrator (NMAIN.EXE).
    // Reset some variables:
    hBaseKey = NULL;
    dwValueType = NULL;

    //  Get the OtherSwitches value from the CmdLines key.
	bAbort = TRUE;
    if (ERROR_SUCCESS == RegOpenKeyEx (HKEY_LOCAL_MACHINE, lpszExeKey, 0, KEY_READ, &hBaseKey))
    {
        if (ERROR_SUCCESS == RegQueryValueEx (hBaseKey, szValue, 0, &dwValueType, (LPBYTE)szValueBuf, &dwValueBufLen))
        {
            if (dwValueType == REG_SZ && dwValueBufLen)
            {
            _tcscpy (szNavwExePath, szValueBuf);
			bAbort = FALSE;
            }
        }

		RegCloseKey (hBaseKey);
    }

    if ( bAbort )
    {
        CCTRACEE ("ApwActivateNavw - failed getting OtherSwitches");
        return FALSE; // Will cause nothing to happen
    }

	// Check to see if config wizard has been run.
	if(true)	// scope bSuccess
	{
		BOOL bSuccess = IsCfgWizFinished(bCfgwizFinished);
		bAbort = !bSuccess;
	}

    if ( bAbort )
    {
        CCTRACEW ("ApwActivateNavw - ConfigWiz not finished");
        return FALSE; // Will cause nothing to happen
    }

	if( bCfgwizFinished )
	{
		// Check to see if NAV is part of NIS suite.
		TCHAR szNisPlugParam[MAX_PATH*2] = {0};
		DWORD dwSize = sizeof(szNisPlugParam) / sizeof(TCHAR);

        CApwConfig* pConfig = ApwGetConfig();
        CApwCmd* pCmd = NULL;
        bool bAggregated = false;

        if( pConfig )
        {
            pCmd = pConfig->GetCmd();
            if(pCmd)
            {
                bAggregated = pCmd->IsAggregated() ? true : false;
                CCTRACEI("ApwActivateNavw() - bAggregated = %d",bAggregated);
            }
            else
                CCTRACEE("ApwActivateNavw() - Failed to get the AP Cmd object");
        }
        else
            CCTRACEE("ApwActivateNavw() - Failed to get the AP Config object");

		if(SUCCEEDED(NAVUIHelp::BuildNisPluginParams(szNisPlugParam, dwSize, &bAggregated)))
		{
			// Launch NIS UI with NAV Status panel active.
            CCTRACEI("ApwActivateNavw() - Launching NIS with NAV status panel active. Paramaters: %s", szNisPlugParam);
			_tcscpy(szParameters, szNisPlugParam);
		}
		else
		{
            CCTRACEI("ApwActivateNavw() - PEP started");
            STAHLSOFT_HRX_TRY(hr)
            {
			    // Launch NAV UI as standalone product.

				CNAVInfo NAVInfo;
				CString csTitle;
				CString csStatusPanel;
				CString csAVRES;

			    STRCPY (szParameters, SZ_INTEGRATOR_CMDLINESWITCH);
			    STRCAT (szParameters, szNavwExePath);
			    // Append the executable name
			    STRCAT (szParameters, SZ_INTEGRATOR_PROG);

                CCTRACEI("ApwActivateNavw() - Launching NAV status panel (No NIS). Paramaters: %s", szParameters);

			    DJSMAR_LicenseType licType = DJSMAR_LicenseType_Violated;

                // Getting licensing and subscription properties needed to enable product features
                CPEPClientBase pepBase;
                hrx << pepBase.Init();
                pepBase.SetContextGuid(clt::pep::CLTGUID);
                pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT);
                pepBase.SetPolicyID(clt::pep::POLICY_ID_QUERY_LICENSE_RECORD);
                pepBase.SetCollectionDWORD(clt::pep::SZ_PROPERTY_DISPOSITION,clt::pep::DISPOSITION_NONE);
                pepBase.SetCollectionDWORD(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION,SUBSCRIPTION_STATIC::DISPOSITION_NONE);

                hrx << pepBase.QueryPolicy();
                // Getting licensing properties
                hrx << pepBase.GetValueFromCollection(DRM::szDRMLicenseType, (DWORD &)licType, DJSMAR_LicenseType_Violated);

			    if(licType == DJSMAR_LicenseType_Rental)
			    {
				    STRCAT (szParameters, _T(" "));
				    STRCAT (szParameters, SZ_NOSYSWORKS);
			    }

				// Now tell NMain to give focus to the NAV panel, since
				// Norton Protection Center is the default home

				// Setup path to AVRES.DLL
				csAVRES.Format(_T("%s\\AVRES.DLL"), NAVInfo.GetNAVDir());

				StahlSoft::CSmartModuleHandle shMod(LoadLibrary(csAVRES));
				hrx << (((HANDLE) shMod != (HANDLE)NULL) ? S_OK : CO_E_DLLNOTFOUND);

				// Get names of NAV main UI title and status panel.
				csTitle.LoadString(shMod, IDS_PRODUCTNAME);
				csStatusPanel.LoadString(shMod, IDS_STATUS_CATEGORYTITLE);

				csTitle.Remove(_T('&'));
				csStatusPanel.Remove(_T('&'));

				STRCAT (szParameters, _T(" "));
				STRCAT (szParameters, _T("/goto:"));
				STRCAT (szParameters, csTitle.GetBuffer());
				STRCAT (szParameters, _T("\\"));
				STRCAT (szParameters, csStatusPanel.GetBuffer());
				csTitle.ReleaseBuffer();
				csStatusPanel.ReleaseBuffer();
            }
            STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
            CCTRACEI("ApwActivateNavw() - PEP done");
        }
	}
	else
	{
		// Launch ConfigWiz
		CString csCfgwiz;
		CString csParam;

		if(SuiteOwnerHelper::GetOwnerCfgWizCmdLine(csCfgwiz, csParam))
		{
			CString csPath(csCfgwiz);
			PathRemoveFileSpec(csPath.GetBuffer());
			csPath.ReleaseBuffer();

			_tcscpy(szNavwExePath, csPath);
			_tcscpy(szNMainPathBuf, csCfgwiz);
			_tcscpy(szParameters, csParam);

			CCTRACEI(_T("ApwActivateNavw - CurDir: %s"), szNavwExePath);
			CCTRACEI(_T("ApwActivateNavw - App: %s"), szNMainPathBuf);
			CCTRACEI(_T("ApwActivateNavw - Param: %s"), szParameters);
		}
	}

    NavwStartInfo.cbSize        = sizeof(NavwStartInfo);
    NavwStartInfo.fMask         = 0;
    NavwStartInfo.lpVerb        = "Open";
    NavwStartInfo.lpFile        = szNMainPathBuf;
    NavwStartInfo.lpParameters  = szParameters;
    NavwStartInfo.nShow         = SW_SHOW;
    NavwStartInfo.lpDirectory   = szNavwExePath;

    if ( ShellExecuteEx(&NavwStartInfo))
    {
        CCTRACEI ( "ApwActivateNavw - NAVUI started %d %s %s", g_timerStartedNAVW.GetElapsedTime(), szNMainPathBuf, szParameters);
        g_timerStartedNAVW.Start();
        return TRUE;
    }
    else
    {
        CCTRACEE ( "ApwActivateNavw - NAVUI failed to start - %s , %s", szNMainPathBuf, szParameters);
        g_timerStartedNAVW.Reset();
        return FALSE;
    }
} // ApwActivateNavw()


//************************************************************************
// ApwGetCurrentNavWindow()
//
// APWUTILAPI BOOL WINAPI MainDlgGetCurrentNavWindow (lpMainWindow, lpCurrentWindow)
//
// This return the handle of Navw's main window and
// the handle of the currently open window (dialog, etc.).
//
// See warning re: bug in GetLastActivePopup()
//
// Returns:
//      TRUE if any Navw windows are open
//      FALSE otherwise.
//************************************************************************
// 03/10/1998 TCASHIN, created.
//************************************************************************

APWUTILAPI BOOL WINAPI ApwGetCurrentNavWindow (LPHANDLE lpMainWindow,
                                               LPHANDLE lpCurrentWindow)
{
	_InitResources();

	auto    HWND    hNavwMainWnd  = NULL;
    auto    HWND    hNavwScanWnd  = NULL;
    auto    HWND    hCurNavwPopupWnd = NULL;
    auto    BOOL    bResult = FALSE;

    // Look for Navw's Main window
    hNavwMainWnd = ::FindWindowEx(NULL,NULL, SZ_NAVW_WNDCLASS, NULL);

    if ( hNavwMainWnd == NULL )
        return ( bResult );
    else
    {
        // We have a Navw main window.
        *lpMainWindow = hNavwMainWnd;
        bResult = TRUE;

        // Look for Navw's interesting windows.
        hCurNavwPopupWnd = GetLastActivePopup ( hNavwMainWnd );
        *lpCurrentWindow = hCurNavwPopupWnd;

        // Warning ... Warning ... Warning
        //
        // This is here to work around a bug in GetLastActivePopup(). If the
        // last active popup is currently minimized then the HWND returned is
        // the parent.
        //
        // This only happens in the case of ScanWindow! So look at the windows owned
        // by the current thread until ScanWindow is found.
        //
        // This really sucks...
        //

        if ( hCurNavwPopupWnd == hNavwMainWnd )
        {
            ::EnumThreadWindows ( GetWindowThreadProcessId(hNavwMainWnd,NULL),
                                  ApwNavwThrdWinEnum,
                                  MAKELPARAM(LOWORD(&hNavwScanWnd), HIWORD(&hNavwScanWnd)));

            if ( hNavwScanWnd )
            {
                *lpCurrentWindow = hNavwScanWnd;
            }
        }

        return ( bResult );

    }
} // ApwGetCurrentNavWindow()


//************************************************************************
// ApwNavwThrdWinEnum()
//
// static BOOL CALLBACK ApwNavwThrdWinEnum ( HWND hWnd, LPARAM lParam )
//
// This checks to see if a given window handle is our ScanWindow
// class. If so, return the HWND to the caller.
//
// Returns:
//      TRUE on to continue Window enumeration
//      FALSE to stop enumerating.
//************************************************************************
// 03/10/1998 TCASHIN, created.
//************************************************************************

static BOOL CALLBACK ApwNavwThrdWinEnum ( HWND hWnd, LPARAM lParam )
{
    auto    TCHAR            szClass [ SYM_MAX_PATH ];
    auto    HWND            *lphScanWnd;

    GetClassName( hWnd, szClass, sizeof( szClass ) / sizeof( TCHAR ) );

    if ( 0 == _tcsicmp( SZ_SCAN_WNDCLASS, szClass ) )
    {
    	lphScanWnd = (HWND *)MAKELP(HIWORD(lParam), LOWORD(lParam));
        *lphScanWnd = hWnd;
        return (FALSE);
    }
    else
        return (TRUE);

} //ApwNavwThrdWinEnum


//************************************************************************
// ApwShutdown()
//
// APWUTILAPI BOOL WINAPI ApwShutdown ()
//
// This routine tells NAVAPW to signal a shutdown. It may be called at any
// from any thread.
//
// Currently implemented as a PostMessage() to NAVAPW's main window.
//
// Returns:
//      TRUE if message was sent (doesn't guarantee shutdown will occur).
//      FALSE if message was not sent.
//************************************************************************
// 05/18/1997 DALLEE, created.
//************************************************************************

APWUTILAPI BOOL WINAPI ApwShutdown ()
{
	_InitResources();

    auto    CApwConfig  *pConfig;
    auto    HWND        hMainWnd;

    // Get main window.

    pConfig = ApwGetConfig();
    hMainWnd = pConfig->GetMainWnd();

    // Send message to have NAVAPW shut down windows.
    // Klugey use of PostMessage() will allow current thread to finish
    // processing AP callback if called from DriverComm ServiceCallback().
    // Need to modify this to truly separate AP service from UI.

    return ( PostMessage( hMainWnd,
                          APWM_SHUTDOWN,
                          APWM_SHUTDOWN_MAGIC_WPARAM,
                          APWM_SHUTDOWN_MAGIC_LPARAM ) );
} // ApwShutdown()


//************************************************************************
// ApwMinimize()
//
// APWUTILAPI BOOL WINAPI ApwMinimize ()
//
// This routine minimizes or hides the NAVAPW main window based on
// current options and presence of shell tray window.
//
// Returns:
//      TRUE on success.
//      FALSE otherwise.
//************************************************************************
// 05/28/1997 DALLEE, created.
//************************************************************************

APWUTILAPI BOOL WINAPI ApwMinimize ()
{
	_InitResources();

    auto    CApwConfig  *pConfig;
    auto    int         nCmdShow;

    // Get main window.

    pConfig = ApwGetConfig();

    // Figure out what to do with main window.

    if ( !ShellTrayPresent() && pConfig->GetShowIcon() )
    {
        nCmdShow = SW_MINIMIZE;
    }
    else
    {
        nCmdShow = SW_HIDE;
    }

    return ( ShowWindow( pConfig->GetMainWnd(), nCmdShow ) );
} // ApwMinimize()


//************************************************************************
// ApwCenterWindow()
//
// APWUTILAPI BOOL WINAPI ApwCenterWindow (
//      HWND    hWnd,                   // Window to center
//      HWND    hParent );              // Parent window to center on,
//                                      //   NULL to center on desktop.
//
// Yes, yet another instance of window centering code.
//
// This routine centers a window relative to its parent. If there is no
// parent, or the parent is hidden or minimized, the window will be
// centered on the desktop.
//
// Returns:
//      TRUE on success.
//      FALSE otherwise.
//************************************************************************
// 05/28/1997 DALLEE, created.
//************************************************************************

APWUTILAPI BOOL WINAPI ApwCenterWindow ( HWND hWnd, HWND hParent )
{
	_InitResources();

    auto    RECT            rWindow;
    auto    RECT            rParent;
    auto    LONG            lWidth;
    auto    LONG            lHeight;

    auto    BOOL            bSuccess;

    // Initialize locals.

    bSuccess = FALSE;

    memset( &rWindow, 0, sizeof( rWindow ) );

    // Find position of parent window.

    if ( ( NULL == hParent ) ||
         ( IsIconic( hParent ) ) ||
         ( FALSE == IsWindowVisible( hParent ) ) )
    {
        // If no parent or parent is hidden or invisible, center on desktop.

        hParent = GetDesktopWindow();
    }

    if ( FALSE == GetWindowRect( hParent, &rParent ) )
        goto BailOut;

    // Find size of window to center.

    if ( FALSE == GetWindowRect( hWnd, &rWindow ) )
        goto BailOut;

    lWidth  = rWindow.right  - rWindow.left;
    lHeight = rWindow.bottom - rWindow.top;

    // New postion is ( center of parent ) - ( 1/2 size of window )

    rWindow.left = ( ( rParent.left + rParent.right  ) - ( lWidth  ) ) / 2;
    rWindow.top  = ( ( rParent.top  + rParent.bottom ) - ( lHeight ) ) / 2;

    if ( 0 > rWindow.left )
        rWindow.left = 0;

    if ( 0 > rWindow.top )
        rWindow.top = 0;

    // Move that window.

    bSuccess = SetWindowPos( hWnd, NULL,
                             rWindow.left,
                             rWindow.top,
                             lWidth,
                             lHeight,
                             SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOREDRAW );

BailOut:
    return ( bSuccess );
} // ApwCenterWindow()


//************************************************************************
// ApwNotifyNavw()
//
// APWUTILAPI BOOL WINAPI ApwNotifyNavw (
//      DWORD    dwMessage );           // Message to send
//
// This routine sends a command message to NAVW.
// Currently implemented as a link to NAVW's RPC server.
//
// Possible commands/messages are:
//      UWM_NAVAP_ACTIVATING
//
// Returns:
//  TRUE    on success.
//  FALSE   otherwise.
//************************************************************************
// 06/11/1997 DALLEE, created.
//************************************************************************

APWUTILAPI BOOL WINAPI ApwNotifyNavw ( DWORD dwMessage )
{
	_InitResources();

#if 0
	auto    CLinkClient         client;
    auto    BOOL                bSuccess;

    // Initialize locals.

    bSuccess = FALSE;

    // Protect against re-entrancy -- CLinkClient utilizes some
    // statics for RPC binding info.

    EnterCriticalSection( &s_CritSecRpcClient );

    // Connect to NAVW's RPC server. Address empty for local machine.

    if ( ( client.SetProtocol( NAVW_PROTOCOL,
                               _T(""),
                               NAVW_ENDPOINT ) ) &&
         ( client.Connect() ) )
    {
        // Send command message.

        if ( client.SendCommand( dwMessage ) )
        {
            bSuccess = TRUE;
        }

        client.DisConnect();
    }

    LeaveCriticalSection( &s_CritSecRpcClient );

    return ( bSuccess );
#else
	// TODO: This routine is no longer needed.  What really needs to happen
	// is that the apw95cmd module needs to pulse the NAVAP_STARTED_EVENT.
	return TRUE;
#endif
} // ApwNotifyNavw()


//************************************************************************
// ApwUpdateUI()
//
// APWUTILAPI BOOL WINAPI ApwUpdateUI ()
//
// This routine notifies the NAVAPW UI to resync its UI with the current
// state of the driver.
// Currently used after the service portion does an automatic reload
// of definitions (on error the UI needs to reflect a disabled state).
//
// Returns:
//  TRUE    if message sent.
//  FALSE   on error - agent not present or couldn't send message..
//************************************************************************
// 08/11/1997 DALLEE, created.
//************************************************************************

APWUTILAPI BOOL WINAPI ApwUpdateUI ( )
{
	_InitResources();

    auto    HWND        hWnd;
    auto    BOOL        bSuccess;

    // Initialize locals.

    bSuccess = FALSE;

    // Find Agent window.

    hWnd = FindWindow( APW_CLASS_NAME, NULL );

    // Send update message.

    if ( NULL != hWnd )
    {
        bSuccess = PostMessage( hWnd, APWM_UPDATE_UI, 0, 0 );
    }

    return ( bSuccess );
} // ApwUpdateUI()


//************************************************************************
// ApwGetNavwExePath
//
// Purpose:
//    Set lpszNavw to the full path to NAVW32/NAVWNT based on platform
//
// Parameters:
//    lpszNavw  [out] - The full path to NAV.
//    lpszDir   [out] - The directory portion of lpszNavw.
//
//************************************************************************
// 05/28/1997 JBRENNA - Function Created
// 03/12/1998 TCASHIN - Moved from ApwNtCmd.cpp
// 05/25/1999 GWESTER - Changed to use Installed Apps key, which is universal
//            between 9X & NT. Since this key only gives us the path, we
//            append the hardcoded .EXE name of the NAV integrator.
//************************************************************************

APWUTILAPI void WINAPI ApwGetNavwExePath ( LPSTR lpszNavw, LPSTR lpszDir )
{
	_InitResources();

//    LPCTSTR    szNavNTKey   = _T("Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\NAVWNT.EXE");
//    LPCTSTR    szNav95Key   = _T("Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\NAVW32.EXE");
    LPCTSTR    szNavKey   = _T("Software\\Symantec\\InstalledApps");
    LPCTSTR    lpszExeKey   = NULL;
//    LPCTSTR    szValue      = _T("");
    LPCTSTR    szValue      = _T("NAV");

    TCHAR      szValueBuf[SYM_MAX_PATH];
    DWORD      dwValueBufLen = sizeof (szValueBuf);
    HKEY       hBaseKey;
    DWORD      dwValueType;

    // Empty the strings.
    *lpszNavw = '\0';
    *lpszDir  = '\0';

//---------Code disabled on 5/25/99---------------------------------------------
//    //  Use the reg key for the platform we're on.
//    if ( GetWindowsPlatform() == VER_PLATFORM_WIN32_NT )
//        lpszExeKey = szNavNTKey;
//    else
//        lpszExeKey = szNav95Key;
//---------End of Code disabled on 5/25/99--------------------------------------

    lpszExeKey = szNavKey;

    //  Get the OtherSwitches value from the CmdLines key.
    if (ERROR_SUCCESS == RegOpenKeyEx (HKEY_LOCAL_MACHINE, lpszExeKey, 0, KEY_READ|KEY_WRITE, &hBaseKey))
    {
        if (ERROR_SUCCESS == RegQueryValueEx (hBaseKey, szValue, 0, &dwValueType, (LPBYTE)szValueBuf, &dwValueBufLen))
        {
            if (dwValueType == REG_SZ && dwValueBufLen)
                _tcscpy (lpszNavw, szValueBuf);
        }
    }

    RegCloseKey (hBaseKey);

//---------Code disabled on 5/25/99---------------------------------------------
//    // If we were successful at setting up lpszNavw, copy the path portion
//    // to lpszDir.
//    if (*lpszNavw)
//    {
//        LPSTR lpszCurSrc  = lpszNavw;
//        LPSTR lpszCurDest = lpszDir;
//        LPSTR lpszLastDir = lpszDir;
//        int   iChar;
//
//        while (*lpszCurSrc != '\0')
//        {
//            iChar = *lpszCurSrc;
//
//            // Copy the character.
//            *lpszCurDest = *lpszCurSrc;
//            ++lpszCurSrc;
//            ++lpszCurDest;
//
//            if (isleadbyte (iChar))
//                continue;
//
//            // Now, we are guaranteed that the character is SBCS.
//
//            // Memorize the position of the last \ in the destination string.
//            if (iChar == '\\')
//                lpszLastDir = (lpszCurDest - 1);
//        }
//
//        // Terminate the lpszDir string at the last \ character. (Does not
//        // include the \ character).
//        *lpszLastDir = '\0';
//    }

    // That previous section of disabled code was all about extracting the path
    // from the full executable path. We don't need to do this, since the info
    // we get from the InstalledApps key is only the path info:
    _tcscpy (lpszDir, lpszNavw);

    // Append the executable name
    _tcscat (lpszNavw, SZ_INTEGRATOR_PROG);

} // ApwGetNavwExePath


///////////////////////////////////////////////////////////////////////////////////////
//
// ApwErrorMessageBox - This will display a CED error dialog loading the
//                      error message from the resources of the instance handle
//                      passed in. If the instance is NULL apwutil resources
//                      will be used.
//
///////////////////////////////////////////////////////////////////////////////////////
APWUTILAPI HRESULT WINAPI ApwErrorMessageBox(HINSTANCE hInst, long nErrorId)
{
	_InitResources();

	StahlSoft::HRX hrx;
	HRESULT hr = S_OK;
	CString csText;

    // Use our resources if a handle is not passed in
    if(hInst == NULL)
        hInst = g_hUtilInst;

	try
	{
		if(g_csProductName.IsEmpty())
		{
			CBrandingRes BrandRes;
			g_csProductName = BrandRes.ProductName();
		}

		CString csFormat;
		csFormat.LoadString(_Module.GetResourceInstance(), nErrorId);
		csText = csFormat;

		// Format error message with product name
		switch(nErrorId)
		{
		case IDS_ERR_DEFS_REVERTED:
		case IDS_ERR_NO_AUTH_DEFS:
			csText.Format(csFormat, g_csProductName);
			break;

		default:
			break;
		}

		// Debug Log this error first
		CCTRACEE(csText);

		// Get a NAVError object
		CComBSTR bstrText = csText;
		CComBSTR bstrNAVErr(_T("NAVError.NAVCOMError"));
		CComPtr <INAVCOMError> spNavError;

		// Create, Populate, Log, and Display the error
		hrx << spNavError.CoCreateInstance(bstrNAVErr, NULL, CLSCTX_INPROC_SERVER);
		hrx << spNavError->put_ModuleID(AV_MODULE_ID_NAVAPW32);		// Module ID of AV_MODULE_ID_NAVOPTREFRESH
		hrx << spNavError->put_ErrorID(nErrorId); // Error ID is the resource ID for the error message
		hrx << spNavError->put_Message(bstrText);
		hrx << spNavError->LogAndDisplay(0);
	}
	catch(_com_error& e)
	{
		hr = e.Error();
        CCTRACEE(_T("APWUTIL::ErrorMessageBox() - Com Error = %s. HRESULT = 0x%X"),e.ErrorMessage(), hr);
	}
	catch(...)
	{
        CCTRACEE(_T("APWUTIL::ErrorMessageBox() - Unknown Exception"));
		hr = E_UNEXPECTED;
	}

	if(FAILED(hr) && !csText.IsEmpty())
	{
		// If failed, display the error in the form of win32 message box.
		::MessageBox(NULL, csText, g_csProductName, MB_ICONERROR);
	}

	return hr;
}

APWUTILAPI void WINAPI ApwFireDefUpdateEvent( )
{
	_InitResources();

   // Notify that the defs may have changed.
	CGlobalEvent eventDefs;

	eventDefs.Create( SYM_REFRESH_VIRUS_DEF_STATUS_EVENT );
	if ( eventDefs )
		::PulseEvent ( eventDefs );
}
