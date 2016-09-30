// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
///////////////////////////////////////////////////////////////////////////////
//
// NavLogon.cpp : 
//
//   Defines the entry point for the DLL application.
//
//   NavLogon.dll is a WinLogon Notification Package. 
//
//   For more info see:
//
//   http://www.msdn.microsoft.com/library/default.asp?URL=/library/psdk/logauth/notify_4x4j.htm
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <assert.h>
#include <cstdlib>
#include "NavLogon.h"
#include "npapi.h"
#include "vpcommon.h"
#include "clientreg.h"
#include "tchar.h"

///////////////////////////////////////////////////////////////////////////////
// Local defines
///////////////////////////////////////////////////////////////////////////////
#define WIN_STATION_ZERO    _T("WINSTA0")


///////////////////////////////////////////////////////////////////////////////
// Local function prototypes
///////////////////////////////////////////////////////////////////////////////
BOOL IsWinStation0(LPWSTR lpWinStationName);


///////////////////////////////////////////////////////////////////////////////
//
// Function name:   DllMain
//
// Description:     When a process is attaching, open NAVs main reg key
//                  and delete the RebootStatus reg key. If this code is
//                  being executed, then a reboot has been done. We're 
//                  the first NAV-related code to start up, so we get to 
//                  remove it.
//
// Return type:     Always return TRUE
//
///////////////////////////////////////////////////////////////////////////////
// 7/12/00: TCashin Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    HKEY    hKey = NULL;
    LONG    lRet = ERROR_SUCCESS;


    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:

            DisableThreadLibraryCalls( (HINSTANCE)hModule );

            //
            // Check for the RebootStatus reg key. Delete it if it's there.
            //

            lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE, szReg_Key_Main, 0, KEY_ALL_ACCESS, &hKey );

            if( ERROR_SUCCESS == lRet)
            {
                RegDeleteValue( hKey, _T("RebootStatus") );
                RegCloseKey( hKey );
            }

            break;

		case DLL_PROCESS_DETACH:

			break;
    }
    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name:   NavStartShellEvent
//
// Description:     Event handler for the Winlogon StartShell event.
//                  We handle this event for WinSta0 only.
//
// Return type:     DWORD   Returns ERROR_SUCCESS if handle is successfully
//                          closed.
//
///////////////////////////////////////////////////////////////////////////////
// 7/12/00: TCashin Function created / header added 
///////////////////////////////////////////////////////////////////////////////
NAVLOGON_API VOID NavStartShellEvent (PWLX_NOTIFICATION_INFO pInfo)
{
    HKEY    hKey = NULL;
    LONG    lRet = ERROR_SUCCESS;
	LPTSTR  lpDomain = NULL;


    //
    // Is WinStation 0 starting a shell?
    //
    if ( IsWinStation0(pInfo->WindowStation) )
    {
        //
        // Yep. Open our StartShell event
        //
        HANDLE hEvent = OpenEvent (EVENT_MODIFY_STATE, FALSE, NAV_STARTSHELL_EVENT);

        if ( hEvent )
        {   //
            // Houston, we have a handle. Pulse the event.
            //
            PulseEvent (hEvent);
            CloseHandle (hEvent);
        }

		//
		// Save users login domain
		//
		if(pInfo->Domain)
		{
			// Open the NavLogon key.  This key is created by SAV install.
			lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE, szReg_Key_NavLogon, 0, KEY_ALL_ACCESS, &hKey );

			if( ERROR_SUCCESS == lRet)	// if key opened successfully
			{
				// Character conversion.  Use this instead of ATL conversion macros as this
				// causes some problems on 64-bit build.
				char domainA[LOGIN_DOMAIN_SIZE + 1] = {0};
				DWORD size = (DWORD)wcstombs(domainA, pInfo->Domain, LOGIN_DOMAIN_SIZE);
				if(size > 0  && size <= LOGIN_DOMAIN_SIZE)
				{
					++size;				// size must include NULL termination

					// Save the logon domain as a REG_SZ value under the key.
					RegSetValueEx( hKey, szReg_Val_LoginDomain, 0, REG_SZ, (const BYTE*) domainA, size );
				}

				RegCloseKey( hKey );	// close NavLogon key
			}
		}
    }
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name:   NavLogoffEvent
//
// Description:     Event handler for the Winlogon Logoff event.
//                  We handle this event for WinSta0 only.
//
// Return type:     DWORD   Returns ERROR_SUCCESS if handle is successfully
//                          closed.
//
///////////////////////////////////////////////////////////////////////////////
// 7/12/00: TCashin Function created / header added 
///////////////////////////////////////////////////////////////////////////////
NAVLOGON_API VOID NavLogoffEvent (PWLX_NOTIFICATION_INFO pInfo)
{
    HKEY    hKey = NULL;
    LONG    lRet = ERROR_SUCCESS;
	BYTE    byNullDomain[] = _T("");

    //
    // Is WinStation 0 logging off?
    //
    if ( IsWinStation0(pInfo->WindowStation) )
    {
        HANDLE hEvent = OpenEvent (EVENT_MODIFY_STATE, FALSE, NAV_LOGOFF_EVENT);

        if ( hEvent )
        {
            PulseEvent (hEvent);
            CloseHandle (hEvent);
        }

		//
		// Clear the login domain
		// 
        lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE, szReg_Key_NavLogon, 0, KEY_ALL_ACCESS, &hKey );

        if( ERROR_SUCCESS == lRet)
        {
            RegSetValueEx( hKey, szReg_Val_LoginDomain, 0, REG_SZ, byNullDomain, 1 );
            RegCloseKey( hKey );
        }

    }

}


///////////////////////////////////////////////////////////////////////////////
//
// Function name:   NPGetCaps
//
// Description:     
//
// Return type:     DWORD
//
///////////////////////////////////////////////////////////////////////////////
// 7/12/00: TCashin Function created / header added 
///////////////////////////////////////////////////////////////////////////////
DWORD NPAPI NPGetCaps(DWORD nIndex)
{
    DWORD dwRes;

    // This entry point is called to query the provider. The nIndex
    // parameter specifies what information is being queried. For
    // credential managers, only the following index values apply.

    switch (nIndex) {
        
        
        case WNNC_NET_TYPE:
            
            // Return network type in hiword, subtype in loword.
            
            dwRes = 0xffff0000; // credential manager
            break;

        case WNNC_SPEC_VERSION:
            dwRes = WNNC_SPEC_VERSION51;  // Provider spec version
            break;

        case WNNC_DRIVER_VERSION:
            dwRes = 1;  // Version number of this provider
            break;


        case WNNC_START:
            
            // Return 0 if the provider won't start, 1 if provider
            // is already started, a value (in milliseconds) of 
            // how long we estimate it will take for this provider
            // to start, or 0xFFFFFFF to indicate start time is 
            // unkown.
            
            dwRes = 1;  // We are already "started"
            break;

        default:
            dwRes = 0;  // zero means we don't support this query
            break;
    }

    return dwRes;

}


///////////////////////////////////////////////////////////////////////////////
//
// Function name:   NPLogonNotify
//
// Description:     
//
// Return type:     DWORD
//
///////////////////////////////////////////////////////////////////////////////
// 7/14/00: TCashin Function created / header added 
///////////////////////////////////////////////////////////////////////////////
DWORD NPAPI NPLogonNotify(PLUID lpLogonId, 
        LPCWSTR lpAuthentInfoType, LPVOID lpAuthentInfo,
        LPCWSTR lpPreviousAuthentInfoType, 
        LPVOID lpPreviousAuthentInfo, LPWSTR lpStationName,
        LPVOID StationHandle, LPWSTR  *lpLogonScript)
{
    // We don't have a logon script or app to run
    *lpLogonScript = NULL;

    // Is this guy sitting at the physical machine?
    if ( IsWinStation0(lpStationName) )
    {
        // Yep. Let RTVScan know about it. This isn't really a 
        // startshell event, but it will do.

        HANDLE hEvent = OpenEvent (EVENT_MODIFY_STATE, FALSE, NAV_STARTSHELL_EVENT);

        if ( hEvent )
        {
            //
            // Houston, we have a handle. Pulse the event.
            //
            PulseEvent (hEvent);
            CloseHandle (hEvent);
        }
    }

    return NO_ERROR;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name:   NPPasswordChangeNotify
//
// Description:     
//
// Return type:     DWORD
//
///////////////////////////////////////////////////////////////////////////////
// 7/14/00: TCashin Function created / header added 
///////////////////////////////////////////////////////////////////////////////
DWORD NPAPI NPPasswordChangeNotify(LPCWSTR lpAuthentInfoType,
    LPVOID lpAuthentInfo, LPCWSTR lpPreviousAuthentInfoType,
    LPVOID lpPreviousAuthentInfo, LPWSTR lpStationName,
    LPVOID StationHandle, DWORD dwChangeInfo)
{
    // Called to notify a credential manager provider of a password 
    // (authentication information) change for an account.
    
    return NO_ERROR;
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name:   IsWinStation0
//
// Description:     Is the windows station for this notification the
//                  actual, physical machine?
//
// Return type:     BOOL    True if lpWinStationName is winsta0
//
///////////////////////////////////////////////////////////////////////////////
// 7/12/00: TCashin Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL IsWinStation0(LPWSTR lpWinStationName)
{
    BOOL    bIsWinStation0 = FALSE;
    TCHAR   szWinStation[MAX_PATH] = {0};

    //
    // Is there a station name?
    //

    if ( lpWinStationName )
    {
        //
        // Convert Unicode WindowStation to ANSI
        //
        WideCharToMultiByte ( CP_ACP,
                              0,
                              lpWinStationName,
                              -1,
                              szWinStation,
                              sizeof(szWinStation),
                              NULL,
                              NULL );

        //
        // Is this station Winsta0, that is, the physical machine itself?
        //
        if ( _tcscmp(WIN_STATION_ZERO, _tcsupr(szWinStation)) == 0 )
        {
            //
            // Yep
            //
            bIsWinStation0 = TRUE;
        }
    }

    return bIsWinStation0;

}

