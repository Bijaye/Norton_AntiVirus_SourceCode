// SnoozeAlertObj.h : Declaration of the CSnoozeAlert

#pragma once
#include "resource.h"       // main symbols
#include <ObjSafe.h>
#include <atlctl.h>
#include "NAVOptions.h"
#include "NavTrust.h"

// CSnoozeAlert
//
// --
// Due to design change, this object will not be called from a script anymore.
// However, we are leaving it IDispatch to avoid NAV installer modification.
// Just removing the ScriptSafe implementation and adding trust verification.

class ATL_NO_VTABLE CSnoozeAlertObj : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSnoozeAlertObj, &CLSID_SnoozeAlertObj>,
    public IDispatchImpl<ISnoozeAlert, &IID_ISnoozeAlert>
//	public CScriptSafe<CSnoozeAlertObj>,						  // For SafeScript
//	public IObjectSafetyImpl<CSnoozeAlertObj, INTERFACESAFE_FOR_UNTRUSTED_DATA | INTERFACESAFE_FOR_UNTRUSTED_CALLER>
{
public:
	CSnoozeAlertObj()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SNOOZEALERT)

BEGIN_COM_MAP(CSnoozeAlertObj)
	COM_INTERFACE_ENTRY(ISnoozeAlert)
	COM_INTERFACE_ENTRY(IDispatch)
//	COM_INTERFACE_ENTRY(IObjectSafety)
//	COM_INTERFACE_ENTRY(IObjectWithSite)  // For SafeScript
//	COM_INTERFACE_ENTRY(ISymScriptSafe)   // Allow programmatic enabling/disabling of safety check
END_COM_MAP()

BEGIN_CATEGORY_MAP(CSnoozeAlertObj)
//	IMPLEMENTED_CATEGORY(CATID_SafeForScripting)
//	IMPLEMENTED_CATEGORY(CATID_SafeForInitializing)
END_CATEGORY_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		if (NAVToolbox::IsSymantecApplication() != NAVToolbox::NAVTRUST_OK)
			return E_ACCESSDENIED;

		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

public:

	STDMETHOD(Snooze)(long hParentWnd, SnoozeFeature snoozeFeatures, BOOL bCanTurnOffAP, BOOL* pResult);
	STDMETHOD(SetSnoozePeriod)(SnoozeFeature snoozeFeatures, long lValue);
	STDMETHOD(GetSnoozePeriod)(SnoozeFeature snoozeFeature, long* pValue);
};

OBJECT_ENTRY_AUTO(__uuidof(SnoozeAlertObj), CSnoozeAlertObj)
