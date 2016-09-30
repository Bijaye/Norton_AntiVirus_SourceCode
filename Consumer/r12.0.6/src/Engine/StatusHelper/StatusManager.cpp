#include "StdAfx.h"
#include ".\statusmanager.h"

#include "StateALU.h"
#include "StateAP.h"
#include "StateDefs.h"
#include "StateFSS.h"
#include "StateIWP.h"
#include "StateEmail.h"
#include "StateLicensing.h"
#include "StateIM.h"
#include "StateSpyware.h"

#include "navapnotify.h"    // For AP tray icon
#include "NAVEventCommon.h"

#include "GlobalMutex.h"
#include "avccModuleid.h"

CStatusManager::CStatusManager(void) :
    m_bInternalSubscriber(false)
{
    m_szObjectName = "StatusHelper";
    m_eventNewWork.Create ( NULL, FALSE, FALSE, NULL, FALSE ); // auto-reset
    m_eventEMShutdown.Create ( NULL, FALSE, FALSE, NULL, FALSE ); // auto-reset
}

CStatusManager::~CStatusManager(void)
{
    // Don't destroy objects while we are still processing. Wait for it.
    // 1-4K8GT1
    //
    ccLib::CSingleLock lockWorking(&m_critWorking, INFINITE, FALSE);
    m_shutdown = true;  // Set the shutdown flag, just in case. It is also set by the CPlugin
}

void CStatusManager::startup(void)
{
	// Turn on the listener
    startListening();

    // Build the listen events
    //
    CStateAP* pstateAP = new CStateAP (&m_WMI, &m_NSC);
    m_mapFeatures[pstateAP->m_lEventID] = pstateAP;
    pstateAP->Create(NULL, 0, 0);    // Start the thread. This will call Save()

    CStateALU* pstateALU = new CStateALU(&m_WMI, &m_NSC);
    m_mapFeatures[pstateALU->m_lEventID] = pstateALU;
    pstateALU->Create(NULL, 0, 0);

    CStateDefs* pstateDefs = new CStateDefs(&m_WMI, &m_NSC);
    m_mapFeatures[pstateDefs->m_lEventID] = pstateDefs;
    pstateDefs->Create(NULL, 0, 0);

    CStateEmail* pstateEmail = new CStateEmail(&m_WMI, &m_NSC);
    m_mapFeatures[pstateEmail->m_lEventID] = pstateEmail;
    pstateEmail->Create(NULL, 0, 0);

    CStateFSS* pstateFSS = new CStateFSS(&m_WMI, &m_NSC);
    m_mapFeatures[pstateFSS->m_lEventID] = pstateFSS;
    pstateFSS->Create(NULL, 0 ,0 );

    CStateIWP* pstateIWP = new CStateIWP(&m_WMI, &m_NSC);
    m_mapFeatures[pstateIWP->m_lEventID] = pstateIWP;
    pstateIWP->Create(NULL, 0, 0);

    CStateLicensing* pstateLic = new CStateLicensing(&m_WMI, &m_NSC);
    m_mapFeatures[pstateLic->m_lEventID] = pstateLic;
    pstateLic->Create(NULL, 0, 0);

    CStateIM* pstateIM = new CStateIM(&m_WMI, &m_NSC);
    m_mapFeatures[pstateIM->m_lEventID] = pstateIM;
    pstateIM->Create(NULL, 0, 0);

    CStateSpyware* pstateSpyware = new CStateSpyware(&m_WMI, &m_NSC);
    m_mapFeatures[pstateSpyware->m_lEventID] = pstateSpyware;
    pstateSpyware->Create(NULL, 0, 0);

    // Ask the WMI and NSC integration if it wants immediate status on any item
    //
    for ( iterStates iter = m_mapFeatures.begin ();
          iter != m_mapFeatures.end();
          iter++ )
    {
        CState* pState = iter->second;
        if ( pState )
        {
            if ( m_WMI.Subscribe (pState->m_lEventID) || m_NSC.Subscribe (pState->m_lEventID))
            {
                m_bInternalSubscriber = true;
            }
        }
    }

    if ( m_bInternalSubscriber )
    {
        // Query for the current state
        //
        CEventData edCurrentStatus;
        CState::GetCurrentStatus (edCurrentStatus);

        // Give the final whole package to WMI so it can process it all at once
        m_WMI.OnStatusChange ( 0, edCurrentStatus );

		// Don't talk to NSC - defect 1-4ARSR5
    }
}

void CStatusManager::shutdown(void)
{
    // Nuke the states
    // 
    for ( iterStates iter = m_mapFeatures.begin ();
          iter != m_mapFeatures.end();
          iter++ )
    {
        CState* pState = iter->second;
        if ( pState )
        {
            pState->Terminate(INFINITE);
            delete pState;
            pState = NULL;
        }
    }

    CCTRACEI ("Shutdown subscriber");
    // Shut down
    if ( m_pEMSubscriber.get() )
        m_pEMSubscriber->Disconnect();

    CCTRACEI ("Shutdown done!");
}

//////////////////////////////////////////////////////////////////////
// CStatusManager::Run( ICCApp* pCCApp );
//
// The thread that ccApp starts us on
//
void CStatusManager::Run( ICCApp* pCCApp )
{
	CCTRACEI("Run()");

	// Kill event is not there. We won't be able to respond to
	// shut downs.
	//
    if (!m_eventShutdown.Create ( NULL, TRUE, FALSE, NULL, FALSE ))
    {
        CCTRACEE( _T("CPlugin::Run() - Failed to create shutdown event" ));
        return;
    }

    // Make sure we are the only StatusHelper running for this session.
    //
    ccLib::CMutex mutexBeginning;

    if ( !mutexBeginning.Create ( NULL, TRUE, NAV_STATUS_HELPER_MUTEX_SESSIONWIDE_BEGIN, FALSE ))
    {
		CCTRACEE( _T( "CPlugin::Run() - Failed to create StatusHelper begin mutex" ));
        assert(false);
        return; // fatal
    }

	DWORD dwRet = ::WaitForSingleObject(mutexBeginning,0);
	if(dwRet != WAIT_OBJECT_0)
	{
		// someone else owns this mutex, so we close our handle to it only.
		// DO NOT RELEASE unless we own it.
        CCTRACEE( _T("CPlugin::Run() - StatusHelper already beginning" ));
		return;
	}

#ifndef _DEBUG // Skip this for debug since we might be running all by ourselves.
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
        else
            CCTRACEI ("CPlugin::Run() - Tray icon up");
    }
    else
        CCTRACEE ("CPlugin::Run() - failed opening tray event");
#endif

    ccLib::CCoInitialize COM (ccLib::CCoInitialize::eMTAModel);

    // Signal to the world that we are up and running
    //
    ccLib::CMutex mutexRunning;

    if ( !mutexRunning.Create ( NULL, TRUE, NAV_STATUS_HELPER_MUTEX_SESSIONWIDE_RUNNING, FALSE ))
    {
		CCTRACEE( _T( "CPlugin::Run() - Failed to create StatusHelper running mutex" ));
        assert(false);
        return; // fatal
    }

	dwRet = ::WaitForSingleObject(mutexRunning,0);
	if(dwRet != WAIT_OBJECT_0)
	{
		// someone else owns this mutex, so we close our handle to it only.
		// DO NOT RELEASE unless we own it.
        CCTRACEE( _T("CPlugin::Run() - StatusHelper already running" ));
        assert(false);
		return;
	}  

    startup();

	while (!m_shutdown)
	{
        HANDLE ahWaitObjects[] = {m_eventShutdown, m_eventNewWork, m_eventEMShutdown };
        ccLib::CMessageLock msglock (FALSE, FALSE);

		CCTRACEI("Run - listening...");

        for (;;)
        {
            DWORD dwWait = msglock.Lock ( 3, &ahWaitObjects[0], FALSE, INFINITE, FALSE ); // pump messages        
		    // Check for error.
		    if( dwWait == WAIT_FAILED )
		    {
                assert (false);
                CCTRACEE ("StatusManager - Wait failed");
                shutdown();
                return;
		    } 
		    else if( dwWait - WAIT_OBJECT_0 == 0 )
		    {
			    // exit signaled.
			    shutdown();
                CCTRACEI ("StatusManager - shut down");
                return;
		    }else if( dwWait - WAIT_OBJECT_0 == 1 )
		    {
		        // Process the queue.
		        processQueue();
                break;
		    }else if ( dwWait - WAIT_OBJECT_0 == 2)
            {
                waitForEM ();
            }
        }
    }

    CCTRACEI ("CPlugin::Run () - exiting");

    // Wait for exit
    return;
}

void CStatusManager::waitForEM()
{
    ccLib::CMessageLock msgLock ( TRUE, TRUE );

    // EM shut down, poll for when it restarts
    while (!m_shutdown)
	{
		if (m_pEMSubscriber->IsEMActive())
		{
			CCTRACEI("Run - EM restarted - updating status");
            m_eventEMShutdown.ResetEvent();
            startListening();
			break;
		}

		msgLock.Lock(m_eventShutdown, 5000); // 5 secs
	}
}

void CStatusManager::startListening (void)
{
    if (!m_pEMSubscriber.get())
    {
		CCTRCTXI0(_T("!m_pEMSubscriber.get(), need to construct the EMSubscriber"));

        // Construct the EMSubscriber and set up 
        // the alert events.
        //
        m_pEMSubscriber = std::auto_ptr<CEMSubscriber> (new CEMSubscriber (this));
        m_pEMSubscriber->AddRef();
    }
	else
	{
		CCTRCTXI0(_T("Succeeded m_pEMSubscriber.get()"));
	}

    // Build the resulting list of unique subscribed events
    //
    ccEvtMgr::CSubscriberHelper::SubscriberInfo subinfo;
    subinfo.m_nEventType = AV::Event_ID_StatusRequest;
    subinfo.m_nPriority = 0;
	subinfo.m_pEventStates = NULL;
	subinfo.m_nEventStatesCount = 0;
    subinfo.m_nProviderId = 0;
    subinfo.m_bMonitorOnly = false;

    m_pEMSubscriber->m_vecSubscriberInfo.push_back (subinfo);

    // Start listening for events
    if ( !m_pEMSubscriber->Connect())
    {
        CCTRCTXE0("Connect failed");
        
        // If we failed because EM is not running, wait for it.
        if ( !m_pEMSubscriber->IsEMActive())
		{
			CCTRCTXI0("!m_pEMSubscriber->IsEMActive()");

            BOOL bSetEvent = m_eventEMShutdown.SetEvent ();
			if(!bSetEvent)
				CCTRCTXE0("Failed m_eventEMShutdown.SetEvent()");
		}
    }
	else
	{
		CCTRCTXI0("Connect succeeded");
	}
}

void CStatusManager::EMSubscriberOnShutdown (void)
{
    // Start polling to see if EM comes back up, then re-subscribe.
    // IsEventManagerActive() checks a mutex. Should we turn this into
    // a waitable event??? &$

	CCTRACEI("CStatusManager::EMSubscriberOnShutdown - EM is shutting down");
    m_eventEMShutdown.SetEvent();
}


void CStatusManager::EMSubscriberOnEvent (const ccEvtMgr::CEventEx& Event,
                                          ccEvtMgr::CSubscriberEx::EventAction& eAction)
{

    // We want to alert on ALL NAV error events for this session, or
    // if no session data is available.
    //
    CNAVEventCommonQIPtr pNAVEvent (&Event);
    ASSERT (pNAVEvent != NULL);
	if(pNAVEvent == NULL)
	{
		CCTRCTXE0(_T("pNAVEvent == NULL"));
	}
	else
    {
        long lSessionID = 0;
		long lRequesterID = 0;
        pNAVEvent->props.GetData(AV::Event_Base_propSessionID, lSessionID );
		pNAVEvent->props.GetData(AVStatus::propRequesterID, lRequesterID );

		DWORD dwThisSessionID = COSInfo::GetSessionID();
        if ( dwThisSessionID == lSessionID )
        {
			CCTRCTXI3(_T("Event is for this session, handle it. CurrentSessionID=%d, AV::Event_Base_propSessionID=%d, AVStatus::propRequesterID=%d"), 
				dwThisSessionID, lSessionID, lRequesterID);

            ::ccEvtMgr::CEventEx* pEvent = NULL;

            if ( m_pEMSubscriber->CopyEvent (Event, pEvent))
            {
                AddItem ( pEvent );
                eAction = ccEvtMgr::CSubscriberEx::EventAction::eNoAction; // We will commit this manually
            }
			else
			{
				CCTRCTXE0(_T("m_pEMSubscriber->CopyEvent() returned false"));
			}
        }
		else
		{
			CCTRCTXI3(_T("Event is NOT for this session, skip it. CurrentSessionID=%d, AV::Event_Base_propSessionID=%d, AVStatus::propRequesterID=%d")
				, dwThisSessionID, lSessionID, lRequesterID);
		}
    }
}

void CStatusManager::processQueue ()
{
    ::ccEvtMgr::CEventEx* pEvent = NULL;
    while (!m_shutdown)
    {
        ccLib::CSingleLock lockWorking(&m_critWorking, INFINITE, FALSE);

        {// scope the lock
            // Get a new work item
            ccLib::CSingleLock lock (&m_critWorkQueue, INFINITE, FALSE);
            if ( !m_queueWork.empty())
            {
                pEvent = m_queueWork.front ();
                m_queueWork.pop();
            }
        }// end lock
        
        // Done
        if ( !pEvent )
            return;

        // *******  INIT REQUEST  *********
        //
        CNAVEventCommon* pNAVEvent = static_cast<CNAVEventCommon*>(pEvent);

        // Get the list of requested features
        //
        long lFeatureBytes = 0;

        // Get the size of the array. This will always return fail.
        //
        pNAVEvent->props.GetData(AVStatus::propRequestedFeatures,
                                    NULL,
                                    lFeatureBytes );

        if ( 0 == lFeatureBytes )
        {
            CCTRACEE ("No requested features");
            break;
        }
        else
            CCTRACEI ("# requested features: %d", lFeatureBytes/sizeof(long));

        std::vector<long> vecRequestedFeatures;
        vecRequestedFeatures.resize (lFeatureBytes/sizeof(long));
        if (!pNAVEvent->props.GetData( AVStatus::propRequestedFeatures,
                                       (BYTE*)&vecRequestedFeatures[0],
                                       lFeatureBytes ))
        {
            CCTRACEE ("No requested feature list!");
            break;
        }

        // Get status for each requested feature
        //
        for ( std::vector<long>::iterator iter = vecRequestedFeatures.begin();
            iter != vecRequestedFeatures.end();
            iter++)
        {
            long lFeatureID = *iter;
            iterStates State = m_mapFeatures.find (lFeatureID);
            if ( State == m_mapFeatures.end())
            {
                CCTRACEE ("Request for unsupported feature %d", lFeatureID );            
            }
            else
            {
                CState* pState = State->second;

                // Make sure it's running OK.
                if (pState->IsThread ())
                {
                    pState->HardRefresh();
                }
            }
        }

        // Query for the current state and save the result in the master list
        //
        CEventData edCurrentStatus;
        
        // If this state doesn't have status on this item it will gather it now.
        CState::GetCurrentStatus (edCurrentStatus);

        pNAVEvent->props.SetNode (AVStatus::propCurrentStatus, edCurrentStatus );

        // Send the event back to the client
        if (m_pEMSubscriber->CommitEvent(*(pEvent)) != ccEvtMgr::CError::eNoError)
        {
            CCTRACEE ("Failed to commit event");
        }
        else
        {
            long lRequesterID = 0;

            if ( pNAVEvent->props.GetData( AVStatus::propRequesterID, lRequesterID ) &&
                 AV_MODULE_ID_AVNSCPLG == lRequesterID )
            {
                // Set the Clean Bit(tm) mutex for the avNSCPlugin.
                ccLib::CEveryoneDACL daclEveryone;
                if ( daclEveryone.Create())
                {
                    char szTempSession [10] = {0};
                    ATL::CString strMutexName = NAV_STATUS_HELPER_MUTEX_MACHINEWIDE_NSC_CLEAN;
                    ultoa ( COSInfo::GetSessionID(), szTempSession, 10 );
                    strMutexName += szTempSession;

                    m_NSC.m_mutexNSCCleanBit.Create ( daclEveryone, TRUE, strMutexName, TRUE );
                }
            }
        }
        
        // Delete the event
        m_pEMSubscriber->DeleteEvent(pEvent);
        pEvent = NULL;
    }   // end while()
}

void CStatusManager::AddItem ( ::ccEvtMgr::CEventEx* pEvent )
{
    ccLib::CSingleLock lock ( &m_critWorkQueue, INFINITE, FALSE );
    m_queueWork.push (pEvent);
    m_eventNewWork.SetEvent ();
}
