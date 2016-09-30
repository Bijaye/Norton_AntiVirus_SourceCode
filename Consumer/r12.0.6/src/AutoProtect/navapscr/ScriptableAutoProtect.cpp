// ScriptableAutoProtect.cpp : Implementation of CScriptableAutoProtect
#include "stdafx.h"
#include "NAVAPSCR.h"
#include "ScriptableAutoProtect.h"
#include "GlobalEvents.h"
#include "GlobalEvent.h"
#include "NAVInfo.h"
#include "apoptnames.h"     // Names of options
#include "optnames.h"
#include "osinfo.h"
#include "Service.h"
#include "Savrt32.h"
#include "AVccModuleID.h"
#define _INIT_COSVERSIONINFO
#define SIMON_INITGUID
#include "NAVSettingsHelperEx.h"
#include "NAVErrorResource.h"

const TCHAR g_cszCCApp[]        = _T("\\ccApp.exe");      // AP agent exe
const TCHAR szNAVAPWindowClass[]  = _T("NAVAP Wnd Class");
const TCHAR NAVAP_SERVICE_NAME[]  = _T("NAVAPSVC");
const TCHAR NAVAP_STATICVXD_KEY[] = _T("System\\CurrentControlSet\\Services\\VxD\\SAVRT");
const TCHAR NAVAP_AGENT_KEY[]     = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
const TCHAR szAPService[]         = _T("navapsvc.exe");
const TCHAR szSingleInstMutexNT[]   = _T("Norton AntiVirus Agent Is Running");

/////////////////////////////////////////////////////////////////////////////
// CScriptableAutoProtect

CScriptableAutoProtect::CScriptableAutoProtect()
{
	CComBSTR bstrErrorClass ("NAVError.NAVCOMError");

	// Check NAVError module for Symantec Signature...
	//if( NAVTRUST_OK == IsSymantecComServer(_T("NAVError.NAVCOMError")) )
	{
		if ( FAILED ( m_spError.CoCreateInstance( bstrErrorClass,
			NULL,
			CLSCTX_INPROC_SERVER)))
		{
		}
	}
}

CScriptableAutoProtect::~CScriptableAutoProtect()
{
}

// If this method is successful the output service pointer will not be NULL
void CScriptableAutoProtect::GetAPServiceObject(NAVAPSVCLib::INAVAPServicePtr& spAPService)
{
    HRESULT hr = S_OK;

    // Establish communication with the AP service.
    //
    try
    {
        if( FAILED(hr = spAPService.CreateInstance(__uuidof( NAVAPSVCLib::NAVAPService))) )
        {
            // If the error is that the service is disabled let's change the configuration
            // to fix it (NT Only)
            if( hr == HRESULT_FROM_WIN32(ERROR_SERVICE_DISABLED) && m_OSInfo.IsWinNT() )
            {
                // Get the current AP start mode settings and set the AP service accordingly
                DWORD dwStart = SERVICE_AUTO_START;
                if( SAVRT_OPTS_OK == m_SAVRTOptions.Load() )
                {
                    m_SAVRTOptions.GetDwordValue(STARTUP_LoadVxD, &dwStart, 1);

                    if( dwStart == 0 )
                        dwStart = SERVICE_DEMAND_START;
                    else
                        dwStart = SERVICE_AUTO_START;
                }

                CCTRACEW("CScriptableAutoProtect::GetAPServiceObject() - The service is disabled, attempting to change it to start mode %d", dwStart);
                if( SetServiceStartupMode(NAVAP_SERVICE_NAME, dwStart) )
                {
                    CCTRACEI("CScriptableAutoProtect::GetAPServiceObject() - Successfully changed service start mode. Now creating object.");

                    if( FAILED(hr = spAPService.CreateInstance(__uuidof( NAVAPSVCLib::NAVAPService))) )
                    {
                        CCTRACEE("CScriptableAutoProtect::GetAPServiceObject() - CreateInstance() failed after setting the service start mode. hr = 0x%X", hr);
                    }
                }
            }
        }
    }
    catch (...)
    {
        CCTRACEE("CScriptableAutoProtect::GetAPServiceObject() - Caught exception attempting to connect to the AP service.");
    }

    // If we still have not connected to the AP service last do a last ditch effor
    // at reregistering it
    if( spAPService == NULL )
    {
        try
        {
            if (RegisterAPService ())
            {
                if( FAILED(hr = spAPService.CreateInstance(__uuidof( NAVAPSVCLib::NAVAPService))) )
                {
                    CCTRACEE("CScriptableAutoProtect::GetAPServiceObject() - CreateInstance() failed after re-registering the service. hr = 0x%X", hr);
                }
            }
        }
        catch(...)
        {
            CCTRACEE("CScriptableAutoProtect::GetAPServiceObject() - Caught exception attempting to re-register and connect to the AP service.");
        }
    }
}

// Gets the enabled/disabled state of AP
//
// Returns : E_FAIL - Error getting value
//           S_FALSE - AP is not loaded, will always return bEnabled = FALSE
//           S_OK - AP is loaded, bEnabled reflects AP's enabled/disabled state
//
STDMETHODIMP CScriptableAutoProtect::get_Enabled(BOOL *pbEnabled)
{
	if ( forceError (ERR_SECURITY_FAILED) || !IsItSafe())
	{
		MakeError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
		return E_ACCESSDENIED;
	}

	HRESULT hr = E_FAIL;

	try
	{
		if ( forceError (ERR_UNKNOWN_GET_ENABLED) )
			throw FALSE;

		*pbEnabled = FALSE;

		// Make sure AP is loaded, since if it isn't load, it obviously can't be enabled  
		if ( forceError (ERR_CANT_LOAD) || FAILED( get_Loaded ( pbEnabled )) )
		{
			// Loading error.
			//
			CCTRACEE ( "CScriptableAutoProtect::get_Enabled() - get_Loaded FAILED" );
			MakeError ( ERR_CANT_LOAD, E_FAIL, IDS_NAVERROR_INTERNAL );
			hr = E_FAIL;
		}
		else if ( *pbEnabled )
		{
			// If the Agent isn't running on 9x then AP is obviously not enabled and we
			// don't want to make calls to a COM server that's not running, so
			// just return E_FAIL to indicate AP is not loaded
			if( forceError (ERR_CANT_LOAD) || !m_OSInfo.IsWinNT() && (NULL == FindWindow( szNAVAPWindowClass, NULL )) )
			{
				CCTRACEE ( "CScriptableAutoProtect::get_Enabled() - Agent not running - 9x" );
				MakeError ( ERR_CANT_LOAD, E_FAIL, IDS_NAVERROR_INTERNAL );
				return E_FAIL;
			}

			NAVAPSVCLib::INAVAPServicePtr spAPService = NULL;

			// Establish communication with the AP service.
			//
            GetAPServiceObject(spAPService);

			if ( forceError (ERR_FAILED_TO_CONNECT) || spAPService == NULL )
			{
				CCTRACEE ( "CScriptableAutoProtect::get_Enabled() - Failed connecting to AP service.");
				MakeError ( ERR_FAILED_TO_CONNECT, E_FAIL, IDS_NAVERROR_INTERNAL );
				return E_FAIL;
			}

			// Get the state of AP from the service.
			//
			long lAPEnabled = 0;
			spAPService -> GetAutoProtectEnabled (&lAPEnabled);

			if ( lAPEnabled == 1 ) 		// 1 = enabled, 0 = disabled
				*pbEnabled = TRUE;
			else
				*pbEnabled = FALSE;

			hr = S_OK;
		}
		else
		{
			// Nope, AP ain't running at this time.
			//
			hr = S_FALSE;
		}
	}
	catch(...)
	{
		MakeError ( ERR_UNKNOWN_GET_ENABLED, E_FAIL, IDS_NAVERROR_INTERNAL );
		return E_FAIL;
	}

	return hr;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:    Enable()
//
// Description:
//  Enables/Disables AutoProtect.
//
// Input:
//  BOOLEAN  - TRUE to Enable, FALSE to disable.
//
// Returns:
//  S_OK if all AP components were loaded OK, E_FAIL if not.
//
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CScriptableAutoProtect::put_Enabled(BOOL bEnabled)
{
	try
	{
		CCTRACEI("put_Enabled(%d)", bEnabled);

		if (forceError (ERR_UNKNOWN_PUT_ENABLED))
			throw FALSE;

		// Should we even be here?
		if (forceError (ERR_SECURITY_FAILED) || !IsItSafe())
		{
			MakeError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
			return E_ACCESSDENIED;
		}		

		// We can't do anything in Safe Boot mode since AP can't load without drivers.
		if (forceError (ERR_SAFE_MODE) || ::GetSystemMetrics(SM_CLEANBOOT))
		{			
			MakeError (ERR_SAFE_MODE, E_FAIL, IDS_NAVERROR_INTERNAL);
			return E_FAIL;
		}

		HRESULT hr = S_OK;

		if( bEnabled )
		{
			// Enable the driver.  On WinNT based systems, this will start the service if it's
			// not already running.  On Win9x, this stuff talks to the agent.

			// If we're on 9x we should make sure that the Agent is up and running before
			// sending any commands 
			if( !m_OSInfo.IsWinNT() )
			{
				if ( NULL == FindWindow( szNAVAPWindowClass, NULL ) )
				{
					CCTRACEE("CScriptableAutoProtect::put_Enabled() - Could not find the AP Agent window class, will attempt to load it manually");

					// There's a slight chance that the Agent isn't running and we
					// can start it, so we'll give it a try before giving up and returning E_FAIL
					TCHAR szCCAppPath [MAX_PATH] = {0};
					TCHAR szParams [MAX_PATH] = {0};

					// Get the path to ccApp.exe
					::_tcscpy( szCCAppPath, m_NAVInfo.GetSymantecCommonDir () );
					::_tcscat( szCCAppPath, g_cszCCApp );

					// Now load up the agent
					wsprintf(szParams, "/loadproduct:\"%s\\navprod.dll\"", m_NAVInfo.GetNAVDir());
					CCTRACEI("CScriptableAutoProtect::put_Enabled() - Shell Executing: %s %s", szCCAppPath, szParams);
					HINSTANCE hInst = ShellExecute(NULL, NULL, szCCAppPath, szParams, NULL, SW_SHOWNORMAL);             

					// Now check for the window again and if it's not there the Agent suffered a crash
					// so we have to return E_FAIL
					for ( int i = 0; i<12 && NULL == FindWindow( szNAVAPWindowClass, NULL ); i++ )
					{
						// Try waiting up to 3 seconds...
						Sleep(1000);
					}

					if (forceError (ERR_CANT_START) || NULL == FindWindow( szNAVAPWindowClass, NULL))
					{
						CCTRACEE ( "Failed put_Enabled");
						MakeError ( ERR_CANT_START, E_FAIL, IDS_NAVERROR_INTERNAL );
						return E_FAIL;
					}
					else
					{
						CCTRACEI("CScriptableAutoProtect::put_Enabled() - Successfully loaded the agent. Waiting 2 seconds.");

						// Needs some time to load up the COM server that scriptable will be communicating
						// with
						Sleep(2000);
					}
				}
			}
			NAVAPSVCLib::INAVAPServicePtr spAPService = NULL;

			// Establish communication with the AP service.
			//
            GetAPServiceObject(spAPService);

			if ( forceError (ERR_FAILED_TO_CONNECT) || spAPService == NULL )
			{
				CCTRACEE ( "put_Enabled:Failed connecting to AP service");
				MakeError ( ERR_FAILED_TO_CONNECT, E_FAIL, IDS_NAVERROR_INTERNAL );
				return E_FAIL;
			}

			_bstr_t sCommand;
			sCommand = NAVAPCMD_ENABLE;
			spAPService -> SendCommand( sCommand, NULL );
		}
		else
		{
			// First find out if AP is loaded
			//
			BOOL bLoaded = FALSE;

			// If AP is not loaded, there's no reason to disable it
			//
			if ( forceError (ERR_CANT_LOAD) || FAILED ( get_Loaded ( &bLoaded )) )
			{
				CCTRACEE ( "Failure put_Enabled (false) and AP not loaded" );
				MakeError ( ERR_CANT_LOAD, E_FAIL, IDS_NAVERROR_INTERNAL );
				return E_FAIL;
			}

			if ( !bLoaded )  // No point in continuing...
			{  
				return S_OK;
			}

			// Create object and call it.
			NAVAPSVCLib::INAVAPServicePtr spAPService = NULL;

			// Establish communication with the AP service.
			//
            GetAPServiceObject(spAPService);

			if ( forceError (ERR_FAILED_TO_CONNECT) || spAPService == NULL )
			{
				CCTRACEE ( "put_Enabled:Failed connecting to AP service");
				MakeError ( ERR_FAILED_TO_CONNECT, E_FAIL, IDS_NAVERROR_INTERNAL );
				return E_FAIL;
			}

			_bstr_t sCommand;
			sCommand = NAVAPCMD_DISABLE;
			spAPService -> SendCommand( sCommand, NULL );
		}

		// Tell the world that the state of AP has changed.
		//
		CGlobalEvent event;
		if( event.Create( SYM_REFRESH_AP_STATUS_EVENT ) )
		{
			::PulseEvent ( event );
		}
	}
	catch (...)
	{
		CCTRACEE ( "Failure in put_Enabled");  
		MakeError ( ERR_UNKNOWN_PUT_ENABLED, E_FAIL, IDS_NAVERROR_INTERNAL );
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CScriptableAutoProtect::get_Loaded(BOOL *pAPLoaded)
{
	// Not running
	//
	*pAPLoaded = FALSE;

	if ( forceError (ERR_SECURITY_FAILED) || !IsItSafe())
	{
		MakeError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
		return E_ACCESSDENIED;
	}

	// Make sure AP is running.
	//
	ccLib::CMutex mutex;
	if (!mutex.Open( SYNCHRONIZE, FALSE, NAVAPSVC_RUNNING_MUTEX, TRUE))
	{
		CCTRACEI("Failed to open (Global) NAVAPSVC_RUNNING_MUTEX: %d", GetLastError());

		if (!mutex.Open( SYNCHRONIZE, FALSE, NAVAPSVC_RUNNING_MUTEX, FALSE))
		{
			CCTRACEI("Failed to open (Local) NAVAPSVC_RUNNING_MUTEX: %d", GetLastError());

			CCTRACEI("AP is not loaded");
			return S_OK;
		}
	}

	// Yes;  Do our thing.
	mutex.Destroy();

	// Extra check for NT agent
	//
	COSInfo OSInfo;

	if ( OSInfo.IsWinNT () )
	{
		if( mutex.Open( SYNCHRONIZE, FALSE, szSingleInstMutexNT, FALSE) )
		{
			// Yes;  Do our thing.
			*pAPLoaded = TRUE;
			CCTRACEI("AP is loaded");
		}
		else
		{
			CCTRACEI("Failed to open (Local) %s MUTEX: %d", szSingleInstMutexNT, GetLastError());
		}
	}
	else
	{
		*pAPLoaded = TRUE;
		CCTRACEI("AP is loaded");
	}

	return S_OK;
}

// The put_Enabled (1) loads AP for us, and there's not much
// point in Loading AP in a disabled state.
//
STDMETHODIMP CScriptableAutoProtect::Load()
{
	if (forceError (ERR_SECURITY_FAILED) || !IsItSafe())
	{
		MakeError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
		return E_ACCESSDENIED;
	}

	return put_Enabled ( 1 );
}

//
// This function sets AP to load or not load at startup, it does not change
// the current state of AP, that will be up to the script after querying the
// user. The configuration will take effect after next re-boot if no additional
// action is taken.
//
STDMETHODIMP CScriptableAutoProtect::Configure(/*[in]*/ BOOL bStartup)
{
	//This function should NOT attempt to start the agent.  The Configuration will take effect
	//when the machine is rebooted.  This function only talks to ths SVC on NT and relies on
	//SAVRT32.dll to actually change the startup mode for the drivers.

	CCTRACEI("Configure(%d)", bStartup);

	// Should we even be here?
	if (forceError (ERR_SECURITY_FAILED) || !IsItSafe())
	{
		MakeError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
		return E_ACCESSDENIED;
	}

	static enum { reg_Uninitialized = -1  // Do we have premission to mess with the registry?.
		, reg_NoAccess
		, reg_CanModify } s_eCanAccessRegKey(reg_Uninitialized);


	if (reg_Uninitialized == s_eCanAccessRegKey)
	{
		CRegKey rkeyNAV;
		DWORD dwBogusValue = 0UL;

		// We also have to check for write access to a reg key because
		// we set the registery trying to configure AP.
		// Note : Under Win 2000 we found that you can request write access
		//        on a reg key hive but still not be able to change values of
		//        keys located inside it. Hence, the double check below.
		s_eCanAccessRegKey = ERROR_SUCCESS == rkeyNAV.Create(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Norton AntiVirus"),  _T("KEY_WRITE"))
			&& ERROR_SUCCESS == rkeyNAV.SetDWORDValue( _T("OptionsTest"),dwBogusValue) ? reg_CanModify : reg_NoAccess;

		if (s_eCanAccessRegKey)
			rkeyNAV.DeleteValue(_T("OptionsTest"));
	}

	if (forceError (IDS_Err_RegistryPermissions) || reg_NoAccess == s_eCanAccessRegKey)
	{
		CCTRACEE( "Failed getting write access to registry");		
		MakeError ( IDS_Err_RegistryPermissions, E_FAIL, IDS_NAVERROR_NAVAPSCR_REGISTRYPERMISSIONS );
		return E_FAIL;
	}

	// Set the driver startup type
	if( bStartup )
	{
		// On NT we need to set the drivers to demand start if the options specify to
		// Delay load Auto-Protect
		DWORD dwDelayLoad = 0;
		if( m_OSInfo.IsWinNT() )
		{
			CNAVOptSettingsEx NavOpts(m_ccSettings);
			// Don't use business rules in case we do a save, there's no need
			// to incur on option refresh for the delay load setting
			//
			// m_NAVInfo.GetNAVOptPath() -- always returns NAVOPTS.DAT
			// NavOpts.Init(m_NAVInfo.GetNAVOptPath(), FALSE);
			NavOpts.Init();
			NavOpts.GetValue(AUTOPROTECT_DelayLoad, dwDelayLoad, 0);
		}

		if( !dwDelayLoad )
        {
			// System start
			m_SAVRTDrivers.SetDriversSystemStart();
        }
		else
			// Demand start for the drivers then the AP service will load them
			m_SAVRTDrivers.SetDriversDemandStart();
	}
	else
	{
		// Demand start
		m_SAVRTDrivers.SetDriversDemandStart();
	}

	// Now on NT set the service startup type
	if( m_OSInfo.IsWinNT() )
	{
		//
		// On NT: Startup - Set service to automatic
		//        No Startup - Set service to manual
		//

		// Get a handle to the AP service
		//
		CService APService( NAVAP_SERVICE_NAME );

		if ( !APService.GetCurrentState ())
		{
			CCTRACEE ( "Failed to open AP service" );

			if (forceError (ERR_CANT_REGISTER_SERVICE) || !RegisterAPService ())
			{
				CCTRACEE ( "Failed to register AP service" );
				MakeError ( ERR_CANT_REGISTER_SERVICE, E_FAIL, IDS_NAVERROR_INTERNAL );
				return E_FAIL;
			}
		}

		// Set the start mode for the service.
		//
		if ( forceError (ERR_CANT_CONFIGURE_SERVICE) || !APService.ChangeStartType ( bStartup ? SERVICE_AUTO_START : SERVICE_DEMAND_START ))
		{	
			CCTRACEE ( "Failed to configure AP service" );			
			MakeError ( ERR_CANT_CONFIGURE_SERVICE, E_FAIL, IDS_NAVERROR_INTERNAL );
			return E_FAIL;
		}
	}

	return S_OK;
}

STDMETHODIMP CScriptableAutoProtect::SetSAVRTPELStartMode(int iMode)
{
	// Should we even be here?
	if (forceError (ERR_SECURITY_FAILED) || !IsItSafe())
	{
		MakeError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
		return E_ACCESSDENIED;
	}

	HRESULT hrRetVal = S_OK;

    if( !SetServiceStartupMode("SAVRTPEL", iMode) )
	{
        // The SetServiceStartupMode will do the MakeError(...) for us
		hrRetVal = E_FAIL;
	}

	return hrRetVal;
}

BOOL CScriptableAutoProtect::RegisterAPService()
{
	BOOL bReturn = FALSE;

	// Need to RE-register the service?
	//
	if (m_OSInfo.IsWinNT() == true)
	{
		// Try registering 3 times. Sometimes we have timing issues. ATL related?
		// Defect # 375576
		//
		for ( int iRetries = 0; iRetries < 2; iRetries++ )
		{
			// Register the service
			TCHAR szAPServicePath[MAX_PATH] = {0};
			wsprintf(szAPServicePath, "%s\\%s", m_NAVInfo.GetNAVDir(), szAPService);
			SHELLEXECUTEINFO Info = {0};
			Info.fMask = SEE_MASK_NOCLOSEPROCESS;
			Info.cbSize = sizeof(Info);
			Info.hwnd = NULL;
			Info.lpFile = szAPServicePath;
			Info.lpParameters = _T("/service");
			Info.nShow = SW_SHOWNORMAL;

			CCTRACEI ( "RegisterAPService:Registering AP Service - %s", szAPServicePath );

			ShellExecuteEx(&Info);

			// Wait for it to finish

			while (WaitForSingleObject(Info.hProcess, 1000) == WAIT_TIMEOUT)
			{
				MSG msg = {0};
				while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				};
			}

			CCTRACEI ( "RegisterAPService:Finished registering AP Service" );

			// Check the result of the registration.
			//
			DWORD dwExitCode = 0;
			if ( GetExitCodeProcess ( Info.hProcess, &dwExitCode) )
			{
				CCTRACEI ( "RegisterAPService:Service returned %d", dwExitCode );

				if ( SUCCEEDED ( dwExitCode ))
				{
					bReturn = TRUE;
					CloseHandle(Info.hProcess);
					break;
				}
			}

			CloseHandle(Info.hProcess);
		}
	}
	else // Windows 9x
	{
		// Register the 9x COM Object
		TCHAR szCCappPath[MAX_PATH] = {0};
		TCHAR szParams[MAX_PATH] = {0};
		DWORD dwSize = MAX_PATH;

		// Get the path to ccApp.exe
		::_tcscpy( szCCappPath, m_NAVInfo.GetSymantecCommonDir () );
		::_tcscat( szCCappPath, g_cszCCApp );

		// Build the command line to register the AP COM object
		// i.e: /load:c:\program files\norton antivirus\navapw32.dll /regserver
		wsprintf(szParams, "/load:\"%s\\navapw32.dll\" /regserver", m_NAVInfo.GetNAVDir());

		SHELLEXECUTEINFO Info = {0};
		Info.fMask = SEE_MASK_NOCLOSEPROCESS;
		Info.cbSize = sizeof(Info);
		Info.hwnd = NULL;
		Info.lpFile = szCCappPath;
		Info.lpParameters = szParams;
		Info.nShow = SW_SHOWNORMAL;

		CCTRACEI ( "RegisterAPService:Registering AP c - %s %s", szCCappPath, szParams );

		ShellExecuteEx(&Info);

		// Wait for it to finish

		while (WaitForSingleObject(Info.hProcess, 1000) == WAIT_TIMEOUT)
		{
			MSG msg = {0};
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			};
		}

		CCTRACEI ( "RegisterAPService:Finished registering AP COM Object" );

		// Check the result of the registration.
		//
		DWORD dwExitCode = 0;
		if ( GetExitCodeProcess ( Info.hProcess, &dwExitCode) )
		{
			CCTRACEI ( "RegisterAPService:Registration returned %d", dwExitCode );

			if ( SUCCEEDED ( dwExitCode ))
			{
				bReturn = TRUE;
			}
		}

		CloseHandle(Info.hProcess);
	}

	return bReturn;
}

STDMETHODIMP CScriptableAutoProtect::get_NAVError(INAVCOMError **pVal)
{
	if ( !m_spError )
		return E_FAIL;

	*pVal = m_spError;
	(*pVal)->AddRef (); // We get a ref and the caller gets one

	return S_OK;
}

void CScriptableAutoProtect::MakeError(long lMessageID, long lHResult, long lNAVErrorResID)
{
	if ( !m_spError )
		return;

	m_spError->put_ModuleID ( SAVRT_MODULE_ID_NAVAPSCR );
	m_spError->put_ErrorID ( lMessageID );
	m_spError->put_HResult ( lHResult );
	m_spError->put_ErrorResourceID( lNAVErrorResID );
}

bool CScriptableAutoProtect::forceError(long lErrorID)
{
	if ( !m_spError )
		return false;

	long lTempErrorID = 0;
	long lTempModuleID = 0;

	m_spError->get_ForcedModuleID ( &lTempModuleID );
	m_spError->get_ForcedErrorID ( &lTempErrorID );

	if ( lTempModuleID == SAVRT_MODULE_ID_NAVAPSCR &&
		lTempErrorID == lErrorID )
		return true;
	else
		return false;
}

bool CScriptableAutoProtect::SetServiceStartupMode(LPCTSTR pcszService, DWORD dwMode)
{
    if( !pcszService )
    {
        CCTRACEE(_T("CScriptableAutoProtect::SetServiceStartupMode() - Invalid service name."));
        MakeError ( E_INVALIDARG, E_FAIL, IDS_NAVERROR_INTERNAL );
        return false;
    }

    bool bRet = true;

    SC_HANDLE hMgr = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ALL_ACCESS);

    if(hMgr)
    {
        SC_HANDLE hSvc = OpenService(hMgr, pcszService, SERVICE_CHANGE_CONFIG);

        if(hSvc)
        {
            if(!ChangeServiceConfig(hSvc,
                                    SERVICE_NO_CHANGE,
                                    dwMode,
                                    SERVICE_NO_CHANGE,
                                    NULL,
                                    NULL,
                                    NULL,
                                    NULL,
                                    NULL,
                                    NULL,
                                    NULL ) )
            {
                CCTRACEE(_T("CScriptableAutoProtect::SetServiceStartupMode(%s) - Failed to configure the service = %ld."), pcszService, GetLastError());
                MakeError ( ERR_FAILED_TO_CONFIGURE_SAVRTPEL, E_FAIL, IDS_NAVERROR_INTERNAL );
                bRet = false;
            }
            CloseServiceHandle(hSvc);
        }
        else
        {
            CCTRACEE(_T("CScriptableAutoProtect::SetServiceStartupMode(%s) - Failed to open the service."), pcszService);
            MakeError ( ERR_FAILED_TO_OPEN_SAVRTPEL, E_FAIL, IDS_NAVERROR_INTERNAL );
            bRet = false;
        }

        CloseServiceHandle(hMgr);
    }
    else
    {
        CCTRACEE(_T("CScriptableAutoProtect::SetServiceStartupMode(%s) - Failed to open the SCM."), pcszService);
        MakeError ( ERR_FAILED_TO_OPEN_SCMANAGER, E_FAIL, IDS_NAVERROR_INTERNAL );
        bRet = false;
    }

    return bRet;
}
