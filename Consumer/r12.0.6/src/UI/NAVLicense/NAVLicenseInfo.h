// NAVLicenseInfo.h : Declaration of the CNAVLicense

#ifndef __NAVLICENSEINFO_H_
#define __NAVLICENSEINFO_H_

#include "resource.h"       // main symbols
#include "ScriptSafeVerifyTrustCached.h"		// For scripting security
#include "NAVError.h"    // For errors
#include "ccModuleId.h"     // For module ids
#include "cltLicenseConstants.h"
#include "SymAlertStatic.h"

/////////////////////////////////////////////////////////////////////////////
// CNAVLicense
class ATL_NO_VTABLE CNAVLicenseInfo : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CNAVLicenseInfo, &CLSID_NAVLicenseInfo>,
	public IDispatchImpl<INAVLicenseInfo, &IID_INAVLicenseInfo, &LIBID_NAVLICENSELib>,
	public IObjectSafetyImpl<CNAVLicenseInfo, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA>,
    public CScriptSafe<CNAVLicenseInfo>  // For SafeScript
{
public:
	CNAVLicenseInfo();

DECLARE_REGISTRY_RESOURCEID(IDR_NAVLICENSEINFO)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CNAVLicenseInfo)
	COM_INTERFACE_ENTRY(INAVLicenseInfo)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IObjectSafety)
    COM_INTERFACE_ENTRY(IObjectWithSite)  // For SafeScript
	COM_INTERFACE_ENTRY(ISymScriptSafe)   // Allow programmatic enabling/disabling of safety check
END_COM_MAP()

	HRESULT FinalConstruct();
	void FinalRelease();

// INAVLicenseInfo
public:
	STDMETHOD(get_CanRenewSubscription)(BOOL* pbVal);
	STDMETHOD(get_ShouldCheckSubscription)(BOOL* pbVal);
	STDMETHOD(get_ProductBrandingId)(LONG* pVal);
	STDMETHOD(LaunchActivationWizardEx)(LONG hWndParent, LONG nParam, LONG nFlag);
	STDMETHOD(GetLicenseStateEx)(/*[in]*/ DJSMAR00_QueryMode mode, /*[out, retval]*/ DJSMAR00_LicenseState *pState);
	STDMETHOD(LaunchActivationWizard)(/*[in]*/ long hWndParent, /*[in]*/ long nFlag);
	STDMETHOD(get_LicenseVendorId)(LONG* pVal);
	STDMETHOD(get_LicenseExpirationDate)(BSTR* pVal);
	STDMETHOD(get_ProductName)(BSTR* pVal);
	STDMETHOD(get_LicenseDaysRemaining)(LONG* pVal);
	STDMETHOD(GetLicenseSubType)(/*[out, retval]*/ long* plSubType);
	STDMETHOD(GetLicenseZone)(/*[out,retval]*/long* plZone);
	STDMETHOD(GetLicenseState)(/*[out,retval]*/ DJSMAR00_LicenseState *pState);
	STDMETHOD(GetLicenseType)(/*[out, retval]*/ DJSMAR_LicenseType* pType);
	STDMETHOD(RefreshLicensing)();
	STDMETHOD(ActivateLicense2)(/*[in]*/ long hWndParent, /*[out, retval]*/ BOOL* pbSuccess);
	STDMETHOD(UpdateLicense)(/*[in]*/ BSTR bstrClsWndName, /*[out, retval]*/ BOOL *pbSuccess);
	STDMETHOD(LaunchRentalProductReg)();
	STDMETHOD(LaunchRentalTechSupport)();
	STDMETHOD(LaunchRentalMoreInfo)();
	STDMETHOD(ActivateLicense)(/*[in]*/ BSTR bstrClsWndName, /*[out, retval]*/ BOOL* pbSuccess);
	STDMETHOD(RenewLicense)(/*[in]*/ BSTR bstrClsWndName, /*[out, retval]*/ BOOL *pbSuccess);
	STDMETHOD(get_NAVError)(/*[out, retval]*/ INAVCOMError* *pVal);
	STDMETHOD(get_CancelCode)(LONG* pVal);
	STDMETHOD(get_SubscriptionExpirationDate)(BSTR* pVal);
	STDMETHOD(get_SubscriptionRemaining)(LONG* pVal);
	STDMETHOD(get_SubscriptionLength)(LONG* pVal);
	STDMETHOD(LaunchSubscriptionWizard)(LONG hwndParent);
	STDMETHOD(put_OwnerSeed)(BSTR newVal);
	STDMETHOD(get_VendorName)(BSTR* pVal);
    STDMETHOD(get_SubscriptionDateDW)(DWORD* pVal);
    STDMETHOD(get_SubscriptionExpired)(BOOL* pVal);
    STDMETHOD(get_SubscriptionWarning)(BOOL* pVal);
    STDMETHOD(get_CfgWizFinished)(BOOL* pVal);
    STDMETHOD(get_HasUserAgreedToEULA)(BOOL* pVal);
	STDMETHOD(get_SubscriptionCanAutoRenew)(BOOL* pVal);
	STDMETHOD(get_SubscriptionAutoRenewReady)(BOOL* pVal);

private:
	HRESULT GetSubscriptionInfo();
	HRESULT InternalRefreshLicensing();
	HRESULT InternalActivateLicense(HWND hWndParent, LONG nParam, BOOL* pbSuccess);
	HRESULT GetLicenseInfo(bool bForce = false);
	BOOL SimonLaunchURL(UINT uResID, bool bSimonFunc);

    CString GetSuiteOwnerGUID(void);

    void makeError ( long lMessageID, long HResult );

	// This method displays error message and returns the errCode.
	void DisplayError(HRESULT errCode, LPCTSTR errMsg);

    // Are we asked to force this error to occur?
	bool forceError( long lErrorID );

private:
    CComPtr <INAVCOMError> m_spError;
    CString m_sOwnerSeed;
	CString m_csVendorName;
	CString m_csProductName;

	// License
	bool m_bRefreshed;
	DJSMAR_LicenseType m_Type;
	DJSMAR00_LicenseState m_State;
	DJSMAR00_VendorId m_VendorId;
	DWORD m_Zone;
	DWORD m_dwRemainingDays;
	DWORD m_dwTrialRemainingDays;
	DWORD m_dwCancelCode;
	DWORD m_dwDieDate;

	// Subscription
	bool m_bSubRefreshed;
	bool m_bSubCanRenew;
	DWORD m_dwSubRemainingDays;
	DWORD m_dwSubEndDate;
	DWORD m_dwSubLength;
    BOOL m_bSubExpired;
	BOOL m_bSubWarning;
	BOOL m_bSubCanAutoRenew;
	BOOL m_bSubAutoRenewReady;
};

#endif // __NAVLICENSEINFO_H_
