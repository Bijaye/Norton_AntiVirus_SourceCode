#include "StdAfx.h"
#include "plugin.h"
#include "IWPSettingsInterface.h"
#include "FirewallAlerts.h"
#include "AVRESBranding.h"
#include "IDSManager.h"
#include "SymIDSI.h"
#include "NAVEventCommon.h" // For CNAVEventCommonQIPtr
#include "OSInfo.h"
#include "Subscription.h"

#include "navapnotify.h"        // For AP tray icon event

const char IWP_MUTEX_SESSIONWIDE [] = "IWP_MUTEX_SESSIONWIDE";
const char IWP_MUTEX_MACHINEWIDE [] = "IWP_MUTEX_MACHINEWIDE";

CPlugin::CPlugin(void) : m_lEndDate(0)
{
	CCTRACEI("CPlugin()");

    m_szObjectName = "IWP";
#ifdef _DEBUG
        ::MessageBox ( NULL, "IWP.DLL", "starting", MB_OK );
#endif

}

CPlugin::~CPlugin(void)
{
}

//////////////////////////////////////////////////////////////////////
// CPlugin::Run( ICCApp* pCCApp );

void CPlugin::Run( ICCApp* pCCApp )
{
    CCTRACEI ("CPlugin::Run - starting");

    // Kill event is not there. We won't be able to respond to
	// shut downs.
	//
    if (!m_eventShutdown.Create ( NULL, FALSE, FALSE, NULL, FALSE ))
    {
        CCTRACEE ( "CPlugin::Run() - Failed to create shutdown event" );
        return;
    }

    // Make sure we are the only StatusHelper running for this session.
    //
    ccLib::CMutex mutexIWP;

    if ( !mutexIWP.Create ( NULL, TRUE, IWP_MUTEX_SESSIONWIDE, FALSE ))
    {
		CCTRACEE ( "CPlugin::Run() - Failed to create IWP sessionwide mutex" );
        return; // fatal
    }

	DWORD dwRet = ::WaitForSingleObject(mutexIWP,0);
	if(dwRet != WAIT_OBJECT_0)
	{
		// someone else owns this mutex, so we close our handle to it only.
		// DO NOT RELEASE unless we own it.
        CCTRACEE ( "CPlugin::Run() - IWP already running" );
		return;
	}

	ccLib::CMessageLock msgLock ( TRUE, FALSE);

#ifndef _DEBUG // only do this in release mode
    // Wait for the AP tray icon to be up.
    //
    ccLib::CEvent eventAPTrayIcon;

    if ( eventAPTrayIcon.Create ( NULL, TRUE, FALSE, SYM_NAV_AP_TRAY_ICON_EVENT, FALSE ))
    {
        std::vector<HANDLE> vecWaitEvents;
        vecWaitEvents.push_back (m_eventShutdown);
        vecWaitEvents.push_back (eventAPTrayIcon);

        // We don't care why we woke up, just go
        ccLib::CMessageLock msgLock (TRUE, TRUE);

        DWORD dwWaitResult = msgLock.Lock ( (DWORD)vecWaitEvents.size(), &vecWaitEvents[0], FALSE, 30*1000, FALSE);
        if ( dwWaitResult == WAIT_TIMEOUT )
            CCTRACEW ("CPlugin::Run() - timeout waiting for tray");
        
        if ( dwWaitResult == WAIT_OBJECT_0)
        {
            CCTRACEI ("CPlugin::Run() - Told to exit");
            return;
        }
        else
            CCTRACEI ("CPlugin::Run() - Tray icon up");            
    }
    else
        CCTRACEE ("CPlugin::Run() - failed opening tray event");
#endif // _DEBUG

    // scope for memory
    {
        CSMHelper SMHelper;

        if ( !SMHelper.IsSettingsManagerInstalled())
        {
            CCTRACEE ( "CPlugin::Run () - SettingsManager is not installed!" );
            return;
        }
    
        CCTRACEI ("CPlugin::Run - waiting for SM");
        
        // Wait for SM
        //
        if ( !SMHelper.IsSettingsManagerActive (true))
        {
            CCTRACEE ( "CPlugin::Run () - Isn't active" );
            return;
        }
    }

    // Take a ref count on the object since this object is on the stack
    CFirewallAlerts fwAlerts;
    fwAlerts.AddRef();

    // We need COM for EventManager, need STA for licensing
    ccLib::CCoInitialize COM (ccLib::CCoInitialize::eSTAModel);

    // Get subscription data and pass to IDS
    //  
    if ( initSubData ())
    {
        subscribeToIDS();

        m_pEMSubscriber = std::auto_ptr <CEMSubscriber> (new CEMSubscriber (this));
        m_pEMSubscriber->AddRef();

        ccEvtMgr::CSubscriberHelper::SubscriberInfo subinfo;
        subinfo.m_nEventType = AV::Event_ID_StatusLicensing;
        subinfo.m_nPriority = 0;
	    subinfo.m_pEventStates = NULL;
	    subinfo.m_nEventStatesCount = 0;
        subinfo.m_nProviderId = 0;
        subinfo.m_bMonitorOnly = false;

        m_pEMSubscriber->m_vecSubscriberInfo.push_back (subinfo);

        // Start listening for events
        if ( !m_pEMSubscriber->Connect())
        {
            CCTRACEE ("EMSubscriber - connect failed");
        }
    }

    // Start the SymFirewallAgent to listen for firewall events.
    // If it's not supposed to run it will sit idle.
    //
    CCTRACEI ("CPlugin::Run - creating");
    fwAlerts.Create ( NULL, 0, 0 );

    // Wait for exit
    //
	CCTRACEI("Run - listening...");
    m_eventNewSubscription.Create ( NULL, FALSE, FALSE, NULL, FALSE);

    std::vector <HANDLE> vecWaitHandles;
    vecWaitHandles.push_back (m_eventShutdown);
    vecWaitHandles.push_back (m_eventNewSubscription);

    for (;;)
    {
        DWORD dwWaitResult = msgLock.Lock ( vecWaitHandles.size(), &vecWaitHandles[0], FALSE, INFINITE, FALSE );
        if ( WAIT_OBJECT_0 + 1 == dwWaitResult )
        {
            // New Subscription
            subscribeToIDS();
        }
        else
            break;
    }

    CCTRACEI ("CPlugin::Run - exiting");
    fwAlerts.Terminate (INFINITE);
    CCTRACEI ("CPlugin::Run - exited");

    return;
}

bool CPlugin::initSubData (void)
{
    // Query the data directly from subscription during startup to avoid
    // timing issues.
    //
    CSubscription subscription;
    CSubscription::LICENSE_SUB_DATA subdata;
    subdata = subscription.GetData ();
    m_lEndDate = (long) subdata.dwEndDate;
    return true;
}

bool CPlugin::subscribeToIDS(void)
{
    time_t timeExpiredDateTime = 0;

    // If the end date is zero, don't compute a date. 
    // Send 0 to IDS. 0 means all good, and no expiration date.
    //
    if ( 0 != m_lEndDate )
    {
        DATE dateEnd = (DATE) m_lEndDate;

        SYSTEMTIME stimeExpiredDate;
        VariantTimeToSystemTime ( dateEnd, &stimeExpiredDate);			
        
        // Systemtime to time_t
        //
        struct tm tmTime;
        ZeroMemory ( &tmTime, sizeof (tm));
        tmTime.tm_year = stimeExpiredDate.wYear - 1900;
        tmTime.tm_mon = stimeExpiredDate.wMonth - 1; // 0-11 months...
        tmTime.tm_mday = stimeExpiredDate.wDay;

        timeExpiredDateTime = mktime ( &tmTime );
        CCTRACEI ("Subscribing to IDS - %d/%d/%d", tmTime.tm_mon, tmTime.tm_mday, tmTime.tm_year);
    }
    else
        CCTRACEI ("Subscribing to IDS - no date, sending 0");

    CIDSManager IDSManager;
    SYMRESULT result = IDSManager.Subscribe ( timeExpiredDateTime );
    if ( SYM_SUCCEEDED (result))
        CCTRACEI ("Subscribing to IDS - success");
    else
    {
        CCTRACEE ("Subscribing to IDS - error %d", result);
        return false;
    }

    return true;
}

void CPlugin::EMSubscriberOnEvent (const ccEvtMgr::CEventEx& Event,
                                         ccEvtMgr::CSubscriberEx::EventAction& eAction)
{
    CNAVEventCommonQIPtr pNAVEvent (&Event);
    assert (pNAVEvent != NULL);
    if ( pNAVEvent )
    {
        long lSessionID = 0;
        pNAVEvent->props.GetData(AV::Event_Base_propSessionID, lSessionID );
        
        if ( COSInfo::GetSessionID() == lSessionID )
        {
            CEventData edSubData;
            if (!pNAVEvent->props.GetNode ( AVStatus::propNewStatus, edSubData ))
            {
                CCTRACEE ("No status bag");
            }
            else
            {
                long lEndDate = 0;
                if ( edSubData.GetData ( AVStatus::propSubDate, lEndDate ))
                {
                    if (lEndDate == m_lEndDate )
                        CCTRACEI ("CPlugin::EMSubscriberOnEvent - sub date is the same", m_lEndDate);
                    else
                    {
                        m_lEndDate = lEndDate;
                        m_eventNewSubscription.SetEvent();
                    }
                }
                else
                    CCTRACEW ("CPlugin::EMSubscriberOnEvent - error getdata");
            }
        }
    }
}

void CPlugin::EMSubscriberOnShutdown ()
{
}


//////////////////////////////////////////////////////////////////////
// virtual void RequestStop();

void CPlugin::RequestStop()
{
    CCTRACEI ("CPlugin::RequestStop");
    m_eventShutdown.SetEvent ();
}

//////////////////////////////////////////////////////////////////////
// virtual void CanStopNow();

bool CPlugin::CanStopNow()
{
    CCTRACEI ("CPlugin::CanStopNow");
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
