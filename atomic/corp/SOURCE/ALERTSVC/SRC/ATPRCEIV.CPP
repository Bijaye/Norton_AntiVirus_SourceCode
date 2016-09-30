// Copyright 1997 Symantec Corporation                                  
//***************************************************************************
// 
// $Header:   S:/ALERTSVC/VCS/atprceiv.cpv   1.5   23 May 1997 18:11:04   GDZIECI  $ 
// 
// Description:                   
//   Implements ATPReceiver class that listens for SAM alerts via AppleTalk.                                      
// 
// Contains:                                                            
// 
// See Also:                                                            
// 
//***************************************************************************
// $Log:   S:/ALERTSVC/VCS/atprceiv.cpv  $ 
// 
//    Rev 1.5   23 May 1997 18:11:04   GDZIECI
// Fixed a bug in the ATP packet header.
// 
//    Rev 1.4   14 May 1997 16:45:24   GDZIECI
// Moved WINSOCKVERSION definition to ALRTRECV.H.
// 
//    Rev 1.3   09 May 1997 11:13:10   GDZIECI
// Bug fixes.
// 
//    Rev 1.2   08 May 1997 18:23:04   GDZIECI
// Updated.
// 
//    Rev 1.1   07 May 1997 15:33:12   GDZIECI
// Changes in comments.
// 
//    Rev 1.0   06 May 1997 15:58:46   GDZIECI
// Initial revision.
//***************************************************************************

#include <windows.h>
#include <winsock.h>
#include <atalkwsh.h>

#include "alrtntfy.h"
#include "alrtrecv.h"


//***************************************************************************
// Defines
//***************************************************************************

#define NAVNET_TYPE_NAME		"NAVNET_Server"
#define NAVNET_TYPE_LENGTH		strlen("NAVNET_Server")

#define LOCAL_ZONE				"*"
#define LOCAL_ZONE_LENGTH		1

#define DEFAULT_COMPUTER_NAME	"NAV_Alert_Service"

#define ATP_DATA_SIZE	578
#define ATP_REPLY_PACKET		0x90


//***************************************************************************
// Type definitions
//***************************************************************************

typedef struct tagATPHeader
{
	BYTE	byFlags;			// control flags
	BYTE	bySequenceNo;		// buffer bitmap/sequence number
	WORD	wTID;				// transaction ID
	BYTE	userData[4];		// additiona header info
} ATP_HEADER;

typedef struct tagATPPacket
{
	ATP_HEADER	header;
	BYTE		data[ATP_DATA_SIZE];
} ATP_PACKET;


//***************************************************************************
// Static data
//***************************************************************************

static BYTE byMacToPC[128] = {
    0x8e, 0x8f, 0x80, 0x90, 0xa5, 0x99, 0x9a, 0xa0,     // 80-87
    0x85, 0x83, 0x84, 0x61, 0x86, 0x87, 0x82, 0x8a,     // 88-8f
    0x88, 0x89, 0xa1, 0x8d, 0x8c, 0x8b, 0xa4, 0xa2,     // 90-97
    0x95, 0x93, 0x94, 0x6f, 0xa3, 0x97, 0x96, 0x81,     // 98-9f
    0x20, 0xf8, 0x9b, 0x9c, 0x15, 0x00, 0x14, 0xe1,     // a0-a7
    0x20, 0x20, 0x20, 0x27, 0x22, 0x20, 0x92, 0x20,     // a8-af
    0xec, 0xf1, 0xf3, 0xf2, 0x90, 0xe6, 0xeb, 0xe4,     // b0-b7
    0x20, 0xe3, 0x20, 0xa6, 0xa7, 0xea, 0x91, 0xed,     // b8-bf
    0xa8, 0xad, 0xaa, 0xfb, 0x9f, 0xf7, 0x20, 0xae,     // c0-c7
    0xaf, 0x20, 0x20, 0x41, 0x41, 0x4f, 0x20, 0x20,     // c8-cf
    0x2d, 0x2d, 0x22, 0x22, 0x27, 0x27, 0xf6, 0x20,     // d0-d7
    0x98, 0x59, 0x2f, 0x20, 0x3c, 0x3e, 0x20, 0x20,     // d8-df
    0x20, 0x20, 0x2c, 0x22, 0x20, 0x41, 0x45, 0x41,     // e0-e7
    0x45, 0x45, 0x49, 0x49, 0x49, 0x49, 0x4f, 0x4f,     // e8-ef
    0x20, 0x4f, 0x55, 0x55, 0x55, 0x20, 0x20, 0x7e,     // f0-f7
    0x20, 0x20, 0x20, 0xf8, 0x20, 0x22, 0x20, 0x20,     // f8-ff
};


//***************************************************************************
// Member functions
//***************************************************************************

//***************************************************************************
// ATPReceiver::ATPReceiver()
// 
// Description:
//   ATPReceiver class contractor. Initializes worker thread proc address.
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

ATPReceiver::ATPReceiver()
{
    lpThreadProc = &AlertReceiverThreadProc;
}


//***************************************************************************
// ATPReceiver::init()
// 
// Description:
//   Initialized ATPReceiver. Initializes Winsock DLL and opens an ATP
// socket. Then it registers our service with NBP.
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

BOOL ATPReceiver::init ()
{
	WSADATA				winsockData;
	SOCKADDR_AT         localAddr;
	WSH_REGISTER_NAME   NBPName;
	char				computerName[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD				dwBufferLength;
	int					iErr = 0;

    // make sure we are not already running
    if (INVALID_SOCKET != s)
        kill();

    if (!bInitialized)
	    iErr = WSAStartup ( WINSOCKVERSION, &winsockData );

	if (0 == iErr)
	{
        bInitialized = TRUE;

		s = socket ( AF_APPLETALK, SOCK_DGRAM, DDPPROTO_ATP );
		
		if (s != INVALID_SOCKET)
		{
			memset ( &localAddr, 0, sizeof(localAddr) );
			localAddr.sat_family = AF_APPLETALK;

			iErr = bind ( s, (LPSOCKADDR)&localAddr, sizeof(localAddr) );

			if (0 == iErr)
			{
				dwBufferLength = MAX_COMPUTERNAME_LENGTH;
				if (!GetComputerNameA ( computerName, &dwBufferLength ))
					strcpy ( computerName, DEFAULT_COMPUTER_NAME );

				// truncate the name to conform to NBP name limit
				computerName[MAX_ENTITY] = 0;

				strcpy ( NBPName.ObjectName, computerName );
				NBPName.ObjectNameLen = strlen ( computerName );

				strcpy ( NBPName.TypeName, NAVNET_TYPE_NAME );
				NBPName.TypeNameLen = NAVNET_TYPE_LENGTH;

				strcpy ( NBPName.ZoneName, LOCAL_ZONE );
				NBPName.ZoneNameLen = LOCAL_ZONE_LENGTH;

                // register our service with NBP
                iErr = setsockopt ( s, SOL_APPLETALK, SO_REGISTER_NAME,
					(char *)&NBPName, sizeof(WSH_NBP_NAME));
            }
        }
        
        if (s == INVALID_SOCKET || iErr == SOCKET_ERROR)
            iErr = WSAGetLastError();
    }

    if (iErr)
        iLastError = iErr;

    return (0 == iErr);
}


//***************************************************************************
// ATPReceiver::AlertReceiverThreadProc()
// 
// Description:
//   Class method - ATPReceiver thread procedure. Listens for ATP alerts
// and posts them to alert queue.
// 
// Parameters:
//  LPVOID p - ptr to ATPReceiver object
// 
// Return Value: 
//   DWORD (actually nothing)
// 
// See Also: 
// 
//***************************************************************************
// 05/02/1997 GDZIECI Function Created.                                 
//***************************************************************************

DWORD ATPReceiver::AlertReceiverThreadProc ( LPVOID p )
{
	SOCKADDR_AT         remoteAddr;
	int					iAddressLength;
	int					i;
	ATP_PACKET			packet;
	WORD				wPrevTID = 0;
    ATPReceiver         *pATPRcvr = (ATPReceiver *)p;
    int                 iRcvErrorCount = 0;

    pATPRcvr->bThreadRunning = TRUE;

	iAddressLength = sizeof(remoteAddr);

    // 10 consecutive errors on read abort the thread
	while (iRcvErrorCount < 10 && !pATPRcvr->bThreadExiting)
	{
		i = recvfrom ( pATPRcvr->s, (char *)&packet, sizeof(packet), 0,
			(LPSOCKADDR)&remoteAddr, &iAddressLength );

		if (SOCKET_ERROR != i)
		{
            iRcvErrorCount = 0;

			packet.header.byFlags = ATP_REPLY_PACKET;
			packet.header.bySequenceNo = 0;

			i = sendto ( pATPRcvr->s, (char *)&packet, i, 0,
				(LPSOCKADDR)&remoteAddr, iAddressLength );

			if (SOCKET_ERROR != i)
			{
				if (wPrevTID != packet.header.wTID)
				{
					wPrevTID = packet.header.wTID;

					pATPRcvr->processIncomingPacket (
                        (EVENT_DATA_PACKET *)packet.data );
				}
			}
        }
        else
            iRcvErrorCount++;

        if (SOCKET_ERROR == i)
            pATPRcvr->iLastError = WSAGetLastError();
    }

    pATPRcvr->bThreadRunning = FALSE;

    pATPRcvr->deregisterService();

    return 0;
}


//***************************************************************************
// ATPReceiver::processIncomingPacket()
// 
// Description:
//   Processes an arrived ATP packet. Parses the virus information so that
// it can be submitted to the AlertService queue for processing.
// 
// Parameters:
//   EVENT_DATA_PACKET *pPacket     [in] ptr to arrived data
// 
// Return Value:
//   None
// 
// See Also: 
// 
//***************************************************************************
// 05/06/1997 GDZIECI Function Created.                                 
//***************************************************************************

void ATPReceiver::processIncomingPacket ( EVENT_DATA_PACKET *pPacket )
{
    LPSTR   lpszVirusName;
    LPSTR   lpszUserName;
    LPSTR   lpszPathName;

    switch (pPacket->byCommand)
    {
    case EVENT_WORKSTATION_VIRUS:
        if (!strncmp (pPacket->chID, EVENT_SAM_ID, EVENT_ID_SIZE ))
        {
            // Parse out strings, terminate strings with 0
            lpszVirusName = pPacket->data;
            lpszPathName  = lpszVirusName + *lpszVirusName + 1;
            lpszUserName  = lpszPathName + *lpszPathName + 1;
            lpszVirusName++;
            *lpszPathName++ = 0;
            *lpszUserName++ = 0;

            // convert to PC character set
            convertMacToPC ( (LPBYTE)lpszPathName );
            convertMacToPC ( (LPBYTE)lpszVirusName );
            convertMacToPC ( (LPBYTE)lpszUserName );

            // Create event for processing
            sendVirusNotificationPacket ( lpszVirusName, lpszUserName,
                lpszPathName, PACKET_SOURCE_SAM );
        }
        // else source is not SAM - ignore the msg

        break;

    default:
        // other commands ignored
        break;
    }
}


//***************************************************************************
// ATPReceiver::convertMacToPC()
// 
// Description:
//   Converts a null-terminatated string in MAC character set to a PC string.
// 
// Parameters:
//   LPBYTE lpbyStringBuffer    [IN/OUT] string buffer to convert
// 
// Return Value:
//   None
// 
// See Also: 
// 
//***************************************************************************
// 05/06/1997 GDZIECI Function Created.                                 
//***************************************************************************

void ATPReceiver::convertMacToPC ( LPBYTE lpbyStringBuffer )
{
    while (*lpbyStringBuffer)
        {
        // convert control chars to space
        if (*lpbyStringBuffer < ' ')
            *lpbyStringBuffer = ' ';
        // translate chars above 0x80
        else if (*lpbyStringBuffer > 0x7f)
            *lpbyStringBuffer = byMacToPC[*lpbyStringBuffer - 0x80];
        // else is OK
        lpbyStringBuffer++;
        }
}

