// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// FindMom.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Application_SAVCE_Data.h"
#include <crtdbg.h>
#include "SymSaferRegistry.h"

#define ARRAY_SIZEOF(x) ((sizeof(x))/(sizeof(x[0])))


//
// COM GUIDs
//

CLSID CLSID_CliProxy  = { 0xe381f1c0, 0x910e, 0x11d1, { 0xab, 0x1e, 0x0, 0xa0, 0xc9, 0xf, 0x8f, 0x6f } };
CLSID CLSID_Transman  = { 0xe381f1e0, 0x910e, 0x11d1, { 0xab, 0x1e, 0x0, 0xa0, 0xc9, 0xf, 0x8f, 0x6f } };

IID IID_IVirusProtect = { 0xe381f1df, 0x910e, 0x11d1, { 0xab, 0x1e, 0x0, 0xa0, 0xc9, 0xf, 0x8f, 0x6f } };
IID IID_IConsoleMisc  = { 0xe381f1e8, 0x910e, 0x11d1, { 0xab, 0x1e, 0x0, 0xa0, 0xc9, 0xf, 0x8f, 0x6f } };


// The handle returned from an InitTransman call.

DWORD	g_TransHan = 0;

// The event used to signal the main thread when a parent server responds.

HANDLE	g_Event = NULL;

// The buffer used to store the name of the first parent server to respond.

TCHAR	g_Mom[MAX_PATH] = {0};
BOOL	g_MomFound = FALSE;

// The critical section used to synchronize callbacks from Transman.

CRITICAL_SECTION g_Sentry = {0};


// Virus notification callback.

extern "C" DWORD VirusAlertCallback( void* event )
{
	return( 0 );
}

// Server replied to PONG callback.

extern "C" DWORD FoundServerPong( char* server, char* domain, char* mom, void* pong )
{
	EnterCriticalSection( &g_Sentry );

	// Make sure the server name is valid.

	if( server != NULL && _tcslen( server ) > 0 )
	{
		// Fix up params.

		if( domain == NULL )
		{
			domain = "";
		}

		// Tell the user that we got a response.

		if( mom != NULL && _tcslen( mom ) > 0 )
		{
			_tprintf( _T("FSCSServe: Response from secondary server: %s [domain: %s, primary: %s]\n"), server, domain, mom );
		}
		else
		{
			_tprintf( _T("FSCSServe: Response from primary server: %s [domain: %s]\n"), server, domain );
		}

		// If this is the first response, save the name of this parent and signal the main thread.

		if( g_MomFound == FALSE )
		{
			_tcsncpy( g_Mom, server, ARRAY_SIZEOF( g_Mom ) );
			g_Mom[ ARRAY_SIZEOF( g_Mom ) - 1 ] = 0;

			g_MomFound = TRUE;

			SetEvent( g_Event );
		}
	}
	else
	{
		_tprintf( _T("FSCSServe: Ignoring null response.\n") );
	}

	LeaveCriticalSection( &g_Sentry );

    return( 0 );
}

// Fire up the SAV comm layer.

bool TransmanStart()
{
	HMODULE transman = NULL;
	CVTCBA2GRC CvtCBA2GRC = NULL;
	INITTRANSMAN InitTransman = NULL;

	// Initialize Transman if it has not already been done.

	if( g_TransHan == 0 )
	{
		// Get the CvtCBA2GRC and InitTransman functions from Transman.dll.

		transman = GetModuleHandle( _T(TRANSMAN_DLL_NAME) );

		if( transman != NULL )
		{
			CvtCBA2GRC =
					reinterpret_cast<CVTCBA2GRC>( GetProcAddress( transman, (LPCSTR)ORDINAL_CVTCBA2GRC ) );
			InitTransman =
					reinterpret_cast<INITTRANSMAN>( GetProcAddress( transman, (LPCSTR)ORDINAL_INITTRANSMAN ) );

			if( CvtCBA2GRC != NULL && InitTransman != NULL )
			{
				CvtCBA2GRC( 0 ); // This is obfuscation, it doesn't really do anything.

				g_TransHan = InitTransman( &VirusAlertCallback, &FoundServerPong );
			}
		}
	}

	return( g_TransHan != 0 );
}

// Tear down the SAV comm layer.

void TransmanStop()
{
	HMODULE transman = NULL;
	DEINITTRANSMAN DeInitTransman = NULL;

	// Unload Transman if it has not already been done.

	if( g_TransHan != 0 )
	{
		// Get the DeInitTransman function from Transman.dll.

		transman = GetModuleHandle( _T(TRANSMAN_DLL_NAME) );

		if( transman != NULL )
		{
			DeInitTransman =
					reinterpret_cast<DEINITTRANSMAN>( GetProcAddress( transman, (LPCSTR)ORDINAL_DEINITTRANSMAN ) );

			if( DeInitTransman != NULL )
			{
				DeInitTransman( g_TransHan );
			}
		}

		g_TransHan = 0;
	}
}

// Make the local SAV/SCS client check in with the new parent server that we assigned it to.

void ForceCheckIn( IVirusProtect* vp, HKEY sav_key )
{
	CBA_Addr local_host_addr = {0};
	HRESULT hr = E_FAIL;
	DWORD port = 0;
	DWORD size = sizeof(port);
	IConsoleMisc* con_misc = NULL;
	DWORD err = 0;

	// Create a CBA localhost address so we can talk to SAV installed on this machine.

	local_host_addr.addrSize = sizeof(local_host_addr);
	local_host_addr.ucProtocol = CBA_PROTOCOL_IP;
	local_host_addr.dstAddr.netAddr.ipAddr = 0x0100007f; // localhost: 127 0 0 1
	local_host_addr.dstAddr.usPort = 0x970b; // 2967 in network order -- the default

	// Get the SAV port from the local registry.

	if( SymSaferRegQueryValueEx( sav_key,
						 _T(szReg_Val_AgentIPPort),
						 NULL,
						 NULL,
						 reinterpret_cast<LPBYTE>(&port),
						 &size ) == ERROR_SUCCESS )
	{
		// Convert to unsigned short, network order.

		((BYTE*)&local_host_addr.dstAddr.usPort)[0] = ((BYTE*)&port)[1];
		((BYTE*)&local_host_addr.dstAddr.usPort)[1] = ((BYTE*)&port)[0];
	}

	// Get an IConsoleMisc interface and tell the local Rtvscan to pong its parent.

	hr = vp->CreateByIID( IID_IConsoleMisc, reinterpret_cast<void**>(&con_misc) );

	if( SUCCEEDED(hr) )
	{
		// Create a CBA Addr struct that represents localhost.

		err = con_misc->SendControlMessage( reinterpret_cast<char*>(&local_host_addr), SERVER_SERVICE_RELOAD_PONG_DATA );

		if( err == 0 )
		{
			_tprintf( _T("FSCSServe: Check in with new parent [%s] succeeded.\n"), g_Mom );
		}
		else
		{
			_tprintf( _T("FSCSServe: Check in with new parent [%s] failed (0x%08x).  Wait for regular check in.\n"), g_Mom, err );
		}

		con_misc->Release();
	}
	else
	{
		_tprintf( _T("FSCSServe: Check in with new parent [%s] failed to init (0x%08x).  Wait for regular check in.\n"), g_Mom, hr );
	}
}

int main( int argc, char* argv[] )
{
	int ret = 0;
	DWORD err = 0;
	HRESULT hr = E_FAIL;
	IVirusProtect* vp = NULL;
	HKEY sav_key = 0;
	HMODULE transman = NULL;
	QUICKFINDALLLOCALCOMPUTERS QuickFindAllLocalComputers = NULL;

	DWORD n = 0;

	// In debug builds, check for leaks.
#ifdef _DEBUG
	_CrtSetDbgFlag( _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG ) | _CRTDBG_LEAK_CHECK_DF );
#endif // _DEBUG

	// Transman exports IConsoleMisc::SendControlMessage and QuickFindAllLocalComputers,
	// which are the APIs we will use through to get our job done.
	
	// We're using COM to find and load Transman.dll rather than just loading it directly
	// because COM has a more consistent way to locate Transman than our various versions
	// and platforms.


	// Open the main SAV key.

	err = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
						_T(szReg_Key_Main),
						0,
						KEY_SET_VALUE | KEY_QUERY_VALUE | KEY_CREATE_SUB_KEY,
						&sav_key );

	if( err != ERROR_SUCCESS )
	{
		_tprintf( _T("FSCSServe: FAIL [-4]: SAV/SCS is not installed.\n") );

		return( -4 );
	}

// Fire up COM.

	hr = CoInitializeEx( NULL, COINIT_MULTITHREADED );

	if( SUCCEEDED(hr) )
	{
		_tprintf( _T("FSCSServe: COM initialized.\n") );
	}
	else
	{
		_tprintf( _T("FSCSServe: FAIL [-1]: COM initialization failed: 0x%08x.\n"), hr );

		return( -1 );
	}

	// Create the signalling event for the callbacks to use.

	g_Event = CreateEvent( NULL, TRUE, FALSE, NULL );

	// Create the callback synchronization critical section.

	InitializeCriticalSection( &g_Sentry );

	// Get the base interface that Transman exposes -- this will load the Transman DLL.

	hr = CoCreateInstance( CLSID_Transman,
						   NULL,
						   CLSCTX_INPROC_SERVER,
						   IID_IVirusProtect,
						   reinterpret_cast<void**>(&vp) );

	// We should have Transman loaded now.

	if( TransmanStart() )
	{
		_tprintf( _T("FSCSServe: Transman initialized: handle = 0x%08x.\n"), g_TransHan );

		// Get the QuickFindAllLocalComputers function.

		transman = GetModuleHandle( _T(TRANSMAN_DLL_NAME) );

		if( transman != NULL )
		{
			QuickFindAllLocalComputers =
					reinterpret_cast<QUICKFINDALLLOCALCOMPUTERS>( GetProcAddress( transman, (LPCSTR)ORDINAL_QUICKFIND ) );

			if( QuickFindAllLocalComputers != NULL )
			{
				_tprintf( _T("FSCSServe: Pinging local subnet ...\n") );

				QuickFindAllLocalComputers();

				_tprintf( _T("FSCSServe: Pinging local subnet complete.\n") );

				// In our callback functions, we will signal event on the first response.
				// Wait on that event.

				err = WaitForSingleObject( g_Event, 10000 );

				// If someone responded, write their info into the local SAV client's parent key.

				EnterCriticalSection( &g_Sentry );

				if( g_MomFound )
				{
					err = RegSetValueEx( sav_key,
										 _T(szReg_Val_Parent),
										 0,
										 REG_SZ,
										 reinterpret_cast<CONST BYTE*>(g_Mom),
										 _tcslen(g_Mom) * sizeof(TCHAR) );

					if( err == ERROR_SUCCESS )
					{
						_tprintf( _T("FSCSServe: Parent set to %s.\n"), g_Mom );
					}
					else
					{
						_tprintf( _T("FSCSServe: FAIL [-6]: Failed to set the Parent registry value (0x%08X).\n"), err );

						ret = -6;
					}
				}
				else
				{
					// Nobody responded.

					_tprintf( _T("FSCSServe: FAIL [-5]: No parent server found.\n") );

					ret = -5;
				}

				LeaveCriticalSection( &g_Sentry );
			}
			else
			{
				_tprintf( _T("FSCSServe: FAIL [-2]: Transman doesn't support the discovery type we need.\n") );

				ret = -2;
			}
		}

		if( ret == 0 )
		{
			ForceCheckIn( vp, sav_key );
		}

		TransmanStop();
	}
	else
	{
		_tprintf( _T("FSCSServe: FAIL [-3]: Transman failed to initialize.\n") );

		ret = -3;
	}

	// Cleanup.

	if( vp != NULL )
	{
		vp->Release();
	}

	CoUninitialize();

	CloseHandle( g_Event );

	DeleteCriticalSection( &g_Sentry );

	RegCloseKey( sav_key );

	return( ret );
}

/*
// *WORK* Enhancement notes:

  Report the ip or ipx address of the new parent (just read it from the AddressCache)
  Add command line params to specify:
	the number of seconds to wait for a response in the main thread
	subnets other than the local subnet
	quiet mode
	log file
*/

