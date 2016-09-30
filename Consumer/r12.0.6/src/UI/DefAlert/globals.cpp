// globals.cpp
// Contains global variables to be used in DefAlertPlugin.cpp and SymNavDefAlert.cpp


#include "stdafx.h"
#define _INIT_COSVERSIONINFO
#include "SSOsinfo.h"
#include <InitGuid.h>
#define SIMON_INITGUID
#include <SIMON.h>

// PEP Headers
#define _PEP_IMPL_
#define _V2AUTHORIZATION_SIMON
#include "ComponentPepHelper.h"
#include "cltPEPV2Map.h"

#include "NavOptHelperEx.h"
#include "NavOptionRefreshHelperInterface.h"
#include "NAVSettingsHelperEx.h"
#include "SymAlertInterface.h"
#include "NAVError.h"
#include "ccModuleId.h"
#include "AVccModuleId.h"
#include "CfgwizNames.h"
#include "cltLicenseHelper.h"

const long	MAX_DEFS_AGE			= 14;			// 14 days
const DWORD	_5_MINUTES				= 300;			// in seconds
const DWORD	_6_HOURS				= 21600;		// in seconds
const DWORD _1_HOUR					= 3600;			// in seconds
const DWORD _1_DAY					= 86400;		// in seconds
const TCHAR SZ_NAV_SUBEXPIRED_ONSCAN_ALERT_MUTEX[]	= _T("NAV_SUBEXPIRED_ONSCAN_ALERT_MUTEX_{C8F07646-4179-4ca8-B118-012ED19810D2}");
const TCHAR SZ_NAV_OLDDEFS_ONSCAN_ALERT_MUTEX[]		= _T("NAV_OLDEFS_ONSCAN_ALERT_MUTEX_{661A9690-0BDA-4133-A1CA-AB8B6D8DEAC0}");

TCHAR g_szAlertFile[MAX_PATH]		= {0};
TCHAR g_szUserName[MAX_PATH]		= {0};

ALERT_MAP g_AlertMap[] =  
{
	SYMALERT_TYPE_OldDefs								,SYMALERT_CATEGORY_DEFINITIONS		,ALERT_LastDisplayOldDefs				,ALERT_DefsOutOfDateCycle	
	,SYMALERT_TYPE_OldDefsScan							,SYMALERT_CATEGORY_DEFINITIONS		,ALERT_ScanLastDisplayOldDefs			,ALERT_ScanDefsOutOfDateCycle
	,SYMALERT_TYPE_SubWarning							,SYMALERT_CATEGORY_SUBSCRIPTION		,ALERT_LastDisplaySubWarning			,NULL						
	,SYMALERT_TYPE_SubExpired							,SYMALERT_CATEGORY_SUBSCRIPTION		,ALERT_LastDisplaySubExpired			,ALERT_SubsExpiredCycle		
	,SYMALERT_TYPE_SubExpiredScan						,SYMALERT_CATEGORY_SUBSCRIPTION		,ALERT_ScanLastDisplaySubExpired		,ALERT_ScanSubsExpiredCycle	
	,SYMALERT_TYPE_SubWarningAutoRenew					,SYMALERT_CATEGORY_SUBSCRIPTION		,ALERT_LastDisplaySubWarningAutoRenew   ,NULL
	,SYMALERT_TYPE_SubExpiredAutoRenew					,SYMALERT_CATEGORY_SUBSCRIPTION		,ALERT_LastDisplaySubExpiredAutoRenew   ,NULL
	,SYMALERT_TYPE_SubExpiredAutoRenewScan				,SYMALERT_CATEGORY_SUBSCRIPTION		,NULL									,NULL
	,SYMALERT_TYPE_LicRetailPreActiv					,SYMALERT_CATEGORY_LICENSING		,ALERT_LastDisplayLicPreTrial			,NULL						
	,SYMALERT_TYPE_LicRetailPreActivExpired				,SYMALERT_CATEGORY_LICENSING		,ALERT_LastDisplayLicPreTrialExpired	,NULL						
	,SYMALERT_TYPE_LicTryBuyPreActiv					,SYMALERT_CATEGORY_LICENSING		,ALERT_LastDisplayLicPreTrial			,NULL						
	,SYMALERT_TYPE_LicTryBuyPreActivExpired				,SYMALERT_CATEGORY_LICENSING		,ALERT_LastDisplayLicPreTrialExpired	,NULL						
	,SYMALERT_TYPE_LicTryDiePreActiv					,SYMALERT_CATEGORY_LICENSING		,ALERT_LastDisplayLicPreTrial			,NULL						
	,SYMALERT_TYPE_LicTryDiePreActivExpired				,SYMALERT_CATEGORY_LICENSING		,ALERT_LastDisplayLicPreTrialExpired	,NULL						
	,SYMALERT_TYPE_LicRentalPreActivExpired				,SYMALERT_CATEGORY_LICENSING		,ALERT_LastDisplayLicPreExpired			,NULL						
	,SYMALERT_TYPE_LicRentalPreActivTrial				,SYMALERT_CATEGORY_LICENSING		,ALERT_LastDisplayLicPreTrial			,NULL						
	,SYMALERT_TYPE_LicRentalPreActivTrialExpired		,SYMALERT_CATEGORY_LICENSING		,ALERT_LastDisplayLicPreTrialExpired	,NULL						
	,SYMALERT_TYPE_LicRentalWarning						,SYMALERT_CATEGORY_LICENSING		,ALERT_LastDisplayLicWarning			,ALERT_LicenseWarningCycle	
	,SYMALERT_TYPE_LicRentalExpired						,SYMALERT_CATEGORY_LICENSING		,ALERT_LastDisplayLicExpired			,NULL	
	,SYMALERT_TYPE_LicRentalKilled						,SYMALERT_CATEGORY_LICENSING		,ALERT_LastDisplayLicKilled				,NULL						
	,SYMALERT_TYPE_LicViolated							,SYMALERT_CATEGORY_LICENSING		,ALERT_LastDisplayLicViolated			,NULL						
	,SYMALERT_TYPE_ALUDisabled							,SYMALERT_CATEGORY_ALUDISABLED		,NULL									,NULL					
	,SYMALERT_TYPE_LicRentalPreActivExpiredSS			,SYMALERT_CATEGORY_LICENSING		,ALERT_LastDisplayLicPreExpired			,NULL						
	,SYMALERT_TYPE_LicRentalPreActivGrace				,SYMALERT_CATEGORY_LICENSING		,ALERT_LastDisplayLicPreGrace			,NULL						
	,SYMALERT_TYPE_LicRentalPreActivGraceExpired		,SYMALERT_CATEGORY_LICENSING		,ALERT_LastDisplayLicPreGraceExpired	,NULL						
	,SYMALERT_TYPE_INVALID								,SYMALERT_CATEGORY_ERROR			,NULL									,NULL
};

bool _LookupAlertMap(SYMALERT_TYPE Type, ALERT_MAP& item)
{

    bool bResult = false;
    ZeroMemory(&item,sizeof(item));
    LPCTSTR lpcReturn = NULL;
    int nIndex = 0;
    while(g_AlertMap[nIndex].Type != SYMALERT_TYPE_INVALID)
    {
        if(g_AlertMap[nIndex].Type == Type)
        {
            item = g_AlertMap[nIndex];
            bResult = true;
            break;
        }
        ++nIndex;
    }
    return bResult;
}

LPCTSTR _LookupALERT_TimeDisplay(SYMALERT_TYPE Type)
{
    LPCTSTR lpcReturn = NULL;
    int nIndex = 0;
    while(g_AlertMap[nIndex].Type != SYMALERT_TYPE_INVALID)
    {
        if(g_AlertMap[nIndex].Type == Type)
        {
            lpcReturn = g_AlertMap[nIndex].lpszLastDisplay;
            break;
        }
        ++nIndex;
    }
    return lpcReturn;
}

/////////////////////////////////////////////////////////////////
//
// Function:	_GetAlertDataFile
//
// Description:	Return the path of alert.dat
//				(i.e. C:\Documents and Settings\All Users
//					\Application Data\Symantec\Norton AntiVirus)
//
/////////////////////////////////////////////////////////////////
HRESULT _GetAlertDataFile(void)
{
	HRESULT hr = S_OK;
	StahlSoft::HRX hrx;

	try
	{
		// Get the file path.
		TCHAR szPath[MAX_PATH] = {0};
		DWORD dwSize = MAX_PATH/sizeof(TCHAR);
		StahlSoft::CSmartModuleHandle shMod(::LoadLibrary("shfolder.dll"));
		hrx << (((HANDLE)shMod == (HANDLE)NULL) ? E_POINTER : S_OK);

		typedef HRESULT (WINAPI *pfnSHGetFolderPath)(HWND, int, HANDLE, DWORD, LPTSTR);
		pfnSHGetFolderPath pGetFolderPath = NULL;

		#ifdef _UNICODE
		pGetFolderPath = (pfnSHGetFolderPath) GetProcAddress(shMod, _T("SHGetFolderPathW"));
		#else
		pGetFolderPath = (pfnSHGetFolderPath) GetProcAddress(shMod,_T("SHGetFolderPathA"));
		#endif	

		hrx << (pGetFolderPath? S_OK : E_POINTER);

		if(FAILED(pGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, szPath)))
		{
			CRegKey key;	
			hrx << HRESULT_FROM_WIN32(key.Open(HKEY_LOCAL_MACHINE
										,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders")
										,KEY_READ|KEY_QUERY_VALUE));		
			hrx << HRESULT_FROM_WIN32(key.QueryStringValue(szPath, _T("Common AppData"), &dwSize));		
		}

		::wsprintf(g_szAlertFile, _T("%s\\Symantec\\Norton AntiVirus\\alert.dat"), szPath);		
	}
	catch(...)
	{
		hr = E_FAIL;
	}

	return hr;
}

/////////////////////////////////////////////////////////////////
//
// Function:	_GetUserName
//
/////////////////////////////////////////////////////////////////
HRESULT _GetUserName(void)
{
	HRESULT hr = S_OK;

	try
	{
		DWORD dwSize = MAX_PATH;
		if(::GetUserName(g_szUserName, &dwSize) == 0)
		{
			::_tcscpy(g_szUserName, _T("DefaultUser"));
		}
	}
	catch(...)
	{
		hr = E_FAIL;
	}

	return hr;
}

/////////////////////////////////////////////////////////////////
//
// Function:	_GetAlertDword
//
/////////////////////////////////////////////////////////////////
DWORD _GetAlertDword(LPCSTR lpszName, DWORD dwDefaultVal, BOOL bUserName)
{
	DWORD dwRetVal = dwDefaultVal;
	StahlSoft::HRX hrx;

	try
	{
		if(lpszName == NULL)
			return dwDefaultVal;

		// Setup key name
		char szName[33] = {0};

		if(bUserName)
		{
			// Check length of name. It must be less than 11 characters.
			// full_name (<=32 chars) = user_name (<=20 chars) + ":" +  value_name (<=11 chars)
			if(_tcslen(lpszName) > 11)
			{
				CCTRACEE(_T("%s length exceeds 11 characters!!!, return default value"), lpszName);
				return dwDefaultVal;
			}

			if(*g_szUserName == '\0')
				_GetUserName();

			wsprintf(szName, _T("%s:%s"), lpszName, g_szUserName);
		}
		else
		{
			_tcscpy(szName, lpszName);	
		}

		if(*g_szAlertFile == '\0')
			_GetAlertDataFile();

		CNAVOptFileEx alertFile;
		hrx << (alertFile.Init(g_szAlertFile, FALSE) ? S_OK : E_FAIL);
		alertFile.GetValue(szName, dwRetVal, dwDefaultVal);
	}
	catch(_com_error& e)
	{
		CCTRACEE(_T("Error while accessing alert.dat file 0x%08X"), e.Error());
	}

	return dwRetVal;
}

/////////////////////////////////////////////////////////////////
//
// Function:	_SetAlertDword
//
/////////////////////////////////////////////////////////////////
HRESULT _SetAlertDword(LPCSTR lpszName, DWORD dwVal, BOOL bUserName)
{
	StahlSoft::HRX hrx;
	HRESULT hr = S_OK;

	try
	{
		if(lpszName == NULL)
			return E_FAIL;


		// Setup key name
		char szName[33] = {0};

		if(bUserName)
		{
			// Check length of name. It must be less than 11 characters.
			// full_name (<=32 chars) = user_name (<=20 chars) + ":" +  value_name (<=11 chars)
			if(_tcslen(lpszName) > 11)
			{
				CCTRACEE(_T("%s length exceeds 11 characters!!!, return E_FAIL."), lpszName);
				return E_FAIL;
			}

			if(*g_szUserName == '\0')
				_GetUserName();

			wsprintf(szName, _T("%s:%s"), lpszName, g_szUserName);
		}
		else
		{
			_tcscpy(szName, lpszName);	
		}

		if(*g_szAlertFile == '\0')
			_GetAlertDataFile();

		CNAVOptFileEx alertFile;
		hrx << (alertFile.Init(g_szAlertFile, FALSE) ? S_OK : E_FAIL);
		hrx << alertFile.SetValue(szName, dwVal);
		hrx << (alertFile.Save()? S_OK : E_FAIL);
	}
	catch(_com_error& e)
	{
		hr  = e.Error();
		CCTRACEE(_T("Error while writing to alert.dat file 0x%08X"), hr);
	}

	return hr;
}

/////////////////////////////////////////////////////////////////
//
// Function:	SetCollectionAlertDword
//
// Description:	This function sets a given name to a given 
//				value for all users.
//				i.e. lpszName = "SECycle", dwValue = 3600
//				All names with "SECycle:xxx" will be set to 3600.
//
/////////////////////////////////////////////////////////////////
HRESULT _SetCollectionAlertDword(LPCSTR lpszName, DWORD dwVal)
{
	HRESULT hr = S_OK;
	StahlSoft::HRX hrx;
	CNAVOptFileEx alertFile;
	PNAVOPT32_VALUE_INFO pInfo = NULL;

	try
	{
		hrx << (lpszName? S_OK : E_FAIL);

		UINT uCount = 0;
		hrx << (alertFile.Init(g_szAlertFile, FALSE) ? S_OK : E_FAIL);
		hrx << alertFile.GetValuesInfo(&pInfo, &uCount);

		char szName[MAX_PATH] = {0};
		strcpy(szName, lpszName);
		strcat(szName, ":");

		for(int i=0; i<uCount; ++i)
		{
			LPTSTR lpstr = ::_tcsstr(pInfo[i].szValue, szName);
			if(lpstr == pInfo[i].szValue)
				alertFile.SetValue(pInfo[i].szValue, dwVal);
		}

		alertFile.Save();
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("_SetCollectionAlertDword - Exception while accessing alert.dat %08X"), hr);
	}

	try
	{
		// Free the list of names.
		if(pInfo)
			alertFile.FreeValuesInfo(pInfo);
	}
	catch(...)
	{
		hr = E_FAIL;
	}

	return hr;	
}

void _ErrorMessageBox(HWND hWnd, UINT uErrorID, UINT uNAVErrorResID)
{
	StahlSoft::HRX hrx;

	try
	{
		// Get a NAVError object
		CComBSTR bstrNAVErr(_T("NAVError.NAVCOMError"));
		CComPtr <INAVCOMError> spNavError;

		// Create, Populate, Log, and Display the error
		hrx << spNavError.CoCreateInstance(bstrNAVErr, NULL, CLSCTX_INPROC_SERVER);
		hrx << spNavError->put_ModuleID(AV_MODULE_ID_DEF_ALERT);		// Module ID of DefAlert
		hrx << spNavError->put_ErrorID(uErrorID); // Error ID is the resource ID for the error message
		hrx << spNavError->put_ErrorResourceID(uNAVErrorResID);
		hrx << spNavError->LogAndDisplay(reinterpret_cast<unsigned long>(hWnd));
	}
	catch(_com_error& e)
	{
		CCTRACEE(_T("_ErrorMessageBox - Exception while accessing NAVError object %08X"), e.Error());
	}
}

/////////////////////////////////////////////////////////////////
//
// Function:	_IsCfgWizRunning
//
// Description:	Return true if ANY cfgwiz is currently running;
//				otherwise, return false.
//
/////////////////////////////////////////////////////////////////
bool _IsCfgWizRunning()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		// If the event can be opened, CfgWiz is running.
		StahlSoft::CSmartHandle CfgWizEvent;
		CfgWizEvent = ::OpenEvent(EVENT_ALL_ACCESS, FALSE, CfgWiz::SZ_GLOBAL_RUNNING_EVENT);
		if((HANDLE)CfgWizEvent != (HANDLE) NULL)
			return true;
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)

	return false;
}

/////////////////////////////////////////////////////////////////
//
// Function:	_IsActWizRunning
//
// Description:	Return true if ACTWIZ is currently running;
//				otherwise, return false.
//				Note: Only one instance of ACTWIZ is allowed to run
//				at any time.
//
/////////////////////////////////////////////////////////////////
bool _IsActWizRunning()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		// If the mutex can be opened, Activation Wizard is running.
		StahlSoft::CSmartHandle shMutex;
		shMutex = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, _T("ActivationMutex_{90C80508-276B-4bcd-AAE9-8C31F5C47C1F}"));
		if((HANDLE)shMutex != (HANDLE)NULL)
			return true;
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)

	return false;
}
