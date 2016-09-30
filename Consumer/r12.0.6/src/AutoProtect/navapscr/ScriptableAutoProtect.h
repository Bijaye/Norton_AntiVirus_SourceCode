// ScriptableAutoProtect.h : Declaration of the CScriptableAutoProtect

#ifndef __SCRIPTABLEAUTOPROTECT_H_
#define __SCRIPTABLEAUTOPROTECT_H_

#include "resource.h"       // main symbols
#import "navapsvc.tlb"      // AP COM interface
#include "navapcommands.h"  // COM commands
#include "NAVInfo.h"        // Toolbox
#include "OSInfo.h"
#include "ScriptSafeVerifyTrustCached.h"		// For scripting security
#include "NAVError.h"       // For errors
#include "ccModuleId.h"     // For module ids
#include "NavSettings.h"
#include "AutoProtectWrapper.h"

/////////////////////////////////////////////////////////////////////////////
// CScriptableAutoProtect
class ATL_NO_VTABLE CScriptableAutoProtect : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CScriptableAutoProtect, &CLSID_ScriptableAutoProtect>,
	public IDispatchImpl<IScriptableAutoProtect, &IID_IScriptableAutoProtect, &LIBID_NAVAPSCRLib>,
	public ISupportErrorInfoImpl<&IID_IScriptableAutoProtect>,
    public IObjectSafetyImpl<CScriptableAutoProtect, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA>,
    public CScriptSafe<CScriptableAutoProtect>  // For SafeScript

{
public:
	CScriptableAutoProtect();
    virtual ~CScriptableAutoProtect();

DECLARE_REGISTRY_RESOURCEID(IDR_SCRIPTABLEAUTOPROTECT)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CScriptableAutoProtect)
	COM_INTERFACE_ENTRY(IScriptableAutoProtect)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
    COM_INTERFACE_ENTRY(IObjectSafety)
	COM_INTERFACE_ENTRY(IOleObject)  // For SafeScript
	COM_INTERFACE_ENTRY(IObjectWithSite)
	COM_INTERFACE_ENTRY(ISymScriptSafe)   // Allow programmatic enabling/disabling of safety check    
END_COM_MAP()


// IScriptableAutoProtect
public:
	STDMETHOD(get_NAVError)(/*[out, retval]*/ INAVCOMError* *pVal);
	STDMETHOD(Configure)(/*[in]*/ BOOL bStartup);
	STDMETHOD(Load)();
	STDMETHOD(get_Loaded)(/*[out, retval]*/ BOOL *pAPLoaded);
	STDMETHOD(get_Enabled)(/*[out, retval]*/ BOOL *pbEnabled);
	STDMETHOD(put_Enabled)(/*[in]*/ BOOL bEnabled);
	STDMETHOD(SetSAVRTPELStartMode)(/*[in]*/ int iMode);
    
protected:
	BOOL RegisterAPService();
    bool SetServiceStartupMode(LPCTSTR pcszService, DWORD dwMode);
    void GetAPServiceObject(/*[out]*/NAVAPSVCLib::INAVAPServicePtr& spAPService);
    CNAVInfo m_NAVInfo;
    COSInfo m_OSInfo;

    // Are we asked to force this error to occur?
    //
    bool forceError(long lErrorID);
    // Set error info.
    //  lNAVErrorResID identifies an error string in NAVError.dll
    void MakeError(long lMessageID, long lHResult, long lNAVErrorResID);
    CComPtr <INAVCOMError> m_spError;
	NAVToolbox::CCSettings m_ccSettings;
    CAutoProtectWrapper m_SAVRTDrivers;
    CAutoProtectOptions m_SAVRTOptions;
};

#endif //__SCRIPTABLEAUTOPROTECT_H_
