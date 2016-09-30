// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005,2006 Symantec Corporation. All rights reserved.
// OEMSettingsManager.cpp : Implementation of COEMSettingsManager
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "OEMSettingsManager.h"
#include "vpstrutils.h"
#include "SymSaferRegistry.h"
#include "oem.h"
#include "slic.h"
#include "OEMSettings.h"

COEMSettings OEMSettings;

COEMSettings::COEMSettings()
{
	LogLine(_T("COEMSettingsManager Constructor"));
	m_hOEM = NULL;

	m_fpIsOEM_CustomerBoot = NULL;
	m_fpSetOEM_CustomerBoot = NULL;
	m_fpIsOEM_EULAAccepted = NULL;
	m_fpSetOEM_EULAAccepted = NULL;
	m_fpIsOEM_InitDialogsDone = NULL;
	m_fpSetOEM_InitDialogsDone = NULL;
	m_fpIsOEM_Initialized = NULL;
	m_fpIsOEM_DoEnableAP = NULL;
	m_fpSetOEM_DoEnableAP = NULL;
	m_fpIsOEM_ShowVPTray = NULL;
	m_fpLaunchConfigWiz = NULL;
	m_fpOEM_DisableDriversServices = NULL;
	m_fpReadTimeFromRegistry = NULL;
	m_fpWriteTimeToRegistry = NULL;
	m_fpIsOEM_NoTriggerEULA = NULL;
	m_fpSetOEM_NoTriggerEULA = NULL;
	m_fpGetOEM_RegisterInfo = NULL;
	m_fpSetOEM_RegisterInfo = NULL;
	m_fpEncryptConfigSettings = NULL;
	m_fpGetOEM_LicenseType = NULL;
	m_fpSetOEM_LicenseType = NULL;
	m_fpGetOEM_EULAPath = NULL;
	m_fpCheckAlertDays = NULL;
	m_fpSetOEM_ConfigFileLocation = NULL;
	m_fpGetOEM_ConfigFileLocation = NULL;
	m_fpCheckOEM_BootCount = NULL;
	m_fpDecrementOEM_BootCount = NULL;
	m_fpGetOEM_BootCount = NULL;
	m_fpSetOEM_BootCount = NULL;
	m_fpSetOEM_ShowVPTray = NULL;
	m_fpGetOEM_Url = NULL;
	m_fpIsOEM_RealELSMode = NULL;
	m_fpSetOEM_RealELSMode = NULL;
	m_fpIsOEM_RealSubscriptionMode = NULL;
	m_fpSetOEM_RealSubscriptionMode = NULL;
	m_fpIsOEM_Install = NULL;
	m_fpSetOEM_Install = NULL;
	m_fpIsOEM_TrialApp = NULL;
	m_fpSetOEM_TrialApp = NULL;
	m_fpGetOEM_TrialLen = NULL;
	m_fpSetOEM_TrialLen = NULL;
	m_fpGetOEM_NagDialogText = NULL;
	m_fpGetOEM_ExpireDialogText = NULL;
	m_fpCreateOEM_Lock = NULL;
	m_fpCreateOEM_Lock = NULL;
	m_fpSetTrialLenInSessionFile = NULL;
	m_fpLoadConfigIniToRegistry = NULL;	
	m_fpIsOEM_NoUrl = NULL;
	m_fpSetOEM_NoUrl = NULL;
	m_fpGetOEM_AdminText = NULL;
	m_fpSetOEM_AdminText = NULL;
	m_fpDoesOEM_UserModeRegKeyExist = NULL;
	m_fpGetOEM_UserModeRegKey = NULL;
	m_fpSetOEM_UserModeRegKey = NULL;
}

COEMSettings::~COEMSettings()
{
	if (m_hOEM)
	{
		FreeLibrary(m_hOEM);
		m_hOEM = NULL;
	}
}

STDMETHODIMP COEMSettingsManager::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IOEMSettingsManager
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

BOOL COEMSettings::GetOEM_Dir(LPTSTR szDirBuf, DWORD dwMaxLen)
{
	HKEY hKey;
	BOOL bRet = FALSE;
	
	if ( ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		SYMANTEC_ABOVE_OEM_KEY, 0, KEY_READ, &hKey) )
	{
		TCHAR szSharedDir[MAX_PATH] = _T("");
		DWORD dwType = REG_SZ, dwDataLen = dwMaxLen;
		if (ERROR_SUCCESS == SymSaferRegQueryValueEx(hKey, 
			SYMANTEC_SHARED_KEY, NULL, 
			&dwType, (LPBYTE)szSharedDir, &dwDataLen))
		{
			sprintf(szDirBuf, _T("%s\\OEM"), szSharedDir);
			//vpvsnprintf(szDirBuf, dwMaxLen, _T("%s\\OEM"), szSharedDir);
			//::MessageBox(NULL, szSharedDir, "Info", MB_OK);
			bRet = TRUE;
		}
		else
		{
			DWORD dwError = GetLastError();
			TCHAR szError[32] = _T("");
			sprintf(szError, _T("Error: %d"), dwError);
			::MessageBox(NULL, szError, "Info", MB_OK);
		}
		RegCloseKey(hKey);
	}
	
	return bRet;
}
HMODULE COEMSettings::LoadOEMLibrary()
{
	if ( m_hOEM != NULL )
		return m_hOEM;

	TCHAR szOEMDll[1024] = _T("");
	DWORD dwMaxLen = sizeof(szOEMDll);
	if (GetOEM_Dir(szOEMDll, dwMaxLen))
	{
		vpstrnappendfile(szOEMDll, OEMDLL, dwMaxLen);
		return LoadLibrary(szOEMDll);
	}
	else
		return NULL;
}

STDMETHODIMP COEMSettingsManager::get_CustomerBoot(VARIANT_BOOL* pVal)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

	if (pVal == NULL)
		return E_POINTER;

	*pVal = (OEMSettings.IsOEM_CustomerBoot() == TRUE) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

BOOL COEMSettings::IsOEM_CustomerBoot()
{
	BOOL bRet = TRUE;

	m_hOEM = LoadOEMLibrary();
	if (m_hOEM && !m_fpIsOEM_CustomerBoot)
		m_fpIsOEM_CustomerBoot = (lpfnIsOEM_CustomerBoot) GetProcAddress(m_hOEM, _T("IsOEM_CustomerBoot"));
	if (m_fpIsOEM_CustomerBoot != NULL)
	{
		LogLine(_T("CustomerBoot: Calling OEM_CustomerBoot"));
		bRet = m_fpIsOEM_CustomerBoot();
	}
	else LogLine(_T("CustomerBoot: Failed to load DLL"));

	LogLine(_T("CustomerBoot: %d"), bRet);
	return bRet;
}

STDMETHODIMP COEMSettingsManager::get_EULAAccepted(VARIANT_BOOL* pVal)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
	if (pVal == NULL)
		return E_POINTER;

	*pVal = (OEMSettings.IsOEM_EULAAccepted()) ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

BOOL COEMSettings::IsOEM_EULAAccepted()
{
	BOOL bRet = FALSE;
	m_hOEM = LoadOEMLibrary();
	if (m_hOEM && !m_fpIsOEM_EULAAccepted)
		m_fpIsOEM_EULAAccepted = (lpfnIsOEM_EULAAccepted) GetProcAddress(m_hOEM, _T("IsOEM_EULAAccepted"));
	
	if (m_fpIsOEM_EULAAccepted != NULL) 
	{
		bRet = (m_fpIsOEM_EULAAccepted());
	}
	else bRet = FALSE;

	LogLine(_T("EULA Accepted: %d"), bRet);
	return bRet;
}

STDMETHODIMP COEMSettingsManager::put_EULAAccepted(VARIANT_BOOL newVal)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

	OEMSettings.SetOEM_EULAAccepted((newVal == VARIANT_TRUE) ? TRUE : FALSE);
    return S_OK;
}
void COEMSettings::SetOEM_EULAAccepted(BOOL bAccepted)
{
	m_hOEM = LoadOEMLibrary();

	if (m_hOEM && !m_fpSetOEM_EULAAccepted)
		m_fpSetOEM_EULAAccepted = (lpfnSetOEM_EULAAccepted) GetProcAddress(m_hOEM, _T("SetOEM_EULAAccepted"));
	if (m_fpSetOEM_EULAAccepted)
		m_fpSetOEM_EULAAccepted(bAccepted);
}

STDMETHODIMP COEMSettingsManager::get_InitDialogsDone(VARIANT_BOOL* pVal)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
	if (pVal == NULL)
		return E_POINTER;

	*pVal = (OEMSettings.IsOEM_InitDialogsDone() == TRUE) ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;


}
BOOL COEMSettings::IsOEM_InitDialogsDone()
{
	BOOL bRet = FALSE;
	m_hOEM = LoadOEMLibrary();

	if (m_hOEM && !m_fpIsOEM_InitDialogsDone)
		m_fpIsOEM_InitDialogsDone = (lpfnIsOEM_InitDialogsDone) GetProcAddress(m_hOEM, _T("IsOEM_InitDialogsDone"));
	if (m_fpIsOEM_InitDialogsDone != NULL) 
	{
		bRet = m_fpIsOEM_InitDialogsDone();
	}
	else bRet = FALSE;

	LogLine(_T("InitDialogsDone: %d"), bRet);
	return bRet;
}

STDMETHODIMP COEMSettingsManager::put_InitDialogsDone(VARIANT_BOOL newVal)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

	OEMSettings.SetOEM_InitDialogsDone((newVal == VARIANT_TRUE) ? TRUE : FALSE);
	return S_OK;

}

void COEMSettings::SetOEM_InitDialogsDone(BOOL bVal)
{
	m_hOEM = LoadOEMLibrary();

	if (m_hOEM && !m_fpSetOEM_InitDialogsDone)
		m_fpSetOEM_InitDialogsDone = (lpfnSetOEM_InitDialogsDone) GetProcAddress(m_hOEM, _T("SetOEM_InitDialogsDone"));
	if (m_fpSetOEM_InitDialogsDone)
		m_fpSetOEM_InitDialogsDone(bVal);
}

STDMETHODIMP COEMSettingsManager::get_OEMInitialized(VARIANT_BOOL* pVal)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
	if (pVal == NULL)
		return E_POINTER;

	*pVal = (OEMSettings.IsOEM_Initialized() == TRUE) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;

}

BOOL COEMSettings::IsOEM_Initialized()
{
	BOOL bRet = FALSE;
	m_hOEM = LoadOEMLibrary();

	if (m_hOEM && !m_fpIsOEM_Initialized)
		m_fpIsOEM_Initialized = (lpfnIsOEM_Initialized) GetProcAddress(m_hOEM, _T("IsOEM_Initialized"));
	if (m_fpIsOEM_Initialized != NULL) 
	{
		bRet = m_fpIsOEM_Initialized();
	}
	else bRet = FALSE;;	

	LogLine(_T("OEM Initialized: %d"), bRet);
	return bRet;
}

STDMETHODIMP COEMSettingsManager::get_EnableAP(VARIANT_BOOL* pVal)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
	if (pVal == NULL)
		return E_POINTER;

	*pVal = (OEMSettings.IsOEM_DoEnableAP() == TRUE) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

BOOL COEMSettings::IsOEM_DoEnableAP()
{
	BOOL bRet = TRUE;

	m_hOEM = LoadOEMLibrary();

	if (m_hOEM && !m_fpIsOEM_DoEnableAP)
		m_fpIsOEM_DoEnableAP = (lpfnIsOEM_DoEnableAP) GetProcAddress(m_hOEM, _T("IsOEM_DoEnableAP"));
	if (m_fpIsOEM_DoEnableAP != NULL)
	{
		bRet = m_fpIsOEM_DoEnableAP();
	} 
	else bRet = TRUE;

	LogLine(_T("Enable AP: %d"), bRet);
	return bRet;
}

STDMETHODIMP COEMSettingsManager::put_EnableAP(VARIANT_BOOL newVal)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

	OEMSettings.SetOEM_DoEnableAP((newVal == VARIANT_TRUE) ? TRUE : FALSE);
	return S_OK;
}

void COEMSettings::SetOEM_DoEnableAP(BOOL bVal)
{
	m_hOEM = LoadOEMLibrary();

	if (m_hOEM && !m_fpSetOEM_DoEnableAP)
		m_fpSetOEM_DoEnableAP = (lpfnSetOEM_DoEnableAP) GetProcAddress(m_hOEM, _T("SetOEM_DoEnableAP"));
	if (m_fpSetOEM_DoEnableAP)
		m_fpSetOEM_DoEnableAP(bVal);

	LogLine(_T("Set Enable AP: %d"), bVal);
}

STDMETHODIMP COEMSettingsManager::get_ShowVPTray(VARIANT_BOOL* pVal)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
	if (pVal == NULL)
		return E_POINTER;

	*pVal = (OEMSettings.IsOEM_ShowVPTray() == TRUE) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

BOOL COEMSettings::IsOEM_ShowVPTray()
{
	BOOL  bRet = TRUE;

	m_hOEM = LoadOEMLibrary();

	if (m_hOEM && !m_fpIsOEM_ShowVPTray)
		m_fpIsOEM_ShowVPTray = (lpfnIsOEM_ShowVPTray) GetProcAddress(m_hOEM, _T("IsOEM_ShowVPTray"));
	if (m_fpIsOEM_ShowVPTray != NULL)
	{
		bRet = m_fpIsOEM_ShowVPTray();
	}
	else bRet = TRUE;;

	LogLine(_T("Show VPTray: %d"), bRet);
	return bRet;
}

STDMETHODIMP COEMSettingsManager::get_NoTriggerEULA(VARIANT_BOOL* pVal)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
	if (pVal == NULL)
		return E_POINTER;

	*pVal = (OEMSettings.IsOEM_NoTriggerEULA() == TRUE) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

BOOL COEMSettings::IsOEM_NoTriggerEULA()
{
	BOOL bRet = FALSE;
	m_hOEM = LoadOEMLibrary();

	if (m_hOEM && !m_fpIsOEM_NoTriggerEULA)
		m_fpIsOEM_NoTriggerEULA = (lpfnIsOEM_NoTriggerEULA) GetProcAddress(m_hOEM, _T("IsOEM_NoTriggerEULA"));
	if (m_fpIsOEM_NoTriggerEULA != NULL)
	{
		bRet = m_fpIsOEM_NoTriggerEULA();
	}
	else bRet = FALSE;;

	LogLine(_T("NoTriggerEULA: %d"), bRet);
	return bRet;
}

STDMETHODIMP COEMSettingsManager::put_NoTriggerEULA(VARIANT_BOOL newVal)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

	OEMSettings.SetOEM_NoTriggerEULA((newVal == VARIANT_TRUE) ? TRUE : FALSE);
	return S_OK;
}

void COEMSettings::SetOEM_NoTriggerEULA(BOOL bVal)
{
	m_hOEM = LoadOEMLibrary();

	if (m_hOEM && !m_fpSetOEM_NoTriggerEULA)
		m_fpSetOEM_NoTriggerEULA = (lpfnSetOEM_NoTriggerEULA) GetProcAddress(m_hOEM, _T("SetOEM_NoTriggerEULA"));
	if (m_fpSetOEM_NoTriggerEULA)
		m_fpSetOEM_NoTriggerEULA(bVal);
}

STDMETHODIMP COEMSettingsManager::get_LicenseType(int* pVal)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

	if (pVal == NULL)
		return E_POINTER;

	*pVal = OEMSettings.GetOEM_LicenseType();
	return S_OK;
}
int COEMSettings::GetOEM_LicenseType()
{
	int dRet = 0;

	m_hOEM = LoadOEMLibrary();

	if (m_hOEM && !m_fpGetOEM_LicenseType)
		m_fpGetOEM_LicenseType = (lpfnGetOEM_LicenseType) GetProcAddress(m_hOEM, _T("GetOEM_LicenseType"));
	if (m_fpGetOEM_LicenseType != NULL)
	{
		dRet = m_fpGetOEM_LicenseType();
	}
	else dRet = 0;

	LogLine(_T("License type: %d"), dRet);
	return dRet;
}

// returns S_OK if such path is found in the system.
// returns S_FALSE if no such path is found.
STDMETHODIMP COEMSettingsManager::get_EULAPath(BSTR* pVal)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

	USES_CONVERSION;
	HRESULT hr = S_OK;
	TCHAR szEULATextPath[MAX_PATH] = {0};
	if (pVal == NULL)
		return E_POINTER;
	::SysFreeString(*pVal);

	if (OEMSettings.GetOEM_EULAPath(szEULATextPath) == TRUE)
	{
		*pVal = ::SysAllocStringLen(T2COLE(szEULATextPath), MAX_PATH);
		LogLine(_T("EULA file: %s"), *pVal);
	}
	else
		hr = S_FALSE;

	return hr;

}

BOOL COEMSettings::GetOEM_EULAPath(LPTSTR szEULATextPath)
{
	BOOL bRet = TRUE;
	m_hOEM = LoadOEMLibrary();

	if (m_hOEM && !m_fpGetOEM_EULAPath)
		m_fpGetOEM_EULAPath = (lpfnGetOEM_EULAPath) GetProcAddress(m_hOEM, _T("GetOEM_EULAPath"));
	
	if (m_fpGetOEM_EULAPath != NULL)
	{
		if (m_fpGetOEM_EULAPath(szEULATextPath))
		{
			LogLine(_T("EULA file: %s"), szEULATextPath);
		}
		else bRet = S_FALSE;
			
	}
	else bRet = S_FALSE;

	return bRet;
}

STDMETHODIMP COEMSettingsManager::get_CheckAlertDays(LONG lDaysUntilExpire, VARIANT_BOOL* pVal)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
	BOOL bAlert = FALSE;
	if (pVal == NULL)
		return E_POINTER;

	OEMSettings.CheckAlertDays(lDaysUntilExpire, &bAlert);
	*pVal = (bAlert == TRUE) ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;

}
void COEMSettings::CheckAlertDays(LONG lDaysUntilExpire, BOOL* pbAlert)
{
	m_hOEM = LoadOEMLibrary();

	if (m_hOEM && !m_fpCheckAlertDays)
		m_fpCheckAlertDays = (lpfnCheckAlertDays) GetProcAddress(m_hOEM, _T("CheckAlertDays"));
	if (m_fpCheckAlertDays)
		m_fpCheckAlertDays(lDaysUntilExpire, pbAlert);

	LogLine(_T("Alert: %d"), *pbAlert);
}

STDMETHODIMP COEMSettingsManager::CheckBootCount(VARIANT_BOOL* pVal)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
	if (pVal == NULL)
		return E_POINTER;

	*pVal = (OEMSettings.CheckOEM_BootCount() == TRUE) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

BOOL COEMSettings::CheckOEM_BootCount()
{
	BOOL bRet = FALSE;

	m_hOEM = LoadOEMLibrary();

	if (m_hOEM && !m_fpCheckOEM_BootCount)
		m_fpCheckOEM_BootCount = (lpfnCheckOEM_BootCount) GetProcAddress(m_hOEM, _T("CheckOEM_BootCount"));
	if (m_fpCheckOEM_BootCount != NULL)
	{
		bRet = m_fpCheckOEM_BootCount();
	}
	else bRet = FALSE;

	LogLine(_T("Zero boot count: %d"), bRet);
	return bRet;
}

STDMETHODIMP COEMSettingsManager::DecrementBootCount(void)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

	OEMSettings.DecrementOEM_BootCount();
	return S_OK;
}


void COEMSettings::DecrementOEM_BootCount(void)
{
	m_hOEM = LoadOEMLibrary();

	if (m_hOEM && !m_fpDecrementOEM_BootCount)
		m_fpDecrementOEM_BootCount = (lpfnDecrementOEM_BootCount) GetProcAddress(m_hOEM, _T("DecrementOEM_BootCount"));
	if (m_fpDecrementOEM_BootCount)
		m_fpDecrementOEM_BootCount();
}

// returns S_OK if customized URL is found in the registry.
// returns S_FALSE if no such URL is found.
STDMETHODIMP COEMSettingsManager::get_Url(BSTR* pVal)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
	USES_CONVERSION;
	HRESULT hr = S_OK;
	TCHAR szUrl[MAX_PATH] = {0};
	if (pVal == NULL)
		return E_POINTER;
	::SysFreeString(*pVal);

	if (OEMSettings.GetOEM_Url(szUrl, MAX_PATH) == TRUE)
	{
		*pVal = ::SysAllocStringLen(T2COLE(szUrl), MAX_PATH);
	}
	else
	{
		hr = S_FALSE;
	}

	return hr;
}

BOOL COEMSettings::GetOEM_Url(LPTSTR szUrl, DWORD dwMaxLen)
{
	BOOL bRet = TRUE;

	m_hOEM = LoadOEMLibrary();

	if (m_hOEM && !m_fpGetOEM_Url)
		m_fpGetOEM_Url = (lpfnGetOEM_Url) GetProcAddress(m_hOEM, _T("GetOEM_Url"));
	
	if (m_fpGetOEM_Url != NULL)
	{
		if (m_fpGetOEM_Url(szUrl, dwMaxLen))
		{
			LogLine(_T("URL: %s"), szUrl);
		}
		else bRet = FALSE;
	}
	else bRet = FALSE;

	return bRet;
}

STDMETHODIMP COEMSettingsManager::get_RealELSMode(VARIANT_BOOL* pVal)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
	if (pVal == NULL)
		return E_POINTER;

	*pVal = (OEMSettings.IsOEM_RealELSMode() == TRUE) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

BOOL COEMSettings::IsOEM_RealELSMode()
{
	BOOL bRet = FALSE;

	m_hOEM = LoadOEMLibrary();

	if (m_hOEM && !m_fpIsOEM_RealELSMode)
		m_fpIsOEM_RealELSMode = (lpfnIsOEM_RealELSMode) GetProcAddress(m_hOEM, _T("IsOEM_RealELSMode"));
	if (m_fpIsOEM_RealELSMode != NULL)
	{
		bRet = m_fpIsOEM_RealELSMode();
	}
	else bRet = FALSE;

	LogLine(_T("Real ELS mode: %d"), bRet);
	return bRet;
}

STDMETHODIMP COEMSettingsManager::put_RealELSMode(VARIANT_BOOL newVal)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

	OEMSettings.SetOEM_RealELSMode((newVal == VARIANT_TRUE) ? TRUE : FALSE);
	return S_OK;
}

void COEMSettings::SetOEM_RealELSMode(BOOL bVal)
{
	m_hOEM = LoadOEMLibrary();

	if (m_hOEM && !m_fpSetOEM_RealELSMode)
		m_fpSetOEM_RealELSMode = (lpfnSetOEM_RealELSMode) GetProcAddress(m_hOEM, _T("SetOEM_RealELSMode"));
	if (m_fpSetOEM_RealELSMode)
		m_fpSetOEM_RealELSMode(bVal);
}

STDMETHODIMP COEMSettingsManager::get_RealSubscriptionMode(VARIANT_BOOL* pVal)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
	if (pVal == NULL)
		return E_POINTER;

	*pVal = (OEMSettings.IsOEM_RealSubscriptionMode() == TRUE) ? VARIANT_TRUE : VARIANT_FALSE;
	
	return S_OK;
}

BOOL COEMSettings::IsOEM_RealSubscriptionMode()
{
	BOOL bRet = FALSE;

	m_hOEM = LoadOEMLibrary();

	if (m_hOEM && !m_fpIsOEM_RealSubscriptionMode)
		m_fpIsOEM_RealSubscriptionMode = (lpfnIsOEM_RealSubscriptionMode) GetProcAddress(m_hOEM, _T("IsOEM_RealSubscriptionMode"));
	
	if (m_fpIsOEM_RealSubscriptionMode != NULL)
	{
		bRet = m_fpIsOEM_RealSubscriptionMode();
	}
	else bRet = FALSE;;

	LogLine(_T("Real subscription mode: %d"), bRet);

	return bRet;
	
}

STDMETHODIMP COEMSettingsManager::put_RealSubscriptionMode(VARIANT_BOOL newVal)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

	OEMSettings.SetOEM_RealSubscriptionMode((newVal == VARIANT_TRUE) ? TRUE : FALSE);
	return S_OK;
}
void COEMSettings::SetOEM_RealSubscriptionMode(BOOL bVal)
{
	m_hOEM = LoadOEMLibrary();

	if (m_hOEM && !m_fpSetOEM_RealSubscriptionMode)
		m_fpSetOEM_RealSubscriptionMode = (lpfnSetOEM_RealSubscriptionMode) GetProcAddress(m_hOEM, _T("SetOEM_RealSubscriptionMode"));
	if (m_fpSetOEM_RealSubscriptionMode)
		m_fpSetOEM_RealSubscriptionMode(bVal);
}

STDMETHODIMP COEMSettingsManager::get_OEMInstall(VARIANT_BOOL* pVal)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
	if (pVal == NULL)
		return E_POINTER;

	*pVal = (OEMSettings.IsOEM_Install() == TRUE) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

BOOL COEMSettings::IsOEMBuild()
{
	return IsOEM_Install();
}

BOOL COEMSettings::IsOEM_Install()
{
	BOOL bRet = FALSE;
	m_hOEM = LoadOEMLibrary();

	if (m_hOEM && !m_fpIsOEM_Install)
		m_fpIsOEM_Install = (lpfnIsOEM_Install) GetProcAddress(m_hOEM, _T("IsOEM_Install"));
	if (m_fpIsOEM_Install != NULL)
	{
		bRet = m_fpIsOEM_Install();
	}
	else bRet = FALSE;

	LogLine(_T("OEM Install: %d"), bRet);
	return bRet;
}

STDMETHODIMP COEMSettingsManager::put_OEMInstall(VARIANT_BOOL newVal)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

	OEMSettings.SetOEM_Install((newVal == VARIANT_TRUE) ? TRUE : FALSE);
	return S_OK;
}

void COEMSettings::SetOEM_Install(BOOL bVal)
{
	m_hOEM = LoadOEMLibrary();

	if (m_hOEM && !m_fpSetOEM_Install)
		m_fpSetOEM_Install = (lpfnSetOEM_Install) GetProcAddress(m_hOEM, _T("SetOEM_Install"));
	if (m_fpSetOEM_Install)
		m_fpSetOEM_Install(bVal);
}

STDMETHODIMP COEMSettingsManager::get_OEMTrial(VARIANT_BOOL* pVal)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
	if (pVal == NULL)
		return E_POINTER;

	*pVal = (OEMSettings.IsOEM_TrialApp() == TRUE) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

BOOL COEMSettings::IsOEM_TrialApp()
{
	BOOL bRet = FALSE;
	m_hOEM = LoadOEMLibrary();

	if (m_hOEM && !m_fpIsOEM_TrialApp)
		m_fpIsOEM_TrialApp = (lpfnIsOEM_TrialApp) GetProcAddress(m_hOEM, _T("IsOEM_TrialApp"));
	if (m_fpIsOEM_TrialApp != NULL)
	{
		bRet = m_fpIsOEM_TrialApp();
	}
	else bRet = FALSE;

	return bRet;
}

// returns S_OK if customized text is found in the registry.
// returns S_FALSE if no such text is found.
STDMETHODIMP COEMSettingsManager::get_NagDialogText(VARIANT_BOOL bSCS, UINT nRemainingDays, UINT nMaxLen, BSTR* pVal)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
	USES_CONVERSION;
	HRESULT hr = S_OK;
	TCHAR* szText = NULL;

	if (pVal == NULL)
		return E_POINTER;
	::SysFreeString(*pVal);

	szText = new TCHAR[nMaxLen+1];
	if (!szText)
		return E_OUTOFMEMORY;
	memset(szText, 0, sizeof(szText));

	if (OEMSettings.GetOEM_NagDialogText((bSCS == TRUE) ? VARIANT_TRUE : VARIANT_FALSE,
		nRemainingDays, szText, nMaxLen) == TRUE)
	{
		*pVal = ::SysAllocStringLen(T2COLE(szText), nMaxLen);
	}
	else
	{
		hr = S_FALSE;
	}
	delete []szText;
	return hr;
}

BOOL COEMSettings::GetOEM_NagDialogText(BOOL bSCS, UINT nRemainingDays, LPTSTR szText, UINT nMaxLen)
{
	BOOL bRet = TRUE;
	m_hOEM = LoadOEMLibrary();

	if (m_hOEM && !m_fpGetOEM_NagDialogText)
		m_fpGetOEM_NagDialogText = (lpfnGetOEM_NagDialogText) GetProcAddress(m_hOEM, _T("GetOEM_NagDialogText"));
	if (m_fpGetOEM_NagDialogText)
	{
		if (m_fpGetOEM_NagDialogText(bSCS, nRemainingDays, szText, nMaxLen))
		{
		}
		else bRet = FALSE;
	}
	else bRet = FALSE;

	return bRet;
}


// returns S_OK if customized text is found in the registry.
// returns S_FALSE if no such text is found.
STDMETHODIMP COEMSettingsManager::get_ExpireDialogText(VARIANT_BOOL bSCS, UINT nMaxLen, BSTR* pVal)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
	USES_CONVERSION;
	HRESULT hr = S_OK;
	TCHAR* szText = NULL;
	if (pVal == NULL)
		return E_POINTER;
	::SysFreeString(*pVal);

	szText = new TCHAR[nMaxLen+1];
	if (!szText)
		return E_OUTOFMEMORY;
	memset(szText, 0, sizeof(szText));
	
	if (OEMSettings.GetOEM_ExpireDialogText((bSCS == TRUE) ? VARIANT_TRUE : VARIANT_FALSE,
		szText, nMaxLen) == TRUE)
	{
		*pVal = ::SysAllocStringLen(T2COLE(szText), nMaxLen);
	}
	else
	{
		hr = S_FALSE;
	}

	delete []szText;
	return hr;
}

BOOL COEMSettings::GetOEM_ExpireDialogText(BOOL bSCS, LPTSTR szText, UINT nMaxLen)
{
	BOOL bRet = TRUE;

	m_hOEM = LoadOEMLibrary();

	if (m_hOEM && !m_fpGetOEM_ExpireDialogText)
		m_fpGetOEM_ExpireDialogText = (lpfnGetOEM_ExpireDialogText) GetProcAddress(m_hOEM, _T("GetOEM_ExpireDialogText"));
	if (m_fpGetOEM_ExpireDialogText)
	{
		if (m_fpGetOEM_ExpireDialogText(bSCS, szText, nMaxLen))
		{
		}
		else bRet = FALSE;
	}
	else bRet = FALSE;

	return bRet;
}

STDMETHODIMP COEMSettingsManager::CreateOEMLock(BSTR bstrLockName, LONG* phLock)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
	USES_CONVERSION;
	if (phLock == NULL)
		return E_POINTER;
	if (::SysStringLen(bstrLockName) == 0)
		return E_INVALIDARG;

	*phLock = (LONG)OEMSettings.CreateOEM_Lock(OLE2CT(bstrLockName));

	return S_OK;
}

HANDLE COEMSettings::CreateOEM_Lock(LPCTSTR szLockName)
{
	HANDLE lLockHan = 0;
	m_hOEM = LoadOEMLibrary();

	if (m_hOEM && !m_fpCreateOEM_Lock)
		m_fpCreateOEM_Lock = (lpfnCreateOEM_Lock) GetProcAddress(m_hOEM, _T("CreateOEM_Lock"));
	
	if (m_fpCreateOEM_Lock != NULL)
	{
		lLockHan = m_fpCreateOEM_Lock(szLockName);
	}
	
	return lLockHan;
}

STDMETHODIMP COEMSettingsManager::SetTrialLenInSessionFile(void)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

	OEMSettings.SetTrialLenInSessionFile();
	return S_OK;
}

void COEMSettings::SetTrialLenInSessionFile(void)
{
	m_hOEM = LoadOEMLibrary();

	if (m_hOEM && !m_fpSetTrialLenInSessionFile)
		m_fpSetTrialLenInSessionFile = (lpfnSetTrialLenInSessionFile) GetProcAddress(m_hOEM, _T("SetTrialLenInSessionFile"));
	if (m_fpSetTrialLenInSessionFile)
		m_fpSetTrialLenInSessionFile();
}

STDMETHODIMP COEMSettingsManager::LoadConfigIni(VARIANT_BOOL bDelete, BSTR* pbstrErrorDescription)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
	USES_CONVERSION;
	TCHAR szErrorDescription[MAX_PATH] = {0};
	if (pbstrErrorDescription != NULL) // if NULL, error description is not requested
		::SysFreeString(*pbstrErrorDescription);

	OEMSettings.LoadConfigIniToRegistry((bDelete == VARIANT_TRUE) ? TRUE : FALSE, szErrorDescription);
	if (_tcslen(szErrorDescription) > 0 && pbstrErrorDescription != NULL)
	{
		 // if there is error and error description is requested, populate the error string
		::SysAllocStringLen(T2COLE(szErrorDescription), MAX_PATH);
	}
	return S_OK;
}

void COEMSettings::LoadConfigIniToRegistry(BOOL bDelete, LPTSTR szErrorDescription)
{
	m_hOEM = LoadOEMLibrary();

	if (m_hOEM && !m_fpLoadConfigIniToRegistry)
		m_fpLoadConfigIniToRegistry = (lpfnLoadConfigIniToRegistry) GetProcAddress(m_hOEM, _T("LoadConfigIniToRegistry"));
	if (m_fpLoadConfigIniToRegistry)
	{
		m_fpLoadConfigIniToRegistry(bDelete, szErrorDescription);
	}
}

STDMETHODIMP COEMSettingsManager::get_UseNoUrl(VARIANT_BOOL* pVal)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
	if (pVal == NULL)
		return E_POINTER;

	*pVal = (OEMSettings.IsOEM_NoUrl() == TRUE) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

BOOL COEMSettings::IsOEM_NoUrl()
{
	BOOL bRet = FALSE;
	m_hOEM = LoadOEMLibrary();

	if (m_hOEM && !m_fpIsOEM_NoUrl)
		m_fpIsOEM_NoUrl = (lpfnIsOEM_NoUrl) GetProcAddress(m_hOEM, _T("IsOEM_NoUrl"));
	if (m_fpIsOEM_NoUrl)
	{
		bRet = m_fpIsOEM_NoUrl();
	}
	else bRet = FALSE;

	return bRet;
}

// returns S_OK if customized text is found in the registry.
// returns S_FALSE if no such text is found.
STDMETHODIMP COEMSettingsManager::get_AdminText(BSTR* pVal)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
	USES_CONVERSION;
	TCHAR szText[MAX_PATH] = {0};
	HRESULT hr = S_OK;
	if (pVal == NULL)
		return E_POINTER;
	::SysFreeString(*pVal);

	if (OEMSettings.GetOEM_AdminText(szText, MAX_PATH) == TRUE)
	{
		SysAllocStringLen(T2COLE(szText), MAX_PATH);
	}
	else
		hr = S_FALSE;

	return hr;
}

BOOL COEMSettings::GetOEM_AdminText(LPTSTR szText, DWORD dwMaxLen)
{
	BOOL bRet = TRUE;
	m_hOEM = LoadOEMLibrary();

	if (m_hOEM && !m_fpGetOEM_AdminText)
		m_fpGetOEM_AdminText = (lpfnGetOEM_AdminText) GetProcAddress(m_hOEM, _T("GetOEM_AdminText"));
	if (m_fpGetOEM_AdminText)
	{
		if (m_fpGetOEM_AdminText(szText, dwMaxLen))
		{
		}
		else bRet = FALSE;
	}
	else bRet = FALSE;

	return bRet;
}

// IOEMLicenseManager interface 
STDMETHODIMP COEMSettingsManager::StartSubscriptionNow(void)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
	return OEMSettings.StartSubscriptionNow();
}

HRESULT COEMSettings::StartSubscriptionNow(void)
{
	HRESULT hr = E_FAIL;

	SLIC_STATUS slic_status = SLICSTATUS_OKFALSE;

	try
	{
		if (!IsOEM_RealELSMode())
		{
			slic_status = SlicStartSubscription((GetOEM_LicenseType() == ANNUAL) ? SLIC_SUBSCRIPTION_ANNUAL : SLIC_SUBSCRIPTION_TRIAL);
		}
	}
	catch(...)
	{
		MessageBox(NULL, _T("SlicStartSubscription() throws exception"), _T("SLIC Error"), MB_ICONSTOP | MB_OK);
		slic_status = SLICSTATUS_UNEXPECTED;
	}

	hr = slic_status;

	return hr;
}

// GLEE (2/13/06):
// not sure if this is needed. If client app returns right value from calling SlicWrap, this won't be necessary.
STDMETHODIMP COEMSettingsManager::get_SubscriptionMode(LONG* pVal)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

	*pVal = OEMSettings.IsSubscriptionMode();

    return S_OK;
}

HRESULT COEMSettings::IsSubscriptionMode()
{
	HRESULT hr = E_FAIL;

	SLIC_STATUS slic_status = SLICSTATUS_OKFALSE;
	SLIC_MODE slic_mode = SLIC_MODE_ELS;
	try
	{
		slic_status = SlicGetSubscriptionMode(&slic_mode);
	}
	catch(...)
	{
//		MessageBox(NULL, _T("SlicGetSubscriptionMode() throws exception"), _T("SLIC Error"), MB_ICONSTOP | MB_OK);
		slic_status = SLICSTATUS_UNEXPECTED;
	}

	// We want to remain In LiveSubscribe mode even when the we are in "Pre-Subscription" state.
	// This is done by checking whether we have initialized the EULA or not.
	// The reason is that we don't want to switch to ELS mode and receive License updates in "Pre-Subscription state".

	hr = ((!IsOEM_Initialized()) || (slic_mode == SLIC_MODE_SUBSCRIPTION));
	return hr;
}

STDMETHODIMP COEMSettingsManager::SynchronizeSubscription(void)
{
#ifndef SERVICE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

	return OEMSettings.SynchronizeSubscription();
}

HRESULT COEMSettings::SynchronizeSubscription(void)
{
	HRESULT hr = E_FAIL;
	SLIC_STATUS slic_status = SLICSTATUS_OKFALSE;

	try
	{
		//HANDLE hMutex = CreateOEM_Lock(SYNC_WITH_SERVER_MUTEX);
		slic_status = SlicSynchronizeSubscription();
		//if (hMutex)
		//{
		//	ReleaseMutex(hMutex);
		//	CloseHandle(hMutex);
		//}
	}
	catch(...)
	{
		//MessageBox(NULL, _T("SlicSynchronizeSubscription() throws exception"), _T("SLIC Error"), MB_ICONSTOP | MB_OK);
		slic_status = SLICSTATUS_UNEXPECTED;
	}

	hr = slic_status;

	return hr;
}