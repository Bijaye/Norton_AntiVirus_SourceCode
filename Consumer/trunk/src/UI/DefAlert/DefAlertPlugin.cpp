////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

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
#include "ccEveryoneDacl.h"
#include "ccOSInfo.h"
#include "ISVersion.h"
#include "isResource.h"
#include "uiNISDataElementGuids.h"
#include "DefHelper.h"
#include "ErrorHandling.h"          // For components that report errors states in real-time

#include "ccResourceLoader.h"
extern cc::CResourceLoader g_ResLoader;

#include <vector>

static const TCHAR SESSIONWIDE_DEF_ALERT_MUTEX[]    = _T("SESSIONWIDE_DEF_ALERT_MUTEX_{3FD9B483-1106-4388-95A3-D8FE49D36458}");

//////////////////////////////////////////////////////////////////////
// CDefAlertPlugin::CDefAlertPlugin()

CDefAlertPlugin::CDefAlertPlugin() : m_bInit(false)
{
    CCTRCTXI0(_T("Enter"));

    m_bInitTimeOut = TRUE;
    m_bAggregated = FALSE;
    m_LicenseType = clt::cltLicenseType_Violated;
    m_LicenseAttrib = clt::cltLicenseAttrib_Violated;
    m_dwRepeatTimeOut = _5_MINUTES;

    // Create the event which will be used to stop the DefAlert and ALU threads.
    m_KillEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    // Open the LiveUpdate event.
    m_DefUpdatedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    // License updated event.
    m_LicenseRefreshedEvent = ::CreateEvent(NULL, TRUE, FALSE, szNAVOptionRefreshEngine_DefAlertLicenseChange);

    // License updated event.
    m_SubsRefreshedEvent = ::CreateEvent(NULL, TRUE, FALSE, SYM_REFRESH_SUBSCRIPTION_STATUS_EVENT);

    m_pAlertList = NULL;
    m_shutdown = false;
    m_bDefUpdateRegistered = FALSE;
    ZeroMemory (&m_stDefDate, sizeof (SYSTEMTIME));
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
    assert(false);

    CCTRCTXI0(_T("Run()"));
    
    // Kill event is not there. We won't be able to respond to
    // shut downs.
    //
    if ( !m_KillEvent || m_shutdown)
    {
        CCTRCTXW0(_T("Run() cannot run!"));
        return;
    }

    // Have we been told to shut down?
    //
    DWORD dwRet = ::WaitForSingleObject(m_KillEvent,0);

    if(dwRet == WAIT_OBJECT_0)
        return;

    StahlSoft::CSmartHandle shMutex;

    // Use a NULL dacl so that all users can open this mutex
    ccLib::CEveryoneDACL everyonedacl;
    if ( !everyonedacl.Create() )
        CCTRACEE(_T("CPlugin::Run() - Failed to create the NULL dacl"));

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

    StahlSoft::HRX hrx;
    HRESULT hrCoInit = E_FAIL;
    try
    {
        hrCoInit = ::CoInitialize(NULL);

        // check upfront if all the wanted events are created.

        CCTRCTXI0(L"Check for events");

        hrx << ((HANDLE(m_KillEvent)          == HANDLE(NULL))?E_HANDLE:S_OK);
        hrx << ((HANDLE(m_DefUpdatedEvent)      == HANDLE(NULL))?E_HANDLE:S_OK);

        // Get alert.dat file path and the current user name.

        _GetAlertDataFile();
        _GetUserName();

        // Kick off error handling routines. Poke the tiger to see if it's alive.
        //
        CSymPtr<CErrorHandling> spErrorHandling = new CErrorHandling();
        spErrorHandling->Create(NULL, 0, 0);

        // Start monitoring NAV status and alerts
        CCTRCTXI0(_T("Run - listening..."));
        Monitor( this );

        // Unregister this object for definitions changed event
        // Registration is done in delayInit()
        if(m_bDefUpdateRegistered)
        {
            CCTRCTXI0(L"Unsubscribe definitions changed event");
            m_spDefUpdate->Unregister(this);
        }

		CCTRCTXI0(_T("Stopping ErrorHandlling thread"));

		if(!spErrorHandling->Terminate(10 * 1000))
		{
			CCTRCTXI0(L"ErrorHandling thread was terminated on time out");
		}

        CCTRCTXI0(_T("Stopping plug-in"));
    }
    catch(_com_error& e)
    {
        CCTRCTXE1(L"Exception thrown. HR: 0x%08X", e.Error());
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

    CCTRCTXI0(_T("Plug-in stopped"));
}

//////////////////////////////////////////////////////////////////////
// virtual void RequestStop();

void CDefAlertPlugin::RequestStop()
{
    CCTRCTXI0(_T("RequestStop"));

    m_shutdown = true;

    if ( m_KillEvent )
    {
        CCTRCTXI0(_T("Set kill event"));
        SetEvent( m_KillEvent );
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
    CCTRCTXI0(_T("VirusDefsLUEvent"));

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

HRESULT CDefAlertPlugin::CheckLicensingStatus(HWND hWndParent)
{
    CCTRCTXI0(L"Enter");

    HRESULT hr = S_OK;
    StahlSoft::HRX hrx;

    try
    {
        ALERT_ITEM alert = {0};
        alert.hWndParent = hWndParent;

        SYMALERT_TYPE Type = SYMALERT_TYPE_INVALID;
        BOOL bShow = FALSE;
        BOOL bCheckTimer = TRUE;

        // Check to see if there is a licensing alert.
        hrx << m_spLicense->ShouldShowAlert(&bShow, &Type, bCheckTimer, 0);

        if(bShow)
        {
            _LookupAlertMap(Type, alert.alertMap);
            m_pAlertList->Insert(alert);
        }

        // Cache license state,
        m_spLicense->GetLicenseAttribute(&m_LicenseAttrib);
        m_spLicense->GetLicenseType(&m_LicenseType);
    }
    catch(_com_error& e)
    {
        hr = e.Error();
        CCTRCTXE1(L"Exception while accessing licensing alert object %08X", hr);
    }

    CCTRCTXI0(L"Exit");
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
    CCTRCTXI0(_T("Enter"));

    // If DefAlert option enabled, check virus
    //  definitions and subscription status.
    ccLib::COSInfo OSInfo;

    if ( OSInfo.IsActiveSession() )
    {
        CCTRACEI(CCTRCTX _T("Active session"));

        CheckLicensingStatus(NULL);

        // Nag subscription and virus defs out of date
        //  only if CfgWiz has finished.
        //  Wait for 5 minutes after startup.
        if(_IsCfgWizFinished())
        {
            CCTRCTXI0(L"Cfgwiz is finished");
            switch(m_LicenseType)
            {
            case clt::cltLicenseType_OEM:
            case clt::cltLicenseType_Activation:
            case clt::cltLicenseType_Unlicensed:
            case clt::cltLicenseType_TryBuy:
            case clt::cltLicenseType_ESD:
            case clt::cltLicenseType_Beta:
                if((m_LicenseAttrib & clt::cltLicenseAttrib_Active) == 
                    clt::cltLicenseAttrib_Active)
                {
                    CCTRCTXI0(L"Check subscription status");
                    CheckVirusSubscriptionStatus();
                }
                else
                {
                    CCTRCTXI0(L"Skip checking subscription status because license has not been activated");
                }
                break;

            case clt::cltLicenseType_SOS:
                if((m_LicenseAttrib & clt::cltLicenseAttrib_Active) == 
                    clt::cltLicenseAttrib_Active)
                {
                    CCTRCTXI0(L"Check product upgrade");
                    CNAVUpgradeAlert UpgradeAlert;
                    UpgradeAlert.CheckProductUpgrade();
                }
                else
                {
                    CCTRCTXI0(L"Skip checking product because license is not Active");
                }
                break;

            default:
                break;
            }

            // Check virus defs only if license is valid
            //  and virus subscription is not expired.
            if( ((m_LicenseAttrib & clt::cltLicenseAttrib_Active)
                    == clt::cltLicenseAttrib_Active) ||
                ((m_LicenseAttrib & clt::cltLicenseAttrib_Trial) 
                    == clt::cltLicenseAttrib_Trial) )
            {
                if((m_LicenseAttrib & clt::cltLicenseAttrib_Subscription_Expired) 
                    != clt::cltLicenseAttrib_Subscription_Expired)
                {
                    CCTRCTXI0(L"Check definition status");
                    CheckVirusDefsStatus();
                }
                else
                {
                    CCTRCTXI0(L"Skip checking definitions status because subscription has expired");
                }
            }
            else
            {
                CCTRCTXI0(L"Skip checking definitions status because license is not valid");
            }
        }
    }
    else
    {
        CCTRACEE(CCTRCTX _T("Not the active session"));
    }

    // Adjust repeat time out.
    m_dwRepeatTimeOut = GetRepeatTimeOut();

    CCTRCTXI0(L"OnTimeOut <<");
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
    CCTRCTXI0(_T("Enter"));

    HRESULT hr = S_OK;
    StahlSoft::HRX hrx;

    try
    {
        // Check to see if license has been activated successfully.
        long nZone = 0;
        bool bWarning = false;

        hrx << m_spLicense->Refresh(0);
        hrx << m_spLicense->GetLicenseAttribute(&m_LicenseAttrib);

        bWarning = ((m_LicenseAttrib & clt::cltLicenseAttrib_Subscription_Warning) ==
                        clt::cltLicenseAttrib_Subscription_Warning);

        if(m_LicenseAttrib == clt::cltLicenseAttrib_Active && !bWarning)
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
            CheckLicensingStatus(NULL);
        }
    }
    catch(_com_error& e)
    {
        hr = e.Error();
        CCTRACEE(_T("OnLicenseRefreshed - Exception 0x%08X"), hr);
    }

    CCTRCTXI1(_T("Exit. hr=0x%08X"), hr);

    return hr;
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
        case clt::cltLicenseType_Activation:
        case clt::cltLicenseType_Unlicensed:
        case clt::cltLicenseType_TryBuy:
        case clt::cltLicenseType_ESD:
        case clt::cltLicenseType_Beta:
            if((m_LicenseAttrib & clt::cltLicenseAttrib_Active) == clt::cltLicenseAttrib_Active)
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
            m_spLicense->GetLicenseAttribute(&m_LicenseAttrib);
            if((m_LicenseAttrib & clt::cltLicenseAttrib_Subscription_Active) 
                != clt::cltLicenseAttrib_Subscription_Active)
            {
                return S_OK;
            }

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
// CDefAlertPlugin::isSymSetupRunning()

bool CDefAlertPlugin::isSymSetupRunning()
{
    bool bSymSetup = false;
    ccLib::CMutex mtxSymSetup;
    if(FALSE != mtxSymSetup.Open(SYNCHRONIZE, FALSE, SYMSETUP_MUTEX, TRUE) ) 
    {
        CCTRCTXW0(_T("SymSetup is running"));
        bSymSetup = true;
    }
    return bSymSetup;
}

//////////////////////////////////////////////////////////////////////
// CDefAlertPlugin::ShowALUCompleteNotification()

HRESULT CDefAlertPlugin::ShowALUCompleteNotification()
{
    HRESULT hr = S_OK;
    if(isSymSetupRunning())
    {
        CCTRCTXW0(_T("notification not shown - SymSetup is running"));
        return hr;
    }
    
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

		ccLib::CString csCaption;
		CISVersion::LoadString(csCaption, IDS_SYMANTEC);

        CSymInterfaceLoader SymIntHelper;
        CSymPtr<cc::INotify> spNotify;
        hrx << ((SymIntHelper.Initialize(szccAlert) != SYM_OK)? E_FAIL : S_OK);
        hrx << ((SymIntHelper.CreateObject(cc::IID_Notify, cc::IID_Notify, (void**)&spNotify) != SYM_OK)? E_FAIL : S_OK);

        spNotify->SetProperty(cc::INotify::PROPERTY_TASKBAR_ENTRY, false);
        spNotify->SetProperty(cc::INotify::PROPERTY_OK_BUTTON, true);
        spNotify->SetButton(cc::INotify::BUTTONTYPE_OK, csOK);
        spNotify->SetText(csText);
		spNotify->SetTitle(csCaption);
        spNotify->SetTimeout(20000);
        spNotify->Display(NULL);
    }
    catch(_com_error& e)
    {
        hr = e.Error();
        CCTRCTXE1(_T("exception while accessing ccAlert:0x%08x"), hr);
    }

    return hr;
}

//////////////////////////////////////////////////////////////////////
// Former CDefAlertPlugin::ThreadProc()

unsigned __stdcall CDefAlertPlugin::Monitor(void* pArgument)
{
    int nRet = 0;

    CDefAlertPlugin* pThis = (CDefAlertPlugin*) pArgument;
    ccLib::CExceptionInfo exceptionInfo;

    try
    {
        // Setup the events that we will wait on.
        HANDLE Handles[4] = {0};
        Handles[EVENT_QUIT] =           pThis->m_KillEvent;
        Handles[EVENT_VIRUSDEFS_LU] =    pThis->m_DefUpdatedEvent;
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

            CCTRCTXI1(_T("dwTimeOut: %d"), dwTimeOut);

            DWORD dwWait = MsgWaitForMultipleObjects(nCount, Handles, FALSE, dwTimeOut, QS_ALLINPUT);

            CCTRCTXI1(_T("Do work - %d"), dwWait);

            switch(dwWait)      // It should be (dwWait - STATUS_WAIT_0), but see WAIT_TIMEOUT. Let's leave as is for now.
            {
            case EVENT_QUIT:
                // The kill event was signaled. It's time to exit!
            default:
                CCTRCTXI0(_T("Quit"));
                goto BailOut;
                // FATAL ERROR
                break;

            case EVENT_VIRUSDEFS_LU:
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
        CCTRCTXI0(_T("CDefAlertPlugin::Monitor - Stopping monitor."));

    }
    CCCATCHCOM(exceptionInfo)
    CCCATCHMEM(exceptionInfo)

    if(exceptionInfo.IsException())
    {
        CCTRCTXE1(L"Exception. Error: %s", exceptionInfo.GetDescription());
        nRet = 1;
    }

    return nRet;
}

void CDefAlertPlugin::delayedInit ()
{
    CCTRCTXI0(L"Enter");

    m_bInit = true;

    StahlSoft::HRX hrx;

    CCTRCTXI0(L"Create LicensingAlert");
    hrx << SIMON::CreateInstanceByDLLInstance(_Module.GetModuleInstance(), CLSID_CLicensingAlert, IID_ILicensingAlert,(void**)&m_spLicense);

    CCTRCTXI0(L"Create SubscriptionAlert");
    hrx << SIMON::CreateInstanceByDLLInstance(_Module.GetModuleInstance(), CLSID_CSubscriptionAlert, IID_ISubscriptionAlert,(void**)&m_spSubscription);

    CCTRCTXI0(L"Create DefinitionsAlert");
    hrx << SIMON::CreateInstanceByDLLInstance(_Module.GetModuleInstance(), CLSID_CDefinitionsAlert, IID_ISymAlert,(void**)&m_spDefinitions);

    // Get licensing info. This is needed
    //  because license is set to violated in constructor.
    m_spLicense->Init();
    m_spLicense->Refresh(0);
    m_spLicense->GetLicenseType(&m_LicenseType);
    m_spLicense->GetLicenseAttribute(&m_LicenseAttrib);
    m_spLicense->IsAggregated(&m_bAggregated);

    m_spSubscription->Init();
    m_spDefinitions->Init();

    // Initialize AlertList
    CCTRCTXI0(L"Initialize AlertList");

    m_pAlertList = new CAlertList;

    if ( !m_pAlertList )
        return;

    m_pAlertList->Initialize(NULL);

    // Due to performance issue and definitions are updated
    // a few minutes on startup, we delay subscription
    // for definitions updated.

    // Create the data provider object
    CCTRCTXI0(L"Create DataProviderLoader");
    hrx << (SYM_SUCCEEDED(m_spDataProviderLoader.CreateObject(GETMODULEMGR(), &m_spDataProvider))? S_OK : E_FAIL);

    // Get definitions element object.
    CCTRCTXI0(L"Get VirusDef element");
    hrx << m_spDataProvider->GetElement(ISShared::CLSID_NIS_VirusDefs, m_spDefElement);

    if ( m_spDefElement)
    {
        ui::IDataPtr pData;
        if ( SUCCEEDED (m_spDefElement->GetData (pData)))
        {
            ui::IDateDataQIPtr pDateData = pData;
            if ( pDateData )
            {
                pDateData->GetDate (m_stDefDate);
            }
        }
    }

    // QI for definitions update object 
    m_spDefUpdate = m_spDefElement;

    // Register this object for definitions changed event
    CCTRCTXI0(L"Subscribe for definitions changed event");
    hrx << m_spDefUpdate->Register(this, m_spDefElement);
    m_bDefUpdateRegistered = TRUE;

    CCTRCTXI0(L"Exit");
}

HRESULT CDefAlertPlugin::Update(IUpdate* pSource, LPVOID pCookie) throw()
{
    CCTRCTXI0(L"On Definitions Update event");

    // The update can happen even if the dates don't change in the
    // case of DRM or ConfigWiz changing. If ALU is running
    // when this happens we will think that the defs updated!
    //
    if ( m_spDefElement )
    {
        ui::IDataPtr pData;
        if ( SUCCEEDED (m_spDefElement->GetData (pData)))
        {
            ui::IDateDataQIPtr pDateData = pData;
            if ( pDateData )
            {
                SYSTEMTIME stDate;
                ZeroMemory ( &stDate, sizeof (SYSTEMTIME));

                if ( SUCCEEDED (pDateData->GetDate (stDate))
                    && memcmp (&m_stDefDate, &stDate, sizeof(SYSTEMTIME)) != 0)
                {
                    CCTRCTXI0(L"Defs updated");

                    m_stDefDate = stDate;

                    // Set event to display a notification
                    SetEvent(m_DefUpdatedEvent);
                }
            }
        }
    }
    return S_OK;
}
