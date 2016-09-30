// SymNavDefAlert.h : Declaration of the CSymNavDefAlert

#ifndef __SYMNAVDEFALERT_H_
#define __SYMNAVDEFALERT_H_

#include "resource.h"       // main symbols
#include "ScriptSafeVerifyTrustCached.h"		// For scripting security
#include "NAVError.h"    // For errors
#include "ccModuleId.h"     // For module ids

/////////////////////////////////////////////////////////////////////////////
// CSymNavDefAlert
class ATL_NO_VTABLE CSymNavDefAlert : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSymNavDefAlert, &CLSID_SymNavDefAlert>,
	public IDispatchImpl<ISymNavDefAlert, &IID_ISymNavDefAlert, &LIBID_DEFALERTLib>,
	public IObjectSafetyImpl<CSymNavDefAlert, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA>,
	public CScriptSafe<CSymNavDefAlert>  // For SafeScript
{
public:
	CSymNavDefAlert();

DECLARE_REGISTRY_RESOURCEID(IDR_SYMNAVDEFALERT)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CSymNavDefAlert)
	COM_INTERFACE_ENTRY(ISymNavDefAlert)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IObjectSafety)
	COM_INTERFACE_ENTRY(IObjectWithSite)  // For SafeScript
	COM_INTERFACE_ENTRY(ISymScriptSafe)   // Allow programmatic enabling/disabling of safety check (optional)
END_COM_MAP()

// ISymNavDefAlert
public:
	STDMETHOD(get_ShowSSC)(VARIANT_BOOL* pVal);
	STDMETHOD(IsHighContrast)(BOOL* pbRet);
	STDMETHOD(get_SubExpiredOptionEnabled)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(SetDefsOutOfDateCycle)(/*[in]*/ long dwDays);
	STDMETHOD(SetSubExpiredCycle)(/*[in]*/ long dwDays);
	STDMETHOD(LaunchLiveUpdate)();
	STDMETHOD(get_NAVError)(/*[out, retval]*/ INAVCOMError* *pVal);
	STDMETHOD(SetAlertDword)(BSTR bstrName, LONG iValue);
	STDMETHOD(GetAlertDword)(BSTR bstrName, LONG iDefaultValue, LONG* iValue);
	STDMETHOD(LaunchURLFromBrandingResource)(LONG iResId);
	STDMETHOD(GetStringFromBrandingResource)(LONG iResId, BOOL bRemoveAmpersand, BSTR* pbstrBuffer);
	STDMETHOD(GetVirusDefSubDaysLeft)(LONG* pnDaysLeft);
	STDMETHOD(get_VirusDefsThreatCount)(LONG* pVal);
	STDMETHOD(WriteToWMI)(/*[in]*/ VARIANT_BOOL pVal);

private:
	CComPtr <INAVCOMError> m_spError;
	// Are we asked to force this error to occur?
	//
	bool forceError( long lErrorID );

	// Setup error info to be display in a CED
	// lNAVErrorResID identifies error string in NAVError.dll
	void makeError ( long lMessageID, long HResult, long lNAVErrorResID );

	// This method displays error message and returns the errCode.
	void DisplayError(HRESULT errCode, LPCTSTR errMsg);
public:
};

#endif //__SYMNAVDEFALERT_H_
