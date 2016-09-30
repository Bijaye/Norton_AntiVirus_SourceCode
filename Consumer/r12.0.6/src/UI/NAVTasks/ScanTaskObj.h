// ScanTaskObj.h : Declaration of the CNAVScanTask

#ifndef __ScanTaskObj_h_
#define __ScanTaskObj_h_

#include "Resource.h"       // main symbols
#include "..\NavTasksRes\ResResource.h"
#include "ScriptSafeVerifyTrustCached.h"		// For scripting security
#include "NAVError.h"    // For errors
#include "ccModuleId.h"     // For module ids
#include "SymInterfaceLoader.h"

/////////////////////////////////////////////////////////////////////////////
// CNAVScanTask
class ATL_NO_VTABLE CNAVScanTask : public INAVScanTaskCust
                                 , public CComObjectRootEx<CComSingleThreadModel>
                                 , public CComCoClass<CNAVScanTask, &CLSID_NAVScanTask>
                                 , public ISupportErrorInfoImpl<&IID_INAVScanTask>
                                 , public IDispatchImpl<INAVScanTask, &IID_INAVScanTask>
                                 , public IProvideClassInfo2Impl<&CLSID_NAVScanTask, NULL>
                                 , public IObjectSafetyImpl<CNAVScanTask, INTERFACESAFE_FOR_UNTRUSTED_DATA
                                                                        | INTERFACESAFE_FOR_UNTRUSTED_CALLER>
{
	friend class CNAVScanTasks;  // Allow private members access for the collection

public:
	CNAVScanTask();
    ~CNAVScanTask();

DECLARE_REGISTRY_RESOURCEID(IDR_SCANTASK)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CNAVScanTask)
	COM_INTERFACE_ENTRY(INAVScanTask)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(INAVScanTaskCust)
    COM_INTERFACE_ENTRY(IObjectSafety) 
   	COM_INTERFACE_ENTRY(IProvideClassInfo)
    COM_INTERFACE_ENTRY(IProvideClassInfo2) 
    COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CNAVScanTask)
	IMPLEMENTED_CATEGORY(CATID_SafeForScripting)
	IMPLEMENTED_CATEGORY(CATID_SafeForInitializing)
END_CATEGORY_MAP()


// INAVScanTask
public:
	STDMETHOD(get_NAVError)(/*[out, retval]*/ INAVCOMError* *pVal);
	STDMETHOD(DeleteSchedule)();
	STDMETHOD(get_CanSchedule)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(get_CanEdit)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(get_TaskPath)(/*[out, retval]*/ BSTR *pVal);
	// Properties
	STDMETHOD(get_TaskName)(/*[out, retval]*/ BSTR *pbstrTaskName);
	STDMETHOD(put_TaskName)(/*[in]*/ BSTR bstrTaskName);
	STDMETHOD(get_TaskType)(/*[out, retval]*/ long *peTaskType);
	STDMETHOD(get_Scheduled)(/*[out, retval]*/ EScheduled *peScheduled);
	STDMETHOD(get_LastRunTime)(/*[out, retval]*/ DATE *pdLastRunTime);
	// Methods
	STDMETHOD(Scan)(void);
	STDMETHOD(Schedule)(/*[in]*/ BOOL* pbWeekly);
	STDMETHOD(Load)(void);
	STDMETHOD(Save)(void);
	STDMETHOD(Edit)(void);
	STDMETHOD(ScanAndWait)(void);

// INAVScanTaskCust
	// Properties
	STDMETHOD(get_ScanTask)(/*[out, retval]*/ VARIANT *pvScanTask);

    HRESULT FinalConstruct(void);

protected:
	bool renameSchedule ( BSTR bstrOldScheduleName );
	bool showSchedule ();
	void getScheduleName( CComBSTR &bstrScheduleName );
	bool makeSchedule( bool bDefaultWeekly = false );
    CSymInterfaceLoader m_TaskLoader;
    IScanTaskPtr m_pScanTask;

    // Are we asked to force this error to occur?
    //
	bool forceError( long lErrorID );

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

    // implementation for fns that can Wait or not
    STDMETHOD(Scan)(bool bWait);
};

#endif //__ScanTask_h_