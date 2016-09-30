// ServiceUtil.cpp: implementation of the CServiceUtil class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ServiceUtil.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CServiceUtil::StartService()

bool CServiceUtil::StartService( LPCTSTR szServiceName )
{
	// Open handle to Service control manager.
	CServiceHandle hScm = OpenSCManager( NULL, 
		SERVICES_ACTIVE_DATABASE,  
		SC_MANAGER_ALL_ACCESS );

	if( hScm.IsValid() == false )
		return false;

	// Open handle to specified service
	CServiceHandle hService = OpenService( hScm, szServiceName, SERVICE_START | SERVICE_QUERY_STATUS );
	if( hService.IsValid() == false )
		return false;

	// Try 5 times to start the service.
	bool bRet;
	for( int i = 0; i < 5; i++ )
	{
		// Start the service.
		bRet = ::StartService( hService, 0, NULL ) == TRUE;
		if( bRet )
			break;

		// If the service is already running, all is well.
		if( GetLastError() == ERROR_SERVICE_ALREADY_RUNNING )
			return true;

		// An error has occurred.  Retry.
		Sleep( 200 );
	}
	
	// Wait here for service to fully start
	if( bRet )
		bRet = waitForState( hService, SERVICE_RUNNING );

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
// CServiceUtil::StopService()

bool CServiceUtil::StopService( LPCTSTR szServiceName )
{
	// Open handle to Service control manager.
	CServiceHandle hScm = OpenSCManager( NULL, 
		SERVICES_ACTIVE_DATABASE,  
		SC_MANAGER_ALL_ACCESS );

	if( hScm.IsValid() == false )
		return false;

	// Open handle to specified service
	CServiceHandle hService = OpenService( hScm, szServiceName, SERVICE_STOP | SERVICE_QUERY_STATUS  );
	if( hService.IsValid() == false )
		return false;

	// Stop the service.
	SERVICE_STATUS status;
	bool bRet = ::ControlService( hService, SERVICE_CONTROL_STOP, &status ) == TRUE;
	if( bRet == false )
		return false;

	// Wait here for service to fully stop
	bRet = waitForState( hService, SERVICE_STOPPED );
	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
// CServiceUtil::ChangeServicePath()

bool CServiceUtil::ChangeServicePath( LPCTSTR szServiceName, LPCTSTR szServicePath )
{
	// Open handle to Service control manager.
	CServiceHandle hScm = OpenSCManager( NULL, 
		SERVICES_ACTIVE_DATABASE,  
		SC_MANAGER_ALL_ACCESS );

	if( hScm.IsValid() == false )
    {
        CCTRACEE(_T("CServiceUtil::ChangeServicePath() - Failed to open the SCM"));
		return false;
    }

	// Open handle to specified service
	CServiceHandle hService = OpenService( hScm, szServiceName, SERVICE_ALL_ACCESS );
	if( hService.IsValid() == false )
    {
        CCTRACEE(_T("CServiceUtil::ChangeServicePath() - Failed to open the service %s to set path %s"), szServiceName, szServicePath);
		return false;
    }

	// Check to see if the current service path is the same as the one we are 
	// trying to set.  If so, no work needs to be done.
	DWORD dwBytesUsed = 0;
	vector<unsigned char> vBuffer( 4096 );
	LPQUERY_SERVICE_CONFIG pServiceConfig = reinterpret_cast<LPQUERY_SERVICE_CONFIG>( &vBuffer[0] );
	if( QueryServiceConfig( hService, pServiceConfig, 4096, &dwBytesUsed ) )
	{
		// Q: Do they match?
		if( _tcsicmp( _tcsninc(pServiceConfig->lpBinaryPathName,4), szServicePath ) == 0 )
		{
            CCTRACEI(_T("CServiceUtil::ChangeServicePath() - The path %s matches for service %s. So returning true without changing it."), szServicePath, szServiceName);
			return true;
		}
	}


	// Change the path to the one specified.
	return ::ChangeServiceConfig( hService, 
							SERVICE_NO_CHANGE,
							SERVICE_NO_CHANGE,
							SERVICE_NO_CHANGE,
							szServicePath,
							NULL,
							NULL,
							NULL,
							NULL,
							NULL,
							NULL ) == TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CServiceUtil::waitForState()

bool CServiceUtil::waitForState( SC_HANDLE hService, DWORD dwState )
{
	SERVICE_STATUS status;

    // Check the status until the service is no longer start pending. 
    if (!QueryServiceStatus( 
            hService,     // handle to service 
            &status) )  // address of status information structure
    {
        return false;
    }
 
    // Save the tick count and initial checkpoint.

    DWORD dwStartTickCount = GetTickCount();
    DWORD dwOldCheckPoint = status.dwCheckPoint;
    ccLib::CEvent eventNothing;
    ccLib::CMessageLock msgLock (TRUE, TRUE);

    while (status.dwCurrentState != dwState ) 
    { 
        // Do not wait longer than the wait hint. A good interval is 
        // one tenth the wait hint, but no less than 1 second and no 
        // more than 10 seconds. 
 
        DWORD dwWaitTime = status.dwWaitHint / 10;

        if( dwWaitTime < 1000 )
            dwWaitTime = 1000;
        else if ( dwWaitTime > 10000 )
            dwWaitTime = 10000;

        // wait and pump messages
        eventNothing.Create ( NULL, FALSE, FALSE, NULL, FALSE);
        msgLock.Lock ( eventNothing, dwWaitTime );

        // Check the status again. 
 
        if (!QueryServiceStatus( 
                hService,   // handle to service 
                &status) )  // address of structure
            break; 
 
        if ( status.dwCheckPoint > dwOldCheckPoint )
        {
            // The service is making progress.

            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = status.dwCheckPoint;
        }
        else
        {
            if(GetTickCount()-dwStartTickCount > status.dwWaitHint)
            {
                // No progress made within the wait hint
                break;
            }
        }
    } 

    if (status.dwCurrentState == dwState) 
    {
        return true;
    }

	return false;
}
