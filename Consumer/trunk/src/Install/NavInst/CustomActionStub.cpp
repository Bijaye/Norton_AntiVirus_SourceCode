// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright 2006 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////


//CustomActionStub.cpp : 
//This file basically acts as a stub for calling custom actions when you are
//calling in to a file that is already installed on the disk.

#include "StdAfx.h"
#include <comdef.h>
#include "CustomAction.h"
#include "isLocations.h"

typedef UINT (__stdcall *FN_CustomAction)(MSIHANDLE);
#define CUSTOM_ACTION_DLL	_T("CLTVault.dll")
#define INSTALL_ALLDISTRIBUTION_FUNC _T("_InstallAllDistributions@4")
#define UNINSTALL_ALLDISTRIBUTION_FUNC _T("_UninstallAllDistributions@4")

using namespace InstallToolBox;

BOOL CallDistributionCustomAction(MSIHANDLE hInstall, LPCSTR szFunctionName)
{
	ccLib::CExceptionInfo exInfo;
	try
	{
		//Get the path to the NIS directory
		CRegKey rk;
		CString szSuiteOwnerPath;
		DWORD dwSize = 0;
		if(ERROR_SUCCESS == rk.Open(HKEY_LOCAL_MACHINE, isRegLocations::szInstalledAppsKey))
		{
			//Calculate the size of the string
			if((ERROR_SUCCESS != rk.QueryStringValue(isRegLocations::szInstalledAppsISShared, NULL, &dwSize)) || (dwSize > MAX_PATH*sizeof(TCHAR)))
			{
				CCustomAction::LogMessage(hInstall, CCustomAction::logError, _T("\"Norton AntiVirus\" value length is greater than MAX_PATH."));
				return TRUE;
			}
			//Read the path
			if(ERROR_SUCCESS != rk.QueryStringValue(isRegLocations::szInstalledAppsISShared, szSuiteOwnerPath.GetBuffer(dwSize + sizeof(TCHAR)), &dwSize))
			{
				CCustomAction::LogMessage(hInstall, CCustomAction::logError, _T("Failed to query \"Norton AntiVirus\"."));
				return TRUE;
			}
			szSuiteOwnerPath.ReleaseBuffer();
		}
		else
		{
			CCustomAction::LogMessage(hInstall, CCustomAction::logError, _T("Failed to open InstalledApps key"));
		}
		szSuiteOwnerPath.Append(_T("\\"));
		szSuiteOwnerPath.Append(CUSTOM_ACTION_DLL);

		//Load Library
		HANDLE hHandle = NULL;
		DWORD dwFlag = LOAD_WITH_ALTERED_SEARCH_PATH;
		HINSTANCE hSuiteOwner = LoadLibraryEx(szSuiteOwnerPath, hHandle, dwFlag);
		DWORD dwError = GetLastError();
		if(hSuiteOwner)
		{
			//Call Both the Exported functions.
			FN_CustomAction pfnCustomAction = (FN_CustomAction)GetProcAddress(hSuiteOwner, szFunctionName);
			if(pfnCustomAction)
			{
				//Call the Function
				if(ERROR_SUCCESS == pfnCustomAction(hInstall))
				{
					CCustomAction::LogMessage(hInstall, CCustomAction::logAlways, _T("Function Succeeded :%s"),szFunctionName);
				}
				else
				{
					CCustomAction::LogMessage(hInstall, CCustomAction::logError, _T("Function Failed :%s"),szFunctionName);
				}
			}
			else
			{
				dwError = GetLastError();
				CCustomAction::LogMessage(hInstall, CCustomAction::logError, _T("Function %s not Exported. GetLastError()=%d"),szFunctionName,dwError);
			}

			if(FreeLibrary(hSuiteOwner))
			{
				CCustomAction::LogMessage(hInstall, CCustomAction::logAlways, _T("FreeLibrary Succeeded"));
			}
			else
			{
				CCustomAction::LogMessage(hInstall, CCustomAction::logError, _T("FreeLibrary Not Succeeded"));
			}
		}
		else
		{
			dwError = GetLastError();
			CCustomAction::LogMessage(hInstall, CCustomAction::logError, _T("Failed to Load %s. GetLastError()=%d"),szSuiteOwnerPath,dwError);
		}
	}
	CCCATCHMEM(exInfo)
	CCCATCHCOM(exInfo)
	if(exInfo.IsException())
	{
		CCustomAction::LogMessage(hInstall, CCustomAction::logError, _T("An exception was thrown: %s "), exInfo.GetFullDescription());
	}
	CCustomAction::LogMessage(hInstall, CCustomAction::logAlways, _T("Returning"));	
	return TRUE;
}


extern "C" __declspec(dllexport) UINT __stdcall UninstallAllDistributions_Stub(MSIHANDLE hInstall)
{
	BOOL bRet = FALSE;
	bRet = CallDistributionCustomAction(hInstall,UNINSTALL_ALLDISTRIBUTION_FUNC);
	if(bRet)
		CCustomAction::LogMessage(hInstall, CCustomAction::logAlways, _T("CallDistributionCustomAction() SUCCEEDED"));
	else
		CCustomAction::LogMessage(hInstall, CCustomAction::logError, _T("CallDistributionCustomAction() FAILED"));
	return ERROR_SUCCESS;
}

extern "C" __declspec(dllexport) UINT __stdcall InstallAllDistributions_Stub(MSIHANDLE hInstall)
{
	BOOL bRet = FALSE;
	bRet = CallDistributionCustomAction(hInstall,INSTALL_ALLDISTRIBUTION_FUNC);
	if(bRet)
		CCustomAction::LogMessage(hInstall, CCustomAction::logAlways, _T("CallDistributionCustomAction() SUCCEEDED"));
	else
		CCustomAction::LogMessage(hInstall, CCustomAction::logError, _T("CallDistributionCustomAction() FAILED"));

	return ERROR_SUCCESS;
}