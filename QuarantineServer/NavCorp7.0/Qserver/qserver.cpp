// qserver.cpp : Implementation of WinMain


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f qserverps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "qserver.h"
#include "serverthreads.h"

#include "qserver_i.c"
#include "const.h"


#include <stdio.h>
#include "QuarantineServer.h"
#include "QuarantineServerItem.h"
#include "EnumQserverItemsVariant.h"
#include "QserverConfig.h"
#include "CopyItemData.h"

CServiceModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_QuarantineServer, CQuarantineServer)
OBJECT_ENTRY(CLSID_QuarantineServerItem, CQuarantineServerItem)
OBJECT_ENTRY(CLSID_EnumQserverItemsVariant, CEnumQserverItemsVariant)
OBJECT_ENTRY(CLSID_QserverConfig, CQserverConfig)
OBJECT_ENTRY(CLSID_CopyItemData, CCopyItemData)
END_OBJECT_MAP()


LPCTSTR FindOneOf(LPCTSTR p1, LPCTSTR p2)
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

inline HRESULT CServiceModule::RegisterServer(BOOL bRegTypeLib, BOOL bService)
{
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
        return hr;

    // Remove any previous service since it may point to
    // the incorrect file
    Uninstall();

    // Add service entries
    UpdateRegistryFromResource(IDR_Qserver, TRUE);

    // Adjust the AppID for Local Server or Service
    CRegKey keyAppID;
    LONG lRes = keyAppID.Open(HKEY_CLASSES_ROOT, _T("AppID"), KEY_WRITE);
    if (lRes != ERROR_SUCCESS)
        return lRes;

    CRegKey key;
    lRes = key.Open(keyAppID, _T("{C60B0154-894B-11D2-8E5F-2A0712000000}"), KEY_WRITE);
    if (lRes != ERROR_SUCCESS)
        return lRes;
    key.DeleteValue(_T("LocalService"));
    
    if (bService)
    {
        key.SetValue(_T("qserver"), _T("LocalService"));
        key.SetValue(_T("-Service"), _T("ServiceParameters"));
        // Create service
        Install();
    }

    // Add object entries
    hr = CComModule::RegisterServer(bRegTypeLib);

    // 
    // Register ourselves with the event logging service.
    //
    CRegKey eventLogKey;
    TCHAR szBuffer[ MAX_PATH * 2 ];
    DWORD dwData;
    wsprintf( szBuffer, _T("SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\%s"), m_szEventLogServiceName );
    lRes = eventLogKey.Create( HKEY_LOCAL_MACHINE, szBuffer );
    if( lRes != ERROR_SUCCESS )
        return lRes;

    // 
    // Set up path.
    // 
    GetModuleFileName( NULL, szBuffer, MAX_PATH );
    eventLogKey.SetValue( szBuffer, _T("EventMessageFile") );

    // 
    // Set up event types.
    // 
    dwData = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE;
    eventLogKey.SetValue( dwData, _T("TypesSupported") );



    CoUninitialize();
    return hr;
}

inline HRESULT CServiceModule::UnregisterServer()
{
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
        return hr;

    // Remove service entries
    UpdateRegistryFromResource(IDR_Qserver, FALSE);

    // 
    // Remove server snapin guid.
    // 
    CRegKey reg;
    if( ERROR_SUCCESS == reg.Open( HKEY_LOCAL_MACHINE, _T("System\\CurrentControlSet\\Control\\Server Applications"),KEY_WRITE ) ) 
        {
        reg.DeleteValue( _T("{26140C85-B554-11D2-8F45-3078302C2030}") );
        reg.Close();
        }
    

    // Remove service
    Uninstall();
    // Remove object entries
    CComModule::UnregisterServer(TRUE);

    // 
    // Need to remove event log registration.
    // 
    CRegKey eventLogKey;
    if( ERROR_SUCCESS == eventLogKey.Open( HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application"), KEY_WRITE ) )
        {
        eventLogKey.DeleteSubKey( m_szEventLogServiceName );
        }

    CoUninitialize();
    return S_OK;
}

inline void CServiceModule::Init(_ATL_OBJMAP_ENTRY* p, HINSTANCE h, UINT nServiceNameID, const GUID* plibid)
{
    CComModule::Init(p, h, plibid);

    m_bService = TRUE;

    LoadString(h, nServiceNameID, m_szServiceName, sizeof(m_szServiceName) / sizeof(TCHAR));
    LoadString( h, IDS_LOG_SERVICE_NAME, m_szEventLogServiceName, sizeof( m_szEventLogServiceName ) / sizeof( TCHAR ) );

    // set up the initial service status 
    m_hServiceStatus = NULL;
    m_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    m_status.dwCurrentState = SERVICE_STOPPED;
    m_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    m_status.dwWin32ExitCode = 0;
    m_status.dwServiceSpecificExitCode = 0;
    m_status.dwCheckPoint = 0;
    m_status.dwWaitHint = 0;

    m_pThreadPool = NULL;
    m_hStopping = NULL;
}

LONG CServiceModule::Unlock()
{
    LONG l = CComModule::Unlock();
    if (l == 0 && !m_bService)
        PostThreadMessage(dwThreadID, WM_QUIT, 0, 0);
    return l;
}

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

inline BOOL CServiceModule::Install()
{
    if (IsInstalled())
        return TRUE;

    TCHAR szMessage[ MAX_PATH ];

    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCM == NULL)
    {
        LoadString( GetResourceInstance(), ID_ERR_NO_SCM, szMessage, MAX_PATH );
        MessageBox(NULL, szMessage, m_szServiceName, MB_ICONSTOP | MB_OK);
        return FALSE;
    }

    // Get the executable file path
    TCHAR szFilePath[_MAX_PATH];
    ::GetModuleFileName(NULL, szFilePath, _MAX_PATH);

    // 
    // Load display name of our service.
    // 
    CComBSTR bStr;
    bStr.LoadString( IDS_SERVICE_DISPLAY_NAME );

    SC_HANDLE hService = ::CreateService(
        hSCM, m_szServiceName, bStr,
        SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
        SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
        szFilePath, NULL, NULL, _T("RPCSS\0"), NULL, NULL);

    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);
        LoadString( GetResourceInstance(), ID_ERR_NO_SERVICE, szMessage, MAX_PATH );
        MessageBox(NULL, szMessage, m_szServiceName, MB_ICONSTOP | MB_OK);
        return FALSE;
    }

    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);
    return TRUE;
}

inline BOOL CServiceModule::Uninstall()
{
    if (!IsInstalled())
        return TRUE;

    TCHAR szMessage[ MAX_PATH ];

    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (hSCM == NULL)
    {
        LoadString( GetResourceInstance(), ID_ERR_NO_SCM, szMessage, MAX_PATH );
        MessageBox(NULL, szMessage, m_szServiceName, MB_ICONSTOP | MB_OK);
        return FALSE;
    }

    SC_HANDLE hService = ::OpenService(hSCM, m_szServiceName, SERVICE_STOP | DELETE);

    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);
        LoadString( GetResourceInstance(), ID_ERR_NO_SERVICE, szMessage, MAX_PATH );
        MessageBox(NULL, szMessage, m_szServiceName, MB_ICONSTOP | MB_OK);
        return FALSE;
    }
    SERVICE_STATUS status;
    ::ControlService(hService, SERVICE_CONTROL_STOP, &status);

    BOOL bDelete = ::DeleteService(hService);
    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);

    if (bDelete)
        return TRUE;

    LoadString( GetResourceInstance(), ID_ERR_DELETING_SERVICE, szMessage, MAX_PATH );
    MessageBox(NULL, szMessage, m_szServiceName, MB_ICONSTOP | MB_OK);
    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////
// Logging functions

////////////////////////////////////////////////////////////////////////////
// Function name    : CServiceModule::LogEvent
//
// Argument         : LPSTR lpszParam1
// Argument         : LPSTR lpszParam2
// Argument         : LPSTR lpszParam3
// Argument         : LPSTR lpszParam4
//
////////////////////////////////////////////////////////////////////////////
// 12/10/97 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CServiceModule::LogEvent( DWORD dwMsgID,
					   WORD wType /* = EVENTLOG_ERROR_TYPE */, 					   
					   LPSTR lpszParam1 /* = NULL */,
					   LPSTR lpszParam2 /* = NULL */,
					   LPSTR lpszParam3 /* = NULL */,
					   LPSTR lpszParam4 /* = NULL */ )
{
	auto	LPSTR	aParams[ 4 ];
	auto	WORD	wParamCount = 0;
	auto	HANDLE	hEventSource;
    

	// 
    // Construct paramater array.
    // 
	aParams[0] = lpszParam1;
	aParams[1] = lpszParam2;
	aParams[2] = lpszParam3;
	aParams[3] = lpszParam4;

	// 
    // Compute number of parameters.
    //
	for( int i = 0; i < 4; i++ )
		wParamCount += (aParams[i] == NULL) ? 0 : 1;
	
	// 
    // Set ourselves up with the event mangaer
    // 
	hEventSource = RegisterEventSource( NULL, m_szEventLogServiceName );
	if( hEventSource == NULL )
		{
		// 
        // Badness occured.  Too bad.
        // 
		ATLTRACE( "Error reporting event." );
		return;
		}

	// 
    // Report event
    // 
	if( !ReportEvent (
		  hEventSource,             // Handle to Event Source
		  wType,					// Event type to log
		  wType,                    // Event category
		  dwMsgID,					// Event ID (from the MC file)
		  NULL,                     // Security ID
		  wParamCount,              // Number of strings
		  0,
		  (LPCTSTR * )aParams,       // Array of strings to merge with MSG
		  NULL) )                   // Address of RAW data (not used)
				 
		{
		// 
        // Badness occured.  Too bad.
        // 
		ATLTRACE( "Error reporting event." );
		}

	// 
    // Cleanup
	// 
	DeregisterEventSource( hEventSource );

}




//////////////////////////////////////////////////////////////////////////////////////////////
// Service startup and registration
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

inline void CServiceModule::ServiceMain(DWORD /* dwArgc */, LPTSTR* /* lpszArgv */)
{
    // Register the control request handler
    m_status.dwCurrentState = SERVICE_START_PENDING;
    m_hServiceStatus = RegisterServiceCtrlHandler(m_szServiceName, _Handler);
    if (m_hServiceStatus == NULL)
    {
        return;
    }
    SetServiceStatus(SERVICE_START_PENDING);

    m_status.dwWin32ExitCode = S_OK;
    m_status.dwCheckPoint = 0;
    m_status.dwWaitHint = 0;

    //
    // When the Run function returns, the service has stopped.
    //
    Run();
    
    m_status.dwWaitHint = 1000;
    SetServiceStatus(SERVICE_STOPPED);
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name: CServiceModule::InitializeQserver
//
// Description: 
//
// Return type: BOOL 
//
//
///////////////////////////////////////////////////////////////////////////////
// 12/19/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CServiceModule::InitializeQserver()
{
    BOOL bRet = TRUE;


    // 
    // Get location of quarantine folder from registry.
    // 
    CRegKey reg;
    if( ERROR_SUCCESS != reg.Create( HKEY_LOCAL_MACHINE, REGKEY_QSERVER, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_READ ) )
        return FALSE;

    DWORD dwSize = MAX_PATH;
    if( ERROR_SUCCESS != reg.QueryValue( m_szQuarantineFolder, REGVALUE_QUARANTINE_FOLDER, &dwSize ) ||
        ERROR_SUCCESS != reg.QueryValue( (DWORD&)m_iWinsockPort, REGVALUE_PORT ) ||
        ERROR_SUCCESS != reg.QueryValue( (DWORD&)m_iSPXSocket, REGVALUE_SPXSOCKET ) ||
        ERROR_SUCCESS != reg.QueryValue( (DWORD&)m_bListenIP, REGVALUE_LISTEN_IP ) ||
        ERROR_SUCCESS != reg.QueryValue( (DWORD&)m_bListenSPX, REGVALUE_LISTEN_SPX ) ||
        ERROR_SUCCESS != reg.QueryValue( (DWORD&)m_uDiskQuota, REGVALUE_MAX_SIZE ) ||
        ERROR_SUCCESS != reg.QueryValue( (DWORD&)m_uFileQuota, REGVALUE_MAX_FILES ))
        {
        LogEvent( IDM_INFO_NO_CONFIG, EVENTLOG_INFORMATION_TYPE );
        }


    // 
    // Initialize Winsock
    // 
    WSADATA wsData;
    ZeroMemory( &wsData, sizeof( WSADATA ) );
    if( WSAStartup( MAKEWORD(2,2), &wsData ) )
        {
        // 
        // Log error here.
        // 
        LogEvent( IDM_ERR_INITIALIZING_WINSOCK );
        return FALSE;
        }

    // 
    // No outstand config objects for now.
    // 
    m_iConfigCount = 0;

    // 
    // Get current size of quarantine.
    // 
    CountQserverFileBytes();

    // 
    // Create our start/stop event
    // 
    m_hStopping = CreateEvent( NULL, TRUE, FALSE, NULL );

    // 
    // Create configuration event.
    // 
    m_hConfigEvent = CreateEvent( NULL,TRUE, FALSE, FALSE );

    // 
    // Create socket reconfig event.
    // 
    m_hSocketConfig = CreateEvent( NULL,TRUE, FALSE, FALSE );

    // 
    // Create server listening and worker threads.
    // 
    m_pThreadPool = new CThreadPool( 13 );

    //
    // Initialize configuration watch thread.
    //
    m_pThreadPool->DoWork( ConfigWatchThread, (DWORD) this );

    // 
    // Initialize listening threads.
    // 
    m_pThreadPool->DoWork( IP_ListenerThread, (DWORD) this );
    m_pThreadPool->DoWork( SPX_ListenerThread, (DWORD) this );
    
    // 
    // Report that everything is OK.
    // 
    LogEvent( IDM_INFO_STARTING, EVENTLOG_INFORMATION_TYPE ); 

    return bRet;
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name: CServiceModule::ShutdownQserver
//
// Description: 
//
// Return type: BOOL 
//
//
///////////////////////////////////////////////////////////////////////////////
// 12/19/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CServiceModule::ShutdownQserver()
{
    // 
    // Tell threads that we are done.
    //     
    SetEvent( m_hStopping );

    delete m_pThreadPool;
    WSACleanup();
    CloseHandle( m_hStopping );
    CloseHandle( m_hConfigEvent );
    CloseHandle( m_hSocketConfig );

    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CServiceModule::CountQserverFileBytes
//
// Description  : This routine will compute the total number of bytes worth
//                of quarantine items we are currently holding.
//
// Return type  : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 2/4/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CServiceModule::CountQserverFileBytes()
{
    // 
    // Nuke contents of quarantine folder.
    // 
    CEnumerator* pEnum = NULL;
    TCHAR szQuarantineDir[ MAX_PATH + 1 ];
    TCHAR szFile[ MAX_PATH + 1];
    TCHAR szFullPath[ MAX_PATH + 1];
    _Module.GetQuarantineDir( szQuarantineDir );

    try
        {
        // 
        // Enumerate the quarantine folder.
        // 
        pEnum = new CEnumerator( szQuarantineDir );
        if( FALSE == pEnum->Enumerate() )
            throw(0);
        
        m_uQserverByteCount = 0;
        
        // 
        // Get initial count of items.
        // 
        m_ulItemCount = pEnum->GetCount();

        // 
        // Add up all file sizes.
        // 
        while( pEnum->GetNext( szFile ) )
            {
            // 
            // Build full path to file and open it.
            // 
            wsprintf( szFullPath, _T("%s\\%s"), szQuarantineDir, szFile );
            HANDLE hFile = CreateFile( szFullPath, 
                                        GENERIC_READ, 
                                        FILE_SHARE_READ, 
                                        NULL,
                                        OPEN_EXISTING,
                                        0,
                                        NULL );
            if( hFile != INVALID_HANDLE_VALUE )
                {
                DWORD dwSize = GetFileSize( hFile, NULL );
                if( dwSize != 0xFFFFFFFF )
                    m_uQserverByteCount += dwSize;
                CloseHandle( hFile );
                }
            }
        }
    catch(...)
        {
        }

    if( pEnum )
        delete pEnum;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CServiceModule::Handler
//
// Description  : 
//
// Return type  : inline void 
//
// Argument     : DWORD dwOpcode
//
///////////////////////////////////////////////////////////////////////////////
// 2/4/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
inline void CServiceModule::Handler(DWORD dwOpcode)
{
    switch (dwOpcode)
    {
    case SERVICE_CONTROL_STOP:
        m_status.dwWaitHint = 2000;
        SetServiceStatus(SERVICE_STOP_PENDING);
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
    }
}

void WINAPI CServiceModule::_ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
    _Module.ServiceMain(dwArgc, lpszArgv);
}
void WINAPI CServiceModule::_Handler(DWORD dwOpcode)
{
    _Module.Handler(dwOpcode); 
}

void CServiceModule::SetServiceStatus(DWORD dwState)
{
    m_status.dwCurrentState = dwState;
    ::SetServiceStatus(m_hServiceStatus, &m_status);
}

void CServiceModule::Run()
{
    _Module.dwThreadID = GetCurrentThreadId();

//    HRESULT hr = CoInitialize(NULL);
//  If you are running on NT 4.0 or higher you can use the following call
//  instead to make the EXE free threaded.
//  This means that calls come in on a random RPC thread
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);


    _ASSERTE(SUCCEEDED(hr));

    // This provides a NULL DACL which will allow access to everyone.
    CSecurityDescriptor sd;
    sd.InitializeFromThreadToken();
    hr = CoInitializeSecurity(sd, -1, NULL, NULL,
        RPC_C_AUTHN_LEVEL_CONNECT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
    //_ASSERTE(SUCCEEDED(hr));

    hr = _Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE);
    _ASSERTE(SUCCEEDED(hr));

    if (m_bService)
        SetServiceStatus(SERVICE_RUNNING);

    // 
    // Set up quarantine server.
    // 
    if( FALSE == InitializeQserver() )
        {
        _ASSERT( FALSE );
        LogEvent( IDM_ERR_INITIALIZING_QSERVER );    
        return;
        }

    MSG msg;
    while (GetMessage(&msg, 0, 0, 0))
        DispatchMessage(&msg);

    // 
    // Clean things up
    // 
    ShutdownQserver();

    _Module.RevokeClassObjects();

    CoUninitialize();
}

/////////////////////////////////////////////////////////////////////////////
//
extern "C" int WINAPI _tWinMain(HINSTANCE hInstance, 
    HINSTANCE /*hPrevInstance*/, LPTSTR lpCmdLine, int /*nShowCmd*/)
{
    lpCmdLine = GetCommandLine(); //this line necessary for _ATL_MIN_CRT
    _Module.Init(ObjectMap, hInstance, IDS_SERVICENAME, &LIBID_QSERVERLib);
    _Module.m_bService = TRUE;

    TCHAR szTokens[] = _T("-/");

    LPCTSTR lpszToken = FindOneOf(lpCmdLine, szTokens);
    while (lpszToken != NULL)
    {
        if (lstrcmpi(lpszToken, _T("UnregServer"))==0)
            return _Module.UnregisterServer();

        // Register as Local Server
        if (lstrcmpi(lpszToken, _T("RegServer"))==0)
            return _Module.RegisterServer(TRUE, TRUE); // FALSE);
        
        // Register as Service 
        if (lstrcmpi(lpszToken, _T("Service"))==0)
            return _Module.RegisterServer(TRUE, TRUE);
        
        lpszToken = FindOneOf(lpszToken, szTokens);
    }

    // Are we Service or Local Server
    CRegKey keyAppID;
    LONG lRes = keyAppID.Open(HKEY_CLASSES_ROOT, _T("AppID"), KEY_READ);
    if (lRes != ERROR_SUCCESS)
        return lRes;

    CRegKey key;
    lRes = key.Open(keyAppID, _T("{C60B0154-894B-11D2-8E5F-2A0712000000}"), KEY_READ);
    if (lRes != ERROR_SUCCESS)
        return lRes;

    TCHAR szValue[_MAX_PATH];
    DWORD dwLen = _MAX_PATH;
    lRes = key.QueryValue(szValue, _T("LocalService"), &dwLen);

    _Module.m_bService = FALSE;
    if (lRes == ERROR_SUCCESS)
        _Module.m_bService = TRUE;

    _Module.Start();

    // When we get here, the service has been stopped
    return _Module.m_status.dwWin32ExitCode;
}


void CServiceModule::GetQuarantineDir( LPTSTR pszDir )
{
    // 
    // Grab shared access to data.
    // 
    m_configER.Lock( ER_SHARED );

    // 
    // Copy Data
    // 
    lstrcpy( pszDir, m_szQuarantineFolder );

    // 
    // Release lock 
    // 
    m_configER.Unlock( ER_SHARED );        
}


void CServiceModule::SetQuarantineDir( LPCTSTR pszDir )
{
    // 
    // Grab exclusive access to data.
    // 
    m_configER.Lock( ER_EXCLUSIVE );

    // 
    // Copy Data
    // 
    lstrcpyn( m_szQuarantineFolder, pszDir, MAX_PATH );

    // 
    // Release lock 
    // 
    m_configER.Unlock( ER_EXCLUSIVE );        
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CServiceModule::AddOpenItem
//
// Description  : 
//
// Return type  : BOOL 
//
// Argument     :  DWORD dwItemID
// Argument     : LPUNKNOWN pUnkItem
//
///////////////////////////////////////////////////////////////////////////////
// 3/25/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CServiceModule::AddOpenItem( DWORD dwItemID, LPUNKNOWN pUnkItem )
{
    // 
    // Claim critical section for this operation.
    // 
    EnterCriticalSection( &m_csOpenMap );

    // 
    // Add this item to our list
    // 
    m_OpenItemMap.insert( VALUEPAIR( dwItemID, pUnkItem ) );

    // 
    // Release our lock.
    // 
    LeaveCriticalSection( &m_csOpenMap );

    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CServiceModule::RemoveOpenItem
//
// Description  : 
//
// Return type  : void 
//
// Argument     : DWORD dwItemID
// Argument     : LPUNKNOWN pUnkItem
//
///////////////////////////////////////////////////////////////////////////////
// 3/25/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CServiceModule::RemoveOpenItem( DWORD dwItemID, LPUNKNOWN pUnkItem )
{
    // 
    // Claim critical section for this operation.
    // 
    EnterCriticalSection( &m_csOpenMap );

    // 
    // Try to find this item in our map.
    // 
    int iCount = m_OpenItemMap.count( dwItemID );
    if( iCount != 0 )
        {
        // 
        // We now need to find correct open instance of the item.
        // 
        MAP::iterator MapIterator = m_OpenItemMap.lower_bound( dwItemID );
        for( int i = 0; i < iCount; i++, MapIterator++ )
            {
            if( (*MapIterator).second == pUnkItem )
                {
                // 
                // Found it so blow it away.
                // 
                m_OpenItemMap.erase( MapIterator );
                break;
                }
            }
        }

    // 
    // Release our lock.
    // 
    LeaveCriticalSection( &m_csOpenMap );

}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CServiceModule::IsItemOpen
//
// Description  : 
//
// Return type  : BOOL 
//
// Argument     : DWORD dwItemID
//
///////////////////////////////////////////////////////////////////////////////
// 3/25/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CServiceModule::IsItemOpen( DWORD dwItemID )
{
    BOOL bRet = FALSE;

    // 
    // Claim critical section for this operation.
    // 
    EnterCriticalSection( &m_csOpenMap );

    // 
    // Try to find this item in our map.
    // 
    int iCount = m_OpenItemMap.count( dwItemID );
    if( iCount != 0 )
        bRet = TRUE;

    // 
    // Release our lock.
    // 
    LeaveCriticalSection( &m_csOpenMap );

    return bRet;
}


