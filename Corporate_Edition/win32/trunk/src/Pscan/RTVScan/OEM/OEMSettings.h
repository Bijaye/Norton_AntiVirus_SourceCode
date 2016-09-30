#ifndef __OEMSETTINGS_H_
#define __OEMSETTINGS_H_
#pragma once
#ifndef SERVICE
#include "resource.h"       // main symbols
#endif
#include "oem_common.h"
#ifndef SERVICE
#include "SymOEMSvc.h"
#else
EXTERN_C const IID LIBID_RTVScanLib;
#include "Rtvscan.h"
#endif


// COEMSettings
class COEMSettings
{
public:
	COEMSettings();
	~COEMSettings();
	BOOL						IsOEM_CustomerBoot();
	BOOL						IsOEM_EULAAccepted();
	void						SetOEM_EULAAccepted(BOOL bAccepted);
	BOOL						IsOEM_InitDialogsDone();
	void						SetOEM_InitDialogsDone(BOOL bVal);
	BOOL						IsOEM_Initialized();
	BOOL						IsOEM_DoEnableAP();
	void						SetOEM_DoEnableAP(BOOL bVal);
	BOOL						IsOEM_ShowVPTray();
	BOOL						IsOEM_NoTriggerEULA();
	void						SetOEM_NoTriggerEULA(BOOL bVal);
	int							GetOEM_LicenseType();
	BOOL						GetOEM_EULAPath(LPTSTR pVal);
	void						CheckAlertDays(LONG lDaysUntilExpire, BOOL* bAlert);
	BOOL						CheckOEM_BootCount();
	void						DecrementOEM_BootCount(void);
	BOOL						GetOEM_Url(LPTSTR szUrl, DWORD dwMaxLen);
	BOOL						IsOEM_RealELSMode();
	void						SetOEM_RealELSMode(BOOL bVal);
	BOOL						IsOEM_RealSubscriptionMode();
	void						SetOEM_RealSubscriptionMode(BOOL bVal);
	BOOL						IsOEM_Install();
	void						SetOEM_Install(BOOL bVal);
	BOOL						IsOEM_TrialApp();
    BOOL                        GetOEM_NagDialogText(BOOL bSCS, UINT nRemainingDays, LPTSTR szText, UINT nMaxLen);
    BOOL						GetOEM_ExpireDialogText(BOOL bSCS, LPTSTR szText, UINT nMaxLen);
	HANDLE 						CreateOEM_Lock(LPCTSTR szLockName);
	void						SetTrialLenInSessionFile(void);
	void						LoadConfigIniToRegistry(BOOL bDelete, LPTSTR szErrorDescription);
	BOOL						IsOEM_NoUrl();
	BOOL						GetOEM_AdminText(LPTSTR szText, DWORD dwMaxLen);
	HRESULT						StartSubscriptionNow(void);
	HRESULT						IsSubscriptionMode();
	HRESULT						SynchronizeSubscription(void);
	BOOL						IsOEMBuild();


private:
	BOOL GetOEM_Dir(LPTSTR lpszDirBuf, DWORD dwMaxLen);
	HMODULE LoadOEMLibrary();
	HMODULE m_hOEM;
	TCHAR m_strOEMDll[1024];
	lpfnIsOEM_CustomerBoot		m_fpIsOEM_CustomerBoot;
	lpfnSetOEM_CustomerBoot		m_fpSetOEM_CustomerBoot;
	lpfnIsOEM_EULAAccepted		m_fpIsOEM_EULAAccepted;
	lpfnSetOEM_EULAAccepted		m_fpSetOEM_EULAAccepted;
	lpfnIsOEM_InitDialogsDone	m_fpIsOEM_InitDialogsDone;
	lpfnSetOEM_InitDialogsDone	m_fpSetOEM_InitDialogsDone;
	lpfnIsOEM_Initialized		m_fpIsOEM_Initialized;
	lpfnIsOEM_DoEnableAP		m_fpIsOEM_DoEnableAP;
	lpfnSetOEM_DoEnableAP		m_fpSetOEM_DoEnableAP;
	lpfnIsOEM_ShowVPTray		m_fpIsOEM_ShowVPTray;
	lpfnLaunchConfigWiz			m_fpLaunchConfigWiz;
	lpfnOEM_DisableDriversServices	m_fpOEM_DisableDriversServices;
	lpfnReadTimeFromRegistry	m_fpReadTimeFromRegistry;
	lpfnWriteTimeToRegistry		m_fpWriteTimeToRegistry;
	lpfnIsOEM_NoTriggerEULA		m_fpIsOEM_NoTriggerEULA;
	lpfnSetOEM_NoTriggerEULA	m_fpSetOEM_NoTriggerEULA;
	lpfnGetOEM_RegisterInfo		m_fpGetOEM_RegisterInfo;
	lpfnSetOEM_RegisterInfo		m_fpSetOEM_RegisterInfo;
	lpfnGetOEM_LicenseType		m_fpGetOEM_LicenseType;
	lpfnSetOEM_LicenseType		m_fpSetOEM_LicenseType;
	lpfnGetOEM_EULAPath			m_fpGetOEM_EULAPath;
	lpfnCheckAlertDays			m_fpCheckAlertDays;
	lpfnSetOEM_ConfigFileLocation	m_fpSetOEM_ConfigFileLocation;
	lpfnGetOEM_ConfigFileLocation	m_fpGetOEM_ConfigFileLocation;
	lpfnCheckOEM_BootCount		m_fpCheckOEM_BootCount;
	lpfnDecrementOEM_BootCount	m_fpDecrementOEM_BootCount;
	lpfnGetOEM_BootCount		m_fpGetOEM_BootCount;
	lpfnSetOEM_BootCount		m_fpSetOEM_BootCount;
	lpfnSetOEM_ShowVPTray		m_fpSetOEM_ShowVPTray;
	lpfnGetOEM_Url				m_fpGetOEM_Url;
	lpfnIsOEM_RealELSMode		m_fpIsOEM_RealELSMode;
	lpfnSetOEM_RealELSMode		m_fpSetOEM_RealELSMode;
	lpfnIsOEM_RealSubscriptionMode	m_fpIsOEM_RealSubscriptionMode;
	lpfnSetOEM_RealSubscriptionMode	m_fpSetOEM_RealSubscriptionMode;
	lpfnIsOEM_Install			m_fpIsOEM_Install;
	lpfnSetOEM_Install			m_fpSetOEM_Install;
	lpfnIsOEM_FactoryMode		m_fpIsOEM_FactoryMode;
	lpfnSetOEM_FactoryMode		m_fpSetOEM_FactoryMode;
	lpfnIsOEM_TrialApp			m_fpIsOEM_TrialApp;
	lpfnSetOEM_TrialApp			m_fpSetOEM_TrialApp;
	lpfnGetOEM_TrialLen			m_fpGetOEM_TrialLen;
	lpfnSetOEM_TrialLen			m_fpSetOEM_TrialLen;
	lpfnGetOEM_NagDialogText	m_fpGetOEM_NagDialogText;
	lpfnGetOEM_ExpireDialogText m_fpGetOEM_ExpireDialogText;
	lpfnEncryptConfigSettings	m_fpEncryptConfigSettings;
	lpfnIsOEM_Resealed			m_fpIsOEM_Resealed;
	lpfnCreateOEM_Lock			m_fpCreateOEM_Lock;
	lpfnSetTrialLenInSessionFile m_fpSetTrialLenInSessionFile;
	lpfnLoadConfigIniToRegistry  m_fpLoadConfigIniToRegistry;
	lpfnIsOEM_NoUrl	m_fpIsOEM_NoUrl;
	lpfnSetOEM_NoUrl m_fpSetOEM_NoUrl;
	lpfnGetOEM_AdminText m_fpGetOEM_AdminText;
	lpfnSetOEM_AdminText m_fpSetOEM_AdminText;
	lpfnDoesOEM_UserModeRegKeyExist m_fpDoesOEM_UserModeRegKeyExist;
	lpfnGetOEM_UserModeRegKey m_fpGetOEM_UserModeRegKey;
	lpfnSetOEM_UserModeRegKey m_fpSetOEM_UserModeRegKey;
};

extern COEMSettings OEMSettings;
#endif // __OEMSETTINGS_H_