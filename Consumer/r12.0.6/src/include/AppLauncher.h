// AppLauncher.h : Declaration of the CAppLauncher

#ifndef __APPLAUNCHER_H_
#define __APPLAUNCHER_H_

#include "resource.h"       // main symbols
#include "NAVInfo.h"        // Toolbox
#include "OSInfo.h"        // Toolbox
#include "ScriptSafeVerifyTrustCached.h"		// For scripting security
#include "NAVError.h"    // For errors
#include "ccModuleId.h"     // For module ids
#include "ccSymCommonClientInfo.h"
#include "ccLogViewerPluginId.h"    // Log viewer category IDs
#include <tchar.h>
#include <stdio.h>

/////////////////////////////////////////////////////////////////////////////
// CAppLauncher
class ATL_NO_VTABLE CAppLauncher : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAppLauncher, &CLSID_AppLauncher>,
	public IDispatchImpl<IAppLauncher, &IID_IAppLauncher, &LIBID_NAVLNCHLib>,
    public IObjectSafetyImpl<CAppLauncher, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA>,
    public CScriptSafe<CAppLauncher>  // For ScriptSafe
{
public:
	CAppLauncher();

// So we don't need to include the resource header or resources.
// This is only used when it is compiled anyway

#ifndef IDR_APPLAUNCHER
#define IDR_APPLAUNCHER 102
#endif

DECLARE_REGISTRY_RESOURCEID(IDR_APPLAUNCHER)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CAppLauncher)
	COM_INTERFACE_ENTRY(IAppLauncher)
	COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IObjectSafety)
    COM_INTERFACE_ENTRY(IObjectWithSite)  // For SafeScript
	COM_INTERFACE_ENTRY(ISymScriptSafe)   // Allow programmatic enabling/disabling of safety check    
END_COM_MAP()

// IAppLauncher
public:
	STDMETHOD(GetBrandingURL)(LONG nResourceId, BSTR* pVal);
	STDMETHOD(LaunchSubscriptionWizard)();
	STDMETHOD(LaunchNAV)();
	STDMETHOD(LaunchWipeInfo)();
	STDMETHOD(LaunchUnEraseWizard)();
	STDMETHOD(get_CanLaunchLiveUpdate)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(LaunchEmailProxy)();
	STDMETHOD(get_IsRescueDiskInstalled)(/*[out, retval]*/ BOOL *pbRDInstalled);
	STDMETHOD(get_IsLiveUpdateInstalled)(/*[out, retval]*/ BOOL *pbLUInstalled);
	STDMETHOD(LaunchRescueDisk)();
	STDMETHOD(LaunchLiveUpdate)();
	STDMETHOD(LaunchQuarantine)();
	STDMETHOD(LaunchHelp)(DWORD dwHelpID);
	STDMETHOD(LaunchActivityLog)();
	STDMETHOD(LaunchURL)(BSTR bstrURL);
	STDMETHOD(LaunchVirusEncyclopedia)(void);
	STDMETHOD(LaunchLiveUpdateAndWait)();

	STDMETHOD(get_NAVError)(/*[out, retval]*/ INAVCOMError* *pVal);

protected:
	// Are we asked to force this error to occur?
	//
	bool forceError( long lErrorID );

	// Setup error info to be displayed in a CED.
	// lNAVErrorResID identifies an error string in NAVError.dll
	void MakeError ( long lMessageID, long HResult, long lNAVErrorResID );
	CNAVInfo    m_NAVInfo;
	COSInfo     m_OSInfo;
	CComPtr <INAVCOMError> m_spError;

    // implementation for Launch fns that can Wait or not
	STDMETHOD(LaunchLiveUpdate)(bool bWait);
};

#endif //__APPLAUNCHER_H_
