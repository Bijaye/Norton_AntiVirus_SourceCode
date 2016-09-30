// Copyright 1996-1997 Symantec Corporation
//************************************************************************
//
// $Header:   S:/APWCMD/VCS/ApwNtCmd.cpv   1.7   29 Jun 1998 22:21:04   RStanev  $
//
// Description:
//    This is where the functions go that communicate through the pipe
//    to the NAVAP Service.
//
// Contains:
//    CmdInit()
//    CmdDeInit()
//    CmdOptions()
//    CmdWaitForStartupPending()
//    CmdDisableDriver()
//    CmdEnableDriver()
//    CmdGetStatusOfDriver()
//    CmdStartDriver()
//    CmdStopDriver()
//    CmdShutdownScan()
//    ConfirmApwComTasksReceived()
//
//************************************************************************
// $Log:   S:/APWCMD/VCS/ApwNtCmd.cpv  $
// 
//    Rev 1.7   29 Jun 1998 22:21:04   RStanev
// Added new functionality to support asynchronous processing of tasks
// triggered by driver notifications. This is a workaround for a problem
// with RPC in Win9x.
//
//    Rev 1.6   01 Jun 1997 12:38:18   DALLEE
// Changed naming scheme to be more readable - "APW" in classes and
// functions is now "Apw".
//
//    Rev 1.5   31 May 1997 19:53:00   JBRENNA
// 1. Use the CApwNtCmd class (which is derived from CAPWCmd) to provide the
//    APW command encapulation. This means all of the Cmd* functions become
//    member functions of CApwNtCmd.
// 2. Add APWCmdNew().
//
//    Rev 1.4   29 May 1997 13:13:40   DALLEE
// 1. Changed DllExport to APWMCMDAPI.
//    This is __declspec(dllexport) when building this dll, and
//    __declspec(dllimport) for all others.
// 2. Changed LPVOID parameter to LPVOID * -- we need to be able to
//    set the value of the LPVOID which our client tracks for us.
//
//    Rev 1.3   28 May 1997 17:32:24   JBRENNA
// 1. Change SYM_EXPORT to DllExport. The DllExport is defined n ApwCmdApi.h.
//    This means that clients of ApwCmd.DLL do not have to include platform.h
//    before including ApwCmdApi.h.
// 2. Change the szValue in GetNavwExePath from "@" to "".
//
//    Rev 1.2   28 May 1997 15:39:10   JBRENNA
// Change all "#include <...>" to '#include "..."'. This enables R:\MK\MkDeps
// to function better.
//
//    Rev 1.1   28 May 1997 15:20:14   JBRENNA
// Implement the CmdOptions() function.
//
//    Rev 1.0   28 May 1997 12:25:34   JBRENNA
// Initial revision.
//
//    Rev 1.0   24 Apr 1997 21:27:52   ksackin
// Initial revision.
//
//    Rev 1.0   13 Mar 1997 15:12:24   RFULLER
// Initial revision
//
//    Rev 1.0   08 Nov 1996 12:13:38   DKEEFER
// Initial revision
//
//    Rev 1.6   04 Aug 1996 22:10:20   DSACKIN
// Added CmdWaitForStartupPending
//
//    Rev 1.5   19 Jul 1996 01:27:34   DSACKIN
// Use our CNAVAPSVCClient object to talk to NAVAPSVC.
//
//    Rev 1.4   18 Jun 1996 22:11:36   DSACKIN
// Updated NAVAPW to support our new packet system.
//
//    Rev 1.3   21 May 1996 15:40:02   DSACKIN
// Added a new command that will scan the A: drive on logoff.
//
//    Rev 1.2   21 May 1996 00:54:44   DSACKIN
// Added a function that will start the NAVAP service.
//
//    Rev 1.1   08 May 1996 20:30:10   DSACKIN
// This file is now dedicated to ONLY hold CmdXXX functions.
//************************************************************************


//************************************************************************
// Include Files *********************************************************
//************************************************************************

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif

#include "stdafx.h"


#include "platform.h"
#include "ApwNtRes.h"                   // This DLL's resource IDs.
#include "ApwRes.h"                     // This common resource IDs.
#include "ApwNtCmd.h"                   // CApwNtCmd declaration
#include "apwutil.h"
#include "apwcfg.h"
#include "eventhandler.h"
#include "ccSymCommonClientInfo.h"

#include "EventQueue.h"
#include "DefAuthenticationQueue.h"

#include "process.h"
#include "wtsapi32.h"

#import "navapsvc.tlb" 					// For COM interfaces to service.
#include "navapcommands.h"				// Commands for service.

#include "VirusAlertPopup.h"
#include "osinfo.h"
#include "globalevent.h"
#include "globalevents.h"

#include "NAVInfo.h"

#include "commctrl.h"
#include "navtrust.h"

#include "optnames.h"                   // For options defines
// -- end cack

#include "ShutdownScan.h"

#include "savrt32.h"
#define SAVRT_SUCCEEDED(x) ((x == SAVRT_DRIVER_OK) || (x == SAVRT_DRIVER_ALREADY_LOADED) || (x == SAVRT_DRIVER_FALSE))

#include "ccResourceLoader.h"

CNAVInfo g_NAVInfo;

//************************************************************************
// Externs ***************************************************************
//************************************************************************

extern "C"      HINSTANCE               g_hInstance;

extern ::cc::CResourceLoader g_ResLoader;

//************************************************************************
// Statics ***************************************************************
//************************************************************************

//************************************************************************
// Defines ***************************************************************
//************************************************************************

#define     TRACE     ERR_TRACE_ID (_NAVAPCMD_CPP)

#define     ERROR_CALLNAMEDPIPE   (0x1000)
#define     ERROR_LOADSTRING      (0x1001)


//************************************************************************
// External Function Definitions *****************************************
//************************************************************************

//////////////////////////////////////////////////////////////////////////
// CApwNtCmd
//
// Purpose:
//    constructor.
//
//////////////////////////////////////////////////////////////////////////
// 05/31/1997 JBRENNA - Function Created
//////////////////////////////////////////////////////////////////////////
CApwNtCmd::CApwNtCmd() :
	m_pHandler( NULL ),
    m_pEventQueue( NULL ),
	m_hStopEvent( NULL ),
	m_hThread( NULL ),
    m_bFeatureEnabled( TRUE ),
    m_bIsAggregated (FALSE),
    m_pDefAuthQueue( NULL )
{
    // Init NT specific globals
    //
    ccLib::CString sTempCCPath;
    DWORD dwSize = MAX_PATH;
    if ( ccSym::CCommonClientInfo::GetCCDirectory (sTempCCPath))
    {
        g_strCCAlertPath = sTempCCPath;
        g_strCCAlertPath += _T("\\ccAlert.dll");
    }
}

//////////////////////////////////////////////////////////////////////////
// ~CApwNtCmd
//
// Purpose:
//    Do nothing. Have this available in case we need to destroy some
//    data or something.
//
//////////////////////////////////////////////////////////////////////////
// 05/31/1997 JBRENNA - Function Created
//////////////////////////////////////////////////////////////////////////
CApwNtCmd::~CApwNtCmd()
{
	if( m_pHandler )
	{
		m_pHandler->Release();
		m_pHandler = NULL;
	}

    if( m_pEventQueue )
        delete m_pEventQueue;

    if( m_pDefAuthQueue )
        delete m_pDefAuthQueue;

    // Wait here for thread to terminate.
	if( m_hThread )
	{
        ccLib::CMessageLock msgLock (TRUE, TRUE);
        if ( WAIT_TIMEOUT == msgLock.Lock ( 1, &m_hThread, FALSE, INFINITE, FALSE ))
		CloseHandle( m_hThread );
	}
}

//////////////////////////////////////////////////////////////////////////
// ApwCmdNew
//
// Purpose:
//    Create an instance of our derived APWCMD class. To free this class
//    call the C++ "delete" function.
//
// Paramters:
//    none
//
// Returns:
//    A "new"ly allocated CApwNtCmd command class.
//
//////////////////////////////////////////////////////////////////////////
// 05/31/1997 JBRENNA - Function Created
//////////////////////////////////////////////////////////////////////////

APWCMDAPI CApwCmd *WINAPI ApwCmdNew ()
{
	if(NULL != g_ResLoader.GetResourceInstance())
		return (new CApwNtCmd);

	if(g_ResLoader.Initialize())
	{
		_Module.SetResourceInstance(g_ResLoader.GetResourceInstance());
		return (new CApwNtCmd);
	}
	else
	{
		CCTRACEE(_T("%s - Failed to initialize resources"), __FUNCTION__);
		return NULL;
	}
}


//////////////////////////////////////////////////////////////////////////
// CmdInit
//
// Purpose:
//    This function initializes the data. COM must be initialized before
//    calling this function and stay initialized until DeInit() is called.
//
// Parameters:
//    hParent - ignored in NT.
//
// Return Value:
//    TRUE  - success.
//    FALSE - failure
//
//////////////////////////////////////////////////////////////////////////
// 05/27/1997 JBRENNA - Function Created
//////////////////////////////////////////////////////////////////////////
BOOL CApwNtCmd::Init( HWND hParent )
{
    CCTRACEI (_T("NAVAP Agent : Starting"));

    BOOL bSuccess = FALSE;

	// Fail query interface if the loading application is not signed
	// by Symantec.
    if(NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecApplication())
    {
        CCTRACEE (_T("CApwNtCmd::Init() - Digital signature check failed bailing out."));
		return FALSE;
    }

	InitCommonControls();

	// Figure out which nav running mutex to use.
    COSInfo osInfo;
	if( COSInfo::IsTerminalServicesInstalled() )	
		wsprintf( m_szNAVAPrunningMutex, _T("Global\\%s"), NAVAPSVC_RUNNING_MUTEX );
	else
		lstrcpyn( m_szNAVAPrunningMutex, NAVAPSVC_RUNNING_MUTEX, 32 );

    // Create event queue object.
	m_pEventQueue = new CEventQueue ();
	if( m_pEventQueue == NULL )
    {
        CCTRACEE (_T("CApwNtCmd::Init() - Failed to create event queue bailing out."));
		goto bailout;
    }

    // Start it.
    if ( !m_pEventQueue->Create(NULL,0,0))
    {
        CCTRACEE (_T("CApwNtCmd::Init() - Failed to create the event queue thread bailing out."));
        goto bailout;
    }
    
	// Create state thread.
	m_hStopEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	if( m_hStopEvent == NULL )
    {
        CCTRACEE (_T("CApwNtCmd::Init() - Failed to create the stop event bailing out."));
		goto bailout;
    }

    //
    // Initialize the values we have stored in ccSettings, the Init version of this function
    // will use a local copy of the settings helper where as the Get version will use the
    // member copy. This avoids having two different threads talking to the same ccSettings
    // object which is causing a failure and reconnection to the ccSettings Manager.
    //
    InitCCsettingsOptions();

	unsigned uID;
	m_hThread = (HANDLE)_beginthreadex( NULL, 0, &maintenenceThreadProc, this, 0, &uID );
	if( m_hThread == NULL )
    {
        CCTRACEE (_T("CApwNtCmd::Init() - Failed to create maintenence thread bailing out."));
		goto bailout;
    }

	// Register ourself with NAVAPSVC
	if( isServiceRunning() && registerHandler() == FALSE )
    {
        CCTRACEE (_T("CApwNtCmd::Init() - Failed to register with the navapsvc bailing out."));
		goto bailout;
    }

	bSuccess = TRUE;

bailout:

    if( !bSuccess )
        CCTRACEE (_T("CApwNtCmd::Init() - Error: Init() did not succeed."));

	// Tell the world that the state of AP has changed.
	CGlobalEvent event;
	if( event.Create( SYM_REFRESH_AP_STATUS_EVENT ) )
	{
		::PulseEvent ( event );
	}

	return bSuccess;
}


//////////////////////////////////////////////////////////////////////////
// CmdDeInit
//
// Purpose:
//    Matching uninitialization for command object.
//
// Parameters:
//    None.
//
// Return Value:
//    TRUE  - success.
//    FALSE - failure
//
//////////////////////////////////////////////////////////////////////////
// 05/27/1997 JBRENNA - Function Created
//////////////////////////////////////////////////////////////////////////
BOOL CApwNtCmd::DeInit()
{
	CCTRACEI ( "CApwNtCmd::DeInit() - calling unRegisterHandler()" );

    // Unregister with NAVAPSVC.
	unRegisterHandler( );

	// Stop thread.
	if( m_hThread )
	{
        ::SetEvent ( m_hStopEvent );
        ccLib::CMessageLock msgLock (TRUE, TRUE);
        msgLock.Lock ( 1, &m_hThread, FALSE, INFINITE, FALSE );
		CloseHandle( m_hStopEvent );
        m_hStopEvent = NULL;
	}

    CCTRACEI ( "CApwNtCmd::DeInit() - exiting" );

    return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// CmdOptions
//
// Purpose:
//    This routine displays the NAV options panel.
//
// Parameters:
//    None.
//
// Return Value:
//    TRUE  - success.
//    FALSE - failure
//
//////////////////////////////////////////////////////////////////////////
// 05/27/1997 JBRENNA - Function Created
//////////////////////////////////////////////////////////////////////////
BOOL CApwNtCmd::Options (HWND hParent)
{
	TCHAR szPath[MAX_PATH];

	::_tcscat(::_tcscpy(szPath, g_NAVInfo.GetNAVDir()), _T("\\NAVStub.exe"));
	::ShellExecute(hParent, NULL, szPath, NULL, NULL, SW_SHOW);

	return ( FALSE );
}


//////////////////////////////////////////////////////////////////////////
// CmdWaitForStartupPending
//
// Purpose:
//    This function will send a code to NAVAPSVC that will only return
//    when it is either done starting up or an error happens.
//
// Parameters:
//    None.
//
// Return Value:
//    TRUE  - Command sent successfully.
//    FALSE - Command failure
//
//////////////////////////////////////////////////////////////////////////
// 05/08/1996 DSACKIN - Function Created
// 07/17/1996 DSACKIN - Reworked to use our RPC based object
// 05/27/1997 JBRENNA - Updated so that NAVAPW can link to this DLL
//////////////////////////////////////////////////////////////////////////
BOOL CApwNtCmd::WaitForStartupPending()
{
	BOOL bRet = FALSE;
	
	// Make sure service is up and running.
	if( !isServiceRunning() )
		return FALSE;

	try
	{
		// Create object and call it.
		NAVAPSVCLib::INAVAPServicePtr p( __uuidof( NAVAPSVCLib::NAVAPService ) );
		_bstr_t sCommand( NAVAPCMD_WAITFORSTARTUP );
		bRet = (S_OK == p->SendCommand( sCommand, NULL ) );
	}
	catch( _com_error e )
	{
		SYM_ASSERT( FALSE );
	}

	return bRet;
}


//////////////////////////////////////////////////////////////////////////
// CmdDisableDriver
//
// Purpose:
//    This function will send a code through a pipe to the NAVAP
//    service which will in turn tell the Device driver to disable.
//
// Parameters:
//    None.
//
// Return Value:
//    TRUE  - Command sent successfully.
//    FALSE - Command failure
//
//////////////////////////////////////////////////////////////////////////
// 05/08/1996 DSACKIN - Function Created
// 07/17/1996 DSACKIN - Reworked to use our RPC based object
// 05/27/1997 JBRENNA - Updated so that NAVAPW can link to this DLL
//////////////////////////////////////////////////////////////////////////
BOOL CApwNtCmd::DisableDriver()
{
	CCTRACEI("DisableDriver");

	BOOL bRet = FALSE;

	// Make sure service is up and running.
	if( !isServiceRunning() )
		return FALSE;

	try
	{
		// Create object and call it.
		NAVAPSVCLib::INAVAPServicePtr p( __uuidof( NAVAPSVCLib::NAVAPService ) );
		_bstr_t sCommand( NAVAPCMD_DISABLE );
		bRet = (S_OK == p->SendCommand( sCommand, NULL ) );

		// Tell the world that the state of AP has changed.
		CGlobalEvent event;
		if( event.Create( SYM_REFRESH_AP_STATUS_EVENT ) )
		{
			::PulseEvent ( event );
		}
	}
	catch( _com_error e )
	{
		SYM_ASSERT( FALSE );
	}

	return bRet;
}


//////////////////////////////////////////////////////////////////////////
// CmdEnableDriver
//
// Purpose:
//    This function will send a code through a pipe to the NAVAP
//    service which will in turn tell the Device driver to enable.
//
// Parameters:
//    None.
//
// Return Value:
//    TRUE  - Command sent successfully.
//    FALSE - Command failure
//
//////////////////////////////////////////////////////////////////////////
// 05/08/1996 DSACKIN - Function Created
// 07/17/1996 DSACKIN - Reworked to use our RPC based object
// 05/27/1997 JBRENNA - Updated so that NAVAPW can link to this DLL
//////////////////////////////////////////////////////////////////////////
BOOL CApwNtCmd::EnableDriver()
{
	CCTRACEI("EnableDriver");

	BOOL bRet = FALSE;

	try
	{
		// Create object and call it.
		NAVAPSVCLib::INAVAPServicePtr p( __uuidof( NAVAPSVCLib::NAVAPService ) );
		_bstr_t sCommand( NAVAPCMD_ENABLE );
		bRet = (S_OK == p->SendCommand( sCommand, NULL ) );

		// If the service us up and running, make sure
		// we are connected.  If we are not yet connected,
		// register with the service.
		if( m_pHandler == NULL )
		{
			if( !registerHandler() )
				return FALSE;
		}

		// Tell the world that the state of AP has changed.
		CGlobalEvent event;
		if( event.Create( SYM_REFRESH_AP_STATUS_EVENT ) )
		{
			::PulseEvent ( event );
		}
	}
	catch( _com_error e )
	{
		SYM_ASSERT( FALSE );
	}

	return bRet;
}



//////////////////////////////////////////////////////////////////////////
// CmdGetStatusOfDriver
//
// Purpose:
//    This function will send a code through a pipe to the NAVAP
//    service which will query the status of the driver.  Additionally,
//    this routine makes sure we are connected to the service, if the 
//    service is running.  The reason this code is in here is because this 
//    function is called from the main application thread.
//
// Parameters:
//    None.
//
// Return Value:
//    AP_STATE_ENABLED  - Device is enabled.
//    AP_STATE_DISABLED - Device is disabled.
//    AP_STATE_ERROR    - A problem occured.
//
//////////////////////////////////////////////////////////////////////////
// 05/08/1996 DSACKIN - Function Created
// 07/17/1996 DSACKIN - Reworked to use our RPC based object
// 05/27/1997 JBRENNA - Updated so that NAVAPW can link to this DLL
//////////////////////////////////////////////////////////////////////////
AP_STATE CApwNtCmd::GetStatusOfDriver()
{
    // Assume the worst - diabled
	AP_STATE ApState = AP_STATE_DISABLED;

    SAVRT_DRIVER_STATUS     savrtReturn = SAVRT_DRIVER_ERROR;
    
    CAutoProtectWrapper APWrapper (true);

    // Get the state of the driver
    BOOL bEnabled = FALSE;
    savrtReturn = APWrapper.GetEnabledState( &bEnabled );
	if ( SAVRT_SUCCEEDED(savrtReturn) )
    {
        ApState = bEnabled ? AP_STATE_ENABLED : AP_STATE_DISABLED;
        CCTRACEI(_T("CApwNtCmd::GetStatusOfDriver() : SAVRT_GetEnabledState() returned - %d"), ApState);
    }
    else
        CCTRACEE (_T("CApwNtCmd::GetStatusOfDriver() : SAVRT_GetEnabledState failed - %d"), savrtReturn );

    // Connect to the service if it is up and running
	if( !isServiceRunning() )
	{
		// Service is no longer running.  Release handler.
		if( m_pHandler )
		{
            CCTRACEW(_T("CApwNtCmd::GetStatusOfDriver() - The service is no longer running, releasing handler"));
			m_pHandler->Release();
			m_pHandler = NULL;
		}
	}
	else
	{
		// If the service is up and running, make sure
		// we are connected.  If we are not yet connected,
		// register with the service.
		if( m_pHandler == NULL )
		{
			if( !registerHandler() )
                CCTRACEE(_T("CApwNtCmd::GetStatusOfDriver() - Failed to register handler"));
            else
                CCTRACEI(_T("CApwNtCmd::GetStatusOfDriver() - Successfully registered handler with the AP service"));
		}
	}

	return ApState;
}


//////////////////////////////////////////////////////////////////////////
// CmdStartDriver
//
// Purpose:
//    This function will attempt to start the AP Service which will
//    in turn start all of the device drivers.
//
// Parameters:
//    None.
//
// Return Value:
//    TRUE  - Drivers started successfully.
//    FALSE - Failure to start them.
//
//////////////////////////////////////////////////////////////////////////
// 05/20/1996 DSACKIN - Function Created
// 05/27/1997 JBRENNA - Updated so that NAVAPW can link to this DLL
//////////////////////////////////////////////////////////////////////////
BOOL CApwNtCmd::StartDriver()
{
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// CmdStopDriver
//
// Purpose:
//    This function will attempt to stop the AP Service. This is
//    not currently implemented.
//
// Parameters:
//    None.
//
// Return Value:
//    TRUE  - Drivers started successfully.
//    FALSE - Failure to start them.
//
//////////////////////////////////////////////////////////////////////////
// 05/27/1997 JBRENNA - Function Created
//////////////////////////////////////////////////////////////////////////
BOOL CApwNtCmd::StopDriver()
{
    return FALSE; // Fail - this function is not implemented.
}

//////////////////////////////////////////////////////////////////////////
// CmdShutdownScan
//
// Purpose:
//    This function will call the device to scan the A drive.  If the
//    floppy is infected, the user will be prompted to remove the
//    floppy from the A drive and the scan will happen again.
//
// Parameters:
//    None.
//
// Return Value:
//    TRUE  - Continue shutdown
//    FALSE - Fail shutdown
//
//////////////////////////////////////////////////////////////////////////
// 05/20/1996 DSACKIN - Function Created
// 07/17/1996 DSACKIN - Reworked to use our RPC based object
// 05/27/1997 JBRENNA - Updated so that NAVAPW can link to this DLL
//////////////////////////////////////////////////////////////////////////
BOOL CApwNtCmd::ShutdownScan(
   HWND   hDlg)                         // [in] - Parent window
{
    CCTRACEI ( _T("CApwNtCmd::ShutdownScan - starting"));
    BOOL bRet = FALSE;

	// Make sure we are the active session.  If not, allow shutdown.
	if( COSInfo::IsActiveSession() == false )
    {
        CCTRACEI ( _T("CApwNtCmd::ShutdownScan - not the active session, exit"));	
        return TRUE;
    }

	// Check to see if we are configured for this operation.
	if( ApwGetConfig()->GetScanFloppyOnShutdown() == FALSE )
    {
        CCTRACEI ( _T("CApwNtCmd::ShutdownScan - not scanning on shutdown, exit"));	
		return TRUE;  // Allow shutdown allways.
    }

    CCTRACEI ( _T("CApwNtCmd::ShutdownScan - start scanner"));
    CShutdownScan Scan (this);
    Scan.Create (NULL,0,0);
    Scan.WaitForExit( INFINITE );  

     // Ok to shutdown.
    CCTRACEI ( _T("CApwNtCmd::ShutdownScan - exiting OK"));
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// ConfirmApwComTasksReceived
//
// Purpose:
//    This function does nothing in NT.
//
// Parameters:
//    None.
//
// Return Value:
//    None.
//
//////////////////////////////////////////////////////////////////////////
VOID CApwNtCmd::ConfirmApwComTasksReceived( VOID )
{
}

//////////////////////////////////////////////////////////////////////////
// QueueEvent
//
// Purpose:
//    Called by INAVAPEventHandler object when events are recieved.
//
// Parameters:
//    SAFEARRAY* psaEvent - pointer to a SAFEARRAY object containing 
//				 event data.
//
// Return Value:
//    None.
//
//////////////////////////////////////////////////////////////////////////

void CApwNtCmd::QueueEvent( CEventData*& pEventData )
{
	if ( !pEventData )
        return;

	// Add it to the queue
	//
    m_pEventQueue->AddItem( pEventData );    
}

//////////////////////////////////////////////////////////////////////////
// registerHandler
//
// Purpose:
//    Creates instance of event handler object and registers it 
//    with NAVAPSVC. 
//
// Parameters:
//    None.
//
// Return Value:
//    None.
//
//////////////////////////////////////////////////////////////////////////

BOOL CApwNtCmd::registerHandler( )
{
	BOOL bRet = FALSE;

	// Make sure service is up and running.
	if( !isServiceRunning() )
		return FALSE;

	try
	{
		// Allocate handler object.
		m_pHandler = new CEventHandler( *this );
		if( m_pHandler == NULL )
        {
            CCTRACEE (_T("CApwNtCmd::registerHandler() - Failed to create the event handler."));
			return FALSE;
        }

		// Keep a reference count for ourselves.
		m_pHandler->AddRef();

		// Create NAVAPService object instance.
		NAVAPSVCLib::INAVAPServicePtr p( __uuidof( NAVAPSVCLib::NAVAPService ) );
		bRet = (S_OK == p->RegisterHandler( reinterpret_cast<NAVAPSVCLib::INAVAPEventHandler*>( m_pHandler ) ) );

        if( !bRet )
            CCTRACEE (_T("CApwNtCmd::registerHandler() - Error: Call to the APService RegsisterHandler() method failed."));
	}
	catch( _com_error e )
	{
        CCTRACEE(_T("CApwNtCmd::registerHandler() - Caught COM Exception. HRESULT = 0x%x, Error = %s."), e.Error(), e.Description() );
		SYM_ASSERT( FALSE );
	}

	return bRet;
}

//////////////////////////////////////////////////////////////////////////
// unRegisterHandler
//
// Purpose:
//    Unregisters our handler with NAVAPSVC
//
// Parameters:
//    None.
//
// Return Value:
//    None.
//
//////////////////////////////////////////////////////////////////////////

BOOL CApwNtCmd::unRegisterHandler()
{
	BOOL bRet = FALSE;

	// Make sure service is up and running.
	if( !isServiceRunning() )
		return FALSE;

	try
	{
		// Make sure we should be here
		if( m_pHandler == NULL )
			return FALSE;

		// Create NAVAPService object instance.
		NAVAPSVCLib::INAVAPServicePtr p( __uuidof( NAVAPSVCLib::NAVAPService ) );
		bRet = (S_OK == p->UnRegisterHandler( reinterpret_cast<NAVAPSVCLib::INAVAPEventHandler*>( m_pHandler ) ) );
	}
	catch( _com_error e )
	{
		SYM_ASSERT( FALSE );
	}

	return bRet;
}

//////////////////////////////////////////////////////////////////////////
// isServiceRunning
//
// Purpose:
//    Checks for the existance of the global mutex which indicates if the
//    NAVAPSVC is running.
//
// Parameters:
//    None.
//
// Return Value:
//    BOOLEAN - True if service is running, false otherwise.
//
//////////////////////////////////////////////////////////////////////////

BOOL CApwNtCmd::isServiceRunning()
{
	BOOL bRet = FALSE;
	HANDLE hMutex = OpenMutex( SYNCHRONIZE, FALSE, m_szNAVAPrunningMutex );
	if( hMutex )
	{
		bRet = TRUE;
		CloseHandle( hMutex );
	}

    if( !bRet )
         CCTRACEE (_T("CApwNtCmd::isServiceRunning() - Failed to open the AP running mutex bailing out."));

	return bRet;
}

//////////////////////////////////////////////////////////////////////////
// OnServiceStopped
//
// Purpose:
//    This notification is sent by the service when it has been shutdown.
//
// Parameters:
//    None.
//
// Return Value:
//    None.
//
//////////////////////////////////////////////////////////////////////////

void CApwNtCmd::OnServiceStopped()
{
	// Perform any tasks that need to be done when the service shuts down.

    // Tell the world that the state of AP has changed.
	CGlobalEvent event;
	if( event.Create( SYM_REFRESH_AP_STATUS_EVENT ) )
	{
		::PulseEvent ( event );
	}
}								

//////////////////////////////////////////////////////////////////////////
// maintenenceThreadProc
//
// Purpose:
//    This thread watches the current state of NAVAPSVC, and forces UI 
//	  updates when the state changes.  Also watches for options changes.
//
// Parameters:
//    pData - Thread data
//
// Return Value:
//    None.
//
//////////////////////////////////////////////////////////////////////////

unsigned CApwNtCmd::maintenenceThreadProc( void* pData )
{
	CApwNtCmd* pThis = (CApwNtCmd*) pData;
	BOOL bCurrentState, bSavedState = FALSE;

	// Create event object
	CGlobalEvent event;
	if( false == event.Create( SYM_OPTIONS_CHANGED_EVENT ) )
	{
		SYM_ASSERT( FALSE );
		return -1;
	}

    ccLib::CCoInitialize COM;
    COM.CoInitialize ( ccLib::CCoInitialize::eMTAModel );

	// Start waiting.
	HANDLE aHandles[2] = { pThis->m_hStopEvent, event };

    ccLib::CMessageLock msgLock (TRUE, TRUE);

	for(;;)
	{
		// Wait here
        DWORD dwWait = msgLock.Lock ( sizeof(aHandles)/sizeof(aHandles[0]), aHandles, FALSE, 1000 , 0 );

		// Check for exit signal
		if( dwWait - WAIT_OBJECT_0 == 0 )
		{
			// Yes, bail out.
			break;
		} else 
		
		// Check to see if options were changed.
		if( dwWait - WAIT_OBJECT_0 == 1 )
		{
			// Yes.  Reload.
			ApwGetConfig()->ReadNavopts();
            pThis->InitCCsettingsOptions (); // Must refresh helper due to caching
			ApwUpdateUI();
		} else

		// Check for timeout.
		if( dwWait == WAIT_TIMEOUT )
		{
			// Get the current state of NAVAPSVC
			bCurrentState = pThis->isServiceRunning();
			
			// If the state has changed, update things.
			if( bCurrentState != bSavedState )
			{
				ApwUpdateUI();
				bSavedState = bCurrentState;
			}
		}
	}

    _endthreadex(0);
	return 0;
}

void CApwNtCmd::InitCCsettingsOptions()
{
    //
    // Initialize all of the options stored in ccSettings using a local copy of
    // the settings object
    //
    CNAVOptSettingsEx options;
    DWORD dwBlockedDelay = 2*1000;
    DWORD dwWaitTimeMinDisplayBlocked = 2*1000;
    DWORD dwWaitTimeMaxDisplayBlocked = 2*60*1000;
    DWORD dwMaxActiveBlocked = 3;
    DWORD dwDisplayAlert = 1;
    DWORD dwForcedAction = 0;
    DWORD dwAutoScanSideEffects = 0;

    if ( options.Init())
    {
	    options.GetValue (APCOMPRESSED_MaxActiveBlocked, dwMaxActiveBlocked, dwMaxActiveBlocked );

        // For the UI
        //
        options.GetValue (APCOMPRESSED_BlockedDelay, dwBlockedDelay, dwBlockedDelay );
        options.GetValue (APCOMPRESSED_WaitTimeMinDisplayBlocked, dwWaitTimeMinDisplayBlocked, dwWaitTimeMinDisplayBlocked );
        options.GetValue (APCOMPRESSED_WaitTimeMaxDisplayBlocked, dwWaitTimeMaxDisplayBlocked, dwWaitTimeMaxDisplayBlocked );
        options.GetValue (APCOMPRESSED_WaitTimeMaxDisplayBlocked, dwWaitTimeMaxDisplayBlocked, dwWaitTimeMaxDisplayBlocked );
        options.GetValue (APCOMPRESSED_DisplayAlert, dwDisplayAlert, dwDisplayAlert );
        options.GetValue (APCOMPRESSED_ForcedAction, dwForcedAction, dwForcedAction );
        options.GetValue (AUTOPROTECT_AutoScanSideEffects, dwAutoScanSideEffects, dwAutoScanSideEffects );

        // Feature enabled
        DWORD dwValue = 1;
        options.GetValue (LICENSE_FeatureEnabled, dwValue, 1);
        m_bFeatureEnabled = dwValue != 0;

        // Aggrgated?
        dwValue = 0;
        options.GetValue (LICENSE_Aggregated, dwValue, 0);
        m_bIsAggregated = dwValue != 0;
    }
    else
        CCTRACEE(_T("CApwNtCmd::InitCCsettingsOptions() - Failed to initialize NAVOptSettingsEx."));

    CBlockedAppGroup::SetWaitBeforeDisplay ( dwBlockedDelay );
    CBlockedAppUI::SetWaitTimeMinDisplayBlocked ( dwWaitTimeMinDisplayBlocked );
    CBlockedAppUI::SetWaitTimeMaxDisplayBlocked ( dwWaitTimeMaxDisplayBlocked );
    CBlockedAppUI::SetForcedAction ( dwForcedAction );
    CBlockedAppQueue::SetMaxActiveBlocked ( dwMaxActiveBlocked );
    CVirusAlertQueue::SetDisplayAlert ( (bool)(dwDisplayAlert == 1) );
    CCTRACEI("CApwNtCmd::InitCcSettingsOptions - AutoScanSE: %d", dwAutoScanSideEffects);
    CVirusAlertQueue::SetAutoScanSideEffects (dwAutoScanSideEffects ? true : false);
}

BOOL CApwNtCmd::FeatureEnabled( VOID )
{
    CCTRACEI(_T("CApwNtCmd::FeatureEnabled() - FeatureEnabled bit returning %d."), m_bFeatureEnabled);
    return m_bFeatureEnabled;
}

BOOL CApwNtCmd::IsAggregated( VOID )
{
    CCTRACEI(_T("CApwNtCmd::IsAggregated() - IsAggregated bit returning %d."), m_bIsAggregated);
    return m_bIsAggregated;
}

void CApwNtCmd::DisplayDefAuthenticationFailure(DWORD dwFailureCode)
{
    // Create the queue if necessary
    if( m_pDefAuthQueue == NULL )
    {
        m_pDefAuthQueue = new CDefAuthenticationAlertQueue;
	    if( m_pDefAuthQueue == NULL )
        {
            CCTRACEE (_T("CApwNtCmd::DisplayDefAuthenticationFailure() - Failed to create the def authentication failure event queue to alert the user."));
		    return;
        }
        
        // Start it.
        if ( !m_pDefAuthQueue->Create(NULL,0,0))
        {
            CCTRACEE (_T("CApwNtCmd::DisplayDefAuthenticationFailure() - Failed to start the def authentication failure event queue to alert the user."));
            delete m_pDefAuthQueue;
            return;
        }
    }

    // Now the queue is started so add the failure
    CCTRACEI(_T("CApwNtCmd::DisplayDefAuthenticationFailure() - Adding failure code %d to the authentication alert queue"), dwFailureCode);
    m_pDefAuthQueue->AddItem(dwFailureCode);
    return;
}