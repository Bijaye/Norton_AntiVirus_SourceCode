////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// APChecks.cpp - This file contains the class that verifies the user mode AP components
//

#include "stdafx.h"
#include "APChecks.h"
#include <Shellapi.h>
#include "SVCCTRL.H"


// Implementation of class that checks Navapsvc
//
CCheckNavapsvc::CCheckNavapsvc() : CDriverCheck(_T("Navapsvc"))
{
}

// This will re-register the service - stolen from the navapscr object
bool CCheckNavapsvc::Install()
{
	// Register the service
	TCHAR szAPServicePath[MAX_PATH] = {0};
	wsprintf(szAPServicePath, "%s\\%s", g_NAVInfo.GetNAVDir(), _T("navapsvc.exe"));
	SHELLEXECUTEINFO Info = {0};
	Info.fMask = SEE_MASK_NOCLOSEPROCESS;
	Info.cbSize = sizeof(Info);
	Info.hwnd = NULL;
	Info.lpFile = szAPServicePath;
	Info.lpParameters = _T("/service");
	Info.nShow = SW_SHOWNORMAL;

    g_log.Log ( "RegisterAPService:Registering AP Service - %s", szAPServicePath );

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

    g_log.Log ( "RegisterAPService:Finished registering AP Service" );

    // Check the result of the registration.
    //
    DWORD dwExitCode = 0;
    if ( GetExitCodeProcess ( Info.hProcess, &dwExitCode) )
    {
        g_log.Log ( "RegisterAPService:Service returned %d", dwExitCode );
    }

    CloseHandle(Info.hProcess);

	// Now set the service to load automatically
	// Make sure the service can be opened by the Service control manager
	CServiceControl cService;

	if (ERROR_SUCCESS != cService.Open(_T("navapsvc")))
	{
		g_log.Log(_T("Error: CCheckNavapsvc::Install() - Could not open the navapsvc service"));
	}

	if( ERROR_SUCCESS != cService.ChangeServiceConfig(SERVICE_NO_CHANGE,
													  SERVICE_AUTO_START) )
	{
		g_log.Log(_T("Error: CCheckNavapsvc::Install() - Could not set the navapsvc service to auto-start"));
	}

	return true;
}

bool CCheckNavapsvc::VerifyStartType()
{
	// Start can be 2 or 3 depending on the users settings

	if( !CDriverCheck::VerifyStartType() )
	{
		// Set the StartType to Automatic
		if( ERROR_SUCCESS != m_ServiceKey.SetValue(2, _T("Start")) )
		{
			g_log.Log(_T("Error: CCheckNavapsvc::VerifyStartType() - The Start mode for Navapsvc could not be set to 2."));
			return false;
		}

		g_log.Log(_T("CCheckNavapsvc::VerifyStartType() - The Start mode for Navapsvc was changed to 2."));
	}

	return true;
}

bool CCheckNavapsvc::ExtraChecks()
{
	// Make sure the service is signed.
	if( !FileExistsAndSigned(m_szPath) )
		return false;

	// Make sure the service can be opened by the Service control manager
	CServiceControl cService;

	if (ERROR_SUCCESS != cService.Open(_T("navapsvc")))
	{
		g_log.Log(_T("Error: CCheckNavapsvc::ExtraChecks() - Could not open the navapsvc service"));
		return false;
	}
	

	return true;
}

bool CCheckNavapsvc::StopService()
{
	CServiceControl cService;
	DWORD dwStatus = TRUE;

	if (ERROR_SUCCESS != cService.Open(_T("navapsvc")))
	{
		g_log.Log(_T("Error: CCheckNavapsvc::StopService() - Could not open the navapsvc service"));
		return false;
	}

	if (!cService.QueryServiceStatus(&dwStatus))
	{
		g_log.Log(_T("Error: CCheckNavapsvc::StopService() - Unable to Query the navapsvc service status"));
		return false;
	}

	// if it is not stopped, try to stop it.
	if (dwStatus == SERVICE_STOPPED)
	{
		// Already stopped
		g_log.Log(_T("CCheckNavapsvc::StopService() - The service is already stopped"));
		return true;
	}

	if (cService.Stop() != ERROR_SUCCESS)
	{
		g_log.Log(_T("Error: CCheckNavapsvc::StopService() - Failed to stop the navapsvc service."));
		return false;
	}

	return true;
}

bool CCheckNavapsvc::StartService()
{
	CServiceControl cService;
	DWORD dwStatus = TRUE;

	if (ERROR_SUCCESS != cService.Open(_T("navapsvc")))
	{
		g_log.Log(_T("Error: CCheckNavapsvc::StartService() - Could not open the navapsvc service"));
		return false;
	}

	if (!cService.QueryServiceStatus(&dwStatus))
	{
		g_log.Log(_T("Error: CCheckNavapsvc::StartService() - Unable to Query the navapsvc service status"));
		return false;
	}

	// if it is not running, try to start it.
	if (dwStatus == SERVICE_RUNNING)
	{
		// Already running
		g_log.Log(_T("CCheckNavapsvc::StartService() - The service is already running"));
		return true;
	}

	// Attempt to stop the service first just in case...
	StopService();

	if (cService.Start() != ERROR_SUCCESS)
	{
		g_log.Log(_T("Error: CCheckNavapsvc::StartService() - Failed to start the navapsvc service."));

		// Attempt to re-register the service
		Install();

		// Try to stop and start it again
		StopService();
		if (cService.Start() != ERROR_SUCCESS)
		{
			g_log.Log(_T("Error: CCheckNavapsvc::StartService() - Failed to start the navapsvc service after re-registering."));
			return false;
		}
	}

	g_log.Log(_T("CCheckNavapsvc::StartService() - Successfully started the navapsvc service."));

	return true;
}

bool CheckAPFiles()
{
	TCHAR szNavapScr[MAX_PATH] = {0};
	TCHAR szNavapw32[MAX_PATH] = {0};

	_tcscpy(szNavapScr, g_NAVInfo.GetNAVDir());
	_tcscat(szNavapScr, _T("\\navapscr.dll"));

	_tcscpy(szNavapw32, g_NAVInfo.GetNAVDir());
	_tcscat(szNavapw32, _T("\\navapw32.dll"));

	if( !FileExistsAndSigned(szNavapScr) || !FileExistsAndSigned(szNavapw32) )
		return false;

	if( !RegisterComDll(szNavapScr) )
		return false;

	return true;
}

bool IsAPServiceRunning()
{
	// Make sure the AP Agent is running
	TCHAR szNAVAPrunningMutex[64];
	if( g_OSInfo.IsTerminalServicesInstalled() )	
		wsprintf( szNAVAPrunningMutex, _T("Global\\%s"), _T("NAVAPSVC_MUTEX") );
	else
		lstrcpyn( szNAVAPrunningMutex, _T("NAVAPSVC_MUTEX"), 64 );

	// Is the AP service or agent running on this computer?
	//
    HANDLE hMutex = OpenMutex( SYNCHRONIZE, FALSE, szNAVAPrunningMutex );

	if( hMutex )
	{
		// Yes
		CloseHandle( hMutex );
		g_log.Log(_T("The AP Service mutex was found. The Service is running."));
		return true;
    }

	g_log.Log(_T("Error: The AP Service mutex was not found. The Service is not running."));
	return false;
}

bool IsAPAgentRunning()
{
	HANDLE hMutex = OpenMutex( SYNCHRONIZE, FALSE, _T("Norton AntiVirus Agent Is Running") );

	if( hMutex )
	{
		// Yes
		CloseHandle( hMutex );
        g_log.Log(_T("The AP Agent mutex was found. The Agent is running."));
		return true;
    }

	g_log.Log(_T("Error: The AP Agent mutex was not found. The Agent is not running."));
	return false;
}