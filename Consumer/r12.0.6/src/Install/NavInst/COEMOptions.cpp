#include "StdAfx.h"
#include "helper.h"
#include "COEMOptions.h"
#include "NAVAPSCR_i.c"
#include "NavOptions_i.c"
#include "symscriptsafe.h"
#include "ccServicesConfigInterface.h"
#include "ccInstLoader.h"
//
//COEMOptions::COEMOptions() -- standard constructor
//
COEMOptions::COEMOptions()
{
	pOptionsObject = NULL;
	pAPScriptObject = NULL;
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

	//close the AP com object
	
	if (pAPScriptObject != NULL)
		pAPScriptObject->Release();

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
		m_hrCOM = CoInitialize(NULL);

		//instantiate the Options com object
		if( FAILED (CoCreateInstance(CLSID_NAVOptions, NULL, CLSCTX_INPROC_SERVER, 
			IID_INAVOptions, (void**) &pOptionsObject)) )
		{
			g_Log.Log("COEMOptions::Initialize -- Could not create Options Object");
			return false;
		}
		else
		{
			if(pOptionsObject)
			{
				ISymScriptSafe* pss = NULL;
				HRESULT hr = pOptionsObject->QueryInterface(IID_ISymScriptSafe, (void**)&pss);
				if(SUCCEEDED(hr) && pss)
				{
					hr = pss->SetAccess(ISYMSCRIPTSAFE_UNLOCK_KEYA, ISYMSCRIPTSAFE_UNLOCK_KEYB);

					pss->Release();
				}
			}
		}

        // set the business rules flag to false here so any save
        // calls on this object do not use navoptrf and cause a joyous
        // 8 minute hang in the install
        pOptionsObject->SetUseBusinessRules(FALSE);

		//instantiate the AP com object
		if( FAILED (CoCreateInstance(CLSID_ScriptableAutoProtect, NULL, CLSCTX_INPROC_SERVER, 
										IID_IScriptableAutoProtect, (void**) &pAPScriptObject)) )
		{
			g_Log.Log("COEMOptions::Initialize -- Could not get AP Object");
			return false;
		}
		else
		{
			if(pAPScriptObject)
			{
				ISymScriptSafe* pss = NULL;
				HRESULT hr = pAPScriptObject->QueryInterface(IID_ISymScriptSafe, (void**)&pss);
				if(SUCCEEDED(hr) && pss)
				{
					hr = pss->SetAccess(ISYMSCRIPTSAFE_UNLOCK_KEYA, ISYMSCRIPTSAFE_UNLOCK_KEYB);

					pss->Release();
				}
			}
		}
	}
	catch(exception &ex)
	{
		g_Log.LogEx(ex.what());
		return false;
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in COEMOptions::Initialize()");
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
	wsprintf(szOptName, "THREAT:Threat%d", iIndex);

	//example:	
	if( FAILED(pOptionsObject->Put(CComBSTR(szOptName), CComVariant( (int) bYes_No))) )
	{
		g_Log.Log("COEMOptions::TurnOnThreatCat() -- Failed to set Threat Options");
		return_value = false;
	}

	return return_value;
}


//
//COEMOptions::TurnOnAP() -- Turn AP On or leave it off
//
bool COEMOptions::TurnOnAP(bool bYes_No)
{
	if(!m_bAllOK)
		return false;
	
	bool return_value = true;
	
	//which OS is this?
	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx (&osvi);

	
	//
	//set the service mode to auto or manual or on 9x set the static vxd key (NOTE: this does not affect current session)
	//
	if( FAILED(pAPScriptObject->Configure(bYes_No)) )
	{
		g_Log.Log("COEMOptions::TurnOnAp() -- Call to AP->Configure Failed.");
		return_value = false;
	}

	//
	//set the Startup_LoadVXD setting in options.
	//
	if(FAILED (pOptionsObject->Put(CComBSTR(STARTUP_LoadVxD), CComVariant( (int) bYes_No))))
	{
		g_Log.Log("COEMOptions::TurnOnAp() -- Failed to set LoadVXD Option.");
		return_value = false;
	}
		
	//
	//On NT platforms, if a reboot isn't needed, we should enable/disable AP for the current session
	//
	if( !RebootNeeded() && osvi.dwPlatformId == VER_PLATFORM_WIN32_NT && FAILED(pAPScriptObject->put_Enabled(bYes_No)) )
	{
		g_Log.Log("COEMOptions::TurnOnAp() -- Failed to turn on AP.");
		return_value = false;
	}
	
	return return_value;
}

//
//COEMOptions::TurnOnDelayLoad() -- Turn DelayLoad on or off accordingly
//
bool COEMOptions::TurnOnDelayLoad(bool bYes_No)
{
	if(!m_bAllOK)
		return false;

	bool return_value = true;

	if( FAILED(pOptionsObject->Put(CComBSTR(AUTOPROTECT_DelayLoad), CComVariant( (int) bYes_No))) )
	{
		g_Log.Log("COEMOptions::TurnOnDelayLoad() -- Failed to set DelayLoad Options");
		return_value = false;
	}

    // Need to commit this value right now so the configure will read it
    pOptionsObject->Save();

	return return_value;
}

//
//COEMOptions::TurnOnEMI -- Turn inbound e-mail scanning on or off
//
bool COEMOptions::TurnOnEMI(bool bYes_No)
{
	if(!m_bAllOK)
		return false;

	bool return_value = true;
	
	if( FAILED(pOptionsObject->Put(CComBSTR(NAVEMAIL_ScanIncoming), CComVariant( (int) bYes_No))) )
	{
		g_Log.Log("COEMOptions::TurnOnEMI() -- Failed to Set Incomming Email Option.");
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
	
	if( FAILED(pOptionsObject->Put(CComBSTR(NAVEMAIL_ScanOutgoing), CComVariant( (int) bYes_No))) )
	{
		g_Log.Log("COEMOptions::TurnOnEMI() -- Failed to Set Outgoing Email Option.");
		return_value = false;
	}
	return return_value;
}


//
//COEMOptions::TurnOnMSN -- Turns MSN Scanning on or Off
//
bool COEMOptions::TurnOnMSN(bool bYes_No)
{
	if(!m_bAllOK)
		return false;
	
	bool return_value = true;
	
	if( FAILED(pOptionsObject->Put(CComBSTR(IMSCAN_ScanMIM), CComVariant( (int) bYes_No))) )
	{
		g_Log.Log("COEMOptions::TurnOnMSN() -- Failed to Set Outgoing Email Option.");
		return_value = false;
	}
	return return_value;
}

bool COEMOptions::SetSAVRTPELManual()
{
	if(!m_bAllOK)
		return false;
	
	bool return_value = true;

	//
	// set SAVRT to Manual mode
	//
	if( FAILED(pAPScriptObject->SetSAVRTPELStartMode(SERVICE_DEMAND_START)) )
	{
		g_Log.Log("COEMOptions::SetSAVRTPELManual() -- Call to AP->SetSAVRTPELManual Failed.");
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
			g_Log.Log(_T("CInstallWatcher::SetCCServicesStates() - Unable to load IServicesConfig Object.\n"));
			return false;
		}

		if(!spSvcCfg->Create())
		{
			g_Log.Log(_T("CInstallWatcher::SetCCServicesStates() - spSvcCfg->Create() == false\n"));
			return false;
		}

		g_Log.Log(_T("CInstallWatcher::SetCCServicesStates() - Setting CC services to automatic start.\n"));
		if(spSvcCfg->SetServicesStartup(bAutoStart ? spSvcCfg->SC_AUTO : spSvcCfg->SC_MANUAL) == FALSE)
		{
			g_Log.Log(_T("CInstallWatcher::SetCCServicesStates() - spSvcCfg->SetAllServicesAuto() == FALSE.\n"));
			return_value = false;
		}
		
		if(bStartNow && !RebootNeeded())
		{
			g_Log.Log(_T("CInstallWatcher::SetCCServicesStates() - Starting all CC Services.\n"));
			if(spSvcCfg->Start() == FALSE)
			{
				g_Log.Log(_T("CInstallWatcher::SetCCServicesStates() - spSvcCfg->Start() == FALSE.\n"));
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