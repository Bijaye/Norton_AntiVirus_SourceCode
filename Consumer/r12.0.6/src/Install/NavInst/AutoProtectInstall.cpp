#include "stdafx.h"
#include "NAVInfo.h"
#include "NAVWCOMM.H"
#include "helper.h"
#include "NAVDefutilsLoader.h"
#include "unmark.h"

#define NAVAP_WND_CLASS                   _T("NAVAP Wnd Class")
#define NUMBER_OF_SERVICE				  _T("NumberOfService")
#define NAVEX15							  _T("\\NAVEX15.SYS")
#define NAVENG							  _T("\\NAVENG.SYS")
#define SAVRT							  _T("SAVRT.SYS")
#define SAVRTPEL						  _T("SAVRTPEL.SYS")

// Register the AP service (navapsvc.exe)
// returns TRUE if successful
extern "C" __declspec(dllexport) UINT __stdcall RegisterAPService(MSIHANDLE hInstall)
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug RegisterAPService()", "Debug Break", NULL);
#endif

	// Try registering 3 times. Sometimes we have timing issues. ATL related?
	// Defect # 375576
	//
	for ( int iRetries = 0; iRetries < 2; iRetries++ )
	{	
		// Register the service
		TCHAR szAPService[MAX_PATH] = {0};
		TCHAR szNavDir[MAX_PATH] = {0};
		DWORD dwSize = MAX_PATH;
		
		CNAVInfo NAVInfo;
		wsprintf(szNavDir, "%s", NAVInfo.GetNAVDir());
		
		wsprintf(szAPService, "%s\\navapsvc.exe", szNavDir);
		SHELLEXECUTEINFO Info = {0};
		Info.fMask = SEE_MASK_NOCLOSEPROCESS;
		Info.cbSize = sizeof(Info);
		Info.hwnd = NULL;
		Info.lpFile = szAPService;
		Info.lpParameters = _T("/service");
		Info.nShow = SW_SHOWNORMAL;

		g_Log.Log ( "ChangeLinks:Registering AP Service - %s", szAPService );
	
		ShellExecuteEx(&Info);

		// Wait for it to finish

		while (WaitForSingleObject(Info.hProcess, 100) == WAIT_TIMEOUT)
		{
			MSG msg = {0};
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			};
		}

		g_Log.Log ( "ChangeLinks:Finished registering AP Service" );

		// Check the result of the registration.
		//
		DWORD dwExitCode = 0;
		if ( GetExitCodeProcess ( Info.hProcess, &dwExitCode) )
		{
			g_Log.Log ( "ChangeLinks:Service returned %d", dwExitCode );
        
			if ( SUCCEEDED (dwExitCode))
			{
				CloseHandle(Info.hProcess);
				break;
			}
		}

		CloseHandle(Info.hProcess);
	}

	return ERROR_SUCCESS;
}

extern "C" __declspec(dllexport) UINT __stdcall UnregisterAPService(MSIHANDLE hInstall)
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug UnregisterAPService()", "Debug Break", NULL);
#endif

	// Try registering 3 times. Sometimes we have timing issues. ATL related?
	// Defect # 375576
	//
	for ( int iRetries = 0; iRetries < 2; iRetries++ )
	{
		// Unregister the service
		TCHAR szAPService[MAX_PATH] = {0};
		TCHAR szNavDir[MAX_PATH] = {0};
		DWORD dwSize = MAX_PATH;
		
		CNAVInfo NAVInfo;
		wsprintf(szNavDir, "%s", NAVInfo.GetNAVDir());
		
		wsprintf(szAPService, "%s\\navapsvc.exe", szNavDir);
		SHELLEXECUTEINFO Info = {0};
		Info.fMask = SEE_MASK_NOCLOSEPROCESS;
		Info.cbSize = sizeof(Info);
		Info.hwnd = NULL;
		Info.lpFile = szAPService;
		Info.lpParameters = _T("/unregserver");
		Info.nShow = SW_SHOWNORMAL;

		g_Log.Log ( "ChangeLinks:Unregistering AP Service - %s", szAPService );
	
		ShellExecuteEx(&Info);

		// Wait for it to finish

		while (WaitForSingleObject(Info.hProcess, 100) == WAIT_TIMEOUT)
		{
			MSG msg = {0};
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			};
		}

		g_Log.Log ( "ChangeLinks:Finished unregistering AP Service" );

		// Check the result of the unregistration.
		//
		DWORD dwExitCode = 0;
		if ( GetExitCodeProcess ( Info.hProcess, &dwExitCode) )
		{
			g_Log.Log ( "ChangeLinks:UnregService returned %d", dwExitCode );
        
			if ( SUCCEEDED (dwExitCode))
			{
				CloseHandle(Info.hProcess);
				break;
			}
		}

		CloseHandle(Info.hProcess);
	}

	return ERROR_SUCCESS;
}

extern "C" __declspec(dllexport) UINT __stdcall QuoteSvcBinaryNavapsvc( MSIHANDLE hInstall )
{
	QuoteServiceBinaryPath(_T("navapsvc"));

	return ERROR_SUCCESS;
}


extern "C" __declspec(dllexport) UINT __stdcall StopSingleService(MSIHANDLE hInstall)
{
	BOOL  bStopService = TRUE;
	BOOL  bDeleteService = TRUE;

    TCHAR szServiceName[MAX_PATH] = {0};
	TCHAR szNumOfService[MAX_PATH] = {0};
	TCHAR szTemp[MAX_PATH]	= {0};	

	int dwNumOfService = 0;
	DWORD dwSize = MAX_PATH;
	BOOL bReturn = TRUE;
    CServiceControl cService;
    DWORD dwStatus = TRUE;
	
	
	MsiGetProperty(hInstall, NUMBER_OF_SERVICE, szNumOfService, &dwSize);		
	dwNumOfService = atoi(szNumOfService);
	
	for (int i = 1; i <= dwNumOfService; i++ )
	{

		// Get the service Name.
		dwSize = MAX_PATH;
		wsprintf(szTemp, _T("ServiceName:%d"), i);
		MsiGetProperty(hInstall, szTemp, szServiceName, &dwSize);			
	
		if (ERROR_SUCCESS == cService.Open(szServiceName))
		{

	        if  ((bStopService)
		    &&  (cService.QueryServiceStatus(&dwStatus))
			&&  (SERVICE_STOPPED != dwStatus))
			{
				// if it is not stopped, try to stop it
				if (ERROR_SUCCESS != cService.Stop())
                {
					bReturn = FALSE;				   
					g_Log.Log ( "StopSingleServiceHelp: Stop Service FAILED");
                }
            }

			if (bDeleteService)
            {
				// don't do error checking here because there are times when delete will
				// fail, but the services are set up correctly 
				cService.Delete();
            }
        }
    
		else    // failed to open it?
        {
			bReturn = FALSE;			
			g_Log.Log ( "StopSingleServiceHelp: Open Service FAILED");
        }
	
	} //end of for loop

    return ERROR_SUCCESS;

}  // end of "StopSingleService"


/////////////////////////////////////////////////////////////////////////////////////////////
//  RemoveAPUI								KEVIN MONG
//
//  This function attempts to close the AP window, if it is running.
//
//  Input:  nothing
//  Output: a value of TRUE if the window was found, FALSE if it was not
//
/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) UINT __stdcall RemoveAPUI(MSIHANDLE hInstall)
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug RemoveAPUI()", "Debug Break", NULL);
#endif

	BOOL bReturnValue;
    HWND hNavAp;
	UINT uNavApWMessage = 0L;

    hNavAp = FindWindow(NAVAP_WND_CLASS, NULL);
    if (hNavAp)
    {
		// Get the custom message that NAVW uses to talk to the agent.
		uNavApWMessage = RegisterWindowMessage (NAVW_NAVAPW_COMM);

		// Send unload.
		if ( uNavApWMessage != NULL )
		{
			PostMessage(hNavAp,uNavApWMessage, NAVW_CMD_UNLOADNAVAPW, 0L);
			bReturnValue = TRUE;
        }
    }
    else
    {
		g_Log.Log ( "RemoveAPUIHelp: Unable to find AP window");
		bReturnValue = FALSE;
    }

    return ERROR_SUCCESS;
}

BOOL InstallSingleService(LPTSTR szBinaryName, LPTSTR szServiceDisplayName, LPTSTR szServiceName,
								DWORD dwServiceType, BOOL bAutoStart, LPTSTR szDependancies = NULL)
{
	BOOL bStatus;			
	CServiceControl cService;

	if (ERROR_SUCCESS == cService.Open(szServiceName))
	{
		// service exists, change its config
		if (ERROR_SUCCESS != cService.ChangeServiceConfig(
                               dwServiceType,                  // Service type
                               bAutoStart ?                    // do we want autostart?
                               SERVICE_AUTO_START :            // attempt autostart
                               SERVICE_DEMAND_START,           // manual start
                               SERVICE_ERROR_NORMAL,           // severity if service fails
                               szBinaryName,                   // path to the program binary
							   0, 0,						   // unused params
							   szDependancies))				   // dependancies
        {
			// fixing defect #409698 -- since we moved th SAVRTPEL driver but its marked for deletion it
			// can't be updated by the installer, we need to manually do this
			TCHAR szKeyName[MAX_PATH] = {0};
			wsprintf(szKeyName, _T("SYSTEM\\CurrentControlSet\\Services\\%s"), szServiceName);
            
			TCHAR szValue[MAX_PATH] = {0};
			wsprintf(szValue, _T("\\??\\%s"), szBinaryName);
			
			ATL::CRegKey RegKey;
			RegKey.Create(HKEY_LOCAL_MACHINE, szKeyName);
			RegKey.SetStringValue(_T("ImagePath"), szValue);
			RegKey.SetStringValue(_T("DisplayName"), szServiceDisplayName);
			RegKey.SetDWORDValue(_T("StartType"), bAutoStart ? SERVICE_AUTO_START : SERVICE_DEMAND_START);
			RegKey.SetDWORDValue(_T("Type"), dwServiceType);
			
			bStatus = FALSE;
			g_Log.Log ( "InstallSingleService: Open Service FAILED");			
        }
    }
	else
    {			
		// initialize our service object; must feed it at least one parameter
		// or the compiler will complain about us being ambiguous
		cService.Open((LPTSTR) NULL);
		if (ERROR_SUCCESS != cService.CreateService(
                             szServiceName,                // name of service
                             szServiceDisplayName,         // display name
                             SERVICE_ALL_ACCESS,             // type of access to service
                             dwServiceType,                  // type of service
                             bAutoStart ?                    // when to start service
                             SERVICE_AUTO_START :            // attempt autostart
                             SERVICE_DEMAND_START,           // manual start
                             SERVICE_ERROR_NORMAL,           // severity if service fails
                             szBinaryName,                   // Binary name
							 0, 0,							 // unused
							 szDependancies))				 // dependancies
        {
			// fixing defect #409698 -- since we moved th SAVRTPEL driver but its marked for deletion it
			// can't be updated by the installer, we need to manually do this
			TCHAR szKeyName[MAX_PATH] = {0};
			wsprintf(szKeyName, _T("SYSTEM\\CurrentControlSet\\Services\\%s"), szServiceName);
            
			TCHAR szValue[MAX_PATH] = {0};
			wsprintf(szValue, _T("\\??\\%s"), szBinaryName);
			
			ATL::CRegKey RegKey;
			RegKey.Create(HKEY_LOCAL_MACHINE, szKeyName);
			RegKey.SetStringValue(_T("ImagePath"), szValue);
			RegKey.SetStringValue(_T("DisplayName"), szServiceDisplayName);
			RegKey.SetDWORDValue(_T("StartType"), bAutoStart ? SERVICE_AUTO_START : SERVICE_DEMAND_START);
			RegKey.SetDWORDValue(_T("Type"), dwServiceType);
			
			bStatus = FALSE;
			g_Log.Log ( "InstallSingleService: Create Service FAILED");			
        }
	}


	return (bStatus);
}

BOOL StartSingleService(LPTSTR szServiceName)
{
	CServiceControl cService;
	BOOL bStatus = TRUE;
	
	if  ( ERROR_SUCCESS != cService.Open(szServiceName) )
	{
		bStatus = FALSE;
		g_Log.Log ( "InstallSingleService: Open Service FAILED when service installed successfully");				
	}
	else
	{
		if ( ERROR_SUCCESS != cService.Start() )
		{
			bStatus = FALSE;
			g_Log.Log ( "InstallSingleService: Start Service FAILED");	
		}
	}
	return bStatus;
}

extern "C" __declspec(dllexport) UINT __stdcall StartAPDrivers(MSIHANDLE hInstall)
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug StartAPDrivers()", "Debug Break", NULL);
#endif

	BOOL bReturn = TRUE;
	TCHAR szRunServices[MAX_PATH] = {0};	
	TCHAR *szServiceName = NULL;
	DWORD dwSize = MAX_PATH;
	
	// The list of drivers that need to be started are put into the property "StartAPDrivers"
	// so that this deferred action can pick up the list.  The list is just service names
	// delimited by semicolons.
	MsiGetProperty(hInstall, _T("CustomActionData"), szRunServices, &dwSize);		
	szServiceName = _tcstok(szRunServices, _T(";"));
	
	while(szServiceName != NULL)
	{
		if (szServiceName[0] != NULL)
			if (TRUE !=	StartSingleService(szServiceName))
				bReturn = FALSE;

		szServiceName = _tcstok(NULL, _T(";"));
	}

	return ERROR_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//	RegisterAPDrivers									KEVIN MONG
//  
//  This function sets up all the property for the AP drivers: including the path to 
//  SAVRT.sys, SAVRTPEL.sys, NAVEX15.sys, and NAVENG.sys, so that a call to InstallSingleDriver() will setup 
//  all the necessary drivers.
//
//  Input: Reads from the MSI Property Table to get the drivers information.
//	Output:
//
/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) UINT __stdcall RegisterAPDrivers(MSIHANDLE hInstall)
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug RegisterAPDrivers()", "Debug Break", NULL);
#endif

	BOOL bReturn = TRUE;
	BOOL bAutoStart = FALSE;
	BOOL bRunNow = FALSE;
	TCHAR szBuffer[MAX_PATH] = {0};	
	DWORD dwSize = MAX_PATH;
	TCHAR szRunServices[MAX_PATH] = {0};

	// Get the path to latest defs directory.
	TCHAR szDefsTargetPath[MAX_PATH] = {0};	
	g_hDefsInstall = hInstall;
	
	MSIDefUtilsLoader DefUtilsLoader;

	// scoping the loader to make sure that the IDefUtilsPtr goes out of scope before the
	// the loader does
	{
		IDefUtilsPtr pDefUtils;
	
		if (SYM_FAILED(DefUtilsLoader.CreateObject(pDefUtils.m_p)) ||pDefUtils == NULL)
			return -1;
		pDefUtils->InitWindowsApp("NAV_NAVW32");	
		pDefUtils->UseNewestDefs(szDefsTargetPath, MAX_PATH);
	}
	
	// Get the path to the install directory.		
	TCHAR szAPSysPath[MAX_PATH] = {0};	
	MsiGetProperty(hInstall, _T("INSTALLDIR"), szAPSysPath, &dwSize);
	
	// Set the path for SAVRT.sys
	_tcscpy(szBuffer, szAPSysPath);
	_tcscat(szBuffer, SAVRT);	
	if(MsiSetProperty(hInstall, _T("BinaryName:1"), szBuffer) != ERROR_SUCCESS)
		bReturn = FALSE;

	// Set the path for NAVEX15.sys
	_tcscpy(szBuffer, szDefsTargetPath);
	_tcscat(szBuffer, NAVEX15);
	if(MsiSetProperty(hInstall, _T("BinaryName:2"), szBuffer) != ERROR_SUCCESS)
		bReturn = FALSE;

	// Set the path for NAVENG.sys
	_tcscpy(szBuffer, szDefsTargetPath);
	_tcscat(szBuffer, NAVENG);
	if(MsiSetProperty(hInstall, _T("BinaryName:3"), szBuffer) != ERROR_SUCCESS)
		bReturn = FALSE;

	// Set the path for SAVRTPEL.sys
	_tcscpy(szBuffer, szAPSysPath);
	_tcscat(szBuffer, SAVRTPEL);	
	if(MsiSetProperty(hInstall, _T("BinaryName:4"), szBuffer) != ERROR_SUCCESS)
		bReturn = FALSE;

	// Get the number of services we are going to register.
	TCHAR szNumOfService[MAX_PATH]= {0};
	int dwNumOfService = 0;
	MsiGetProperty(hInstall, NUMBER_OF_SERVICE, szNumOfService, &dwSize);		
	dwNumOfService = atoi(szNumOfService);


	for (int i = 1; i <= dwNumOfService; i++ )
	{
		
		// Get the Service Binary.
		dwSize = MAX_PATH;
		TCHAR szBinaryName[MAX_PATH] = {0};
		wsprintf(szBuffer, _T("BinaryName:%d"), i);
		MsiGetProperty(hInstall, szBuffer, szBinaryName, &dwSize);		
		
		// Get the service display name.
		dwSize = MAX_PATH;
		TCHAR szServiceDisplayName[MAX_PATH] = {0};
		wsprintf(szBuffer, _T("ServiceDisplayName:%d"), i);
		MsiGetProperty(hInstall, szBuffer, szServiceDisplayName, &dwSize);

		// Get the service Name.
		dwSize = MAX_PATH;
		TCHAR szServiceName[MAX_PATH] = {0};
		wsprintf(szBuffer, _T("ServiceName:%d"), i);
		MsiGetProperty(hInstall, szBuffer, szServiceName, &dwSize);		

		// Get the service type.	
		dwSize = MAX_PATH;
		TCHAR szServiceType[MAX_PATH] = {0};
		wsprintf(szBuffer, _T("ServiceType:%d"), i);
		MsiGetProperty(hInstall, szBuffer, szServiceType, &dwSize);	
		DWORD dwServiceType = atoi(szServiceType);	

		//Get info on if service is to run now or later.
		dwSize = MAX_PATH;
		TCHAR szRunNow[MAX_PATH] = {0};
		wsprintf(szBuffer, _T("RunNow:%d"), i);
		MsiGetProperty(hInstall, szBuffer, szRunNow, &dwSize);	
		// We build a list of drivers that need to be started during the install
		// and set an MSI property with that list at the end of this function.
		// The drivers must be started in a deferred action (StartAPDrivers)
		// after the files have been installed.
		if(_tcscmp(szRunNow, "TRUE") == 0)
		{
			_tcscat(szRunServices, szServiceName);
			_tcscat(szRunServices, _T(";"));
			bRunNow = TRUE;
		}
		
		// Check if the service is to start automatically or not 
		dwSize = MAX_PATH;	
		TCHAR szAutoStart[MAX_PATH] = {0};
		wsprintf(szBuffer, _T("AutoStart:%d"), i);
		MsiGetProperty(hInstall, szBuffer, szAutoStart, &dwSize);	
		if(_tcscmp(szAutoStart, "TRUE") == 0)
			bAutoStart = TRUE;
		
		// Get any dependancies
		dwSize = MAX_PATH;
		TCHAR szServiceDependancies[MAX_PATH] = {0};
		wsprintf(szBuffer, _T("ServiceDepends:%d"), i);
		if(ERROR_SUCCESS != MsiGetProperty(hInstall, szBuffer, szServiceDependancies, &dwSize))
			ZeroMemory(szServiceDependancies, sizeof(szServiceDependancies));

		// Call InstallSingleService to register the AP drivers.
		InstallToolBox::UnMarkServiceForDeletion(szServiceName);
		InstallSingleService(szBinaryName, szServiceDisplayName, szServiceName,
								dwServiceType, bAutoStart, szServiceDependancies);
	
	}

	// Here we set a property with the list of drivers that need to be
	// started during install such that the StartAPDrivers deferred
	// action can pick it up.
	MsiSetProperty(hInstall, _T("StartAPDrivers"), szRunServices);

	return ERROR_SUCCESS;
}
