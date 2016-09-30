// Scheduler.h : Declaration of the CScheduler

#ifndef __SCHEDULER_H_
#define __SCHEDULER_H_

#include "resource.h"       // main symbols
#include "..\NavTasksRes\ResResource.h"
#include "osinfo.h"
#include "ScriptSafeVerifyTrustCached.h"		// For scripting security
#include "NAVError.h"    // For errors
#include "ccModuleId.h"     // For module ids

/////////////////////////////////////////////////////////////////////////////
// CScheduler
class ATL_NO_VTABLE CScheduler : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CScheduler, &CLSID_Scheduler>,
	public IDispatchImpl<IScheduler, &IID_IScheduler, &LIBID_NAVTASKSLib>,
    public IObjectSafetyImpl<CScheduler, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA>,
    public CScriptSafe<CScheduler>  // For Script security
{
public:
	CScheduler();

    HRESULT FinalConstruct();
    void FinalRelease();
DECLARE_REGISTRY_RESOURCEID(IDR_SCHEDULER)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CScheduler)
	COM_INTERFACE_ENTRY(IScheduler)
	COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IObjectSafety) 
	COM_INTERFACE_ENTRY(IObjectWithSite)  // For SafeScript
	COM_INTERFACE_ENTRY(ISymScriptSafe)   // Allow programmatic enabling/disabling of safety check (optional)
END_COM_MAP()

// IScheduler
public:
	STDMETHOD(get_RunAtStartup)(/*[out, retval]*/ BOOL *pbRunAtStartup);
	STDMETHOD(put_RunAtStartup)(/*[in]*/ BOOL bRunAtStartup);
	STDMETHOD(get_Installed)(/*[out, retval]*/ BOOL *pbInstalled);
	STDMETHOD(put_Running)(/*[in]*/ BOOL bStart);
	STDMETHOD(get_Running)(/*[out, retval]*/ BOOL *pbRunning);
    STDMETHOD(get_NAVError)(/*[out, retval]*/ INAVCOMError* *pVal);    

protected:
	BOOL stopSchedulerNT();
	BOOL startSchedulerNT();
	BOOL startScheduler9x();
	//
    // bRunning - Wait until it's started or stopped (default = running )
    // dwRetries - Number of 100 millisecond retries (default = 1 sec )
    //
    BOOL waitForScheduler ( BOOL bRunning = TRUE /*in*/, DWORD dwRetries = 10 /*in*/ );
    COSInfo m_OSInfo;   // Our Toolbox OSInfo class
	// Display a CED error dialog with specific error message.
	// lMessageID is a resource ID of a the error string in NAVTask.
	//
	void makeError(long lMessageID, long lHResult);

	// Display a CED error dialog with generic message.
	// lNAVErrorResID is a resource ID of a generic error string in NAVError.
	//
	void makeGenericError (long lErrorID, long lHResult, long lNAVErrorResID);
    CComPtr <INAVCOMError> m_spError;
	CString m_csProductName;
};

#endif //__SCHEDULER_H_