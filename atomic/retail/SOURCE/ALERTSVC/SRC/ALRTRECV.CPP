// Copyright 1997 Symantec Corporation
//***************************************************************************
//
// $Header:   S:/ALERTSVC/VCS/alrtrecv.cpv   1.4   18 May 1997 05:06:26   DSACKIN  $
//
// Description:
//   Implements AlertReceiver class that listens for incoming alerts. This
// is an abstract class that must be extended. Provides basic functionality
// for listening to alerts.
//
// Contains:
//
// See Also:
//
//***************************************************************************
// $Log:   S:/ALERTSVC/VCS/alrtrecv.cpv  $
// 
//    Rev 1.4   18 May 1997 05:06:26   DSACKIN
// Switched over from CAlertNotification to CVirusAlert
//
//    Rev 1.3   09 May 1997 11:13:00   GDZIECI
// Bug fixes.
//
//    Rev 1.2   08 May 1997 18:20:04   GDZIECI
// First round of revisions.
//
//    Rev 1.1   07 May 1997 15:31:40   GDZIECI
// Minor change in a comment.
//
//    Rev 1.0   06 May 1997 15:56:34   GDZIECI
// Initial revision.
//***************************************************************************

#include "platform.h"

#include <winsock.h>

#include "navrpc.h"         // ALERTSVC_PROTOCOL...

// Zoso
// #include "alrtntfy.h"       // CALertNotification class

                                        // CVirusAlert class
#include "CVAlert.h"

#include "navutil.h"        // NavRpcSendPacket


#include "alrtrecv.h"


//***************************************************************************
// Member functions
//***************************************************************************

//***************************************************************************
// AlertReceiver::AlertReceiver()
//
// Description:
//   AlertReceiver class default constructor.
//
// Parameters:
//   None
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 05/02/1997 GDZIECI Function Created.
//***************************************************************************

AlertReceiver::AlertReceiver ()
{
    bInitialized    = FALSE;
    bThreadRunning  = FALSE;
    bThreadExiting  = FALSE;
    s               = INVALID_SOCKET;
    hThread         = NULL;
    iLastError      = 0;
}


//***************************************************************************
// AlertReceiver::~AlertReceiver()
//
// Description:
//   AlertReceiver class constructor.
//
// Parameters:
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 05/02/1997 GDZIECI Function Created.
//***************************************************************************

AlertReceiver::~AlertReceiver ()
{
    // derived class is responsible for real cleanup
}


//***************************************************************************
// AlertReceiver::run()
//
// Description:
//   Starts a thread listening and servicing alerts.
//
// Parameters:
//   None
//
// Return Value:
//   TRUE if thread started successfully, FALSE if an error enountered
//
// See Also:
//
//***************************************************************************
// 05/02/1997 GDZIECI Function Created.
//***************************************************************************

BOOL AlertReceiver::run ()
{
    DWORD dwThreadID;
    BOOL  bRet = TRUE;

    // make sure that our service is registered before we can start listening
    if (INVALID_SOCKET == s)
        bRet = init();

    if (bRet)
        {
        hThread = CreateThread ( NULL, 0,
            lpThreadProc, (LPVOID)this, 0, &dwThreadID );
        bRet = (NULL != hThread);
        }

    return bRet;
}


//***************************************************************************
// AlertReceiver::kill()
//
// Description:
//   Kills the listening thread (if running) and cleans up: closes the
// socket, unregisters our service and deinitializes Winsock.
// We use brutal force to terminate the thread (TerminateThread() call). This
// is because we use blocking Winsock calls for simplicity and efficiency.
//
// Parameters:
//   None
//
// Return Value:
//   None
//
// See Also:
//
//***************************************************************************
// 05/02/1997 GDZIECI Function Created.
//***************************************************************************

void AlertReceiver::kill ()
{
    if (hThread)
        {
        if (bThreadRunning)
            {
            // first try to terminate gracefully in case we are
            // currently processing something
            bThreadExiting = TRUE;

            // wait 100ms for the thread to exit
            Sleep ( 100 );

            if (bThreadRunning)
                {
                // the thread did not exit, must be blocking on receive
                deregisterService();
                TerminateThread ( hThread, 0 );
                }

            bThreadExiting = FALSE;
            bThreadRunning = FALSE;
            }
        CloseHandle ( hThread );
        hThread = NULL;
        }

    if (INVALID_SOCKET != s)
        {
        closesocket ( s );
        s = INVALID_SOCKET;
        }

    if (bInitialized)
        {
        WSACleanup();
        bInitialized = FALSE;
        }
}


//***************************************************************************
// AlertReceiver::deregisterService()
//
// Description:
//   Default procedure to deregister service that listens for alert. Does
// nothing. Typically, derived class should override this method.
//
// Parameters:
//   None
//
// Return Value:
//   None
//
// See Also:
//
//***************************************************************************
// 05/02/1997 GDZIECI Function Created.
//***************************************************************************

void AlertReceiver::deregisterService ()
{
    // Do nothing
}


//***************************************************************************
// AlertReceiver::sendVirusNotificationPacket()
//
// Description:
//   Creates an AlertService packet and submits it to the queue for
// processing.
//
// Parameters:
//   LPSTR lpszVirusName    [in] found virus name
//   LPSTR lpszUserName     [in] name of the user accessing the infected file
//   LPSTR lpszPathName     [in] file name
//   DWORD dwEventSource    [in] identifies event source (product)
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 05/06/1997 GDZIECI Function Created.
//***************************************************************************

void AlertReceiver::sendVirusNotificationPacket ( LPSTR lpszVirusName,
    LPSTR lpszUserName, LPSTR lpszPathName, DWORD dwEventSource )
{
    CVirusAlert         cVirusAlert;
    LPVOID              pBuffer;

    cVirusAlert.SetPacketSource ( PACKET_SOURCE_REMOTE | dwEventSource );

                                        // Set the UUID
    cVirusAlert.CreateUniqueID();

                                        // Set the time in CAlert.
    cVirusAlert.StampTime();

    cVirusAlert.SetAlertType (FILE_KNOWN_VIRUS);

    cVirusAlert.SetAction ( ACTION_UNDEFINED );
    cVirusAlert.SetActionSuccess ( TRUE );

                                        // Set the string information
    cVirusAlert.SetUserName ( lpszUserName );
    cVirusAlert.SetObjectName ( lpszPathName );
    cVirusAlert.SetVirusName ( lpszVirusName );

    pBuffer = (LPVOID)cVirusAlert.GetPacket();

    NavRpcSendPacket (
       ALERTSVC_PROTOCOL,
       ALERTSVC_ADDRESS,
       NULL,
       pBuffer );

    cVirusAlert.ReleasePacket();
}
