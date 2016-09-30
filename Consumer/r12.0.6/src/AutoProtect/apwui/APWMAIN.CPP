// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/APWUI/VCS/apwmain.cpv   1.26   25 Aug 1998 19:22:00   CEATON  $
//
// Description:
//      Source for NAVAPW main window.
//
// Contains:
//      WinMain()
//      ApwMainWndProc()
//
// See Also:
//************************************************************************
// $Log:   S:/APWUI/VCS/apwmain.cpv  $
// 
//    Rev 1.26   25 Aug 1998 19:22:00   CEATON
// Added modification to disable NAVAP.VXD before Windows is shutdown to disable
// the floppy scanning of drive A:. This addresses the problem with hot-swappable
// floppies that are removed before shutdown.
// 
//    Rev 1.25   14 Aug 1998 12:24:52   mdunn
// Changed trial-expired message to support varying-length trial periods
// 							 
//    Rev 1.24   14 Jul 1998 14:58:24   mdunn
// Moved preprocessor stuff around to get the trialware GUIDs properly defined.
// 
//    Rev 1.23   29 Jun 1998 22:50:18   RStanev
// All calls to CApwCmd::Init() have now been updated to pass either a handle
// or NULL.
//
//    Rev 1.22   29 Jun 1998 22:21:34   RStanev
// CApwCmd now takes HWND as a parameter.  In addition, added processing
// for APWM_COM_TASKS message, which triggers calls to the quarantine APIs
// for checking the incoming directory.
//
//    Rev 1.21   27 May 1998 13:09:18   mdunn
// Nuked ApwActivatePrevInst() since we don't want the Agent UI shown any more.
//
//    Rev 1.20   13 Mar 1998 17:57:08   tcashin
// Start Navw when the "Start Nav" menu item is selected.
//
//    Rev 1.19   18 Feb 1998 18:32:38   DALLEE
// Moved InitNLMMessenger() call to APWCMD Apw95Cmd.cpp.
// APWUI should only contain UI and rely on the APWCMD project for guts.
//
//    Rev 1.18   17 Feb 1998 14:49:24   MKEATIN
// Roswell Port:
//
//     Rev 1.17   16 Jan 1998 12:42:04   CEATON
//  Added call to InitNLMMessenger(), which is necessary for alerting NAV NLMs.
//
//
//    Rev 1.17   26 Jan 1998 13:57:12   DALLEE
// Mask low 4 bits of uCmd in WM_SYSCOMMAND.
//
//    Rev 1.16   11 Aug 1997 17:08:56   DALLEE
// Added magic wParam and lParam values to APWM_SHUTDOWN so we don't respond
// if we ever get this message by accident.
// Added APWM_UPDATE_UI handling - notification that we should resync the
// displayed status of the driver with the current status.
//
//    Rev 1.15   05 Aug 1997 18:05:16   DALLEE
// Added call to ApwActivate() before ExitWindowsEx() when shutting down.
// Main thread was yielding in ExitWindowsEx() and never woke up again.
//
// Activating the main window seems to work around the problem - so does
// moving the mouse over the main dialog if visible - but I'm still not
// sure of the root of the problem.
//
//    Rev 1.14   29 Sep 1997 16:30:42   DALLEE
// Moved trialware check after single instance check.
// On startup, we got two copies of NAVAPW - one from VxD, one from Run
// command. Starting two of the same RPC servers ( happens in pCmd->Init() )
// causes the first process to exit to crash.
//
//    Rev 1.13   17 Jul 1997 12:07:58   DALLEE
// Show minimized in all cases now when launched for the first time.
//
//    Rev 1.12   15 Jul 1997 17:54:58   MKEATIN
// If the trial period is expired we now disable the VxD.
//
//    Rev 1.11   11 Jul 1997 17:42:04   MKEATIN
// Added trial version code
//
//    Rev 1.10   09 Jul 1997 13:04:56   DALLEE
// Initialize CApwCmd before using it in WM_SHUTDOWN.
// Egads, how did this ever work?
//
//    Rev 1.9   27 Jun 1997 19:17:54   DALLEE
// No longer send NAVAP starting message to NAVW.
// This will be handled by the AP service.
//
//    Rev 1.8   24 Jun 1997 16:59:30   DALLEE
// Handling NAVW_CMD_UNLOADNAVAPW: Disable driver before deinit in
// response to unload command from NAVW.
// This will bypass the floppy scanning ( and the annoying blue screen ).
//
//    Rev 1.7   23 Jun 1997 18:12:56   DALLEE
// Fix for showing minimized if /LOADQUIET, and not previously loaded by VxD.
//
//    Rev 1.6   17 Jun 1997 18:04:12   DALLEE
// 1. Added shutdown bitmap replacement code when shutting down due to virus.
// 2. DeInit() command object before shutting down Windows due to virus.
//
//    Rev 1.5   16 Jun 1997 11:04:46   DALLEE
// Show minimized, no active on /LOADQUIET.
//
//    Rev 1.4   11 Jun 1997 12:14:54   DALLEE
// Added ApwTrayTimer() to wait for tray if not present on startup.
// Don't allow exit unless on NT.
// Notify NAVW when starting so NAVW can disable AP for its process.
//
//    Rev 1.3   10 Jun 1997 14:51:40   DALLEE
// Added registered message handling for NAVW NAVAPW communication.
// Allows update of icon/disable options, and unloading auto-protect.
//
//    Rev 1.2   09 Jun 1997 00:04:36   DALLEE
// Added WM_ENDSESSION handling to perform cleanup - prevent hang on shutdown.
//
//    Rev 1.1   05 Jun 1997 17:10:14   DALLEE
// Added disabling of "Disable" button based on options.
//
//    Rev 1.0   02 Jun 1997 13:56:56   DALLEE
// Initial revision.
//************************************************************************

#define STRICT
#include "stdafx.h"

#include "platform.h"
#include <windowsx.h>
#include <tchar.h>
#include <initguid.h>
#include <atlbase.h>
#include <atlcom.h>

#define INITIIDS
#include "syminterface.h"
#include "ccAppPlugin.h"

#include "StahlSoft.h"
#define SIMON_INITGUID
#include <SIMON.h>
#define _INIT_COSVERSIONINFO
#include "SSOsinfo.h"
#include "cltLicenseConstants.h"

// PEP Headers
#define _INIT_V2LICENSINGAUTHGUIDS
#define _V2AUTHORIZATION_SIMON
#define INIT_V2AUTHORIZATION_IMPL
#define _DRM_V2AUTHMAP_IMPL
#define _SIMON_PEP_
#include "ComponentPepHelper.h"
#include "DRMNamedProperties.h"

#include "ccModuleId.h"
#include "AVccModuleId.h"
#include "NAVError.h"

#include "resource.h"
#include "apw.h"
#include "apwutil.h"
#include "apwcfg.h"
#include "apwtray.h"

#include "drvcomm.h"

#include "navwcomm.h"
#include "msgreg.h"

#include "navver.h"

#include "navapcommands.h"
#include "navapsvc.h"
#include "navinfo.h"

#include "iquaran.h"

#include <process.h>
#include <vector>
#include <crtdbg.h>
#include "NAVTrust.h"
#include "OptNames.h"
#include "DefAlertOptNames.h"
#include "AVRESBranding.h"
#include "NAVErrorResource.h"

#include "NAVOptRefresh.h"
#include "NAVBusinessRules.h"
#include "NavSettingsCache.h"
#include "GlobalEvents.h"	// Option change events
#include "GlobalEvent.h"    
#include "BusinessRules.h"
//#include "GlobalMutex.h"        // For NAVOptRF mutex
#include "navapnotify.h"    // For tray icon event

#include "NAVPwd_h.h"
#include "NAVPwd_i.c"
#include "NAVPwdexp.h"

#include "ccLib.h"
#include "ccCriticalSection.h"
#include "ccLibDllLink.h"
#include "ccSymDelayLoader.h"
#include "ccEveryoneDacl.h"

#include "NAVSettingsHelperEx.h"
#include "NAVSnoozeMonitor.h"

#include "QuarantineDllLoader.h"

//Resource loading code
#include "..\apwuires\resource.h"
#include "ccResourceLoader.h"
#define SYMC_RESOURCE_DLL _T("navapw32.loc")
CAtlBaseModule _Module;
::cc::CResourceLoader g_ResLoader(&_Module, SYMC_RESOURCE_DLL);

// ccLib delay loader
// This will allow the ccLib dll to delay load at run-time
ccSym::CDelayLoader g_DelayLoader;

#include "ccTrace.h"
#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("NAVAPW32"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

StahlSoft::CSmartModuleHandle           g_smLibNavOptRefresh;
SIMON::CSimonModule _SimonModule;

bool g_bOKToClose = false;

//************************************************************************
// DEFINES
//************************************************************************

#define APW_WND_STYLE   (CS_HREDRAW | CS_VREDRAW )

// Values for timer used to wait for shell tray to appear.

#define TRAY_TIMER_ID       1
#define TRAY_TIMER_TIMEOUT  5000        // in milliseconds.
#define CLOCK_FIX_ID        2
#define CLOCK_FIX_TIMEOUT   30000       // in milliseconds.

//************************************************************************
// TYPEDEFS
//************************************************************************


//************************************************************************
// GLOBALS
//************************************************************************

const   char    g_szClassName[] = APW_CLASS_NAME;
const   char    g_szSingleInstMutex[] = "Norton AntiVirus Agent Is Running";
const	char	g_szApw95cmdDll[] = "apwcmd9x.dll";
const	char	g_szApwNTcmdDll[] = "apwcmdnt.dll";
const	char	NAVOPTRF_MUTEX_SESSIONWIDE[] = "NAVOPTRF_MUTEX_SESSIONWIDE";

static	UINT	s_WM_TASKBAR_CREATED = 0;
static  HINSTANCE g_hInstance = NULL;
CString g_csProductName;

//************************************************************************
// PROTOTYPES
//************************************************************************

#ifdef __cplusplus
extern "C"  {
#endif
	
	LRESULT __declspec( dllexport ) WINAPI ApwMainWndProc (
		HWND      hWnd,
		UINT      uMessage,
		WPARAM    wParam,
		LPARAM    lParam );
	
#ifdef __cplusplus
}
#endif

static BOOL ApwMainWM_CREATE ( HWND hWnd, LPCREATESTRUCT lpCreateStruct );
static VOID ApwMainWM_SYSCOMMAND ( HWND hWnd, UINT cmd, int x, int y );
static VOID ApwMainWM_COMMAND ( HWND hWnd, int nID, HWND hWndControl, UINT uCode );
static VOID ApwMainWM_ENDSESSION ( HWND hWnd, BOOL bEnding );
static VOID ApwMainWM_DESTROY ( HWND hWnd );
static VOID ApwMainWM_CLOSE ( HWND hWnd );
static VOID ApwMainWM_TIMECHANGE ( HWND hWnd );
static LRESULT ApwMainAPWM_ACTIVATE ( HWND hWnd, WPARAM wParam, LPARAM lParam );
static LRESULT ApwMainAPWM_INITDIALOG ( HWND hWnd, WPARAM wParam, LPARAM lParam );
static LRESULT ApwMainAPWM_SHUTDOWN ( HWND hWnd, WPARAM wParam, LPARAM lParam );
static LRESULT ApwMainAPWM_UPDATE_UI ( HWND hWnd, WPARAM wParam, LPARAM lParam );
static LRESULT ApwMainAPWM_COM_TASKS ( HWND hWnd, WPARAM wParam, LPARAM lParam );
static LRESULT ApwMainDefaultProc ( HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam );
static BOOL ApwRegisterClass ( HINSTANCE hInst );
static HWND ApwCreateMainWnd ( HINSTANCE hInst );
static VOID ApwMainDlgUpdate ( VOID );
static VOID CALLBACK ApwTrayTimer( HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime );
static BOOL ApwReplaceShutdownBitmaps ( VOID );
static BOOL HWIsNEC();
static CApwCmd *ApwMainGetCmdObject();
static HRESULT ProcessCommandLine( LPSTR pszCommandLine );
static LPCTSTR FindOneOf(LPCTSTR p1, LPCTSTR p2);
static void RegisterServer();
static void UnRegisterServer();
static HRESULT RegisterObject( REFCLSID rclsid );
static HRESULT UnRegisterObject( REFCLSID rclsid );
static HRESULT RegisterTlb( BOOL bRegister );
static BOOL CreateKey( HKEY hKeyParent, LPTSTR szKeyName, LPTSTR szDefaultValue );
static bool CheckPassword();
static HRESULT SetProductTampered(long nErrorId);

static unsigned __stdcall mainUIThread( void *pParam );

//************************************************************************
// CODE
//************************************************************************


class CAPPlugin : 
public ISymBaseImpl< CSymThreadSafeRefCount >, 
public ICCAppPlugin
{
private:
	volatile bool m_bRequestStop;
	//HANDLE m_hStop;
    ccLib::CEvent m_eventStop;
    ccLib::CEvent m_eventSnooze;

public:
	CAPPlugin() : m_bRequestStop(false)
    {
    CCTRACEI("CAPPlugin()");

		// Create stop event.
		m_eventStop.Create( NULL, TRUE, FALSE, NULL, FALSE );

        ccLib::CEveryoneDACL everyonedacl;
		everyonedacl.Create();

		if (!m_eventSnooze.Create( everyonedacl, FALSE, FALSE, NAVSnoozeSettingsChangedEvent, TRUE ))
		{
            assert(false);
			CCTRACEE("Failed to create the snooze changed event");

			// Create any event to keep plugin alive
			if (!m_eventSnooze.Create( NULL, FALSE, FALSE, NULL, FALSE ))
			{
				CCTRACEE("Failed to create an event!");
			}
		}
    }
	virtual ~CAPPlugin()
	{
	}
									    
	SYM_INTERFACE_MAP_BEGIN()                
		SYM_INTERFACE_ENTRY( IID_CCAppPlugin, ICCAppPlugin ) 
	SYM_INTERFACE_MAP_END()                  
		
	virtual void Run( ICCApp* pCCApp )
	{
        CCTRACEI("Run()");

        //::MessageBox ( NULL, "break here", "navapw32.dll", MB_OK );
        assert(false);

		if(g_csProductName.IsEmpty())
		{
			CBrandingRes BrandRes;
			g_csProductName = BrandRes.ProductName();
			CCTRACEI(_T("CAPPlugin::Run() - ProductName: %s"), g_csProductName);
		}

        // Process command line here.
	    HRESULT hr = ProcessCommandLine( NULL );
	    if( FAILED( hr ) )
        {
            CCTRACEE(_T("CAPPlugin::Run() - Failed to process the commandline. Exiting"));
		    return;
        }

		if (m_eventStop == NULL)
            return;
        if (WaitForSingleObject(m_eventStop, 0) == WAIT_OBJECT_0)
            return;

        // Display the tray icon and get all the windows hooked up, first.
        //
        ccLib::CMessageLock msgLock (TRUE, TRUE);
        
        if ( !g_eventTrayIcon.Create (NULL, TRUE, FALSE, SYM_NAV_AP_TRAY_ICON_EVENT, FALSE))
        {
            CCTRACEE ( "CPlugin::Run() - failed to create tray icon event %d", ::GetLastError() );
            assert(false);
        }

		// Set the main thread in motion.
		unsigned dwID;
		HANDLE hThread = (HANDLE) _beginthreadex( NULL, 0, mainUIThread, this, 0, &dwID );  
		if( hThread == (HANDLE)-1 )
		{
			CCTRACEE( "CAPPlugin::Run() - Could not create mainUIThread thread" );
			return;
		}

        // Wait for the tray icon
        //
        if ( g_eventTrayIcon.IsEvent ())
        {
            std::vector<HANDLE> vecWaitEvents;
            vecWaitEvents.push_back (m_eventStop);
            vecWaitEvents.push_back (g_eventTrayIcon);

            // We don't care why we woke up, just go
            DWORD dwWaitResult = msgLock.Lock ( vecWaitEvents.size(), &vecWaitEvents[0], FALSE, 30*1000, FALSE);
            if ( dwWaitResult == WAIT_TIMEOUT )
                CCTRACEW ("CPlugin::Run() - timeout waiting for tray");
            else
                CCTRACEI ("CPlugin::Run() - Tray icon up");
        }

        ccLib::CCoInitialize COMInit(ccLib::CCoInitialize::eNoCOMModel);
        try
        {
            COMInit.CoInitialize(ccLib::CCoInitialize::eSTAModel);
        }
        catch (_com_error err)
        {
            CCTRACEE (_T("CAPPlugin::Run() - Failed to initialize COM, bailing out. 0x%x"), err.Error());
		    return;
        }

// Comment DRM stuff so we can debug
#ifndef _DEBUG
        // Make sure NAVOptRF is running for this session.
        //
        {
            int iRetries = 0;
            ccLib::CMutex mutexNAVOptRF;

            while ( true )
            {
                if ( mutexNAVOptRF.Open ( SYNCHRONIZE, FALSE, NAVOPTRF_MUTEX_SESSIONWIDE, FALSE))
	            {
                    CCTRACEI ( "CPlugin::Run() - NAVOptRF running OK" );
                    break;
	            }
                else
                {
                    CCTRACEE( "CAPPlugin::Run() - The navoptrf running mutex was not found." );

                    ccLib::CMessageLock msgLock (TRUE, TRUE);
                    DWORD dwRet = msgLock.Lock ( m_eventStop, 1000 );

                    // Told to stop
                    if (dwRet == WAIT_OBJECT_0 )
                    {
                        CCTRACEW( "CAPPlugin::Run() - Told to stop while waiting for the navoptrf mutex" );
                        return;
                    }

                    iRetries++;
                    if ( iRetries > 60 )
                    {
                        CCTRACEE ( "CPlugin::Run() - NAVOptRF not running" );

                        // Disable the product and display error dialog.
                        SetProductTampered(ERR_NAVOPTRF_CREATEOBJECT);
		                return;
                    }
                }
            }
        }
#else
    // Create the mutex so that other modules behave normally while AP is debug
    //
    ccLib::CMutex mutexNAVOptRF;

    // Use a NULL dacl so that all users can open this mutex
    ccLib::CEveryoneDACL everyonedacl;
    if ( !everyonedacl.Create() )
        CCTRACEE( "CAPPlugin::Run() - Failed to create the NULL dacl" );

    if ( !mutexNAVOptRF.Create ( everyonedacl, TRUE, NAVOPTRF_MUTEX_SESSIONWIDE, FALSE ))
    {
		CCTRACEE ( "CAPPlugin::Run() - Failed to create mutex" );
        assert(false);
    }
#endif // remove navoptrf.dll for debug

		CNAVSnoozeMonitor snoozeMonitor;

		snoozeMonitor.Init();
	
		// Wait here for someone to signal.
	    DWORD dwRet = -1;
        HANDLE aHandles[] = { m_eventStop, hThread, m_eventSnooze };
	    BOOL bBreak = FALSE;

	    CCTRACEI("Run - listening...");

        for(;;)
	    {
            bBreak = FALSE;
            dwRet = msgLock.Lock ( sizeof(aHandles)/sizeof(aHandles[0]), aHandles, FALSE, snoozeMonitor.GetTimeout(), 0 );

			if (dwRet == WAIT_TIMEOUT)
			{
				snoozeMonitor.DoWork();
				continue;
			}

		    switch(dwRet - WAIT_OBJECT_0)
		    {
		    case 0:
			    // Stop has been signaled.
			   CCTRACEI( "CAPPlugin::Run() - ccApp signaled stop." );
                bBreak = TRUE;
			    break;
		    case 1:
			    // Thread has exited.
			   CCTRACEI( "CAPPlugin::Run() - mainUIThread thread has exited." );
                bBreak = TRUE;
			    break;
			case 2:
				// Snooze settings changed
				snoozeMonitor.RefreshSettings(false);
				break;

            default:
                bBreak = TRUE;
		    }			

            if (bBreak != FALSE)
            {
                break;
            }
	    }

 		CCTRACEI( "CAPPlugin::Run() - exiting normally." );
	}
	
	virtual bool CanStopNow()
	{
		CCTRACEI( "CAPPlugin::CanStopNow() : Entered CanStopNow()" );

		// No work to do on Win9x platforms.
		if( GetWindowsPlatform() != VER_PLATFORM_WIN32_NT )
			return true;

		CApwConfig  *pConfig;
		CApwCmd		*pCmd;
		
		// Get configuration.
		pConfig = ApwGetConfig();
		SYM_ASSERT( pConfig );
		pCmd    = pConfig->GetCmd();
		SYM_ASSERT( pCmd );

		// Attempt a boot record scan.
		if ( pCmd )
        {
            CCTRACEI( "CAPPlugin::CanStopNow() : starting shutdown scan" );
            BOOL bRet = pCmd->ShutdownScan(NULL);
		    CCTRACEI( "CAPPlugin::CanStopNow() : returned %s", bRet ? "true" : "false" );
            return bRet ? true : false;
        }

        return true;
	}
	
	virtual void RequestStop()
	{
		CCTRACEI( "CAPPlugin::RequestStop()" );
		
		if(m_bRequestStop == false)
		{
			m_bRequestStop = true;
            g_bOKToClose = true;
			
			CApwConfig  *pConfig = ApwGetConfig();
			HWND hWnd = pConfig->GetMainWnd();
			if( IsWindow(hWnd) )
			{
				CCTRACEI( "CAPPlugin::RequestStop() : SendMessageTimeout(WM_CLOSE)" );
                SendMessage ( hWnd, WM_CLOSE, 0, 0 );
			}

			// Set the stop event.
            m_eventStop.SetEvent();
		}
	}
	
	virtual const char * GetObjectName()
	{
		return "NAV AutoProtect";
	}
	
	virtual unsigned long OnMessage( int iMessage, unsigned long ulParam )
	{
		return 0;
	}

	bool GetRequestStop()
	{
		return m_bRequestStop;
	}
};

// {7BBBEECE-C842-4444-BD9C-D4471CE595DE}
SYM_DEFINE_GUID(NAVAP_PLUGIN_OBJECT, 
				0x7afeeece, 0xc842, 0x4444, 0xbd, 0x9c, 0xd4, 0x47, 0x1c, 0xe5, 0x95, 0xde);

DECLARE_CCAPP_PLUGIN( NAVAP_PLUGIN_OBJECT )

//////////////////////////////////////////////////////////////////////

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


//////////////////////////////////////////////////////////////////////
// Object map.

SYM_OBJECT_MAP_BEGIN()
INIT_RESOURCES()
SYM_OBJECT_ENTRY( NAVAP_PLUGIN_OBJECT, CAPPlugin )
SYM_OBJECT_MAP_END()


BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
	switch( ul_reason_for_call )
	{
	case DLL_PROCESS_ATTACH:
        // Performance Optimization: see MSDN docs on DisableThreadLibraryCalls() func.
        DisableThreadLibraryCalls((HMODULE)hModule);

        g_hInstance = (HINSTANCE) hModule;
        break;
		
	case DLL_PROCESS_DETACH:
		// Must unregister classes registered in DLLs.
		UnregisterClass( g_szClassName, g_hInstance );
		break;
	}
	
    return TRUE;
}

//////////////////////////////////////////////////////////////////////
// mainUIThread()

unsigned  __stdcall mainUIThread( void *pParam )
{
    CCTRACEI ("mainUIThread() - start");
	// Fetch plugin pointer.
	CAPPlugin* pPlugin = (CAPPlugin*) pParam;
	if( pPlugin == NULL )
		return -1;
	
	auto    MSG         msg;
	auto    HWND        hMainWnd = NULL;
	
	auto    HANDLE      hSingleInstMutex = NULL;
	
	auto    CApwConfig  *pConfig = NULL;
	auto    CApwCmd     *pCmd = NULL;
	
	auto    int         nReturn;

	// Make sure we're still good to go.
	if( pPlugin->GetRequestStop() )
    {
        CCTRACEI ("mainUIThread() - stop immediately");
		return 0;
    }
	
    // Initialize COM for this thread
    // WJORDAN 2-26-02: Fix for Defect# 380432. Changing CoInitializeEx from 
    // multi-threaded to single apartment threaded.  The DefAlert plugin initializes  
    // COM as single apartment threaded causing an error when the AP command
    // DLL tried to initialize COM with multi-threaded since they happen to be
    // in the same thread and the threading model was already set.
    ccLib::CCoInitialize COMInit(ccLib::CCoInitialize::eNoCOMModel);
    try
    {
        COMInit.CoInitialize(ccLib::CCoInitialize::eSTAModel);
    }
    catch (_com_error err)
    {
        CCTRACEE (_T("apwui::mainUIThread() - Failed to initialize COM, bailing out. 0x%x"), err.Error());
		return 0;
    }

	// Activate previous instance, if any.  On Win9x based systems, use the	NAVAPSVC_RUNNING_MUTEX,
	// so the rest of the world knows we are running.
	
    ccLib::CEveryoneDACL everyonedacl;
	everyonedacl.Create();
	hSingleInstMutex = CreateMutex( everyonedacl, FALSE, GetWindowsPlatform() == VER_PLATFORM_WIN32_NT ? 
								g_szSingleInstMutex : NAVAPSVC_RUNNING_MUTEX );
	
	if ( NULL == hSingleInstMutex )
	{
        CCTRACEW("mainUIThread() - Failed creating mutex. Bail Out.");
		goto BailOut;
	}
	
	if ( ERROR_ALREADY_EXISTS == GetLastError() )
	{
		// No longer activating the previous instance, since we don't
		// want the Agent UI shown any more.
		CCTRACEW("mainUIThread() - There is already an instance of the Agent running. Bail Out.");
		goto BailOut;
	}

    CCTRACEI ("mainUIThread() - register window message");
	// Initialize locals.
	
	nReturn     = 0;
	pCmd        = NULL;
	
	s_WM_TASKBAR_CREATED = RegisterWindowMessage( __TEXT("TaskbarCreated") );

	// Save instance handle.
	CCTRACEI("mainUIThread() - Getting config");
	pConfig = ApwGetConfig();
	pConfig->SetInstance( g_hInstance );
	
	// Get external command implementation.
	
	pCmd = ApwMainGetCmdObject();
	
	if ( NULL == pCmd )
	{
		//&? Need bad error message? Unlikely ever to fail...
		
		goto BailOut;
	}
	
	pConfig->SetCmd( pCmd );

    // Read in the configuration
    CCTRACEI("mainUIThread() - Reading options");
    pConfig->ReadNavopts();

    // Check the licensing state, scoped to release the settings objects. Note that the licensing
    // state flag is also exposed from the AP command object, but this will not be set until the
    // command object has been initialized and then after that it will be requeried every time the
    // Option changed event is fired. During initialization we must check it directly.
    {
        CNAVOptSettingsEx navSettings;
        if( navSettings.Init() )
        {
            DWORD dwFeatureEnabled = 1;
            if( navSettings.GetValue(LICENSE_FeatureEnabled, dwFeatureEnabled, 1) )
            {
                if( dwFeatureEnabled == 0 )
                {
                    // AP_Feature is disabled (i.e. license is not valid), disable it.
                    CCTRACEE("mainUIThread(): License is not valid. Disable driver.");
		            if( pCmd->Init(NULL))
			            pCmd->DisableDriver();
                }
            }
            else
                CCTRACEE("mainUIThread(): Unable to query CNAVOptSettingsEx for the licensing state.");
        }
        else
             CCTRACEE("mainUIThread(): Unable to create CNAVOptSettingsEx to check the licensing state.");
    }
	
	// Register class.
    CCTRACEI("mainUIThread() - Registering class");
	_Module.SetResourceInstance(g_ResLoader.GetResourceInstance());
	if ( FALSE == ApwRegisterClass( g_hInstance ) )
	{
		goto BailOut;
	}
	
	// Create main window/dialog.
	CCTRACEI("mainUIThread() - Creating window");
	hMainWnd = ApwCreateMainWnd( g_hInstance );
	
	if ( NULL != hMainWnd )
	{
		// Show main window.
		// Displaying hidden/minimized in all cases now...
		
		ApwMinimize();
		
		// Message loop.
		
		if( pPlugin->GetRequestStop() )
		{
			pPlugin->RequestStop();
		}
		
		CCTRACEI( "NAVAP: mainUIThread() : Message pump started" );

		while ( GetMessage( &msg, NULL, 0, 0 ) == 1)
		{
            if ( !IsDialogMessage( hMainWnd, &msg ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
		}
		
       CCTRACEI( "NAVAP: mainUIThread() : Message pump ended" );
		
		nReturn = msg.wParam;
	}
	
BailOut:
	if ( NULL != pCmd )
	{
		pConfig->SetCmd( NULL );
		delete pCmd;
	}							  
	
	if ( NULL != hSingleInstMutex )
		CloseHandle( hSingleInstMutex );

	return 0;
}

//************************************************************************
// ApwRegisterClass()
//
// static BOOL ApwRegisterClass (
//      HINSTANCE   hInst );            // Our instance.
//
// This routine registers NAVAPW's window class.
//
// Returns:
//      TRUE        on success.
//      FALSE       failed.
//************************************************************************
// 05/18/1997 DALLEE, created.
//************************************************************************

static BOOL ApwRegisterClass ( HINSTANCE  hInst )
{
    auto    WNDCLASSEX  wndclass;
	
    wndclass.cbSize         = sizeof( WNDCLASSEX );
    wndclass.style          = APW_WND_STYLE;
    wndclass.lpfnWndProc    = ApwMainWndProc;
    wndclass.cbClsExtra     = 0;
    wndclass.cbWndExtra     = sizeof( LONG ) * APWL_MAX;
    wndclass.hInstance      = g_ResLoader.GetResourceInstance();
    wndclass.hIcon          = LoadIcon( hInst, MAKEINTRESOURCE( IDI_APW_APP_ENABLED ) );
    wndclass.hIconSm        = ( HICON ) LoadImage ( hInst,
		MAKEINTRESOURCE( IDI_APW_APP_ENABLED ),
		IMAGE_ICON,
		16, 16, 0 );
    wndclass.hCursor        = NULL;
    wndclass.hbrBackground  = ( HBRUSH ) ( COLOR_BTNFACE + 1 );
    wndclass.lpszMenuName   = NULL;
    wndclass.lpszClassName  = g_szClassName;
	
    return ( RegisterClassEx( &wndclass ) ? TRUE : FALSE );
} // ApwRegisterClass()


//************************************************************************
// ApwCreateMainWnd()
//
// static HWND ApwCreateMainWnd (
//      HINSTANCE   hInst )             // Our instance.
//
// This routine creates NAVAPW's main window/dialog.
//
//
// Returns:
//      HWND    Handle of main window.
//      NULL    if not created.
//************************************************************************
// 05/18/1997 DALLEE, created.
//************************************************************************

static HWND ApwCreateMainWnd ( HINSTANCE    hInst )
{
    auto    CApwConfig  *pConfig;
    auto    HWND        hWnd;
	
    // Create main window. Actually a dialog using our registered class.
	hWnd = CreateDialog(g_ResLoader.GetResourceInstance(),
		MAKEINTRESOURCE( IDD_APW_MAIN ),
		NULL,
		NULL );
	
    pConfig = ApwGetConfig();
    pConfig->SetMainWnd( hWnd );
	
    return ( hWnd );
} // ApwCreateMainWnd()


//************************************************************************
// ApwMainWndProc()
//
// Main window procedure.
//
// Parameters:
//
// Returns:
//
//************************************************************************
// 05/18/1997 DALLEE, created.
//************************************************************************


LRESULT __declspec( dllexport ) WINAPI ApwMainWndProc ( HWND      hWnd,
													   UINT      uMessage,
													   WPARAM    wParam,
													   LPARAM    lParam )
{
    auto    LONG    lReturn = 0;

    switch ( uMessage )
    {
    case WM_CREATE:
        lReturn = HANDLE_WM_CREATE( hWnd, wParam, lParam,
			ApwMainWM_CREATE );
        break;
		
    case WM_COMMAND:
        lReturn = HANDLE_WM_COMMAND( hWnd, wParam, lParam,
			ApwMainWM_COMMAND );
        break;

    case WM_CLOSE:
        lReturn = HANDLE_WM_CLOSE( hWnd, wParam, lParam,
			ApwMainWM_CLOSE );
        break;

    case WM_SYSCOMMAND:
        lReturn = HANDLE_WM_SYSCOMMAND( hWnd, wParam, lParam,
			ApwMainWM_SYSCOMMAND );
        break;
		
    case WM_ENDSESSION:
        lReturn = HANDLE_WM_ENDSESSION( hWnd, wParam, lParam,
			ApwMainWM_ENDSESSION );
        lReturn = TRUE;
        break;
		
    case WM_DESTROY:
        lReturn = HANDLE_WM_DESTROY( hWnd, wParam, lParam,
			ApwMainWM_DESTROY );
        break;

	case WM_TIMECHANGE:
		lReturn = HANDLE_WM_TIMECHANGE( hWnd, wParam, lParam,
			ApwMainWM_TIMECHANGE );
		break;
		
    case APWM_INITDIALOG:
        lReturn = ApwMainAPWM_INITDIALOG( hWnd, wParam, lParam );
        break;
		
    case APWM_ACTIVATE:
        lReturn = ApwMainAPWM_ACTIVATE( hWnd, wParam, lParam );
        break;
		
    case APWM_TRAY_NOTIFY:
        lReturn = ApwTrayNotify( hWnd, wParam, lParam );
        break;
		
    case APWM_SHUTDOWN:
        lReturn = ApwMainAPWM_SHUTDOWN( hWnd, wParam, lParam );
        break;
		
    case APWM_UPDATE_UI:
        lReturn = ApwMainAPWM_UPDATE_UI( hWnd, wParam, lParam );
        break;
		
    case APWM_COM_TASKS:
        lReturn = ApwMainAPWM_COM_TASKS( hWnd, wParam, lParam );
        break;

	default:
        // Handle message when taskbar is created.
        if (uMessage == s_WM_TASKBAR_CREATED)
        {
            // Tell the current config that the icon is missing.
		    ApwGetConfig()->SetInTray( FALSE );
		    
		    // Force an update of the tray icon.
		    ApwUpdateTrayIcon();
		    lReturn = 0;
        }
        else
        {
			if (uMessage == WM_DISPLAYCHANGE)
			{
				CCTRACEI("WM_DISPLAYCHANGE");
				ApwMainDlgUpdate();
				ApwUpdateTrayIcon(true);
			}

		    // ApwMainDefaultProc() handles custom registered messages
            // (for communication with NAVW), or passes through to DefDlgProc().
            lReturn = ApwMainDefaultProc( hWnd, uMessage, wParam, lParam );
        }
        break;
    }
	
    return ( lReturn );
} // ApwMainWndProc()


//************************************************************************
// ApwMainWM_SYSCOMMAND()
//
// static VOID ApwMainWM_SYSCOMMAND (
//      HWND        hWnd,               // Handle of main window.
//      UINT        uCmd,               // Command issued.
//      int         x,                  // Window position.
//      int         y );
//
//
// Respond to WM_SYSCOMMAND for main window. Need special handling for
// the minimize command.
//
// Parameters:
//      HWND        hWnd                Handle of main window.
													   //      int         nID                 Control ID.
													   //      HWND        hWndControl         Window handle of control.
													   //      UINT        uCode               Notification code.
//
// Returns:
//      Nothing.
//************************************************************************
// 05/18/1997 DALLEE, created.
//************************************************************************

static VOID ApwMainWM_SYSCOMMAND ( HWND hWnd, UINT uCmd, int x, int y )
{
    // Mask low 4 bits of uCmd - internal use by Windows only.
	
    switch ( uCmd & 0xFFFFFFF0 )
    {
    case SC_MINIMIZE:
        // Conceal main window - either minimize, or hide.
		
        ApwMinimize();
        break;
		
    default:
        FORWARD_WM_SYSCOMMAND( hWnd, uCmd, x, y, DefWindowProc );
        break;
    }
} // ApwMainWM_SYSCOMMAND()


//************************************************************************
// ApwMainWM_COMMAND()
//
// Respond to WM_COMMAND for main window.
//
// Parameters:
//      HWND        hWnd                Handle of main window.
//      int         nID                 Control ID.
//      HWND        hWndControl         Window handle of control.
//      UINT        uCode               Notification code.
//
// Returns:
//      Nothing.
//************************************************************************
// 05/18/1997 DALLEE, created.
//************************************************************************

static VOID ApwMainWM_COMMAND ( HWND    hWnd,
							   int     nID,
							   HWND    hWndControl,
							   UINT    uCode )
{
    auto    CApwConfig  *pConfig;
    auto    CApwCmd     *pCmd;
	
    // Get configuration.
	
    pConfig = ApwGetConfig();
    pCmd    = pConfig->GetCmd();
	
    SYM_ASSERT( pConfig );
    SYM_ASSERT( pCmd );
	
    switch ( nID )
    {
    case IDC_MAIN_MINIMIZE:
        // Conceal main window - either minimize, or hide.
		
        ApwMinimize();
        break;
		
    case IDM_TRAY_OPEN:
        // Open our main window.
		
        ApwActivateNavw();
        break;
		
    case IDC_MAIN_ENABLE:
    case IDM_TRAY_ENABLE:
        //&? Bogon! Should use separate function probably...
		
        switch( pConfig->GetState() )
        {
        case AP_STATE_ENABLED:
			if( CheckPassword() && CNAVSnoozeMonitor::SnoozeAutoProtect(false))
				pCmd->DisableDriver();
            break;
			
        case AP_STATE_DISABLED:
			CNAVSnoozeMonitor::SnoozeAutoProtect(true);
            pCmd->EnableDriver();
            break;
        }
		
        pConfig->SetState( pCmd->GetStatusOfDriver() );
        ApwMainDlgUpdate();
        ApwUpdateTrayIcon();
        break;
		
		case IDC_MAIN_OPTIONS:
		case IDM_TRAY_OPTIONS:
			
			pCmd->Options( hWnd );
			break;
			
		case IDOK:
			
			break;
			
		case IDCANCEL:
			// Only allow exit here on NT where UI is separate from
			// service and driver.
			
			if ( VER_PLATFORM_WIN32_NT == GetWindowsPlatform() )
			{
				g_bOKToClose = true;
                DestroyWindow( hWnd );
			}
			else
			{
				ApwMinimize();
			}
			break;
    }
} // ApwMainWM_COMMAND()


//************************************************************************
// ApwMainWM_CREATE()
//
// Do startup chores in WM_CREATE of main window.
// Also posts APWM_INITDIALOG message to signal initialization which
// needs to take place after child controls are created.
//
// Parameters:
//      HWND            hWnd            Handle of window being created.
//      LPCREATESTRUCT  lpCreateStruct  Window creation info.
//
// Returns:
//      TRUE                            Continue creation of window.
//      FALSE                           Fail CreateWindow().
//************************************************************************
// 05/18/1997 DALLEE, created.
//************************************************************************

static BOOL ApwMainWM_CREATE ( HWND hWnd, LPCREATESTRUCT lpCreateStruct )
{
    // Center window before first WM_NCPAINT.
	
    ApwCenterWindow( hWnd, NULL );
	
	// Make sure we are at the top.  Gets around problem with APC
	// Power-chute (c) software.  See defect #355979
	SetWindowPos(hWnd, HWND_TOPMOST, 0,0,0,0,SWP_NOMOVE|SWP_NOREDRAW|SWP_NOREPOSITION|SWP_NOSIZE);
	SetForegroundWindow(hWnd);
	SendMessage(hWnd, WM_USER, 0, 0);
	
    // Send fakey dialog init message to main window.
	
    PostMessage( hWnd, APWM_INITDIALOG, 0, 0 );
	
    return ( TRUE );
} // ApwMainWM_CREATE()

//************************************************************************
// ApwMainWM_ENDSESSION()
//
// static VOID ApwMainWM_ENDSESSION (
//      HWND hWnd,                      // Handle of main window.
//      BOOL bEnding )                  // TRUE if session is really ending.
//
// If session is ending, need to perform cleanup here.
//
// Returns:
//      Nothing.
//************************************************************************
// 06/08/1997 DALLEE, created.
//************************************************************************

static VOID ApwMainWM_ENDSESSION ( HWND hWnd, BOOL bEnding )
{
	CCTRACEI( "NAVAP : ApwMainWM_ENDSESSION() : Enter" );
    
	auto    CApwConfig  *pConfig;
    auto    CApwCmd     *pCmd;
	
    if ( bEnding )
    {
        // Get rid of tray icon if present.
		
        pConfig = ApwGetConfig();
        pCmd    = pConfig->GetCmd();
		
        pConfig->SetShowIcon( FALSE );
        ApwUpdateTrayIcon();
		
        // DeInit the command module.
		
        if ( FALSE == pConfig->GetDoingShutdown() )
        {
            DWORD AvailableDrives;
			
            AvailableDrives = GetLogicalDrives();
			
            pConfig->SetDoingShutdown( TRUE );
			
			// On Win9x systems, if there is no A: drive, disable the 
			// driver now.  I assume that doing this keeps the VxD from
			// attempting the shutdown scan on non-existant drives.	 Not 
			// a problem on WinNT systems.
			if ( VER_PLATFORM_WIN32_NT != GetWindowsPlatform() && !( AvailableDrives & 0x1 ) )
                pCmd->DisableDriver();
			
            pCmd->DeInit();
        }

    	PostQuitMessage( 0 );

        // This is not the plugin thread but the plugin is waiting for us
        // There is no cleenup code after the message pump so it is ok to 
        // end the thread here.
       CCTRACEI( "NAVAP : ApwMainWM_ENDSESSION() : _endthreadex()" );
		_endthreadex( 0 );
    }

   CCTRACEI( "NAVAP : ApwMainWM_ENDSESSION() : Exit" );

} // ApwMainWM_ENDSESSION()


//************************************************************************
// ApwMainWM_CLOSE()
//
// Do cleanup chores in WM_CLOSE of main window.
//
// Parameters:
//      HWND        hWnd                Handle of main window.
//
// Returns:
//      Nothing.
static VOID ApwMainWM_CLOSE ( HWND    hWnd )
{

   CCTRACEI( "NAVAP : ApwMainWM_CLOSE() : Enter" );

    if ( !g_bOKToClose )
    {
       CCTRACEE( "NAVAP : ApwMainWM_CLOSE() : Close rejected!" );
        //::MessageBox ( ::GetDesktopWindow (), "Received illegal close", "APUI", MB_OK );
        return;
    }
    
    //
    // Nothing really do do here, so call DestroyWindow()
    // and return;
    //
    if(!DestroyWindow(hWnd))
    {
       CCTRACEE("NAVAP : ApwMainWM_CLOSE() DestroyWindow() failed. GetLastError() returned 0x%08X", GetLastError());
    }
    
    
   CCTRACEI( "NAVAP : ApwMainWM_CLOSE() : Exit" );
    
    return;
}


//************************************************************************
// ApwMainWM_DESTROY()
//
// Do cleanup chores in WM_DESTROY of main window.
//
// Parameters:
//      HWND        hWnd                Handle of main window.
//
// Returns:
//      Nothing.
//************************************************************************
// 05/18/1997 DALLEE, created.
//************************************************************************

static VOID ApwMainWM_DESTROY ( HWND    hWnd )
{
   CCTRACEI( "NAVAP : ApwMainWM_DESTROY() : Enter" );

    auto    CApwConfig  *pConfig;
    auto    CApwCmd     *pCmd;
	
    // Get rid of tray icon if present.
	
    pConfig = ApwGetConfig();
    pCmd    = pConfig->GetCmd();
	
    pConfig->SetShowIcon( FALSE );
    CCTRACEI( "NAVAP : ApwMainWM_DESTROY() : Updating icon" );
    ApwUpdateTrayIcon();
	
    // DeInit the command module.
	
    if ( FALSE == pConfig->GetDoingShutdown() )
    {
        pConfig->SetDoingShutdown( TRUE );
		
        CCTRACEI( "NAVAP : ApwMainWM_DESTROY() : Deinit" );
        pCmd->DeInit();
    }
	
    OSVERSIONINFO osInfo = { sizeof(osInfo) };
    
    GetVersionEx( &osInfo );
	
    //
    // Time fix is only ment for the WIN98 non-NEC platform...
    //
    if (
        ( osInfo.dwMajorVersion == 4  && osInfo.dwMinorVersion > 0 ) &&
        ( !HWIsNEC() )
		)
    {
		KillTimer( hWnd, CLOCK_FIX_ID );
    }
	
	CCTRACEI( "NAVAP : ApwMainWM_DESTROY() : Posting quit" );
    PostQuitMessage( 0 );

   CCTRACEI( "NAVAP : ApwMainWM_DESTROY() : Exit" );

} // ApwMainWM_DESTROY()

//************************************************************************

static VOID ApwMainWM_TIMECHANGE(HWND hWnd)
{
	CCTRACEI("Time changed");
	// Notify snooze monitor that the time has been changed
    ccLib::CEvent eventSnooze;
	if (eventSnooze.Open(EVENT_MODIFY_STATE, FALSE, NAVSnoozeSettingsChangedEvent, TRUE))
	{
		eventSnooze.SetEvent();
	}
	else
	{
		CCTRACEE("Failed to open the snooze changed event");
	}
}

//************************************************************************
// ApwMainAPWM_INITDIALOG()
//
// static LRESULT ApwMainAPWM_INITDIALOG (
//      HWND    hWnd,
//      WPARAM  wParam,
//      LPARAM  lParam );
//
// This routine is called in response to the APWM_INITDIALOG custom message
// which is a kluged surrogate for a WM_INITDIALOG since our main dialog is
// really a window not a dialog.
// This routine calls all NAVAPW initialization routines.
//
// Returns:
//      0 Always.
//************************************************************************
// 05/18/1997 DALLEE, created.
//************************************************************************

static LRESULT ApwMainAPWM_INITDIALOG( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
    auto    CApwConfig  *pConfig;
    auto    CApwCmd     *pCmd;
	
    // Set 3d look of dialog.
	
    ApwDlgSet3d( hWnd );
	
    if ( FALSE == NewShellPresent() )
    {
        // Force repaint of non-client area on NT 3.51 to see
        // 3d border on a thick frame dialog style main window.
        // At this point, we're already past the first WM_NCPAINT.
		
        auto    HRGN        hNCRegion;
        auto    RECT        rRect;
		
        GetWindowRect( hWnd, &rRect );
        hNCRegion = CreateRectRgnIndirect( &rRect );
		
        SendMessage( hWnd, WM_NCPAINT, ( WPARAM ) hNCRegion, 0 );
		
        DeleteObject( hNCRegion );
    }
	
    // Get configuration.
	
    pConfig = ApwGetConfig();
    pCmd    = pConfig->GetCmd();
	
    pConfig->SetState( AP_STATE_LOADING );
	
    ApwMainDlgUpdate();
    ApwUpdateTrayIcon();
	
    // Ensure window is painted before doing long init tasks.
	
    UpdateWindow( hWnd );
	
    // Intialize command module for service control.
	
    if ( FALSE == pCmd->Init( hWnd ) )
    {
        //&? Need error message function? a la old NAVAPW?
		
        auto    APWMESSAGEINFO  rMsgInfo;
		
        memset( &rMsgInfo, 0, sizeof( rMsgInfo ) );
		

        rMsgInfo.hInstance = _Module.GetResourceInstance();
        rMsgInfo.hParent   = hWnd;
        rMsgInfo.uMessage  = IDS_ERR_LOAD_DRIVER;
        rMsgInfo.uTitle    = 0;
		
        ApwMessage( &rMsgInfo );
    }
	
    pConfig->SetState( pCmd->GetStatusOfDriver() );
	
    ApwMainDlgUpdate();
    ApwUpdateTrayIcon();
	
    // If started before the shell, set up a timer so we can minimize
    // into the tray once it appears.
	
    if ( FALSE == ShellTrayPresent() )
    {
        SetTimer( hWnd, TRAY_TIMER_ID, TRAY_TIMER_TIMEOUT, ApwTrayTimer );
    }
	
    return ( 0 );
} // ApwMainAPWM_INITDIALOG()


//************************************************************************
// ApwMainAPWM_ACTIVATE()
//
// static LRESULT ApwMainAPWM_ACTIVATE (
//      HWND    hWnd,
//      WPARAM  wParam,
//      LPARAM  lParam );
//
// This routine restores and activates the NAVAPW main window in
// response to the APWM_ACTIVATE custom message.
//
// Returns:
//      0 Always.
//************************************************************************
// 05/18/1997 DALLEE, created.
//************************************************************************

static LRESULT ApwMainAPWM_ACTIVATE ( HWND      hWnd,
									 WPARAM    wParam,
									 LPARAM    lParam )
{
    ApwActivate();
	
    return ( 0 );
} // ApwMainAPWM_ACTIVATE()


//************************************************************************
// ApwMainAPWM_SHUTDOWN()
//
// static LRESULT ApwMainAPWM_SHUTDOWN (
//      HWND    hWnd,
//      WPARAM  wParam,
//      LPARAM  lParam );
//
// Handle requests from other threads in NAVAPW to shutdown windows.
// Backend implementation for ApwShutdown. Please do not post this
// message directly.
//
// Returns:
//      0 Always.
//************************************************************************
// 05/18/1997 DALLEE, created.
//************************************************************************

static LRESULT ApwMainAPWM_SHUTDOWN ( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
    auto    CApwConfig  *pConfig;
    auto    CApwCmd     *pCmd;
	
    // Get configuration.
	
    pConfig = ApwGetConfig();
    pCmd    = pConfig->GetCmd();
	
    if ( ( APWM_SHUTDOWN_MAGIC_WPARAM != wParam ) ||
		( APWM_SHUTDOWN_MAGIC_LPARAM != lParam ) )
    {
        // Someone sent a shutdown message w/o the magic passwords.
		
        SYM_ASSERT( FALSE );
        goto BailOut;
    }
	
    // Shutdown Windows.
	
    if ( FALSE == pConfig->GetDoingShutdown() )
    {
        pConfig->SetDoingShutdown( TRUE );
		
        ApwReplaceShutdownBitmaps();
        pCmd->DeInit();
		
        // Klugery: Show and restore main window before shutdown.
        // Main thread yields in ExitWindowsEx() and never wakes up
        // otherwise. Explanation anyone?
		
        ApwActivate();
		
       CCTRACEI( "NAVAP : ApwMainAPWM_SHUTDOWN() : ExitWindowsEx()" );
        ExitWindowsEx( EWX_FORCE | EWX_SHUTDOWN, 0 );
    }
	
BailOut:
    return ( 0 );
} // ApwMainAPWM_SHUTDOWN()


//************************************************************************
// ApwMainAPWM_UPDATE_UI()
//
// static LRESULT ApwMainAPWM_UPDATE_UI (
//      HWND    hWnd,
//      WPARAM  wParam,
//      LPARAM  lParam );
//
// Handle notification to resync UI with current state of driver.
// Service may send this message if it may have changed the current state
// of the driver.
//
// Returns:
//      0 Always.
//************************************************************************
// 05/18/1997 DALLEE, created.
//************************************************************************

static LRESULT ApwMainAPWM_UPDATE_UI ( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
    auto    CApwConfig  *pConfig;
    auto    CApwCmd     *pCmd;
	
    // Get configuration.
	
    pConfig = ApwGetConfig();
    pCmd    = pConfig->GetCmd();
	
    // Get current status and update UI.
	
    pConfig->SetState( pCmd->GetStatusOfDriver() );
	
    ApwMainDlgUpdate();
    ApwUpdateTrayIcon();
	
    return ( 0 );
} // ApwMainAPWM_UPDATE_UI()


//************************************************************************
// ApwMainAPWM_COM_TASKS()
//
// static LRESULT ApwMainAPWM_COM_TASKS (
//      HWND    hWnd,
//      WPARAM  wParam,
//      LPARAM  lParam );
//
// This routine performs com tasks upon requests by the driver's callback.
// It currently processes the items in quarantine's incoming directory.
//
// Returns:
//      0 Always.
//************************************************************************
// 05/27/1998 RSTANEV, created.
//************************************************************************

static LRESULT ApwMainAPWM_COM_TASKS ( HWND      hWnd,
                                      WPARAM    wParam,
                                      LPARAM    lParam )
{
    auto    CApwConfig      *pConfig;
    auto    CApwCmd         *pCmd;
    auto    IQuarantineDLL  *pIQuarantine;
	QuarDLL_Loader	QuarLoader;

    if ( VER_PLATFORM_WIN32_NT == GetWindowsPlatform() )
	{
		// Processing quarantine items is done in the NAVAP service
		// on NT systems.
		return 0;
	}
	
    // Get configuration.
	
    pConfig = ApwGetConfig();
    pCmd    = pConfig->GetCmd();
	
    // Notify the sender of the message that we have received it.
	
    pCmd->ConfirmApwComTasksReceived();
	
    // Now we can process the quarantine items.  Note, that this whole thing
    // of sending a message is just to work around a problem, where
    // CoCreateInstance() once used in a thread other than the main APW
    // thread, it causes RPC to hang on shutdown.
	
    // Create the quarantine server object.
    if(SYM_SUCCEEDED(QuarLoader.CreateObject(&pIQuarantine)) &&
        pIQuarantine != NULL )
    {
        if ( SUCCEEDED( pIQuarantine->Initialize() ) )
        {
            HRESULT hRes = S_OK;
            if( FAILED(hRes = pIQuarantine->ProcessAllIncomingFiles()) )
                CCTRACEE(_T("ApwMainAPWM_COM_TASKS() - Failed process all incoming objects. Error = 0x%X"), hRes);
        }
        else
            CCTRACEE(_T("ApwMainAPWM_COM_TASKS() - Failed to initialize the quarantine object"));
		
		pIQuarantine->Release();
        pIQuarantine = NULL;
    }
    else
        CCTRACEE(_T("ApwMainAPWM_COM_TASKS() - Failed to create the quarantine object"));
	
	
    return ( 0 );
} // ApwMainAPWM_COM_TASKS()


//************************************************************************
// ApwReplaceShutdownBitmaps()
//
// This routine replaces the standard Windows shutdown bitmaps with
// NAV's custom virus found bitmaps.
// Should be called when shutting down Windows 95 due to a virus.
//
// Returns:
//  TRUE on success,
//  FALSE otherwise.
//************************************************************************
// 06/16/1997 DALLEE, guts taken from NavHaltSystem() in NAVXUTIL.
//************************************************************************

static BOOL ApwReplaceShutdownBitmaps( VOID )
{
    auto    TCHAR       szWinDir [ MAX_PATH ];
	
    auto    TCHAR       szNavFile [ MAX_PATH ];
    auto    TCHAR       szWinFile [ MAX_PATH ];
    auto    TCHAR       szSaveFile [ MAX_PATH ];
	
    auto    TCHAR       szWinInitFile [ MAX_PATH ];
	
    auto    TCHAR       szName [ MAX_PATH ];
	
    auto    CApwConfig  *pConfig;
    auto    HINSTANCE   hInstance;
    auto    DWORD       dwResult;
    auto    BOOL        bSuccess;
	
#ifdef _UNICODE
    // Unicode needs some hoop jumping which I haven't implemented to
    // get the conversion to OEM on WritePrivateProfileString().
	
    SYM_ASSERT( FALSE );
    return ( FALSE );
#endif
	
    // Initialize locals.
	
    bSuccess  = FALSE;
    pConfig   = ApwGetConfig();
    SYM_ASSERT( pConfig );
	
    hInstance = pConfig->GetInstance();
    SYM_ASSERT( hInstance );
	
    // Verify Win95 platform.
	
    if ( VER_PLATFORM_WIN32_WINDOWS != GetWindowsPlatform() )
    {
        goto BailOut;
    }
	
    // Get Windows dir and construct various filenames which will be used.
	
    dwResult = GetWindowsDirectory( szWinDir, MAX_PATH );
	
    if ( ( 0 == dwResult ) ||
		( MAX_PATH <= dwResult ) )
    {
        goto BailOut;
    }
	
    // Get WININIT.INI
	
    if ( 0 == LoadString( _Module.GetResourceInstance(), IDS_WININIT_INI, szName, sizeof( szName ) / sizeof( TCHAR ) ) )
        goto BailOut;
    _tcscpy( szWinInitFile, szWinDir );
    ApwNameCat( szWinInitFile, szName );
	
    // Get names of shutdown BMPs: NAV, Win95, SaveWin95.
	
    if ( 0 == LoadString( _Module.GetResourceInstance(), IDS_SHUTDOWN_BANNER_NAV, szName, sizeof( szName ) / sizeof( TCHAR ) ) )
        goto BailOut;
    _tcscpy( szNavFile, szWinDir );
    ApwNameCat( szNavFile, szName );
	
    if ( 0 == LoadString( _Module.GetResourceInstance(), IDS_SHUTDOWN_BANNER_WIN, szName, sizeof( szName ) / sizeof( TCHAR ) ) )
        goto BailOut;
    _tcscpy( szWinFile, szWinDir );
    ApwNameCat( szWinFile, szName );
	
    if ( 0 == LoadString( _Module.GetResourceInstance(), IDS_SHUTDOWN_BANNER_SAVE, szName, sizeof( szName ) / sizeof( TCHAR ) ) )
        goto BailOut;
    _tcscpy( szSaveFile, szWinDir );
    ApwNameCat( szSaveFile, szName );
	
    // Backup the Win95 shutdown BMP file.
	
    CopyFile( szWinFile, szSaveFile, FALSE );
	
    // Copy custom NAV BMP over Win95 shutdown BMP.
	
    CopyFile( szNavFile, szWinFile, FALSE );
	
    // Update the WININIT.INI to restore the Win95 shutdown BMP
    // and delete the saved BMP.
    // Note that WININIT.INI expects OEM data since it is processed
    // before Windows is properly loaded.
	
    CharToOem( szWinFile, szWinFile );
    CharToOem( szSaveFile, szSaveFile );
	
    // Get Wininit.ini rename section name.
	
    if ( 0 == LoadString( _Module.GetResourceInstance(), IDS_WININIT_RENAME, szName, sizeof( szName ) / sizeof( TCHAR ) ) )
        goto BailOut;
	
    // Restore Win95 BMP.
	
    WritePrivateProfileString( szName,
		szWinFile,
		szSaveFile,
		szWinInitFile );
	
    // Delete saved copy.
	
    WritePrivateProfileString( szName,
		szSaveFile,
		_T(""),
		szWinInitFile );
	
	
    // Get names of wait for shutdown BMPs: NAV, Win95, SaveWin95.
	
    if ( 0 == LoadString( _Module.GetResourceInstance(), IDS_SHUTDOWN_WAIT_NAV, szName, sizeof( szName ) / sizeof( TCHAR ) ) )
        goto BailOut;
    _tcscpy( szNavFile, szWinDir );
    ApwNameCat( szNavFile, szName );
	
    if ( 0 == LoadString( _Module.GetResourceInstance(), IDS_SHUTDOWN_WAIT_WIN, szName, sizeof( szName ) / sizeof( TCHAR ) ) )
        goto BailOut;
    _tcscpy( szWinFile, szWinDir );
    ApwNameCat( szWinFile, szName );
	
    if ( 0 == LoadString( _Module.GetResourceInstance(), IDS_SHUTDOWN_WAIT_SAVE, szName, sizeof( szName ) / sizeof( TCHAR ) ) )
        goto BailOut;
    _tcscpy( szSaveFile, szWinDir );
    ApwNameCat( szSaveFile, szName );
	
    // Backup the Win95 wait for shutdown BMP file.
	
    CopyFile( szWinFile, szSaveFile, FALSE );
	
    // Copy custom NAV BMP over Win95 wait for shutdown BMP.
	
    CopyFile( szNavFile, szWinFile, FALSE );
	
    // Update the WININIT.INI to restore the Win95 wait for shutdown BMP
    // and delete the saved BMP.
    // Note that WININIT.INI expects OEM data since it is processed
    // before Windows is properly loaded.
	
    CharToOem( szWinFile, szWinFile );
    CharToOem( szSaveFile, szSaveFile );
	
    // Get Wininit.ini rename section name.
	
    if ( 0 == LoadString( _Module.GetResourceInstance(), IDS_WININIT_RENAME, szName, sizeof( szName ) / sizeof( TCHAR ) ) )
        goto BailOut;
	
    // Restore Win95 BMP.
	
    WritePrivateProfileString( szName,
		szWinFile,
		szSaveFile,
		szWinInitFile );
	
    // Delete saved copy.
	
    WritePrivateProfileString( szName,
		szSaveFile,
		_T(""),
		szWinInitFile );
	
    bSuccess = TRUE;
	
BailOut:
    return ( bSuccess );
} // ApwReplaceShutdownBitmaps()


//************************************************************************
// ApwMainDefaultProc()
//
// static LRESULT ApwMainDefaultProc (
//      HWND    hWnd,                   // Main window.
//      UINT    uMessage,               // Message ID
//      WPARAM  wParam,
//      LPARAM  lParam )
//
// This routine checks whether an unhandled message is one of our
// registered messages for communication with other NAV executables and
// makes the appropriate response.
// If not registered, the message is passed on to DefDlgProc().
//
// Returns:
//      Depends on message.
//************************************************************************
// 06/10/1997 DALLEE, created.
//************************************************************************

static LRESULT ApwMainDefaultProc ( HWND    hWnd,
								   UINT    uMessage,
								   WPARAM  wParam,
								   LPARAM  lParam )
{
    auto    LRESULT     lResult;
	
    auto    CApwConfig  *pConfig;
    auto    CApwCmd     *pCmd;
	
    // Get configuration.
	
    pConfig = ApwGetConfig();
    pCmd    = pConfig->GetCmd();
	
    SYM_ASSERT( pConfig );
    SYM_ASSERT( pCmd );
	
    // Custom message used in communication with NAVW.
	
    if ( pConfig->GetCustomMessageID() == uMessage )
    {
        lResult = TRUE;
		
        switch ( wParam )
        {
        case NAVW_CMD_SHOWHIDEICON:
            pConfig->SetShowIcon( lParam ? TRUE : FALSE );
            ApwUpdateTrayIcon();
            break;
			
        case NAVW_CMD_CANBEDISABLED:
            pConfig->SetCanDisable( lParam ? TRUE : FALSE );
            ApwMainDlgUpdate();
            break;
			
        case NAVW_CMD_UNLOADNAVAPW:
            pCmd->DisableDriver();
            g_bOKToClose = true;
            DestroyWindow( hWnd );
            break;
			
        default:
            // Somebody sent a seemingly bogus message.
            // Is this something new that needs to be handled?
			
            SYM_ASSERT( FALSE );
            break;
        }
    }
    else
    {
        lResult = DefDlgProc( hWnd, uMessage, wParam, lParam );
    }
	
    return ( lResult );
} // ApwMainDefaultProc()


//************************************************************************
// ApwMainDlgUpdate()
//
// static VOID ApwMainDlgUpdate ( VOID );
//
// This updates the main dialog controls based on the current state of
// the driver.
// Note that this routine uses the current value in the config object.
// It does not directly query the driver, nor set the config value.
//
// Returns:
//************************************************************************
// 05/18/1997 DALLEE, created.
//************************************************************************

static VOID ApwMainDlgUpdate ( VOID )
{
#define MAX_BUTTON_TEXT 80
#define MAX_STATUS_TEXT 200
	
    auto    CApwConfig      *pConfig;
    auto    HWND            hWnd;
    auto    HINSTANCE       hInst;
	
    auto    TCHAR           szText [ max( MAX_BUTTON_TEXT, MAX_STATUS_TEXT ) + 1 ];
    auto    BOOL            bEnableDisableActive;
    auto    UINT            uEnableID;
    auto    UINT            uStatusID;
    auto    UINT            uIcon;
    auto    HICON           hIcon;
	auto	bool			b16color = true;
	auto	CString csFormat;
	
    // Initialize locals.
	
    pConfig = ApwGetConfig();
    hWnd    = pConfig->GetMainWnd();
    hInst   = pConfig->GetInstance();
	
	// On non-WinXP systems, need to use 16 color icon.
	// Windows XP == Windows version 5.1...
	DWORD dwVersion = GetVersion();
	if( dwVersion < 0x80000000 &&  // NT
		LOBYTE( LOWORD( dwVersion ) ) >= 5 && HIBYTE( LOWORD(dwVersion)) >= 1 )	 // Windows XP
	{
		b16color = false;
	}
	
	// Get enabled/disabled UI ID's.
	
    switch ( pConfig->GetState() )
    {
    case AP_STATE_ENABLED:
        uEnableID = IDS_MAIN_DISABLE_BUTTON;
        uStatusID = IDS_MAIN_STATUS_ENABLED;
        uIcon     = b16color ? IDI_APW_APP_ENABLED_16 : IDI_APW_APP_ENABLED;
        bEnableDisableActive = pConfig->GetCanDisable();
        break;
		
    case AP_STATE_LOADING:
        uEnableID = IDS_MAIN_DISABLE_BUTTON;
        uStatusID = IDS_MAIN_STATUS_LOADING;
        uIcon     = b16color ? IDI_APW_APP_ENABLED_16 : IDI_APW_APP_ENABLED;
        bEnableDisableActive = FALSE;
        break;
		
    case AP_STATE_DISABLED:
    default:
        uEnableID = IDS_MAIN_ENABLE_BUTTON;
        uStatusID = IDS_MAIN_STATUS_DISABLED;
        uIcon     = b16color ? IDI_APW_APP_DISABLED_16 : IDI_APW_APP_DISABLED;
        bEnableDisableActive = TRUE;
        break;
    }
	
	
    // Set Enable/Disable button text.
    // Also disable "Disable" button according to options.
    if ( 0 != LoadString( _Module.GetResourceInstance(),
		uEnableID,
		szText,
		sizeof( szText ) / sizeof( TCHAR ) ) )
    {
        SetDlgItemText( hWnd, IDC_MAIN_ENABLE, szText );
    }
	
    EnableWindow( GetDlgItem( hWnd, IDC_MAIN_ENABLE ), bEnableDisableActive );
	
    // Set main status text.
	csFormat.LoadString(_Module.GetResourceInstance(), uStatusID);
	wsprintf(szText, csFormat, g_csProductName);		
    SetDlgItemText( hWnd, IDC_MAIN_STATUS, szText );
	
    // Kluge to make icons repaint in NT 3.51. Set icons to NULL
    // before doing InvalidateRect().
	
    SetClassLong( hWnd, GCL_HICON, ( LONG ) NULL );
    SetClassLong( hWnd, GCL_HICONSM, ( LONG ) NULL );
	
    InvalidateRect( hWnd, NULL, FALSE );
	
    // Set large icon.
	
    hIcon = LoadIcon( hInst, MAKEINTRESOURCE( uIcon ) );
	
    Static_SetIcon( GetDlgItem( hWnd, IDC_MAIN_ICON), hIcon );
    SetClassLong( hWnd, GCL_HICON, ( LONG ) hIcon );
    pConfig->SetIcon( hIcon );
	
    // Set small icon.
	
    hIcon = ( HICON ) LoadImage ( hInst,
		MAKEINTRESOURCE( uIcon ),
		IMAGE_ICON,
		16, 16, 0 );
	
	
    SetClassLong( hWnd, GCL_HICONSM, ( LONG ) hIcon );
    pConfig->SetSmallIcon( hIcon );
} // ApwMainDlgUpdate()

//************************************************************************
// ApwTrayTimer()
//
// static VOID CALLBACK ApwTrayTimer (
//      HWND     hWnd,                  // Main window
//      UINT     uMessage,              // WM_TIMER
//      UINT     uId,                   // Timer ID
//      DWORD    dwTime )               // Current system time
//
// This is the callback routine for the timer which waits for the
// shell tray window to appear.
// If NAVAPW is started before the shell and the tray window are present,
// a timer is started and this routine is called to check for the
// tray and add our icon and fix the minimize/hide state of the main
// window.
//
// Returns:
//************************************************************************
// 06/11/1997 DALLEE, created.
//************************************************************************

static VOID CALLBACK ApwTrayTimer( HWND     hWnd,
								  UINT     uMessage,
								  UINT     uId,
								  DWORD    dwTime )
{
    // Check if shell tray has appeared yet.
	
    if ( ShellTrayPresent() )
    {
        // Update tray icon.
		
        ApwUpdateTrayIcon();
		
        // If main window was minimized, hide instead now that tray is present.
		
        if ( IsIconic( hWnd ) )
        {
            ApwMinimize();
        }
		
        KillTimer( hWnd, uId );
    }
} // ApwTrayTimer()


//************************************************************************
// HWIsNEC()
//
// static BOOL HWIsNEC()
//
// Added this function here so that the agent doesn't need to link to 
// Quake to get it...
//
// Returns: TRUE if running on an NEC -
//          FALSE otherwise
//
//************************************************************************

static BOOL HWIsNEC()
{
	return ( GetKeyboardType(0) == 7 && GetKeyboardType(1) >= 0x0d01 );
}

//************************************************************************
// ApwMainGetCmdObject()
//
// static CApwCmd *ApwMainGetCmdObject()
//
// This routine loads the appropriate APWCMD dll and returns an allocated
// APWCMD object from that DLL.
//
// Returns: Allocated CApwCmd object
//          NULL if an error occurs.
//
//************************************************************************
static CApwCmd *ApwMainGetCmdObject()
{
	// Figure out what we are 
	const char * pDllName;
    if ( VER_PLATFORM_WIN32_NT == GetWindowsPlatform() )
		pDllName = g_szApwNTcmdDll;
	else
		pDllName = g_szApw95cmdDll;
	
	char szFileName[ MAX_PATH ];
	CNAVInfo info;
	strcpy( szFileName,info.GetNAVDir() );
	strcat( szFileName, "\\" );
	strcat( szFileName, pDllName );
	
	
	// Load the DLL.
	HINSTANCE hInst = LoadLibraryEx( szFileName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH );
	if( hInst )
	{
		// Get pointer to DLL entry point.
		typedef CApwCmd* (WINAPI *pfnAPWCMDNEW)();
		pfnAPWCMDNEW p = (pfnAPWCMDNEW) GetProcAddress( hInst, _T( "_ApwCmdNew@0" ) );
		if( p != NULL )
			return p();
        else
            CCTRACEE ("APWMain::Error getting procaddress command file 0x%x", ::GetLastError ());
	}
    else
    {
        CCTRACEE ("APWMain::Error loading command file 0x%x", ::GetLastError ());
    }
	
	// Something bad has happened.
    assert (false);
	return NULL;
}

//************************************************************************
// ProcessCommandLine()
//
// static void ProcessCommandLine( LPSTR pszCommandLine )
//
// This routine processes any command line switches.  Currently does nothing
// on Windows NT based platforms, but on Windows 9x platforms, we support
// the /regserver and /unregserver COM server switches.
//
// Returns: S_OK on success.
//          OLE error on failure.
//
//************************************************************************
static HRESULT ProcessCommandLine( LPSTR pszCommandLine )
{
	// Nothing to do on NT platforms (yet).
	if( GetWindowsPlatform() == VER_PLATFORM_WIN32_NT )
		return S_OK;

	if( pszCommandLine == NULL )
		pszCommandLine = GetCommandLine();

    TCHAR szTokens[] = _T("-/");						 
    LPCTSTR lpszToken = FindOneOf(pszCommandLine, szTokens);
    while (lpszToken != NULL)
    {
        // Unregister ourselves.
		if (lstrcmpi(lpszToken, _T("UnregServer"))==0)
        {
            CCTRACEI(_T("CAPPlugin::ProcessCommandLine() - Unregistering AP COM object."));
            UnRegisterServer();
        }
		
        // Register as Local Server
        if (lstrcmpi(lpszToken, _T("RegServer"))==0)
        {
            CCTRACEI(_T("CAPPlugin::ProcessCommandLine() - Registering AP COM object."));
            RegisterServer();
        }
        
		// Bail out if we get the /embedded command line.
		// we should never be invoked this way.
        if (lstrcmpi(lpszToken, _T("Embedding"))==0)
			return E_FAIL;
		
        lpszToken = FindOneOf(lpszToken, szTokens);
    }
	
	// No recognized command line switches.   
	return S_OK;
}

//************************************************************************
// RegisterServer()
//
// static HRESULT RegisterServer()
//
// This routine registers the COM objects exported by this application
//
// Returns: nothing
//
//************************************************************************
static void RegisterServer()
{
	// Register the type library.
	HRESULT hr = RegisterTlb( TRUE );
	if( FAILED( hr ) )
		ExitProcess( hr );
	
	// Register the class object.
	ExitProcess( RegisterObject( __uuidof( NAVAPService ) ) );
}

//************************************************************************
// UnRegisterServer()
//
// static HRESULT UnRegisterServer()
//
// This routine un-registers the COM objects exported by this application
//
// Returns: nothing.
//
//************************************************************************
static void UnRegisterServer()
{
	// Unregister the type library
	RegisterTlb( FALSE );	
	ExitProcess( UnRegisterObject( __uuidof( NAVAPService ) ) );
}

////////////////////////////////////////////////////////////////////////////
// Function name	: RegisterObject
//
// Description	    : Local helper routine to register an object
//
// Return type		: BOOL TRUE on success
//
// Argument         : REFCLSID rclsid - class to register
//
////////////////////////////////////////////////////////////////////////////
// 2/23/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
static HRESULT RegisterObject( REFCLSID rclsid )
{
    // registers objects supported by this module.
    OLECHAR wCLSIDStr[64];
    TCHAR szCLSID[64];
    TCHAR szRegKey[MAX_PATH];
    DWORD dwResult;
    HKEY hKey;
	HRESULT hr = S_OK;
	
    if( 0 != (StringFromGUID2( rclsid, (LPOLESTR)wCLSIDStr, 64)) )
	{
        // Need to convert to MBCS here.
#ifdef UNICODE
        lstrcpy( szCLSID, wCLSIDStr );
#else
        if( 0 == WideCharToMultiByte( CP_ACP,
			0,
			wCLSIDStr,
			-1,
			szCLSID,
			64,
			NULL,
			NULL) )
		{
            return E_FAIL;
		}
#endif
        wsprintf(szRegKey, _T("CLSID\\%s"), szCLSID);
		
        // Create reg key for this CLSID
        if( ERROR_SUCCESS == ( RegCreateKeyEx( HKEY_CLASSES_ROOT,            
			szRegKey,
			0,
			NULL,
			0,
			KEY_WRITE,
			NULL,
			&hKey,
			&dwResult ) ) )
		{
			// Need to get path to this module.
			TCHAR szPath[SYM_MAX_PATH];
			GetModuleFileName( (HMODULE) g_hInstance, szPath, SYM_MAX_PATH );
			GetShortPathName( szPath, szPath, SYM_MAX_PATH );
			
			// Create all necessary sub-keys.
			if( CreateKey( hKey, _T("LocalServer32"), szPath ) == FALSE ||
				CreateKey( hKey, _T("ProgID"), _T("Navapsvc.NAVAPService.1") ) == FALSE ||
				CreateKey( hKey, _T("TypeLib"), _T("{F743EA98-42BD-4E2C-A221-3F7B646748C7}") ) == FALSE ||
				CreateKey( hKey, _T("VersionIndependentProgID"), _T("Navapsvc.NAVAPService") ) == FALSE )
			{
				hr = E_FAIL;
			}
			
            RegCloseKey(hKey);
		}
	}
	
    return hr;
}

//************************************************************************
// CreateKey()
//
// static BOOL CreateKey( HKEY hKeyParent, LPTSTR szKeyName, LPTSTR szDefaultValue )
//
// This routine creates a registry key with the specified default value.
//
// Returns: TRUE on success, FALSE otherwise.
//
//************************************************************************
static BOOL CreateKey( HKEY hKeyParent, LPTSTR szKeyName, LPTSTR szDefaultValue )
{
	HKEY hKey;
	DWORD dwResult;
	BOOL bRet = FALSE;
	
	// Create the key
    if( ERROR_SUCCESS == ( RegCreateKeyEx( hKeyParent,            
		szKeyName,
		0,
		NULL,
		0,
		KEY_WRITE,
		NULL,
		&hKey,
		&dwResult ) ) )
	{
        // Set default value for the key.
        if( ERROR_SUCCESS == RegSetValueEx( hKey,
			_T(""),
			0,
			REG_SZ,
			(LPBYTE)szDefaultValue,
			lstrlen( szDefaultValue ) + sizeof(TCHAR))) 
		{
            // Clean up and return success.
			bRet = TRUE;
		}
        
		// Cleanup
		RegCloseKey(hKey);
	}
	
	return bRet;
}

////////////////////////////////////////////////////////////////////////////
// Function name	: UnRegisterObject
//
// Description	    : Local helper function to unregister obejcts
//
// Return type		: HRESULT
//
// Argument         : REFCLSID rclsid
//
////////////////////////////////////////////////////////////////////////////
// 2/23/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
HRESULT UnRegisterObject( REFCLSID rclsid )
{
    OLECHAR wCLSIDStr[64];
    TCHAR szCLSID[64];
    TCHAR szRegKey[MAX_PATH];
    HKEY hKey=NULL;
	
    if( 0 != StringFromGUID2( rclsid, (LPOLESTR)wCLSIDStr, 64)) 
    {
		// Need to convert to MBCS here.
#ifdef UNICODE        
		lstrcpy( szCLSID, wCLSIDStr );
#else
		if( 0 == WideCharToMultiByte( CP_ACP, 
			0,
			wCLSIDStr,
			-1,
			szCLSID,
			64, 
			NULL,
			NULL) )
		{
			return E_FAIL;
		}
#endif
		wsprintf(szRegKey, _T("CLSID\\%s"), szCLSID);
		if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_CLASSES_ROOT,
			szRegKey,
			0,
			KEY_WRITE | KEY_ENUMERATE_SUB_KEYS,
			&hKey ))
		{
			// Delete all sub keys first.
			TCHAR szBuffer[MAX_PATH+1];            
			DWORD dwIndex = 0;
			
			// Make sure we have a valid handle first
			if( hKey )
			{
				// Enumerate the keys to delete
				while( RegEnumKey( hKey, dwIndex, szBuffer, MAX_PATH +1 ) == ERROR_SUCCESS)
				{
					// Delete each key until no more left
					if( RegDeleteKey( hKey, szBuffer ) != ERROR_SUCCESS )
						break;
					dwIndex++;
				}
			}
			
			RegCloseKey(hKey);
			
			// Delete the GUID key
			if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_CLASSES_ROOT,
				_T("CLSID"),
				0,
				KEY_WRITE,
				&hKey ))
			{
				RegDeleteKey( hKey, szCLSID );
				RegCloseKey(hKey);
			}
			return S_OK;
		}
    }
    
    return E_FAIL;
}


//************************************************************************
// RegisterTlb()
//
// static HRESULT RegisterTlb()
//
// This routine registers or unregisterd the type library based on 
// the bRegister flag.
//
// Returns: S_OK on success.
//          OLE error on failure.
//
//************************************************************************
static HRESULT RegisterTlb( BOOL bRegister )
{
	HRESULT hr;
	
	if( bRegister )	
	{
		// Get short absolute path to dll.
		TCHAR szPath[SYM_MAX_PATH];
		CNAVInfo NavInfo;
		_tcscpy(szPath,NavInfo.GetNAVDir());
		ApwNameCat( szPath, g_szApw95cmdDll );
		GetShortPathName( szPath, szPath, SYM_MAX_PATH );
		
		// Add the typelib resource ID.
		lstrcat( szPath, _T("\\1") );
		
		// Convert path to wide char.
		int iLength = mbstowcs( NULL, szPath, 0 ) + 1;
		wchar_t* pwPath = new wchar_t[iLength];
		if( pwPath == NULL )
			return E_OUTOFMEMORY;
		ZeroMemory( pwPath, sizeof(wchar_t) * iLength );
		mbstowcs( pwPath, szPath, lstrlen( szPath ) );
		
		// Load the typelibrary.
		ITypeLib* pTlb = NULL;
		hr = LoadTypeLib( (OLECHAR*) pwPath, &pTlb );
		if( SUCCEEDED( hr ) )
		{
			hr = RegisterTypeLib( pTlb, (OLECHAR*) pwPath, NULL );
			pTlb->Release();
		}
		
		// Cleanup
		delete [] pwPath;
	}
	else
	{
		// Unregister the typelib.
		const IID LIBID_NAVAPSVCLib = {0xF743EA98,0x42BD,0x4E2C,{0xA2,0x21,0x3F,0x7B,0x64,0x67,0x48,0xC7}};
		hr = UnRegisterTypeLib(LIBID_NAVAPSVCLib, 1, 0, LOCALE_NEUTRAL, SYS_WIN32);
	}
	
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// FindOneOf()

static LPCTSTR FindOneOf(LPCTSTR p1, LPCTSTR p2)
{
    while (p1 != NULL && *p1 != NULL)
    {
        LPCTSTR p = p2;
        while (p != NULL && *p != NULL)
        {
            if (*p1 == *p)
                return CharNext(p1);
            p = CharNext(p);
        }
        p1 = CharNext(p1);
    }
    return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CheckPassword()

static bool CheckPassword()
{
	// Need to fetch password protection enabled flag
    if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(CLSID_Password) )
    {
        // Invalid signature return failure
        return false;
    }

    CComPtr<IPassword> spNAVPass;
    if (SUCCEEDED(spNAVPass.CoCreateInstance(CLSID_Password, NULL, CLSCTX_INPROC)))
    {
        CComBSTR bstrProductName;
        CComBSTR bstrFeatureName(g_csProductName);
        bstrProductName.LoadString(_Module.GetResourceInstance(), IDS_AUTOPROTECT);
        BOOL bEnabled = FALSE;

        if( SUCCEEDED(spNAVPass->put_ProductID(NAV_CONSUMER_PASSWORD_ID)) &&
            SUCCEEDED(spNAVPass->put_ProductName(bstrProductName)) &&
            SUCCEEDED(spNAVPass->put_FeatureName(bstrFeatureName)) &&
            SUCCEEDED(spNAVPass->get_Enabled(&bEnabled)) )
        {
            if( bEnabled )
            {
                // Reusing the bEnabled variable to check if the password
                if( SUCCEEDED(spNAVPass->Check(&bEnabled)) )
                {
                    // return whether the check succeeded or not
                    return (bEnabled ? true : false);
                }
            }
            else
                return true;
        }
    }

    return true;
}

static HRESULT SetProductTampered(long nErrorId)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		CNAVOptSettingsCache OptFile;
		if(OptFile.Init(NULL, FALSE))
		{
            OptFile.SetValue(DRM::szDRMLicenseState	,DWORD(DJSMAR00_LicenseState_Violated));
			OptFile.SetValue(DRM::szDRMLicenseZone	,DWORD(DJSMAR_LicenseZone_Violated));

			CBusinessRules Rules;
			Rules.Calc(&OptFile);

			OptFile.Save();

			CGlobalEvent OptionsEvent;
#ifdef _UNICODE
			if (OptionsEvent.CreateW(SYM_OPTIONS_CHANGED_EVENT))
#else  _UNICODE
			if (OptionsEvent.Create(SYM_OPTIONS_CHANGED_EVENT))
#endif  _UNICODE
			{
				PulseEvent(OptionsEvent);
			}

		}

		// Show error dialog.
		// Get a NAVError object
		CComBSTR bstrNAVErr(_T("NAVError.NAVCOMError"));
		CComPtr <INAVCOMError> spNavError;

		// Create, Populate, Log, and Display the error
		hrx << spNavError.CoCreateInstance(bstrNAVErr, NULL, CLSCTX_INPROC_SERVER);
		hrx << spNavError->put_ModuleID(AV_MODULE_ID_NAVAPW32);		// Module ID of AV_MODULE_ID_NAVOPTREFRESH
		hrx << spNavError->put_ErrorID(nErrorId); // Error ID is the resource ID for the error message
		hrx << spNavError->put_ErrorResourceID(IDS_NAVERROR_INTERNAL_REINSTALL);
		hrx << spNavError->LogAndDisplay(0);
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);
}
