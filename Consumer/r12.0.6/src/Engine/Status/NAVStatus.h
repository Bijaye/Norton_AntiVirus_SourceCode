	
// NAVStatus.h : Declaration of the CNAVStatus

#ifndef __NAVSTATUS_H_
#define __NAVSTATUS_H_

#include "resource.h"       // main symbols
#include "status.h"                 // IDL compiled
#include "StatusCP.h"
#include "StatusAutoProtect.h"      // For AP status item
#include "StatusVirusDefs.h"        // For Virus defs status item
#include "StatusAutoLiveUpdate.h"   // For Automatic LiveUpdate status item
#include "StatusEmail.h"            // For Email status item
#include "StatusFullSystemScan.h"   // For Full System scan status item, aka My Computer
#include "StatusLicensing.h"        // For Licensing status item
#include "StatusIWP.h"              // For Internet Worm Protection
#include "StatusSpyware.h"          // For Spyware categories
#include "ScriptSafeVerifyTrustCached.h"		// For scripting security
#include "NAVError.h"               // For errors
#include "ccModuleId.h"             // For module ids - for NAVError
#include "IEUtils.h"                // For getting IE version for cheesy hack.
#include "SyncQueue.h"
#include "RequestStatusThread.h"    // For requesting status async.

#include "ccGIT.h"
#include "comdef.h"

#include "ccLib.h"                  // For critical section
#include "EMSubscriber.h"           // Event Manager subscriber

#include <map>
typedef std::map <long /*Event ID*/, CStatusItem*> mapStatusItems;
typedef mapStatusItems::iterator iterStatusItems;

/////////////////////////////////////////////////////////////////////////////
// CNAVStatus
class ATL_NO_VTABLE CNAVStatus : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CNAVStatus, &CLSID_NAVStatus>,
	public IConnectionPointContainerImpl<CNAVStatus>,
	public IDispatchImpl<INAVStatus, &IID_INAVStatus, &LIBID_STATUSLib>,
	public CProxy_INAVStatusEvents< CNAVStatus >,
    public IProvideClassInfo2Impl<&CLSID_NAVStatus, NULL, &LIBID_STATUSLib>, 
    public IObjectSafetyImpl<CNAVStatus, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA>,
    public CScriptSafe<CNAVStatus>,  // For SafeScript
    public CEMSubscriberSink,        // For listening for updates
    public CSyncQueue<ccEvtMgr::CEventEx*>, // For updating items
    public CRequestStatusThreadSink // For request status async
{
public:
	CNAVStatus();
    virtual ~CNAVStatus();

DECLARE_REGISTRY_RESOURCEID(IDR_NAVSTATUS)
DECLARE_GET_CONTROLLING_UNKNOWN()

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CNAVStatus)
	COM_INTERFACE_ENTRY(INAVStatus)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
   	COM_INTERFACE_ENTRY(IProvideClassInfo)
    COM_INTERFACE_ENTRY(IProvideClassInfo2) 
    COM_INTERFACE_ENTRY(IObjectSafety) 
    COM_INTERFACE_ENTRY(IObjectWithSite)  // For SafeScript
	COM_INTERFACE_ENTRY(ISymScriptSafe)   // Allow programmatic enabling/disabling of safety check
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CNAVStatus)
CONNECTION_POINT_ENTRY(DIID__INAVStatusEvents)
END_CONNECTION_POINT_MAP() 

	HRESULT FinalConstruct();
	void FinalRelease();

public:
	STDMETHOD(get_NAVError)(/*[out, retval]*/ INAVCOMError* *pVal);
	STDMETHOD(GetStatus)(BOOL bNotifyChanges);
	STDMETHOD(get_ProductLicenseValid)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(get_LicensingStatus)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_UseLicensing)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(put_UseLicensing)(/*[in]*/ BOOL newVal);
	STDMETHOD(get_EmailCanEnable)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(get_APCanEnable)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(get_ScriptBlockingCanEnable)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(get_ALUCanEnable)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(get_VirusDefSubDate)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_EmailSMTP)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(get_EmailPOP)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(get_QuarStatus)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_FullSystemScanAge)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_FullSystemScanDate)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_FullSystemScanStatus)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_UseFullSystemScan)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(put_UseFullSystemScan)(/*[in]*/ BOOL newVal);
	STDMETHOD(get_VirusDefStatus)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_EmailStatus)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_ScriptBlockingStatus)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_UseEmail)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(put_UseEmail)(/*[in]*/ BOOL newVal);
	STDMETHOD(get_UseScriptBlocking)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(put_UseScriptBlocking)(/*[in]*/ BOOL newVal);
	STDMETHOD(get_RescueDiskLastRunDay)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_RescueDiskLastRunMonth)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_RescueDiskLastRunYear)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_RescueDiskStatus)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_UseRescueDisk)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(put_UseRescueDisk)(/*[in]*/ BOOL newVal);
	STDMETHOD(get_VirusDefSubState)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_VirusDefSubStatus)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_VirusDefSubDaysLeft)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_UseVirusDefSubscription)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(put_UseVirusDefSubscription)(/*[in]*/ BOOL newVal);
	STDMETHOD(get_VirusDefCount)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_QuarFileCount)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_UseQuar)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(put_UseQuar)(/*[in]*/ BOOL newVal);
	STDMETHOD(get_UseALU)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(put_UseALU)(/*[in]*/ BOOL newVal);
	STDMETHOD(get_ALUStatus)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_UseVirusDef)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(put_UseVirusDef)(/*[in]*/ BOOL newVal);
	STDMETHOD(get_UseAP)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(put_UseAP)(/*[in]*/ BOOL newVal);
	STDMETHOD(get_VirusDefAge)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_VirusDefDate)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_APStatus)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_ProductLicenseType)(/*[out, retval]*/ DJSMAR_LicenseType *pVal);
	STDMETHOD(get_ProductLicenseState)(/*[out, retval]*/ DJSMAR00_LicenseState *pVal);
	STDMETHOD(get_ProductLicenseZone)(/*[out, retval]*/ long *pVal);
    STDMETHOD(put_UseIWP)(/*[in]*/ BOOL newVal);
    STDMETHOD(get_IWPStatus)(/*[out, retval]*/ long *pVal);
    STDMETHOD(get_APSpywareStatus)(/*[out, retval]*/ long *pVal);
    STDMETHOD(get_SpywareCategory)(/*[out, retval]*/ long *pVal);
    STDMETHOD(put_UseSpyware)(/*[in]*/ BOOL newVal);
	STDMETHOD(get_SpywareCanEnable)(/*[out, retval]*/ BOOL *pVal);

// CRequestStatusThreadSink
    void OnRequestStatusReceived (CEventData& edNewData);

protected:
    // CSyncQueue
    void processQueue();

	bool m_bIsFirstRefresh;
    ccLib::CCriticalSection m_critSignalScript;
	bool m_bWaitBeforeSignaling;

    BOOL m_bUseVirusDef;
	BOOL m_bUseAP;
    BOOL m_bUseALU;
    BOOL m_bUseVirusDefSub;
    BOOL m_bUseEmail;
    BOOL m_bUseFullSystemScan;
	BOOL m_bUseLicensing;
    BOOL m_bUseIWP;
    BOOL m_bUseSpyware;

    BOOL m_bNotification;
	bool m_bInit;

	// false = the server isn't up
    bool init ();
    void OnNotifyClient ();
    mapStatusItems m_mapStatusItems;
   
    HRESULT getLong (long lName, long* lReturn);
    HRESULT getString (long lName, BSTR* bstrReturn);

    // Are we asked to force this error to occur?
    //
	bool forceError( long lErrorID );
    HRESULT makeError(long lMessageID, long lHResult, long lErrorResID);
    CComPtr <INAVCOMError> m_spError;

    std::auto_ptr<CRequestStatusThread> m_pRequestStatusThread;

    // EMSubscriber
    //
    // New event arrived. Make a copy of it if you want to process it.
    // The Event Manager is waiting for this call to return, so hurry.
    void EMSubscriberOnEvent (const ccEvtMgr::CEventEx& Event,
                                    ccEvtMgr::CSubscriberEx::EventAction& eAction);
    void EMSubscriberOnShutdown ();
    
    std::auto_ptr<CEMSubscriber> m_pEMSubscriber;

    ccLib::CGIT m_GIT;
    DWORD m_dwCookie;
};

#endif //__NAVSTATUS_H_
