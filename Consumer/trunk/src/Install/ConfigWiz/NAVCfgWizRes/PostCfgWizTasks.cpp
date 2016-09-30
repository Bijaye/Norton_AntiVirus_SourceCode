////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// PostCfgWizTasks.cpp: implementation of the PostCfgWizTasks class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "simon.h"
#include "ccVerifyTrustInterface.h"
#include "ccServicesConfigInterface.h"
#include "ccSettingsInterface.h"
#include "PostCfgWizTasks.h"
#include "Shellapi.h"
#include "LuErrors.h"
#include "SRRestorePtAPI.h"
#include "ISVersion.h"
#include "OsInfo.h"
#include "AVDefines.h"
#include "OptNames.h"
#include "AVInterfaces.h"
#include "AVInterfaceLoader.h"
#include "IMConfigInterface.h"
#include "IMConfigLoader.h"
#include "resource.h"
#include "ccosinfo.h"
#define SZ_OEHEUR_DLL                          _T("OEHeur.dll")
#define SZ_OEHEUR_GUID						   _T("{4AA748DA-9D8A-4271-B507-DB5616B93844}")
#define _MIN_NV_CATEGORY 4
#define _MAX_NV_CATEGORY 11

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
	HRESULT hr;

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
                        CCTRCTXI1 (_T( "Task launched : %s"), szTaskValue );

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

                            CCTRCTXI0 (_T( "Task finished" ));

							
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

								CCTRCTXI1 (_T("LU exit code is: %d"), dwExitCode);
							}
						    
                            // Wait for the any shut down messages. Why? Because we can't tell between
                            // an app that was closed by the user and one that was closed because we
                            // are restarting. Let's listen for the message.
                            //
                            StahlSoft::WaitForMultipleObjectsWithMessageLoop ( NULL, 2000 );
                        }
                        else
                            CCTRCTXI0 (_T( "Task finished" ));
					
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

	wsprintf(RestPtInfo.szDescription,_T("%s %s"),CISVersion::GetProductName(),_T("post configuration restore point"));


	
	if (ccLib::COSInfo::IsWinVista(true) ||ccLib::COSInfo::IsWinXP(true))
	{
		StahlSoft::CSmartModuleHandle hLibrary;
		hLibrary = LoadLibraryEx ( _T("SrClient"), NULL, NULL );
		if ( hLibrary == NULL )
		{
			CCTRCTXE0 (_T( "CreateSystemRestorePoint()..Failed to load SrClient.dll!" ));
			return ;
		}

		static char szSRSetRestorePoint[] = "SRSetRestorePointW";

		typedef BOOL (__stdcall *fnSRSetRestorePoint)(PRESTOREPOINTINFO, PSTATEMGRSTATUS);
		fnSRSetRestorePoint pfnSRSetRestorePoint = (fnSRSetRestorePoint) GetProcAddress(hLibrary, szSRSetRestorePoint);

		if( pfnSRSetRestorePoint )
		{
			// Notify the system that changes are to be made and that
			// the beginning of the restore point should be marked. 
			if(!pfnSRSetRestorePoint(&RestPtInfo, &SMgrStatus)) 
			{
				CCTRCTXE0 (_T( "CreateSystemRestorePoint()..Couldn't set the beginning of the restore point!" ));
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
				CCTRCTXE0 (_T( "CreateSystemRestorePoint()..Couldn't set the end of the restore point!" ));
			}
		}
		else
		{
			CCTRCTXE0 (_T( "CreateSystemRestorePoint()..Failed to get pfnSRSetRestorePoint" ));
		}
	}
	else
	{
		CCTRCTXE0 (_T( "CreateSystemRestorePoint()..System Restore not Supported on this OS!" ));
		return;		// System Restore not supported on any other OS ( Except ME & XP)
	}
}

// This function retrieves the navopts.dat system defaults for AVComponent
// options and propagates them to the appropriate component.
// Not using options for performance reasons.
HRESULT CPostCfgWizTasks::ConfigureAVComponentDefaults()
{
	HRESULT hr;
	CNAVOptSettingsCache navOptDefs;

	if(!navOptDefs.Init(_T("NAVOPTS.DEF"), FALSE))	// Connect to defaults
	{
		CCTRCTXE0(_T("Failed to initialize navopts.def cache object."));
		return E_ACCESSDENIED;
	}

    // AutoProtect
    if(FAILED(hr = this->ConfigureAPDefaults(&navOptDefs)))
    {
        CCTRCTXE1(_T("Error configuring AP Defaults. HR = 0x%08x"), hr);
        return hr;
    }

    // Email Scanning
    if(FAILED(hr = this->ConfigureEmailDefaults(&navOptDefs)))
    {
        CCTRCTXE1(_T("Error configuring Email defaults. HR = 0x%08x"), hr);
        return hr;
    }

    // IM Scanning
    if(FAILED(hr = this->ConfigureIMDefaults(&navOptDefs)))
    {
        CCTRCTXE1(_T("Error configuring IM Defaults. HR = 0x%08x"), hr);
        return hr;
    }

    // Threat Categories
    if(FAILED(hr = this->ConfigureThreatCatDefaults(&navOptDefs)))
    {
        CCTRCTXE1(_T("Error configuring Threat Cat Defaults. Hr = 0x%08x"), hr);
        return hr;
    }

	return S_OK;
}

// PRECONDITION: Initialized CNAVOptSettingsCache.
// POSTCONDITION: AP Defaults properly propagated from NAVOPTS.DEF to AP.
HRESULT CPostCfgWizTasks::ConfigureAPDefaults(CNAVOptSettingsCache* pDefaults)
{
	HRESULT hr;
	DWORD dwVal;
	AVModule::IAVMapStrDataPtr APOptsMap;
	AVModule::IAVAutoProtectPtr AutoProtectPtr;
	AVModule::AVLoader_IAVAutoProtect apLoader;

	if(!pDefaults)
	{
		CCTRCTXE0(_T("Invalid Defaults pointer."));
		return E_INVALIDARG;
	}

	if(SYM_FAILED(apLoader.CreateObject(AutoProtectPtr)))
	{
		CCTRCTXE0(_T("Failed to create IAVAutoProtectPtr."));
		return E_POINTER;
	}

	if(FAILED(hr = AutoProtectPtr->GetOptions(APOptsMap)))
	{
		CCTRCTXE1(_T("Error loading AP Options Map. Hr = 0x%08x"), hr);
		return hr;
	}

	if(FAILED(hr = pDefaults->GetValue(AUTOPROTECT_CheckFloppyOnMount, dwVal, 1)))
	{
		CCTRCTXE1(_T("Error getting AP:CheckRemovableOnMount default. HR = 0x%08x"), hr);
		return hr;
	}
	else
	{
		if(FAILED(hr = APOptsMap->SetValue(AVModule::AutoProtect::APOPTION_SCAN_REMOVABLE_BRSMBRS_ON_MOUNT, dwVal)))
		{
			CCTRCTXE1(_T("Error setting CROM default into map. HR = 0x%08x"), hr);
			return hr;
		}
	}

	if(FAILED(hr = pDefaults->GetValue(AUTOPROTECT_DriversSystemStart, dwVal, 1)))
	{
		CCTRCTXE1(_T("Error getting AP:DelayLoad default. HR = 0x%08x"), hr);
		return hr;
	}
	else
	{
		if(FAILED(hr = APOptsMap->SetValue(AVModule::AutoProtect::APOPTION_SYSTEMSTART_DRIVERS, dwVal)))
		{
			CCTRCTXE1(_T("Error setting SystemStart default into map. HR = 0x%08x"), hr);
			return hr;
		}
	}

	if(FAILED(hr = pDefaults->GetValue(AUTOPROTECT_EnableHeuristicScan, dwVal, 1)))
	{
		CCTRCTXE1(_T("Error getting AP:EnableHeuristicScan default. HR = 0x%08x"), hr);
		return hr;
	}
	else
	{
		if(FAILED(hr = APOptsMap->SetValue(AVModule::AutoProtect::APOPTION_ENABLE_HEURISTICS, dwVal)))
		{
			CCTRCTXE1(_T("Error setting Heuristic Scanning default into map. HR = 0x%08x"), hr);
			return hr;
		}
	}

	// Commit map
	if(FAILED(hr = AutoProtectPtr->SetOptions(APOptsMap)))
	{
		CCTRCTXE1(_T("Error committing AP Options map. HR = 0x%08x"), hr);
		return hr;
	}

	// Always enable AP post cfgwiz
	if(FAILED(hr = AutoProtectPtr->EnableAP(true)))
	{
		CCTRCTXE1(_T("CPostCfgWizTasks::ConfigureAPDefaults() - Error Enabling/Disabling AutoProtect. HR = 0x%08x"), hr);
		return hr;
	}


	// Prevent loader/releasing issues.
    APOptsMap.Release();
	AutoProtectPtr.Release();

	return S_OK;
}

// PRECONDITION: Initialized CNAVOptSettingsCache.
// POSTCONDITION: Defaults properly propagated from NAVOPTS.DEF to AVEMAIL.
HRESULT CPostCfgWizTasks::ConfigureEmailDefaults(CNAVOptSettingsCache* pDefaults)
{
	HRESULT hr;
	DWORD dwVal;
	TCHAR szOEHDllPath[512];
	size_t nOEHDllPathSize = 512;
	AVModule::AVLoader_IAVEmail AVEmailLoader;
	AVModule::IAVEmailPtr spAVEmail;

	if(!pDefaults)
	{
		CCTRCTXE0(_T("Invalid Defaults pointer."));
		return E_INVALIDARG;
	}

	if(SYM_FAILED(AVEmailLoader.CreateObject(spAVEmail)))
	{
		CCTRCTXE0(_T("Error creating AV Email object."));
		return E_POINTER;
	}

	ccSym::CNAVPathProvider::GetPath(szOEHDllPath, nOEHDllPathSize);
	PathAppend(szOEHDllPath, SZ_OEHEUR_DLL);

	// These we need to set, always.
	if(FAILED(hr = spAVEmail->SetOption(AVModule::IAVEmail::eAVEmailOpt_OEHDLLPath, szOEHDllPath)))
	{
		CCTRCTXE1(_T("Error setting IAVEmail OEH DLL Path. HR = 0x%08x"), hr);
		return hr;
	}

	if(FAILED(hr = spAVEmail->SetOption(AVModule::IAVEmail::eAVEmailOpt_OEHSymObjectID, SZ_OEHEUR_GUID)))
	{
		CCTRCTXE1(_T("Error setting IAVEmail OEH SymObjectID. HR = 0x%08x"), hr);
		return hr;
	}

	// Warning! Synchronous calls, errors here will cause badness down the road.
	if(FAILED(hr = pDefaults->GetValue(AVEMAIL_ScanIncoming, dwVal, 1)))
	{
		CCTRCTXE1(_T("Error getting default value for AVEMAIL:ScanIncoming. Hr = 0x%08x"), hr);
		return hr;
	}
	else if(FAILED(hr = spAVEmail->SetOption(AVModule::IAVEmail::eAVEmailOpt_ScanIncoming, dwVal)))
	{
		CCTRCTXE1(_T("Error setting IAVEmail Option: Scan Incoming. Hr = 0x%08x"), hr);
		return hr;
	}

	if(FAILED(hr = pDefaults->GetValue(AVEMAIL_ScanOutgoing, dwVal, 1)))
	{
		CCTRCTXE1(_T("Error getting default value for AVEMAIL:ScanOutgoing. Hr = 0x%08x"), hr);
		return hr;
	}
	else if(FAILED(hr = spAVEmail->SetOption(AVModule::IAVEmail::eAVEmailOpt_ScanOutgoing, dwVal)))
	{
		CCTRCTXE1(_T("Error setting IAVEmail Option: Scan Outgoing. Hr = 0x%08x"), hr);
		return hr;
	}

	if(FAILED(hr = pDefaults->GetValue(AVEMAIL_OEH, dwVal, 1)))
	{
		CCTRCTXE1(_T("Error getting default value for AVEMAIL:OEH. Hr = 0x%08x"), hr);
		return hr;
	}
	else if(FAILED(hr = spAVEmail->SetOption(AVModule::IAVEmail::eAVEmailOpt_UseOEH, dwVal)))
	{
		CCTRCTXE1(_T("Error setting IAVEmail Option: Use OEH. Hr = 0x%08x"), hr);
		return hr;
	}

    CString cszReplacementFileNameFmt, cszReplacementFileName;
    cszReplacementFileNameFmt.LoadString(IDS_EMAIL_MIME_REPLACEMENT_FILENAME);
    cszReplacementFileName.Format(cszReplacementFileNameFmt, CISVersion::GetProductName());
    if(FAILED(hr = spAVEmail->SetOption(AVModule::IAVEmail::eAVEmailOpt_MimeReplaceFileName, cszReplacementFileName)))
    {
        CCTRCTXE1(_T("Error setting IAVEmail Option: Mime Replacement Filename. Hr = 0x%08x"), hr);
        return hr;
    }

    if(FAILED(hr = spAVEmail->SetOption(AVModule::IAVEmail::eAVEmailOpt_MimeReplaceProductName, CISVersion::GetProductName())))
    {
        CCTRCTXE1(_T("Error setting IAVEmail Option: Mime Replacement Product Name. Hr = 0x%08x"), hr);
        return hr;
    }

	// Prevent premature loader unloading issues.
	spAVEmail.Release();

	return S_OK;
}

HRESULT CPostCfgWizTasks::ConfigureIMDefaults(CNAVOptSettingsCache* pDefaults)
{
	ISShared::IIMConfig::IMSTATUS imStatus;
	ISShared::IIMConfigPtr spIMConfig;
	ISShared::ISShared_IMLoader imLoader;
	DWORD dwVal;

	if(!pDefaults)
	{
		CCTRCTXE0(_T("Invalid Defaults pointer."));
		return E_INVALIDARG;
	}

	if(SYM_FAILED(imLoader.CreateObject(GETMODULEMGR(), spIMConfig)))
	{
		CCTRCTXE0(_T("Error creating IMConfigInterface object. Symresult = 0x%08x"));
		return E_FAIL;
	}

	if((imStatus = spIMConfig->SetDefaults()) == ISShared::IIMConfig::IMSTATUS_ERROR)
	{
		CCTRCTXE1(_T("Error setting IM Configuration Defaults. IMConfig = [%d]"), (unsigned long)imStatus);
		return E_FAIL;
	}

	if(FAILED(pDefaults->GetValue(IMSCAN_ScanMIM, dwVal, 1)))
	{
		CCTRACEE(_T("Error setting MSN IM Configuration Defaults."));
		return E_FAIL;
	}
	else if(dwVal == 1)
	{
		if((imStatus = spIMConfig->ConfigureIM(ISShared::IIMConfig::IMTYPE_MSN)) == ISShared::IIMConfig::IMSTATUS_ERROR)
		{
			CCTRACEE(_T("Error configuring MSN IM Option."));
			return E_FAIL;
		}
	}
	else
	{
		if((imStatus = spIMConfig->UnconfigureIM(ISShared::IIMConfig::IMTYPE_MSN)) == ISShared::IIMConfig::IMSTATUS_ERROR)
		{
			CCTRACEE(_T("Error configuring MSN IM Option."));
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CPostCfgWizTasks::ConfigureThreatCatDefaults(CNAVOptSettingsCache* pDefaults)
{
	HRESULT hr;
	DWORD dwVal;
	CStringW optDefName;
	AVExclusions::INonViralCategoryExclusionsPtr spTCExclu;
	AVModule::AVLoader_IAVNVCatExclusions avTCExcluLoader;

	if(!pDefaults)
	{
		CCTRCTXE0(_T("Invalid Defaults pointer."));
		return E_INVALIDARG;
	}

	if(SYM_FAILED(avTCExcluLoader.CreateObject(spTCExclu)))
	{
		CCTRCTXE0(_T("Error creating AVModule ThreatCat exclusions object."));
		return E_FAIL;
	}

	// Pull defaults from ccSettings (navopts.def key).
	for(unsigned long l = _MIN_NV_CATEGORY; l <= _MAX_NV_CATEGORY; l++)
	{
		optDefName = _T("THREAT:Threat%d");
		optDefName.Format(optDefName, l);

		if(FAILED(hr = pDefaults->GetValue(optDefName, dwVal, 1)))
		{
			CCTRCTXW2(_T("Error getting default value for Threat Category %d, hr = 0x%08x"), l, hr);
			continue;
		}

		if(dwVal)
		{
			if(FAILED(hr = spTCExclu->IncludeCategory(l)))
			{
				CCTRCTXW2(_T("Error including threat category %d, hr = 0x%08x"), l, hr);
			}
		}
		else
		{
			if(FAILED(hr = spTCExclu->ExcludeCategory(l)))
			{
				CCTRCTXW2(_T("Error excluding threat category %d, hr = 0x%08x"), l, hr);
			}
		}
	}

	// Prevent premature loader unloading.
	spTCExclu.Release();

	return S_OK;
}