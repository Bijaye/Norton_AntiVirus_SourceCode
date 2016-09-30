/////////////////////////////////////////////////////////////////////
// NTSNMP.CPP - Created - 11/17/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTSVC/VCS/ntsnmp.cpv   1.10   11 Jul 1998 15:42:56   jbelden  $
//
// Description:
//    This file will contain the code for our SNMP alerting.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTSVC/VCS/ntsnmp.cpv  $
// 
//    Rev 1.10   11 Jul 1998 15:42:56   jbelden
// added code so that "action taken" is tacked on to the alert message.
//
//    Rev 1.9   19 Jun 1997 14:16:14   DSACKIN
// When sending up a char * buffer to GetUserName, we must explicitly call
// the GetUserNameA function.
//
//    Rev 1.8   09 Jun 1997 01:57:10   DSACKIN
// We must clean up the lpcVirusAlert and lpcTarget when we are done.
//
//    Rev 1.7   20 May 1997 18:58:42   GDZIECI
// Fixed a bug with Enterprise ID set incorrectly.
//
//    Rev 1.6   20 May 1997 14:16:04   GDZIECI
// Hooked up to CVirusAlert packet.
//
//    Rev 1.5   19 May 1997 11:34:36   GDZIECI
// Added some comments.
//
//    Rev 1.4   19 May 1997 10:58:02   GDZIECI
// Fixed some encoding bugs.
//
//    Rev 1.3   18 May 1997 05:07:10   DSACKIN
// Switched over from CAlertNotification to CVirusAlert
//
//    Rev 1.2   16 May 1997 11:36:18   GDZIECI
// First implementation of ASN.1 encoding in our trap PDU packet.
//
//    Rev 1.1   14 May 1997 16:41:38   GDZIECI
// Created a skeleton for sending SNMP traps.
//
//    Rev 1.0   18 Apr 1997 13:41:18   DSACKIN
// Ported from Tesla
//
//    Rev 1.1   24 Mar 1997 20:32:40   DSACKIN
// Changed the prototype of the ProcessWorkOrder function.
//
//    Rev 1.0   13 Mar 1997 15:06:10   RFULLER
// Initial revision
//
//    Rev 1.1   18 Jan 1997 07:01:20   DSACKIN
// Reworked the AlertSvc to use the CAlertQueueManager object to handle all
// alerting.
//
//    Rev 1.0   18 Nov 1996 12:32:48   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////


#define _WIN32_WINNT 0x0400

/////////////////////////////////////////////////////////////////////

#include "platform.h"

#include <winsock.h>
#include <wsipx.h>
#include <snmp.h>
#include <lmcons.h>             // required for managers.h

#include "service.h"
#include "AlertQ.h"
#include "Managers.h"
#include "cvalert.h"
#include "at_tgt.h"
#include <linksrv.h>
#include "alertsvc.h"
#include "alertstr.h"

#include "ctsn.h"               // for SIZE_VIRUSNAME


//***************************************************************************
// Typedefs
//***************************************************************************

typedef struct tagASNTYPE_ANY
{
    BYTE    type;
    BYTE    data[1];
} ASNTYPE_ANY;

typedef struct tagASNTYPE_INTEGER
{
    BYTE    type;
    BYTE    length;
    BYTE    data;
} ASNTYPE_INTEGER;

typedef struct tagASNTYPE_IPADDRESS
{
    BYTE    type;
    BYTE    length;
    BYTE    data[4];
} ASNTYPE_IPADDRESS;

typedef struct tagASNTYPE_TIMETICKS
{
    BYTE    type;
    BYTE    length;
    BYTE    data[3];
} ASNTYPE_TIMETICKS;

typedef struct tagASNTYPE_ENTERPRISE_ID
{
    BYTE    type;
    BYTE    length;
    BYTE    data[8];
} ASNTYPE_ENTERPRISE_ID;

typedef struct tagASNTYPE_OBJECT_ID
{
    ASNTYPE_ENTERPRISE_ID enterprise;
    BYTE                  variable;
} ASNTYPE_OBJECT_ID;

typedef struct tagASNTYPE_PUBLIC_OCTET
{
    BYTE    type;
    BYTE    length;
    BYTE    data[6];
} ASNTYPE_PUBLIC_OCTET;


//***************************************************************************
// Defines
//***************************************************************************

#define WINSOCKVERSION	        0x0101

#define SNMP_TRAP_UDP_PORT      162
#define SNMP_TRAP_IPX_SOCKET    0x9010

#define SNMP_VERSION            0

#define SNMP_NORTONALERT_VAR_COMPNAME   1
#define SNMP_NORTONALERT_VAR_MESSAGE    2

#define SNMP_NORTONTRAP_CRITICAL        1
#define SNMP_NORTONTRAP_WARNING         2
#define SNMP_NORTONTRAP_INFORMATIONAL   3

#define MAX_COMPNAME_LEN        48
#define MAX_SNMP_MESSAGE_LEN    384

#define MAX_FORMAT_LEN          60


//***************************************************************************
// Local prototypes
//***************************************************************************

static int BuildSNMPTrapPDU ( CVirusAlert *lpcVirusAlert, char *packet );

static int BuildAlertMessage ( CVirusAlert *lpcVirusAlert, char *szMessage );

static int GetCompName ( CVirusAlert *lpcVirusAlert, char *szCompName );

static BYTE GetEventSeverityLevel ( CVirusAlert *lpcVirusAlert );

static int ASNSegmentLength ( int iDataSize );

static int InsertASNLength ( LPBYTE p, int iLength );

static int InsertSequence ( LPBYTE p, int iSequenceLength );

static int InsertObject ( LPBYTE p, void *pObject, int iObjectLen );

static int InsertASNOctetString ( LPBYTE p, char *szString, int iStringLen );

static int InsertStringVarSegment (
    LPBYTE            p,
    ASNTYPE_OBJECT_ID *pObjectID,
    char              *szString,
    int               iStringLen );


/////////////////////////////////////////////////////////////////////
// VOID CSNMPAlertQueueManager::ProcessWorkOrder(
//
// Purpose:
//    This function takes a work order and processes the SNMP alert
//
// Parameters:
//   CVirusAlert  *lpcAlert  - [in] - Information about the alert
//   CAlertTarget *lpcTarget - [in] - Where to send it.
//
// Returns:
//   none
//
// Notes:
//   We are responsible here for deleting the lpcTarget when we
//   are done with it.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/16/97 - DSACKIN - Function Created.
// 05/14/97 - GDZIECI - Implemented
/////////////////////////////////////////////////////////////////////
VOID CSNMPAlertQueueManager::ProcessWorkOrder(
   CVirusAlert  *lpcVirusAlert,         // [in] - Information about the alert
   CAlertTarget *lpcTarget)             // [in] - Where to send it.
{
	WSADATA		winsockData;
    SOCKADDR    remoteAddress;
    SOCKET      s;
    char        packet[512];
    int         iPacketLength;
    int         iErr;

    LogMessage( _T("ProcessWorkOrder for SNMP Alert.") );

	iErr = WSAStartup ( WINSOCKVERSION, &winsockData );

	if (0 == iErr)
	{
        ((CSNMPTarget *)lpcTarget)->GetAddress ( &remoteAddress );

		s = socket ( remoteAddress.sa_family, SOCK_DGRAM,
            remoteAddress.sa_family == AF_INET ? IPPROTO_UDP : NSPROTO_IPX );

		if (s != INVALID_SOCKET)
		{
            if (remoteAddress.sa_family == AF_INET)
			    ((LPSOCKADDR_IN)&remoteAddress)->sin_port = htons ( SNMP_TRAP_UDP_PORT );
            else
			    ((LPSOCKADDR_IPX)&remoteAddress)->sa_socket = htons ( SNMP_TRAP_IPX_SOCKET );

            iPacketLength = BuildSNMPTrapPDU ( lpcVirusAlert, packet );

            if (iPacketLength)
			    iErr = sendto ( s, packet, iPacketLength, 0, &remoteAddress,
                                sizeof(remoteAddress) );

            closesocket ( s );
        }

    WSACleanup();
    }


                                        // If we had alerts or targets, this is
                                        // the place to delete them.
   if (lpcVirusAlert)
   {
      delete lpcVirusAlert;
      lpcVirusAlert = NULL;
   }

   if (lpcTarget)
   {
      delete lpcTarget;
      lpcTarget = NULL;
   }



}


//***************************************************************************
// BuildSNMPTrapPDU()
//
// Description:
//   This function builds our SNMP trap PDU packet.
//
// Parameters:
//   CVirusAlert *lpcVirusAlert   [in]  event notification packet
//   char        *packet          [out] packet buffer to be filled out
//
// Return Value:
//   int    - returns the length of the built packet in bytes
//
// See Also:
//
//***************************************************************************
// 05/14/1997 GDZIECI Function Created.
//***************************************************************************

static int BuildSNMPTrapPDU (
   CVirusAlert *lpcVirusAlert,
   char *packet )
{
    char    szCompName[MAX_COMPNAME_LEN + 1];
    char    szMessage[MAX_SNMP_MESSAGE_LEN + 1];
    int     iMessageLen;
    int     iCompNameLen;
    int     iMsgSegmentLen;
    int     iVarSequenceLen;
    int     iTrapPDULen;
    int     iSNMPDataLen;
    int     iTotalPacketLen = 0;
    LPBYTE  p = (LPBYTE)packet;

    ASNTYPE_INTEGER      version   = {ASN_INTEGER, 1, SNMP_VERSION};
    ASNTYPE_INTEGER      generic   = {ASN_INTEGER, 1, SNMP_GENERICTRAP_ENTERSPECIFIC};
    ASNTYPE_INTEGER      specific  = {ASN_INTEGER, 1, SNMP_NORTONTRAP_CRITICAL};
    ASNTYPE_IPADDRESS    address   = {ASN_RFC1155_IPADDRESS, 4, 0, 0, 0, 0};
    ASNTYPE_TIMETICKS    time      = {ASN_RFC1155_TIMETICKS, 3, 0, 0, 0};
    ASNTYPE_PUBLIC_OCTET community = {ASN_OCTETSTRING, 6, 'p', 'u', 'b', 'l', 'i', 'c'};
    ASNTYPE_OBJECT_ID    object    =
    {
        {ASN_OBJECTIDENTIFIER, 8, 0x2b, 0x06, 0x01, 0x04, 0x01, 0x83, 0x09, 0x0b},
        SNMP_NORTONALERT_VAR_COMPNAME,
    };

    iMessageLen = BuildAlertMessage ( lpcVirusAlert, szMessage );

    if (iMessageLen)
        {
        iCompNameLen = GetCompName ( lpcVirusAlert, szCompName );

        iMsgSegmentLen = ASNSegmentLength ( iMessageLen ) +
            sizeof(ASNTYPE_OBJECT_ID);

        iVarSequenceLen = ASNSegmentLength ( iMsgSegmentLen ) +
            2 + ASNSegmentLength ( iCompNameLen ) + sizeof(ASNTYPE_OBJECT_ID);

        iTrapPDULen = ASNSegmentLength ( iVarSequenceLen ) +
            sizeof(ASNTYPE_TIMETICKS) + 2 * sizeof(ASNTYPE_INTEGER) +
            sizeof(ASNTYPE_IPADDRESS) + sizeof(ASNTYPE_ENTERPRISE_ID);

        iSNMPDataLen = ASNSegmentLength ( iTrapPDULen ) +
            sizeof(ASNTYPE_INTEGER) + sizeof(ASNTYPE_PUBLIC_OCTET);

        iTotalPacketLen = ASNSegmentLength ( iSNMPDataLen );

        p += InsertSequence ( p, iSNMPDataLen );

        p += InsertObject ( p, &version, sizeof(version) );

        p += InsertObject ( p, &community, sizeof(community) );

        *p++ = ASN_RFC1157_TRAP;
        p += InsertASNLength ( p, iTrapPDULen );

        p += InsertObject ( p, &object, sizeof(ASNTYPE_ENTERPRISE_ID) );

        p += InsertObject ( p, &address, sizeof(ASNTYPE_IPADDRESS) );

        p += InsertObject ( p, &generic, sizeof(generic) );

        specific.data = GetEventSeverityLevel ( lpcVirusAlert );
        p += InsertObject ( p, &specific, sizeof(specific) );

        p += InsertObject ( p, &time, sizeof(time) );

        p += InsertSequence ( p, iVarSequenceLen );

        object.enterprise.length += 1;
        p += InsertStringVarSegment ( p, &object, szCompName, iCompNameLen );

        object.variable = SNMP_NORTONALERT_VAR_MESSAGE;
        p += InsertStringVarSegment ( p, &object, szMessage, iMessageLen );
        }

    return iTotalPacketLen;
}


//***************************************************************************
// BuildAlertMessage()
//
// Description:
//   Builds our alert message to be sent in the trap to an SNMP management
// console.
//
// Parameters:
//   CVirusAlert *lpcVirusAlert   [in]  event notification packet
//   char        *szMessage       [out] message buffer to be filled out
//
// Return Value:
//   int - returns the length of the created message in bytes
//
// See Also:
//
//***************************************************************************
// 05/16/1997 GDZIECI Function Created.
//***************************************************************************

static int BuildAlertMessage ( CVirusAlert *lpcVirusAlert, char *szMessage )
{
    char szVirusName[SIZE_VIRUSNAME + 1];
    char szObjectName[SYM_MAX_PATH + 1];
    char szUserName[UNLEN + 1];
    char szFormat[MAX_FORMAT_LEN + 1];
    int  iMaxUserName;
    CString szAction;
    char    szActionBuffer[80];
    DWORD   dwAlertType;
    DWORD   dwAction;
    BOOL    bActionSuccess;
    UINT    uResId;

    int  iRet = 0;

    // use the ANSI version
    if (LoadStringA ( ghInst, IDS_SNMP_MSG_FORMAT, szFormat, MAX_FORMAT_LEN ))
    {
        // start with empty strings
        szVirusName[0]  = 0;
        szObjectName[0] = 0;
        szUserName[0]   = 0;
        szActionBuffer[0] = 0;

        lpcVirusAlert->GetVirusName ( SIZE_VIRUSNAME, szVirusName );
        lpcVirusAlert->GetObjectName ( SYM_MAX_PATH, szObjectName );

                                        // Explicitly call the A version of the
                                        // function!
        lpcVirusAlert->GetUserNameA ( UNLEN, szUserName );


        lpcVirusAlert->GetAlertType(
            dwAlertType);

        lpcVirusAlert->GetAction(
            dwAction);

        lpcVirusAlert->GetActionSuccess(
            bActionSuccess);

        uResId = ComposeActionString(
                                        dwAlertType,
                                        dwAction,
                                        bActionSuccess,
                                        &szAction );

        LoadStringA ( ghInst, uResId, szActionBuffer, sizeof(szActionBuffer) );


        // make sure our message fits within the PDU packet
        iMaxUserName = MAX_SNMP_MESSAGE_LEN - (strlen (szFormat ) - 6 +
            strlen ( szVirusName ) + strlen ( szObjectName ) +
            strlen ( szActionBuffer ) );

        // truncate the user name if necessary
        if (iMaxUserName < strlen ( szUserName ))
            szUserName[iMaxUserName] = 0;

        // we are dealing with ANSI strings only here
        iRet = sprintf ( szMessage, szFormat, szVirusName, szObjectName,
            szUserName, szActionBuffer );
    }

    return iRet;
}


//***************************************************************************
// GetCompName()
//
// Description:
//   Retrieves computer name from the alert notification packet.
//
// Parameters:
//   CVirusAlert *lpcVirusAlert [in]  event notification packet
//   char        *szCompName    [out] buffer to be filled out
//
// Return Value:
//   int - returns the length of the computer name in bytes
//
// See Also:
//
//***************************************************************************
// 05/16/1997 GDZIECI Function Created.
//***************************************************************************

static int GetCompName ( CVirusAlert *lpcVirusAlert, char *szCompName )
{
    lpcVirusAlert->GetSystemName ( MAX_COMPNAME_LEN, szCompName );
    return strlen ( szCompName );
}


//***************************************************************************
// GetEventSeverityLevel()
//
// Description:
//   Returns severity level of an alert (event).
//
// Parameters:
//   CVirusAlert *lpcVirusAlert   [in]  event notification packet
//
// Return Value:
//   BYTE - returns severity level
//
// See Also:
//
//***************************************************************************
// 05/16/1997 GDZIECI Function Created.
//***************************************************************************

static BYTE GetEventSeverityLevel ( CVirusAlert *lpcVirusAlert )
{
    // right now everything is critical
    return SNMP_NORTONTRAP_CRITICAL;
}


//***************************************************************************
// ASNSegmentLength()
//
// Description:
//   This function returns the length of a segment of data in ASN.1 encoding
// given the size of the data. The segment consists of an initial byte
// indicating data type, followed by the length of the data and then the data
// itself. The length field can be one byte long (if it is less than 128), or
// two bytes long otherwise.
//
// Parameters:
//   int iDataSize      [in] data size in bytes
//
// Return Value:
//   int - ASN.1 segment length in bytes
//
// See Also:
//
//***************************************************************************
// 05/16/1997 GDZIECI Function Created.
//***************************************************************************

static int ASNSegmentLength ( int iDataSize )
{
    return ((iDataSize > 127) ? (iDataSize + 3) : (iDataSize + 2));
}


//***************************************************************************
// InsertASNLength()
//
// Description:
//   This functions inserts a length field in ASN.1 encoding into a buffer.
// The length field can be one or two bytes long.
//
// Parameters:
//   LPBYTE p           [out] buffer where the length field is to be inserted
//   int    iLength     [in]  length to insert
//
// Return Value:
//   int - number of bytes inserted into the buffer
//
// See Also:
//
//***************************************************************************
// 05/16/1997 GDZIECI Function Created.
//***************************************************************************

static int InsertASNLength ( LPBYTE p, int iLength )
{
    int iRet = 1;

    if (iLength > 127)
    {
        *p++ = 0x80 | (iLength / 128);
        iRet = 2;
    }

    *p = (BYTE)iLength;

    return iRet;
}


//***************************************************************************
// InsertSequence()
//
// Description:
//   Inserts the heading of an ASN.1 sequence into a buffer.
//
// Parameters:
//   LPBYTE p               [out] buffer where the sequence is to be inserted
//   int    iSequenceLength [in]  sequence length
//
// Return Value:
//   int - number of bytes inserted into the buffer
//
// See Also:
//
//***************************************************************************
// 05/16/1997 GDZIECI Function Created.
//***************************************************************************

static int InsertSequence ( LPBYTE p, int iSequenceLength )
{
    int iLen;

    *p++ = ASN_SEQUENCE;

    iLen = InsertASNLength ( p, iSequenceLength );

    return (iLen + 1);
}


//***************************************************************************
// InsertObject()
//
// Description:
//   Inserts an object into a buffer.
//
// Parameters:
//   LPBYTE p           [out] buffer where the object is to be inserted
//   void   *pObject    [in]  object to insert
//   int    iObjectLen  [in]  object length in bytes
//
// Return Value:
//   int - number of bytes inserted into the buffer
//
// See Also:
//
//***************************************************************************
// 05/16/1997 GDZIECI Function Created.
//***************************************************************************

static int InsertObject ( LPBYTE p, void *pObject, int iObjectLen )
{
    memcpy ( p, pObject, iObjectLen );

    return iObjectLen;
}


//***************************************************************************
// InsertASNOctetString()
//
// Description:
//   Inserts a string as an ASN.1 Octet String object into a buffer.
//
// Parameters:
//   LPBYTE p            [out] buffer where the string is to be inserted
//   char   *szString    [in]  string to insert
//   int    iStringLen   [in]  string length
//
// Return Value:
//   int - number of bytes inserted into the buffer
//
// See Also:
//
//***************************************************************************
// 05/16/1997 GDZIECI Function Created.
//***************************************************************************

static int InsertASNOctetString ( LPBYTE p, char *szString, int iStringLen )
{
    int iLenSize;

    *p++ = ASN_OCTETSTRING;

    iLenSize = InsertASNLength ( p, iStringLen );
    p += iLenSize;

    memcpy ( p, szString, iStringLen );

    return (1 + iLenSize + iStringLen);
}


//***************************************************************************
// InsertStringVarSegment()
//
// Description:
//   Inserts a string variable as a segment of data into a buffer.
//
// Parameters:
//   LPBYTE            p            [out] buffer to insert the variable
//   ASNTYPE_OBJECT_ID *pObjectID   [in]  variable object ID in SNMP tree
//   char              *szString    [in]  string
//   int               iStringLen   [in]  string length
//
// Return Value:
//   int - number of bytes inserted into the buffer
//
// See Also:
//
//***************************************************************************
// 05/16/1997 GDZIECI Function Created.
//***************************************************************************

static int InsertStringVarSegment (
    LPBYTE            p,
    ASNTYPE_OBJECT_ID *pObjectID,
    char              *szString,
    int               iStringLen )
{
    int iLength;

    iLength = InsertSequence ( p,
        ASNSegmentLength ( iStringLen ) + sizeof(ASNTYPE_OBJECT_ID) );
    p += iLength;

    iLength += sizeof(ASNTYPE_OBJECT_ID);
    p += InsertObject ( p, pObjectID, sizeof(ASNTYPE_OBJECT_ID) );

    iLength += InsertASNOctetString ( p, szString, iStringLen );

    return iLength;
}

