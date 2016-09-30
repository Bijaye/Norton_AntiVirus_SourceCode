////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "subscription.h"
//#include "GlobalMutex.h"

// For renew
#include "ISComponentFrameworkGuids.h"
#include "SuiteOwnerHelper.h"
#include "InstOptsNames.h"
#include "NAVTrust.h"
#include "NavOptHelperEx.h"

CSubscription::CSubscription(void)
{}

CSubscription::~CSubscription(void)
{}

bool CSubscription::init (void)
{
    return true;
}

// We use the COM licensing object so we don't have to link in SIMON stuff!!!
//
HRESULT CSubscription::LaunchSubscriptionWizard(HWND hWndParent /*NULL*/)
{
    if (!init())
    {
        CCTRACEE (_T("CSubscription::LaunchSubscriptionWizard - Failed to init"));
        return E_FAIL;
    }

    if ( NULL == hWndParent )
        hWndParent = ::GetDesktopWindow ();

	try
	{
        HRESULT hr = S_OK;
        CCLTProductHelper cltHelper;
        hr = cltHelper.Initialize();
        if( FAILED(hr) )
        {
            CCTRCTXE1(_T("Could not initialize CLT Helper. HR = 0x%08x"), hr);
            return hr;
        }

        hr = cltHelper.LaunchSubscriptionWizard(hWndParent);
		if ( FAILED (hr))
        {
            CCTRCTXE1(_T("Failed to launch Subscription Wizard Hr = 0x%08x"), hr);
            return hr;
        }
	}
	catch(_com_error& e)
	{
        CCTRACEE(_T("CSubscription::LaunchSubscriptionWizard - Exception while launching subscription wizard 0x%08X"), e.Error());
        return e.Error();
	}

	return S_OK;
}

HRESULT CSubscription::HasUserAgreedToEULA(BOOL* pbAgreed)
{
    if (!init())
    {
        CCTRACEE (_T("CSubscription::HasUserAgreedToEULA - Failed to init"));
        return E_FAIL;
    }

    if(!pbAgreed)
    {
        CCTRCTXE0(_T("Invalid argument."));
        return E_INVALIDARG;
    }

    *pbAgreed = FALSE;
    CString csCfgwizDat;

    if(SuiteOwnerHelper::GetOwnerCfgWizDat(csCfgwizDat))
    {
        CNAVOptFileEx cfgWizFile;
        if(cfgWizFile.Init(csCfgwizDat, FALSE))
        {
            DWORD dwAgreedEULA = 0;
            DWORD dwOEM = 0;

            cfgWizFile.GetValue(InstallToolBox::CFGWIZ_OEM, dwOEM, 0);
            if(dwOEM == 1 || dwOEM == 2)
            {
                // Check to see if OEM and CTO user 
                //  has already accepted EULA during cfgwiz.
                cfgWizFile.GetValue(InstallToolBox::CFGWIZ_AgreedToEULA, dwAgreedEULA, 0);
                if(dwAgreedEULA)
                    *pbAgreed = TRUE;
            }
            else
            {
                // Retail/SCSS users already accepted EULA during install.
                *pbAgreed = TRUE;
            }

            CCTRACEI(_T("CNAVLicenseInfo::get_HasUserAgreedToEULA - OEM=%d, Agreed=%d"), dwOEM, dwAgreedEULA);
            return S_OK;
        }
        else
        {
            CCTRCTXE1(_T("Could not initialize Options file wrapper to file [%s]"), csCfgwizDat);
            return E_FAIL;
        }
    }

    CCTRCTXE0(_T("Could not find CfgWiz DAT file."));
    return E_FAIL;
}

HRESULT CSubscription::IsCfgWizFinished(BOOL* pbFinished)
{
    if (!init())
    {
        CCTRACEE (_T("CSubscription::IsCfgWizFinished - Failed to init"));
        return E_FAIL;
    }

    if(!pbFinished)
    {
        CCTRCTXE0(_T("Invalid argument."));
        return E_INVALIDARG;
    }

    HRESULT hr = E_FAIL;
    *pbFinished = FALSE;

    // Load the CfgWiz.dat.

    CString csCfgwizDat;

    if(SuiteOwnerHelper::GetOwnerCfgWizDat(csCfgwizDat))
    {
        // Check to see if CfGwiz is finished or if we're OEM.
        DWORD dwCfgWizFinished = 0;
        DWORD dwOEM = 0;
        CNAVOptFileEx cfgWizOpt;
        if(cfgWizOpt.Init(csCfgwizDat, FALSE))
        {
            hr = cfgWizOpt.GetValue(InstallToolBox::CFGWIZ_Finished, dwCfgWizFinished, 1);
            *pbFinished = (BOOL)dwCfgWizFinished;
            CCTRCTXI2(_T("CfgWiz finished: %s | hr = 0x%08x"), (*pbFinished?_T("YES"):_T("NO")), hr);
        }
        else
        {
            CCTRCTXE1(_T("Error while initializing CfgWiz DAT file to file [%s]"), csCfgwizDat);
        }
    }
    else
    {
        CCTRCTXE0(_T("Error while getting CfgWiz DAT file path."));
    }

    return hr;
}