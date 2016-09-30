////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/************************************************************************/
/* NAV Firewall (IWP) Options Object                                    */
/************************************************************************/

#include "stdafx.h"
#include "IWPOptions.h"
#include "NAVSettingsCache.h"

#include "IDSOptions.h"
#include "IDSUIInterface.h"
#include "IDSUILoader.h"

#include "PgmOptions.h"
#include "ProgCtrlUIInterface.h"
#include "ProgCtrlUILoader.h"

#include "RuleUIInterface.h"
#include "RuleUILoader.h"

#include "ccSymModuleLifetimeMgrHelper.h"
#include "resource.h"
#include "../navoptionsres/resource.h"
#include "SNDHelper.h"
#include "ProductType.h"
#include "isDataNoUIInterface.h"
#include "Symhelp.h"

static idsui::IDSExclusionsLoader s_IDSExclusionsUILoader;
static fwui::RuleUILoader s_RuleUILoader;
static ISShared::ISShared_IProvider s_providerFactory;

CIWPOptions::CIWPOptions(): m_bInitialized(false), m_llState(ISShared::i64StateError), 
							m_pIWPElement(NULL), m_pProvider(NULL), 
							m_llUserState(ISShared::i64StateError),
							m_bTrustLocalLans(true), m_bTrustLocalLansOrg(true)
{}

CIWPOptions::~CIWPOptions()
{}

HRESULT CIWPOptions::Initialize()
{
    SYMRESULT sr;

    if(m_bInitialized)
        return S_FALSE;

    // Initialize IElement

    if(SYM_FAILED(sr = s_providerFactory.CreateObject(GETMODULEMGR(), &m_pProvider)))
    {
        CCTRCTXE1 (_T("Failed to create IElement provider. SR = 0%x80"), sr);
    }
    else
    {
        if (SYM_FAILED(sr = m_pProvider->GetElement ( ISShared::CLSID_NAV_IWP, m_pIWPElement)))
        { 
            CCTRCTXE1(_T("Failed to create IElement. SR = 0x%08x"), sr); 
        }
    }

	// get the SNDHelper object
	CSNDHelper::CreateSNDHelper(SND_CLIENT_NAV, m_sndhelper);

	m_bInitialized = true;
    return S_OK;
}

HRESULT CIWPOptions::Save()
{
    HRESULT hr;

    if(FAILED(hr = Initialize()))
        return hr;

    // Not installed?
    if(!m_pIWPElement)
    {
        CCTRCTXW0(_T("IWP not installed - ignoring save."));
        return S_FALSE;
    }

	hr = S_FALSE;

	// changes to on/off?
    if(m_llUserState != m_llState)
	{
		hr = S_OK;

		CCTRCTXI1(_T("saving IWP enabled, %d"), m_llUserState);

		// Yes, it takes this much code to turn IWP on/off.
		ui::IDataPtr pData;
		if(FAILED(hr = m_pIWPElement->GetData(pData)))
		{
			CCTRCTXE1(_T("Could not get IData from IElement. HR = 0x%08x"), hr);
			return hr;
		}

		ISShared::INumberDataNoUIQIPtr pDataNumber = pData;
		if(!pDataNumber)
		{
			CCTRCTXE0(_T("Could not QI to INumberDataNoUI."));
			return E_POINTER;
		}

		if(FAILED(hr = pDataNumber->SetNumberNoUI(m_llUserState)))
			CCTRCTXE1(_T("Failed to change IWP state. HR = 0x%08x"), hr);
		else
			m_llState = m_llUserState;
	}

	// changes to trust/protect?
	if (m_bTrustLocalLans != m_bTrustLocalLansOrg)
	{
		CCTRCTXI1(_T("saving TrustLocalLans, %d"), m_bTrustLocalLans?1:0);

		// don't overwrite an error from above!
		if (SUCCEEDED(hr))
			hr = S_OK;

		if (m_sndhelper.m_p)
		{
			DWORD dwLocalLinkTrust1 = m_bTrustLocalLans ? 1 : 0;
			m_sndhelper->SetSNDSetting(SymNeti::FS_TrustLinkLocal1, dwLocalLinkTrust1);
			m_bTrustLocalLansOrg = m_bTrustLocalLans;
		}
		else
		{
            CCTRCTXE0(_T("Failed to create SNDHelper"));
			hr = E_FAIL;
		}
	}

    return hr;
}

// hardcoded to default to on
HRESULT CIWPOptions::Default()
{
    HRESULT hr;
	CNAVOptSettingsCache navOptDefs;

    if(FAILED(hr = Initialize()))
        return hr;

    if(!m_pIWPElement)
    {
        CCTRCTXW0(_T("IWP not installed - ignoring Default."));
        return S_FALSE;
    }

	// Get default value from NAVOPTS.DEF
	if(!navOptDefs.Init(_T("NAVOPTS.DEF"), FALSE))
	{
		CCTRCTXW0(_T("Could not connect to NAV defaults."));
		m_llUserState = ISShared::i64StateOn;
	}
	else
	{
		DWORD dwVal;
		if(FAILED(hr = navOptDefs.GetValue(_T("DEFAULT:IWP"), dwVal, 1)))
		{
			CCTRCTXW1(_T("Error retrieving IWP default state. HR = 0x%08x"), hr);
			m_llUserState = ISShared::i64StateOn;
		}
		else
		{
			m_llUserState = dwVal?ISShared::i64StateOn:ISShared::i64StateOff;
		}
	}

	m_bTrustLocalLans = true;
    return hr;
}

bool CIWPOptions::IsDirty() { return (m_llUserState != m_llState) || (m_bTrustLocalLans != m_bTrustLocalLansOrg); }

HRESULT CIWPOptions::Notify() { return S_FALSE; }

/** Business Methods **/
HRESULT CIWPOptions::IsIWPInstalled(bool& bInstalled)
{
    HRESULT hr;
    if(FAILED(hr = Initialize()))
        return hr;

    bInstalled = m_pIWPElement ? true:false;
    return S_OK;
}

HRESULT CIWPOptions::CanChangeIWPState(bool &bAllowed)
{
    HRESULT hr;

    if(FAILED(hr = Initialize()))
        return hr;
    
    // If no IWP, skip and imply read-only.
    if(!m_pIWPElement)
	{
		bAllowed = false;
        return S_OK;
	}

	bool bReadOnly = false;
    if(FAILED(hr = m_pIWPElement->GetReadOnly(bReadOnly)))
    {
        CCTRCTXE1(_T("Failed to check if element is read-only. HR = 0x%08x"),hr);
    }
	else
		bAllowed = !bReadOnly;

    return hr;
}

HRESULT CIWPOptions::IsIWPOn(bool &bOn)
{
    HRESULT hr;
    LONGLONG llState;

    bOn = false;

    if(FAILED(hr = this->GetIWPState(llState)))
        return hr;

    bOn = (llState == ISShared::i64StateOn);
    return S_OK;
}

/**  IWP External UI Launching Methods **/
HRESULT CIWPOptions::LaunchExclusionsUI(HWND hWndParent)
{
    HRESULT hr;
    SYMRESULT sr;
	CStringW cswTitle;

	idsui::IIDSSigExclUIPtr exclUi;

    if (FAILED(hr = Initialize()))
        return hr;

    if (!m_pIWPElement)
        return S_FALSE;

    cswTitle.LoadString(g_ResModule, IDS_IWP_FEATURE_NAME);

     if (SYM_FAILED(sr = s_IDSExclusionsUILoader.CreateObject(exclUi)))
    {
         CCTRCTXE1(_T("Could not create IDS Sig Exclusion UI object. Symresult = 0x%08x"), (DWORD)sr);
         return E_FAIL;
    }

	// Get the icon
	HICON hIcon = LoadIcon(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_NAVOPTIONS));

	if(FAILED(hr = exclUi->Initialize(hWndParent, hIcon, cswTitle, FWUI_PRODUCT_NAV)))
        return hr;

    return exclUi->ShowModalDialog();
}

HRESULT CIWPOptions::LaunchAppRulesUI(HWND hWndParent)
{
    HRESULT hr;
	CSymPtr<CPgmOptions> PgmUi;
    CStringW cswIWPName;
	CStringW cswFormat;
    CStringW cswTitle;

    if(FAILED(hr = Initialize()))
        return hr;

    if(!m_pIWPElement)
        return S_FALSE;

    cswIWPName.LoadString(g_ResModule, IDS_IWP_FEATURE_NAME);
    cswFormat.LoadString(g_ResModule, IDS_IWP_APPRULES_TITLE);
    cswTitle.Format(cswFormat, cswIWPName);

	PgmUi = new(std::nothrow) CPgmOptions();
	if (PgmUi.m_p == NULL)
	{
		CCTRACEE(_T("LaunchAppRulesUI:: failed to create Pgm UI object"));
		return E_FAIL;
	}

	// Get the icon
	HICON hIcon = LoadIcon(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_NAVOPTIONS));

	if (FAILED(PgmUi->Initialize(hWndParent, cswTitle, hIcon, m_sndhelper)))
	{
		CCTRACEE(_T("LaunchAppRulesUI:: failed to init Pgm UI object"));
		return E_FAIL;
	}

	PgmUi->ShowModalDialog();

	return S_OK;
}

HRESULT CIWPOptions::LaunchAutoBlockUI(HWND hWndParent)
{
    HRESULT hr;
    CSymPtr<CIDSOptions> IDSUi;
    CStringW cswIWPName;
	CStringW cswFormat;
    CStringW cswTitle;

    if(FAILED(hr = Initialize()))
        return hr;

    if(!m_pIWPElement)
        return S_FALSE;

    cswIWPName.LoadString(g_ResModule, IDS_IWP_FEATURE_NAME);
    cswFormat.LoadString(g_ResModule, IDS_IWP_AUTOBLOCK_TITLE);
    cswTitle.Format(cswFormat, cswIWPName);

	IDSUi = new(std::nothrow) CIDSOptions();
	if (IDSUi.m_p == NULL)
	{
		CCTRACEE(_T("LaunchAutoBlockUI:: failed to create AutoBlock UI object"));
		return E_FAIL;
	}

	// Get the icon
	HICON hIcon = LoadIcon(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_NAVOPTIONS));

	if (FAILED(IDSUi->Initialize(hWndParent, cswTitle, hIcon)))
	{
		CCTRACEE(_T("LaunchAutoBlockUI:: failed to init AutoBlock UI object"));
		return E_FAIL;
	}

	IDSUi->ShowModalDialog();

	return S_OK;
}

HRESULT CIWPOptions::LaunchFWRulesUI(HWND hWndParent)
{
    HRESULT hr;
    SYMRESULT sr;
    fwui::IFWRuleUIPtr fwRuleUi;

    if(FAILED(hr = Initialize()))
        return hr;

    if(!m_pIWPElement)
        return S_FALSE;

	if (m_sndhelper.m_p == NULL)
		return E_FAIL;

	if(SYM_FAILED(sr = s_RuleUILoader.CreateObject(&fwRuleUi)))
    {
        CCTRCTXE1(_T("Could not create FW Rule UI object. Symresult = 0x%08x"), (DWORD)sr);
        return E_FAIL;
    }

	// Get the title
    CStringW cswIWPName;
	CStringW cswFormat;
    CStringW cswTitle;

	cswIWPName.LoadString(g_ResModule, IDS_IWP_FEATURE_NAME);
    cswFormat.LoadString(g_ResModule, IDS_IWP_GENRULES_TITLE);
    cswTitle.Format(cswFormat, cswIWPName);

	// Get the icon
	HICON hIcon = LoadIcon(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_NAVOPTIONS));

	if(FAILED(hr = fwRuleUi->Initialize(hWndParent, cswTitle, cswIWPName, hIcon)))
        return hr;

	fwRuleUi->SetDwordProperty(fwui::RULE_PROPDW_HELPID, NAV_IWP_customize_general_rules);
	fwRuleUi->SetDwordProperty(fwui::RULE_PROPDW_INBOUND_ONLY, TRUE);
	fwRuleUi->SetDwordProperty(fwui::RULE_PROPDW_PRODUCT_ID, FWUI_PRODUCT_NAV);

	CStringW cswFeatureName = L"";
	cswFeatureName.LoadString(_Module.GetResourceInstance(), IDS_IWP_FEATURE_NAME);
	fwRuleUi->SetStringProperty(fwui::RULE_PROPSZ_FIREWALL_NAME, (LPCWSTR)cswFeatureName);

	// get the root db...

	SymNeti::IFWRecordPtr pRootDB;
	pRootDB = m_sndhelper->GetSharedRootDB();
	if (pRootDB == NULL)
	{
		CCTRCTXE0(_T("unable to get the Root DB"));
		return E_FAIL;
	}

	// get the general rules
	SymNeti::IFWListPtr spSysRules;
	if (SN_FAILED(pRootDB->GetItem(SymNeti::xSystemRules, &spSysRules)) || spSysRules.m_p == NULL)
	{
		CCTRCTXE0(_T("unable to get the system rules"));
		return E_FAIL;
	}

	// make a copy of the sys rule list

	SymNeti::IFWListPtr spTempRuleList;
	if (SN_FAILED(spSysRules->Create(&spTempRuleList)))
	{
		CCTRCTXE0(_T("failed to create temp rule list"));
		return E_FAIL;
	}

	if (SN_FAILED(spTempRuleList->CopyFrom(spSysRules)))
	{
		CCTRCTXE0(_T("failed to init temp rule list"));
		return E_FAIL;
	}

	if (FAILED(hr = fwRuleUi->SetRuleList(spTempRuleList, fwui::RULE_GENERAL)))
	{
		CCTRCTXE0(_T("failed to set the rule UI list"));
		return E_FAIL;
	}

	bool bDirty = false;
	fwRuleUi->ShowModalDialog(bDirty);

	// save changes, if asked to

	if (bDirty)
	{
		if (SN_FAILED(spSysRules->CopyFrom(spTempRuleList)))
		{
			CCTRCTXE0(_T("unable to update rule list"));
			return E_FAIL;
		}

		if (m_sndhelper->CommitSharedRuleDB() != true)
		{
			CCTRCTXE0(_T("unable to save rule list"));
			return E_FAIL;
		}
	}

	return S_OK;
}

/** Gets And Sets **/
HRESULT CIWPOptions::GetIWPState(LONGLONG& llState)
{
    HRESULT hr;

    if(FAILED(hr = Initialize()))
        return hr;

    if(!m_pIWPElement)
    {
        m_llState = llState = E_NOTIMPL;
    }
    else
    {
        ui::IDataPtr pData;
        ui::INumberDataQIPtr pDataNumber;
        llState = ISShared::i64StateError;

        if(FAILED(hr = m_pIWPElement->GetData(pData)))
        {
            CCTRCTXE1(_T("Could not get IData from IElement. SymResult = 0x%08x"), hr);
            return E_FAIL;
        }

        pDataNumber = pData;
        if(!pDataNumber)
        {
            CCTRCTXE0(_T("Could not QI to INumberData."));
            return E_POINTER;
        }

        if(FAILED(hr = pDataNumber->GetNumber(llState)))
        {
            CCTRCTXE1(_T("Failed to get Number from IData. HR = 0x%08x"), hr);
            return hr;
        }

        m_llState = llState;
		if(m_llUserState == ISShared::i64StateError)
		{
			m_llUserState = m_llState;	// Cache actual state as user state the first time.

			m_bTrustLocalLans = false;
			m_bTrustLocalLansOrg = false;

			if (m_sndhelper.m_p)
			{
				DWORD dwLocalLinkTrust1 = 0;
				if (m_sndhelper->GetSNDSetting(SymNeti::FS_TrustLinkLocal1, dwLocalLinkTrust1))
				{
					if (dwLocalLinkTrust1 == 1)
					{
						m_bTrustLocalLans = true;
						m_bTrustLocalLansOrg = true;
					}
				}
			}
			else
			{
				CCTRCTXE0(_T("Failed to create SNDHelper"));
				return E_FAIL;
			}
		}
    }
    return S_OK;
}

HRESULT CIWPOptions::GetIWPUserTrust(bool& bEnable)
{
	HRESULT hr;
	LONGLONG llState;	// Throwaway

	// If we're in StateError, we haven't been initialized.
	// Calling GetIWPState will initialize CIWPOptions and
	// then appropriately set m_llUserState and m_llState.
	if(m_llUserState == ISShared::i64StateError && FAILED(hr = this->GetIWPState(llState)))
	{
		return hr;
	}

	bEnable = m_bTrustLocalLans;

	return S_OK;
}

HRESULT CIWPOptions::SetIWPUserTrust(const bool& bEnable)
{
    HRESULT hr;
    bool bCanChange;

    if(FAILED(hr = this->CanChangeIWPState(bCanChange)))
        return hr;

    if(!m_pIWPElement)
        return S_FALSE;

    if(!bCanChange)
        return E_ACCESSDENIED;

	m_bTrustLocalLans = bEnable;

    return S_OK;
}

HRESULT CIWPOptions::GetIWPUserEnabled(bool& bEnable)
{
	HRESULT hr;
	LONGLONG llState;	// Throwaway

	// If we're in StateError, we haven't been initialized.
	// Calling GetIWPState will initialize CIWPOptions and
	// then appropriately set m_llUserState and m_llState.
	if(m_llUserState == ISShared::i64StateError && FAILED(hr = this->GetIWPState(llState)))
	{
		return hr;
	}

	bEnable = m_llUserState == ISShared::i64StateOn;

	return S_OK;
}

HRESULT CIWPOptions::SetIWPUserEnabled(const bool& bEnable)
{
    HRESULT hr;
    bool bCanChange;

    if(FAILED(hr = this->CanChangeIWPState(bCanChange)))
        return hr;

    if(!m_pIWPElement)
        return S_FALSE;

    if(!bCanChange)
        return E_ACCESSDENIED;

	m_llUserState = bEnable ? ISShared::i64StateOn : ISShared::i64StateOff;

    return S_OK;
}
