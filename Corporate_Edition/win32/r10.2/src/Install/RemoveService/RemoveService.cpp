// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// RemoveService.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "windows.h"
#include "msi.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}



//---------------------------------------------------------------------------------
//
//	Routine: RemoveService
//
//	Purpose:	Removes a Service from the SCM
//
//	Parameters:	LPSTR sInternalServiceName	(Internal Service Name)
//
//---------------------------------------------------------------------------------
// 07/26/02 Joe Antonelli Created.
//---------------------------------------------------------------------------------
void RemoveService( LPSTR sInternalServiceName, BOOL StopService = FALSE )
{
	// Open a handle to the SC Manager database.
	SC_HANDLE hSCManager = OpenSCManager( NULL,						// local machine
	                                      NULL,						// ServicesActive database
	                                      SC_MANAGER_ALL_ACCESS);	// full access rights
	if( hSCManager )
	{
		SC_HANDLE hService = OpenService( hSCManager,				// handle to SCM database
		                                  sInternalServiceName,		// service name
		                                  SERVICE_ALL_ACCESS );		// access

		if( hService )
		{
			if( StopService == TRUE ){
				SERVICE_STATUS tServiceStatus;
				ControlService(hService, SERVICE_CONTROL_STOP, &tServiceStatus);
			}

			DeleteService( hService );

			CloseServiceHandle( hService );
		}

		CloseServiceHandle( hSCManager );
	}
}

//---------------------------------------------------------------------------------
//
//	Routine: MsiRemoveServicesFromSCM
//	Purpose: Make sure the services are removed properly from the SCM on uninstall
//
//---------------------------------------------------------------------------------
// 07/26/02 Joe Antonelli Created.
//---------------------------------------------------------------------------------
extern "C" __declspec(dllexport) UINT __stdcall MsiRemoveServiceFromSCM(MSIHANDLE hInstall)
{

	RemoveService("SAVRT", TRUE);
	RemoveService("SAVRTPEL", TRUE);
	RemoveService("NAVENG");
	RemoveService("NAVEX15");

	return ERROR_SUCCESS;
}
