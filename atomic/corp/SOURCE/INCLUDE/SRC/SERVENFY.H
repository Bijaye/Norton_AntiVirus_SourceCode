// Copyright 1993 Symantec Corporation
//***************************************************************************
//
// $Header:   S:/INCLUDE/VCS/servenfy.h_v   1.4   18 Jun 1997 21:20:02   DSACKIN  $
//
// Description: typedefs, defines, prototypes for servenfy.c
//
// See Also:
//
//***************************************************************************
// $Log:   S:/INCLUDE/VCS/servenfy.h_v  $
// 
//    Rev 1.4   18 Jun 1997 21:20:02   DSACKIN
// Added the ProcessCVirusAlert function.
//
//    Rev 1.3   24 Apr 1997 19:06:30   JBRENNA
// Tesla port: DSACKIN r1.1:
// Added a prototype for NotifyNAVNLMServer
//
//    Rev 1.2   08 Apr 1997 13:43:02   DALLEE
// Put #ifdef SYM_WIN32 around EnumNAVNLMServers().
//
//    Rev 1.1   18 Feb 1997 17:58:46   DALLEE
// Ported the following from Hydra (env LUIGI, stamp HYDRA.15):
//            Rev 1.5   13 Dec 1996 00:23:00   DSACKIN
//         Exported the enumeration function
//
//            Rev 1.4   13 Dec 1996 00:12:54   DSACKIN
//         Added a prototype of the Network NLM enumeration function.
//
//            Rev 1.3   16 Oct 1996 17:03:22   CCOTRIL
//         Ported to DX platform.
//
//            Rev 1.2   09 Oct 1996 10:25:54   CCOTRIL
//         Merged with Gryphon
//
//            Rev 1.2   20 Aug 1996 02:35:38   DSACKIN
//         Added prototypes for InitNLMMessenger and DeInitNLMMessenger.
//
//            Rev 1.1   17 May 1996 13:55:02   PVENKAT
//         Added new functions
//
//    Rev 1.0   06 Feb 1997 20:51:32   RFULLER
// Initial revision
//
//    Rev 1.2   22 Aug 1996 11:09:28   JBRENNA
// Port in changes from DOSNAV: JALLEE: Added defines for IPX packet
// handling structs for DX platform. Other platforms get these by
// nxt.h Novell include. Novdos libraries used by DOS are incompatible
// w/ DX platform.
//
//    Rev 1.1   17 May 1996 13:55:02   PVENKAT
// Added new functions
//
//
//    Rev 1.0   30 Jan 1996 15:47:32   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 10:11:56   BARRY
// Initial revision.
//
//    Rev 1.1   29 Nov 1994 11:30:22   MFALLEN
// Using the trunk.
//
//    Rev 1.0   31 Oct 1994 12:50:56   MFALLEN
// Initial revision.
//
//    Rev 1.2   18 Nov 1993 16:24:50   DALLEE
// delete TIMEOUT structure (now found in symtime.h)
//
//    Rev 1.1   20 Sep 1993 15:14:26   JMILLARD
// When sending SYM_NLM notifications, wait until the IO has completed the send.
//
//    Rev 1.0   12 Sep 1993 03:16:04   BRAD
// Initial revision.
//
//    Rev 1.0   12 Sep 1993 02:39:10   BRAD
// Initial revision.
//
//    Rev 1.1   12 Aug 1993 23:19:36   JMILLARD
// add GetPreferredConnectionId and GetFileServerName
//
//    Rev 1.0   12 Aug 1993 11:57:18   MFALLEN
// Initial Revision
//
//    Rev 1.1   23 Jul 1993 08:05:52   DALLEE
// fix DOS indirect prototypes, (they are not PASCAL)
//
//    Rev 1.0   09 Jul 1993 17:43:54   JMILLARD
// Initial revision.
//***************************************************************************


#ifndef _SERVENFY_H
#define _SERVENFY_H

#ifdef SYM_WIN32

#if !defined(__CALERT_H)
#   include "CAlert.h"
#endif // if !defined(__CAlert_H)

#if !defined(__CVALERT_H)
#   include "CVAlert.h"
#endif // if !defined(__CVAlert_H)

#endif


#ifdef _SERVENFY_C

//**********************************************************************
//
//  Overrides and definition relays
//
//**********************************************************************

#ifdef SYM_NLM

typedef IPX_ECB ECB;        // SYM_WIN and DOS call it ECB

#endif

typedef ECB FAR *LPECB;


#ifdef SYM_NLM

typedef IPX_HEADER IPXHEADER;   // SYM_NLM version

#else

typedef IPXHeader  IPXHEADER;   // DOS, Win16

#endif


//**********************************************************************
//
//  defines
//
//**********************************************************************

#define NetwareDll  "NWNETAPI.DLL"
#define IpxDll      "NWIPXSPX.DLL"

#define IPX_WORKSTATION_VIRUS   0x94

#ifdef SYM_DOS
// Various Novell defines
#define NWCC_NAME_FORMAT_BIND   2
#define NWCC_OPEN_UNLICENSED    2
#define NWCC_RESERVED           0
#define SUCCESS                 0
#define NWCC_INFO_SERVER_NAME   7       // ???++++
#endif  // SYM_DOS

// dynamically imported function names

#ifdef SYM_DOS
// Note: a number of the functions called by the windows version
//  are not present in the DOS version or are scheduled to be
//  no longer supported by Novell as of 1/97.  While adding DOS/DX
//  support, I have made sure the routines used will be supported
//  in the forseeable future.  However, somebody will need to rev
//  the windows code real soon now. (CWC)

#define NWCCOPENCONNBYNAME_NAME     "NWCCOpenConnByName"
#define NWSCANOBJECT_NAME           "NWScanObject"
#define NWREADPROPERTYVALUE_NAME    "NWReadPropertyValue"
#define NWCCCLOSECONN_NAME          "NWCCCloseConn"
#define IPXINITIALIZE_NAME          "IPXInitialize"
#define IPXDEINITIALIZE_NAME        "IPXSPXDeinit"
#define IPXSENDPACKET_NAME          "IPXSendPacket"
#define IPXGETLOCALTARGET_NAME      "IPXGetLocalTarget"
#define IPXRELINQUISHCONTROL_NAME   "IpxRelinquishControl"
#define NWCCGETPRIMCONNREF_NAME     "NWCCGetPrimConnRef"
#define NWCCGETCONNREFINFO_NAME     "NWCCGetConnRefInfo"
#define NWCCSCANCONNREFS_NAME       "NWCCScanConnRefs"

#else   // !SYM_DOS

#define SCANBINDERYOBJECT_NAME      "ScanBinderyObject"
#define READPROPERTYVALUE_NAME      "ReadPropertyValue"
#define IPXINITIALIZE_NAME          "IPXInitialize"
#define IPXDEINITIALIZE_NAME        "IPXSPXDeinit"
#define IPXSENDPACKET_NAME          "IPXSendPacket"
#define IPXGETLOCALTARGET_NAME      "IPXGetLocalTarget"
#define IPXRELINQUISHCONTROL_NAME   "IpxRelinquishControl"
#define GETPREFERREDCONNECTIONID_NAME "GetPreferredConnectionID"
#define GETFILESERVERNAME_NAME      "GetFileServerName"

#endif  // SYM_DOS

// Ipx initialization defines

#define MAX_IPX_ECBS    4

#define IPX_HEADER_SIZE         30
#define IPX_BODY_SIZE           546
#define MAX_IPX_PACKET_SIZE     576

#define ECB_AVAILABLE_FOR_USE   0

#define SEND_TIMEOUT            2000        // 2 seconds

//**********************************************************************
//
//  typedefs
//
//**********************************************************************

// indirect calls to Netware functions

#ifdef SYM_WIN

typedef WORD (WINAPI *SCANBINDERYOBJECT_FCN)
            ( char   far *searchObjectName,
              WORD        searchObjectType,
              DWORD  far *objectID,
              char   far *objectName,
              WORD   far *objectType,
              BYTE   far *objectHasProperties,
              BYTE   far *objectFlag,
              BYTE   far *objectSecurity );

typedef WORD (WINAPI* READPROPERTYVALUE_FCN)
            ( char   far *objectName,
              WORD        objectType,
              char   far *propertyName,
              WORD        segmentNumber,
              BYTE   far *propertyValue,
              BYTE   far *moreSegments,
              BYTE   far *propertyFlags );

typedef WORD (WINAPI* IPXINITIALIZE_FCN) (
    DWORD FAR *lpdwIpxTaskId,
    WORD      wMaxECBs,
    WORD      wMaxPackets );

typedef VOID (WINAPI* IPXDEINITIALIZE_FCN) ( DWORD );

typedef VOID (WINAPI* IPXRELINQUISHCONTROL_FCN) ( VOID );

typedef WORD (WINAPI* IPXGETLOCALTARGET_FCN) (
    DWORD      dwIpxTaskId,
    BYTE  FAR *lpbyNetworkAddress,
    BYTE  FAR *lpbyImmediateAddress,
    int   FAR *nTransportTime );

typedef WORD (WINAPI* IPXSENDPACKET_FCN) (
    DWORD     lpdwIpxTaskId,
    ECB   FAR *lpEcb );

typedef WORD (WINAPI* GETPREFERREDCONNECTIONID_FCN) (
    VOID );

typedef WORD (WINAPI* GETFILESERVERNAME_FCN)
            ( BYTE byConnectionId,
              char   far *szServerName );

#else
typedef WORD (FAR * NWCCOPENCONNBYNAME_FCN)
            ( DWORD       connHandle,
              char   far *name,
              WORD        nameFormat,
              WORD        openState,
              WORD        reserved,
              DWORD  far *pConnHandle);

typedef WORD (FAR * NWCCCLOSECONN_FCN)
            ( DWORD       connHandle);

typedef WORD (FAR * NWSCANOBJECT_FCN)
            ( DWORD       connHandle,
              char   far *searchObjectName,
              WORD        searchObjectType,
              DWORD  far *objectID,
              char   far *objectName,
              WORD   far *objectType,
              BYTE   far *objectHasProperties,
              BYTE   far *objectFlag,
              BYTE   far *objectSecurity );

typedef WORD (FAR * NWREADPROPERTYVALUE_FCN)
            ( DWORD       connHandle,
              char   far *objectName,
              WORD        objectType,
              char   far *propertyName,
              WORD        segmentNumber,
              BYTE   far *propertyValue,
              BYTE   far *moreSegments,
              BYTE   far *propertyFlags );


typedef WORD (FAR * IPXINITIALIZE_FCN) ( VOID );

typedef VOID (FAR * IPXDEINITIALIZE_FCN) ( VOID );

typedef VOID (FAR * IPXRELINQUISHCONTROL_FCN) ( VOID );

typedef WORD (FAR * IPXSENDPACKET_FCN) (
    ECB   FAR *lpEcb );

typedef WORD (FAR * IPXGETLOCALTARGET_FCN) (
    BYTE  FAR *lpbyNetworkAddress,
    BYTE  FAR *lpbyImmediateAddress,
    int   FAR *nTransportTime );

typedef WORD (FAR * GETPREFERREDCONNECTIONID_FCN) (
    VOID );

typedef WORD (FAR * GETFILESERVERNAME_FCN)
            ( BYTE byConnectionId,
              char   far *szServerName );

typedef WORD (FAR * NWCCGETPRIMCONNREF_FCN)
            ( DWORD  far *connRef );

typedef WORD (FAR * NWCCGETCONNREFINFO_FCN)
            ( DWORD      connRef,
              WORD       infoType,
              WORD       len,
              BYTE   far *buffer );

typedef WORD (FAR * NWCCSCANCONNREFS_FCN)
            ( DWORD  far *iterator,
              DWORD  far *connRef );

#endif


typedef struct tagIPXECBLIST             // complete declaration
    {

    ECB        Ecb;

    IPXHEADER  IpxHeader;
    BYTE       byIpxMessage[IPX_BODY_SIZE];

    } IPXECBLIST;

typedef IPXECBLIST FAR *LPIPXECBLIST;


#endif  // _SERVENFY_C


//**********************************************************************
//
//  prototypes
//
//**********************************************************************

#ifdef __cplusplus
extern "C" {
#endif

WORD SYM_EXPORT WINAPI NotifyServersOfVirus(
    LPSTR   szVirusName,
    LPSTR   szUserName,
    LPSTR   szPath
    );

BOOL SYM_EXPORT WINAPI  GetNWServerName (LPTSTR lpszNLMServer);
BOOL SYM_EXPORT WINAPI  WriteNWServerName (LPTSTR lpszNLMServer);

#ifdef __cplusplus
}
#endif

                                        // New API for notifying NLM servers.
BOOL SYM_EXPORT NotifyNAVNLMServer (
   LPSTR lpszVirus,                     // [in] - Virus name
   LPSTR lpszFile,                      // [in] - File containing the virus
   LPSTR szUser,                        // [in] - User who discovered the virus
   LPSTR szServer);                     // [in] - Server to notify.

                                        // Functions for our CNLMMessenger
                                        // handling
BOOL SYM_EXPORT WINAPI InitNLMMessenger (VOID);
VOID SYM_EXPORT WINAPI DeInitNLMMessenger (VOID);

/////////////////////////////////////////////////////////////////////
// This is where we will take care of the NLM server enumerator.


                                        // The NLM Enumeration Proc is where
                                        // data gets sent back to the calling
                                        // application.  The data is only valid
                                        // for the duration of the function call
                                        // and should be copied elsewhere if
                                        // needed for future reference.

                                        // The function should return false if
                                        // it doesn't want any more information
typedef BOOL (*NAVNLMENUMERATIONPROC)(LPSTR lpszServerName, LPVOID lpAddress, LPVOID lpExtra);

#ifdef SYM_WIN32

                                        // This is the function the application
                                        // should call to begin the enumerations
BOOL SYM_EXPORT WINAPI EnumNAVNLMServers(
   NAVNLMENUMERATIONPROC lpfnEnumProc,  // Callback procedure
   BOOL                  bReturnAddresses, // Should we get addresses?
   LPNETRESOURCE         lpCurrentResource,   // Always start with NULL!
   LPVOID                lpExtra);      // Will be passed back to callback.

#endif



/////////////////////////////////////////////////////////////////////
// Here is the function for the 95 clients to send our new alerts

#ifdef SYM_WIN32

BOOL SYM_EXPORT WINAPI ProcessCVirusAlert(
   CVirusAlert *lpcVirusAlert);         // [in] - Alert Information

#endif


#endif _SERVENFY_H

