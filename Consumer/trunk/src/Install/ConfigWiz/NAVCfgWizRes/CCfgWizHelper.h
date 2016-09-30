////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// CCfgWizHelper.h: 
//
//////////////////////////////////////////////////////////////////////

#if !defined(CCfgWizHelper_H__95491444_0349_4c59_900B_C85617E714AE__INCLUDED_)
#define CCfgWizHelper_H__95491444_0349_4c59_900B_C85617E714AE__INCLUDED_

// DRM/CfgWiz Headers
#include "ICfgWizHelper.h"
#include "CCfgWizHelperImpl.h"
#include "CfgWizNames.h"

// Licensing Headers
#include "cltLicenseConstants.h"
#include "cltLicenseHelper.h"
#include "V2LicensingAuthGuids.h"

// Misc NAV Headers
#include "resource.h"
#include "cfgwizdat.h"
#include "cfrat.h"
#include "PostCfgWizTasks.h"
#include "NAVOptHelperEx.h"
#include "IWPSettingsInterface.h"
#include "IWPSettingsLoader.h"

// CC lib headers
#include "ccThread.h"
#include "ccEvent.h"

//AV
#include "AVDefines.h"
#include "AVInterfaceLoader.h"

const TCHAR g_cszCfgWizRunKey[] = _T("NAV CfgWiz");

extern HMODULE g_hModule;

#define NAV_WW_WIDTH        760
#define NAV_WW_HEIGHT       530

#import "cfgwiz.tlb" named_guids 

// forward declaration
class CPreBlockThread;

class CCfgWizHelper  :
	public CCfgWizHelperImpl<CCfgWizHelper,&CLSID_CCfgWizHelper>
{

public:
	CCfgWizHelper();
    ~CCfgWizHelper();
    
    HRESULT FinalConstruct();
	void FinalRelease();
	HRESULT Initialize(IDispatch* pDispatch);
	HRESULT RunConditions(CCfgWizDat& CfgWizDat);
    HRESULT SetInstallTime();

	SIMON_INTERFACE_MAP_BEGIN()
		SIMON_INTERFACE_ENTRY(IID_ICfgWizHelper,ICfgWizHelper)
	SIMON_INTERFACE_MAP_END()

	SIMON_STDMETHOD(CfgWizPreBlock)();
	SIMON_STDMETHOD(CfgWizPostBlock)();

    friend class CPreBlockThread;

private:

	_bstr_t                         m_szCommandLine;
	ConfigWizLib::IDRMCollectionPtr m_spColl;    
	StahlSoft::CSmartModuleHandle   m_shSuiteOwner;
	
	IWP_IWPSettings					m_IWPSettingsLoader;
	IWP::IIWPSettings2Ptr			m_pIWPSettings;

	DWORD				m_LicenseType;			
	DWORD			    m_LicenseAttrib;	
	
    DWORD m_dwAggregated;
	CString m_csProductName;

    void StartPreBlockThread();
    HRESULT _CfgWizPreBlock();
    CPreBlockThread* m_pPreBlockThread;

	SYMRESULT InitIWPSettings();
	BOOL NeedReboot();
    void StartAP();
	HRESULT SetIWPEnableStatus(BOOL enable, const DWORD dwOEM);
	HRESULT ScheduleFullSystemScan(BOOL bScheduleWeekly);
	HRESULT SetAutoProtectDefaults();
	SYMRESULT SetPortBlockingFlags();
	bool DisableWindowsFirewall();
};

class CPreBlockThread : public ccLib::CThread 
{
public:
	CPreBlockThread() : m_pCfgWiz(0)
	{
        m_hPreblockEvent.Create(NULL, FALSE, FALSE, _T("NAV_CFGWIZ_PREBLOCK_DONE"), FALSE);
		m_Options.m_eCOMModel = ccLib::CCoInitialize::eMTAModel;
	}

	virtual ~CPreBlockThread()
	{
		Terminate(5000);
	}

	void Init(CCfgWizHelper* cfgWiz)
	{
		m_pCfgWiz = cfgWiz;
	}

	int Run();

private:
    CCfgWizHelper*	m_pCfgWiz;
    ccLib::CEvent	m_hPreblockEvent;
};

#endif // !defined(CCfgWizHelper_H__95491444_0349_4c59_900B_C85617E714AE__INCLUDED_)
