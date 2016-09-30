// Copyright 1997 Symantec Corporation                                  
//***************************************************************************
// 
// $Header:   S:/ALERTSVC/VCS/IPRECEIV.CPv   1.3   09 May 1997 11:13:16   GDZIECI  $ 
// 
// Description:                   
//   Implements IPReceiver class that listens for IP based alerts. This
// is only skeleton now, needs to be implemented.
// 
// Contains:                                                            
// 
// See Also:                                                            
// 
//***************************************************************************
// $Log:   S:/ALERTSVC/VCS/IPRECEIV.CPv  $ 
// 
//    Rev 1.3   09 May 1997 11:13:16   GDZIECI
// Bug fixes.
// 
//    Rev 1.2   08 May 1997 18:22:08   GDZIECI
// Updated.
// 
//    Rev 1.1   07 May 1997 15:33:48   GDZIECI
// Changes in comments.
// 
//    Rev 1.0   06 May 1997 15:59:16   GDZIECI
// Initial revision.
//***************************************************************************

#include <windows.h>
#include <tchar.h>
#include <winsock.h>

#include "alrtrecv.h"


//***************************************************************************
// Member functions
//***************************************************************************

//***************************************************************************
// IPReceiver::IPReceiver()
// 
// Description:
//   IPReceiver class contractor. Initializes worker thread proc address.
// 
// Parameters:
//   None
// 
// Return Value: 
// 
// See Also: 
// 
//***************************************************************************
// 05/08/1997 GDZIECI Function Created.                                 
//***************************************************************************

IPReceiver::IPReceiver()
{
    lpThreadProc = &AlertReceiverThreadProc;
}


//***************************************************************************
// IPReceiver::init()
// 
// Description:
//   Initialized IPReceiver. Initializes Winsock DLL and opens an IP
// socket (well-known port).
// 
// Parameters:
//   None
// 
// Return Value: 
//   TRUE if initialization successful, FALSE otherwise
// 
// See Also: 
// 
//***************************************************************************
// 05/02/1997 GDZIECI Function Created.                                 
//***************************************************************************

BOOL IPReceiver::init ()
{
    return TRUE;
}


//***************************************************************************
// IPReceiver::AlertReceiverThreadProc()
// 
// Description:
//   Class method - IPReceiver thread procedure. Listens for IP alerts
// and posts them to alert queue.
// 
// Parameters:
//  LPVOID p - ptr to IPReceiver object
// 
// Return Value: 
//   DWORD (actually nothing)
// 
// See Also: 
// 
//***************************************************************************
// 05/02/1997 GDZIECI Function Created.                                 
//***************************************************************************

DWORD IPReceiver::AlertReceiverThreadProc ( LPVOID p )
{
	SOCKADDR_IN         remoteAddr;
	int					iAddressLength;
	int					i;
    IPReceiver          *pIPRcvr = (IPReceiver *)p;
    int                 iRcvErrorCount = 0;
    char                packet[512];

    pIPRcvr->bThreadRunning = TRUE;

	iAddressLength = sizeof(remoteAddr);

    // 10 consecutive errors on read abort the thread
	while (iRcvErrorCount < 10 && !pIPRcvr->bThreadExiting)
	{
		i = recvfrom ( pIPRcvr->s, packet, sizeof(packet), 0,
			(LPSOCKADDR)&remoteAddr, &iAddressLength );

		if (SOCKET_ERROR != i)
		{
            iRcvErrorCount = 0;

			pIPRcvr->processIncomingPacket ( (EVENT_DATA_PACKET *)packet );
        }
        else
            {
            iRcvErrorCount++;
            pIPRcvr->iLastError = WSAGetLastError();
            }
    }

    pIPRcvr->bThreadRunning = FALSE;

    pIPRcvr->deregisterService ();

    return 0;
}


//***************************************************************************
// IPReceiver::processIncomingPacket()
// 
// Description:
//   Processes incoming IP alert packets.
// 
// Parameters:
// 
// Return Value: 
// 
// See Also: 
// 
//***************************************************************************
// 05/06/1997 GDZIECI Function Created.                                 
//***************************************************************************

void IPReceiver::processIncomingPacket ( EVENT_DATA_PACKET *pPacket )
{
}


