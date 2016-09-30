// ScanTasksObj.h : Declaration of the CNAVScanTasks

#ifndef __ScanTasksObj_h_
#define __ScanTasksObj_h_

#include "Resource.h"       // main symbols
#include "..\NavTasksRes\ResResource.h"
#include "ScanTaskObj.h"
#include "ScriptSafeVerifyTrustCached.h"		// For scripting security
#include "NAVError.h"    // For errors
#include "ccModuleId.h"     // For module ids
#include "TaskWizard.h"     // For NAV Task Wizard

////////////////////////////////////////////////////////////////////// 
// disable warning C4786: symbol greater than 255 character,
// okay to ignore, MSVC chokes on STL templates!
#pragma warning(disable: 4786)

#include <vector>

// Store the data in a vector of std::strings
typedef ::std::vector<INAVScanTask*> ContainerType;

// Use IEnumVARIANT as the enumerator for VB compatibility
typedef VARIANT      EnumeratorExposedType;
typedef IEnumVARIANT EnumeratorInterface;

// Typedef the copy classes using existing typedefs
class _Copy2Variant
{
public:
	static HRESULT copy(VARIANT* pv, INAVScanTask** ps)
	{
		HRESULT hr;

		if (SUCCEEDED(hr = (*ps)->QueryInterface(IID_IUnknown, (void**)&pv->punkVal)))
			pv->vt = VT_UNKNOWN;

		return hr;
	}
	static void init(VARIANT* pv) { ::VariantInit(pv);}
	static void destroy(VARIANT* pv) {VariantClear(pv);}
};


typedef CComEnumOnSTL<EnumeratorInterface, &__uuidof(EnumeratorInterface)
                                         , EnumeratorExposedType
                                         , _Copy2Variant
                                         , ContainerType>         EnumeratorType;

typedef ICollectionOnSTLImpl<INAVScanTasks, ContainerType
                                          , INAVScanTask*
                                          , _CopyInterface<INAVScanTask>
                                          , EnumeratorType> CollectionType;

/////////////////////////////////////////////////////////////////////////////
// CNAVScanTasks
class ATL_NO_VTABLE CNAVScanTasks : public CComObjectRootEx<CComSingleThreadModel>
                                  , public CComCoClass<CNAVScanTasks, &CLSID_NAVScanTasks>
                                  , public IConnectionPointContainerImpl<CNAVScanTasks>
//                                  , public IEnumOnSTLImpl<IEnumVARIANT, &IID_IEnumVARIANT, VARIANT, _Copy2Variant, ContainerType>
                                  , public IDispatchImpl<CollectionType, &IID_INAVScanTasks>
                                  , public IProvideClassInfo2Impl<&CLSID_NAVScanTasks, NULL>
                                  , public IObjectSafetyImpl<CNAVScanTasks, INTERFACESAFE_FOR_UNTRUSTED_DATA
                                                                       | INTERFACESAFE_FOR_UNTRUSTED_CALLER>
                                  , public CScriptSafe<CNAVScanTasks>  // For Script security
{
	struct _TestPtr
	{
		CComQIPtr<INAVScanTask, &IID_INAVScanTask> m_spst;

		_TestPtr(IUnknown *punk) : m_spst(punk) {}
		bool operator()(const INAVScanTask* pst) { return !m_spst ? false : pst == m_spst; }
	};

	struct _TestName
	{
		BSTR m_bstrName;

		_TestName(BSTR bstrName) : m_bstrName(bstrName) {}
		bool operator()(INAVScanTask* pst)
		{
			CComBSTR bstrName;

			return FAILED(pst->get_TaskName(&bstrName)) ? false : !::wcscmp(bstrName, m_bstrName);
		}
	};

	struct _Remove
	{
		void operator()(INAVScanTask* pst)
        {
            if (pst)
                pst->Release(); 
        }
	};

	// MS Scheduler object
	//CComPtr<ITaskScheduler> m_spTaskScheduler;

public:
	CNAVScanTasks(void);
    ~CNAVScanTasks(void);

DECLARE_REGISTRY_RESOURCEID(IDR_SCANTASKS)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CNAVScanTasks)
	COM_INTERFACE_ENTRY(INAVScanTasks)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(IObjectSafety) 
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
	COM_INTERFACE_ENTRY(IObjectWithSite)  // For SafeScript
	COM_INTERFACE_ENTRY(ISymScriptSafe)   // Allow programmatic enabling/disabling of safety check (optional)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CNAVScanTasks)
	IMPLEMENTED_CATEGORY(CATID_SafeForScripting)
	IMPLEMENTED_CATEGORY(CATID_SafeForInitializing)
END_CATEGORY_MAP()

BEGIN_CONNECTION_POINT_MAP(CNAVScanTasks)
END_CONNECTION_POINT_MAP()

	HRESULT FinalConstruct(void);
    HRESULT FinalRelease (void);

// INAVScanTasks
public:
	STDMETHOD(get_NAVError)(/*[out, retval]*/ INAVCOMError* *pVal);
	STDMETHOD(get_TaskIndex)(BSTR* pbstrTaskName, /*[out, retval]*/ long *pVal);
	STDMETHOD(ValidateName)(BSTR* pbstrTaskName);
	STDMETHOD(get_MyComputer)(/*[out, retval]*/ INAVScanTask **ppMyComputer);
	STDMETHOD(Delete)(VARIANT vItem);
	STDMETHOD(ScanMyComputer)();
	STDMETHOD(ScanMyComputerAndWait)();
	STDMETHOD(ScheduleMyComputer)();
	STDMETHOD(DeleteAll)();
	// Properties
	STDMETHOD(get_Item)(/*[in]*/ VARIANT vIndex, /*[out, retval]*/ INAVScanTask** ppItem);
	// Methods
	STDMETHOD(Add)(/*[in]*/ INAVScanTask* pScanTask);
	STDMETHOD(Create)(/*[out, retval]*/ INAVScanTask** ppItem);
	STDMETHOD(Remove)(/*[in]*/ VARIANT vItem);
    STDMETHOD(FullOrQuickScan)();
    STDMETHOD(QuickScan)();

protected:
	HRESULT find(VARIANT vItem, ContainerType::iterator& it);
	void    emptyTaskList(void);
	HRESULT addTasks(const char* pszDir);
    bool    generateFilePath( std::string& strFilePath );
    
    // Remove read-only, truncate, then delete file
    //
    bool deleteFileAgressive(LPCTSTR lpcszFilePath);
    
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
    STDMETHOD(ScanMyComputer)(bool bWait);
};

#endif //__ScanTasks_h_