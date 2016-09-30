////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

	
// NAVOptionsObj.h : Declaration of the CNAVOptions

#ifndef __NAVOPTIONSOBJ_H_
#define __NAVOPTIONSOBJ_H_

#include <ObjSafe.h>
#include <atlctl.h>
#include "Resource.h"       // main symbols
#include "SymHelpLauncher.h"
#include "NAVSettingsCache.h"
#include "NAVDispatch.h"
#include "EmailProxyInterface.h"  // For Email options
#include "IWPOptions.h"
#include "SymProtectControlHelper.h"
#include "AVInterfaces.h"	// All AV Component interfaces
#include "ExclusionInterface.h"
#include "AVInterfaceLoader.h"
#include "EmailOptions.h"
#include "ThreatCatOptions.h"
#include "IMOptions.h"
#include "SrtExclusionsInterface.h"
#include "SnoozeDialogInterface.h"
#include "SnoozeDialogLoader.h"
#include "uiNISDataElementGuids.h"
#include "ALUWrapper.h"
#include "APExtWrapper.h"	// AP Extension Exclusion helper class
#include "APSRTSPLoader.h"	// AP Exclusion DLL Loader
#include "NavOptionsConstants.h"
#include "ccStringConvert.h"
#include "SymTheme.h"		// Eeeeevil.

#ifndef SYM_WIN32         // When not building in the build system, we need
#define SYM_WIN32         // to define these symbols manually.
#endif

const _MAX_PROPERTIES_ = 1;

typedef ::std::map< ::std::basic_string<WCHAR>, int > OPTYPEMAP;

// Structure used to hold AP Exclusions internally
typedef struct
{
    DWORD dwFlags;
    WCHAR *szExclusion;
}APEXCLUSIONITEM, *PAPEXCLUSIONITEM;

// Structure used to hold Threat category exclusions internally
typedef struct
{
    BOOL bSubFolders;
    tstring tstrPath;
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
		HRESULT Default(const wchar_t* pcszDefault);
		virtual bool    IsDirty(void);
		virtual void    Init(void);
	};

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
   
    CComBSTR m_sbOption, m_sbValue;

	bool m_bIsDirty;    // Was any of the properties changed
    bool m_bIsPasswordDirty;
    bool m_bNavDefsInitialized;

	NAVToolbox::CCSettings m_ccSettings;
    CNAVOptSettingsCache* m_pNavOpts;    // Regular options
    CNAVOptSettingsCache* m_pNavDefs;    // Default options
	
	OPTYPEMAP m_OptionsTypes;  // Data type of the option (string or DWORD)

	// Does the current process has the permissions to modify the registry
	bool     m_bCanAccessRegKey;
    HWND m_MyHwnd;
	CString m_csProductName;
	CString m_csTitle;

	// Is password protection supported?
	// TRUE for XP; FALSE for Vista because Vista already has UAC.
	bool m_bPasswordSupported;

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
    STDMETHOD(get_HWND)(/*[out, retval]*/ long *pVal);
    STDMETHOD(get_IsHighContrastMode)(/*[out, retval]*/ BOOL *pVal);
    STDMETHOD(get_OEMVendor)(/*[out, retval]*/ BSTR *pVal);

	/** Anomaly/VID Exclusions **/
	STDMETHOD(get_AnomalyExclusionCount)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_AnomalyExclusionItemName)(/*[in]*/ long index, /*[out, retval]*/ BSTR *pVal);
    STDMETHOD(get_AnomalyExclusionItemVid)(/*[in]*/ long index, /*[out, retval]*/ unsigned long* pVal);
	STDMETHOD(RemoveAnomalyExclusionItem)(/*[in]*/ long index);
	STDMETHOD(SetAnomalyExclusionDefaults)();
	STDMETHOD(SetAnomalyExclusionItem)(/*[in]*/ unsigned long ulVid, /*[in]*/ BSTR bstrThreatName, /*[out,retval]*/ EExclusionAddResult *peResult);

    /** FS Exclusions **/
    STDMETHOD(SetPathExclusionDefaults)();
    STDMETHOD(RemovePathExclusionItem)(/*[in]*/ long index);
	STDMETHOD(SetPathExclusionItem)(/*[in]*/ long index, /*[in]*/ BSTR bstrPath, /*[in]*/ BOOL bSubFolders, /*[out,retval]*/ EExclusionAddResult *peResult);
	STDMETHOD(get_PathExclusionItemPath)(/*[in]*/ long index, /*[out, retval]*/ BSTR *pVal);
    STDMETHOD(get_PathExclusionItemSubFolder)(/*[in]*/ long index, /*[out, retval]*/ BOOL *pVal);
	STDMETHOD(get_PathExclusionCount)(/*[out, retval]*/ long *pVal);

    /** Generic Exclusions **/
    STDMETHOD(ValidateExclusionPath)(/*[in]*/ BSTR bstrExclusion, /* [out,retval] */ EExclusionValidateResult *pResult);

	/** AP Exclusions **/
	STDMETHOD(get_APExclusionCount)(/*[out,retval*/ unsigned long *pVal);
	STDMETHOD(get_APExclusionPath)(/*[in]*/ long index, /*[out, retval] */ BSTR* pVal);
	STDMETHOD(get_APExclusionSubFolder)(/*[in]*/ long index, /*[out, retval] */ BOOL *pVal);
	STDMETHOD(AddAPExclusion)(/*[in]*/ BSTR bstrPath, /*[in]*/ BOOL bSubFolders, /*[out,retval]*/EExclusionAddResult* index);
	STDMETHOD(EditAPExclusion)(/*[in]*/ long lIndex, /*[in]*/ BSTR bstrPath, /*[in]*/ BOOL bSubFolders, /*[out,retval]*/long* plResult);
	STDMETHOD(DeleteAPExclusion)(/*[in]*/ long index);
	STDMETHOD(SetAPExclusionDefaults)();

	// Other stuff
	STDMETHOD(get_ALUrunning)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(get_IsPasswordDirty)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(put_IsPasswordDirty)(/*[in]*/ BOOL newVal);
	STDMETHOD(ConfigureIM)(/*[in]*/ EIMType IMType);
	STDMETHOD(get_IsIMInstalled)(/*[in]*/ EIMType IMType, /*[out, retval]*/ BOOL *pVal);

    /** AV Component Threat Category Exclusions **/
    STDMETHOD(get_ThreatCategoryEnabled)(/*[in]*/ unsigned long ulCategory, /*[out,retval]*/ BOOL* bEnabled);
    STDMETHOD(put_ThreatCategoryEnabled)(/*[in]*/ unsigned long ulCategory, /*[in]*/ BOOL bEnabled);
    STDMETHOD(DefaultThreatCategoryEnabled) (/*[in]*/ unsigned long ulCategory, /*[out,retval]*/ BOOL *bEnabled);

	/** AV Component Email Options **/
	STDMETHOD(get_EmailScanIncoming)(/*[out,retval]*/BOOL* bEnabled);
	STDMETHOD(put_EmailScanIncoming)(/*[in]*/BOOL bEnabled);
	STDMETHOD(get_EmailScanOutgoing)(/*[out,retval]*/BOOL* bEnabled);
	STDMETHOD(put_EmailScanOutgoing)(/*[in]*/BOOL bEnabled);
	STDMETHOD(get_EmailScanOEH)(/*[out,retval]*/BOOL* bEnabled);
	STDMETHOD(put_EmailScanOEH)(/*[in]*/BOOL bEnabled);
	STDMETHOD(SetEmailPageDefaults)();

// Methods
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

// Properties
	STDMETHOD(get_Version)(/*[out, retval]*/ unsigned long *pulVersion);
	STDMETHOD(get_IsDirty)(/*[out, retval]*/ BOOL *pbIsDirty);
    STDMETHOD(put_IsDirty)(/*[in]*/ BOOL bIsDirty);
	// Components parmaeters
	STDMETHOD(get_Value)(/*[out, retval]*/ BOOL *pbValue) { *pbValue = VARIANT_FALSE; return S_FALSE; }
	STDMETHOD(put_Value)(/*[in]*/ BOOL bValue) { return S_FALSE; }

	STDMETHOD(get_LiveUpdate)(/*[out, retval]*/ BOOL *pbLiveUpdate);
	STDMETHOD(put_LiveUpdate)(/*[in]*/ BOOL bLiveUpdate);
	STDMETHOD(DefaultLiveUpdate)();
	STDMETHOD(get_OfficePlugin)(/*[out, retval]*/ BOOL *pbOfficePlugin);
	STDMETHOD(put_OfficePlugin)(/*[in]*/ BOOL bOfficePlugin);
	// General information properies
	STDMETHOD(get_IsTrialValid)(/*[out, retval]*/ BOOL *pbIsTrialValid);
	STDMETHOD(get_EXCLUSIONS)(/*[out, retval]*/ IDispatch **ppINAVExclusions);
	STDMETHOD(get_SystemMode)(/*[out, retval]*/ long *plSystemMode);
	STDMETHOD(get_NortonAntiVirusPath)(/*[out, retval]*/ BSTR *pbstrNortonAntiVirusPath);
	STDMETHOD(get_NAVError)(/*[out, retval]*/ INAVCOMError* *pVal);
    
    //IWP
    STDMETHOD(get_IWPIsInstalled)(/*[out, retval*/ BOOL *pVal);
    STDMETHOD(get_IWPState)(/*[out, retval*/ long *pVal);
    STDMETHOD(get_IWPCanEnable)(/*[out, retval*/ BOOL *pEnable);
    STDMETHOD(get_IWPUserWantsToTrust)(/*[out, retval*/ BOOL *pVal);
    STDMETHOD(put_IWPUserWantsToTrust)(/*[in]*/ BOOL bUserWantsOn);
    STDMETHOD(get_IWPUserWantsOn)(/*[out, retval*/ BOOL *pVal);
    STDMETHOD(put_IWPUserWantsOn)(/*[in]*/ BOOL bUserWantsOn);
    STDMETHOD(SetWorkingDirIWP)(/*[in]*/ BOOL bIWPDir);
    STDMETHOD(get_IWPFirewallOn(/*[out, retval]*/ BOOL *pOn));
    STDMETHOD(InvokeIWPDialog)(/*[in]*/ unsigned long ulDialog);
	STDMETHOD(SetIWPDefaults)();

    // SymProtect
    STDMETHOD(get_SymProtectEnabled)(/*[out, retval]*/ BOOL *bEnable);
    STDMETHOD(put_SymProtectEnabled)(/*[in]*/ BOOL bEnable);

	// Branding
	STDMETHOD(get_ProductName)(/*[out, retval]*/ BSTR *Name);
	STDMETHOD(get_FeatureName)(/*[out,retval]*/ BSTR *Name);

    // Retrieve IThreatsByVID interface
    STDMETHOD(get_THREATSBYVID)(/*[out, retval]*/ IThreatsByVID **ppThreatsByVID);

    // IM-Related
    STDMETHOD(get_IMEnabled)(/*[in]*/ unsigned long ulImType, /*[out,retval]*/ BOOL *pEnabled);
    STDMETHOD(put_IMEnabled)(/*[in]*/ unsigned long ulImType, /*[in]*/ BOOL bEnable);
	STDMETHOD(DefaultIMEnabled)();

	// AutoProtect
	STDMETHOD(get_APEnabled)(/*[out,retval]*/ BOOL *pEnabled);
	STDMETHOD(put_APEnabled)(/*[in]*/ BOOL bEnabled);
	STDMETHOD(SetAPDefaults)();

	STDMETHOD(get_PasswordSupported)(/*[out, retval*/ BOOL *pVal);
    STDMETHOD(get_Is64Bit)(/*[out,retval]*/ BOOL *pVal);
    STDMETHOD(get_IsVista)(/*[out,retval]*/ BOOL *pVal);

	HRESULT get__TypeOf(LPCWCH bstrOption, long *plType);
	bool CanAccessRegKey(void) { return m_bCanAccessRegKey; }
	HRESULT Default(const wchar_t* pcszDefault, DWORD& dwDefault);
	static HRESULT Error(UINT uiIDS, PTCHAR pszLocation, bool bReturnFalse = true);
	static HRESULT Error(PTCHAR pszLocation, HRESULT hr, bool bReturnFalse = true);

    static bool Compare_ByExclusionName(AVExclusions::IExclusion* pExclusion1, AVExclusions::IExclusion* pExclusion2);

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

private:
    void notify(void);
	PTCHAR  url(void);
    HRESULT registerCommandLines(void);
    bool EnumerateSettings(_NAVSETTINGSMAP *pMap);
	HRESULT SplitOptionName( const CStringW wcsOption, /*out*/CStringW& wcsOptName, /*out*/CStringW& wcsOptGroup );

    // Holds the maximum name and data values for all of the settings
    DWORD m_dwMaxName;
    DWORD m_dwMaxData;

	CComPtr <INAVCOMError> m_spError;

	// ALU
	CALUOptions m_ALUWrapper;

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
    ::std::vector<AVExclusions::IExclusionPtr> m_vFSExclusions;
    ::std::vector<AVExclusions::IExclusionPtr> m_vAnomalyExclusions;

    // Exclusion manager and loader
    AVExclusions::IExclusionManagerPtr m_spExclusionMgr;
    AVModule::AVLoader_IExclusionManager m_ExclusionFactory;

    // Exclusion state variables
    bool m_bExclusionsDirty;
    bool m_bExclusionsLoaded;
	bool m_bAPExclusionsLoaded;

    // Loads/Re-Loads the threat exclusions
    HRESULT LoadExclusions(bool bReload = false);
	HRESULT LoadAPExclusions(bool bReload = false);
	HRESULT UnloadAPExclusions();
	HRESULT ClearAPExclusions();
	typedef ::std::vector<PAPEXCLUSIONITEM> APExclusionList;
	APExclusionList m_APExclusions;
	DWORD m_dwAPExclusionMaxLength;

	// AP Exclusion Extension Helpers
	HRESULT IsExtensionExclusion(PCWSTR pszExclusion, bool& bExtension);
	HRESULT GetExtensionIndex(long lIndex, long& lExtensionIndex);
	HRESULT LoadExtensions();
	HRESULT SaveExtensions();
	CAPExtWrapper m_APExtensions;

    // Synchronization
    StahlSoft::CMutexLock	m_NavOptsObjLockMutex;
	StahlSoft::CSmartHandle m_shNavOptsObjMutex;

    // IWP
    CIWPOptions m_IWPOptions;

    // SymProtect
    CSymProtectControlHelper* m_pSymProtect;
    bool InitSymProtect();
    bool m_bSymProtectEnable;
    bool m_bSymProtectDirty;

    // Help launcher class
    isshared::CHelpLauncher m_Help;

    // Working directory
    TCHAR m_szCurrentWorkingDir[MAX_PATH+1];

    // Initial page
    EShowPageID m_eInitialPageID;

    // IThreatsByVID cached interface
    CComPtr<IThreatsByVID> m_spThreatsByVID;

    // Internal Email Options wrapper
    CEmailOptions m_EmailOptions;

    // Threat Category Exclusions
    CThreatCatOptions m_ThreatCatOptions;

    // Internal IM Scanner Options wrapper
    CIMOptions m_IMOptions;

	// AP Exclusions Object & Loader
	SRTSP::SRTSPLoader_ISrtExclusions m_APExclusionLoader;
	ISrtExclusionsPtr m_spAPExclusions;

	// Snooze
	ISShared::ISDataCl_ISnoozeDialog m_SnoozeDlgLoader;
	ISShared::ISnoozeDialogPtr m_spSnoozeDlg;

	// This vector contains strings, the presence of which indicate
	// that a given setting was set to ON upon loading.
	// This is used to track whether or not a given setting
	// has changed, and thus needs to be checked for Snooze.
	bool m_bEmailIncomingEnabledAtStart, m_bEmailOutgoingEnabledAtStart;
	HRESULT ProcessSnoozeables();	// Checks state of features, calls snooze

	// AVComponent AP Stuff
	bool m_bAPLoaded;
	bool m_bAPEnabled;
	bool m_bAPEnabledAtStart;
	AVModule::IAVAutoProtectPtr m_spAP;
	AVModule::AVLoader_IAVAutoProtect m_APLoader;
	AVModule::IAVMapStrDataPtr m_APOptsMap;
	HRESULT LoadAP();
	HRESULT SetAPOption(LPCWSTR pwszOption, const DWORD dwValue);
	HRESULT GetAPOption(LPCWSTR pwszOption, DWORD &dwValue);

	// SymTheme
    mutable symtheme::ISymSkinWindowPtr m_spSkin;
	HRESULT InitSymTheme();

    // Vista support
    bool m_bWin64;
    bool m_bVista;
};

#endif //__NAVOPTIONSOBJ_H_
