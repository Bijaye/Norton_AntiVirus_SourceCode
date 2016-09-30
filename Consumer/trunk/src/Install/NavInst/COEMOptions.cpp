////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "helper.h"
#include "COEMOptions.h"
#include "NavOptions_i.c"
#include "ccServicesConfigInterface.h"
#include "ccInstLoader.h"
#include "AVDefines.h"
#include "AVInterfaceLoader.h"

//
//COEMOptions::COEMOptions() -- standard constructor
//
COEMOptions::COEMOptions()
{
	pOptionsObject = NULL;
	m_bAllOK = false;
	m_hrCOM = E_FAIL;

	return;
}


//
//COEMOptions::~COEMOptions() -- standard destructor
//
COEMOptions::~COEMOptions()
{

	//close & save the Options com object
	
	if(pOptionsObject != NULL)
	{
		pOptionsObject->Save();
		pOptionsObject->Release();
	}

	m_pProvider.Release();

	//unintialize COM
	
	if(SUCCEEDED(m_hrCOM))
		CoUninitialize();

	return;
}


//
//CBooteWarn::Initialize() --  Initialize the Options and ScriptableAP COM objects and ensure scriptsafe
//
bool COEMOptions::Initialize()
{
	try
	{
        ISShared::ISShared_IProvider loader;
        SYMRESULT sr = loader.CreateObject(GETMODULEMGR(), &m_pProvider);
        if(SYM_FAILED(sr))
            CCTRCTXE1 (_T("Failed to get provider 0x%x"), sr);   

		m_hrCOM = CoInitialize(NULL);

		//instantiate the Options com object
		if( FAILED (CoCreateInstance(CLSID_NAVOptions, NULL, CLSCTX_INPROC_SERVER, 
			IID_INAVOptions, (void**) &pOptionsObject)) )
		{
			CCTRACEE(_T("COEMOptions::Initialize -- Could not create Options Object"));
			return false;
		}
	}
	catch(exception &ex)
	{
		CCTRACEE(CA2CT(ex.what()));
		return false;
	}
	catch(...)
	{
		CCTRACEE(_T("Unknown exception in COEMOptions::Initialize()"));
		return false;
	}

	m_bAllOK = true;

	return true;
}

//
//COEMOptions::TurnOnThreatCats() -- Turn all ThreatCats on or off accordingly
//
bool COEMOptions::TurnOnThreatCat(int iIndex, bool bYes_No)
{
	if(!m_bAllOK)
		return false;

	bool return_value = true;

	TCHAR szOptName[50] = {0};
	wsprintf(szOptName, _T("THREAT:Threat%d"), iIndex);

	//example:	
	if( FAILED(pOptionsObject->Put(CComBSTR(szOptName), CComVariant( (int) bYes_No))) )
	{
		CCTRACEE(_T("COEMOptions::TurnOnThreatCat() -- Failed to set Threat Options"));
		return_value = false;
	}

	return return_value;
}

bool COEMOptions::DefaultThreatCat(int iIndex)
{
	BOOL dVal;
	if( FAILED(pOptionsObject->DefaultThreatCategoryEnabled(iIndex, &dVal)))
	{
		CCTRACEE(_T("COEMOptions::DefaultThreatCat() -- Failed to set Default Threat Category Option"));
		return false;
	}
	return true;
}

//
//COEMOptions::TurnOnAPEarlyLoad() -- Turn Early Load on or off accordingly
//
bool COEMOptions::TurnOnAPEarlyLoad(bool bYes_No)
{
	if(!m_bAllOK)
		return false;

	bool return_value = true;
	
	if( FAILED(pOptionsObject->Put(CComBSTR(AUTOPROTECT_DriversSystemStart), CComVariant( (int) bYes_No))) )
	{
		MessageBox(NULL, _T("Failed to set AP DSS option"), _T("crap"), MB_OK);
		CCTRACEE(_T("COEMOptions::TurnOnAPEarlyLoad() -- Failed to Set AutoProtect DriversSystemStart Option."));
		return_value = false;
	}
	return return_value;
}

bool COEMOptions::SetAPValue(const wchar_t* lpszOptionName, DWORD dwValue )
{
	AVModule::IAVAutoProtectPtr spAP;
	AVModule::AVLoader_IAVAutoProtect APLoader;
	AVModule::IAVMapStrDataPtr APOptsMap;

    SYMRESULT sr;

	if(SYM_FAILED(sr = APLoader.CreateObject(&spAP)) 
       || !spAP.m_p )
	{
		CCTRCTXE1(_T("Failed to create AVComponent AP object. SR = 0x%08x"), sr);
		return false;
	}

    HRESULT hr = spAP->GetOptions(APOptsMap);

    if ( SUCCEEDED (hr))
    {
        // Early load = system start (driver loads at boot)
        // Delay load = manual start (service starts it when it loads)
        //
        APOptsMap->SetValue ( lpszOptionName, dwValue);
        hr = spAP->SetOptions (APOptsMap);
        
        if ( SUCCEEDED (hr))
        {
            
            CCTRCTXI2 (_T("Set AP option %s %d"), ATL::CW2T (lpszOptionName), dwValue);
            return true;
        }
        else
            CCTRCTXE1(_T("Failed to Set AP Options Map. HR = 0x%08x"), hr);
    }
    else
        CCTRCTXE1(_T("Failed to Get AP Options Map. HR = 0x%08x"), hr);

    return false;
}

bool COEMOptions::TurnOnAutoProtect(bool bEnable)
{
	AVModule::IAVAutoProtectPtr spAP;
	AVModule::AVLoader_IAVAutoProtect APLoader;

    SYMRESULT sr;

	if(SYM_FAILED(sr = APLoader.CreateObject(&spAP)) 
       || !spAP.m_p )
	{
		CCTRCTXE1(_T("Failed to create AVComponent AP object. SR = 0x%08x"), sr);
		return false;
	}

	HRESULT hr = spAP->EnableAP (bEnable); 

	if ( SUCCEEDED (hr))
	{
		CCTRCTXI1(_T("AP enabled %d"), bEnable);
		spAP.Release();
		return true;
	}
	else
	{
		CCTRCTXE2(_T("failed to enable AP %d 0x%x"), bEnable, hr);
		spAP.Release();
		return false;
	}
}


//
//COEMOptions::TurnOnEMI -- Turn inbound e-mail scanning on or off
//
bool COEMOptions::TurnOnEMI(bool bYes_No)
{
	if(!m_bAllOK)
		return false;

	bool return_value = true;
	
	if( FAILED(pOptionsObject->Put(CComBSTR(AVEMAIL_ScanIncoming), CComVariant( (int) bYes_No))) )
	{
		CCTRACEE(_T("COEMOptions::TurnOnEMI() -- Failed to Set Incomming Email Option."));
		return_value = false;
	}
	return return_value;
}


//
//COEMOptions::TurnOnEMO -- Turn outbound e-mail scanning on or off
//
bool COEMOptions::TurnOnEMO(bool bYes_No)
{
	if(!m_bAllOK)
		return false;
	
	bool return_value = true;
	
	if( FAILED(pOptionsObject->Put(CComBSTR(AVEMAIL_ScanOutgoing), CComVariant( (int) bYes_No))) )
	{
		CCTRACEE(_T("COEMOptions::TurnOnEMI() -- Failed to Set Outgoing Email Option."));
		return_value = false;
	}
	return return_value;
}

bool COEMOptions::SetCCServicesStates(BOOL bAutoStart, BOOL bStartNow)
{
	bool return_value = true;

	// scope this loader so it go out of scope last
	cc::ccInst_IServicesConfig_Untrusted SvcCfgLdr;
	{
		cc::IServicesConfigPtr spSvcCfg;

		if(SYM_FAILED(SvcCfgLdr.CreateObject(spSvcCfg.m_p)) || spSvcCfg == NULL)
		{
			CCTRACEE(_T("CInstallWatcher::SetCCServicesStates() - Unable to load IServicesConfig Object.\n"));
			return false;
		}

		if(!spSvcCfg->Create())
		{
			CCTRACEE(_T("CInstallWatcher::SetCCServicesStates() - spSvcCfg->Create() == false\n"));
			return false;
		}

		CCTRACEI(_T("CInstallWatcher::SetCCServicesStates() - Setting CC services to automatic start.\n"));
		if(spSvcCfg->SetServicesStartup(bAutoStart ? spSvcCfg->SC_AUTO : spSvcCfg->SC_MANUAL) == FALSE)
		{
			CCTRACEE(_T("CInstallWatcher::SetCCServicesStates() - spSvcCfg->SetAllServicesAuto() == FALSE.\n"));
			return_value = false;
		}
		
		if(bStartNow && !RebootNeeded())
		{
			CCTRACEI(_T("CInstallWatcher::SetCCServicesStates() - Starting all CC Services.\n"));
			if(spSvcCfg->Start() == FALSE)
			{
				CCTRACEE(_T("CInstallWatcher::SetCCServicesStates() - spSvcCfg->Start() == FALSE.\n"));
				return_value = false;
			}
		}
	}

	return return_value;
}

bool COEMOptions::RebootNeeded()
{
	CRegKey rk;
	if(ERROR_SUCCESS == rk.Open(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\SymSetup"), KEY_READ))
	{
		DWORD dwValue;
		if(ERROR_SUCCESS == rk.QueryDWORDValue(_T("RebootNeeded"), dwValue))
		{
			return (0 != dwValue);
		}
	}

	return false;
}
