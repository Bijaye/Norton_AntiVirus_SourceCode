/////////////////////////////////////////////////////////////////////////////
// Copyright 1998 Symantec Corporation.
/////////////////////////////////////////////////////////////////////////////
// QuarSvc.cpp
//
// $Header:   S:/QUARSVC/VCS/QuarSvc.cpv   1.5   29 Jun 1998 20:05:40   sedward  $
//
// Description: Main source file for the Quarantine Server service.
//
// Contains: CQuarantineService
//           WinMain()
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QUARSVC/VCS/QuarSvc.cpv  $
// 
//    Rev 1.5   29 Jun 1998 20:05:40   sedward
// Added code fix from NSDU, we now explicitly ensure that the RPC services we
// need are started ('WorkerThreadProc').
//
//    Rev 1.4   22 Apr 1998 18:34:24   DBuches
// Hooked up stale file handle monitor.
//
//    Rev 1.3   21 Apr 1998 19:26:30   SEDWARD
// Check in work in progress (FileMon stuff).
//
//    Rev 1.1   06 Apr 1998 13:58:38   SEDWARD
// New stuff.
//
/////////////////////////////////////////////////////////////////////////////



#include "stdio.h"
#include "platform.h"
#include "service.h"
#include "svcctrl.h"
#include "QuarStr.h"
#include "QuarSvc.h"
#include "QuarAdd.h"
#include "FileMon.h"    // file handle monitor object
#include "process.h"

#include "islink.h"
#include "LinkSrv.h"





/////////////////////////////////////////////////////////////////////
// Globals

DECLARE_SERVICE(CQuarantineService, QuarService, IDS_ERR_REGISTERSERVICECTRLR_FAILED);


CLinkServer     g_cLinkServer;             // For RPC communications
HINSTANCE       g_hInst;


// ==== CQuarantineService::CQuarantineService ============================
//
//  Input:
//      pMain       -- pointer to the service main function
//      pHandler    -- pointer to the handler
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

CQuarantineService::CQuarantineService(LPSERVICE_MAIN_FUNCTIONW  pMain
                                            , LPHANDLER_FUNCTION pHandler)
        : CService(pMain, pHandler)
{
    LTrace("CQuarantineService constructor");
    m_hEvent = NULL;
    m_bExitingService = FALSE;
}



// ==== CQuarantineService::~CQuarantineService ===========================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

CQuarantineService::~CQuarantineService()
{
                                        // Clean up our event handle
    LTrace("CQuarantineService destructor");
    if (NULL != m_hEvent)
        {
        CloseHandle(m_hEvent);
        m_hEvent = NULL;
        }
}



// ==== CQuarantineService::InitService ===================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

VOID    CQuarantineService::InitService(void)
{
    LTrace("CQuarantineService::InitService, before AllowPauseContinue(FALSE);");
    AllowPauseContinue(FALSE);

    // create an event so we can tell the worker thread to stop
    m_hEvent = CreateEvent(NULL,        // Pointer to security attributes
                            FALSE,      // flag for manual-reset of event
                            FALSE,      // flag for initial state
                            NULL );     // Don't name the event
    if (NULL == m_hEvent)
        {
        #if 0   // IFDEF_DISABLE
        ErrorHandler(ID_ERR_CREATE_EVENT | ID_ERR_FATAL);
        #endif  // IFDEF_DISABLE
        goto  Cleanup;
        }

    // start a thread that will take care of getting everything running
    auto    HANDLE      hThread;
    hThread = (HANDLE)_beginthread(WorkerThreadProc             // start Addr
                                    , 0                         // stack size
                                    , (LPVOID) this);           // send in a reference to our object
    LTrace("CQuarantineService::InitService, hThread = %0x", hThread);

    // bugbug - how do we want to handle failed thread creation?
    #if 0  // IFDEF_DISABLE
    if (NULL == hThread)
        {
        ErrorHandler(
            ID_ERR_CREATE_THREAD |
            ID_ERR_FATAL);

        goto Cleanup;
        }
    #endif  // IFDEF_DISABLE

    // simply report success
    LTrace("CQuarantineService::InitService, before ReportStatusToSCMgr(SERVICE_RUNNING, NO_ERROR, 0)");
    ReportStatusToSCMgr(SERVICE_RUNNING, NO_ERROR, 0);


Cleanup:
    return;

}  // end of "CQuarantineService::InitService"



/////////////////////////////////////////////////////////////////////
// WorkerThreadProc
//
// This function is a C callthrough to our C++ member function
//
// Parameters:
//    none.
//
// Return Value:
//    doesn't return.  We Terminate this thread when we are done.
//
/////////////////////////////////////////////////////////////////////
// 07/28/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
VOID WorkerThreadProc(LPVOID lpInfo)
{
    auto    BOOL            bReturn = FALSE;
    auto    CServiceControl cServiceControl;
    auto    char            szServiceName[MAX_PATH];
    auto    DWORD           dwReturn;


                                        // The first thing that the worker
                                        // thread should do is set our service
                                        // to be AutoStart.

                                        // This has to be in the worker thread
                                        // because otherwise we can't open the
                                        // ServiceControl manager when the
                                        // machine is booting up.

    dwReturn = LoadString (
                   g_hInst,
                   IDS_QUAR_SERVICE_NAME,
                   szServiceName,
                   sizeof (szServiceName) / sizeof (szServiceName[0]));


                                        // This following If statement is going
                                        // to do two things for us.  #1, it
                                        // will set us to AutoStart.  #2, it
                                        // will make sure that upon auto-start,
                                        // ALL other services are up and
                                        // running before we start the RPC
                                        // Server.
    if (dwReturn > 0)
        {
        cServiceControl.Open(
            szServiceName);

                                        // Now set our service to AutoStart
                                        // with the RPCLOCATOR as a dependency.
        cServiceControl.ChangeServiceConfig_StartType(
            SERVICE_AUTO_START);
        }


                                        // Load the RPC Service's name.
    dwReturn = LoadString (
                   g_hInst,
                   IDS_RPC_SERVICE,
                   szServiceName,
                   sizeof (szServiceName) / sizeof (szServiceName[0]));


                                        // Check to make sure that the RPC
                                        // Service is up and running before we
                                        // start the RPC Server.
    if (dwReturn > 0)
        {
        cServiceControl.Open(
            szServiceName);

                                        // Now set our service to AutoStart
                                        // with the RPCLOCATOR as a dependency.
        cServiceControl.Start();
        }


    auto    BOOL                    bResult = FALSE;
    auto    CQuarantineService*     pThis;

    // get a pointer to this object
    pThis = (CQuarantineService*)lpInfo;


    // Start RPC server
    g_cLinkServer.SetProtocol(
        (LPSTR)NULL,                    // Use all
        (LPSTR)NULL,                    // with dynamic endpoints
        FALSE);                         // Don't require security

    if (!g_cLinkServer.StartListen( &ISLink_v1_0_s_ifspec, 1))
        {
        LTrace("CQuarantineService::WorkerThreadProc, Could not start RPC server");
        }


    while(FALSE == pThis->m_bExitingService)
        {
        // sleep for a while
        if (WaitForSingleObject(pThis->m_hEvent, 300000) != WAIT_TIMEOUT)
            {
            // someone signaled us, so bail out
            break;
            }

        // remove any stale copy connections
        pThis->m_handleMonitor.RemoveStaleHandles(MAX_STALE_TIME);
        }

   _endthread();                        // Don't just return!!!
}


// ==== CQuarantineService::ServiceControlStop ============================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

VOID    CQuarantineService::ServiceControlStop(void)
{
    LTrace("CQuarantineService::ServiceControlStop, about to call ReportStatusToSCMgr(SERVICE_STOPPED, NO_ERROR, 0)");
    ReportStatusToSCMgr(SERVICE_STOPPED, NO_ERROR, 0);
    m_bExitingService = TRUE;

    // signal our worker thread to wake up and exit.
    SetEvent (m_hEvent);

    return;

}  // end of "CQuarantineService::ServiceControlStop"




// ==== WinMain ===========================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

int PASCAL  WinMain(HINSTANCE  hInst, HINSTANCE  hPrev, LPSTR  lpszCommand
                                                            , int  nCmdShow)
{
    auto    BOOL        bInstallService = FALSE;
    auto    TCHAR       szService[MAX_PATH + 1];
    auto    int         nRet = 0;

    nRet = LoadString(hInst, IDS_QUAR_SERVICE_NAME, szService
                                , sizeof(szService) / sizeof(szService[0]));

    LTrace("WinMain, LoadString returned %s", szService);


    if (nRet > 0)
        {
        g_hInst = hInst;
        LTrace("WinMain, before call to QuarService.Create(%s)", szService);
        QuarService.Create(szService);
        LTrace("WinMain, after call to QuarService.Create(%s)", szService);
        }

    return 0;

}  // end of "WinMain"




/////////////////////////////////////////////////////////////////////////////
//
//  LTrace() - Function to trace output to a log fle put in the windows dir
//
//  Params: LPCSTR - Format string
//          Variable Args - To be substituted into format string
//
//  Return value:   VOID
//
/////////////////////////////////////////////////////////////////////////////
//  02/14/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////

VOID CDECL LTrace( LPCSTR lpszFmt, ... )
{
    auto    FILE        *fp = NULL;
    auto    DWORD       dwNumBytes = 0L;
    auto    LPSTR       lpArgs = (LPSTR)&lpszFmt + sizeof( lpszFmt );
    auto    TCHAR       szBuffer[MAX_PATH];
    auto    TCHAR       szLogFile[MAX_PATH];


    if (FALSE == FileExists("C:\\TEMP\\DBQSVC.DAT"))
        {
        goto  Exit_Function;
        }

    wvsprintf( (LPSTR)szBuffer, lpszFmt, lpArgs );
    lstrcat( szBuffer, "\n" );

#ifdef _LOGFILE_TRACE_
    GetWindowsDirectory( szLogFile, MAX_PATH - 1 );
    lstrcat( szLogFile, "\\QUARSRV.LOG");
    fp = fopen( szLogFile, "at" );

    if ( fp != NULL )
        {
        fputs( szBuffer, fp );
        fflush( fp );
        fclose( fp );
        }
#else
    OutputDebugString( szBuffer );
#endif

Exit_Function:
    return;
}



// ==== FileExists ========================================================
//
//  This function simply checks to see if the specified file exists or not.
//
//  Input:
//      lpszFile    -- a fully-qualified path to the file
//
//  Output:
//      TRUE if the specified file exists, FALSE if it does not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    FileExists(LPTSTR  lpszFile)
{
    auto    DWORD       dwTempAttributes = 0;
    auto    BOOL        bResult = TRUE;

    // see if the data file really exists
    dwTempAttributes = GetFileAttributes(lpszFile);
    if (0xFFFFFFFF == dwTempAttributes)
        {
        bResult = FALSE;
        }

    return (bResult);

}  // end of "CQuarantineOptions::QuarOptsFileExists"

