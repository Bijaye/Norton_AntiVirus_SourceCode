// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
///////////////////////////////////////////////////////////////////////////
//
// DefWatchService.cpp: implementation of the CDefWatchService class.
//
//
//
//
///////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "defwatch.h"
#include "DefWatchService.h"
#include "DefUpdateEvent.h"

#include "savidefutilsloader.h"
#include "defutilsinterface.h"
#include "SymSaferRegistry.h"
#include "ccLibDllLink.h"

#include "vpstrutils.h"

#define INITGUID
#include "idefevnt.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// Function name : CDefWatchService::CDefWatchService
//
// Description      :
//
// Return type      :
//
//
///////////////////////////////////////////////////////////////////////////////
// 10/10/98 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
CDefWatchService::CDefWatchService()
{
    //
    // Setup service status object
    //
    ZeroMemory( &m_ServiceStatus, sizeof( SERVICE_STATUS ) );

    m_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS | SERVICE_WIN32;
    m_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    m_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    m_ServiceStatus.dwWin32ExitCode = NO_ERROR;

    //
    // Set up default values
    //
    m_ServiceStatusHandle = 0;
    m_hWatchThread = NULL;
    m_hThreadSignal = NULL;
    m_dwThreadID = 0;
    m_pListHead = NULL;
    m_hServiceEvent = NULL;
    m_hDefUpdateSignal = NULL;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CDefWatchService::~CDefWatchService
//
// Description: Destructor
//
///////////////////////////////////////////////////////////////////////////////
// 11/7/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CDefWatchService::~CDefWatchService()
{
    //
    // Cleanup
    //
    PEVENTHANDLER p = m_pListHead;
    while( m_pListHead )
        {
        p = m_pListHead;
        m_pListHead = m_pListHead->next;
        delete p;
        }

    if( m_hThreadSignal )
        {
        CloseHandle( m_hThreadSignal );
        m_hThreadSignal = NULL;
        }

    if( m_hServiceEvent )
        {
        CloseHandle( m_hServiceEvent );
        m_hServiceEvent = NULL;
        }

    if ( m_hDefUpdateSignal )
        {
        m_cDefUpdateEvent.CloseHandle();
        m_hDefUpdateSignal = NULL;
        }
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CDefWatchService::StartService
//
// Description: This routine performs startup chores for the service
//
// Return type: BOOL 
//
//
///////////////////////////////////////////////////////////////////////////////
// 11/7/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CDefWatchService::StartService()
{
    BOOL bRet = TRUE;

    //
    // Tell SCM we are starting.
    //
    SetServiceStatus( SERVICE_START_PENDING, 1000);

    //
    // Create our signal event.
    //
    m_hServiceEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
    if( m_hServiceEvent == NULL )
        {
        DefWatchDebugOut( _T("CreateEvent failed with error code %X"), GetLastError() );
        return FALSE;
        }

    // 
    //  Create the def update event
    // 
    m_hDefUpdateSignal = m_cDefUpdateEvent.GetHandle();
    if( m_hDefUpdateSignal == NULL )
        {
        DefWatchDebugOut( _T("Failed to create the definition update event with error code %X."), GetLastError() );
        DefWatchLogEvent( IDM_ERR_FAILED_TO_CREATE_DEF_UPDATE_EVENT );
        SetServiceStatus( SERVICE_STOPPED );
        return FALSE;
        }

    // 
    // Build event handler list.
    // 
    if( BuildEventHandlerList() == FALSE )
        {
        DefWatchDebugOut( _T("Failed to build handler list.") );
        DefWatchLogEvent( IDM_ERR_NO_HANDLERS, EVENTLOG_WARNING_TYPE );
        }

    //
    // Tell SCM we are starting.
    //
    SetServiceStatus( SERVICE_START_PENDING, 1000);

    // 
    // Create thread signal object.
    // 
    m_hThreadSignal = CreateEvent( NULL, FALSE, FALSE, NULL );
    if( m_hThreadSignal == NULL )
        {
        DefWatchDebugOut( _T("Failed to create thread event.") );
        SetServiceStatus( SERVICE_STOPPED );
        return FALSE;
        }

    // 
    // Start watch thread.
    // 
    m_hWatchThread = CreateThread( NULL,
                                   0,
                                   CDefWatchService::WatchThread,
                                   this,
                                   0,
                                   &m_dwThreadID );
    if( m_hWatchThread == NULL )
        {
        DefWatchDebugOut( _T("Failed to start watch thread") );
        SetServiceStatus( SERVICE_STOPPED );
        return FALSE;
        }

    //
    // Set state to running, and let SCM know about it
    //
    SetServiceStatus( SERVICE_RUNNING );

    return bRet;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CDefWatchService::StopService
//
// Description:  This routine is called when the service needs to terminate.
//
// Return type: VOID 
//
//
///////////////////////////////////////////////////////////////////////////////
// 11/7/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
VOID CDefWatchService::StopService()
{
    //
    // We are stopping.
    //
    SetServiceStatus( SERVICE_STOP_PENDING, 1000 );

    //
    // Tell thread we are shutting down.
    //
    SetEvent( m_hThreadSignal );

    // 
    // Wait here for thread to terminate.
    // 
    SetServiceStatus( SERVICE_STOP_PENDING, 2000 );
    WaitForSingleObject( m_hWatchThread, INFINITE );

    // 
    // Clean up our thread.
    // 
    CloseHandle( m_hWatchThread );    

    //
    // Close the def update event
    //
    m_cDefUpdateEvent.CloseHandle();
    m_hDefUpdateSignal = NULL;

    //
    // We are stopped.
    //
    SetServiceStatus( SERVICE_STOPPED );

}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CDefWatchService::SetServiceStatus
//
// Description: Cover version of ::SetServiceStatus
//
// Return type: BOOL 
//
// Argument         :  DWORD dwStatus
// Argument         : DWORD dwTimeOutHint /* = 0 */
//
///////////////////////////////////////////////////////////////////////////////
// 11/7/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CDefWatchService::SetServiceStatus( DWORD dwStatus, DWORD dwTimeOutHint /* = 0 */ )
{
    //
    // Inform SCM of our current status.
    //
    m_ServiceStatus.dwCurrentState = dwStatus;
    m_ServiceStatus.dwWaitHint = dwTimeOutHint;

    BOOL bRet = ::SetServiceStatus( m_ServiceStatusHandle, &m_ServiceStatus );
    if( bRet == FALSE )
        {
        DefWatchDebugOut( _T("SetServiceStatus failed with error: %X."), GetLastError());
        }

    return bRet;
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name: CDefWatchService::Register
//
// Description:  This routine registers our service control handler with 
//               the Windows NT Service Control Mgr.
//
// Return type: BOOL 
//
// Argument         : LPHANDLER_FUNCTION lpHandlerProc
//
///////////////////////////////////////////////////////////////////////////////
// 11/7/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CDefWatchService::Register( LPHANDLER_FUNCTION lpHandlerProc )
{
    //
    // Set up our handler routine.  This is the routine the
    // SCM uses to notify us of shutdown and/or pause events.
    //
    m_ServiceStatusHandle = RegisterServiceCtrlHandler( DEFWATCH_SERVICE_NAME, lpHandlerProc );

    if( m_ServiceStatusHandle == 0 )
        {
        DefWatchDebugOut( _T("RegisterServiceCtrlHandler failed with code %X"), GetLastError() );
        return FALSE;
        }

    return TRUE;
}


//*************************************************************************
// CDefWatchService::WatchThread()
//
// CDefWatchService::WatchThread(
//      LPVOID pData )
//
// Description: This is the main watch thread proc.
//
// Returns: DWORD 
//
//*************************************************************************
// 11/6/98 DBUCHES, created - header added.
//*************************************************************************

DWORD __stdcall CDefWatchService::WatchThread( LPVOID pData )
{
    DWORD   dwSignaled;
    
    // 
    // Save off this pointer
    // 
    CDefWatchService* pThis = (CDefWatchService*) pData;

    // 
    // Initialize virus def info.
    // 
    pThis->m_cDefVersion.Load( DEFWATCH_REG_KEY );
    pThis->HaveDefsChanged();

    // 
    // Setup array of handles to wait on.
    // 
    HANDLE aHandles[2];
    aHandles[0] = pThis->m_hThreadSignal;
    aHandles[1] = pThis->m_hDefUpdateSignal;

    // 
    // Loop here waiting for changes.
    // 
    do 
        {
        // 
        // Wait here for something to happen
        // 

        dwSignaled = WaitForMultipleObjects( 2, aHandles, FALSE, INFINITE );

        if ( dwSignaled == WAIT_OBJECT_0 + 1 )
            {
            // 
            // Q: Have the virus defs changed since we were here last?
            // 
            if( pThis->HaveDefsChanged() )
                {

                // there are evil race conditions between handling in dhwizard, for example,
                // and the normal virus definition processing in rtvscan - any fired event
                // should be aware of side effects that they may have that interfere with the
                // processing of the defs - in the case of dhwizard, it is doing the same thing
                // that rtvscan itself does, apply the defs

                // 
                // Fire off events
                // 
                pThis->FireEvents();
                }
            }        
        }
    while ( dwSignaled != WAIT_OBJECT_0 ); // While m_hThreadSignal is not signaled

    DefWatchDebugOut( _T("Watch thread terminating.") );

    return 0;
}


//*************************************************************************
// CDefWatchService::FireEvents()
//
// CDefWatchService::FireEvents( )
//
// Description:
//
// Returns: void 
//
//*************************************************************************
// 11/6/98 DBUCHES, created - header added.
//*************************************************************************

void CDefWatchService::FireEvents()
{
    // 
    // Make sure we have handlers to call.
    // 
    if( m_pListHead == NULL )
        {
        DefWatchDebugOut( _T("New defs found, no handlers!") );
        return;
        }


    CoInitialize( NULL );

    // 
    // Call all handlers we know about.
    //
    PDEFWATCHEVENTHANDLER pHandler;
    PEVENTHANDLER p = m_pListHead;
    while( p != NULL )
        {
        // 
        // Create an instance of this handler.
        // 
        if( SUCCEEDED( CoCreateInstance( p->clsid, 
                                     NULL, 
                                     CLSCTX_INPROC_SERVER,
                                     IID_DefWatchEventHandler,
                                     (LPVOID*)&pHandler ) ) )
            {
            // 
            // Call handler.
            // 
            pHandler->OnNewDefsInstalled();
            
            // 
            // Release this object.
            // 
            pHandler->Release();
            }
        else    
            {
            // 
            // Failure.  Should log this error.
            // 
            DefWatchDebugOut( _T("Failed to create a handler.") );
            }

        // 
        // Move to next element.
        // 
        p = p->next;
        }


    CoUninitialize();
}

//*************************************************************************
// CDefWatchService::BuildEventHandlerList()
//
// CDefWatchService::BuildEventHandlerList( )
//
// Description: This routine looks at our registry key for registered event
//              handlers.
//
// Returns: BOOL TRUE on success, FALSE on error.
//
//*************************************************************************
// 11/6/98 DBUCHES, created - header added.
//*************************************************************************

BOOL CDefWatchService::BuildEventHandlerList()
{
    BOOL bRet = TRUE;
    HKEY hKey;
    LONG lRetVal;
    TCHAR szValueName[ MAX_PATH ];
    DWORD dwValueNameLength, dwBufferSize, dwType, dwIndex = 0;
    TCHAR szCLSID[ 64 ];
    WCHAR wszCLSID[ 64 ];
    CLSID clsid;
    
    // 
    // We need to enumerate the registry looking for CLSIDs of 
    // registered event handler objects.
    // 
    if (ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, 
                                       DEFWATCH_HANDLERS_REG_KEY, 
                                       0, 
                                       KEY_READ | KEY_QUERY_VALUE, 
                                       &hKey))
        {
        do
            {
            dwValueNameLength = MAX_PATH;
            dwBufferSize = 64;
            lRetVal = RegEnumValue( hKey,
                                    dwIndex ++,
                                    szValueName,
                                    &dwValueNameLength,
                                    NULL,
                                    &dwType,
                                    (LPBYTE) szCLSID,
                                    &dwBufferSize );
            if( ERROR_SUCCESS == lRetVal && dwType == REG_SZ )
                {
                // 
                // Convert string representation to CLSID
                // 
                MultiByteToWideChar( CP_ACP,
                                     0,
                                     szCLSID,
                                     -1,
                                     wszCLSID,
                                     64 );
                
                if( NOERROR == CLSIDFromString( wszCLSID, &clsid ) ) 
                    {
                    // 
                    // Add to our list.
                    // 
                    PEVENTHANDLER p = NULL;
                    try
                    {
                        p = new CEventHandler;
                    }
                    catch (std::bad_alloc &)
                    {
                    }
                    if( p == NULL )
                        {
                        DefWatchDebugOut( _T("Error allocating memory!") );
                        bRet = FALSE;
                        break;
                        }
            
                    // 

                    // Replace list head with new element.
                    // 
                    CopyMemory( &p->clsid, &clsid, sizeof( CLSID ) );
                    p->next = m_pListHead;
                    m_pListHead = p;
                    }

                }
            }
        while( ERROR_SUCCESS == lRetVal );

        // 
        // Make sure we have at least one handler.
        // 
        if( m_pListHead == NULL )
            bRet = FALSE;

        // 
        // Cleanup
        // 
        RegCloseKey( hKey );
        }
    else
        {
        DefWatchDebugOut( _T("Error opening registry key.") );
        bRet = FALSE;
        }




    return bRet; 
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name: CDefWatchService::HaveDefsChanged
//
// Description: 
//
// Return type: BOOL    Returns TRUE if virus defs have changes since 
//                      the last call to this routine, FALSE if not.
//
//
///////////////////////////////////////////////////////////////////////////////
// 11/7/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CDefWatchService::HaveDefsChanged()
{
    CDefUtilsLoaderHelper objDefUtilsLdr;
    IDefUtils4Ptr   ptrDefUtils;
    bool            bRtn = true;

    CoInitialize(NULL);
    //
    // Get a pointer to IDefUtils
    //
    if (SYM_FAILED (objDefUtilsLdr.CreateObject (ptrDefUtils)))
    {
        DefWatchDebugOut( _T("Error getting IDefUtils pointer.") );
        return FALSE;
    }

    // 
    // Set up defutils object.
    // 
    if( FALSE == ptrDefUtils->InitWindowsApp( HAWKING_APP_ID ) )
    {
        TCHAR szFullErrMsg[256];
        vpstrncpy   (szFullErrMsg, _T("Error setting up DefUtils. "),          sizeof(szFullErrMsg));
        vpstrnappend(szFullErrMsg, ptrDefUtils->DefUtilsGetLastResultString(), sizeof(szFullErrMsg));
        DefWatchDebugOut( szFullErrMsg );
        return FALSE;
    }

    // 
    // Get def information.
    // 
    WORD wYear, wMonth, wDay;
    DWORD dwRev;
    if( FALSE == ptrDefUtils->GetNewestDefsDate( &wYear, &wMonth, &wDay, &dwRev) )
    {
        TCHAR szFullErrMsg[256];
        vpstrncpy   (szFullErrMsg, _T("Error getting def dates. "),            sizeof(szFullErrMsg));
        vpstrnappend(szFullErrMsg, ptrDefUtils->DefUtilsGetLastResultString(), sizeof(szFullErrMsg));
        DefWatchDebugOut( szFullErrMsg );
        return FALSE;
    }

    // 
    // Q: Are dates different?  If so, save off new dates.
    // 
    CVirusDefVersion defVer( wYear, wMonth, wDay, dwRev );

    if( defVer != m_cDefVersion )
    {
        // 
        // Save off new defs date.
        // 
        m_cDefVersion = defVer;
        if( m_cDefVersion.Save( DEFWATCH_REG_KEY ) == FALSE )
        {
            // 
            // Bad things have happened if we are here.  Too bad.
            // 
            DefWatchDebugOut( _T("Error saving def dates.") );
        }
    
        //
        // Make sure our AppID gets updated.
        //
        if (FALSE == ptrDefUtils->UseNewestDefs())
        {
            TCHAR szFullErrMsg[256];
            vpstrncpy   (szFullErrMsg, _T("Error moving to newest defs. "),        sizeof(szFullErrMsg));
            vpstrnappend(szFullErrMsg, ptrDefUtils->DefUtilsGetLastResultString(), sizeof(szFullErrMsg));
            DefWatchDebugOut( szFullErrMsg );
            return FALSE;
        }

        // 
        // Defs have changed!
        // 
        bRtn = true;
    }

    CoUninitialize();
    // 
    // Defs have not changed.
    // 
    return bRtn? TRUE : FALSE;
}
