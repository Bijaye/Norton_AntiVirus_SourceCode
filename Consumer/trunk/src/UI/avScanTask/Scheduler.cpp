////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// Scheduler.cpp : implementation file
//
#include "stdafx.h"
#include "Scheduler.h"
#include "SCSmartHandle.h"

// Scheduler defines
LPCWSTR SCHED_SERVICE_NAME      = L"Schedule";

//-------------------------------------------------------------------------
HRESULT CScheduler::GetRunning(bool& bRunning) throw()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		//
		// Copied from MSDN article "Starting the Task Scheduler Service"
		//
	  
		//////////////////////////////////////////////////
		// If not Windows 95, check to see if Windows NT 
		// version of Task Scheduler is running.
		//////////////////////////////////////////////////
		NAVToolbox::CSCSmartHandle   hSC;
		NAVToolbox::CSCSmartHandle   hSchSvc;

        bRunning = false;

        hSC = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
		hrx << (!hSC? E_FAIL : S_OK);

        hSchSvc = OpenService(hSC, SCHED_SERVICE_NAME, SERVICE_QUERY_STATUS);
		hrx << (!hSchSvc? E_FAIL : S_OK);

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
		hr = E_FAIL; 

        while((dwAttempts < dwRetries) && (S_OK != hr))
        {
            ++dwAttempts;

            if(!QueryServiceStatus(hSchSvc, &SvcStatus))
            {
                CCTRCTXE0(L"QueryServiceStatus failed");
                hrx << E_FAIL;
            }

            if(SERVICE_RUNNING == SvcStatus.dwCurrentState)
            {
                // The service is already running.
                bRunning = true;
				hr = S_OK;
            }
            else if (SERVICE_STOPPED == SvcStatus.dwCurrentState)
            {
                bRunning = false;
                hr = S_OK;
            }

            if(S_OK != hr)
                Sleep(100); // wait 1/10th sec.
        }
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
	CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}
//-------------------------------------------------------------------------
HRESULT CScheduler::SetRunning(bool bStart) throw()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		// If we want to use the scheduler make sure it's running. If
		// the NT service is set to "Disabled" the StartService will fail.
		//
		if(bStart)
			SetRunAtStartup(true);
    
		//
		// Copied from MSDN article "Starting the Task Scheduler Service"
		//

		//////////////////////////////////////////////////
		// Check to see if Windows NT 
		// version of Task Scheduler is running.
		//////////////////////////////////////////////////

		if(bStart)
		{
			// Start the scheduler!
			Sleep(1000); // Wait one sec for the service to stabilize. This is needed!
	        
			// You can't start the scheduler if it is disabled!
			hrx << startScheduler();
		}
		else
		{
			hrx << stopScheduler();
		}   
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
	CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}
//-------------------------------------------------------------------------
HRESULT CScheduler::GetRunAtStartup(bool& bRunAtStartup) throw()
{
	LPQUERY_SERVICE_CONFIG lpServiceConfig = NULL;

	STAHLSOFT_HRX_TRY(hr)
	{
	    bRunAtStartup = false;
    
		//////////////////////////////////////////////////
		// Check to see if Windows NT 
		// version of Task Scheduler is running.
		//////////////////////////////////////////////////
		NAVToolbox::CSCSmartHandle   hSC;
		NAVToolbox::CSCSmartHandle   hSchSvc;

        hSC = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
        if(!hSC)
        {
            CCTRCTXE0(L"OpenSCManager failed");
			hrx << E_FAIL;
        }

        hSchSvc = OpenService(hSC, SCHED_SERVICE_NAME, SERVICE_QUERY_CONFIG);
        if(!hSchSvc)
        {
            CCTRCTXE0(L"OpenService failed");
			hrx << E_FAIL;
		}

		// Allocate a 4K buffer for the configuration information. 
		DWORD dwBufferSize = 4096;

		lpServiceConfig = (LPQUERY_SERVICE_CONFIG) malloc(dwBufferSize); 

		if(!lpServiceConfig)
        {
            CCTRCTXE0(L"(LPQUERY_SERVICE_CONFIG) malloc failed");
            hrx << E_FAIL;
        }

		ZeroMemory(lpServiceConfig, dwBufferSize);

		DWORD dwBytesReturned = 0;

		// Get the configuration information. 
		if(!QueryServiceConfig(hSchSvc,
								    lpServiceConfig,
								    dwBufferSize,
								    &dwBytesReturned))
        {
            CCTRCTXE0(L"QueryServiceConfig failed");
            hrx << E_FAIL;
        }

        if(SERVICE_AUTO_START == lpServiceConfig->dwStartType)
        {
            // The service is set to run at start up
            bRunAtStartup = true;
        }
        else
        {
            bRunAtStartup = false;
        }
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)

	if(lpServiceConfig)
		free(lpServiceConfig);

	CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}
//-------------------------------------------------------------------------
HRESULT CScheduler::SetRunAtStartup(bool bRunAtStartup) throw()
{
	LPQUERY_SERVICE_CONFIG lpServiceConfig = NULL;

	STAHLSOFT_HRX_TRY(hr)
	{
		NAVToolbox::CSCSmartHandle hSC;
		NAVToolbox::CSCSmartHandle hSchSvc;

	    DWORD dwStartType = SERVICE_DISABLED;
    
        if(bRunAtStartup)
        {
            dwStartType = SERVICE_AUTO_START;
        }

		hSC = OpenSCManager(NULL, NULL, GENERIC_WRITE);

		if(!hSC)
        {
            CCTRCTXE0(L"OpenSCManager failed");
            hrx << E_FAIL;
        }

		hSchSvc = OpenService( hSC,
							    SCHED_SERVICE_NAME,
							    SERVICE_ALL_ACCESS );

		if(!hSchSvc)
        {
			CCTRCTXE0(L"OpenService failed");
			hrx << E_FAIL;
        }

		// Allocate a 4K buffer for the configuration information. 
		DWORD dwBufferSize = 4096;

		lpServiceConfig = (LPQUERY_SERVICE_CONFIG) malloc(dwBufferSize); 

		if(!lpServiceConfig)
        {
            CCTRCTXE0(L"(LPQUERY_SERVICE_CONFIG) malloc failed");
			hrx << E_FAIL;
        }

		ZeroMemory(lpServiceConfig, dwBufferSize);

		DWORD dwBytesReturned = 0;

		// Get the configuration information. 
		if(!QueryServiceConfig(hSchSvc, lpServiceConfig,
			dwBufferSize, &dwBytesReturned))
        {
            CCTRCTXE0(L"QueryServiceConfig failed in put_RunAtStartup");
			hrx << E_FAIL;
        }
    
        //
        // Set the start type. Note: this isn't exactly what the MS Task scheduler does
        // when you start/stop it. It just starts/stops the service. So this 
        // should really only be used to start or to *temporarily* stop.
        //
		if(!ChangeServiceConfig(hSchSvc,
										lpServiceConfig->dwServiceType,
										dwStartType,
										lpServiceConfig->dwErrorControl,
										lpServiceConfig->lpBinaryPathName,
										lpServiceConfig->lpLoadOrderGroup,
										NULL,
										NULL,
										NULL,
										NULL,
										lpServiceConfig->lpDisplayName))
        {
            CCTRCTXE0(L"ChangeServiceConfig failed");
			hrx << E_FAIL;
        }
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)

	if(lpServiceConfig)
		free(lpServiceConfig);

	CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}
//-------------------------------------------------------------------------
HRESULT CScheduler::waitForScheduler(bool bRunning, DWORD dwRetries)
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
    bool bResult = false;

    GetRunning(bResult);

    while(dwAttempts < dwRetries && bResult != bRunning)
    {
        dwAttempts ++;
        Sleep(100); // wait 1/10th sec.
        GetRunning(bResult);
    };

    // Return TRUE if the result matches what was requested.
    //
    return((bResult == bRunning) ? S_OK : E_FAIL); 
}
//-------------------------------------------------------------------------
HRESULT CScheduler::startScheduler()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		NAVToolbox::CSCSmartHandle hSC;
		NAVToolbox::CSCSmartHandle hSchSvc;            

		hSC = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
		hrx << (!hSC? E_FAIL : S_OK);

        hSchSvc = OpenService(hSC,
					          SCHED_SERVICE_NAME,
					          SERVICE_START |
                              SERVICE_QUERY_STATUS |
                              SERVICE_PAUSE_CONTINUE );
		hrx << (!hSchSvc? E_FAIL : S_OK);

        SERVICE_STATUS SvcStatus;

		if(!QueryServiceStatus(hSchSvc, &SvcStatus))
            hrx << E_FAIL;

		BOOL bResult = FALSE;

        // Tell it to start
        //
	    if(SERVICE_STOPPED == SvcStatus.dwCurrentState)
            bResult = StartService(hSchSvc, 0, NULL);
        else
            bResult = ControlService(hSchSvc, SERVICE_CONTROL_CONTINUE, &SvcStatus);

        if(!bResult)
        {
            hrx << E_FAIL;
        }

        // Wait 5 secs for scheduler to start
        hrx << waitForScheduler(true, 50);
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
	CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}
//-------------------------------------------------------------------------
HRESULT CScheduler::stopScheduler()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		// Stop the scheduler service
		NAVToolbox::CSCSmartHandle hSC;
		NAVToolbox::CSCSmartHandle hSchSvc;

    	hSC = OpenSCManager(NULL, NULL, GENERIC_WRITE);
		hrx << (!hSC? E_FAIL : S_OK);

		hSchSvc = OpenService(hSC, SCHED_SERVICE_NAME, SERVICE_STOP);
		hrx << (!hSchSvc? E_FAIL : S_OK);

        SERVICE_STATUS SvcStatus;
        ControlService(hSchSvc, SERVICE_CONTROL_STOP, &SvcStatus);

        // Wait 5 secs for scheduler to stop
        hrx << waitForScheduler(false, 1);
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
	CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}

