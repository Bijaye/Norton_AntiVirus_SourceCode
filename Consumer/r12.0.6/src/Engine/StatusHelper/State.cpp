#include "StdAfx.h"
#include "state.h"
#include "WMIIntegration.h"

#include "AvEvents.h"
#include "NAVEventFactoryLoader.h"
#include "SMHelper.h"   // Settings for persistance

const TCHAR SYMSETUP_MUTEX_NAME[] = _T("SYMSETUP_MUTEX");

// I only want to see one of these!!!!
CNAVInfo g_NAVInfo;

// Statics
CEventData CState::m_edStatus;  
ccLib::CCriticalSection CState::m_critStatus;
ATL::CDacl CState::m_nulldacl;
ATL::CSecurityDesc CState::m_secdesc;
ATL::CSecurityAttributes CState::m_secatt;
bool CState::m_bStatusLoaded = false;
bool CState::m_bLoadedOld = false;
static const WCHAR Settings_STATUS_KEY[] = L"Norton AntiVirus\\NAVOPTS.DAT\\STATUS";
static const WCHAR Settings_Persisted_Value[] = L"PersistedStatus";

CState::CState(CWMIIntegration* pWMI, CNSCIntegration* pNSC) :
    m_pWMI(pWMI),
    m_pNSC(pNSC),
    m_bInit(false),
    m_dwTimeout(INFINITE)
{
    // We need COM to send out the update events
    m_Options.m_eCOMModel = ccLib::CCoInitialize::eSTAModel;
    m_Options.m_bPumpMessages = TRUE;
    m_Options.m_bWaitForInitInstance = TRUE;

    m_lEventID = 0;

    // Set our null dacl
    //
    if ( m_nulldacl.IsEmpty())
    {
        m_nulldacl.SetNull();
        m_secdesc.SetDacl (m_nulldacl);
        m_secatt.Set (m_secdesc); // This throws assertions in debug mode, I know.
    }

    // Only load once at startup
    if ( !m_bStatusLoaded )
    {
        loadPersistedStatus ();
        m_bStatusLoaded = true;
    }
}

CState::~CState(void)
{
}

BOOL CState::InitInstance()
{
    // These CState object don't get created until we actually need them.
    Save();

    // Send the "new" state in case a client started before we did. If no one is subscribing this will return quickly.
    sendData ();

    // Done. We are initialized.
    m_bInit = true;

    return TRUE;
}

int CState::Run()
{
    // Set up waiting objects.
    std::vector <HANDLE> vecHandles;
	vecHandles.push_back (m_Terminating.GetHandle());

    for ( DWORD dwIndex = 0;
          dwIndex < m_vecWaitEvents.size ();
          dwIndex ++ )
    {
        vecHandles.push_back (m_vecWaitEvents.at(dwIndex));
    }

    ccLib::CMessageLock msgLock ( TRUE, FALSE );

	// Main thread loop.
	for(;;)
	{
        // Wait here for something to happen
        //
		DWORD dwWait = msgLock.Lock ( (DWORD) vecHandles.size(), &vecHandles[0], FALSE, m_dwTimeout, FALSE );

		// Check for exit signal
		if( dwWait == WAIT_OBJECT_0 )
		{
			CCTRACEI("Stop listening");
            if ( m_bStatusLoaded )
            {
                m_bStatusLoaded = false; // unloaded
                savePersistedStatus();
            }
			break;
		}

        if ( dwWait == WAIT_FAILED )
        {
            CCTRACEE ("CState::Run() - msgLock failed %d", ::GetLastError());
            assert(false);
            return -1;
        }
        else
        {
            if( dwWait == WAIT_TIMEOUT )
                CCTRACEI("Timeout reached");
		
            // Don't refresh if symsetup is running since it could've been our uninstall 
            // that caused a change.
            //
            ccLib::CMutex mutexSymSetup;
            if ( mutexSymSetup.Open(SYNCHRONIZE, FALSE, SYMSETUP_MUTEX_NAME, TRUE ) )
            {
                CCTRACEE( _T( "CState::Run - SymSetup mutex exists. An install is running so aborting refresh." ));
            }
            else
            {
                // *******  UPDATES  *********
                //
                if ( Save())
                    sendData (); // Only send updates if there was a change
            }
        }
    }

    CCTRACEI("Thread exiting");
    return 1;
}

bool CState::makeEvent ( LPCSTR szEventName, BOOL bGlobal, ccLib::CEvent& event )
{
    // Create a notification handle for the refresh event.
    //

    // If I use a EveryoneDACL, or NullDacl, the clients (IWP) get access denied
    // when they try to create the event on their end.
    //
    // Reset the owner
    if ( !event.Create ( &m_secatt,
                            TRUE,
                            FALSE,
                            szEventName,
                            bGlobal))
    {
        CCTRACEE ("CState::makeEvent - unable to create event %s", szEventName);
        return false;
    }

    return true;
}

void CState::GetCurrentStatus ( CEventData& edCurrentStatus )
{
    ccLib::CSingleLock lock ( &m_critStatus, INFINITE, FALSE );

    // Give the caller our current copy
    edCurrentStatus = m_edStatus;
}

void CState::sendData ()
{
    // For sending data
    AV::IAvEventFactoryPtr pProvider;

    if (SYM_FAILED (AV::NAVEventFactory_IAVEventFactory::CreateObject (GETMODULEMGR(), &pProvider)) ||
        pProvider.m_p == NULL )
    {
        CCTRACEE ( "Could not create IAvFactory object. - %d", ::GetLastError() );
        pProvider = NULL;
    }
    else
    {
        pProvider->AddRef();
        ccLib::CSingleLock lock ( &m_critStatus, INFINITE, FALSE );

        // We have to set the event ID before sending
        //
        CEventData edTemp;
        edTemp.SetData ( AV::Event_Base_propType, m_lEventID);
        edTemp.SetData ( AV::Event_Base_propSessionID, COSInfo::GetSessionID());
        edTemp.SetNode ( AVStatus::propNewStatus, m_edStatus );
        if (!pProvider->BroadcastAvEvent (edTemp))
        {
            CCTRACEE ("CState::Run() - Failed to BroadcastAvEvent");
            assert(false);
        }
		pProvider->Release();	//ACR##
    }

    // Send data to WMI and NSC, but only for updates. We handle the init inside the 
    // startup sequence.
    //
    if ( m_bInit )
    {
        ccLib::CSingleLock lock ( &m_critStatus, INFINITE, FALSE );
        
        if ( m_pWMI )
            m_pWMI->OnStatusChange ( m_lEventID, m_edStatus);

        if ( m_pNSC)
            m_pNSC->OnStatusChange ( m_lEventID, m_edStatus);
    }
}


// For a status item to qualify to use persisted status data it must:
// * Not be different for different users
// * Not change while users are logged out (within reason)
// * Not change due to conditions of other apps (AP service not starting)
//
void CState::loadPersistedStatus ()
{
    // Read in the persisted state
    //
    CSMHelper status;
    if(!status.Load ( Settings_STATUS_KEY ))
    {
        CCTRACEE("CState::loadPersistedStatus: status.Load != true");
        return;
    }

    std::vector <BYTE> vecBuffer;
    DWORD dwSize = 2048;
    vecBuffer.resize (dwSize); // pick a number, this should be big enough to get it on the first try

	SYMRESULT sr = status.GetSettings()->GetBinary (Settings_Persisted_Value, &vecBuffer[0], dwSize);

    if ( SYM_FAILED (sr) && SYM_SETTINGS_ERROR_BUFFER_TOO_SMALL == sr )
    {
        vecBuffer.resize (dwSize);

	    sr = status.GetSettings()->GetBinary (Settings_Persisted_Value, &vecBuffer[0], dwSize);
    }

    if ( SYM_SUCCEEDED (sr))
    {
        ccLib::CSingleLock lock ( &m_critStatus, INFINITE, FALSE );
        m_edStatus.LoadDataFromStream (&vecBuffer[0]);
        m_bLoadedOld = true;

        // Now remove the cached data. Why? After this session loads and the data changes then another
        // copy of StatusHP running in another session starts it will load the old data. This prevents
        // us getting out of sync, although we do hurt startup performance for the other sessions. This
        // design issue needs to be resolved when StatusHP moves to a service.
        status.GetSettings()->DeleteValue (Settings_Persisted_Value);
        if ( !status.Save ( Settings_STATUS_KEY ))
            CCTRACEE ("CState::loadPersistedStatus - error saving state");
    }
}

void CState::savePersistedStatus ()
{
    ccLib::CSingleLock lock ( &m_critStatus, INFINITE, FALSE );
    CCTRACEI("savePersistedStatus - start");

    // Save the persisted state
    //
    CSMHelper status;

    CCTRACEI("savePersistedStatus - loading");
    if(!status.Load ( Settings_STATUS_KEY ))
    {
        CCTRACEE("CState::loadPersistedStatus: status.Load != true");
        return;
    }
    CCTRACEI("savePersistedStatus - loaded");
 
    std::vector <BYTE> vecBuffer;
    ULONG ulSize = m_edStatus.GetSize ();
    ULONG ulSizeWritten = 0;

    vecBuffer.resize ( ulSize);

    m_edStatus.SaveDataToStream ( &vecBuffer[0], ulSize, ulSizeWritten );

    CCTRACEI("savePersistedStatus - saving");
	SYMRESULT sr = status.GetSettings()->PutBinary (Settings_Persisted_Value, &vecBuffer[0], ulSize);
    if ( SYM_SUCCEEDED(sr))
    {
        if ( !status.Save ( Settings_STATUS_KEY ))
            CCTRACEE ("CState::savePersistedStatus - error saving state");
    }
    else
        CCTRACEE("CState::savePersistedStatus: SetBinary 0x%x", sr);

    CCTRACEI("savePersistedStatus - end success");
}

// Re-compute the date string in case the format has changed.
// Defect 1-4CYCIV
//
void CState::formatDate ( long lProperty, ATL::CTime time )
{
    SYSTEMTIME stimeDate;

    time.GetAsSystemTime (stimeDate);
    
    WCHAR szDefsDate [64] = {0};

    // Don't format if it's 0
    //
    if ( stimeDate.wYear > 0 )
    {
         GetDateFormatW ( LOCALE_USER_DEFAULT,
                         DATE_SHORTDATE, 
                         &stimeDate,
                         NULL,
                         szDefsDate,
                         64 );
    }

    m_edStatus.SetData ( lProperty, szDefsDate );
}
