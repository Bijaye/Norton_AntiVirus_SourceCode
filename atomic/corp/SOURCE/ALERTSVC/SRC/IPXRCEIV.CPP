// Copyright 1997 Symantec Corporation                                  
//***************************************************************************
// 
// $Header:   S:/ALERTSVC/VCS/ipxrceiv.cpv   1.5   14 May 1997 16:45:58   GDZIECI  $ 
// 
// Description:                   
//   Implements IPXReceiver class that listens for old NAVNLM style alerts
// sent over IPX.
// 
// Contains:                                                            
// 
// See Also:                                                            
// 
//***************************************************************************
// $Log:   S:/ALERTSVC/VCS/ipxrceiv.cpv  $ 
// 
//    Rev 1.5   14 May 1997 16:45:58   GDZIECI
// Moved WINSOCKVERSION definition to ALRTRECV.H.
// 
//    Rev 1.4   09 May 1997 11:28:36   GDZIECI
// One typo fixed.
// 
//    Rev 1.3   09 May 1997 11:13:14   GDZIECI
// Bug fixes.
// 
//    Rev 1.2   08 May 1997 18:21:46   GDZIECI
// First round of updates.
// 
//    Rev 1.1   07 May 1997 15:32:22   GDZIECI
// Changes in comments.
// 
//    Rev 1.0   06 May 1997 15:57:34   GDZIECI
// Initial revision.
//***************************************************************************

#include <windows.h>
#include <tchar.h>
#include <winsock.h>
#include <wsipx.h>
#include <nspapi.h>
#include <svcguid.h>

#include "alrtntfy.h"
#include "alrtrecv.h"


//***************************************************************************
// Defines
//***************************************************************************

#define SERVICE_NAME	_T("NAV_Alert_Service")

#define NAV_IPX_SOCKET	0x84be
#define NAV_SAPID		0x0531

#define NAVNLM_ID       "NAVNLM"
#define NAVNLM_ID_SIZE  6


//***************************************************************************
// Static data
//***************************************************************************

static GUID serviceId = SVCID_NETWARE(NAV_SAPID);

static WORD wSAPID = NAV_SAPID;

static SOCKADDR_IPX localAddr;

static TCHAR computerName[MAX_COMPUTERNAME_LENGTH + 1];

static SERVICE_TYPE_INFO_ABS serviceType =
{
	SERVICE_NAME,					// service type name
	1,								// number of values
	{
		NS_SAP,						// SAP name space
		REG_BINARY,					// binary value type
		sizeof(WORD),				// value size
		SERVICE_TYPE_VALUE_SAPID,	// SAP ID value type
		&wSAPID
	}
};

static SERVICE_ADDRESSES serviceAddress = 
{
	1,								// address count
	{
		AF_IPX,						// address type
		0,							// flags
		sizeof(SOCKADDR_IPX),		// address length
		0,
		(BYTE *)&localAddr,			// address
		NULL
	}
};

static SERVICE_INFO serviceInfo = 
{
	&serviceId,						// service type GUID
	computerName,					// service instance name - our computer name
	_T(""), 						// comment
	_T(""),							// locale - this needs to be set correctly
	RESOURCEDISPLAYTYPE_SERVER,		// display hint
	0,								// version
	0,								// time
	computerName,					// machine name
	&serviceAddress,				// service address
	{
		sizeof(serviceType),		// blob size
		(BYTE *)&serviceType		// blob data
	}
};



//***************************************************************************
// Member functions
//***************************************************************************

//***************************************************************************
// IPXReceiver::IPXReceiver()
// 
// Description:
//   IPXReceiver class contractor. Initializes worker thread proc address.
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

IPXReceiver::IPXReceiver()
{
    lpThreadProc = &AlertReceiverThreadProc;
}


//***************************************************************************
// IPXReceiver::init()
// 
// Description:
//   Initialized IPXReceiver. Initializes Winsock DLL and opens an IPX
// socket (well-known socket number). Then it registers our service and starts
// SAPing using our ID.
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

BOOL IPXReceiver::init ()
{
	WSADATA				winsockData;
    int                 iAddressLength;
	DWORD				dwBufferLength;
	DWORD	            dwStatusFlags;
	int					iErr = 0;
    BOOL                bRet = FALSE;

    // make sure we are not already running
    if (INVALID_SOCKET != s)
        kill();

    if (!bInitialized)
	    iErr = WSAStartup ( WINSOCKVERSION, &winsockData );

	if (0 == iErr)
	{
        bInitialized = TRUE;

		s = socket ( AF_IPX, SOCK_DGRAM, NSPROTO_IPX );
		
		if (s != INVALID_SOCKET)
		{
			iAddressLength = sizeof(SOCKADDR_IPX);

			memset ( &localAddr, 0, sizeof(localAddr) );
			localAddr.sa_family = AF_IPX;
			localAddr.sa_socket = htons ( NAV_IPX_SOCKET );

			iErr = bind ( s, (LPSOCKADDR)&localAddr, iAddressLength );

			if (0 == iErr)
			{
				iErr = getsockname ( s, (LPSOCKADDR)&localAddr, &iAddressLength );

				if (0 == iErr)
				{
					dwBufferLength = MAX_COMPUTERNAME_LENGTH;
					if (!GetComputerName ( computerName, &dwBufferLength ))
						_tcscpy ( computerName, SERVICE_NAME );

					// truncate the name to conform to SAP name limit
					computerName[48] = 0;

	                bRet = (SOCKET_ERROR != SetService ( NS_SAP,
                        SERVICE_REGISTER, 0, &serviceInfo, NULL,
                        &dwStatusFlags ));
                }
            }
        }
        
        if (s == INVALID_SOCKET || iErr == SOCKET_ERROR)
            iErr = WSAGetLastError();
    }

    if (iErr)
        iLastError = iErr;

    return bRet;
}


//***************************************************************************
// IPXReceiver::AlertReceiverThreadProc()
// 
// Description:
//   Class method - IPXReceiver thread procedure. Listens for IPX alerts
// and posts them to alert queue.
// 
// Parameters:
//  LPVOID p - ptr to IPXReceiver object
// 
// Return Value: 
//   DWORD (actually nothing)
// 
// See Also: 
// 
//***************************************************************************
// 05/02/1997 GDZIECI Function Created.                                 
//***************************************************************************

DWORD IPXReceiver::AlertReceiverThreadProc ( LPVOID p )
{
	SOCKADDR_IPX        remoteAddr;
	int					iAddressLength;
	int					i;
    IPXReceiver         *pIPXRcvr = (IPXReceiver *)p;
    int                 iRcvErrorCount = 0;
    BYTE                packet[512];

    pIPXRcvr->bThreadRunning = TRUE;

	iAddressLength = sizeof(remoteAddr);

    // 10 consecutive errors on read abort the thread
	while (iRcvErrorCount < 10 && !pIPXRcvr->bThreadExiting)
	{
		i = recvfrom ( pIPXRcvr->s, (char *)&packet, sizeof(packet), 0,
			(LPSOCKADDR)&remoteAddr, &iAddressLength );

		if (SOCKET_ERROR != i)
		{
            iRcvErrorCount = 0;

			pIPXRcvr->processIncomingPacket ( (EVENT_DATA_PACKET *)packet );
        }
        else
            {
            iRcvErrorCount++;
            pIPXRcvr->iLastError = WSAGetLastError();
            }
    }

    pIPXRcvr->bThreadRunning = FALSE;

    pIPXRcvr->deregisterService ();

    return 0;
}


//***************************************************************************
// IPXReceiver::deregisterService()
// 
// Description:
//   Deregisters our service listening for IPX alerts. Stops advertising.
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

void IPXReceiver::deregisterService ( void )
{
	DWORD	dwStatusFlags;

	SetService ( NS_SAP, SERVICE_DEREGISTER, 0,	&serviceInfo, NULL,
        &dwStatusFlags );
}


//***************************************************************************
// IPXReceiver::processIncomingPacket()
// 
// Description:
//   Processes an arrived IPX alert packet. Parses the virus information
// so that it can be submitted to AlertService queue for procesing.
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

void IPXReceiver::processIncomingPacket ( EVENT_DATA_PACKET *pPacket )
{
    LPSTR   lpszVirusName;
    LPSTR   lpszUserName;
    LPSTR   lpszPathName;
    LPSTR   lpszEnd;
    DWORD   dwSource;

    switch (pPacket->byCommand)
    {
    case EVENT_WORKSTATION_VIRUS:
        if (!strncmp ( pPacket->chID, EVENT_NAV_ID, EVENT_ID_SIZE ))
        {
            // parse out strings, terminate strings with 0
            lpszVirusName = strchr( pPacket->data, ';' ) + 1;
            lpszPathName  = strchr( lpszVirusName, ';' );
            *lpszPathName++ = 0;
            lpszUserName  = strchr( lpszPathName,  ';' );
            *lpszUserName++ = 0;
            lpszEnd       = strchr( lpszUserName,  ';' );
            *lpszEnd++      = 0;

            if (!strncmp ( lpszEnd, NAVNLM_ID, NAVNLM_ID_SIZE ))
                dwSource = PACKET_SOURCE_NAVNLM;
            else
                dwSource = PACKET_SOURCE_NAVW;

            // Create event for processing
            sendVirusNotificationPacket ( lpszVirusName, lpszUserName,
                lpszPathName, dwSource );
        }
        // else source is not NAV - ignore the msg

        break;

    default:
        // other commands ignored
        break;
    }
}
