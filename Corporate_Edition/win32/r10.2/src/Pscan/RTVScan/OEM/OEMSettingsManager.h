// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005,2006 Symantec Corporation. All rights reserved.
// OEMSettingsManager.h : Declaration of the COEMSettingsManager
//////////////////////////////////////////////////////////////////////


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


#ifndef SYMANTEC_SHARED_KEY
#define SYMANTEC_SHARED_KEY		_T("SymantecShared")
#endif 

#ifndef SYMANTEC_ABOVE_OEM_KEY
#define SYMANTEC_ABOVE_OEM_KEY	_T("SOFTWARE\\Symantec\\Symantec AntiVirus\\install\\7.50")
#endif

void LogLine(LPCTSTR lpszFormat, ...);




// COEMSettingsManager

class ATL_NO_VTABLE COEMSettingsManager : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<COEMSettingsManager, &CLSID_OEMSettingsManager>,
	public ISupportErrorInfo,
	public IOEMSettingsManager,
	public IOEMLicenseManager
{
public:
	DECLARE_CLASSFACTORY_SINGLETON(COEMSettingsManager)

	

DECLARE_REGISTRY_RESOURCEID(IDR_OEMSETTINGSMANAGER)


BEGIN_COM_MAP(COEMSettingsManager)
	COM_INTERFACE_ENTRY(IOEMLicenseManager)
	COM_INTERFACE_ENTRY(IOEMSettingsManager)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		HRESULT hr = ERROR_SUCCESS;
	
		return hr;
	}
	
	void FinalRelease() 
	{
	}


public:
	// IOEMSettingsManager
	STDMETHOD(get_CustomerBoot)(VARIANT_BOOL* pVal);
	STDMETHOD(get_EULAAccepted)(VARIANT_BOOL* pVal);
	STDMETHOD(put_EULAAccepted)(VARIANT_BOOL newVal);
	STDMETHOD(get_InitDialogsDone)(VARIANT_BOOL* pVal);
	STDMETHOD(put_InitDialogsDone)(VARIANT_BOOL newVal);
	STDMETHOD(get_OEMInitialized)(VARIANT_BOOL* pVal);
	STDMETHOD(get_EnableAP)(VARIANT_BOOL* pVal);
	STDMETHOD(put_EnableAP)(VARIANT_BOOL newVal);
	STDMETHOD(get_ShowVPTray)(VARIANT_BOOL* pVal);
	STDMETHOD(get_NoTriggerEULA)(VARIANT_BOOL* pVal);
	STDMETHOD(put_NoTriggerEULA)(VARIANT_BOOL newVal);
	STDMETHOD(get_LicenseType)(int* pVal);
	STDMETHOD(get_EULAPath)(BSTR* pVal);
	STDMETHOD(get_CheckAlertDays)(LONG lDaysUntilExpire, VARIANT_BOOL* pVal);
	STDMETHOD(CheckBootCount)(VARIANT_BOOL* pVal);
	STDMETHOD(DecrementBootCount)(void);
	STDMETHOD(get_Url)(BSTR* pVal);
	STDMETHOD(get_RealELSMode)(VARIANT_BOOL* pVal);
	STDMETHOD(put_RealELSMode)(VARIANT_BOOL newVal);
	STDMETHOD(get_RealSubscriptionMode)(VARIANT_BOOL* pVal);
	STDMETHOD(put_RealSubscriptionMode)(VARIANT_BOOL newVal);
	STDMETHOD(get_OEMInstall)(VARIANT_BOOL* pVal);
	STDMETHOD(put_OEMInstall)(VARIANT_BOOL newVal);
	STDMETHOD(get_OEMTrial)(VARIANT_BOOL* pVal);
	STDMETHOD(get_NagDialogText)(VARIANT_BOOL bSCS, UINT nRemainingDays, UINT nMaxLen, BSTR* pVal);
	STDMETHOD(get_ExpireDialogText)(VARIANT_BOOL bSCS, UINT nMaxLen, BSTR* pVal);
	STDMETHOD(CreateOEMLock)(BSTR bstrLockName, LONG* phLock);
	STDMETHOD(SetTrialLenInSessionFile)(void);
	STDMETHOD(LoadConfigIni)(VARIANT_BOOL bDelete, BSTR* pbstrErrorDescription);
	STDMETHOD(get_UseNoUrl)(VARIANT_BOOL* pVal);
	STDMETHOD(get_AdminText)(BSTR* pVal);

	// IOEMLicenseManager
	STDMETHOD(StartSubscriptionNow)(void);
	STDMETHOD(get_SubscriptionMode)(LONG* pVal);
	STDMETHOD(SynchronizeSubscription)(void);


};

OBJECT_ENTRY_AUTO(__uuidof(OEMSettingsManager), COEMSettingsManager)
