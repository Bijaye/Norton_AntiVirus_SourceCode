#include "stdafx.h"
#include "CServiceInstall.h"

#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>


CServiceInstall::CServiceInstall( LPTSTR szServiceName, LPTSTR szDisplay, LPTSTR szPath )
{
	_tcscpy( m_service, szServiceName );
	_tcscpy( m_display, szDisplay );
    _tcscpy( m_path, szPath );
}

void CServiceInstall::Install(DWORD dwType, DWORD dwStart,
							   LPCTSTR lpDepends, LPCTSTR lpName,
							   LPCTSTR lpPassword )
{
	SC_HANDLE hSCM = NULL;
	SC_HANDLE hService = NULL;
	
	if( IsInstalled() == TRUE )
		return;
	
	hSCM = OpenSCManager( NULL,
							NULL,
							SC_MANAGER_CREATE_SERVICE );
	if ( !hSCM )
	{
		ErrorPrinter( _TEXT("OpenSCManager") );
		goto cleanup;
	}

	hService = CreateService( 
		hSCM,
		m_service,
		m_display,
		SERVICE_ALL_ACCESS,
		dwType,
		dwStart,
		SERVICE_ERROR_NORMAL,
		m_path,
		NULL,
		NULL,
		lpDepends,
		lpName,
		lpPassword );

	if ( !hService ) 
	{
		ErrorPrinter( _TEXT("CreateService") );
		goto cleanup;
	}
	else
	{
		_tprintf(_TEXT("%s Created\n"), m_service);
	}

cleanup:
	if (hService) CloseServiceHandle( hService );
	if (hSCM) CloseServiceHandle( hSCM );
	
	return;
}

void CServiceInstall::Remove( BOOL bForce )
{
	SC_HANDLE hSCM = NULL;
	SC_HANDLE hService = NULL;
	BOOL bSuccess = FALSE;

	hSCM = ::OpenSCManager( NULL,
							  NULL,
							  SC_MANAGER_CONNECT );
	if ( !hSCM )
	{
		ErrorPrinter( _TEXT("OpenSCManager") );
		return;
	}
	
	hService = ::OpenService(
		hSCM,
		m_service,
		DELETE );						
	if ( !hService ) 
	{
		ErrorPrinter( _TEXT("OpenService") );
		goto cleanup;
	}
	
	//Force the service to stop 
	if( bForce == TRUE ) 
	{
		SERVICE_STATUS status;
		::ControlService(hService, SERVICE_CONTROL_STOP, &status);
		_tprintf(_TEXT("%s stopped\n"), m_service);
	}

	bSuccess = ::DeleteService( hService );

	if (bSuccess)
		_tprintf(_TEXT("%s removed\n"), m_service); 
	else
		ErrorPrinter( _TEXT("DeleteService") );

cleanup:
	if (hService) CloseServiceHandle( hService );
	if (hSCM) CloseServiceHandle( hSCM );
	
	return;
}

BOOL CServiceInstall::IsInstalled( )
{
    BOOL bRes = FALSE;

    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);

    if (hSCM != NULL)
	{
        SC_HANDLE hService = ::OpenService(hSCM, m_service, SERVICE_QUERY_CONFIG);
        if (hService != NULL)
		{
            bRes = TRUE;
            ::CloseServiceHandle(hService);
        }
        ::CloseServiceHandle(hSCM);
    }
    return bRes;
}

DWORD CServiceInstall::ErrorPrinter( const TCHAR* psz, DWORD dwErr )
{
	LPVOID lpvMsgBuf;
    if ( !FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
		0, dwErr, 
		MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		(LPTSTR) &lpvMsgBuf, 0, 0 ) )
	{
		_tprintf( _TEXT("%s failed: Unknown error %x\n"), psz, dwErr );
	}
	else {
		_tprintf( _TEXT("%s failed: %s\n"), psz, (LPTSTR)lpvMsgBuf );
	}

	LocalFree( lpvMsgBuf );
	return dwErr;
}
