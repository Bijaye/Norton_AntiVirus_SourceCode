// Service.cpp: implementation of the CService class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Service.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CService::CService(LPCTSTR lpcszServiceName)
{
    _tcscpy ( m_lpszServiceName, lpcszServiceName);

    m_dwCurrentAccess = 0;

    // Open the service for most things.
    //
    m_hSvcMgr = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CONNECT);
}

CService::~CService()
{
    // m_hService & m_hSvcMgr are smart so they close automagically
}

/*
// This code hasn't been tested and isn't used by the 'client' object so at the
// request of QA I'm commenting it out. I do intend to reuse this code later
// so I am keeping it around. - Garret Polk Jan. 14, 2001

BOOL CService::Start()
{
    try 
    {
        if ( !openService ( SERVICE_START |
                            SERVICE_QUERY_STATUS |
                            SERVICE_PAUSE_CONTINUE ))
        {
            CCTRACEE ( "Couldn't open service in CService::Start()" );
            throw FALSE;
        }

        SERVICE_STATUS SvcStatus;

        if (QueryServiceStatus(m_hService, &SvcStatus) == FALSE)
        {
            CCTRACEE ( "Couldn't QueryServiceStatus in CService::Start()" );
            throw FALSE;
        }

        BOOL bResult = FALSE;

        // Tell it to start
        //
	    if ( GetCurrentState() == SERVICE_STOPPED )
            bResult = StartService(m_hService, 0, NULL);
        else
            bResult = ControlService ( m_hService, SERVICE_CONTROL_CONTINUE, &SvcStatus );

        if ( !bResult )
        {
            CCTRACEE ( "Couldn't start/continue service in CService::Start()" );
            throw FALSE;
        }

        //
        // Wait 5 secs for service to start
        //
        if ( !waitForService ( SERVICE_RUNNING, 50 ))
        {
            CCTRACEE ( "waitForService failed in CService::Start()" );
            throw FALSE;
        }
    }

    catch (...)
    {
        DWORD dwResult = GetLastError ();   // for debugging
        return FALSE;
    }

    return TRUE;    
}

BOOL CService::Stop()
{
    //
    // Stop the service
    //
    try 
    {
        if ( !openService ( SERVICE_STOP |
                            SERVICE_QUERY_STATUS |
                            SERVICE_PAUSE_CONTINUE ))
        {
            CCTRACEE ( "Couldn't open service in CService::Start()" );
            throw FALSE;
        }

        SERVICE_STATUS SvcStatus;
        
        ControlService ( m_hService, SERVICE_CONTROL_STOP, &SvcStatus );

        //
        // Wait 5 secs for service to stop
        //
        if ( !waitForService ( SERVICE_STOP, 1 ))
        {
            CCTRACEE ( "waitForService failed in CService::Stop()" );
            throw FALSE;
        }
    }

    catch (...)
    {
        DWORD dwResult = GetLastError ();   // for debugging
        return FALSE;
    }

    return TRUE;
}
*/

// dwRetries are in 1/10th secs.
//
BOOL CService::waitForService(DWORD dwState, DWORD dwRetries )
{
    DWORD dwAttempts = 0;
    DWORD dwResult = GetCurrentState ();

    while ( dwAttempts < dwRetries && dwResult != dwState )
    {
        dwAttempts ++;
        Sleep ( 100 ); // wait 1/10th sec.
        dwResult = GetCurrentState ();
    };

    // Return TRUE if the result matches what was requested.
    //
    return ( dwResult == dwState ) ? TRUE : FALSE; 
}

DWORD CService::GetCurrentState()
{
    SERVICE_STATUS SvcStatus;

    try 
    {
        if ( !openService ( SERVICE_QUERY_STATUS ))
        {
            CCTRACEE ( "Couldn't open service in CService::GetCurrentState" );
            throw FALSE;
        }

        if (QueryServiceStatus(m_hService, &SvcStatus) == FALSE)
        {
            CCTRACEE ( "Error in CService::GetCurrentState - QueryServiceStatus");
            throw FALSE;
        }
    }

    catch (...)
    {
        DWORD dwResult = GetLastError ();   // for debugging
        return 0;
    }

    return SvcStatus.dwCurrentState;
}

BOOL CService::ChangeStartType( DWORD dwStartType )
{
    try
    {
        // We need full access to the SCM for this operation.
        //
        m_hSvcMgr.CloseServiceHandle ();

        m_hSvcMgr = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ALL_ACCESS);

        if ( !openService ( SERVICE_CHANGE_CONFIG ))
        {
            CCTRACEE ( "Couldn't open service in CService::GetCurrentState" );
            throw FALSE;
        }

        if ( !::ChangeServiceConfig( m_hService,
                                     SERVICE_NO_CHANGE,
                                     dwStartType,
				                     SERVICE_NO_CHANGE,
				                     NULL,
				                     NULL,
				                     NULL,
				                     NULL,
				                     NULL,
				                     NULL,
						             NULL ))
        {
            CCTRACEE ( "Error in CService::ChangeStartType() - %s", dwStartType );
            throw FALSE;
        }
    }
    catch (...)
    {
        DWORD dwResult = GetLastError ();   // for debugging
        return FALSE;
    }

    return TRUE;
}

BOOL CService::openService(DWORD dwDesiredAccess)
{
    // If we already have the requested access just return TRUE.
    //
    if (( m_dwCurrentAccess & dwDesiredAccess ) == dwDesiredAccess )
        return TRUE;

    // Open for MORE access than before.
    //
    m_dwCurrentAccess = m_dwCurrentAccess | dwDesiredAccess;

    if ( m_hService )
        m_hService.CloseServiceHandle ();

    m_hService = OpenService(  m_hSvcMgr,
	 				           m_lpszServiceName,
					           m_dwCurrentAccess );

    return m_hService ? TRUE : FALSE;
}
