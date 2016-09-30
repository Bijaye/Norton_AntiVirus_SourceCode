////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// CfgWizMgr.h : Declaration of the CCfgWizMgr

#ifndef __CFGWIZMGR_H_
#define __CFGWIZMGR_H_

#include "resource.h"       // main symbols
#include "navopt32.h"

#include "NAVInfo.h"

#include "ccModuleId.h"     // For module ids

#include "cltLicenseConstants.h"

#include "StahlSoft.h"
#include "SSOSInfo.h" // Looks like someone else, like ITB, creates this object but it's compiled ANSI!!

#include "isErrorIDs.h"       // For errors
#include "isErrorResource.h"       // For errors
#include "CCfgWizimpl.h"

#include "NAVError.h"       // For errors
#include "NAVTrust.h"       // For trusting the NAVError object

// CC lib headers
#include "ccThread.h"
#include "ccEvent.h"

// forward delcaration
class CBackGroundTasks;

// Elements
#include "ISDataClientLoader.h"
#include "uiNISDataElementGuids.h"
#include "uiProviderInterface.h"
#include "uiElementInterface.h"
#include "uiNumberDataInterface.h"

// custom interfaces
#include "isDataNoUIInterface.h"

////////////////////////////////////////////////////////////////////////////////
// CNAVProperties - Product Specific Properties Object 
// This object is the abstraction to your data store for settings. 

class CNAVProperties :
    public CCfgWizProperties
{
public:
    CNAVProperties( LPCTSTR szPath = NULL ) :
        m_bSilentMode(VARIANT_FALSE),
        m_bLaunchLiveUpdate(VARIANT_FALSE),
        m_bRegistered(VARIANT_FALSE),
        m_lOEM(0),
        m_bRegistration(VARIANT_TRUE),
        m_hCfgWizDat(NULL)
    {
        _tcscpy( m_szPath, szPath );

        // Create a handle to CfgWiz.dat
        NAVOPTS32_STATUS Status = NavOpts32_Allocate(&m_hCfgWizDat);
        if (Status != NAVOPTS32_OK)
            throw runtime_error("Unable to initialize options library.");

        TCHAR szDatPath[MAX_PATH] = {0};

        wsprintf(szDatPath, _T("%s\\CfgWiz.dat"), szPath );

        // Load CfgWiz.dat
        Status = NavOpts32_Load(CT2CA(szDatPath), m_hCfgWizDat, TRUE);
        if (Status != NAVOPTS32_OK)
            throw runtime_error("Unable to load CfgWiz.dat.");
    };

    ~CNAVProperties()
    {
        // Clean up
        if (m_hCfgWizDat != NULL)
        {
            NavOpts32_Free(m_hCfgWizDat);
            m_hCfgWizDat = NULL;
        }
    };

    HRESULT GetProperty(LPCTSTR pszProperty, VARIANT_BOOL* pbValue)
    {
        NAVOPTS32_STATUS Status = NAVOPTS32_OK;
        DWORD dwValue = 0;

        Status = NavOpts32_GetDwordValue(m_hCfgWizDat, CT2CA(pszProperty), &dwValue, 0);
        if (Status != NAVOPTS32_OK)
        {
            char szErrorText[MAX_PATH];
            sprintf( szErrorText, "Unable to retrieve property: %s", pszProperty );
            throw runtime_error(szErrorText);
			return E_FAIL;
        }
        *pbValue = (dwValue) ? VARIANT_TRUE : VARIANT_FALSE;

        return S_OK;

    };

    HRESULT PutProperty(LPCTSTR pszProperty, VARIANT_BOOL bValue)
    { 
        DWORD dwSetting = (bValue == VARIANT_TRUE ? 1 : 0);

        NAVOPTS32_STATUS Status = NavOpts32_SetDwordValue(m_hCfgWizDat, CT2CA(pszProperty), dwSetting);

        if(Status != NAVOPTS32_OK)
		{
			char szErrorText[MAX_PATH];
            sprintf( szErrorText, "Unable to set property: %s", pszProperty );
            throw runtime_error(szErrorText);
            return E_FAIL;
		}

        // Save CfgWiz.dat 

        TCHAR szDatPath[MAX_PATH] = {0};

        wsprintf(szDatPath, _T("%s\\CfgWiz.dat"), m_szPath);

        Status = NavOpts32_Save(CT2CA(szDatPath), m_hCfgWizDat);

        if(Status != NAVOPTS32_OK)
            return E_FAIL;


        return S_OK; 
    };

    HRESULT GetProperty(LPCTSTR pszProperty, DWORD* dwValue)
    {
            NAVOPTS32_STATUS Status = NAVOPTS32_OK;

        Status = NavOpts32_GetDwordValue(m_hCfgWizDat, CT2CA(pszProperty), dwValue, 0);
        if (Status != NAVOPTS32_OK)
        {
            char szErrorText[MAX_PATH];
            sprintf( szErrorText, "Unable to retrieve property: %s", pszProperty );
            throw runtime_error(szErrorText);
        }
        return S_OK; 
    };
    
    HRESULT PutProperty(LPCTSTR pszProperty, DWORD dwValue)
	{
        NAVOPTS32_STATUS Status = NavOpts32_SetDwordValue(m_hCfgWizDat, CT2CA(pszProperty), dwValue);

        if(Status != NAVOPTS32_OK)
		{
			char szErrorText[MAX_PATH];
            sprintf( szErrorText, "Unable to set property: %s", pszProperty );
            throw runtime_error(szErrorText);
            return E_FAIL;
		}

        // Save CfgWiz.dat 

        TCHAR szDatPath[MAX_PATH] = {0};

        wsprintf(szDatPath, _T("%s\\CfgWiz.dat"), m_szPath);

        Status = NavOpts32_Save(CT2CA(szDatPath), m_hCfgWizDat);

        if(Status != NAVOPTS32_OK)
            return E_FAIL;


        return S_OK; 
	}

    HRESULT GetProperty(LPCTSTR pszProperty, LPTSTR dwValue, DWORD dwlength){return S_OK; };
    HRESULT PutProperty(LPCTSTR pszProperty, LPCTSTR pszValue, DWORD dwlength){return S_OK; };
private:

    // Options library handle to CfgWiz.dat
    HNAVOPTS32 m_hCfgWizDat;

    // Configuration type (typical or custom)
    VARIANT_BOOL m_bSilentMode;

    // Launch LiveUpdate
    VARIANT_BOOL m_bLaunchLiveUpdate;

    // Launch Main UI
    VARIANT_BOOL m_bLaunchUI;

    // Registered with Symantec
    VARIANT_BOOL m_bRegistered;

    // Is this OEM sku
    long m_lOEM;

    // Enable Registration
    VARIANT_BOOL m_bRegistration;

    TCHAR m_szPath[MAX_PATH];
};

/////////////////////////////////////////////////////////////////////////////
// CNAVCfgWizMgr - Product Specific CfgWizMgr Class responsible 
// This COM object supplies the javascript helper functions that are 
// specific to the product.

class ATL_NO_VTABLE CNAVCfgWizMgr : 
public CComObjectRootEx<CComSingleThreadModel>,
public CComCoClass<CNAVCfgWizMgr, &CLSID_NAVCfgWizMgr>,
public CCfgWizImpl<CNAVCfgWizMgr,INAVCCfgWizSubMgr, &LIBID_NAVCFGWIZDLLLib>
{
    typedef CCfgWizImpl<CNAVCfgWizMgr,INAVCCfgWizSubMgr, &LIBID_NAVCFGWIZDLLLib> _CCfgWiz_Base;
    friend _CCfgWiz_Base;

public:
    CNAVCfgWizMgr();
    virtual ~CNAVCfgWizMgr();

	HRESULT FinalConstruct();

    DECLARE_REGISTRY_RESOURCEID(IDR_NAVCFGWIZMGR)
        DECLARE_NOT_AGGREGATABLE(CNAVCfgWizMgr)
        
        DECLARE_PROTECT_FINAL_CONSTRUCT()
        
        BEGIN_COM_MAP(CNAVCfgWizMgr)
        COM_INTERFACE_ENTRY(IObjectSafety) 
        COM_INTERFACE_ENTRY(INAVCCfgWizSubMgr)
        COM_INTERFACE_ENTRY(IDispatch)
        END_COM_MAP()

        // ICfgWizMgr
public:

	//TODO: Product must implement these
	// Methods
	STDMETHOD(RepairShortcuts)();
	STDMETHOD(LaunchUI)();
	STDMETHOD(LaunchLiveUpdate)();
	STDMETHOD(ReactivateSubscription)();


	STDMETHOD (get_SilentMode)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD (get_AutoLiveUpdate)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD (get_AutoLiveUpdateMode)(/*[out, retval]*/ long *pVal);
	STDMETHOD (get_LiveUpdate)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD (get_OSType)(/*[out, retval]*/ long *pVal);
	STDMETHOD (put_CfgWizCompleted)(/*[out, retval]*/ VARIANT_BOOL newVal);

	STDMETHOD (get_Rescue)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD (get_DefinitionAlert)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD (get_FullSystemScan)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD (get_MainUI)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD (get_Registered)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD (get_NAVError)(/*[out, retval]*/ INAVCOMError* *pVal);
	STDMETHOD (get_EnableAsyncScan)(/*[out, retval]*/ VARIANT_BOOL *pVal);
    STDMETHOD (get_ProductType)(/*[out, retval]*/ long *pVal);

	STDMETHOD (get_bDefaultValueForDisableWD)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD (put_bDefaultValueForDisableWD)(VARIANT_BOOL newVal);
	
	STDMETHOD (get_bShowDisableWDCheckBox)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD (put_bShowDisableWDCheckBox)(VARIANT_BOOL newVal);

	//Product Specific Methods 

	STDMETHOD (Test)(){return S_OK;};
	STDMETHOD (RemoveDefAlertDelay)();
    // Enables all IM clients, MSN has an optional switch
	STDMETHOD (EnableIMScan)(); 
	STDMETHOD (LaunchRescue)();
	STDMETHOD (LaunchSystemScan)();
	STDMETHOD (LaunchLiveReg)(VARIANT_BOOL bUIMode);
	STDMETHOD (SetSSARunKey)();
	STDMETHOD (get_StartIWP)(VARIANT_BOOL *pVal);
	STDMETHOD (get_IWPInstalled)(VARIANT_BOOL *pVal);
	STDMETHOD (StartSymTDI)();

	STDMETHOD (get_TotalBrandingText)(LONG* pnTotalText);
	STDMETHOD (GetBrandingText)(LONG nTextId, BSTR* pbstrText);
	STDMETHOD (get_ShowBrandingLogo)(VARIANT_BOOL* pVal);
	STDMETHOD (get_AgreedToEULA)(VARIANT_BOOL* pVal);
	STDMETHOD (put_AgreedToEULA)(VARIANT_BOOL bVal);
	STDMETHOD (get_NortonAntiVirusPath)(BSTR *pbstrNortonAntiVirusPath);
	STDMETHOD (AutoStartLCService)();
	STDMETHOD (WaitForLicenseData)(VARIANT_BOOL *pVal);
	STDMETHOD (RunBackGroundTasks)(VARIANT_BOOL *pVal);
	
	STDMETHOD (get_IsAggregateProduct) (VARIANT_BOOL *pVal);
	STDMETHOD(SignalPreCfgwizUI)();
    
    friend class CBackGroundTasks;

private:
	const TCHAR* GetProductDir();
	HRESULT ValidateFunctionSafety(int iResID);
	HRESULT AddCfgWizTask(const TCHAR* pszTaskName, const TCHAR* pszTaskValue);
	HRESULT GetSubscriptionInfo();

	//TODO: Product must implement these
	bool forceError( long lErrorID );

	// Display a CED with cfgwiz error message.
	// lNAVErrorResID identifies an error string in NAVError.dll
	void makeError ( long lMessageID, long HResult);

	// Display a CED with generic error message.
	// lNAVErrorResID identifies an error string in NAVError.dll
	void makeGenericError ( long lMessageID, long HResult, long lNNAVErrorResID );

    StahlSoft::CSmartPtr<CNAVProperties> m_Properties;

	CComPtr <INAVCOMError> m_spError;

	DWORD m_dwSetting;
	CNAVInfo m_NAVInfo;
	StahlSoft::CSmartModuleHandle m_shAVRES;

	DWORD m_dwSubDaysRemaining;
	bool m_bInitSubscription;

    CBackGroundTasks* m_pBackGroundTasksThread;
};

class CBackGroundTasks : public ccLib::CThread 
{
public:
	CBackGroundTasks() : m_pCfgWizMgr(0)
	{
        m_hBackGroundTasksEvent.Create(NULL, FALSE, FALSE, _T("NAV_CFGWIZ_BACKGROUNDTAKS_DONE"), FALSE);
		m_Options.m_eCOMModel = ccLib::CCoInitialize::eSTAModel;
	}

	virtual ~CBackGroundTasks()
	{
		Terminate(5000);
	}

	void Init(CNAVCfgWizMgr* cfgWizMgr)
	{
		m_pCfgWizMgr = cfgWizMgr;
	}

	int Run();

private:
    CNAVCfgWizMgr*	m_pCfgWizMgr;
    ccLib::CEvent	m_hBackGroundTasksEvent;
};


#endif //__CFGWIZMGR_H_
