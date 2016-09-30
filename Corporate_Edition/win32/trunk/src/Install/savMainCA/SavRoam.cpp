// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// SavRoam.cpp : Defines the routines for the SavRoam service.
//

#include "stdafx.h"
#include "savMainCA.h"

//////////////////////////////////////////////////////////////////////////
//
// Function: SetSavRoamStartup
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		UINT - ERROR_SUCCESS always.
//
// Description:
//		SavRoam will be set to a Manual startup type by the
//		installer.  If the user is installing a managed client,
//		then this custom action will set the startup to Automatic.
//
//////////////////////////////////////////////////////////////////////////
// 01/18/03 - Thomas Brock - function created.
//////////////////////////////////////////////////////////////////////////
UINT __stdcall SetSavRoamStartup( MSIHANDLE hInstall )
{
	SC_HANDLE scManager, scService;

	scManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	if( scManager )
	{
		scService = OpenService( scManager, _T("SAVRoam"), SERVICE_ALL_ACCESS );
		if( scService )
		{
			if( ChangeServiceConfig( scService, SERVICE_NO_CHANGE, SERVICE_AUTO_START, SERVICE_NO_CHANGE, NULL, NULL, NULL, NULL, NULL, NULL, NULL ) )
			{
				MSILogMessage( hInstall, _T("Set SAVRoam service to automatic start.") );
			}
			else
			{
				MSILogMessage( hInstall, _T("Could not set SAVRoam service to automatic start.") );
			}
		}
		CloseServiceHandle( scManager );
	}

	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: StartSavRoam
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		UINT - ERROR_SUCCESS always.
//
// Description:
//		This custom action will start SAVRoam on NT class boxes.
//
//////////////////////////////////////////////////////////////////////////
// 02/09/04 - Thomas Brock - function created.
//////////////////////////////////////////////////////////////////////////
UINT __stdcall StartSavRoam( MSIHANDLE hInstall )
{
	SC_HANDLE scManager, scService;

	scManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	if( scManager )
	{
		scService = OpenService( scManager, _T("SAVRoam"), SERVICE_ALL_ACCESS );
		if( scService )
		{
			if( StartService( scService, NULL, NULL ) )
			{
				MSILogMessage( hInstall, _T("Started SAVRoam.") );
			}
			else
			{
				MSILogMessage( hInstall, _T("Could not start SAVRoam.") );
			}
			CloseServiceHandle( scService );
		}
		CloseServiceHandle( scManager );
	}

	return ERROR_SUCCESS;
}
