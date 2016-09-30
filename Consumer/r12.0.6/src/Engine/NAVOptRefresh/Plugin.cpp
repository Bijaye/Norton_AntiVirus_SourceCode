#include "StdAfx.h"
#include "plugin.h"
#include <simon.h>
#include "NAVOptionRefreshEngine.h"
#include "resource.h"
#include "NavSettingsCache.h"
#include "optnames.h"
#include "NAVBusinessRules.h"
#include "BusinessRules.h"
#include "cltLicenseConstants.h"
#include "GlobalEvents.h"	// Option change events
#include "GlobalEvent.h"
#include "ccModuleId.h"
#include "NAVError.h"
//#include "GlobalMutex.h"
#include "ccEveryoneDacl.h"
#include "AVccModuleId.h"
#include "DRMNamedProperties.h"
#include "NAVErrorResource.h"
#include "Subscription.h"

extern SIMON::CSimonModule _SimonModuleInternal;

const	char	NAVOPTRF_MUTEX_SESSIONWIDE[] = "NAVOPTRF_MUTEX_SESSIONWIDE";
const LPCTSTR   CCSETMGR_BACKUP_KEY			 = _T("CommonClient\\ccSetMgr");
const LPCTSTR   CCSETMGR_BACKUP_VALUE		 = _T("BackupDataFile");

CPlugin::CPlugin(void)
{
	CCTRACEI("CPlugin()");

    m_szObjectName = "NAVOptRF";
}

CPlugin::~CPlugin(void)
{
}

//////////////////////////////////////////////////////////////////////
// CPlugin::Run( ICCApp* pCCApp );

void CPlugin::Run( ICCApp* pCCApp )
{
	CCTRACEI("Run()");

	// Kill event is not there. We won't be able to respond to
	// shut downs.
	//
    if (!m_eventShutdown.Create ( NULL, FALSE, FALSE, NULL, FALSE ))
    {
        CCTRACEE ( "CPlugin::Run() - Failed to create shutdown event" );
        return;
    }

    // Make sure we are the only NAVOptRF running for this session.
    //
    ccLib::CMutex mutexNAVOptRF;

    // Use a NULL dacl so that all users can open this mutex
    ccLib::CEveryoneDACL everyonedacl;

    if ( !everyonedacl.Create() )
        CCTRACEE( "CPlugin::Run() - Failed to create the NULL dacl" );

    if ( !mutexNAVOptRF.Create ( everyonedacl, TRUE, NAVOPTRF_MUTEX_SESSIONWIDE, FALSE ))
    {
		CCTRACEE ( "CPlugin::Run() - Failed to create mutex" );
        return; // fatal
    }

    // Lock the mutex open
    //
	DWORD dwRet = ::WaitForSingleObject(mutexNAVOptRF,0);
	if(dwRet != WAIT_OBJECT_0)
	{
		// someone else owns this mutex, so we close our handle to it only.
		// DO NOT RELEASE unless we own it.
        CCTRACEE ( "CPlugin::Run() - NAVOptRF already running" );
		return;
	}

    // Disable ccSettingsManager backups if we are done with config wiz.
    //
    // We need COM for Settings Manager, need STA for licensing
    ccLib::CCoInitialize COM (ccLib::CCoInitialize::eSTAModel);

    CSubscription subscription;
    BOOL bCfgWizFinished = FALSE;
    if ( SUCCEEDED (subscription.IsCfgWizFinished(&bCfgWizFinished))
         && bCfgWizFinished)
    {
		disableSettingsBackup();
    }

	//////////////////////////////////////////////
	// BEGIN: Bring up the NAV Opt Refresh Engine
	HRESULT hrNAVOptRF = S_OK;
	StahlSoft::HRX hrx;
	SIMON::CSimonPtr<INAVOptionRefreshEngine>   spEngine;
	long nErrorId = 0;
	try
	{
        hrNAVOptRF = SIMON::CreateInstanceByDLLInstance(_g_hInstance,CLSID_CNAVOptionRefreshEngine,IID_INAVOptionRefreshEngine  ,(void**)&spEngine);
		if(FAILED(hrNAVOptRF))
		{
			nErrorId = ERR_NAVOPTRF_CREATEOBJECT;
			hrx << E_FAIL;
		}

		hrNAVOptRF = spEngine->Init();
		if(FAILED(hrNAVOptRF))
		{
			nErrorId = ERR_NAVOPTRF_INIT;
			hrx << E_FAIL;
		}

		hrNAVOptRF = spEngine->Start();
		if(FAILED(hrNAVOptRF))
		{
			nErrorId = ERR_NAVOPTRF_START;
			hrx << E_FAIL;
		}
	}
	catch(_com_error& e)
	{
		hrNAVOptRF = e.Error();
	}
	catch(...)
	{
		hrNAVOptRF = E_UNEXPECTED;
	}

	if(FAILED(hrNAVOptRF))
	{
		// Stop the engine.
		if(spEngine != NULL)
			spEngine->Stop();
		spEngine.Release();

		// Disable the product and display error dialog.
		setProductTampered(nErrorId);

		CCTRACEE( "CPlugin::Run() - NAVOPTRF is missing. Display error and exit." );

		return;
	}
	// END: Bring up the NAV Opt Refresh Engine
	//////////////////////////////////////////////

    // Wait for exit - must pump messages!
    //

    CCTRACEI("Run - listening...");

    ccLib::CMessageLock msgLock ( TRUE, FALSE );
    msgLock.Lock ( m_eventShutdown, INFINITE );

	//////////////////////////////////////////////
	// Cleanup NAVOptRf
	if(spEngine != NULL)
		spEngine->Stop();
	spEngine.Release();

    return;
}

//////////////////////////////////////////////////////////////////////
// virtual void RequestStop();

void CPlugin::RequestStop()
{
    m_eventShutdown.SetEvent ();
}

//////////////////////////////////////////////////////////////////////
// virtual void CanStopNow();

bool CPlugin::CanStopNow()
{
	return true;
}

//////////////////////////////////////////////////////////////////////
// virtual void GetObjectName();

const char * CPlugin::GetObjectName()
{
	return m_szObjectName;
}

//////////////////////////////////////////////////////////////////////
// virtual unsigned long OnMessage( int iMessage, unsigned long ulParam );

unsigned long CPlugin::OnMessage( int iMessage, unsigned long ulParam )
{
	return 0;
}

HRESULT CPlugin::setProductTampered(long nErrorId)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		CNAVOptSettingsCache OptFile;
		if(OptFile.Init(NULL, FALSE))
		{
			OptFile.SetValue(DRM::szDRMLicenseState			,DWORD(DJSMAR00_LicenseState_Violated));
			OptFile.SetValue(DRM::szDRMLicenseZone			,DWORD(DJSMAR_LicenseZone_Violated));

			CBusinessRules Rules;
			Rules.Calc(&OptFile);

			OptFile.Save();

			CGlobalEvent OptionsEvent;
#ifdef _UNICODE
			if (OptionsEvent.CreateW(SYM_OPTIONS_CHANGED_EVENT))
#else  _UNICODE
			if (OptionsEvent.Create(SYM_OPTIONS_CHANGED_EVENT))
#endif  _UNICODE
			{
				PulseEvent(OptionsEvent);
			}
		}

		// Show error dialog.
		errorMessageBox(nErrorId);
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);
}

HRESULT CPlugin::errorMessageBox(long nErrorId)
{
	StahlSoft::HRX hrx;
	HRESULT hr = S_OK;
	TCHAR szTitle[128] = {0};
	TCHAR szText[1024] = {0};

	try
	{
		// Get a NAVError object
		CComBSTR bstrNAVErr(_T("NAVError.NAVCOMError"));
		CComPtr <INAVCOMError> spNavError;

		// Create, Populate, Log, and Display the error
		hrx << spNavError.CoCreateInstance(bstrNAVErr, NULL, CLSCTX_INPROC_SERVER);
		hrx << spNavError->put_ModuleID(AV_MODULE_ID_NAVOPTREFRESH);		// Module ID of AV_MODULE_ID_NAVOPTREFRESH
		hrx << spNavError->put_ErrorID(nErrorId); // Error ID is the resource ID for the error message
		hrx << spNavError->put_ErrorResourceID(IDS_NAVERROR_INTERNAL_REINSTALL);
		hrx << spNavError->LogAndDisplay(0);
	}
	catch(_com_error& e)
	{
		hr = e.Error();
	}
	catch(...)
	{
		hr = E_UNEXPECTED;
	}

	return hr;
}

// Defect 1-4IVIK0
// Turn off settings manager's backup file for performance.
//
void CPlugin::disableSettingsBackup()
{
    NAVToolbox::CCSettings settingsMgr;
    if ( settingsMgr.Init())
    {
    	ccSettings::ISettingsPtr pSettings;
        if ( SYM_SUCCEEDED ( settingsMgr.CreateSettings ( CCSETMGR_BACKUP_KEY,
	                          	                          &pSettings)))
        {
		    DWORD dwValue = 0;
            SYMRESULT sr = SYMERR_UNKNOWN;

		    sr = pSettings->GetDword(CCSETMGR_BACKUP_VALUE, dwValue);
		    if (SYM_SUCCEEDED(sr) && (dwValue == 0))
		    {
			    // Already disabled; Nothing more to do
			    return;
		    }
		    else
		    {
			    // Need to disable
			    sr = pSettings->PutDword(CCSETMGR_BACKUP_VALUE, 0);
			    if (SYM_SUCCEEDED(sr))
			    {
                    settingsMgr.PutSettings (pSettings);
                    CCTRACEI ("CPlugin::disableSettingsBackup - disabled ccSettingsManager backups");
                }
            }
        }
    }

}
