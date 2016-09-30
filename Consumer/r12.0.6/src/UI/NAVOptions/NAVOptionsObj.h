	
// NAVOptionsObj.h : Declaration of the CNAVOptions

#ifndef __NAVOPTIONSOBJ_H_
#define __NAVOPTIONSOBJ_H_

#include <ObjSafe.h>
#include <atlctl.h>
#include "Resource.h"       // main symbols
#include "NAVHelpLauncher.h"
#include "NAVSettingsCache.h"
#include "NAVDispatch.h"
#include "NetDetectController.h"  // LiveUpdate
#include "ScriptSafeVerifyTrustCached.h"		// For scripting security
#include "ProductRegCom.h"
#include "EmailProxyInterface.h"  // For Email options
#include "IWPPrivateSettingsInterface.h"       // For IWP
#include "IWPPrivateSettingsLoader.h"
#include "SNManager.h"
#include "SymProtectControlHelper.h"
#include "fwui_h.h"
#include "AutoProtectWrapper.h"     // For SAVRT
#include "ExclusionManagerInterface.h"
#include "ExclusionManagerLoader.h"

#ifndef SYM_WIN32         // When not building in the build system, we need
#define SYM_WIN32         // to define these symbols manually.
#endif

const _MAX_PROPERTIES_ = 4;

typedef ::std::map< ::std::basic_string<WCHAR>, int > OPTYPEMAP;

// Structure used to hold AP Exclusions internally
typedef struct
{
    DWORD dwFlags;
    TCHAR szExclusion[MAX_PATH];
}APEXCLUSIONITEM, *PAPEXCLUSIONITEM;

// Structure used to hold Threat category exclusions internally
typedef struct
{
    BOOL bSubFolders;
    TSTRING tstrPath;
}THREATEXCLUSIONITEM, *PTHREATEXCLUSIONITEM;

/////////////////////////////////////////////////////////////////////////////
// CNAVOptions
class ATL_NO_VTABLE CNAVOptions : public CComObjectRootEx<CComSingleThreadModel>
                                , public CComCoClass<CNAVOptions, &CLSID_NAVOptions>
                                , public ISupportErrorInfoImpl<&IID_INAVOptions>
                                , public CNAVDispatchImpl<INAVOptions, &IID_INAVOptions>
                                , public IProvideClassInfo2Impl<&CLSID_NAVOptions, NULL, &LIBID_NAVOPTIONSLib>
                                , public IObjectSafetyImpl<CNAVOptions, INTERFACESAFE_FOR_UNTRUSTED_DATA
                                                                      | INTERFACESAFE_FOR_UNTRUSTED_CALLER>
                                , public CScriptSafe<CNAVOptions>  // For SafeScript
{
	class CSemaphore  // Mitigate access between multiple instences of
	{                 // options dialog making sure only one exists at a time.
		HANDLE m_hSemaphore;
		TCHAR  m_szSemaphore[128];

	public:
		CSemaphore(void);
		~CSemaphore(void);

		HRESULT Create(void);
		bool Open(void);
		DWORD Accuire(void);
		void Release(void);
	};

	class IProperty
	{
		PWCHAR       m_pszName;
	protected:
		CNAVOptions& m_rParent;
	public:
		IProperty(CNAVOptions* pParent, PWCHAR pszName) : m_rParent(*pParent)
		                                                , m_pszName(pszName) {}

		virtual bool    IsDirty(void) = 0;
		virtual HRESULT Load(void) = 0;
		virtual HRESULT Save(void) = 0;
		virtual HRESULT Default(void) = 0;
		virtual void    Init(void) = 0;

		PWCHAR  Name(void) { return m_pszName; }
	} *m_Properties[_MAX_PROPERTIES_];

	// CProperty: Property encapsulation
	class CProperty : public IProperty
	{
	protected:
		enum EState { Prop_Unknown = -1
		            , Prop_False
		            , Prop_True } m_eState
		                        , m_eOldState;

	public:
		CProperty(CNAVOptions* pParent, PWCHAR pszName);

		HRESULT Get(BOOL *pbState);
		HRESULT Put(BOOL bState);
		HRESULT Default(const char* pcszDefault);
		virtual bool    IsDirty(void);
		virtual void    Init(void);
	};
	class CPropertyLiveUpdate : public CProperty
	{
	public:
		CPropertyLiveUpdate(CNAVOptions* pParent) : CProperty(pParent, L"LiveUpdate") {}

		virtual HRESULT Load(void);
		virtual HRESULT Save(void);
		virtual HRESULT Default(void);
	} m_LiveUpdate;
	class CPropertyLiveUpdateMode : public IProperty
	{
		DWORD m_dwMode, m_dwOldMode;

	public:
		CPropertyLiveUpdateMode(CNAVOptions* pParent);

		HRESULT Get(EAutoUpdateMode *peLiveUpdateMode);
		HRESULT Put(EAutoUpdateMode eLiveUpdateMode);
		virtual bool    IsDirty(void);
		virtual void    Init(void);
		virtual HRESULT Load(void);
		virtual HRESULT Save(void);
		virtual HRESULT Default(void);
	} m_LiveUpdateMode;
	class CPropertyOfficePlugin : public CProperty
	{
		HINSTANCE m_hInst;
	public:
		CPropertyOfficePlugin(CNAVOptions* pParent) : CProperty(pParent, L"OfficePlugin"), m_hInst(NULL) {}
		~CPropertyOfficePlugin(void) { if (m_hInst) ::FreeLibrary(m_hInst); } // clean up

		virtual HRESULT Load(void);
		virtual HRESULT Save(void);
		virtual HRESULT Default(void);
	} m_OfficePlugin;
	class CPropertyStartupScan : public CProperty
	{
	public:
		CPropertyStartupScan(CNAVOptions* pParent) : CProperty(pParent, L"StartupScan") {}

		virtual HRESULT Load(void);
		virtual HRESULT Save(void);
		virtual HRESULT Default(void);
	private:
		HRESULT GetAutoExecBatPath(PTCHAR *pszAutoExecBat);
		bool FindCommandLine(PTCHAR pszLine, PTCHAR pszCommand, DWORD& dwLineLength);
	} m_StartupScan;

	// CBuffer: a cleanup utility class
	class CBuffer
	{
		char *m_pszBuffer;
	public:
		CBuffer(int nSize = 0) : m_pszBuffer(nSize ? new char[nSize + 1] : NULL) { if (m_pszBuffer) *m_pszBuffer = '\0'; }
		~CBuffer(void) { if (m_pszBuffer) delete [] m_pszBuffer; }

		HRESULT Convert(const VARIANT& vValue);
		HRESULT Convert(VARIANT *pvValue);
		HRESULT Convert(BSTR& bstrValue);
		operator char*(void) { return m_pszBuffer; }
		char const** operator&(void) { return (char const**)(&m_pszBuffer); }
		bool operator==(const CBuffer& rhs) const;
		bool operator!=(const CBuffer& rhs) const { return !(*this == rhs); }
        char* operator=(LPCSTR pcszOther);
	} m_sbOption
	, m_sbValue;

	bool m_bIsDirty;    // Was any of the properties changed
    bool m_bIsPasswordDirty;
    bool m_bNavDefsInitialized;
    bool m_bIMScanDirty;

	NAVToolbox::CCSettings m_ccSettings;
    CNAVOptSettingsCache* m_pNavOpts;    // Regular options
    CNAVOptSettingsCache* m_pNavDefs;    // Default options

	// Map group names to COM objects that implement that properties group
	typedef ::std::map< ::std::basic_string<WCHAR>, CComPtr<INAVOptionGroup> > OPTGRPMAP;
	OPTGRPMAP m_INAVOptionGroups;
	typedef ::std::vector< ::std::basic_string<WCHAR> > VGROUP;
	VGROUP m_Groups;  // List of the proerties groups
	
	OPTYPEMAP m_OptionsTypes;  // Data type of the option (string or DWORD)
	CComPtr<INDScheduler> m_spNDScheduler; // LiveUpdate
    CComPtr<IluProductReg> m_spProductReg; // LiveUpdate Product Registration object
	// Does the current process has the permissions to modify the registry
	bool     m_bCanAccessRegKey;
    HINSTANCE m_hIMScanDll; // Handle to ccIMScan.dll
	BOOL m_bALURunning;
    HWND m_MyHwnd;
	CString m_csProductName;
	CString m_csTitle;

public:
    
	CNAVOptions(void);
	virtual ~CNAVOptions(void);

DECLARE_REGISTRY_RESOURCEID(IDR_NAVOPTIONS)
DECLARE_GET_CONTROLLING_UNKNOWN()

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CNAVOptions)
	COM_INTERFACE_ENTRY(INAVOptions)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IObjectSafety)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
   	COM_INTERFACE_ENTRY(IProvideClassInfo)
    COM_INTERFACE_ENTRY(IProvideClassInfo2)
	COM_INTERFACE_ENTRY(IObjectWithSite)  // For SafeScript
	COM_INTERFACE_ENTRY(ISymScriptSafe)   // Allow programmatic enabling/disabling of safety check
    COM_INTERFACE_ENTRY_FUNC(IID_NULL, 0, _This)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CNAVOptions)
	IMPLEMENTED_CATEGORY(CATID_SafeForScripting)
	IMPLEMENTED_CATEGORY(CATID_SafeForInitializing)
END_CATEGORY_MAP()

	HRESULT FinalConstruct(void);
	void FinalRelease(void);

// CNAVDispatchImpl
	virtual HRESULT Put(LPCWCH pwcName, DISPPARAMS* pdispparams, UINT* puArgErr);
	virtual HRESULT Get(LPCWCH pwcName, VARIANT* pvValue);
	virtual HRESULT Default(LPCWCH pwcName, EXCEPINFO* pexcepinfo);
	virtual HRESULT StdDefault(ITypeInfo* pTInfo, DISPID dispidMember);

// INAVOptions
public:
    STDMETHOD(get_APCanSetSystemStart)(/*[out, retval]*/ BOOL *pVal);
    STDMETHOD(get_HWND)(/*[out, retval]*/ long *pVal);
    STDMETHOD(get_IsHighContrastMode)(/*[out, retval]*/ BOOL *pVal);
    STDMETHOD(get_OEMVendor)(/*[out, retval]*/ BSTR *pVal);
    /** Anomaly Exclusions **/
    STDMETHOD(SetAnomalyExclusionDefaults)();
    STDMETHOD(RemoveAnomalyExclusionItem)(/*[in]*/ long index);
    STDMETHOD(SetAnomalyExclusionItem)(/*[in]*/ unsigned long ulVid, /*[in]*/ BSTR bstrThreatName, /*in*/ BOOL bSubFolders, /*[out,retval]*/ EExclusionAddResult *peResult);
    STDMETHOD(get_AnomalyExclusionItemPath)(/*[in]*/ long index, /*[out, retval]*/ BSTR *pVal);
    STDMETHOD(get_AnomalyExclusionItemVID)(/*[in]*/ long index, /*[out, retval]*/ unsigned long *pulVID);
    STDMETHOD(get_AnomalyExclusionItemSubFolder)(/*[in]*/ long index, /*[out, retval]*/ BOOL *pVal);
    STDMETHOD(get_AnomalyExclusionCount)(/*[out, retval]*/ long *pVal);
    /** FS Exclusions **/
    STDMETHOD(SetThreatExclusionDefaults)();
    STDMETHOD(RemoveThreatExclusionItem)(/*[in]*/ long index);
	STDMETHOD(SetThreatExclusionItem)(/*[in]*/ long index, /*[in]*/ BSTR bstrPath, /*[out,retval]*/ EExclusionAddResult *peResult);
    STDMETHOD(SetThreatExclusionItemWithSubFolder)(/*[in]*/ long index, /*[in]*/ BSTR bstrPath, /*in*/ BOOL bSubFolders, /*[out,retval]*/ EExclusionAddResult *peResult);
	STDMETHOD(get_ThreatExclusionItemPath)(/*[in]*/ long index, /*[out, retval]*/ BSTR *pVal);
    STDMETHOD(get_ThreatExclusionItemSubFolder)(/*[in]*/ long index, /*[out, retval]*/ BOOL *pVal);
	STDMETHOD(get_ThreatExclusionCount)(/*[out, retval]*/ long *pVal);
    /** Generic Exclusions **/
    STDMETHOD(put_ExclusionType)(/*[in]*/ BOOL bIsViral);
    STDMETHOD(get_ExclusionType)(/*[out, retval]*/ BOOL *pIsViral);
    STDMETHOD(ValidateExclusionPath)(/*[in]*/ BSTR bstrExclusion, /* [out,retval] */ EExclusionValidateResult *pResult);
    STDMETHOD(IsExclusionsDifferent)(/*[in]*/ BSTR bstrExclusion1, /*[in]*/ BSTR bstrExclusion2, /*[out, retval]*/ BOOL *pbIsExclusionsDifferent);

	STDMETHOD(get_ALUrunning)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(get_IsPasswordDirty)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(put_IsPasswordDirty)(/*[in]*/ BOOL newVal);
	STDMETHOD(get_IsExtensionExclusion)(/*[in]*/ BSTR bstrPath, /*[out, retval]*/ BOOL *pVal);
	STDMETHOD(get_APExclusionItemFlags)(/*[in]*/ long index, /*[out, retval]*/ long *pVal);
	STDMETHOD(get_APExclusionItemSubFolders)(/*[in]*/ long index, /*[out, retval]*/ BOOL *pVal);
	STDMETHOD(get_APExclusionItemPath)(/*[in]*/ long index, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(RemoveAPExclusionItem)(/*[in]*/ long index);
	STDMETHOD(get_APExtensionList)(/*[out, retval]*/ BSTR *pbstrList);
	STDMETHOD(put_APExtensionList)(/*[in]*/ BSTR bstrList);
	STDMETHOD(SetAPExclusionItem)(/*[in]*/ long index, /*[in]*/ BSTR bstrPath, /*[in]*/ BOOL bSubFolders, /*[in]*/ long Flags, /*[out, retval]*/ EExclusionAddResult *peResult);
	STDMETHOD(DefaultAPExclusions)();
	STDMETHOD(get_APExclusionCount)(/*[out, retval]*/ long *pNumExclusions);
	STDMETHOD(get_IsAluOn)(/*[in]*/ BSTR bstrProductNameSubString, /*[out, retval]*/ EALUonResult *result);
	STDMETHOD(ConfigureIM)(/*[in]*/ EIMType IMType);
	STDMETHOD(EnableALU)(/*[in]*/ BOOL bEnable, /*[in]*/ BOOL bProduct);
	STDMETHOD(get_IsIMInstalled)(/*[in]*/ EIMType IMType, /*[out, retval]*/ BOOL *pVal);
	STDMETHOD(get_IsNAVPro)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(ModifyNPRB)();
	STDMETHOD(ModifyRecycleBin)();
// Methods
	STDMETHOD(Snooze)(/*[in]*/ long hParentWnd, /*[in]*/ SnoozeFeature snoozeFeatures, /*[in]*/ BOOL bCanTurnOffAP, /*[out, retval]*/ BOOL *pResult);
	STDMETHOD(SetSnoozePeriod)(/*[in]*/ SnoozeFeature snoozeFeatures, /*[in]*/ long lValue);
	STDMETHOD(GetSnoozePeriod)(/*[in]*/ SnoozeFeature snoozeFeature, /*[out, retval]*/ long* pValue);
	STDMETHOD(Show)(/*[in]*/ long hWnd);
    STDMETHOD(ShowSpywarePage)(/*[in]*/ long hWnd);
    STDMETHOD(get_InitialPageSpyware)(/*[out, retval]*/ BOOL *pbInitialSpyware);
    STDMETHOD(get_InitialPage)(/*[out, retval]*/ EShowPageID *peShowPageID);
    STDMETHOD(ShowPage)(/*[in]*/ long hWnd, /*[in]*/ EShowPageID eShowPageID);
	STDMETHOD(Load)(void);
	STDMETHOD(Save)(void);
	STDMETHOD(Get)(/*[in]*/ BSTR bstrOption, /*[in]*/ VARIANT vDefault, /*[out, retval]*/ VARIANT *pvValue);
	STDMETHOD(Put)(/*[in]*/ BSTR bstrOption, /*[in]*/ VARIANT vValue);
	STDMETHOD(Help)(/*[in]*/ long iID);
	STDMETHOD(Default)(void);
	STDMETHOD(Browse)(/*[in]*/ EBrowseType eBrowseType, /*[in]*/ BSTR bstrInPath
	                                                  , /*[out, retval]*/ BSTR *pbstrOutPath);
	STDMETHOD(ValidatePath)(/*[in]*/ EBrowseType eBrowseType, /*[in]*/ BSTR bstrPath
	                                                        , /*[out, retval]*/ BOOL *pbValid);
	STDMETHOD(ANSI2OEM)(/*[in]*/ BSTR bstrANSI, /*[out, retval]*/ BSTR *pbstrOEM);
	STDMETHOD(OEM2ANSI)(/*[in]*/ BSTR bstrOEM, /*[out, retval]*/ BSTR *pbstrANSI);
// Properties
	STDMETHOD(get_Version)(/*[out, retval]*/ unsigned long *pulVersion);
	STDMETHOD(get_IsDirty)(/*[out, retval]*/ BOOL *pbIsDirty);
    STDMETHOD(put_IsDirty)(/*[in]*/ BOOL bIsDirty);
	// Components parmaeters
	STDMETHOD(get_Value)(/*[out, retval]*/ BOOL *pbValue) { *pbValue = VARIANT_FALSE; return S_FALSE; }
	STDMETHOD(put_Value)(/*[in]*/ BOOL bValue) { return S_FALSE; }

	STDMETHOD(get_LiveUpdate)(/*[out, retval]*/ BOOL *pbLiveUpdate);
	STDMETHOD(put_LiveUpdate)(/*[in]*/ BOOL bLiveUpdate);
	STDMETHOD(get_LiveUpdateMode)(/*[out, retval]*/ EAutoUpdateMode *peLiveUpdateMode);
	STDMETHOD(put_LiveUpdateMode)(/*[in]*/ EAutoUpdateMode eLiveUpdateMode);
	STDMETHOD(get_OfficePlugin)(/*[out, retval]*/ BOOL *pbOfficePlugin);
	STDMETHOD(put_OfficePlugin)(/*[in]*/ BOOL bOfficePlugin);
	STDMETHOD(get_StartupScan)(/*[out, retval]*/ BOOL *pbStartupScan);
	STDMETHOD(put_StartupScan)(/*[in]*/ BOOL bStartupScan);
	// General information properies
	STDMETHOD(get_IsTrialValid)(/*[out, retval]*/ BOOL *pbIsTrialValid);
	STDMETHOD(get_HaveScriptBlocking)(/*[out, retval]*/ BOOL *pbHaveScriptBlocking);
	STDMETHOD(get_EXCLUSIONS)(/*[out, retval]*/ IDispatch **ppINAVExclusions);
	STDMETHOD(get_CanChange)(/*[out, retval]*/ BOOL *pbCanChange);
	STDMETHOD(get_SystemMode)(/*[out, retval]*/ long *plSystemMode);
	STDMETHOD(get_NortonAntiVirusPath)(/*[out, retval]*/ BSTR *pbstrNortonAntiVirusPath);
	STDMETHOD(get_NAVError)(/*[out, retval]*/ INAVCOMError* *pVal);
    
    //IWP
    STDMETHOD(get_IWPIsInstalled)(/*[out, retval*/ BOOL *pVal);
    STDMETHOD(get_IWPState)(/*[out, retval*/ long *pVal);
    STDMETHOD(get_IWPCanEnable)(/*[out, retval*/ BOOL *pEnable);
    STDMETHOD(get_IWPUserWantsOn)(/*[out, retval*/ BOOL *pVal);
    STDMETHOD(put_IWPUserWantsOn)(/*[in]*/ BOOL bUserWantsOn);
    STDMETHOD(get_AutoBlockEnabled)(/*[out, retval]*/ BOOL *bEnable);
    STDMETHOD(put_AutoBlockEnabled)(/*[in]*/ BOOL bEnable);
    STDMETHOD(SetWorkingDirIWP)(/*[in]*/ BOOL bIWPDir);
    STDMETHOD(get_FWUIPropBag)(/*[out, retval]*/ VARIANT *pvValue);
    STDMETHOD(get_IWPFirewallOn(/*[out, retval]*/ BOOL *pOn));

    // SymProtect
    STDMETHOD(get_SymProtectEnabled)(/*[out, retval]*/ BOOL *bEnable);
    STDMETHOD(put_SymProtectEnabled)(/*[in]*/ BOOL bEnable);

	// Business Rules
	STDMETHOD(SetUseBusinessRules)(/*[in]*/ BOOL bUseBusinessRules);

	// Branding
	STDMETHOD(get_ProductName)(/*[out, retval]*/ BSTR *Name);

    // Retrieve IThreatsByVID interface
    STDMETHOD(get_THREATSBYVID)(/*[out, retval]*/ IThreatsByVID **ppThreatsByVID);

	HRESULT get__TypeOf(LPCWCH bstrOption, long *plType);
	HRESULT InitNDScheduler(void);
    HRESULT InitProductReg(void);
	CComPtr<INDScheduler>& GetNDScheduler(void) { return m_spNDScheduler; } // LiveUpdate
    CComPtr<IluProductReg>& GetProductReg(void) { return m_spProductReg; } // LiveUpdate
    BOOL GetLUMonikerFromProductNameSubString(LPCTSTR pcszSubstr, /*out*/BSTR bstrMonikers[3]);
	bool CanAccessRegKey(void) { return m_bCanAccessRegKey; }
	HRESULT Default(const char* pcszDefault, DWORD& dwDefault);
	static HRESULT Error(UINT uiIDS, PTCHAR pszLocation, bool bReturnFalse = true);
	static HRESULT Error(PTCHAR pszLocation, HRESULT hr, bool bReturnFalse = true);
    bool IsEmailOption( const BSTR bstrOptName, /*out*/BSTR* pbstrOptName );
    bool IsAPOption( LPCWCH pwcOptionGroupName );

    static bool Compare_ByExclusionName(NavExclusions::IExclusion* pExclusion1, NavExclusions::IExclusion* pExclusion2);

	// Setup error info to be displayed in a CED.
	// lErrorID identifies an error string in NAVOptsRes.dll
	void MakeOptionsError (long lErrorID, long lHResult);

	// Setup error info to be displayed in a CED.
	// lNAVErrorResID identifies an error string in NAVError.dll.
	//  This function is used to display generic error string message.
	void MakeError (long lErrorID, long lHResult, long lNAVErrorResID);
    // Are we asked to force this error to occur?
    //
	bool forceError( long lErrorID );
    IEmailOptions* m_pEmailOptions; // Pointer to ccEmailPxy Options

	void SendSettingsEvent(PCTSTR pszFile);

private:
	DWORD m_dwSBEnabled;
	HRESULT UpdateSBEnabled(void);

    void notify(void);
	HRESULT exception(UINT iID, EXCEPINFO* pexcepinfo);
	PTCHAR  url(void);
	HRESULT restore(PCTSTR pszFile);
    HRESULT initializeSAVRTDLL(void);
    HRESULT registerCommandLines(void);
    HRESULT initializeCCEmailPxy(void);
    HRESULT restoreLicensingState(void);
    bool EnumerateSettings(_NAVSETTINGSMAP *pMap);
    // Holds the maximum name and data values for all of the settings
    DWORD m_dwMaxName;
    DWORD m_dwMaxData;

    //
    // Internal functions and variables for handling AP Exclusions
    //
    typedef ::std::vector<APEXCLUSIONITEM> APEXCLUSIONVECT; 
    APEXCLUSIONVECT m_vAPExclusions; // Internal storage for the AP Exclusions
	HANDLE m_hAPExclusions; // Handle to the AP Exclusions
    HRESULT BinaryToString(LPCBYTE,LPTSTR,DWORD);
    HRESULT StringToBinary(LPCTSTR,LPBYTE,DWORD&);
    HRESULT GetExtList(LPCTSTR pcszOptName, LPTSTR pszList);
    HRESULT SetExtList(LPCTSTR pcszOptName, LPTSTR pszList);
    bool IsExtensionExclusion(LPCTSTR);
    HRESULT initializeAPExclusions(void);
    HRESULT reloadAPExclusions(void);
    HRESULT APExcl_ReplacePathWithPath(int nIndexToReplace, APEXCLUSIONITEM NewExclusion, EExclusionAddResult *peResult);
    HRESULT APExcl_ReplacePathWithExt(int nIndexToReplace, APEXCLUSIONITEM NewExclusion, EExclusionAddResult *peResult);
    HRESULT APExcl_ReplaceExtWithExt(int nIndexToReplace, APEXCLUSIONITEM NewExclusion, EExclusionAddResult *peResult);
    HRESULT APExcl_ReplaceExtWithPath(int nIndexToReplace, APEXCLUSIONITEM NewExclusion, EExclusionAddResult *peResult);
    HRESULT APExcl_AddNewExt(APEXCLUSIONITEM NewExclusion, EExclusionAddResult *peResult, bool bAddToInternalList);
    HRESULT APExcl_AddNewPath(APEXCLUSIONITEM NewExclusion, EExclusionAddResult *peResult, bool bAddToInternalList);
    HRESULT APExcl_RemoveItem(int nIndex, bool bRemoveInternal);

	void warning(UINT uiIDS);;
	HRESULT convert(BSTR bstrRAW, BSTR *pbstrCONVERTED, BOOL(WINAPI*fnConvert)(LPCTSTR, LPSTR));
	CComPtr <INAVCOMError> m_spError;
    HINSTANCE m_hccEmlPxy;  // Handle for ccEmlPxy.dll to get the email object
    HINSTANCE m_hSavrRTDll; // Handle for Savrt32.dll to get AP options
	// Need variables to store the current state of the advanced email options
	// since we don't want to write them out until the options file is saved
	bool m_bAdvEmlOptsDirty;
    bool m_bEmailDirty;
	bool m_bEmlTimeout;
	bool m_bEmlTray;
	bool m_bEmlProgress;

	HANDLE m_hALUMutex;

    CAutoProtectOptions m_SavrtOptions;

    // Function pointers for savrt32.dll
    // Exclusions
    pfn_SAVRT_EXCLUSION_OPEN APExclusionOpen;
    pfn_SAVRT_EXCLUSION_CLOSE APExclusionClose;
    pfn_SAVRT_EXCLUSION_GETNEXT APExclusionGetNext;
    pfn_SAVRT_EXCLUSION_MODIFY_ADD APExclusionModifyAdd;
    pfn_SAVRT_EXCLUSION_MODIFY_REMOVE APExclusionModifyRemove;
    pfn_SAVRT_EXCLUSION_MODIFY_EDIT APExclusionModifyEdit;
    pfn_SAVRT_EXCLUSION_SET_DEFAULT APExclusionSetDefault;
    pfn_SAVRT_EXCLUSION_GO_TO_LIST_HEAD APExclusionGoToListHead;
    pfn_SAVRT_EXCLUSION_IS_DIRTY APExclusionIsDirty;

	// This function lets us get to the this pointer from a COM interface pointer
	// It is used in the COM_INTERFACE_ENTRY_FUNC() macro below
	static HRESULT WINAPI _This(void* pv, REFIID iid, void** ppv, DWORD)
	{
#ifdef _ATL_DEBUG_INTERFACES
#  error _ATL_DEBUG_INTERFACES is not allowed!
#endif _ATL_DEBUG_INTERFACES
		ATLASSERT(IID_NULL == iid);
		*ppv = pv;
		return S_OK;
	}

    //
    // Exclusions via IExclusionManager
    //
    // Internal Storage for the Exclusions
    ::std::vector<NavExclusions::IExclusionPtr> m_vFSExclusions;
    ::std::vector<NavExclusions::IExclusionPtr> m_vAnomalyExclusions;

    // Exclusion manager and loader
    NavExclusions::IExclusionManagerPtr m_spExclusionMgr;
    NavExclusions::N32Exclu_IExclusionManagerFactory m_ExclusionFactory;

    // Exclusion state variables
    bool m_bExclusionsAreViral;
    bool m_bExclusionsDirty;
    bool m_bExclusionsLoaded;

    // Loads/Re-Loads the threat exclusions
    HRESULT LoadExclusions(bool bReload = false);

    // Synchronization
    StahlSoft::CMutexLock	m_NavOptsObjLockMutex;
	StahlSoft::CSmartHandle m_shNavOptsObjMutex;

    // IWP
    IWP::IIWPPrivateSettingsPtr m_pIWPSettings;
    bool m_bIWPDirty;
    CSNManager* m_pSNMgr;
    IVarBstrCol* pFWUIPropBag;

    // SymProtect
    CSymProtectControlHelper* m_pSymProtect;
    bool InitSymProtect();
    bool m_bSymProtectEnable;
    bool m_bSymProtectDirty;

    // Help launcher class
    NAVToolbox::CNAVHelpLauncher m_Help;

    // Working directory
    TCHAR m_szCurrentWorkingDir[MAX_PATH+1];

    // Initial page
    EShowPageID m_eInitialPageID;

    // IThreatsByVID cached interface
    CComPtr<IThreatsByVID> m_spThreatsByVID;
    CComPtr<ISymScriptSafe> m_spSymScriptSafeThreatsByVID;
};
static const LPCTSTR NavEmailSettingsKeyName	=	"Norton Antivirus\\NAVOPTS.DAT\\NAVEMAIL";
static const LPCTSTR IMScanSettingsKeyName     =	"Norton Antivirus\\navopts.dat\\IMSCAN";


#endif //__NAVOPTIONSOBJ_H_
