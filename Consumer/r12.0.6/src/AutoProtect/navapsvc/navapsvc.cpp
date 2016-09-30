// navapsvc.cpp : Implementation of WinMain


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL,
//      run nmake -f navapsvcps.mk in the project directory.

#include "stdafx.h"

#include "resource.h"
#include <initguid.h>
#include "navapsvc.h"
#include "navapcommands.h"				// Commands for service.
#include "navapsvc_i.c"
#include "osinfo.h"
#include "scanqueue.h"
#include "eventdata.h"
#include "AllNavEvents.h"

#include <stdio.h>
#include "NAVAPService.h"

#include "NavSettingsHelperEx.h"
#include "OptNames.h"
#include "const.h"
#include "CAPThreatExclusions.h"

// ccLib delay loader
// This will allow the ccLib dll to delay load at run-time
#include "ccLibDllLink.h"
#include "ccTrace.h"
#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("navapsvc"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

// Crash handler
#include "ccSymCrashHandler.h"
ccSym::CCrashHandler g_CrashHandler;

CServiceModule _Module;
CAtlBaseModule _ModuleRes;
//Resource stuff
#include "ccResourceLoader.h"
#include "..\navapsvcres\resource.h"
#define  SYMC_RESOURCE_DLL _T("navapsvc.loc")
::cc::CResourceLoader g_ResLoader(&_ModuleRes,SYMC_RESOURCE_DLL);

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_NAVAPService, CNAVAPService)
END_OBJECT_MAP()

SYM_OBJECT_MAP_BEGIN()
SYM_OBJECT_MAP_END()

using namespace std;

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

// Although some of these functions are big they are declared inline since they are only used once

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::RegisterServer()

inline HRESULT CServiceModule::RegisterServer(BOOL bRegTypeLib, BOOL bService)
{
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
        return hr;

    // Remove any previous service since it may point to
    // the incorrect file
    Uninstall();

    // Add service entries
    UpdateRegistryFromResource(IDR_Navapsvc, TRUE);

    // Adjust the AppID for Local Server or Service
    CRegKey keyAppID;
    LONG lRes = keyAppID.Open(HKEY_CLASSES_ROOT, _T("AppID"), KEY_WRITE);
    if (lRes != ERROR_SUCCESS)
        return lRes;

    CRegKey key;
    lRes = key.Open(keyAppID, _T("{D5BDD990-589C-44AC-B7B8-B7CB4AF509BA}"), KEY_WRITE);
    if (lRes != ERROR_SUCCESS)
        return lRes;
    key.DeleteValue(_T("LocalService"));

    if (bService)
    {
        key.SetStringValue(_T("LocalService"), _T("navapsvc") );
        key.SetStringValue(_T("ServiceParameters"), _T("-Service") );
        // Create service
        Install();
    }

    // Add object entries
    hr = CComModule::RegisterServer(bRegTypeLib);

    CoUninitialize();
    return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::UnregisterServer()

inline HRESULT CServiceModule::UnregisterServer()
{
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
        return hr;

    // Remove service entries
    UpdateRegistryFromResource(IDR_Navapsvc, FALSE);
    // Remove service
    Uninstall();
    // Remove object entries
    CComModule::UnregisterServer(TRUE);

	// Remove this key. CComModule::UpdateRegistryFromResource() is supposed to do this,
	// but it appears to be broken.
	CRegKey key;
	if( ERROR_SUCCESS == key.Open( HKEY_CLASSES_ROOT, "AppID" ) )
	{
		key.DeleteSubKey( "NAVAPSVC.EXE" );
	}

    CoUninitialize();
    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::Init()

inline void CServiceModule::Init(_ATL_OBJMAP_ENTRY* p, HINSTANCE h, UINT nServiceNameID, const GUID* plibid)
{
    CComModule::Init(p, h, plibid);

    m_bService = TRUE;

    LoadString(_Module.GetResourceInstance (), nServiceNameID, m_szServiceName, sizeof(m_szServiceName));
	LoadString(_Module.GetResourceInstance (), IDS_DISPLAYNAME, m_szDisplayName, sizeof(m_szDisplayName));

    // set up the initial service status
    m_hServiceStatus = NULL;
    m_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    m_status.dwCurrentState = SERVICE_STOPPED;
    m_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    m_status.dwWin32ExitCode = 0;
    m_status.dwServiceSpecificExitCode = 0;
    m_status.dwCheckPoint = 0;
    m_status.dwWaitHint = 0;

    g_hInstance = _Module.GetResourceInstance ();
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::Unlock()

LONG CServiceModule::Unlock()
{
    LONG l = CComModule::Unlock();
    if (l == 0 && !m_bService)
        PostThreadMessage(dwThreadID, WM_QUIT, 0, 0);
    return l;
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::IsInstalled()

BOOL CServiceModule::IsInstalled()
{
    BOOL bResult = FALSE;

    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (hSCM != NULL)
    {
        SC_HANDLE hService = ::OpenService(hSCM, m_szServiceName, SERVICE_QUERY_CONFIG);
        if (hService != NULL)
        {
            bResult = TRUE;
            ::CloseServiceHandle(hService);
        }
        ::CloseServiceHandle(hSCM);
    }
    return bResult;
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::Install()

inline BOOL CServiceModule::Install()
{
    if (IsInstalled())
        return TRUE;

    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCM == NULL)
    {
        if ( m_bDebug )
            MessageBox(NULL, "Couldn't open service manager", m_szServiceName, MB_OK);
        return FALSE;
    }

    // Get the executable file path
    char szFilePath[_MAX_PATH];
    ::GetModuleFileName(NULL, szFilePath, _MAX_PATH);

    SC_HANDLE hService = ::CreateService(
        hSCM, m_szServiceName, m_szDisplayName,
        SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
        SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
        szFilePath, NULL, NULL, "RPCSS\0", NULL, NULL);

    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);

        if (m_bDebug)
            MessageBox(NULL, "Couldn't create service", m_szServiceName, MB_OK);

        return FALSE;
    }

	// Add a desciption for the service if we can.
	HMODULE hMod = GetModuleHandle( "Advapi32" );
	if( hMod )
	{
		// Need to late bind to ChangeServiceConfig2 function.  Only works on Win2k or greater
		// sytems.
        // Note: Using the single-byte char version of this function doesn't seem to work.
		typedef BOOL ( WINAPI *PFNCHANGESERVICECONFIG2)( SC_HANDLE hService, DWORD dwInfoLevel, LPVOID lpInfo );
		PFNCHANGESERVICECONFIG2 pfn = (PFNCHANGESERVICECONFIG2) GetProcAddress( hMod, "ChangeServiceConfig2W" );
		if( pfn )
		{
			WCHAR szBuffer[1024];
			SERVICE_DESCRIPTIONW desc;
			LoadStringW( _Module.GetResourceInstance(), IDS_SERVICE_DESC, szBuffer, 1024 );
			desc.lpDescription = szBuffer;
			pfn( hService, SERVICE_CONFIG_DESCRIPTION, &desc );
		}
	}

    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::Uninstall()

inline BOOL CServiceModule::Uninstall()
{
    if (!IsInstalled())
        return TRUE;

    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (hSCM == NULL)
    {
        if (m_bDebug)
            MessageBox(NULL, "Couldn't open service manager", m_szServiceName, MB_OK);

        return FALSE;
    }

    SC_HANDLE hService = ::OpenService(hSCM, m_szServiceName, SERVICE_STOP | DELETE);

    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);

        if (m_bDebug)
            MessageBox(NULL, "Couldn't open service", m_szServiceName, MB_OK);

        return FALSE;
    }
    SERVICE_STATUS status;
    ::ControlService(hService, SERVICE_CONTROL_STOP, &status);

    BOOL bDelete = ::DeleteService(hService);
    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);

    if (bDelete)
        return TRUE;

    if (m_bDebug)
        MessageBox(NULL, "Service could not be deleted", m_szServiceName, MB_OK);
    return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::LogEvent()

void CServiceModule::LogEvent(LPCSTR pFormat, ...)
{
  	if( !m_bDebug )
		return;

    char    chMsg[256];
    HANDLE  hEventSource;
    LPSTR  lpszStrings[1];
    va_list pArg;

    va_start(pArg, pFormat);
    sprintf(chMsg, pFormat, pArg);
    va_end(pArg);

    lpszStrings[0] = chMsg;

    // Sometimes it's an error, sometimes not.
    CCTRACEI ("NAVAPSVC - %s", lpszStrings[0]);

    if (m_bService)
    {
        USES_CONVERSION;
		/* Get a handle to use with ReportEvent(). */
        hEventSource = RegisterEventSource(NULL, m_szServiceName );
        if (hEventSource != NULL)
        {
            /* Write to event log. */
            ReportEventA(hEventSource, EVENTLOG_INFORMATION_TYPE, 0, 0, NULL, 1, 0, (LPCSTR*) &lpszStrings[0], NULL);
            DeregisterEventSource(hEventSource);
        }
    }
    else
    {
        // As we are not running as a service, just write the error to the console.
        puts(chMsg);
    }
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::Start()

inline void CServiceModule::Start()
{
    SERVICE_TABLE_ENTRY st[] =
    {
        { m_szServiceName, _ServiceMain },
        { NULL, NULL }
    };
    if (m_bService && !::StartServiceCtrlDispatcher(st))
    {
        m_bService = FALSE;
    }
    if (m_bService == FALSE)
        Run();
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::ServiceMain()

inline void CServiceModule::ServiceMain(DWORD /* dwArgc */, LPSTR* /* lpszArgv */)
{
    // Register the control request handler
    m_status.dwCurrentState = SERVICE_START_PENDING;
    m_hServiceStatus = RegisterServiceCtrlHandler(m_szServiceName, _Handler);
    if (m_hServiceStatus == NULL)
    {
        LogEvent("Handler not installed");
        return;
    }
    SetServiceStatus(SERVICE_START_PENDING, 1000 );

    m_status.dwWin32ExitCode = S_OK;
    m_status.dwCheckPoint = 0;
    m_status.dwWaitHint = 0;

    // When the Run function returns, the service has stopped.
    Run();

    SetServiceStatus(SERVICE_STOPPED,1000);
    LogEvent("Service stopped");
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::Handler()

inline void CServiceModule::Handler(DWORD dwOpcode)
{
    switch (dwOpcode)
    {
    case SERVICE_CONTROL_STOP:
        SetServiceStatus(SERVICE_STOP_PENDING, 3000);
		PostThreadMessage(dwThreadID, WM_QUIT, 0, 0);
        break;
    case SERVICE_CONTROL_PAUSE:
        break;
    case SERVICE_CONTROL_CONTINUE:
        break;
    case SERVICE_CONTROL_INTERROGATE:
        break;
    case SERVICE_CONTROL_SHUTDOWN:

        break;
    default:
        LogEvent(_T("Bad service request"));
    }
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::_ServiceMain()

void WINAPI CServiceModule::_ServiceMain(DWORD dwArgc, LPSTR* lpszArgv)
{

    // Load debug options
    if (g_DebugOutput.LoadOptions() == FALSE)
    {
        CCTRACEE("CServiceModule::_ServiceMain() : g_DebugOutput.LoadOptions() == FALSE");
    }

    // Enable the crash handler
    if (g_CrashHandler.LoadOptions() == FALSE)
    {
        CCTRACEE("CServiceModule::_ServiceMain() : g_CrashHandler.LoadOptions() == FALSE");
    }

    _Module.ServiceMain(dwArgc, lpszArgv);
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::_Handler()

void WINAPI CServiceModule::_Handler(DWORD dwOpcode)
{
    _Module.Handler(dwOpcode);
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::SetServiceStatus()

void CServiceModule::SetServiceStatus(DWORD dwState, DWORD dwWaitHint )
{
	if( m_bService == FALSE )
		return;

    m_status.dwCurrentState = dwState;
	m_status.dwWaitHint = dwWaitHint;
    ::SetServiceStatus(m_hServiceStatus, &m_status);
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::Run()

void CServiceModule::Run()
{
	CCTRACEI("Run()");

    _Module.dwThreadID = GetCurrentThreadId();
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    _ASSERTE(SUCCEEDED(hr));

    // This provides a NULL DACL which will allow access to everyone.
    CSecurityDescriptor sd;
    sd.InitializeFromThreadToken();
    hr = CoInitializeSecurity(sd, -1, NULL, NULL,
        RPC_C_AUTHN_LEVEL_PKT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
    _ASSERTE(SUCCEEDED(hr));

    hr = _Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE);
    _ASSERTE(SUCCEEDED(hr));

    LogEvent("Service started");
    if (m_bService)
        SetServiceStatus(SERVICE_RUNNING);

	// Create service running mutex.
	// Figure out which nav running mutex to use.
	COSInfo osInfo;
	std::string sMutex;
	if( COSInfo::IsTerminalServicesInstalled() )
	{
		sMutex = _T("Global\\");
		sMutex += NAVAPSVC_RUNNING_MUTEX;
	}
	else
		sMutex += NAVAPSVC_RUNNING_MUTEX;

	// Create a NULL DACL for the running mutex.
	PSECURITY_DESCRIPTOR pSD;
	SECURITY_ATTRIBUTES sa;
	pSD = (PSECURITY_DESCRIPTOR) LocalAlloc(LPTR,
		  SECURITY_DESCRIPTOR_MIN_LENGTH);
	InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION);

	// Add a null DACL to the security descriptor.
	SetSecurityDescriptorDacl(pSD, TRUE, (PACL) NULL, FALSE);
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = pSD;
	sa.bInheritHandle = TRUE;

	// Finally, create the mutex.
	HANDLE hMutex = CreateMutex( &sa, FALSE, sMutex.c_str() );

	// Surpress critical errors (File system, etc.)
	UINT uErrorMode = SetErrorMode( SEM_FAILCRITICALERRORS );

	// Set up our stuff.
	if ( setupNAVService())
    {
    #if defined( _DEBUG )
	    if( m_bService == FALSE )
	    {
		    // Lock ourselves.
		    CComModule::Lock();
		    MessageBox( NULL, "Click OK to stop.", "Test running", MB_OK );
	    }
	    else
	    {
		    doMessagePump();
	    }
    #else

	    // Force swap of all non-locked memory pages.
	    SetProcessWorkingSetSize( GetCurrentProcess(), -1, -1 );

	    CCTRACEI("Run - listening...");

	    // Start the pump!
	    doMessagePump();
    #endif
    }

	// Restore error mode
	SetErrorMode( uErrorMode );

    _Module.RevokeClassObjects();

	// Destroy the running mutex.
	if( hMutex )
	{
		if( pSD )
			LocalFree( (HLOCAL) pSD );

		CloseHandle( hMutex );
	}

	// Shut things down.
	shutdownNAVService();

    CoUninitialize();
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::doMessagePump()

void CServiceModule::doMessagePump()
{
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		DispatchMessage(&msg);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::InsertHandler()

void CServiceModule::InsertHandler(INAVAPEventHandler* pHandler)
{
    if( pHandler )
    {
	    m_handlers.InsertHandler( pHandler );

        // Make sure this handler knows the current state of the AP drivers to display
        pHandler->OnStateChanged(IsAPEnabled());
    }
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::InsertHandler()

void CServiceModule::RemoveHandler(INAVAPEventHandler* pHandler)
{
	m_handlers.RemoveHandler( pHandler );
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::IsAPEnabled()

bool CServiceModule::IsAPEnabled()
{

    BOOL bTemp;
    if ( _Module.pAPWrapper->GetEnabledState( &bTemp ) == SAVRT_DRIVER_OK )
        return bTemp == TRUE;
    else
        return false;
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::EnableAP()

void CServiceModule::EnableAP( bool bEnable )
{
	CCTRACEI("EnableAP(%d)", bEnable? 1 : 0);

    ccLib::CSingleLock lock( &m_csApDrvComm, INFINITE, FALSE );

    // If there are no good defs installed first attempt to reload to see
    // if a good set is available...give up if the reload fails
    if( bEnable && m_bDefsBad )
    {
        CCTRACEE("CServiceModule::EnableAP(true) - The definitions are not authentic, attempting to Reload AP.");
        if( !ReloadAP() )
        {
            // Alert the user
            defAuthenticationFailure(AUTHENTICATION_FAILED);
            return;
        }
        CCTRACEI("CServiceModule::EnableAP() - ReloadAP() succeeded AP will attempt to enable.");
    }

	if( _Module.pAPWrapper->GetAPRunning() )
	{
        BOOL bState = (bEnable) ? TRUE : FALSE;

		if(bEnable)
		{
			// Check the settings to see if
			//  feature AP is allowed.
            if ( !_Module.Config().GetFeatureEnabled())
                return;
		}

        if ( _Module.pAPWrapper->SetEnabledState( bState ) != SAVRT_DRIVER_OK )
            throw runtime_error( "Error communicating with driver." );

		// Tell all connected clients that the enabled state of AP
		// has changed.
		m_handlers.SendStateChange( bEnable );
	}
	else                                 // No connection yet!
    {
        CCTRACEE("CServiceModule::EnableAP() - No driver communication available.");
		throw runtime_error( "Not communicating with AP driver yet" );
    }
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::Config()

CServiceConfig& CServiceModule::Config()
{
	return m_config;
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::GetThreatExclusions()

CAPThreatExclusions* CServiceModule::GetThreatExclusions()
{
    // No spyware? No exclusions.
    if(!_Module.Config().GetDetectSpyware())
        return NULL;

    // If we haven't created exclusions do it now.
    //
    if ( !m_pAPThreatExclusions.get() )
    {
        // Set up AP Exclusions Wrapper
        m_pAPThreatExclusions = std::auto_ptr<CAPThreatExclusions> (new CAPThreatExclusions());

        if(!m_pAPThreatExclusions.get())
        {
            CCTRACEE( "CServiceModule::GetThreatExclusions - Could not create AP threat exclusions." );
            return NULL;
        }
    }

    if(!m_pAPThreatExclusions->Initialize())
    {
        // delete exclusions
        m_pAPThreatExclusions.reset();
        CCTRACEE( "CServiceModule::GetThreatExclusions - Could not initialize AP threat exclusions." );
        return NULL;
    }

    // Have the exclusions changed since the last time we used them?
    //
    if ( m_bExclusionsDirty )
    {
        CCTRACEI("CServiceModule::GetThreatExclusions - reloading AP Exclusions - start");

        if(!m_pAPThreatExclusions->Reload())
        {
            CCTRACEW(_T("CServiceModule::GetThreatExclusions - Error while "\
                        "reloading exclusions."));
        }
        else
        {
            m_bExclusionsDirty = false;
            CCTRACEI(_T("CServiceModule::GetThreatExclusions - Validating the PVID exclusions list after exclusions reload."));
        }

        CCTRACEI("CServiceModule::GetThreatExclusions - reloading AP Exclusions - stop");
    }

    return m_pAPThreatExclusions.get();
}

void CServiceModule::SetExclusionsDirty ()
{
    m_bExclusionsDirty = true;
}

void CServiceModule::StopUsingExclusions ()
{
    // Exclusions might be used by other threads right now (Notification.cpp, etc.).
    // Let's not delete the exclusions for safeties sake, just uninit them.
    if ( m_pAPThreatExclusions.get())
    {
        if (!m_pAPThreatExclusions->Uninitialize())
            CCTRACEW(_T("CServiceModule::StopUsingExclusions - Error while uninitializing exclusions."));
    }
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::CancelScan()

HRESULT CServiceModule::CancelScan(VARIANT& vCancelData )
{
	LogEvent( "About to cancel scan" );
    CCTRACEI ("CServiceModule::Cancelling scan");

    // Make sure we have the correct data.
	if( vCancelData.vt != ( VT_ARRAY | VT_UI1 ) )
		return E_INVALIDARG;

    CEventData eventData ( vCancelData.parray );
    SAVRT_ROUS64 hCookie = 0;
    long lSize = sizeof(SAVRT_ROUS64);
    if ( eventData.GetData ( AV::Event_Container_Cookie, (BYTE*) &hCookie, lSize ))
    {
        if ( _Module.pAPWrapper->CancelSemiSynchronousScan( hCookie) != SAVRT_DRIVER_OK )
        {
            throw runtime_error( "Error communicating with driver." );
            return E_FAIL;
        }

        return S_OK;
    }

    return E_INVALIDARG;
}

HRESULT CServiceModule::ManualScanSpywareDetection(VARIANT& vSpywareItemData)
{
    CCTRACEI ("CServiceModule::ManualScanSpywareDetection() - sending notification to handlers");
    m_handlers.BroadcastData(vSpywareItemData);
    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::GetAppDir()

std::string CServiceModule::GetAppDir()
{
	// Get our file name.
	TCHAR szApp[ _MAX_PATH ];
	GetModuleFileName( NULL, szApp, _MAX_PATH );

	// strip off file name
	std::string::size_type pos = std::string( szApp ).find_last_of( _T('\\') );
	if( pos == std::string::npos )
		throw runtime_error("Error parsing filename.");

	// Truncate string
	szApp[pos] = _T('\0');

	return szApp;
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::WinMain()

extern "C" int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE /*hPrevInstance*/, LPSTR lpCmdLine, int /*nShowCmd*/)
{
    lpCmdLine = GetCommandLine(); //this line necessary for _ATL_MIN_CRT
	g_ResLoader.Initialize();
	_Module.SetResourceInstance(g_ResLoader.GetResourceInstance());
    _Module.Init(ObjectMap, hInstance, IDS_SERVICENAME, &LIBID_NAVAPSVCLib);
    _Module.m_bService = TRUE;

    char szTokens[] = _T("-/");

    LPCSTR lpszToken = FindOneOf(lpCmdLine, szTokens);
    while (lpszToken != NULL)
    {
        if (strcmpi(lpszToken, "UnregServer")==0)
            return _Module.UnregisterServer();

        // Register as Local Server
        if (strcmpi(lpszToken, "RegServer")==0)
            return _Module.RegisterServer(TRUE, FALSE);

        // Register as Service
        if (strcmpi(lpszToken, "Service")==0)
            return _Module.RegisterServer(TRUE, TRUE);

        // Check for debug flag
        if (strcmpi(lpszToken, "Debug")==0)
            _Module.m_bDebug = true;

        lpszToken = FindOneOf(lpszToken, szTokens);
    }

    // Are we Service or Local Server
    CRegKey keyAppID;
    LONG lRes = keyAppID.Open(HKEY_CLASSES_ROOT, _T("AppID"), KEY_READ);
    if (lRes != ERROR_SUCCESS)
        return lRes;

    CRegKey key;
    lRes = key.Open(keyAppID, _T("{D5BDD990-589C-44AC-B7B8-B7CB4AF509BA}"), KEY_READ);
    if (lRes != ERROR_SUCCESS)
        return lRes;

    TCHAR szValue[_MAX_PATH];
    DWORD dwLen = _MAX_PATH;
    lRes = key.QueryStringValue(_T("LocalService"), szValue, &dwLen);

    _Module.m_bService = FALSE;
    if (lRes == ERROR_SUCCESS)
        _Module.m_bService = TRUE;

    _Module.Start();

    // When we get here, the service has been stopped
    return _Module.m_status.dwWin32ExitCode;
}

