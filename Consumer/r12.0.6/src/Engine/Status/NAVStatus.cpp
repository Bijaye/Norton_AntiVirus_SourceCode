// NAVStatus.cpp : Implementation of CNAVStatus
#include "stdafx.h"
#import "Status.tlb"
#include "Status.h"
#include "NAVStatus.h"
#include "navtrust.h"
#include "AVccModuleId.h"
#include "NAVErrorResource.h"

#include "StatusEventBridge.h"

#include "TraceHR.h"
#include "NAVEventCommon.h"

#include "GlobalMutex.h"    // For running mutex

/////////////////////////////////////////////////////////////////////////////
// CNAVStatus

CNAVStatus::CNAVStatus()
{
	m_bInit= false;
	m_bUseVirusDef = 0;
	m_bUseALU = 0;
	m_bUseAP = 0;
	m_bUseVirusDefSub = 0;
	m_bUseEmail = 0;
	m_bUseFullSystemScan = 0;
	m_bUseLicensing = 0;
    m_bUseIWP = 0;
    m_bUseSpyware = 0;

	// Check NAVError module for Symantec Signature...
	if( NAVToolbox::NAVTRUST_OK == NAVToolbox::IsSymantecComServer(_T("NAVError.NAVCOMError")) )
	{
		if ( FAILED ( m_spError.CoCreateInstance( L"NAVError.NAVCOMError",
			NULL,
			CLSCTX_INPROC_SERVER)))
		{
		}
	}

	m_bWaitBeforeSignaling = false;
	m_bIsFirstRefresh = true;   
    g_bIsAdminProcess = g_OSInfo.IsAdminProcess();

    // For SyncQueue
    m_Options.m_eCOMModel = ccLib::CCoInitialize::eSTAModel;
    m_Options.m_bPumpMessages = TRUE;
}


CNAVStatus::~CNAVStatus()
{
    // Kill our CSyncQueue
    Terminate(INFINITE);
}

HRESULT CNAVStatus::FinalConstruct()
{
	// COM Bridge object
	//
	CCTRACEI( "CNAVStatus::FinalConstruct");

	// Check the status dll for a valid symantec signature
	if( forceError(ERR_SECURITY_FAILURE) || NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(_T("Status.NAVStatus")) )
	{
		CCTRACEE ( "Invalid signature" );
		makeError ( ERR_SECURITY_FAILURE, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
		return E_ACCESSDENIED;
	}

    return S_OK;
}

void CNAVStatus::FinalRelease()
{
	CCTRACEI( "Deconstructing objects" );

    // Shut down the subscriber first since it talks to 
    // CStatusItem objects.
    //
    if ( m_pEMSubscriber.get() )
        m_pEMSubscriber->Disconnect();

	// COM Bridge object
	//    
	// Take our interface our of the GIT
	//
    if ( m_bNotification && m_bInit)
    {
        m_GIT.Revoke ( m_dwCookie );
        m_GIT.Destroy();
    }

	// Deconstruct the items in the list
	//
	for ( iterStatusItems iter = m_mapStatusItems.begin ();
		  iter != m_mapStatusItems.end ();
		  iter++ )
	{
		CStatusItem* pStatusItem = (*iter).second;
		if ( pStatusItem )
		{
			delete ( pStatusItem );
			pStatusItem = 0;
		}
    }
}

STDMETHODIMP CNAVStatus::GetStatus(BOOL bNotifyChanges)
{
	// Make sure that it is used by an authorized Symantec application

	if (forceError(ERR_SECURITY_FAILURE) || !IsItSafe())
	{
		makeError ( ERR_SECURITY_FAILURE, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
		return E_ACCESSDENIED;
	}

	if ( m_bInit )
	{
		CCTRACEW ( "GetStatus() called twice" );
		return S_FALSE;
	}

    // Check to see if we have a connection point, regardless of what was
    // passed in.
    //
    m_bNotification = (m_vec.GetSize () > 0);

    if ( bNotifyChanges != m_bNotification)
    {
        CCTRACEE ("CNAVStatus::GetStatus - mismatch in notification type expected!");
        assert(false);
    }

    // Start the monitor thread regardless if the server is up
	//
	if (m_bNotification)
	{
        // If the client wants updates we need a GIT.
        //
	    CComObject< CStatusEventBridge >* pBridge= NULL; // pointer 

	    CComObject< CStatusEventBridge >::CreateInstance(&pBridge);
	    pBridge->SetCallback(this);

	    // Put our helper IUnknown into the GIT
	    //
        HRESULT hr = m_GIT.Create();
        if ( FAILED (hr))
            CCTRACEE ( "Failed to globalize 0x%x", hr );
        else
            m_GIT.Register ( pBridge->GetUnknown(), m_dwCookie);

	    NAVToolbox::CIEUtils IEUtils;
	    if ( IEUtils.m_iMajorVersion <= 5 && IEUtils.m_iMinorVersion == 0 )
		    m_bWaitBeforeSignaling = true;

        // Start up our SyncQueue. You're welcome.
        //
        Create(NULL,0,0); 

        // Construct the EMSubscriber and set up 
        // the alert events.
        //
        m_pEMSubscriber = std::auto_ptr<CEMSubscriber> (new CEMSubscriber (this));
        m_pEMSubscriber->AddRef();	
    }
    else
    {
        // If we don't want updates and the server isn't up then there's no point
        // in sending the request event.
        //
        ccLib::CMutex mutexRunning;
        if ( !mutexRunning.Open ( SYNCHRONIZE, FALSE, NAV_STATUS_HELPER_MUTEX_SESSIONWIDE_RUNNING, FALSE ))
        {
		    CCTRACEE( _T( "CNAVStatus::GetStatus - Failed to open StatusHelper running mutex" ));
            assert(false);
            return S_FALSE;
        }

	    DWORD dwRet = ::WaitForSingleObject(mutexRunning,0);
	    if(dwRet == WAIT_OBJECT_0)
	    {
            CCTRACEE( _T("CPlugin::Run() - StatusHelper not running" ));
		    return S_FALSE;
	    }  
    }

    // Get the initial value in this thread
	//
	if (!init ())
        return S_FALSE; // Server isn't running, but we subscribed anyway

	return S_OK;
}

bool CNAVStatus::init()
{
    bool bReturn = true;

    if ( !m_bInit )
	{

		// Make an AP status item
		//
		if ( m_bUseAP )
		{
			CStatusAutoProtect* pstatusAP = new CStatusAutoProtect;

			if ( pstatusAP )
			{
				// Add it to the list of status items
				//
				m_mapStatusItems[pstatusAP->m_lEventID] = pstatusAP;
			}
			else
			{
				CCTRACEW ( "Couldn't create CStatusAutoProtect object" );
				m_bUseAP = false;
			}
		}

		// Virus def status item
		//
		if ( m_bUseVirusDef )
		{
			CStatusVirusDefs* pstatusVirusDefs = new CStatusVirusDefs;

			if ( pstatusVirusDefs )
			{
				// Add it to the list of status items
				//
				m_mapStatusItems[pstatusVirusDefs->m_lEventID] = pstatusVirusDefs;
			}
			else
			{
				CCTRACEW ( "Couldn't create CStatusVirusDefs object" );
				m_bUseVirusDef = false;
			}
		}

		// Make an ALU status item
		//
		if ( m_bUseALU )
		{
			CStatusAutoLiveUpdate* pstatusALU = new CStatusAutoLiveUpdate;

			if ( pstatusALU )
			{
				// Add it to the list of status items
				//
				m_mapStatusItems[pstatusALU->m_lEventID] = pstatusALU;
			}
			else
			{
				CCTRACEW ( "Couldn't create CStatusAutoLiveUpdate object" );
				m_bUseALU = false;
			}
		}

		// Make a Quarantine status item - not supported
		//

        // Make a Rescue Disk status item - not supported
		//

		// Make a Email status item
		//
		if ( m_bUseEmail )
		{
			CStatusEmail* pstatusEmail = new CStatusEmail;

			if ( pstatusEmail )
			{
				// Add it to the list of status items
				//
				m_mapStatusItems[pstatusEmail->m_lEventID] = pstatusEmail;
			}
			else
			{
				CCTRACEW ( "Couldn't create CStatusEmail object" );
				m_bUseEmail = false;
			}
		}

		// Make a Full System Scan status item
		//
		if ( m_bUseFullSystemScan )
		{
			CStatusFullSystemScan* pstatusFullSystemScan = new CStatusFullSystemScan;

			if ( pstatusFullSystemScan )
			{
				// Add it to the list of status items
				//
				m_mapStatusItems[pstatusFullSystemScan->m_lEventID] = pstatusFullSystemScan;
			}
			else
			{
				CCTRACEW ( "Couldn't create CStatusFullSystemScan object" );
				m_bUseFullSystemScan = false;
			}
		}

		// Make a Licensing status item
		//
		if ( m_bUseLicensing || m_bUseVirusDefSub /*subscriptions, not defs*/)
		{
			CStatusLicensing* pstatusLicensing = new CStatusLicensing;

			if ( pstatusLicensing )
			{
				// Add it to the list of status items
				//
				m_mapStatusItems[pstatusLicensing->m_lEventID] = pstatusLicensing;
			}
			else
			{
				CCTRACEW ( "Couldn't create CStatusLicensing object" );
				m_bUseLicensing = false;
                m_bUseVirusDefSub = false;
			}
		}

        if ( m_bUseIWP )
        {
            CStatusIWP* pstatusIWP = new CStatusIWP;
            m_mapStatusItems[pstatusIWP->m_lEventID] = pstatusIWP;
        }

        if ( m_bUseSpyware )
        {
            CStatusSpyware* pstatusSpyware = new CStatusSpyware;
            m_mapStatusItems[pstatusSpyware->m_lEventID] = pstatusSpyware;
        }

        // We have to set the event ID before sending
        //
        CEventData* pedStatus = new CEventData;
        pedStatus->SetData ( AV::Event_Base_propType, AV::Event_ID_StatusRequest );
        pedStatus->SetData ( AV::Event_Base_propSessionID, COSInfo::GetSessionID());

        // Build the list of requested features
        //
        std::vector <long> vecFeatures;

		for ( iterStatusItems iter = m_mapStatusItems.begin();
				iter != m_mapStatusItems.end ();
				iter++)
		{
			vecFeatures.push_back ((*iter).first);
		}
        pedStatus->SetData ( AVStatus::propRequestedFeatures, (BYTE*)&vecFeatures[0], (long)vecFeatures.size ()*sizeof(long));

        // Start the waiting thread
        if ( !m_pRequestStatusThread.get() )
        {
            m_pRequestStatusThread = std::auto_ptr<CRequestStatusThread> (new CRequestStatusThread(this));
            m_pRequestStatusThread->SetData (pedStatus); // thread will delete this pointer
            m_pRequestStatusThread->Create(NULL,0,0);

            // If the client doesn't want real-time notification just wait for the request to come back
            if ( !m_bNotification )
            {
                m_pRequestStatusThread->WaitForExit(INFINITE);
            }
        }

        // Start the subscriber, even if the server isn't up yet.
        //
        if( m_bNotification && m_pEMSubscriber.get() )
        {
            // Build the resulting list of unique subscribed events
            //
            for ( iterStatusItems iter = m_mapStatusItems.begin(); iter != m_mapStatusItems.end(); iter++)
            {
                ccEvtMgr::CSubscriberHelper::SubscriberInfo subinfo;
                subinfo.m_nEventType = iter->first;
                subinfo.m_nPriority = 0;
	            subinfo.m_pEventStates = NULL;
	            subinfo.m_nEventStatesCount = 0;
                subinfo.m_nProviderId = 0;
                subinfo.m_bMonitorOnly = false;

                m_pEMSubscriber->m_vecSubscriberInfo.push_back (subinfo);
            }

            // Start listening for events
            if ( !m_pEMSubscriber->Connect())
            {
                CCTRACEE ("EMSubscriber - connect failed");
            }
        }

		m_bInit = true;
	}

    return bReturn;
}

// CRequestStatusThreadSink
void CNAVStatus::OnRequestStatusReceived (CEventData& edNewData)
{
    CStatusItem::Initialize ( edNewData );

    // Loop through status item and fill our internal properties
    //
    for ( iterStatusItems iter = m_mapStatusItems.begin();
		    iter != m_mapStatusItems.end ();
		    iter++)
    {
        (*iter).second->UpdateStatus ();
    }

    OnNotifyClient();
}

STDMETHODIMP CNAVStatus::get_NAVError(INAVCOMError **pVal)
{
	if ( !m_spError )
		return E_FAIL;

	*pVal = m_spError;
	(*pVal)->AddRef (); // We get a ref and the caller gets one

	return S_OK;
}

HRESULT CNAVStatus::makeError(long lMessageID, long lHResult, long lErrorResID)
{
	if ( !m_spError )
		return lHResult;

	m_spError->put_ModuleID ( AV_MODULE_ID_STATUS );
	m_spError->put_ErrorID ( lMessageID );
	m_spError->put_HResult ( lHResult );
	m_spError->put_ErrorResourceID( lErrorResID );
    return lHResult;
}

bool CNAVStatus::forceError(long lErrorID)
{
	if ( !m_spError )
		return false;

	long lTempErrorID = 0;
	long lTempModuleID = 0;

	m_spError->get_ForcedModuleID ( &lTempModuleID );
	m_spError->get_ForcedErrorID ( &lTempErrorID );

	if ( lTempModuleID == AV_MODULE_ID_STATUS &&
		lTempErrorID == lErrorID )
		return true;
	else
		return false;
}

void CNAVStatus::OnNotifyClient ()
{
	
    ccLib::CSingleLock lock (&m_critSignalScript, INFINITE, FALSE);

	try
	{
		// COM Bridge object
		//
		IUnknownPtr pUnk = 0;

		// Unmarshal the unknown from the GIT into this thread
		//
		STATUSLib::IStatusEventBridgePtr spBridge = 0;
        if ( SUCCEEDED ( m_GIT.Get ( m_dwCookie, &pUnk )))
        {
		    spBridge = pUnk;

		    if(spBridge != NULL)
            {
		        // Update the script. Use the event from our marshalled
		        // COM Bridge Object.
		        //

		        // Cheesey fix for Microsoft bug this occurs with IE 5.00 before SP2
		        // or WSH version 5.1.0.5907 . Defect # 396183.
		        //
		        if ( m_bWaitBeforeSignaling && !m_bIsFirstRefresh )
		        {
                    ccLib::CEvent eventNothing;
                    ccLib::CMessageLock msgLock (TRUE, TRUE);

                    // wait and pump messages
                    eventNothing.Create ( NULL, FALSE, FALSE, NULL, FALSE);
                    msgLock.Lock ( eventNothing, 1250 );
		        }

		        m_bIsFirstRefresh = false;

		        CCTRACEI ( "CNAVStatus::OnNotifyClient () - Got event, telling script." );
		        spBridge -> StatusChangedEvent();
                CCTRACEI ( "CNAVStatus::OnNotifyClient () - Told script successfully" );
            }
        }
        else
        {
            CCTRACEE ("CNAVStatus::OnNotifyClient - GIT.get failed");
            assert (false);
        }
	}
	catch(...)
	{
		CCTRACEE ( "CNAVStatus::OnNotifyClient failed!" );
	}
}

// Called by NAVStatus1Impl.cpp directly
//
HRESULT CNAVStatus::getLong(long lName, long* lReturn)
{
	TRACEHR (h);

	// Make sure that it is used by an authorized Symantec application

	if (forceError(ERR_SECURITY_FAILURE) || !IsItSafe())
	{
		makeError ( ERR_SECURITY_FAILURE, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
		return h = E_ACCESSDENIED;
	}

    if ( forceError (ERR_GET_ON_INVALID_ITEM))
        return h = makeError ( ERR_GET_ON_INVALID_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL );

    try
    {
        h.Verify(!forceError(ERR_UNKNOWN_GET_STATUS), E_ABORT);

        ccLib::CSingleLock lock ( &CStatusItem::m_critMasterData, INFINITE, FALSE );
        if ( !CStatusItem::m_edMasterData.GetData ( lName, *lReturn ))
		{
            assert(false);
			CCTRACEE("Could not find %d", lName);
            h = E_FAIL;
		}
    }
	catch (_com_error& e)
	{
		h = e;
	}
    catch(...)
    {
        h = makeError ( ERR_UNKNOWN_GET_STATUS, E_UNEXPECTED, IDS_NAVERROR_INTERNAL );
    }

	return h;
}

// Called by NAVStatus1Impl.cpp directly
//
HRESULT CNAVStatus::getString(long lName, BSTR* bstrReturn)
{
	// Make sure that it is used by an authorized Symantec application

	if (forceError(ERR_SECURITY_FAILURE) || !IsItSafe())
	{
		makeError ( ERR_SECURITY_FAILURE, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
		return E_ACCESSDENIED;
	}

    if ( forceError (ERR_GET_ON_INVALID_ITEM) || !bstrReturn )
        return makeError ( ERR_GET_ON_INVALID_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL );

    try
    {
        if (forceError(ERR_UNKNOWN_GET_STATUS))
			throw FALSE;        

        std::wstring wstrReturn;

        ccLib::CSingleLock lock ( &CStatusItem::m_critMasterData, INFINITE, FALSE );
        if ( CStatusItem::m_edMasterData.GetData ( lName, wstrReturn ))
        {
    		CComBSTR tempstring ( wstrReturn.c_str ());
		    *bstrReturn = tempstring.Detach();
            return S_OK;
        }
        else
        {
            assert(false);
            CCTRACEE ("Could not find %d", lName);
        }
    }
    catch(...)
    {
        // You asked for a string when it was a DWORD, or something
        return makeError ( ERR_UNKNOWN_GET_STATUS, E_FAIL, IDS_NAVERROR_INTERNAL );
    }

    return makeError ( ERR_UNKNOWN_GET_STATUS, E_FAIL, IDS_NAVERROR_INTERNAL );
}


// SyncQueue - Updates!!
//
void CNAVStatus::processQueue ()
{
    ::ccEvtMgr::CEventEx* pEvent = NULL;
    bool bUpdated = false;

    while (getQueueFront(pEvent))
    {
        CNAVEventCommon* pNAVEvent = static_cast<CNAVEventCommon*>(pEvent);

        if ( pNAVEvent )
        {
            CEventData edStatus;
            if (pNAVEvent->props.GetNode ( AVStatus::propNewStatus, edStatus ))
            {
                // Just stop if we have been told to terminate.
                //
                if ( !IsTerminating() )
                {
                    CStatusItem::Initialize ( edStatus );

                    // Loop through status item and fill our internal properties
                    //
		            for ( iterStatusItems iter = m_mapStatusItems.begin();
				            iter != m_mapStatusItems.end ();
				            iter++)
		            {
                        CStatusItem* pItem = (*iter).second;
                        if ( pItem )
                            pItem->UpdateStatus ();
                    }

                    bUpdated = true;
                }
            }

            if ( m_pEMSubscriber.get() )
                m_pEMSubscriber->DeleteEvent (pEvent);
        }
    }

    if ( bUpdated )
    {
        // Don't tell the client until they have the initial update with all the data.
        // Otherwise they don't know what to expect. Defect 1-4E8I4O
        //
        if ( m_pRequestStatusThread.get() &&
             m_pRequestStatusThread->IsThread())
        {
            return;
        }

        // Send an event to the main thread!
        OnNotifyClient ();
    }
}

void CNAVStatus::EMSubscriberOnShutdown (void)
{
    // Start polling to see if EM comes back up, then re-subscribe.
    // IsEventManagerActive() checks a mutex. Should we turn this into
    // a waitable event??? &$
    // We need a thread to do this in.
}


void CNAVStatus::EMSubscriberOnEvent (const ccEvtMgr::CEventEx& Event,
                                            ccEvtMgr::CSubscriberEx::EventAction& eAction)
{
    // We want to alert on ALL NAV error events for this session, or
    // if no session data is available.
    //
    CNAVEventCommonQIPtr pNAVEvent (&Event);
    ASSERT (pNAVEvent != NULL);
    if ( pNAVEvent )
    {
        long lSessionID = 0;
        pNAVEvent->props.GetData(AV::Event_Base_propSessionID, lSessionID );
        
        if ( COSInfo::GetSessionID() == lSessionID )
        {
            ::ccEvtMgr::CEventEx* pEvent = NULL;

            if ( m_pEMSubscriber->CopyEvent (Event, pEvent))
            {
                AddItem ( pEvent );
            }
        }
    }
}
