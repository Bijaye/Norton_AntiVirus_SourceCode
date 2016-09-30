// PostCfgWizTasks.cpp: implementation of the PostCfgWizTasks class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "simon.h"
#include "ccVerifyTrustInterface.h"
#include "ccServicesConfigInterface.h"
#include "IWPPrivateSettingsInterface.h"
#include "ccSettingsInterface.h"
#include "IWPSettingsInterface.h"
#include "PostCfgWizTasks.h"
#include "Shellapi.h"
#include "LuErrors.h"
#include "SRRestorePtAPI.h"
#include "AVRES.h"
#include "OsInfo.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPostCfgWizTasks::CPostCfgWizTasks()
{
	
}

CPostCfgWizTasks::~CPostCfgWizTasks()
{

}

HRESULT CPostCfgWizTasks::RunTasks()
{
	// Check to see if there are any tasks

	HKEY hKey;

	long lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, g_cszNAVCfgWizRegKey, 0, 
		KEY_READ | KEY_WRITE, &hKey);

	if (lResult == ERROR_SUCCESS)
	{
		// Read in the tasks list
		
		TCHAR szTasksList[MAX_PATH] = {0};
		DWORD dwSize = sizeof(szTasksList);

		lResult = RegQueryValueEx(hKey, NULL, NULL, NULL, (LPBYTE) szTasksList, &dwSize);

		if (lResult == ERROR_SUCCESS)
		{
			// Make a temporary copy of the tasks list

			TCHAR szTempTaskList[MAX_PATH] = {0};
			TCHAR* pszTasksList = szTasksList;
			
			_tcscpy(szTempTaskList, szTasksList);

			// Retrieve the first task
			
			TCHAR* pszTaskName = NULL;
			
			for (pszTaskName = _tcstok(szTempTaskList, _T(";")); NULL != pszTaskName; pszTaskName = _tcstok(NULL, _T(";")))
			{
                // Pump messages in case there's a shutdown in progress. 
                //
                StahlSoft::WaitForMultipleObjectsWithMessageLoop ( NULL, 1 );

				if ( g_Shutdown.GetShutdown () )
					return E_FAIL;
                
                // Find the value of the task

				TCHAR szTaskValue[MAX_PATH] = {0};

				dwSize = sizeof(szTaskValue);

				lResult = RegQueryValueEx(hKey, pszTaskName, NULL, NULL, (LPBYTE) szTaskValue, &dwSize);

				if (lResult == ERROR_SUCCESS)
				{
					// Run the task
					
					STARTUPINFO rSI;
					PROCESS_INFORMATION rPI;
                    bool bWaitForTask = true;   // Wait for the task to finish?

					ZeroMemory(&rSI, sizeof(STARTUPINFO));
					rSI.cb = sizeof(STARTUPINFO);

		            BOOL bLaunched = CreateProcess(NULL, szTaskValue, NULL, NULL, FALSE, 
						NORMAL_PRIORITY_CLASS, NULL, NULL,&rSI, &rPI);

					if (bLaunched)
					{
                        CCTRACEI( "Task launched : %s", szTaskValue );

                        // Is this the last task? If it is we don't want to wait around.
                        // The last task is always launching the Main UI, which we don't
                        // want to wait for.
                        //
                        // CHEESY HACK ALERT : What we really need is a separate property
                        // for if we want to wait for the task or not. Since I have almost
                        // 0 time to implement this fix I'm going to cheese it here. Sorry.
                        // Defect # 378884. 
                        //
		        		if ( _tcsstr ( CharLower ( szTaskValue ), _T("nmain.exe")) > 0 )
				            bWaitForTask = false;
                        else
                            bWaitForTask = true;

                        // Remove the task from the tasks list

						if (pszTasksList != NULL)
							pszTasksList += (_tcslen(pszTaskName) + 1) * sizeof(TCHAR);

						dwSize = (_tcslen(pszTasksList) + 1) * sizeof(TCHAR);
						
						lResult = RegSetValueEx(hKey, NULL, NULL, REG_SZ, (BYTE*) pszTasksList, dwSize);

						// Remove the task value

						lResult = RegDeleteValue(hKey, pszTaskName);

						if ( bWaitForTask )
                        {
                            // Wait for the task to complete
                            //
                            StahlSoft::WaitForMultipleObjectsWithMessageLoop ( rPI.hProcess, INFINITE );

                            CCTRACEI ( "Task finished" );

							
							// If Luall is running, we need to take in consideration when luall
							// needs a reboot. If so, we will not execute the next post tasks.
		        			if ( _tcsstr ( CharLower ( szTaskValue ), _T("luall.exe")) > 0 )
							{
								DWORD dwExitCode = 0;
								
								// Checks LU exit code:
								// OAM_SUCCESSREBOOT - Updates require a reboot to complete installation.
								if (GetExitCodeProcess(rPI.hProcess, &dwExitCode))
								{
									if (dwExitCode == OAM_SUCCESSREBOOT)
									{
										return E_FAIL;
									}
								}

								CCTRACEI("LU exit code is: %d", dwExitCode);
							}
						    
                            // Wait for the any shut down messages. Why? Because we can't tell between
                            // an app that was closed by the user and one that was closed because we
                            // are restarting. Let's listen for the message.
                            //
                            StahlSoft::WaitForMultipleObjectsWithMessageLoop ( NULL, 2000 );
                        }
                        else
                            CCTRACEI ( "Task finished" );
					
						CloseHandle(rPI.hProcess);
						CloseHandle(rPI.hThread);

                        if ( g_Shutdown.GetShutdown () )
							return E_FAIL;
					}
				}
			}

			// Close the regkey handle

			RegCloseKey(hKey);

			// Delete the regkey since we are done with all the tasks

			lResult = RegDeleteKey(HKEY_LOCAL_MACHINE, g_cszNAVCfgWizRegKey);
		}
	}
	CreateSystemRestorePoint();
	return S_OK;
}

void	CPostCfgWizTasks::CreateSystemRestorePoint()
{
	RESTOREPOINTINFO RestPtInfo;
	STATEMGRSTATUS SMgrStatus;

	// Initialize the RESTOREPOINTINFO structure
	RestPtInfo.dwEventType = BEGIN_SYSTEM_CHANGE;

	// Notify the system that changes are about to be made.
	// An application is to be installed.
	RestPtInfo.dwRestorePtType = APPLICATION_INSTALL;

	// Set RestPtInfo.llSequenceNumber.
	RestPtInfo.llSequenceNumber = 0;

	ATL::CString sAVRES;
	GetModuleFileName(GetModuleHandle(NULL), sAVRES.GetBufferSetLength(MAX_PATH), MAX_PATH);
	sAVRES.ReleaseBuffer();
	PathRemoveFileSpec(sAVRES.GetBuffer());
	sAVRES.ReleaseBuffer();
	PathAddBackslash(sAVRES.GetBuffer());
	sAVRES.ReleaseBuffer();

	sAVRES += _T("AVRES.DLL");
	ATL::CString sProductName;
	StahlSoft::CSmartModuleHandle shMod(LoadLibraryEx(sAVRES, NULL, LOAD_LIBRARY_AS_DATAFILE) );
	if((HANDLE) shMod != (HANDLE)NULL)
	{
		sProductName.LoadString(shMod, IDS_AV_PRODUCT_NAME);
	}
	if(sProductName.IsEmpty())
	{
		sProductName = _T("Norton Antivirus");
	}
	sprintf(RestPtInfo.szDescription,"%s %s",sProductName,"post configuration restore point");


	COSInfo OSInfo;
	if(OSInfo.GetOSType() == COSInfo::OS_TYPE_WINXP)
	{
		HINSTANCE hLibrary = LoadLibrary ( _T("SrClient") );
		if ( hLibrary == NULL )
		{
			CCTRACEE ( "CreateSystemRestorePoint()..Failed to load SrClient.dll!" );
			return ;
		}

		static TCHAR szSRSetRestorePoint[] = _T("SRSetRestorePointA");

		typedef BOOL (__stdcall *fnSRSetRestorePoint)(PRESTOREPOINTINFOA, PSTATEMGRSTATUS);
		fnSRSetRestorePoint pfnSRSetRestorePoint = (fnSRSetRestorePoint) GetProcAddress(hLibrary, szSRSetRestorePoint);

		if( pfnSRSetRestorePoint )
		{
			// Notify the system that changes are to be made and that
			// the beginning of the restore point should be marked. 
			if(!pfnSRSetRestorePoint(&RestPtInfo, &SMgrStatus)) 
			{
				CCTRACEE ( "CreateSystemRestorePoint()..Couldn't set the beginning of the restore point!" );
				return;
			}

			// Re-initialize the RESTOREPOINTINFO structure to notify the 
			// system that the operation is finished.
			RestPtInfo.dwEventType = END_SYSTEM_CHANGE;

			// End the system change by returning the sequence number 
			// received from the first call to SRSetRestorePoint.
			RestPtInfo.llSequenceNumber = SMgrStatus.llSequenceNumber;

			// Notify the system that the operation is done and that this
			// is the end of the restore point.
			if(!pfnSRSetRestorePoint(&RestPtInfo, &SMgrStatus)) 
			{
				CCTRACEE ( "CreateSystemRestorePoint()..Couldn't set the end of the restore point!" );
			}
		}
		else
		{
			CCTRACEE ( "CreateSystemRestorePoint()..Failed to get pfnSRSetRestorePoint" );
		}
	}
	else
	{
		CCTRACEE ( "CreateSystemRestorePoint()..System Restore not Supported on this OS!" );
		return;		// System Restore not supported on any other OS ( Except ME & XP)
	}
}