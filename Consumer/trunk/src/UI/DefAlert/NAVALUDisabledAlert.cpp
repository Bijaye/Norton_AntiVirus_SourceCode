////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVALUDisabledAlert.cpp

#include "stdafx.h"
#include "Resource.h"
#include "NAVALUDisabledAlert.h"
#include "InstOptsNames.h"
#include "OptNames.h"
#include "SSOsinfo.h"
#include "NavOptHelperEx.h"
#include "navtrust.h"
#include "NetDetectController.h"                // For AutoLiveUpdate
#include "NAVSettingsHelperEx.h"

#include "NAVSecurity.h"

SIMON_STDMETHODIMP CALUDisabledAlert::Init()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		DWORD dwEnabled = 0;
        if(!SUCCEEDED(hr = m_cltHelper.Initialize()))
        {
            CCTRCTXE1(_T("Could not init CLT Helper. HR = 0x%08X"), hr);
            m_bAlertable = FALSE;
            return hr;
        }

		READ_SETTING_START()
			READ_SETTING_DWORD(DEFALERT_EnableALUDisabledAlert, dwEnabled, 1)
		READ_SETTING_END

		m_bAlertable = (dwEnabled == 1) ? TRUE : FALSE;

		CCTRACEI(_T("CALUDisabledAlert::Init - Alertable: %d"), m_bAlertable);
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}

SIMON_STDMETHODIMP CALUDisabledAlert::Refresh(BOOL bRefreshCache)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		// Intentionally empty.
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}

SIMON_STDMETHODIMP CALUDisabledAlert::ShouldShowAlert(BOOL* pbShow, SYMALERT_TYPE* pType, BOOL bCheckTimer, DWORD dwFlag)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		if(!m_bAlertable)
			return S_OK;

		USES_CONVERSION;

		hrx << (pType ? S_OK : E_POINTER);
		hrx << (pbShow ? S_OK : E_POINTER);

		*pType = SYMALERT_TYPE_INVALID;
		*pbShow = FALSE;

		// Check to see if ALU is disabled.
		// Check for a valid digital signature on the COM Server before loading it
		hrx << ((NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(CLSID_NDScheduler))? E_FAIL : S_OK);

		CComPtr<INDScheduler> spScheduler;
		BOOL bALUEnabled = FALSE;
		hrx << spScheduler.CoCreateInstance(CLSID_NDScheduler, NULL, CLSCTX_INPROC_SERVER);
		hrx << spScheduler->Init();		
		hr = spScheduler->GetEnabled(&bALUEnabled);
		if(FAILED(hr))
		{
			CCTRACEI(_T("CALUDisabledAlert::ShouldShowAlert - There is no tasks added to run LU -> ALU disabled %08X."), hr);
			bALUEnabled = FALSE;
		}

		if(bALUEnabled)
		{
			// ALU is enabled, no alert. Exit.
			return S_OK;
		}

		// Check to see if this is OEM and if CfgWiz is finished.
		DWORD dwOEM = 0;		
		DWORD dwFinished = 0;
		CNAVInfo NAVInfo;
		TCHAR szCfgWizDat[MAX_PATH] = {0};
		CNAVOptFileEx cfgWizFile;

		::wsprintf(szCfgWizDat, _T("%s\\CfgWiz.dat"), NAVInfo.GetNAVDir());
		hrx << (cfgWizFile.Init(szCfgWizDat, FALSE)? S_OK : E_FAIL);
		cfgWizFile.GetValue(InstallToolBox::CFGWIZ_OEM, dwOEM, 0);
		cfgWizFile.GetValue(InstallToolBox::CFGWIZ_Finished, dwFinished, 0);

		if(dwOEM == 0 || dwFinished == 0)
			return S_OK;

		// ALUDisabled alert to be displayed after OEM customers 
		//  renew subscription for the very first time.
		//  Check to see if we have already displayed it.
		DWORD dwAlertShown = 0;
		dwAlertShown = _GetAlertDword(ALERT_ALUDisabledShown, 0, FALSE);	
		if(dwAlertShown)
			return S_OK;

        bool bAlertable = TRUE;

        STAHLSOFT_HRX_TRY(hr)
        {
            hrx << m_cltHelper.ShouldShowAlerts(bAlertable);

            if( bAlertable )
            {
                *pType = SYMALERT_TYPE_ALUDisabled;
                *pbShow = TRUE;
            }
        }
        STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr);
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}

