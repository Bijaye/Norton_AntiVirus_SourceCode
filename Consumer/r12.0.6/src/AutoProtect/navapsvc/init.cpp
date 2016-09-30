/////////////////////////////////////////////////////////////////////////////
// init.cpp
//	  This module contains the startup and shutdown code for the service.
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "const.h"
#include "ServiceUtil.h"
#include "Navapsvc.h"
#include "notificationqueue.h"
#include "defsmanager.h"
#include "DefinitionsMonitor.h"
#include "navapnotify.h"
#include "optionsmonitor.h"
#include "globalevent.h"
#include "globalevents.h"
#include "scanqueue.h"
#include "cctrace.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::CServiceModule()

CServiceModule::CServiceModule() :
	m_pNotificationQueue( NULL ),
	m_pOptionsMonitorThread( NULL ),
	m_hStartup( NULL ),
    m_pDefMon( NULL ),
	m_bDebug( false ),
	m_pScanQueue( NULL ),
    m_bDefsBad( false ),
    m_bExclusionsDirty(false)
{
    m_LastEvent.dwVID = m_LastBootRepairFailure.dwVID = 0;
    m_LastEvent.wsFile.clear();
    m_LastBootRepairFailure.wsFile.clear();

    // Initialize the actions to -1 (0xFFFFFFFF)
    m_LastEvent.dwActions[0] = m_LastEvent.dwActions[1] = m_LastEvent.dwActions[2] = -1;
    m_LastBootRepairFailure.dwActions[0] = m_LastBootRepairFailure.dwActions[1] = m_LastBootRepairFailure.dwActions[2] = -1;

    // Create the startup event.
	m_hStartup = CreateEvent( NULL, TRUE, FALSE, NULL );
    m_hShutdown.Create ( NULL, TRUE, FALSE, NULL, FALSE );

    pAPWrapper = new CAutoProtectWrapper (true);

	// Set debug flag for debug builds.
#if	defined (_DEBUG )
	m_bDebug = true;
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::CServiceModule()

CServiceModule::~CServiceModule()
{
	if( m_hStartup )
		CloseHandle( m_hStartup );

	if ( pAPWrapper )
    {
        delete pAPWrapper;
        pAPWrapper = NULL;
    }
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::setupNAVService()

bool CServiceModule::setupNAVService()
{
	USES_CONVERSION;
/*
#ifdef _DEBUG   // break here if you want to debug the startup
    while (true)
    {
        Sleep (1000);
    }
#endif
assert(false);
*/   
assert(false);

	// Create notification queue.
	try
	{
        CCTRACEI(_T("CServiceModule::setupNAVService() - Begin"));

        // Load options.
		Config().Load();

        // Set up virus definitions management stuff.
		setupVirusDefs();

        CCTRACEI(_T("CServiceModule::setupNAVService() - Begin our threads"));

		// Create notification queue.
		m_pNotificationQueue = new CNotificationQueue( m_handlers );
    	if( false == m_pNotificationQueue->Create(NULL, 0, 0) )
        {
            delete m_pNotificationQueue;
            m_pNotificationQueue = NULL;
            CCTRACEE( "NAVAPSVC::Init - Could not create notification queue thread." );
			throw runtime_error( "Could not create notification queue thread." );
        }

		// Create options watch object.
		m_pOptionsMonitorThread = new COptionsMonitorThread();
		if( false == m_pOptionsMonitorThread->Create(NULL, 0, 0) )
        {
            delete m_pOptionsMonitorThread;
            m_pOptionsMonitorThread = NULL;
            CCTRACEE( "NAVAPSVC::Init - Could not create options notification thread." );
			throw runtime_error( "Could not create options notification thread." );
        }

		// Create definitions monitor object.
		m_pDefMon = new CDefinitionsMonitor();
		if( false == m_pDefMon->Create(NULL, 0, 0) )
        {
            delete m_pDefMon;
            m_pDefMon = NULL;
            CCTRACEE( "NAVAPSVC::Init - Could not create Definitions monitor thread." );
            throw runtime_error( "Could not create Definitions monitor thread." );
        }

        CCTRACEI(_T("CServiceModule::setupNAVService() - End our threads"));

		// Start all AP drivers
        CCTRACEI(_T("CServiceModule::setupNAVService() - Begin startAPDrivers sequence"));
		startAPDrivers();
        CCTRACEI(_T("CServiceModule::setupNAVService() - End startAPDrivers sequence"));

		// Tell the world that we are up and running.
		startupNotify();

		// Establish communications with NAVAP driver.
        CCTRACEI(_T("CServiceModule::setupNAVService() - Begin connectWithAPDriver sequence"));
		connectWithAPDriver();
        CCTRACEI(_T("CServiceModule::setupNAVService() - End connectWithAPDriver sequence"));

        // If the defs are bad make sure the driver is disabled
        if( m_bDefsBad )
        {
            CCTRACEE(_T("CServiceModule::setupNAVService() - Defs are bad disabling AP."));
            EnableAP(false);
        }

        // We are ready to go.
	    SetEvent( m_hStartup );

        // Tell the world that we are up and running.
	    CGlobalEvent event;
	    if( event.Create( SYM_REFRESH_AP_STATUS_EVENT ) )
	    {
		    ::PulseEvent ( event );
	    }

        CCTRACEI(_T("CServiceModule::setupNAVService() - Finished"));

        return true;
	}
	catch( exception &e )
	{
		LogEvent( e.what() );
        assert(false);
	}
    catch (...)
    {
        LogEvent( "Error occurred durring initialization" );
        assert(false);
    }

    return false;
}

// Creates our scan queue which is shared between the notifications and our
// startup scans.
//
bool CServiceModule::CreateScanQueue()
{
    if ( !m_pScanQueue )
    {
        // Create scan queue
        m_pScanQueue = new CScanQueue( *m_pNotificationQueue );
        if( false == m_pScanQueue->Create(NULL, 0, 0) )
        {
            delete m_pScanQueue;
            m_pScanQueue = NULL;
            CCTRACEE( "CServiceModule::createScanQueue - Could not create scan queue thread." );
            return false;
        }
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::shutdownNAVService()

void CServiceModule::shutdownNAVService()
{
	try
	{
        m_hShutdown.SetEvent();

		SetServiceStatus( SERVICE_STOP_PENDING, 2000 );
		
		// Tell all connected clients that we are shutting down.
		m_handlers.SendStopNotification();
		
		// Disconnect from AP driver.  We want no more notifications
		// at this time.
		disconnectWithAPDriver();
		
		SetServiceStatus( SERVICE_STOP_PENDING, 2000 );

		// Stop AP drivers.
		stopAPDrivers();
		
		SetServiceStatus( SERVICE_STOP_PENDING, 2000 );
		
		// Destroy Queues.
		if ( m_pNotificationQueue )
        {
            delete m_pNotificationQueue;
            m_pNotificationQueue = NULL;
        }

		SetServiceStatus( SERVICE_STOP_PENDING, 2000 );

		// Destroy other misc allocations.
		if (m_pOptionsMonitorThread)
        {
            delete m_pOptionsMonitorThread;
            m_pOptionsMonitorThread = NULL;
        }

		if ( m_pDefMon )
        {
            delete m_pDefMon;
            m_pDefMon = NULL;
        }

        if ( m_pScanQueue )
        {
            delete m_pScanQueue;
            m_pScanQueue = NULL;
        }
	}
	catch( exception &e )
	{
		LogEvent( e.what() );
	}
	catch(...)
	{
		LogEvent( "Error occurred during shutdown" );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CServiceModule::startAPDrivers()

void CServiceModule::startAPDrivers()
{
    // Savrt will do everything for us
    SAVRT_DRIVER_STATUS status = _Module.pAPWrapper->LoadDrivers();

    if( status != SAVRT_DRIVER_OK && status != SAVRT_DRIVER_ALREADY_LOADED )
    {
        // Error
        CCTRACEE (_T("NAVAPSVC : startAPDrivers failed - %d"), status );
        assert(false);
    }
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::stopAPDrivers()

void CServiceModule::stopAPDrivers()
{
    _Module.pAPWrapper->UnloadDrivers();
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::connectWithAPDriver()

void CServiceModule::connectWithAPDriver()
{
    // Open connection with driver
	SAVRT_DRIVER_STATUS savrtReturn;
    /*
	// Tell AP to ignore our thread.
    savrtReturn = _Module.APWrapper.DisableAPForThisThread();
    if ( savrtReturn != SAVRT_DRIVER_OK )
    {
		CCTRACEE (_T("NAVAPSVC : SAVRT_UnprotectProcess failed - %d"), savrtReturn);
        assert(false);
        throw runtime_error( "Unable to unprotect process." );
    }
    */

	// Register our callback routine.
    savrtReturn = _Module.pAPWrapper->RegisterServiceCallback(&CServiceModule::driverCallback );
    if ( savrtReturn != SAVRT_DRIVER_OK )
	{
		CCTRACEE (_T("NAVAPSVC : SAVRT_RegisterServiceCallback failed - %d"), savrtReturn);
        assert(false);
        throw runtime_error( "Could not register service callback." );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::disconnectWithAPDriver()

void CServiceModule::disconnectWithAPDriver()
{
	// Signal to re-protect us.
    _Module.pAPWrapper->EnableAPForThisThread();
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::setupVirusDefs()

void CServiceModule::setupVirusDefs()
{
    CCTRACEI(_T("CServiceModule::setupVirusDefs() - Begin"));
	// Initialize Defutils object.
	CDefsManager DefsMgr;
    DefsMgr.SwitchToLatestDefs ( DEFUTILS_APP_ID, true, _Module.Config().GetTestDefsPath(), false );
    DefsMgr.SwitchToLatestDefs ( DEFUTILS_APP_ID2, false, _Module.Config().GetTestDefsPath(), false );
    CCTRACEI(_T("CServiceModule::setupVirusDefs() - End"));
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::defAuthenticationFailure()
void CServiceModule::defAuthenticationFailure(DWORD dwFailure)
{
    m_handlers.SendDefAuthenticationFailure(dwFailure);
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::ReloadAP()

bool CServiceModule::ReloadAP(bool bForceReloadAVAPI)
{
    CCTRACEI ( _T("CServiceModule::ReloadAP() - CServiceModule::ReloadAP()") );

    ccLib::CSingleLock lock( &m_csReload, INFINITE, FALSE );

	try
	{
		// Open communications with AP driver
        SAVRT_DRIVER_STATUS savrtReturn = SAVRT_DRIVER_OK;

        // Cache of the current state of defs since if they become bad we will want to disable the driver
        bool bDefsAlreadyBad = m_bDefsBad;

        // Switch to the latest definitions. We use a second App ID to prevent AP's defs from
        // being deleted while it's using them. See defect # 419585.
        //
        // First get the old defs path to see if it changes
        basic_string<char> sOldDefsPath = Config().GetCurrentDefsPath();
        // Now switch
	    CDefsManager DefsMgr;
        DefsMgr.SwitchToLatestDefs ( DEFUTILS_APP_ID, true, _Module.Config().GetTestDefsPath());

        // Ensure the drivers are loaded
        startAPDrivers();

        if ( !_Module.pAPWrapper->GetAPRunning ())
        {
            CCTRACEE (_T("CServiceModule::ReloadAP() : Could not open AP communications. %d"), savrtReturn);

            // If the defs are bad let this fall through to the def handling, otherwise return false now
            if( !m_bDefsBad )
            {
                return false;
            }
        }

        // If the defs are bad return false now
        if( m_bDefsBad )
        {
            if( !bDefsAlreadyBad)
            {
                CCTRACEE(_T("CServiceModule::ReloadAP() : The defs have become bad and we have a driver communication handle. Disabling AP."));
                _Module.pAPWrapper->SetEnabledState (FALSE);

                // Tell all connected clients that AP is diabled.
		        m_handlers.SendStateChange( false );

                // Tell the world that we are disabled.
	            CGlobalEvent event;
	            if( event.Create( SYM_REFRESH_AP_STATUS_EVENT ) )
	            {
		            ::PulseEvent ( event );
	            }

                return false;
            }

            // The defs have been bad or we don't have any driver communication available
            CCTRACEE(_T("CServiceModule::ReloadAP() : The defs have already been bad or we can't communicate with the driver"));
            return false;
        }

		// Tell AP to reload its configuration.
        savrtReturn = _Module.pAPWrapper->ReloadConfig();
        if ( savrtReturn != SAVRT_DRIVER_OK && savrtReturn != SAVRT_DRIVER_ALREADY_LOADED)
		{
			LogEvent( _T("NAVAP_COMM_RELOADCONFIG: Error communicating with AP driver.") );
            CCTRACEE (_T("CServiceModule::ReloadAP() : NAVAP_COMM_RELOADCONFIG: Error communicating with AP driver. %d"), savrtReturn);            
			return false;
		}

        // Only reload AVAPI if the defs have been updated or we are told to force the
        // reload (i.e. Bloodhound level changed)
        if( 0 != sOldDefsPath.compare(Config().GetCurrentDefsPath()) || bForceReloadAVAPI )
        {
		    // OK - Everything is shut down now.  Time to reload things.
		    // Pause the scan queue.		
		    if ( m_pScanQueue )
                m_pScanQueue->Pause();

            // Tell AP to reload the definitions, this will restart naveng and navex
            // according to Mark S...
            CCTRACEI(_T("CServiceModule::ReloadAP() - Reloading the engines via SAVRT_ReloadAVAPI()"));
            savrtReturn = _Module.pAPWrapper->ReloadAVAPI();

            if ( savrtReturn != SAVRT_DRIVER_OK && savrtReturn != SAVRT_DRIVER_ALREADY_LOADED)
            {
                LogEvent( _T("SAVRT_COMM_RELOADAVAPI: Error communicating with AP driver.") );
                CCTRACEE (_T("CServiceModule::ReloadAP() : SAVRT_COMM_RELOADAVAPI: Error communicating with AP driver. %d"), (int) savrtReturn);

                // Resume the scan queue.
		        if ( m_pScanQueue )
                    m_pScanQueue->Resume();

			    return false;
            }

            // Resume the scan queue.
		    if ( m_pScanQueue )
                m_pScanQueue->Resume();
        }

        // After AP is done reloading we can switch the fake ID to the new defs.
        //
        DefsMgr.SwitchToLatestDefs ( DEFUTILS_APP_ID2, false, _Module.Config().GetTestDefsPath());

		// All is well.
		return true;
	}
	catch( exception &e )
	{
		LogEvent( e.what() );
	}

	// An error has occurred.
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::ReloadOptions()

bool CServiceModule::ReloadOptions()
{
	// Load the options.
	try
	{
		// TODO: need to call SAVRT_ReloadConfig() on the driver here. 
		Config().Load();
		return true;
	}
	catch(...)
	{
		LogEvent( _T("Error loading configuration options.") );
	}

	return false;	
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::WaitForStartup()

HRESULT CServiceModule::WaitForStartup()
{
	// Wait here for starup signal.
    DWORD dwWait = 60000;
#ifdef _DEBUG
    dwWait = INFINITE;
#endif 
	return WaitForSingleObject( m_hStartup, dwWait ) == WAIT_OBJECT_0 ? S_OK : E_FAIL;
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::startupNotify()

void CServiceModule::startupNotify()
{
	// Create options event
	CGlobalEvent event;
	if( FALSE == event.Create( NAVAP_STARTED_EVENT ) )
	{
		_ASSERT( FALSE );
	}

	// Tell all waiting threads that we are starting.
	::PulseEvent( event );
}

