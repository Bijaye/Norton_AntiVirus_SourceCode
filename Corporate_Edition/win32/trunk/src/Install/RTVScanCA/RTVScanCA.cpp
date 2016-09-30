// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006 Symantec Corporation. All rights reserved.

// RTVScanCA.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "RTVScanCA.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

//////////////////////////////////////////////////////////////////////////
//
// Function: MSILogMessage()
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//		TCHAR [] - Text to log
//
// Returns:
//		NONE
//
// Description:  
//	This function logs messages to the MSI log file
//
//////////////////////////////////////////////////////////////////////////
// 8/07/01 - SKENNED function created.
// 11/14/03 - Thomas Brock: Added CString variation.
// 2/24/05 - Added std::string overload
// 7/20/06 - Copied from SAVMainCA, removed CString usage
//////////////////////////////////////////////////////////////////////////
UINT MSILogMessage( MSIHANDLE hInstall, TCHAR* szString )
{
	UINT uiRetVal = ERROR_SUCCESS;

	PMSIHANDLE hRec = MsiCreateRecord( 1 );

	if( hRec )
	{
		MsiRecordClearData( hRec );
		MsiRecordSetString( hRec, 0, szString );
		MsiProcessMessage( hInstall, INSTALLMESSAGE_INFO, hRec );
	}

	OutputDebugString( szString );

	return uiRetVal;
}


//////////////////////////////////////////////////////////////////////////
//
// Function: StartRTVScan
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		UINT - ERROR_SUCCESS always.
//
// Description:
//		This custom action will start RTVScan.
//
//////////////////////////////////////////////////////////////////////////
// 07/20/06 - Thomas Brock - function created.
//////////////////////////////////////////////////////////////////////////
UINT __stdcall StartRTVScan( MSIHANDLE hInstall )
{
	SC_HANDLE scManager, scService;

	scManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	if( scManager )
	{
		scService = OpenService( scManager, _T("Symantec AntiVirus"), SERVICE_ALL_ACCESS );
		if( scService )
		{
			if( StartService( scService, NULL, NULL ) )
			{
				MSILogMessage( hInstall, LOG_PREFIX _T("Started Symantec AntiVirus.") );
			}
			else
			{
				MSILogMessage( hInstall, LOG_PREFIX _T("Could not start Symantec AntiVirus.") );
			}
			CloseServiceHandle( scService );
		}
		CloseServiceHandle( scManager );
	}

	return ERROR_SUCCESS;
}
