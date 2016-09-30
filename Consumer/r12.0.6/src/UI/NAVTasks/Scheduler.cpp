// Scheduler.cpp : Implementation of CScheduler
#include "stdafx.h"
#include "NAVTrust.h"

#include "mstask.h"
#include "NAVTasks.h"
#include "Scheduler.h"
#include "OSInfo.h"
#include "SCSmartHandle.h"
#include "AVccModuleID.h"
#include "AVRESBranding.h"
#include "NAVErrorResource.h"
#include "ccResourceLoader.h"

// Resource Loader
extern cc::CResourceLoader g_ResLoader;

//
// Scheduler defines
//
#define SCHED_CLASS             TEXT("SAGEWINDOWCLASS")
#define SCHED_TITLE             TEXT("SYSTEM AGENT COM WINDOW")
#define SCHED_SERVICE_APP_NAME  TEXT("mstask.exe")
#define SCHED_SERVICE_NAME      TEXT("Schedule")

#define SCHED_RUN_AT_STARTUP_REGKEY_9X      "Software\\Microsoft\\Windows\\CurrentVersion\\RunServices"
#define SCHED_RUN_AT_STARTUP_REGKEY_VAL_9X  "SchedulingAgent"

/////////////////////////////////////////////////////////////////////////////
// CScheduler

CScheduler::CScheduler ()
{
    CComBSTR bstrErrorClass ("NAVError.NAVCOMError");

    // Check NAVError module for Symantec Signature...
	if( NAVToolbox::NAVTRUST_OK == NAVToolbox::IsSymantecComServer(_T("NAVError.NAVCOMError")) )
    {
        if ( FAILED ( m_spError.CoCreateInstance( bstrErrorClass,
                                                  NULL,
                                                  CLSCTX_INPROC_SERVER)))
        {
        }
    }

	CBrandingRes BrandRes;
	m_csProductName = BrandRes.ProductName();
}

HRESULT CScheduler::FinalConstruct()
{
    return S_OK;
}
void CScheduler::FinalRelease()
{

}

STDMETHODIMP CScheduler::get_Running(BOOL *pbRunning)
{
	// Check our script security
    //
    if ( !IsItSafe())
    {
        makeGenericError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
        return E_ACCESSDENIED;
    }
    
    HRESULT hr = E_FAIL;

    //
    // Copied from MSDN article "Starting the Task Scheduler Service"
    //
  
    if ( !m_OSInfo.IsWinNT () )
    {
        //////////////////////////////////////////////////
        // If Windows 95, check to see if Windows 95 
        // version of Task Scheduler is running.
        //////////////////////////////////////////////////
        HWND hwnd = FindWindow(SCHED_CLASS, SCHED_TITLE);

        if (hwnd != NULL)
        {
            // It is already running.
            *pbRunning = TRUE;
        }
        else
            *pbRunning = FALSE;

        return S_OK;
    }
    else
    {
        //////////////////////////////////////////////////
        // If not Windows 95, check to see if Windows NT 
        // version of Task Scheduler is running.
        //////////////////////////////////////////////////
        NAVToolbox::CSCSmartHandle   hSC;
        NAVToolbox::CSCSmartHandle   hSchSvc;

        try 
        {
            *pbRunning = FALSE;
    
            hSC = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
            if (hSC == NULL)
            {
                CCTRACEI ( "Error in OpenSCManager in get_Running");
                throw hr;
            }
    
            hSchSvc = OpenService(hSC,
                                  SCHED_SERVICE_NAME,
                                  SERVICE_QUERY_STATUS);
    
            if (hSchSvc == NULL)
            {
                CCTRACEI ( "Error in OpenService in get_Running");
                throw hr;
            }
    
            SERVICE_STATUS SvcStatus;
    
            // Adding this extra double check gets rid of the problem where we
            // try to add the task when the scheduler is in the process of
            // starting or just not started yet. For some reason the
            // user name/password profile doesn't get set properly if you 
            // add a task when the scheduler is stopped, even though we start it
            // before we add the task.
            //
            DWORD dwAttempts = 0;
            DWORD dwRetries = 50;   // 5 secs
            BOOL bResult = 0;

            //::MessageBox ( ::GetDesktopWindow (), "Waiting for stop", "sched", MB_OK );

            while ( dwAttempts < dwRetries && hr != S_OK )
            {
                dwAttempts ++;

                if (QueryServiceStatus(hSchSvc, &SvcStatus) == FALSE)
                {
                    CCTRACEI ( "Error in QueryServiceStatus in get_Running");
                    throw hr;
                }

                if (SvcStatus.dwCurrentState == SERVICE_RUNNING)
                {
                    // The service is already running.
                    *pbRunning = TRUE;
                    hr = S_OK;
                }
                else if ( SvcStatus.dwCurrentState == SERVICE_STOPPED)
                {
                    *pbRunning = FALSE;
                    hr = S_OK;
                }

                if ( hr != S_OK )
                    Sleep ( 100 ); // wait 1/10th sec.
            };
        }

        catch (...)
        {
            DWORD dwResult = GetLastError ();   // for debugging
            makeGenericError ( ERR_FAILED_COMMUNICATE_SCHEDULER, E_FAIL, IDS_NAVERROR_INTERNAL );
        }


        return hr;
    }
}

STDMETHODIMP CScheduler::put_Running(BOOL bStart)
{
	// Check our script security
    //
    if ( !IsItSafe())
    {
        makeGenericError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
        return E_ACCESSDENIED;
    }
    
    HRESULT hr = E_FAIL;

    // If we want to use the scheduler make sure it's running. If
    // the NT service is set to "Disabled" the StartService will fail.
    //
    if ( bStart )
        put_RunAtStartup ( TRUE );
    
    //
    // Copied from MSDN article "Starting the Task Scheduler Service"
    //

    if ( !m_OSInfo.IsWinNT () )
    {
        if ( bStart )
        {
            // Start the scheduler!
            if ( startScheduler9x())
                hr = S_OK;
            else
            {
                CCTRACEI ( "Error in startScheduler9x in put_Running");
            }
            
            // startScheduler will fill out the NAVError object
            return hr;
        }
        else
        {
            // Stop the scheduler!

	        /////////////////////////////////////////////////
	        // If Windows 95, check to see if Windows 95 
	        // version of Task Scheduler is running.
	        //////////////////////////////////////////////////
            HWND hwnd = FindWindow(SCHED_CLASS, SCHED_TITLE);

	        if (hwnd != NULL)
	        {
                //
                // Tell the scheduler to quit
                //
                PostMessage ( hwnd, WM_QUIT, 0, 0 );

                //
                // Wait 5 secs for it to quit
                //
                if ( waitForScheduler ( FALSE, 50 ))
                {
                    hr = S_OK;
                }
            }

            if ( FAILED (hr))
                CCTRACEI ( "Couldn't FindWindow in put_Running" );
            
            // Told to stop the schedule but it's not running anyway.
            // 
            return S_FALSE;
        }
        // end 9x
	}
	else
	{
        //////////////////////////////////////////////////
        // If not Windows 95, check to see if Windows NT 
        // version of Task Scheduler is running.
        //////////////////////////////////////////////////

        if ( bStart )
        {
            //::MessageBox ( ::GetDesktopWindow (), "Starting", "sched", MB_OK );
            // Start the scheduler!
            
            Sleep ( 1000 ); // Wait one sec for the service to stabilize. This is needed!
            
            // You can't start the scheduler if it is disabled!
            if ( startSchedulerNT() )
                hr = S_OK;
            else
                CCTRACEI ( "Failed to startSchedulerNT in put_Running");
            
            // startScheduler will fill out the NAVError object
            return hr;
        }
        else
        {
            if ( stopSchedulerNT () )
                hr = S_OK;
            else
                CCTRACEI ( "Failed to stopSchedulerNT in put_Running");

            // stopScheduler will fill out the NAVError object
            return hr;
        }   // end stop NT
    }    // end NT
}


STDMETHODIMP CScheduler::get_Installed(BOOL *pbInstalled)
{
	// Check our script security
    //
    try
    {
        if ( !IsItSafe())
        {
            makeGenericError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
            return E_ACCESSDENIED;
        }
        
        // The best way to tell if MTS is installed if you 
        // can get an interface.
        
        CComPtr<ITaskScheduler> sptrTaskScheduler;
        
        StahlSoft::HRX hrx;
        hrx << sptrTaskScheduler.CoCreateInstance(CLSID_CTaskScheduler,
            NULL,
            CLSCTX_INPROC_SERVER);

        *pbInstalled = TRUE;
        
        return S_OK;
    }
    catch(...)
    {
        *pbInstalled = FALSE;
        CCTRACEI ("CoCreateInstance(CLSID_CTaskScheduler) failed in get_Installed");
        makeGenericError ( ERR_UNKNOWN_GET_INSTALLED, E_FAIL, IDS_NAVERROR_INTERNAL );
    }
    return S_OK;

}


STDMETHODIMP CScheduler::get_RunAtStartup(BOOL *pbRunAtStartup)
{
	// Check our script security
    //
    if ( !IsItSafe())
    {
        makeGenericError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
        return E_ACCESSDENIED;
    }
    
    HRESULT hr = E_FAIL;
    *pbRunAtStartup = FALSE;
    
    if ( !m_OSInfo.IsWinNT ())
	{
        //
        // Check to see if the startup reg key value exists. (We really don't care about it's contents)
        //
        CRegKey regkeyTSStartup;

        if ( ERROR_SUCCESS == regkeyTSStartup.Open ( HKEY_LOCAL_MACHINE,
                                                     SCHED_RUN_AT_STARTUP_REGKEY_9X,
                                                     KEY_READ ))
        {
            TCHAR szTempPath [MAX_PATH];
            DWORD dwCount = MAX_PATH;

            if ( ERROR_SUCCESS == regkeyTSStartup.QueryValue ( szTempPath, 
                                                               SCHED_RUN_AT_STARTUP_REGKEY_VAL_9X,
                                                               &dwCount ))
            {
                //hr = S_OK;
                *pbRunAtStartup = TRUE;
            }
            
            regkeyTSStartup.Close ();
        }

        return S_OK;    // If we can't find the key it's OK, just means it's off.
    }
    else
    {
        //////////////////////////////////////////////////
        // If not Windows 95, check to see if Windows NT 
        // version of Task Scheduler is running.
        //////////////////////////////////////////////////
        NAVToolbox::CSCSmartHandle   hSC;
        NAVToolbox::CSCSmartHandle   hSchSvc;
        LPQUERY_SERVICE_CONFIG lpServiceConfig;

        try 
        {
            *pbRunAtStartup = FALSE;    

            hSC = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);

            if (hSC == NULL)
            {
                CCTRACEI ("OpenSCManager failed in get_RunAtStartup");
                throw hr;
            }
    
            hSchSvc = OpenService(hSC,
                                  SCHED_SERVICE_NAME,
                                  SERVICE_QUERY_CONFIG);
    
            if (hSchSvc == NULL)
            {
                CCTRACEI ("OpenService failed in get_RunAtStartup");
                throw hr;
            }
    
		    // Allocate a 4K buffer for the configuration information. 
		    DWORD dwBufferSize = 4096;

		    lpServiceConfig = (LPQUERY_SERVICE_CONFIG) malloc ( dwBufferSize ); 

		    if (lpServiceConfig == NULL)
            {
                CCTRACEI ("(LPQUERY_SERVICE_CONFIG) malloc failed in get_RunAtStartup");
                throw hr;
            }

		    ZeroMemory ( lpServiceConfig, dwBufferSize );

		    DWORD   dwBytesReturned = 0;

		    // Get the configuration information. 

		    if ( !QueryServiceConfig ( hSchSvc,
								       lpServiceConfig,
								       dwBufferSize,
								       &dwBytesReturned ))
            {
                CCTRACEI ("QueryServiceConfig failed in get_RunAtStartup");
                throw hr;
            }
   
            if (lpServiceConfig->dwStartType == SERVICE_AUTO_START)
            {
                // The service is set to run at start up
                *pbRunAtStartup = TRUE;
                hr = S_OK;
            }
            else
            {
                *pbRunAtStartup = FALSE;
                hr = S_OK;
            }
        }

        catch (...)
        {
            DWORD dwResult = GetLastError ();   // for debugging
            makeGenericError ( ERR_FAILED_COMMUNICATE_SCHEDULER, E_FAIL, IDS_NAVERROR_INTERNAL );
        }

        if ( lpServiceConfig )
            free ( lpServiceConfig );

        return hr;
    }
}

STDMETHODIMP CScheduler::put_RunAtStartup(BOOL bRunAtStartup)
{
	// Check our script security
    //
    if ( !IsItSafe())
    {
        makeGenericError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
        return E_ACCESSDENIED;
    }
    
    HRESULT hr = E_FAIL;
  
    if ( !m_OSInfo.IsWinNT () )
	{
        //
        // Win 9x/ME
        //
        
        if ( bRunAtStartup )
        {
            CRegKey regkeyTSStartup;

            //
		    // Create the run at startup key for the Task Scheduler
		    //
		    if ( ERROR_SUCCESS == regkeyTSStartup.Create ( HKEY_LOCAL_MACHINE,
													       SCHED_RUN_AT_STARTUP_REGKEY_9X,
													       REG_NONE,
													       REG_OPTION_NON_VOLATILE,
													       KEY_WRITE ))
		    {
			    if ( ERROR_SUCCESS == regkeyTSStartup.SetValue ( SCHED_SERVICE_APP_NAME,
                                                                 SCHED_RUN_AT_STARTUP_REGKEY_VAL_9X ))
                {
                    hr = S_OK;
		        }

                regkeyTSStartup.Close ();
            }

            if ( FAILED (hr))
            {
                CCTRACEI ( "Create regkeyTSStartup error in put_RunAtStartup");
                makeError ( ERR_REGISTRY_RIGHTS_SCHEDULER, hr );
            }

            return hr;

        }
        else
        {
            //
            // Remove the startup key
            //
            CRegKey regkeyTSStartup;
            
            //
            // KEY_ALL_ACCESS is needed since we are deleting and will work since it's 9x
            //
            if ( ERROR_SUCCESS == regkeyTSStartup.Open ( HKEY_LOCAL_MACHINE,
                                                         SCHED_RUN_AT_STARTUP_REGKEY_9X,
                                                         KEY_ALL_ACCESS ))
            {
                //
                // Delete the reg key. This is the same behavior as MS Task Scheduler!
                //
                if ( ERROR_SUCCESS == regkeyTSStartup.DeleteValue ( SCHED_RUN_AT_STARTUP_REGKEY_VAL_9X ))
                {
                    hr = S_OK;
                }

                regkeyTSStartup.Close ();
            }
           
            if ( FAILED (hr))
            {
                CCTRACEI ( "Remove regkeyTSStartup error in put_RunAtStartup");
                makeError ( ERR_REGISTRY_RIGHTS_SCHEDULER, hr );
            }

            return hr;
        }
	}	// end Win 9x/ME
	else
	{
	    //
	    // NT/2000
	    //
		NAVToolbox::CSCSmartHandle   hSC;
		NAVToolbox::CSCSmartHandle   hSchSvc;
        LPQUERY_SERVICE_CONFIG lpServiceConfig;

        try 
        {
	        DWORD dwStartType = SERVICE_DISABLED;
        
            if ( bRunAtStartup )
            {
                dwStartType = SERVICE_AUTO_START;
            }

		    hSC = OpenSCManager(NULL, NULL, GENERIC_WRITE);

		    if (hSC == NULL)
            {
                CCTRACEI ("OpenSCManager failed in put_RunAtStartup");
                throw hr;
            }

		    hSchSvc = OpenService( hSC,
							       SCHED_SERVICE_NAME,
							       SERVICE_ALL_ACCESS );

		    if ( hSchSvc == NULL )
            {
                CCTRACEI ("OpenSCManager failed in put_RunAtStartup");
                throw hr;
            }

		    // Allocate a 4K buffer for the configuration information. 
		    DWORD dwBufferSize = 4096;

		    lpServiceConfig = (LPQUERY_SERVICE_CONFIG) malloc ( dwBufferSize ); 

		    if (lpServiceConfig == NULL)
            {
                CCTRACEI ("(LPQUERY_SERVICE_CONFIG) malloc failed in put_RunAtStartup");
                throw hr;
            }

		    ZeroMemory ( lpServiceConfig, dwBufferSize );

		    DWORD   dwBytesReturned = 0;

		    // Get the configuration information. 

		    if ( !QueryServiceConfig ( hSchSvc,
								       lpServiceConfig,
								       dwBufferSize,
								       &dwBytesReturned ))
            {
                CCTRACEI ("QueryServiceConfig failed in put_RunAtStartup");
                throw hr;
            }
        
            //
            // Set the start type. Note: this isn't exactly what the MS Task scheduler does
            // when you start/stop it. It just starts/stops the service. So this 
            // should really only be used to start or to *temporarily* stop.
            //
		    if ( 0 == ChangeServiceConfig ( hSchSvc,
										    lpServiceConfig -> dwServiceType,
										    dwStartType,
										    lpServiceConfig -> dwErrorControl,
										    lpServiceConfig -> lpBinaryPathName,
										    lpServiceConfig -> lpLoadOrderGroup,
										    NULL,
										    NULL,
										    NULL,
										    NULL,
										    lpServiceConfig -> lpDisplayName ))
            {
                CCTRACEI ("ChangeServiceConfig failed in put_RunAtStartup");
                throw hr;
            }

            hr = S_OK;
        }	// end NT/2000

        catch (...)
        {
            DWORD dwResult = GetLastError ();   // for debugging
            makeGenericError ( ERR_FAILED_COMMUNICATE_SCHEDULER, hr, IDS_NAVERROR_INTERNAL ); 
        }

        if ( lpServiceConfig )
            free ( lpServiceConfig );



        return hr;
    }
}

BOOL CScheduler::waitForScheduler(BOOL bRunning, DWORD dwRetries )
{

    // Make sure the scheduler is up and running!
    //
    // Adding this extra double check gets rid of the problem where we
    // try to add the task when the scheduler is in the process of
    // starting or just not started yet. For some reason the
    // user name/password profile doesn't get set properly if you 
    // add a task when the scheduler is stopped, even though we start it
    // before we add the task.
    //
    DWORD dwAttempts = 0;
    BOOL bResult = 0;

    //::MessageBox ( ::GetDesktopWindow (), "Waiting for stop", "sched", MB_OK );

    get_Running ( &bResult );

    while ( dwAttempts < dwRetries && bResult != bRunning )
    {
        dwAttempts ++;
        Sleep ( 100 ); // wait 1/10th sec.
        get_Running ( &bResult );
    };

    // Return TRUE if the result matches what was requested.
    //
    return ( bResult == bRunning ) ? TRUE : FALSE; 
}

BOOL CScheduler::startScheduler9x()
{
    // Start the scheduler!
    BOOL bResult = FALSE;

    /////////////////////////////////////////////////
    // If Windows 95, check to see if Windows 95 
    // version of Task Scheduler is running.
    //////////////////////////////////////////////////
    try
    {
        // Is it already running?
        //
        BOOL bRunning = FALSE;

        if (FAILED ( get_Running ( &bRunning )))
            throw bResult;

        if ( bRunning )
        {
            bResult = TRUE;
            throw bResult;
        }

        //  Execute the task scheduler process.
        //
        STARTUPINFO         sui;
        PROCESS_INFORMATION pi;
        ZeroMemory(&sui, sizeof(sui));
        sui.cb = sizeof (STARTUPINFO);
        TCHAR szApp[MAX_PATH];
        LPTSTR pszPath;

        DWORD dwRet = SearchPath(NULL,
						         SCHED_SERVICE_APP_NAME,
						         NULL,
						         MAX_PATH,
						         szApp,
						         &pszPath);

        // Can't find the file
        if (dwRet == 0)
            throw bResult;

        BOOL fRet = CreateProcess(szApp,
						          NULL,
						          NULL,
						          NULL,
						          FALSE,
						          CREATE_NEW_CONSOLE | 
						          CREATE_NEW_PROCESS_GROUP,
						          NULL,
						          NULL,
						          &sui,
						          &pi);

        // Can't start the task scheduler
        //
        if (fRet == 0)
            throw bResult;

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        //
        // Wait 5 secs for it to quit
        //
        if ( waitForScheduler ( TRUE, 50 ))
        {
            bResult = TRUE;
        }
    }

    catch (...)
    {
        makeError ( ERR_START_SCHEDULER, E_FAIL );
    }

    return bResult;
}

BOOL CScheduler::startSchedulerNT()
{
    BOOL bResult = FALSE;

    NAVToolbox::CSCSmartHandle   hSC;
    NAVToolbox::CSCSmartHandle   hSchSvc;            

    try 
    {
        hSC = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
        if (hSC == NULL)
            throw bResult;

        hSchSvc = OpenService(hSC,
					          SCHED_SERVICE_NAME,
					          SERVICE_START |
                              SERVICE_QUERY_STATUS |
                              SERVICE_PAUSE_CONTINUE );

        if (hSchSvc == NULL)
            throw bResult;

        SERVICE_STATUS SvcStatus;

        if (QueryServiceStatus(hSchSvc, &SvcStatus) == FALSE)
            throw bResult;

        // Tell it to start
        //
	    if ( SvcStatus.dwCurrentState == SERVICE_STOPPED )
            bResult = StartService(hSchSvc, 0, NULL);
        else
            bResult = ControlService ( hSchSvc, SERVICE_CONTROL_CONTINUE, &SvcStatus );

        if ( !bResult )
        {
            DWORD dwLastError = GetLastError ();
            throw bResult;
        }

        //
        // Wait 5 secs for scheduler to start
        //
        if ( waitForScheduler ( TRUE, 50 ))
            bResult = TRUE;
    }

    catch (...)
    {
        DWORD dwResult = GetLastError ();   // for debugging
        makeError ( ERR_START_SCHEDULER, E_FAIL );
    }


    return bResult;
}

BOOL CScheduler::stopSchedulerNT()
{
    //
    // Stop the scheduler service
    //
    BOOL bResult = FALSE;

    NAVToolbox::CSCSmartHandle   hSC;
	NAVToolbox::CSCSmartHandle   hSchSvc;

    try 
    {
    	hSC = OpenSCManager(NULL, NULL, GENERIC_WRITE);

		if (hSC == NULL)
            throw bResult;

		hSchSvc = OpenService( hSC,
							   SCHED_SERVICE_NAME,
							   SERVICE_STOP );

		if ( hSchSvc == NULL )
            throw bResult;

        SERVICE_STATUS SvcStatus;
        
        //::MessageBox ( ::GetDesktopWindow (), "Stopping", "sched", MB_OK );

        ControlService ( hSchSvc, SERVICE_CONTROL_STOP, &SvcStatus );

        //
        // Wait 5 secs for scheduler to stop
        //
        if ( waitForScheduler ( FALSE, 1 ))
            bResult = TRUE;
            //::MessageBox ( ::GetDesktopWindow (), "Failed while waiting", "sched", MB_OK );

    }

    catch (...)
    {
        //::MessageBox ( ::GetDesktopWindow (), "Exception in Stop", "sched", MB_OK );
        DWORD dwResult = GetLastError ();   // for debugging
        makeError ( ERR_STOP_SCHEDULER, E_FAIL );
    }


    return bResult;
}

STDMETHODIMP CScheduler::get_NAVError(INAVCOMError **pVal)
{
	if ( !m_spError )
        return E_FAIL;

    *pVal = m_spError;
    (*pVal)->AddRef (); // We get a ref and the caller gets one

	return S_OK;
}

void CScheduler::makeError(long lMessageID, long lHResult)
{
	if ( !m_spError )
		return;

	CString csMessage;
	CString csFormat;

    g_ResLoader.LoadString(lMessageID,csFormat);
	csMessage = csFormat;

	switch(lMessageID)
	{
	case ERR_UNKNOWN_SAVE:
	case ERR_UNKNOWN_SCHEDULE:
	case ERR_REGISTRY_RIGHTS_SCHEDULER:
	case ERR_STOP_SCHEDULER:
		csMessage.Format(csFormat, m_csProductName);
		break;

	default:
		break;
	}

	CComBSTR bstrErrorMessage( csMessage );
	m_spError->put_Message ( bstrErrorMessage );
	m_spError->put_ModuleID ( AV_MODULE_ID_SCAN_TASKS );
	m_spError->put_ErrorID ( lMessageID );
	m_spError->put_HResult ( lHResult );
}

void CScheduler::makeGenericError(long lErrorID, long lHResult, long lNAVErrorResID)
{
	if ( !m_spError )
		return;

	m_spError->put_ModuleID ( AV_MODULE_ID_SCAN_TASKS );
	m_spError->put_ErrorID ( lErrorID );
	m_spError->put_ErrorResourceID ( lNAVErrorResID );
	m_spError->put_HResult ( lHResult );
}
