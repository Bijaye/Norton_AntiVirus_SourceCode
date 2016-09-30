#include "StdAfx.h"
#include ".\firewallalerts.h"
#include "SymNetDrvAlertEventEx.h"
#include "AlertHandlerIP.h"
#include "AlertHandlerTrojan.h"
#include "AlertHandlerIDS.h"
#include "IWPSettings.h"
#include "IDSManager.h"
#include "FWManager.h"
#include "ccLoginInterface.h"
#include "ccLoginLoader.h"
#include "OSInfo.h"
#include "ccEveryoneDacl.h"
#include "ResourceHelper.h"
#include "Resource.h"

#define IWP_MUTEX_SETTINGS "IWP_MUTEX_SETTINGS"

CFirewallAlerts::CFirewallAlerts(void)
{
    // Init COM MTA-style
    m_Options.m_eCOMModel = ccLib::CCoInitialize::eMTAModel;
    m_Options.m_bPumpMessages = 1;
    m_dwID = 0;
    m_bIsRunning = false;

    // For our null dacl
    ATL::CDacl nulldacl;
    ATL::CSecurityDesc secdesc;
    ATL::CSecurityAttributes secatt;

    // Set our null dacl
    //
    if ( nulldacl.IsEmpty())
    {
        nulldacl.SetNull();
        secdesc.SetDacl (nulldacl);
        secatt.Set (secdesc); // This throws assertions in debug mode, I know.
    }

    // If I use a EveryoneDACL, or NullDacl, the clients (IWP) get access denied
    // when they try to create the event on their end.
    //
    // Reset the owner
    if ( !m_eventResetAgent.Create ( &secatt,
                                    TRUE,
                                    FALSE,
                                    IWP::SYM_REFRESH_IWP_OPTIONS_EVENT,
                                    TRUE))
    {
        CCTRACEE ("CFirewallAlerts - unable to create event");
        assert(false);
    }
}

CFirewallAlerts::~CFirewallAlerts(void)
{
    CCTRACEI ("~CFirewallAlerts");
}


int CFirewallAlerts::Run ()
{
    CCTRACEI ("CFirewallAlerts::Run () - starting");

    m_pSettings = std::auto_ptr <CIWPSettings> (new CIWPSettings ());

    // Try to start
    //
    if (isEnabled ())
    {
        if (start())
            m_pSettings->SetState (IWP::IIWPSettings::IWPStateEnabled);
        else
            m_pSettings->SetState (IWP::IIWPSettings::IWPStateError);
    }
    else
    {
        // Signal our OFF state
        m_pSettings->SetState ( IWP::IIWPSettings::IWPStateDisabled );
    }

    HANDLE hEvents[] = { m_Terminating.GetHandle(), m_eventResetAgent.GetHandle() };

    ccLib::CMessageLock msgLock (TRUE, FALSE);

    while ( true )
    {
        DWORD dwWaitObject = msgLock.Lock ( 2, &hEvents[0], FALSE, INFINITE, FALSE );

        if ( dwWaitObject == WAIT_FAILED )
        {
            assert (false);
            CCTRACEE ( "CFirewallAlerts::Run () - wait failed" );
            return -1;
        }

        if ( dwWaitObject != WAIT_OBJECT_0 )
        {
            // Reload the settings.
            //
            if ( m_pSettings.get ())
            {
                m_pSettings.reset ();
                m_pSettings = std::auto_ptr <CIWPSettings> (new CIWPSettings ());
            }

            // State changed, update
            //
            if (isEnabled ())
            {
                    if (start())
                        m_pSettings->SetState (IWP::IIWPSettings::IWPStateEnabled);
                    else
                        m_pSettings->SetState (IWP::IIWPSettings::IWPStateError);
            }
            else
            {
                    if (stop())
                        m_pSettings->SetState ( IWP::IIWPSettings::IWPStateDisabled );
                    else
                        m_pSettings->SetState (IWP::IIWPSettings::IWPStateError);
            }
        }
        else
        {
            // Exit
            //
            if (stop ())
            {
                // Don't turn our state to OFF since other accounts might be logged in
                // and this is a global setting.
            }
            else
                m_pSettings->SetState ( IWP::IIWPSettings::IWPStateError );

            break;
        }
    }
    CCTRACEI ("CFirewallAlerts::Run () - exiting");

    return 1;
}

bool CFirewallAlerts::isEnabled (void)
{
    // Ask the settings if we should turn on/off
    DWORD dwValue = 0;
    
    // Has DRM told us to be off?
    m_pSettings->GetValue ( IWP::IIWPSettings::IWPCanEnable, dwValue );
    
    if (!dwValue )
        return false;

    // What ever the user wants
    //
    m_pSettings->GetValue ( IWP::IIWPSettings::IWPUserWantsOn, dwValue );
    return ( dwValue );
}

bool CFirewallAlerts::start (void)
{
    CCTRACEI ("CFirewallAlerts::start");

    if ( m_bIsRunning )
        return true;

    // Log the default user in.
    //
    login ();

    bool bReturn = false;
    SYMRESULT symResult = SYM_OK;

    if ( !m_pSymFwAgt.m_p )
    {
        // Need a loader
        //
        if ( !m_pFWLoader.get())
            m_pFWLoader = std::auto_ptr <SymFWAgtLoader> (new SymFWAgtLoader);

        symResult = m_pFWLoader->CreateObject ( &m_pSymFwAgt );

        if ( SYM_FAILED ( symResult) ||
            !m_pSymFwAgt )
        {
            CCTRACEE ("CFirewallAlerts::start - failed to load SymFWAgt - 0x%x", symResult);
            m_pSymFwAgt = NULL;
            return false;
        }
    }

    symResult = m_pSymFwAgt->Initialize ();
    if ( SYM_SUCCEEDED(symResult) )
    {
        // Turn on the firewall
        //
        DWORD dwFirewallOn = 1;
        if ( SYM_SUCCEEDED (m_pSettings->GetValue (IWP::IIWPPrivateSettings::IWPFirewallOn, dwFirewallOn )))
		    CCTRACEI("CFirewallAlerts::start - Checking for FirewallOn Returned: %d", dwFirewallOn);
        else
            CCTRACEE("CFirewallAlerts::start - failed getting IWPFirewallOn");

        {
            // Turn the whole feature on, unless we don't want the firewall.
            //
            CFWManager firewall;
            symResult = firewall.SetFirewallSettings (true);

            if ( SYM_FAILED (symResult ))
                CCTRACEE("CFirewallAlerts::start - failed turning feature on");
            
            if ( !dwFirewallOn )
            {
                symResult = firewall.SetFirewallSettings (false, false, true);
                if ( SYM_FAILED (symResult ))
                    CCTRACEE("CFirewallAlerts::start - failed turning Firewall off");
            }
        }

        if ( SYM_SUCCEEDED(symResult))
        {
            // StartSubscriber is used to start the Subscriber and auto Rule creation
	        // The subscriber runs on the calling thread, so be sure to initialize COM in MTA
	        // And pump messages as needed
            symResult = m_pSymFwAgt->StartSubscriber();
	        if (SYM_SUCCEEDED(symResult) )
            {
                m_dwID = 0;

                symResult = m_pSymFwAgt->RegisterAlert(this, m_dwID);
                if ( SYM_SUCCEEDED(symResult))
                {
                    // Everything is set up correctly. Now just let it run.
                    //
                    bReturn = true;
                }
                else
                    CCTRACEE ( "CFirewallAlerts::start - Failed to register alert - 0x%x", symResult);
            }
            else
                CCTRACEE ("CFirewallAlerts::start - Failed to start SymFwAgt subscriber - 0x%x", symResult);
        }
        else
        {
            CCTRACEE ("CFirewallAlerts::start - Failed to turn on firewall - 0x%x", symResult);
        }

        if ( !bReturn )
        {
            // Unload it completely.
            //
            m_pSymFwAgt->Destroy ();
            m_pSymFwAgt.Release ();
        }
    }
    else
    {
        // Clean up failed Init.
        CCTRACEE ("CFirewallAlerts::start - Failed to init SymFwAgt - 0x%x", symResult);
        m_pSymFwAgt.Release ();
    }

    // Start the IDS def watcher
    m_IDSUpdateWatcher.Create ( NULL, 0, 0);

	// Turn on IDS
    //
    CIDSManager IDSManager;
    SYMRESULT result = IDSManager.Enable (true); 
    if ( SYM_FAILED (result))
    {
        CCTRACEE ("CFirewallAlerts::start() - Failed to enable IDS - 0x%x", result);
        bReturn = false;
    }

    CCTRACEI ("CFirewallAlerts::start - done - %d", bReturn);

    m_bIsRunning = bReturn;

    return bReturn;
}

bool CFirewallAlerts::stop ()
{
    CCTRACEI ("CFirewallAlerts::stop");

    bool bReturn = true;

    if ( m_bIsRunning && m_pSymFwAgt.m_p )
    {
        if ( !m_pSymFwAgt->UnRegisterAlert(m_dwID))
        {
            CCTRACEE ("CFirewallAlerts::stop - Failed to Unregister alert");
            bReturn = false;
        }

        // Unload it completely.
        //
        CCTRACEI ("CFirewallAlerts::stop - destroying SymFwAgt");
        m_pSymFwAgt->Destroy ();
        CCTRACEI ("CFirewallAlerts::stop - destroyed SymFwAgt");
        m_pSymFwAgt.Release ();
        
        // Unload the loader
        if ( m_pFWLoader.get() )
            m_pFWLoader.reset ();

        m_IDSUpdateWatcher.Terminate(INFINITE); // wait for it

        // Log the default user off.
        //
        logout();
    }

    CCTRACEI ("CFirewallAlerts::stop - done - %d", bReturn);
    
    // If this succeeds we are off otherwise keep the same state.
    if ( bReturn )
        m_bIsRunning = false;

    return bReturn;
}

bool CFirewallAlerts::OnDisplayAlert(IAlertData* pAlertData)
{
    // Validate
    if ( !pAlertData )
    {
        CCTRACEE ("CFirewallAlerts::OnDisplayAlert - bad alert data");
        return false;
    }

    // Get the alert type
    IAlertData::ALERT_TYPE eAlertType;
	if(!pAlertData->GetAlertType(eAlertType))
	{
		CCTRACEE(_T("CFirewallAlerts::OnDisplayAlert : Failed to get alert type\n"));
		return false;
	}

    // Get the alert event
    CSymPtr<ccEvtMgr::CEventEx> pEvent;
    if(!pAlertData->GetEvent(pEvent.m_p))
    {
		CCTRACEE(_T("CFirewallAlerts::OnDisplayAlert : Failed to get event\n"));
		return false;
    }

    switch ( eAlertType )
    {
	    case IAlertData::ISALERTTYPE_IDS:
	    {
            CAlertHandlerIDS AlertHelper;
            AlertHelper.DisplayAlert(pEvent);
            return true;
	    }
        break;

	    case IAlertData::ISALERTTYPE_LISTEN:    // Apps listening on ports
        case IAlertData::ISALERTTYPE_IP:        // Traffic coming into opened ports
        {
            // Are we hiding listen events?

            if ( IAlertData::ISALERTTYPE_LISTEN == eAlertType )
            {
                DWORD dwListenAlerts = 0;
                m_pSettings->GetValue ( IWP::IIWPPrivateSettings::IWPListenAlerts, dwListenAlerts );
                if ( !dwListenAlerts )
                {
                    pAlertData->SetAlertResult (IAlertData::NO_ACTION);
                    CCTRACEI ( "CFirewallAlert::OnDisplayAlert - allowing listen event" );
                    return true;
                }
            }

            // Does an ALE exist for this?
            //
            bool bAleExists = false;
			bool bAleCoverTraffic = false;
            char szAppName [MAX_PATH] = {0};
            DWORD dwAppNameSize = MAX_PATH;

            if ( !pAlertData->GetAleInfo(bAleExists, bAleCoverTraffic, szAppName, dwAppNameSize))
            {
                CCTRACEW ("CFirewallAlerts::OnDisplayAlert - GetAleInfo() failed");
            }

            if (bAleExists && bAleCoverTraffic)
            {
                // Check ALE rules against incoming traffic to make sure the rules will
                // allow the traffic.
                pAlertData->SetAlertResult (IAlertData::CREATE_ALE_RULES);
                CCTRACEI ( "CFirewallAlert::OnDisplayAlert - ALE matches traffic" );
                return true;
            }
            else
            {
                if ( bAleExists )
                    CCTRACEI ("CFirewallAlerts::OnDisplayAlert - ALE exists but doesn't match the traffic");

                // Evaluate if the ALEs can match or if we have nothing at all.
                // If we have a close match, evaluate the rules to see if they
                // will cover the traffic. If they do, apply silently. If they
                // don't, send an alert to the user.

	            // Get the event
                CSymQIPtr<SymNeti::CSNAlertEventEx, &SymNeti::IID_SNAlertEventEx> pSNEvent;

                // QI the Event
                pSNEvent = pEvent;

	            if(!pSNEvent)
                {
                    // Not a valid event
                    CCTRACEE ("CFirewallAlert::OnDisplayAlert - invalid alert data");
                    return false;
                }

                CAlertHandlerIP AlertHelper;
                IAlertData::ALERT_RESULT result = AlertHelper.DisplayAlert(pSNEvent, szAppName, eAlertType);
                pAlertData->SetAlertResult (result);
                return true;
            }
        } // end IP and Listen alerts
        break;

        // Trojans
        case IAlertData::ISALERTTYPE_SECURITY:  
        {
	        // Get the event
            CSymQIPtr<SymNeti::CSNAlertEventEx, &SymNeti::IID_SNAlertEventEx> pSNEvent;

            // QI the Event
            pSNEvent = pEvent;

	        if(!pSNEvent)
            {
                // Not a valid event
                CCTRACEE ("CFirewallAlert::OnDisplayAlert - invalid alert data");
                return false;
            }

            // Get the app name
            //
            char szAppName [MAX_PATH] = {0};
            DWORD dwAppNameSize = MAX_PATH;
            bool bAleExists = false;
			bool bAleCoverTraffic = false;
            if ( !pAlertData->GetAleInfo(bAleExists, bAleCoverTraffic, szAppName, dwAppNameSize))
            {
                CCTRACEW ("CFirewallAlerts::OnDisplayAlert - GetAleInfo() failed");
            }

            CAlertHandlerTrojan AlertHelper;
            IAlertData::ALERT_RESULT result = AlertHelper.DisplayAlert(pSNEvent, szAppName, eAlertType);
            pAlertData->SetAlertResult (result);
            return true;
        }
        break;
    }

    return false;
}

void CFirewallAlerts::login ()
{
    CCTRACEI ("CFirewallAlerts::login - starting");    

    // Log in the default user
    //
    cc::ccLogin_ILogin loginLoader;
    cc::ILoginPtr pLogin;

    if ( SYM_SUCCEEDED (loginLoader.CreateObject (&pLogin)))
    {
        if (pLogin->Initialize())
        {
            if (pLogin->SetLoginScheme(cc::LOGIN_SCHEME_CUSTOM)) // NAS, NIS, etc. all use custom
            {
                if ( pLogin->LoginUser(-1, ""))  // This session
                    CCTRACEI ("CFirewallAlerts::login - login successful");
                else
                    CCTRACEE ("CFirewallAlerts::login - failed LoginUser");
            }
            else
                CCTRACEE ("CFirewallAlerts::login - failed SetLoginScheme");
        }
        else
            CCTRACEE ("CFirewallAlerts::login - failed to init ccLogin");
    }
    else
        CCTRACEE ("CFirewallAlerts::login - failed to load ccLogin");
}

void CFirewallAlerts::logout ()
{
    // Log out the default user
    //
    cc::ccLogin_ILogin loginLoader;
    cc::ILoginPtr pLogin;

    if ( SYM_SUCCEEDED (loginLoader.CreateObject (&pLogin)))
    {
        if (pLogin->Initialize())
        {
                if ( pLogin->LogoffUser(-1))  // This session
                    CCTRACEI ("CFirewallAlerts::logout - logout successful");
                else
                    CCTRACEE ("CFirewallAlerts::logout - failed LogoffUser");
        }
        else
            CCTRACEE ("CFirewallAlerts::logout - failed to init ccLogin");
    }
    else
        CCTRACEE ("CFirewallAlerts::logout - failed to load ccLogin");
}
/*
void CFirewallAlerts::SetIDSState (bool bIDSOK)
{
    m_bIDSOK = bIDSOK;

    // If IDS is bad, and we already have settings (meaning we already started), change our state
    if ( !m_bIDSOK && m_pSettings.get())
        m_pSettings->SetState (IWP::IIWPSettings::IWPStateError);
}*/