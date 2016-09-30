/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// qserver.cpp : Implementation of WinMain


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f qserverps.mk in the project directory.

#include "stdafx.h"
#include "safearray.h"
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
#include "QuarantineServerII.h"
#include "AvisConfig.h"
#include "QServerSavInfo.h"
#include "eventdata.h"
#include "util.h"
#include "qscommon.h"
#include "filenameparse.h"
#include "QSSesEvent.h"
#include <wininet.h>
#include "ModulVer.h"
int LoadEventStringTableAndConfigData();  // in GeneralEventDetection.cpp  

extern BOOL AddPlatinumCustomerType(void);

int QSSesInterface::initialized = 0;           //static variable needs to be initialized


CServiceModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_QuarantineServer, CQuarantineServer)
OBJECT_ENTRY(CLSID_QuarantineServerItem, CQuarantineServerItem)
OBJECT_ENTRY(CLSID_EnumQserverItemsVariant, CEnumQserverItemsVariant)
OBJECT_ENTRY(CLSID_QserverConfig, CQserverConfig)
OBJECT_ENTRY(CLSID_CopyItemData, CCopyItemData)
OBJECT_ENTRY(CLSID_QuarantineServerII, CQuarantineServerII)
OBJECT_ENTRY(CLSID_AvisConfig, CAvisConfig)
OBJECT_ENTRY(CLSID_QServerSavInfo, CQServerSavInfo)
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
        key.SetStringValue( _T("LocalService"),_T("qserver"));
        key.SetStringValue( _T("ServiceParameters"), _T("-Service"));
	
		// 7-5-00 tam added code to set the security level for dcom of qserver instead of relying on the 
		// defaults.  The following binary ACLs will set the security to qserver to "System", "Authenticated Users"
		// "Interactive Users"  The security level is set to level 2 which is "Connect"

		// *** DO NOT MODIFY THIS BINARY STREAM DIRECTLY ****
		// The binary stream was created by adding the access permisions via dcomcnfg utility then exporting the key
		// to a text file.  Then copying the binary section in the text file here to the code.
		BYTE bACLaccess[] = {0x01,0x00,0x04,0x80,0x64,00,00,00,0x80,00,00,00,00,00,00,00,0x14,00,
			  00,00,02,00,0x50,00,03,00,00,00,00,00,0x18,00,01,00,00,00,01,01,00,00,00,00,00,
			  05,04,00,00,00,00,00,00,00,00,00,0x18,00,01,00,00,00,01,01,00,00,00,00,00,05,
			  0x12,00,00,00,00,00,00,00,00,00,0x18,00,01,00,00,00,01,01,00,00,00,00,00,05,0x0b,
			  00,00,00,00,00,00,00,01,05,00,00,00,00,00,05,0x15,00,00,00,0x58,0x48,0xa5,0x72,0xdf,0x13,
			  0xa4,03,0x82,0x2d,0x1e,0x46,0xe8,03,00,00,01,05,00,00,00,00,00,05,0x15,00,00,00,0x58,0x48,0xa5,
			  0x72,0xdf,0x13,0xa4,03,0x82,0x2d,0x1e,0x46,0xe8,03,00,00};
		DWORD dwAuthLevel = 0x0002;
		RegSetValueEx(key.m_hKey,_T("AuthenticationLevel"),NULL,REG_DWORD,(LPBYTE)&dwAuthLevel,sizeof (dwAuthLevel));
		RegSetValueEx(key.m_hKey,_T("AccessPermission"),NULL,REG_BINARY,(LPBYTE)&bACLaccess,sizeof (bACLaccess));

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
    eventLogKey.SetStringValue( _T("EventMessageFile"),szBuffer );

    // 
    // Set up event types.
    // 
    dwData = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE;
    eventLogKey.SetDWORDValue(  _T("TypesSupported"),dwData );



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
inline BOOL CServiceModule::RegisterIcePack()
{
    BOOL bRc = FALSE;
	CFileNameParse fnp;
	USES_CONVERSION;
	ULONG ulVersion;
    CRegKey reg;
	LONG rc = FALSE;

	if( ERROR_SUCCESS != reg.Create( HKEY_LOCAL_MACHINE, REGKEY_QSERVER, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS) )
        return FALSE;


    rc = reg.QueryDWORDValue( REGVALUE_VERSION, (DWORD&)ulVersion);
    if( ERROR_SUCCESS != rc )
    {
        if( rc == 2 )   // DOESN'T EXIST
		{
			return FALSE;
		}
    }
	else if (ulVersion == 1)
		return TRUE;

	// Get the executable file path
    TCHAR szFilePath[_MAX_PATH], szPath[_MAX_PATH];
    ::GetModuleFileName(NULL, szFilePath, _MAX_PATH);
	
	fnp.GetFilePath(szFilePath, szPath, _MAX_PATH);
	HINSTANCE hQSInstallLib = LoadLibrary(_T("qsinst.dll"));
	typedef LONG (*tbInstallIcePackServices) (LPSTR pszTargetDir );
	tbInstallIcePackServices bInstallIcePackServices;

	if (hQSInstallLib == NULL)
		return bRc;

	bInstallIcePackServices = (tbInstallIcePackServices )GetProcAddress(hQSInstallLib,"InstallIcePackServices");
	if (bInstallIcePackServices != NULL)
	{
		bInstallIcePackServices(T2A(szPath));
		bRc = TRUE;
	}

	FreeLibrary (hQSInstallLib);

    return bRc;
}

inline void CServiceModule::Init(_ATL_OBJMAP_ENTRY* p, HINSTANCE h, UINT nServiceNameID, const GUID* plibid)
{
    CComModule::Init(p, h, plibid);

    m_bService = TRUE;

    LoadString(h, nServiceNameID, m_szServiceName, sizeof(m_szServiceName) / sizeof(TCHAR));
    LoadString( h, IDS_NEW_LOG_SERVICE_NAME, m_szEventLogServiceName, sizeof( m_szEventLogServiceName ) / sizeof( TCHAR ) );
    LoadString( h, IDS_ICEPACK_SERVICE_NAME, m_szIcePackServiceName, sizeof( m_szIcePackServiceName) / sizeof( TCHAR ) );

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
	m_hQFolderMoveEvent = NULL;

	m_tTimeOfAMSReg.GetCurrentTime();
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
BOOL CServiceModule::IsIcePackInstalled()
{
    BOOL bResult = FALSE;

    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (hSCM != NULL)
    {
        SC_HANDLE hService = ::OpenService(hSCM, m_szIcePackServiceName, SERVICE_QUERY_CONFIG);
        if (hService != NULL)
        {
            bResult = TRUE;
            ::CloseServiceHandle(hService);
        }
        ::CloseServiceHandle(hSCM);
    }
    return bResult;
}
BOOL CServiceModule::StartQServer()
{
    SC_HANDLE           hSCM;
    SC_HANDLE           hService;
    if (IsInstalled())
	{

        hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
        if (hSCM == NULL)
        {
            return (-1);
        }
        hService = OpenService(hSCM, m_szServiceName, SERVICE_START);
        if (hService == NULL)
        {
            CloseServiceHandle(hSCM);
            return (-1);
        }
        if (0 == StartService(hService, 0, NULL))
        CloseServiceHandle(hSCM);
	}
	else
		return 0;

    return TRUE;
}

BOOL CServiceModule::StartIcePack()
{
    SC_HANDLE           hSCM;
    SC_HANDLE           hService;
	ULONG ulVersion;
    CRegKey reg;
	LONG rc = FALSE;
	if( ERROR_SUCCESS != reg.Create( HKEY_LOCAL_MACHINE, REGKEY_QSERVER, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS) )
        return FALSE;


    rc = reg.QueryDWORDValue( REGVALUE_VERSION, (DWORD&)ulVersion);
    if( ERROR_SUCCESS != rc )
    {
        if( rc == 2 )   // DOESN'T EXIST
		{
			return FALSE;
		}
    }
	else if (ulVersion == 1)
		return TRUE;

	if (IsInstalled())
	{

        hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
        if (hSCM == NULL)
        {
            return (-1);
        }
        hService = OpenService(hSCM, m_szIcePackServiceName, SERVICE_START);
        if (hService == NULL)
        {
            CloseServiceHandle(hSCM);
            return (-1);
        }
        if (0 == StartService(hService, 0, NULL))
        CloseServiceHandle(hSCM);
	}
	else
		return 0;

    return TRUE;
}
BOOL CServiceModule::StopQServer()
{
    SC_HANDLE           hSCM;
    SC_HANDLE           hService;
    SERVICE_STATUS      nStatus;
    if (IsInstalled())
	{

        hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
        if (hSCM == NULL)
        {
            return (-1);
        }
        hService = OpenService(hSCM, m_szServiceName, SERVICE_STOP);
        if (hService == NULL)
        {
            CloseServiceHandle(hSCM);
            return (-1);
        }
        if (ControlService(hService, SERVICE_CONTROL_STOP, &nStatus) == 0)
        CloseServiceHandle(hSCM);
	}
	else
		return 0;

    return TRUE;
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
    bStr.LoadString( IDS_NEW_SERVICE_DISPLAY_NAME );

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
					   LPTSTR lpszParam1 /* = NULL */,
					   LPTSTR lpszParam2 /* = NULL */,
					   LPTSTR lpszParam3 /* = NULL */,
					   LPTSTR lpszParam4 /* = NULL */ )
{
	LPTSTR	aParams[ 4 ];
	auto	WORD	wParamCount = 0;
	auto	HANDLE	hEventSource;
	CString sMsg;
	
    

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
	{
		if (aParams[i] != NULL)
		{
			wParamCount++;
			sMsg+=aParams[i];
		}
		
	}
	
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
// Function name: CServiceModule::IsSESAAgentPresent
//
// Description: 
//
// Return type: BOOL 
//
//
///////////////////////////////////////////////////////////////////////////////
// 10/02/01 - tmarles: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CServiceModule::IsSESAAgentPresent()
{
	BOOL bRc = FALSE;

	if (m_dwfGenSESAEvents == TRUE)
	{
		// 
		// Claim critical section for this operation.
		// 
		EnterCriticalSection( &m_csInitSESAAgent );


		// Check the interface to SESA to see if it exists	
		if (m_pQSSesInterface != NULL)
		{
			// are we initialized if not delete the interface
			if (!m_pQSSesInterface->isInitialized())
			{
				delete m_pQSSesInterface;
				m_pQSSesInterface = NULL;
			}
			else // good we are initialized
				bRc = TRUE;
		}

		// check to see if there is no interface at this point.
		if (m_pQSSesInterface == NULL)
		{
			//create the interface to sesa
			m_pQSSesInterface = new QSSesInterface (DEFAULT_IP, m_ulSESAAgentPort);
			// check to see if we were able to establish connection to agent.
			if (m_pQSSesInterface->isInitialized())
			{
				bRc = TRUE;
			}
			else
			{
				delete m_pQSSesInterface;
				m_pQSSesInterface = NULL;
				// Stop sending SESA events since the agent is not present.
				m_dwfGenSESAEvents = FALSE;
			}
		}
		// 
		// Release our lock.
		// 
		LeaveCriticalSection( &m_csInitSESAAgent );
	}

	return bRc;
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
	ULONG ulVersion;



    // 
    // Get location of quarantine folder from registry.
    // 
    CRegKey reg;
    if( ERROR_SUCCESS != reg.Create( HKEY_LOCAL_MACHINE, REGKEY_QSERVER, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS) )
        return FALSE;

    DWORD dwSize = MAX_PATH;
    if( ERROR_SUCCESS != reg.QueryStringValue( REGVALUE_QUARANTINE_FOLDER, m_szQuarantineFolder, &dwSize ) ||
        ERROR_SUCCESS != reg.QueryDWORDValue( REGVALUE_PORT, (DWORD&)m_iWinsockPort ) ||
        ERROR_SUCCESS != reg.QueryDWORDValue( REGVALUE_SPXSOCKET, (DWORD&)m_iSPXSocket) ||
        ERROR_SUCCESS != reg.QueryDWORDValue( REGVALUE_LISTEN_IP, (DWORD&)m_bListenIP ) ||
        ERROR_SUCCESS != reg.QueryDWORDValue( REGVALUE_LISTEN_SPX, (DWORD&)m_bListenSPX ) ||
        ERROR_SUCCESS != reg.QueryDWORDValue( REGVALUE_ALERTINTERVAL, (DWORD&)m_ulScanInterval))
        {
        LogEvent( IDM_INFO_NO_CONFIG, EVENTLOG_INFORMATION_TYPE );
        }

    // CREATE DEFAULTS FOR MAX FILES  jhill 2/12/00 
    LONG rc = reg.QueryDWORDValue( REGVALUE_MAX_FILES, (DWORD&)m_uFileQuota); 
    if( ERROR_SUCCESS != rc )
    {
        m_uFileQuota = 750;
        if( rc == 2 )   // DOESN'T EXIST
            reg.SetDWORDValue( REGVALUE_MAX_FILES, m_uFileQuota);

    }
    // CREATE DEFAULTS FOR MAX SIZE  jhill 2/12/00
    rc = reg.QueryDWORDValue( REGVALUE_MAX_SIZE, (DWORD&)m_uDiskQuota );
    if( ERROR_SUCCESS != rc )
    {
        m_uDiskQuota = 500;
        if( rc == 2 )   // DOESN'T EXIST
            reg.SetDWORDValue( REGVALUE_MAX_SIZE, m_uDiskQuota );
    }

    // CREATE DEFAULTS FOR PURGE  TM 05/08/00
    rc = reg.QueryDWORDValue( REGVALUE_PURGE, m_dwPurge ); 
    if( ERROR_SUCCESS != rc )
    {
        m_dwPurge = FALSE;
        if( rc == 2 )   // DOESN'T EXIST
            reg.SetDWORDValue( REGVALUE_PURGE, m_dwPurge );

    }
    // CREATE DEFAULTS config interface timeout value  tmarles 3/14/00 
    rc = reg.QueryDWORDValue( CONFIG_I_TIMEOUT, (DWORD&)m_ulConfigITimeout);
    if( ERROR_SUCCESS != rc )
    {
        if( rc == 2 )   // DOESN'T EXIST
            reg.SetDWORDValue( CONFIG_I_TIMEOUT,m_ulConfigITimeout );
    }
    // write in version number tmarles 9-25-01 
    rc = reg.QueryDWORDValue( REGVALUE_VERSION, (DWORD&)ulVersion);
    if( ERROR_SUCCESS != rc )
    {
        if( rc == 2 )   // DOESN'T EXIST
		{
			ulVersion = QSVERISON;
			rc = reg.SetDWORDValue(REGVALUE_VERSION, ulVersion);
		}
    }
	else if (ulVersion == QSVERSION2X)
	{
		ulVersion = QSVERISON;
		rc = reg.SetDWORDValue(REGVALUE_VERSION, ulVersion);
	}

    // get the SESA port number.
    rc = reg.QueryDWORDValue( REGVALUE_SESA_AGENT_PORT, (DWORD&)m_ulSESAAgentPort);
    if( ERROR_SUCCESS != rc )
    {
        if( rc == 2 )   // DOESN'T EXIST
		{
			m_ulSESAAgentPort = DEFAULT_PORT;
		}
    }

	// tm 11-11-03 
	// get flag to generate SESA events
    rc = reg.QueryDWORDValue( REGVALUE_GENERATE_SESA_EVENTS, (DWORD&)m_ulSESAAgentPort);
    if( ERROR_SUCCESS != rc )
    {
        if( rc == 2 )   // DOESN'T EXIST
		{
			m_dwfGenSESAEvents = QS_GENERATE_SESA_EVENTS_DEFAULT;
		}
    }

    // winsock trace value tmarles 4-17-00 
    rc = reg.QueryDWORDValue( WINSOCK_TRACE, (DWORD&)m_ulWinSockTrace);

    // get item trace tmarles 5-19-00 
    rc = reg.QueryDWORDValue( QSERVERITEM_TRACE, (DWORD&)m_ulItemInterfaceTrace);

    dwSize = MAX_PATH;
    LoadString( GetResourceInstance(), IDS_QS_EVENT_APP_NAME, m_szQsEventAppName,sizeof(m_szQsEventAppName));

	if (ERROR_SUCCESS != reg.QueryStringValue( REGVALUE_AMS_SERVER_ADDRESS, m_szAmsServerName, &dwSize))
        {
        LogEvent( IDM_INFO_NO_CONFIG, EVENTLOG_INFORMATION_TYPE );
        }
	else
	{
        // LOAD EVENT CONFIG. INCLUDES LOADING STRINGS IN GLOBALS 
        // FROM STRING TABLE IN ICEPACKTOKENS.DLL      jhill 2/11/00
        LoadEventStringTableAndConfigData();

		// register ams events.
		if (!StartAMSEx( ))
		{
			LogEvent (IDM_ERR_INITIALIZING_AMS_EVENTS);
		}
	}

	AddPlatinumCustomerType();

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

	// we are going to get the wininet version
	{
		DWORD dwSize =0;
		INTERNET_VERSION_INFO ivi = {0};
		unsigned long ulEncryption = 0;
		CString sTemp;
		TCHAR szFormat[MAX_PATH];



		dwSize = sizeof (ivi);
		
		LoadString( GetResourceInstance(), IDS_WININETVERSION_FORMAT, szFormat, sizeof(szFormat));
		// get the version info
		BOOL bRc = InternetQueryOption(NULL,INTERNET_OPTION_VERSION ,&ivi,&dwSize);
		sTemp.Format(szFormat, ivi.dwMajorVersion, ivi.dwMinorVersion);

		// save it off now
		m_sWinINetVersion = sTemp;
		

		dwSize = sizeof (ulEncryption);
		LoadString( GetResourceInstance(), IDS_WININETENCRYPTION_BIT_FORMAT, szFormat, sizeof(szFormat));



	}
	{
		TCHAR szCountryCode [MAX_PATH] = {0};

		// okay lets get the country Code
		GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_ICOUNTRY, szCountryCode, sizeof(szCountryCode));

		m_sCountryCode = szCountryCode;
	}
	{
		// get the version of the security dll so we can see if we are 40 or 128 bit encryption.
		TCHAR szTemp[MAX_PATH];
		CModuleVersion ver;
		ver.dwFileVersionMS = 0;
		ver.dwFileVersionLS = 0; 

		ver.GetFileVersionInfo(_T("Schannel.dll"));
		CString sd = ver.GetValue(_T("FileDescription"));
		if (sd.Find(_T("Export"))!= -1)
			LoadString(GetResourceInstance(), IDS_WININET_ENCRYPTION_40_BIT, szTemp, sizeof(szTemp));
		else
			LoadString(GetResourceInstance(), IDS_WININET_ENCRYPTION_128_BIT, szTemp, sizeof(szTemp));
		m_sWinINetEncryptionBit = szTemp;

		CString sv = ver.GetValue(_T("FileVersion"));
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
    // Create qfolder move event.
    // 
    m_hQFolderMoveEvent = CreateEvent( NULL,TRUE, FALSE, FALSE );
    SetEvent(m_hQFolderMoveEvent);
	// init SESA before starting worker threads.
	if (_Module.IsSESAAgentPresent())
	{
	    SESEvent QSEvent;
		//create a log item
   		QSEvent = m_pQSSesInterface->createEvent(SESSWFID_SYMANTEC_CENTRAL_QUARANTINE___QUARANTINE_SERVER,
									SES_EVENT_APPLICATION_START,				// event id
									SES_CAT_APPLICATION,
									SES_SEV_INFORMATIONAL,				// severity
									SES_CLASS_BASE);					// event class

        QSEvent->send();												// send the event
    }
	// 
    // Create server listening and worker threads.
    // 
    m_pThreadPool = new CThreadPool( 20);
    //
    // Initialize configuration watch thread.
    //
	Sleep(100);
    m_pThreadPool->DoWork( ConfigWatchThread, (DWORD) this );

    // 
    // Initialize listening threads.
    // 
	Sleep(100);
    m_pThreadPool->DoWork( IP_ListenerThread, (DWORD) this,THREAD_PRIORITY_ABOVE_NORMAL);
	Sleep(100);
    m_pThreadPool->DoWork( SPX_ListenerThread, (DWORD) this,THREAD_PRIORITY_ABOVE_NORMAL );
	Sleep(100);

    //
	// Initialize sample scanning thread. (1-18-00 tmarles)
	//
    m_pThreadPool->DoWork( ScanSamplesForAlerts, (DWORD) this, THREAD_PRIORITY_BELOW_NORMAL);

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

	if (_Module.IsSESAAgentPresent())
	{
	    SESEvent QSEvent;
		//create a log item
		QSEvent = m_pQSSesInterface->createEvent(SESSWFID_SYMANTEC_CENTRAL_QUARANTINE___QUARANTINE_SERVER,
									SES_EVENT_APPLICATION_STOP,					// event id
									SES_CAT_APPLICATION,
									SES_SEV_INFORMATIONAL,				// severity
									SES_CLASS_BASE);					// event class

        QSEvent->send();												// send the event
	}

    // 
    // Tell threads that we are done.
    //     
    SetEvent( m_hStopping );
#ifdef  MY_SYSTEM
        //void fWidePrintString(LPCSTR lpszFmt, ...);
        fWidePrintString("Stoping QService");
#endif

    delete m_pThreadPool;
    WSACleanup();
    CloseHandle( m_hStopping );
    CloseHandle( m_hConfigEvent );
    CloseHandle( m_hSocketConfig );
    CloseHandle( m_hQFolderMoveEvent );
	if (m_pQSSesInterface)
		delete m_pQSSesInterface;

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
	UninstallAMS();
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
    while (lpszToken != NULL )
    {
        if (lstrcmpi(lpszToken, _T("UnregServer"))==0)
            return _Module.UnregisterServer();

        // Register as Local Server
        if (lstrcmpi(lpszToken, _T("RegServer"))==0)
            // InstallShield requires the second param to be TRUE -- TN 3-2-2000
            return _Module.RegisterServer(TRUE, TRUE); // FALSE is as exe, TRUE forces register as service
        
        // Register as Local Server
        if (lstrcmpi(lpszToken, _T("RunOnce"))==0)
        {
            // InstallShield requires the second param to be TRUE -- TN 3-2-2000
            if (!_Module.RegisterServer(TRUE, TRUE)) // FALSE is as exe, TRUE forces register as service
            {
                if(!_Module.IsIcePackInstalled())
                {
                    if (_Module.RegisterIcePack())
                    {
                        _Module.StartQServer();
                        int iResult = _Module.StartIcePack();

                        /* JLENZ - Removed by request.
                        CComBSTR bstrCaption;
                        CComBSTR bstrMessage;

                        bstrCaption.LoadString(IDS_RUNONCE_COMPLETE_CAPTION);
                        bstrMessage.LoadString(IDS_RUNONCE_COMPLETE_MESSAGE);

                        MessageBox(NULL,bstrMessage,bstrCaption, MB_OK|MB_ICONINFORMATION );
                        */

                        return iResult;
                    }
                }
            }
            return -1;
        }

		// Register as Service 
        if (lstrcmpi(lpszToken, _T("Service"))==0)
            return _Module.RegisterServer(TRUE, TRUE); // FALSE is as exe, TRUE forces register as service
 
		// Start Service 
        if (lstrcmpi(lpszToken, _T("Start"))==0)
            return _Module.StartQServer();

		// Stop Service 
        if (lstrcmpi(lpszToken, _T("Stop"))==0)
            return _Module.StopQServer();
        
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
    lRes = key.QueryStringValue(_T("LocalService"), szValue, &dwLen);

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
    int iCount = m_OpenItemMap.count( dwItemID );
	if ((m_ulItemInterfaceTrace & DEBUGLEVEL_DETIAL) == DEBUGLEVEL_DETIAL)
		fWidePrintString("Adding Item to multimap.  ItemID:  0x%x, count(before operation):  %d, Interface: 0x%lx",dwItemID, iCount, pUnkItem );
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
	if ((m_ulItemInterfaceTrace & DEBUGLEVEL_DETIAL) == DEBUGLEVEL_DETIAL)
		fWidePrintString("Removing Item from multimap.  ItemID:  0x%x, Current count:  %d, Interface: 0x%lx",dwItemID, iCount, pUnkItem );
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



//***********************************************************
// Helper comparison function
bool IsSafeArrayEqual(SAFEARRAY* psa1, SAFEARRAY* psa2) throw(_com_error) {
	bool bEqual = false;

	// Check dimension of each
	if(psa1->cDims != psa2->cDims) {
		return false;
	}

	// Count the total elements in the array while checking bounds
	unsigned long arraysize = 1;
	for(unsigned short ctr = 0; ctr < psa1->cDims; ctr++) {
		if( (psa1->rgsabound[ctr].cElements == psa2->rgsabound[ctr].cElements) &&
			(psa1->rgsabound[ctr].lLbound == psa2->rgsabound[ctr].lLbound) ) {
			arraysize *= psa1->rgsabound[ctr].cElements;
		} else {
			return false;
		}
	}

	// Count the total bytes
	arraysize *= SafeArrayGetElemsize(psa1);

	// Access the data
	void* pData1;
	void* pData2;
	_com_util::CheckError(::SafeArrayAccessData(psa1, &pData1));
	_com_util::CheckError(::SafeArrayAccessData(psa2, &pData2));

	if(!memcmp(pData1, pData2, arraysize)) {
		bEqual = true;
	}

	// Release the array locks
	_com_util::CheckError(::SafeArrayUnaccessData(psa2));
	_com_util::CheckError(::SafeArrayUnaccessData(psa1));
	return bEqual;
}

