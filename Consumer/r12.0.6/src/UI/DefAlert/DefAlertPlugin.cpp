#include "StdAfx.h"
#include "resource.h"
#include "..\defalertres\resource.h"
#include "DefAlert.h"
#include "Simon.h"
#include "SSOsinfo.h"
#include "NAVOptHelperEx.h"
#include "SymAlertInterface.h"
#include "NavOptionRefreshHelperInterface.h"
#include "InstOptsNames.h"
#include "NavLicenseNames.h"
#include "DefAlertPlugin.h"
#include "AgentPluginIDs.h"
#include "GlobalEvents.h"
#include "AutoUpdateDefines.h"
#include "NetDetectController.h"                // For AutoLiveUpdate
#include "navtrust.h"
#include "NAVUpgradeAlert.h"
#include "ccSymInterfaceLoader.h"
#include "ccAlertInterface.h"
#define _NAVOPTREFRESH_CONSTANTS
#include "NavOptRefresh.h"
#include "TraceHR.h"
#include "ccEveryoneDacl.h"
#include "AVRESBranding.h"

#include "ccResourceLoader.h"
extern cc::CResourceLoader g_ResLoader;

#include <vector>
#include "navapnotify.h" // For AP tray icon event


static const TCHAR SESSIONWIDE_DEF_ALERT_MUTEX[]	= _T("SESSIONWIDE_DEF_ALERT_MUTEX_{3FD9B483-1106-4388-95A3-D8FE49D36458}");

//////////////////////////////////////////////////////////////////////
// CDefAlertPlugin::CDefAlertPlugin()

CDefAlertPlugin::CDefAlertPlugin() : m_bInit(false)
{
	CCTRACEI("CDefAlertPlugin()");

	m_bInitTimeOut = TRUE;
	m_bCfgwizFinished = FALSE;
	m_bAggregated = FALSE;
	m_LicenseType = DJSMAR_LicenseType_Violated;
	m_LicenseState = DJSMAR00_LicenseState_Violated;
	m_SubStatus = STATUS_SUBSCRIPTION_OK;
	//m_hWnd = NULL;
	//m_atomWndClass = NULL;
	m_dwRepeatTimeOut = _5_MINUTES;
	m_uLicenseAlertMsgID = ::RegisterWindowMessage(MSG_NAV_LICENSING_ALERT);

	// Create the event which will be used to stop the DefAlert and ALU threads.
	m_shKillEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// Open the LiveUpdate event.
	m_VirusDefsLUEvent.Create(SYM_REFRESH_VIRUS_DEF_STATUS_EVENT);

	// License updated event.
	m_LicenseRefreshedEvent = ::CreateEvent(NULL, TRUE, FALSE, szNAVOptionRefreshEngine_DefAlertLicenseChange);

	// License updated event.
	m_SubsRefreshedEvent = ::CreateEvent(NULL, TRUE, FALSE, SYM_REFRESH_SUBSCRIPTION_STATUS_EVENT);

    m_pAlertList = NULL;
	m_shutdown = false;
}

//////////////////////////////////////////////////////////////////////
// CDefAlertPlugin::~CDefAlertPlugin()

CDefAlertPlugin::~CDefAlertPlugin()
{
}

//////////////////////////////////////////////////////////////////////
// CDefAlertPlugin::Run( ICCApp* pCCApp );

void CDefAlertPlugin::Run( ICCApp* pCCApp )
{
	TRACEHR (hrx);

	CCTRACEI("Run()");
    
	// Kill event is not there. We won't be able to respond to
	// shut downs.
	//
	if ( !m_shKillEvent || m_shutdown)
	{
		CCTRACEW("Run() cannot run!");
		return;
	}

	// Have we been told to shut down?
	//
	DWORD dwRet = ::WaitForSingleObject(m_shKillEvent,0);

	if(dwRet == WAIT_OBJECT_0)
		return;

	StahlSoft::CSmartHandle shMutex;

    // Use a NULL dacl so that all users can open this mutex
    ccLib::CEveryoneDACL everyonedacl;
    if ( !everyonedacl.Create() )
        CCTRACEE( "CPlugin::Run() - Failed to create the NULL dacl" );

	shMutex = ::CreateMutex( everyonedacl, FALSE, SESSIONWIDE_DEF_ALERT_MUTEX);
	if(shMutex == NULL)
		return; // fatal

	dwRet = ::WaitForSingleObject(shMutex,0);
	if(dwRet != WAIT_OBJECT_0)
	{
		// someone else owns this mutex, so we close our handle to it only.
		// DO NOT RELEASE unless we own it.
		return;
	}

#ifndef _DEBUG
    // Wait for the AP tray icon to be up.
    //
    ccLib::CEvent eventAPTrayIcon;

    if ( eventAPTrayIcon.Create ( NULL, TRUE, FALSE, SYM_NAV_AP_TRAY_ICON_EVENT, FALSE ))
    {
        std::vector<HANDLE> vecWaitEvents;
        vecWaitEvents.push_back (m_shKillEvent);
        vecWaitEvents.push_back (eventAPTrayIcon);

        // We don't care why we woke up, just go
        ccLib::CMessageLock msgLock (TRUE, TRUE);

        DWORD dwWaitResult = msgLock.Lock ( (DWORD)vecWaitEvents.size(), &vecWaitEvents[0], FALSE, 30*1000, FALSE);
        if ( dwWaitResult == WAIT_TIMEOUT )
            CCTRACEW ("CDefAlertPlugin::Run() - timeout waiting for tray");

        if ( dwWaitResult == WAIT_OBJECT_0)
            CCTRACEI ("CDefAlertPlugin::Run() - Told to exit");
        else
            CCTRACEI ("CDefAlertPlugin::Run() - Tray icon up");
    }
    else
        CCTRACEE ("CDefAlertPlugin::Run() - failed opening tray event");
#endif

	// StahlSoft::HRX hrx; -- Replaced with TraceHR
	HRESULT hrCoInit = E_FAIL;
	try
	{
		hrCoInit = ::CoInitialize(NULL);

		// check upfront if all the wanted events are created.

		hrx %= "Check for events";

		hrx << ((HANDLE(m_shKillEvent)          == HANDLE(NULL))?E_HANDLE:S_OK);
		hrx << ((HANDLE(m_VirusDefsLUEvent)      == HANDLE(NULL))?E_HANDLE:S_OK);

        // Get alert.dat file path and the current user name.

	    _GetAlertDataFile();
	    _GetUserName();

     	// Create DefAlert window to catch subscription changed message.
		WNDCLASSEX wc;

		wc.cbSize			= sizeof(WNDCLASSEX);
		wc.style			= 0;
		wc.lpfnWndProc		= DefWindowProc;
		wc.cbClsExtra		= 0;
		wc.cbWndExtra		= 0;
		wc.hInstance		= _Module.GetModuleInstance();
		wc.hIcon			= NULL;
		wc.hCursor			= NULL;
		wc.hbrBackground	= NULL;
		wc.lpszMenuName		= NULL;
		wc.lpszClassName	= SZ_DEFALERT_WNDCLASSNAME;
		wc.hIconSm			= NULL;

		m_atomWndClass = ::RegisterClassEx(&wc);

		if(m_atomWndClass)
		{
			m_hWnd = CreateWindow(SZ_DEFALERT_WNDCLASSNAME,
				SZ_DEFALERT_WNDNAME,
				WS_OVERLAPPED,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				NULL,
				NULL,
				_Module.GetModuleInstance(),
				NULL );
		}

		// Start monitoring NAV status and alerts
	    CCTRACEI("Run - listening...");
		Monitor( this );

		CCTRACEI("Stopping plug-in");

		if(m_hWnd)
		{
			if (IsWindow(m_hWnd))
			{
				CCTRACEI("DestroyWindow");
				::DestroyWindow(m_hWnd);
			}
			else
			{
				CCTRACEI("Window has been destroyed already!");
			}
		}

		if(m_atomWndClass)
		{
			CCTRACEI("UnregisterClass");
			::UnregisterClass(SZ_DEFALERT_WNDCLASSNAME, _Module.GetModuleInstance());
		}

	}
	catch(_com_error& e)
	{
		hrx = e;
		// CCTRACEE(_T("CDefAlertPlugin::ThreadProc - Exception while creating alert objects %08X"), e.Error());
	}
	catch(...)
	{
		CCTRACEE("*** Error");
	}

	if(SUCCEEDED(hrCoInit))
	{
		::CoUninitialize();
	}

	if(shMutex != NULL)
	{
		::ReleaseMutex(shMutex);
	}

    if (m_pAlertList)
    {
        delete m_pAlertList;
        m_pAlertList = NULL;
    }

	CCTRACEI("Plug-in stopped");
}

//////////////////////////////////////////////////////////////////////
// virtual void RequestStop();

void CDefAlertPlugin::RequestStop()
{
	CCTRACEI("RequestStop");

	m_shutdown = true;

	if ( m_shKillEvent )
	{
		CCTRACEI("Set kill event");
		SetEvent( m_shKillEvent );
	}
}

//////////////////////////////////////////////////////////////////////
// virtual void CanStopNow();

bool CDefAlertPlugin::CanStopNow()
{
	return true;
}

//////////////////////////////////////////////////////////////////////
// virtual void GetObjectName();

const char * CDefAlertPlugin::GetObjectName()
{
	return "DefAlert";
}

//////////////////////////////////////////////////////////////////////
// virtual unsigned long OnMessage( int iMessage, unsigned long ulParam );

unsigned long CDefAlertPlugin::OnMessage( int iMessage, unsigned long ulParam )
{
	return 0;
}

//////////////////////////////////////////////////////////////////////
// CDefAlertPlugin::OnVirusDefsLUEvent()

VOID CDefAlertPlugin::OnVirusDefsLUEvent()
{
	CCTRACEI("VirusDefsLUEvent");

	// Refresh Virus Defs counts.
	m_spDefinitions->Refresh(TRUE);

	// Virus defintions are being updated.
	//  Show the alert only if the event is fired by AutoLiveUpdate.
	//  Note that the event is fired before virus definitions update completes.

	// Check to see if the event is fired by ALU
	//  or by LU launched by the user.

	// Build correct mutex name.
	StahlSoft::CSmartHandle smMutex;
	smMutex = OpenMutex(SYNCHRONIZE, FALSE, SYM_AUTO_UPDATE_MUTEX);
	if(smMutex != NULL)
	{
		// Open the mutex SYM_AUTO_UPDATE_MUTEX successfully means
		//  AutoLiveUpdate creates the mutex to update virus definitions.

		// Show notification right away.
		//  We don't need to queue this alert.
		ShowALUCompleteNotification();
	}
}

//////////////////////////////////////////////////////////////////////
// CDefAlertPlugin::CheckVirusDefsStatus()

VOID CDefAlertPlugin::CheckVirusDefsStatus()
{
	StahlSoft::HRX hrx;

	try
	{
		// Check to see if we have any alert.
		SYMALERT_TYPE Type = SYMALERT_TYPE_INVALID;
		BOOL bShow = FALSE;
		hrx << m_spDefinitions->ShouldShowAlert(&bShow, &Type, TRUE, SYMALERT_TYPE_OldDefs);
		if(bShow)
		{
			ALERT_ITEM alert = {0};
			_LookupAlertMap(SYMALERT_TYPE_OldDefs,alert.alertMap);
			m_pAlertList->Insert(alert);
		}
	}
	catch(_com_error&)
	{
		CCTRACEE(_T("CDefAlertPlugin::CheckVirusDefsStatus() - Exception while accessing Definitions Alert object"));
	}
}

//////////////////////////////////////////////////////////////////////
// CDefAlertPlugin::CheckLicensingStatus()

HRESULT CDefAlertPlugin::CheckLicensingStatus(CHECKLICENSE_REQUESTTYPE rqt, HWND hWndParent)
{
	HRESULT hr = S_OK;
	StahlSoft::HRX hrx;

	try
	{
		ALERT_ITEM alert = {0};
		alert.hWndParent = hWndParent;

		SYMALERT_TYPE Type = SYMALERT_TYPE_INVALID;
		BOOL bShow = FALSE;
		BOOL bCheckTimer = TRUE;

		if(rqt == CHECKLICENSE_ONDEMAND)
			bCheckTimer = FALSE;

		// Check to see if there is a licensing alert.
		hrx << m_spLicense->ShouldShowAlert(&bShow, &Type, bCheckTimer, 0);

		if(bShow)
		{
			_LookupAlertMap(Type, alert.alertMap);
			m_pAlertList->Insert(alert);
		}

		// Cache license state,
		m_spLicense->GetLicenseState(&m_LicenseState);
		m_spLicense->GetLicenseType(&m_LicenseType);
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("CDefAlertPlugin::CheckLicensingStatus() - Exception while accessing licensing alert object %08X"), hr);
	}

	return hr;
}

//////////////////////////////////////////////////////////////////////
// CDefAlertPlugin::CheckVirusSubscriptionStatus()

VOID CDefAlertPlugin::CheckVirusSubscriptionStatus()
{
	StahlSoft::HRX hrx;

	try
	{
		// Check to see if we have any alert.
		SYMALERT_TYPE Type = SYMALERT_TYPE_INVALID;
		BOOL bShow = FALSE;
		hrx << m_spSubscription->ShouldShowAlert(&bShow, &Type, TRUE, SYMALERT_TYPE_SubExpired | SYMALERT_TYPE_SubWarning);
		if(bShow)
		{
			ALERT_ITEM alert = {0};
			_LookupAlertMap(Type, alert.alertMap);
			m_pAlertList->Insert(alert);
		}

		// Cache subscription status.
		m_spSubscription->GetSubscriptionStatus(&m_SubStatus);
	}
	catch(_com_error& e)
	{
		// Failed to get subscription info (i.e. LS may be busy.)
		//  Assume ok since we will check it again in 6 hrs later.
		CCTRACEE(_T("CDefAlertPlugin::CheckVirusSubscriptionStatus - Exception while accessing subscription object %08X"), e.Error());
	}
}

//////////////////////////////////////////////////////////////////////
// CDefAlertPlugin::OnTimeOut()

VOID CDefAlertPlugin::OnTimeOut()
{
	CCTRACEI(CCTRCTX "OnTimeOut >>");

	// If DefAlert option enabled, check virus
	//  definitions and subscription status.
    bool bActiveSession = true;

    // The first time we are guaranteed to be the active session, so don't bother checking.
    // We have seen this IsActiveSession() call lie to us on certain machines. If we don't
    // assume we are Active the first time we get into an infinite loop.
    // See defects # 420537 and 418042.
    //
    DWORD dwError = ERROR_SUCCESS;
    bActiveSession = COSInfo::IsActiveSession( dwError );

    if ( dwError != ERROR_SUCCESS)
    {
        bActiveSession = true;
        CCTRACEE(CCTRCTX _T("IsActiveSession error, fixing %d"), dwError);
    }
    else
        CCTRACEI(CCTRCTX _T("No errors"));

    if ( bActiveSession )
	{
        CCTRACEI(CCTRCTX _T("Active session"));

		CheckLicensingStatus(CHECKLICENSE_ONTIMER, NULL);

		// Nag subscription and virus defs out of date
		//  only if CfgWiz has finished.
		//  Wait for 5 minutes after startup.
		if(IsNAVCfgWizFinished())
		{
			switch(m_LicenseType)
			{
            case DJSMAR_LicenseType_OEM:
            case DJSMAR_LicenseType_Retail:
			case DJSMAR_LicenseType_Unlicensed:
			case DJSMAR_LicenseType_Trial:
			case DJSMAR_LicenseType_ESD:
			case DJSMAR_LicenseType_Beta:
				if(m_LicenseState == DJSMAR00_LicenseState_PURCHASED)
				{
					CheckVirusSubscriptionStatus();
				}
				break;

			case DJSMAR_LicenseType_Rental:
				{
					if(m_LicenseState == DJSMAR00_LicenseState_PURCHASED)
					{
						CNAVUpgradeAlert UpgradeAlert;
						UpgradeAlert.CheckProductUpgrade();
					}
				}
				break;

			default:
				break;
			}

			// Check virus defs only if license is valid
			//  and virus subscription is not expired.
			if((m_LicenseState == DJSMAR00_LicenseState_PURCHASED) ||
				(m_LicenseState == DJSMAR00_LicenseState_TRIAL))
			{
				if(m_SubStatus != STATUS_SUBSCRIPTION_EXPIRED)
					CheckVirusDefsStatus();
			}
		}
    }
    else
	{
        CCTRACEE(CCTRCTX _T("Not the active session"));
	}

	// Adjust repeat time out.
	m_dwRepeatTimeOut = GetRepeatTimeOut();

	CCTRACEI(CCTRCTX _T("OnTimeOut <<"));
}

//////////////////////////////////////////////////////////////////////
// CDefAlertPlugin::GetRepeatTimeOut()

DWORD CDefAlertPlugin::GetRepeatTimeOut()
{
	if(!m_bInitTimeOut)
		return m_dwRepeatTimeOut;

	StahlSoft::HRX hrx;
	DWORD dwTimeOut = _6_HOURS;

	try
	{
		DWORD dwValue = _GetAlertDword(ALERT_RepeatTimeOut, _6_HOURS, FALSE);
		DWORD dwCycleLicWarning = 0;
		DWORD dwCycleLicExpired = 0;
		DWORD dwCycleSubWarning = 0;
		DWORD dwCycleSubExpired = 0;

		hrx << m_spLicense->GetExpiredCyle(&dwCycleLicExpired);
		hrx << m_spLicense->GetWarningCycle(&dwCycleLicWarning);
		hrx << m_spSubscription->GetExpiredCyle(&dwCycleSubExpired);
		hrx << m_spSubscription->GetWarningCycle(&dwCycleSubWarning);

		DWORD dwaTimeOut[] = {dwValue
			,dwCycleSubWarning
			,dwCycleSubExpired
			,dwCycleLicWarning
			,dwCycleLicExpired
		};

		int iCount = sizeof(dwaTimeOut) / sizeof(long);

		for(int i=0; i<iCount; ++i)
		{
			if(dwTimeOut > dwaTimeOut[i])
				dwTimeOut = dwaTimeOut[i];
		}
	}
	catch(_com_error& e)
	{
		dwTimeOut = 6 * _1_HOUR;
		CCTRACEE(CCTRCTX _T("Exception while accesing license and subscription objects %08X"), e.Error());
	}

	m_dwRepeatTimeOut = dwTimeOut;
	m_bInitTimeOut = FALSE;

	return dwTimeOut;
}

//////////////////////////////////////////////////////////////////////
// CDefAlertPlugin::OnLicenseRefreshed()

HRESULT CDefAlertPlugin::OnLicenseRefreshed()
{
	CCTRACEI("OnLicenseRefreshed");

	HRESULT hr = S_OK;
	StahlSoft::HRX hrx;

	try
	{
		// Check to see if license has been activated successfully.
		long nZone = 0;
		bool bWarning = false;

		hrx << m_spLicense->Refresh(0);
		hrx << m_spLicense->GetLicenseState(&m_LicenseState);
		hrx << m_spLicense->GetLicenseZone(&nZone);
		bWarning = ((nZone & DJSMAR_LicenseZone_Warning) == DJSMAR_LicenseZone_Warning);

		if(m_LicenseState == DJSMAR00_LicenseState_PURCHASED && !bWarning)
		{
			// Reset user's settings for "Notify me in x days" to default value;
			DWORD dwCycleLicWarning = 0;
			DWORD dwCycleLicExpired = 0;
			hrx << m_spLicense->GetExpiredCyle(&dwCycleLicExpired);
			hrx << m_spLicense->GetWarningCycle(&dwCycleLicWarning);
			_SetCollectionAlertDword(ALERT_LicenseExpiredCycle, dwCycleLicExpired);
			_SetCollectionAlertDword(ALERT_LicenseWarningCycle, dwCycleLicWarning);
		}
		else
		{
			CCTRACEI(_T("CDefAlertPlugin::OnLicenseRefreshed"));
			CheckLicensingStatus(CHECKLICENSE_ONREFRESHED, NULL);
		}
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("OnLicenseRefreshed - Exception 0x%08X"), hr);
	}

	CCTRACEI("OnLicenseRefreshed -> 0x%08X", hr);

	return hr;
}

//////////////////////////////////////////////////////////////////////
// CDefAlertPlugin::IsNAVCfgWizFinished()

bool CDefAlertPlugin::IsNAVCfgWizFinished()
{
	if(m_bCfgwizFinished)
		return m_bCfgwizFinished;

	StahlSoft::HRX hrx;

	try
	{
		// Check CfgWiz.dat to see if CFGWIZ_Finished = 1

		CNAVInfo NAVInfo;
		TCHAR szCfgWizDat[MAX_PATH] = {0};
		DWORD dwValue = 0;
		CNAVOptFileEx cfgWizFile;

		::wsprintf(szCfgWizDat, _T("%s\\CfgWiz.dat"), NAVInfo.GetNAVDir());
		hrx << (cfgWizFile.Init(szCfgWizDat, FALSE)? S_OK : E_FAIL);
		cfgWizFile.GetValue(InstallToolBox::CFGWIZ_Finished, dwValue, 0);
		m_bCfgwizFinished = (dwValue == 0) ? false : true;
	}
	catch(_com_error& e)
	{
		CCTRACEE(_T("CDefAlertPlugin::IsNAVCfgWizFinished - Exception while accesing cfgwiz.dat %08X"), e.Error());
	}

	return m_bCfgwizFinished;
}

//////////////////////////////////////////////////////////////////////
// CDefAlertPlugin::OnSubscriptionChanged()

HRESULT CDefAlertPlugin::OnSubscriptionChanged()
{
	HRESULT hr = S_OK;
	StahlSoft::HRX hrx;

	try
	{
		BOOL bCheckALU = FALSE;

		switch(m_LicenseType)
		{
		case DJSMAR_LicenseType_Retail:
		case DJSMAR_LicenseType_Unlicensed:
		case DJSMAR_LicenseType_Trial:
		case DJSMAR_LicenseType_ESD:
		case DJSMAR_LicenseType_Beta:
			if(m_LicenseState == DJSMAR00_LicenseState_PURCHASED)
			{
				bCheckALU = TRUE;
			}
			break;

		default:
			return S_OK;
			break;
		}

		if(bCheckALU)
		{
			// Check if subscription has been renewed.
			hrx << m_spSubscription->Refresh(0);
			hrx << m_spSubscription->GetSubscriptionStatus(&m_SubStatus);
			if(m_SubStatus != STATUS_SUBSCRIPTION_OK)
				return S_OK;

			// Get subscription old end date.
			DWORD dwOldEndDate = 0;
			dwOldEndDate = _GetAlertDword(ALERT_SubsEndDate, 0, FALSE);

			// Get subscription new end date.
			DWORD dwNewEndDate = 0;
			DATE EndDate;
			hrx << m_spSubscription->GetSubscriptionEndDate(&EndDate);
			dwNewEndDate = (DWORD) EndDate;

			if(dwNewEndDate <= dwOldEndDate)
				return S_OK;

			// Subscription just renewed!!!

			// Save new end date
			_SetAlertDword(ALERT_SubsEndDate, dwNewEndDate, FALSE);

			// Clear flag "Don't show subscription expired alert".
			DWORD  dwCycleSubExpired = 0;
			m_spSubscription->GetExpiredCyle(&dwCycleSubExpired);
			_SetCollectionAlertDword(ALERT_SubsExpiredCycle, dwCycleSubExpired);
			_SetCollectionAlertDword(ALERT_ScanSubsExpiredCycle, 0);

			// If ALU is disabled, display a warning.

			// If it is a suite child product, don't display ALU disabled nag.
			//  Let the parent product display it.
			if(m_bAggregated)
				return S_OK;

			BOOL bShow = FALSE;
			SYMALERT_TYPE Type = SYMALERT_TYPE_INVALID;
			SIMON::CSimonPtr<ISymAlert> spALU;
			hrx << SIMON::CreateInstanceByDLLInstance(_Module.GetModuleInstance(), CLSID_CALUDisabledAlert, IID_ISymAlert,(void**)&spALU);
			hrx << spALU->ShouldShowAlert(&bShow, &Type, FALSE, 0);
			if(bShow)
			{
				ALERT_ITEM alert = {0};
				_LookupAlertMap(SYMALERT_TYPE_ALUDisabled, alert.alertMap);
				m_pAlertList->Insert(alert);
			}
		}
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("CDefAlertPlugin::OnSubscriptionChanged - Exception while accessing ALU alert object %08X"), hr);
	}

	return hr;
}

//////////////////////////////////////////////////////////////////////
// CDefAlertPlugin::ShowALUCompleteNotification()

HRESULT CDefAlertPlugin::ShowALUCompleteNotification()
{
	HRESULT hr = S_OK;
	StahlSoft::HRX hrx;

	try
	{
		CString csText;
		CString csOK;

		g_ResLoader.LoadString(IDS_ALU_NOTIFICATION, csText);
		g_ResLoader.LoadString(IDS_OK, csOK);

		TCHAR szccAlert[MAX_PATH] = {0};
		CNAVInfo NAVInfo;
		::wsprintf(szccAlert, _T("%s\\ccAlert.dll"), NAVInfo.GetSymantecCommonDir());

		CBrandingRes BrandRes;
		CSymInterfaceLoader SymIntHelper;
		CSymPtr<cc::INotify> spNotify;
		hrx << ((SymIntHelper.Initialize(szccAlert) != SYM_OK)? E_FAIL : S_OK);
		hrx << ((SymIntHelper.CreateObject(cc::IID_Notify, cc::IID_Notify, (void**)&spNotify) != SYM_OK)? E_FAIL : S_OK);

		spNotify->SetProperty(cc::INotify::PROPERTY_TASKBAR_ENTRY, false);
		spNotify->SetProperty(cc::INotify::PROPERTY_OK_BUTTON, true);
		spNotify->SetButton(cc::INotify::BUTTONTYPE_OK, csOK);
		spNotify->SetText(csText);
		spNotify->SetTitle(BrandRes.ProductName());
		spNotify->SetTimeout(20000);
		spNotify->Display(NULL);
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("CDefAlertPlugin::ShowALUCompleteNotification - Exception while accessing ccAlert"), hr);
	}

	return hr;
}

//////////////////////////////////////////////////////////////////////
// Former CDefAlertPlugin::ThreadProc()

unsigned __stdcall CDefAlertPlugin::Monitor(void* pArgument)
{
	TRACEHR (hr);

	CDefAlertPlugin* pThis = (CDefAlertPlugin*) pArgument;

	try
	{
		// Setup the events that we will wait on.
		HANDLE Handles[4] = {0};
		Handles[EVENT_QUIT] =           pThis->m_shKillEvent;
		Handles[EVENT_VIRUSDEFS_LU] =    pThis->m_VirusDefsLUEvent;
		Handles[EVENT_LICENSE_REFRESHED] = pThis->m_LicenseRefreshedEvent;
		Handles[EVENT_SUBSCRIPTION_REFRESHED] = pThis->m_SubsRefreshedEvent;

		LONG nCount = (sizeof(Handles)/sizeof(HANDLE));
		DWORD dwTimeOut = 0;
		DWORD dwTimeElapsed = 0;
		DWORD dwStartTickCount = GetTickCount();
        bool bStartup = true;

		pThis->m_dwRepeatTimeOut = _GetAlertDword(ALERT_StartTimeOut, _5_MINUTES, FALSE);

		while (!pThis->m_shutdown)
		{
			MSG msg;
			while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if(msg.message == WM_QUIT)
				{
					goto BailOut;
				}
				else if(msg.message == pThis->m_uLicenseAlertMsgID)
				{
                    if ( !pThis->m_bInit )
                        pThis->delayedInit();

					CHECKLICENSE_REQUESTTYPE rqt = CHECKLICENSE_ONDEMAND;
					long nFlag = (long) msg.wParam;

					if((nFlag & REQUEST_LICENSING_ALERT_ON_CHANGE) == REQUEST_LICENSING_ALERT_ON_CHANGE)
					{
						rqt = CHECKLICENSE_ONREFRESHED;
					}

					CCTRACEI(_T("CDefAlertPlugin::Monitor - CheckLicensingStatus on Main UI"));
					pThis->CheckLicensingStatus(rqt, (HWND) msg.lParam);
				}
				else
				{
					DispatchMessage(&msg);
				}
			}

			// Everytime thread wakes up for any reason,
			//  we need to decrement the timeout to make sure
			//  we wake up on startup, next 5 mins, every next 6 hours.
			// Compute time elapsed
			dwTimeElapsed = GetTickCount() - dwStartTickCount;
			LONG lTimeDiff = (pThis->m_dwRepeatTimeOut * 1000) - dwTimeElapsed;

			if(lTimeDiff > 0)
			{
				dwTimeOut = lTimeDiff;
			}
			else
			{
				dwTimeOut = 0;
			}

			CCTRACEI("CDefAlertPlugin::Monitor - dwTimeOut: %d", dwTimeOut);

			DWORD dwWait = MsgWaitForMultipleObjects(nCount, Handles, FALSE, dwTimeOut, QS_ALLINPUT);

			CCTRACEI("CDefAlertPlugin::Monitor - Do work - %d", dwWait);

			switch(dwWait)		// It should be (dwWait - STATUS_WAIT_0), but see WAIT_TIMEOUT. Let's leave as is for now.
			{
			case EVENT_QUIT:
				// The kill event was signaled. It's time to exit!
			default:
				CCTRACEI("CDefAlertPlugin::Monitor - Quit");
				goto BailOut;
				// FATAL ERROR
				break;

			case EVENT_VIRUSDEFS_LU:
                if ( !pThis->m_bInit )
                    pThis->delayedInit();

				// Liveupdate has occurred.
				CCTRACEI(_T("CDefAlertPlugin::Monitor - On virus definition update event"));
				pThis->OnVirusDefsLUEvent();
				break;

			case EVENT_LICENSE_REFRESHED:
                if ( !pThis->m_bInit )
                    pThis->delayedInit();

				// Licensing status changed, check licensing status
				//  and display an alert if neccessary.
				CCTRACEI(_T("CDefAlertPlugin::Monitor - On license refresh event"));
				pThis->OnLicenseRefreshed();
				break;

			case EVENT_SUBSCRIPTION_REFRESHED:
                if ( !pThis->m_bInit )
                    pThis->delayedInit();

				// Subscription status changed, check subscription status
				//  and display an ALU disabled alert if neccessary.
				CCTRACEI(_T("CDefAlertPlugin::Monitor - On subscription refresh event"));
				pThis->OnSubscriptionChanged();
				break;

			case EVENT_WINDOWS_MSG:
				CCTRACEI(_T("Windows message"));
				continue;

			case WAIT_TIMEOUT:
				CCTRACEI(_T("CDefAlertPlugin::Monitor - On time out"));
				dwStartTickCount = GetTickCount();

                // If we are in the startup waiting period, just wait a little
                if ( bStartup )
                {
                    bStartup = false;
                    break;
                }
                
                if ( !pThis->m_bInit )
                    pThis->delayedInit ();

				pThis->OnTimeOut();
				break;
			}
		}

BailOut:
		CCTRACEI("CDefAlertPlugin::Monitor - Stopping monitor.");

	}
	catch(_com_error& e)
	{
		hr = e;
	}
	catch(...)
	{
		hr = E_UNEXPECTED;
	}

	return SUCCEEDED(hr)?0:1;
}

void CDefAlertPlugin::delayedInit ()
{
	TRACEHR (hrx);

    m_bInit = true;

	hrx %= "Create LicensingAlert";
	hrx << SIMON::CreateInstanceByDLLInstance(_Module.GetModuleInstance(), CLSID_CLicensingAlert, IID_ILicensingAlert,(void**)&m_spLicense);

	hrx	%= "Create SubscriptionAlert";
	hrx << SIMON::CreateInstanceByDLLInstance(_Module.GetModuleInstance(), CLSID_CSubscriptionAlert, IID_ISubscriptionAlert,(void**)&m_spSubscription);

	hrx %= "Create DefinitionsAlert";
	hrx << SIMON::CreateInstanceByDLLInstance(_Module.GetModuleInstance(), CLSID_CDefinitionsAlert, IID_ISymAlert,(void**)&m_spDefinitions);

	// Get licensing info. This is needed
	//  because license is set to violated in constructor.
	m_spLicense->Init();
	m_spLicense->Refresh(0);
	m_spLicense->GetLicenseType(&m_LicenseType);
	m_spLicense->GetLicenseState(&m_LicenseState);
	m_spLicense->IsAggregated(&m_bAggregated);

	m_spSubscription->Init();
	m_spDefinitions->Init();

	// Initialize AlertList
	hrx %= "Initialize AlertList";

    m_pAlertList = new CAlertList;

    if ( !m_pAlertList )
        return;

	m_pAlertList->Initialize(NULL);
}
