// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006 Symantec Corporation. All rights reserved.
//
// RtvStart: Application to restart RTVScan service.
//
//           Currently launched from RTVScan to restart service when
//           service needs to shutdown and restart to load new AP interfaces
//           in response to (non-SAV) upgrade of AP.

#include "stdafx.h"
#include "RtvStart.h"
#include "vpcommon.h"


// Result codes for WaitForServiceShutdown()
enum ServiceShutdownResult
{
    SS_STOPPED = 0,     // Service stopped.
    SS_RESTARTED,       // Service switched to stopping state, then restarted.
    SS_TIMEOUT,         // Wait operation timed out.
    SS_ERROR            // Error retrieving service status.
};

// WaitForServiceShutdown(): Wait for a service to transition to SERVICE_STOPPED state.
enum ServiceShutdownResult WaitForServiceShutdown( SC_HANDLE hService, const DWORD dwMaxWaitMs );

// RestartRtvScan(): Wait for RtvScan to shutdown and then restart it.
DWORD   RestartRtvScan();


//
// WinMain
//

int APIENTRY _tWinMain( HINSTANCE   hInstance,
                        HINSTANCE   hPrevInstance,
                        LPTSTR      lpCmdLine,
                        int         nCmdShow )
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(nCmdShow);

    auto    int         nRet = 0;

    if ( _tcsstr( lpCmdLine, _T("-restart") ) )
    {
        nRet = RestartRtvScan();
    }

    return ( nRet );
}


// DWORD RestartRtvScan()
//
// Wait for RtvScan to enter stopped state and restart service.
//
// returns:
//      0 on success, otherwise non-zero.

DWORD   RestartRtvScan()
{
    auto    SC_HANDLE   hServiceControl = NULL;
    auto    SC_HANDLE   hRtvScan = NULL;
    auto    DWORD       dwRet = ERROR_GEN_FAILURE;

    hServiceControl = OpenSCManager( NULL, NULL, SC_MANAGER_CONNECT );

    if( hServiceControl )
    {
        hRtvScan = OpenService( hServiceControl, _T(SERVICE_NAME), SERVICE_QUERY_STATUS | SERVICE_START );

        if( hRtvScan )
        {
            switch ( WaitForServiceShutdown( hRtvScan, 3 * 60 * 1000 ) )
            {
            case SS_STOPPED:
                // Service stopped, restart it.
                if( StartService( hRtvScan, NULL, NULL ) )
                {
                    dwRet = 0;
                }
                else
                {
                    dwRet = GetLastError();
                }
                break;

            case SS_RESTARTED:
                // Service restarted by someone else. Ok.
                dwRet = 0;
                break;

            case SS_TIMEOUT:
                dwRet = WAIT_TIMEOUT;
                break;

            case SS_ERROR:
            default:
                break;
            }

            CloseServiceHandle( hRtvScan );
        }
        else
        {
            dwRet = GetLastError();
        }

        CloseServiceHandle( hServiceControl );
    }
    else
    {
        dwRet = GetLastError();
    }

    return ( dwRet );
}


// enum ServiceShutdownResult WaitForServiceShutdown( SC_HANDLE hService, const DWORD dwMaxWaitMs )
//
// Wait for a service to transition to SERVICE_STOPPED state.
//
// returns:
//      SS_STOPPED      Service stopped.
//      SS_RESTARTED    Service switched to stopping state, then restarted.
//      SS_TIMEOUT      Wait operation timed out.
//      SS_ERROR        Error retrieving service status.

enum ServiceShutdownResult WaitForServiceShutdown( SC_HANDLE hService, const DWORD dwMaxWaitMs )
{
    const   DWORD   SLEEP_TIME        = 500;
    const   DWORD   MAX_LOOPS         = dwMaxWaitMs / SLEEP_TIME;

    SERVICE_STATUS  serviceStatus;
    BOOL            bQueryResult = 0;
    DWORD           dwLoops = 0;
    bool            bServiceWasStopping = false;
    bool            bDone = false;

    enum ServiceShutdownResult nResult = SS_ERROR;

    serviceStatus.dwCurrentState = SERVICE_RUNNING;

    // Wait for service to enter STOPPED state,
    // or to transition to STOP_PENDING and back to RUNNING.

    while ( !bDone && 
            ( 0 != ( bQueryResult = QueryServiceStatus( hService, &serviceStatus ) ) ) &&
            ( dwLoops <=  MAX_LOOPS ) )
    {
        switch( serviceStatus.dwCurrentState )
        {
        case SERVICE_STOP_PENDING:
            // Track the fact that service was stopping, in case someone besides
            // us restarts it.
            bServiceWasStopping = true;
            break;

        case SERVICE_STOPPED:
            bDone = true;
            break;

        case SERVICE_START_PENDING:
            // If someone besides us restarted the service, we're done.
            bDone = true;
            break;

        case SERVICE_RUNNING:
            // If someone besides us restarted the service, we're done.
            if ( bServiceWasStopping )
                bDone = true;
            break;

        case SERVICE_CONTINUE_PENDING:
        case SERVICE_PAUSE_PENDING:
        case SERVICE_PAUSED:
        default:
            // Don't really care about transitions to these states
            break;
        }

        ++dwLoops;
        Sleep( SLEEP_TIME );
    }

    if ( bDone )
    {
        // bDone indicates we exited based on change in service state.
        nResult = ( SERVICE_STOPPED == serviceStatus.dwCurrentState ? SS_STOPPED : SS_RESTARTED );
    }
    else
    {
        // Otherwise, either query error or timeout.
        nResult = ( bQueryResult ? SS_TIMEOUT : SS_ERROR );
    }

    return ( nResult );
}

