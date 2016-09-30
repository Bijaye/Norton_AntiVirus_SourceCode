// Copyright 1993 Symantec Corporation
//***************************************************************************
//
// $Header:   S:/NAVSERVE/VCS/servenfy.cpv   1.3   18 Jun 1997 21:19:18   DSACKIN  $
//
// Description:
//
// Contains:
//
// See Also:
//
//***************************************************************************
// $Log:   S:/NAVSERVE/VCS/servenfy.cpv  $
// 
//    Rev 1.3   18 Jun 1997 21:19:18   DSACKIN
// Switched the hInstance variables to AfxGetModuleInstance() call.
//
//    Rev 1.2   18 Apr 1997 14:01:44   DSACKIN
// Ported from Tesla
//
//    Rev 1.0   13 Mar 1997 15:14:04   RFULLER
// Initial revision
//
//    Rev 1.2   18 Nov 1996 01:26:56   DSACKIN
// Fixed the prototype for NotifyNAVNLMServer function.
//
//    Rev 1.1   18 Nov 1996 01:21:26   DSACKIN
// Added the NotifyNAVNLMServer function to allow applications to notify
// servers without going through the usual DAT file.
//
//    Rev 1.0   08 Nov 1996 11:53:28   DKEEFER
// Initial revision
//
//    Rev 1.8   20 Aug 1996 02:31:04   DSACKIN
// Re-Worked Server Notification functions to use our new CNLMMessenger object.
//
//    Rev 1.7   31 Jul 1996 14:59:52   PVENKAT
// Fixed #62681,  62618.  Now we make use of RES_SOFT_SEARCH instead of
// RES_MULTIPLE in finding IPX address of the given server.  This takes much
// less time (200 milli secs) compared to earlier (7.5 secs).
//
//    Rev 1.6   25 Jul 1996 15:53:40   PVENKAT
// Removed hard-coded breakpoint.
//
//    Rev 1.5   25 Jul 1996 13:55:26   PVENKAT
// If 'All' is selected, now enumerating all servers and sending msgs if NLM
// is running on that.
//
//    Rev 1.4   28 Jun 1996 09:55:30   PVENKAT
// Modified WriteNWServerName to return FALSE on Readonly or such cases too.
//
//
//    Rev 1.3   05 Jun 1996 16:57:24   PVENKAT
// Fixed bugs: Always initialise the servername to EOS, so if not exist we
//                 have valid string.
//                 if no server exists, then simply return EOS.
//
//    Rev 1.2   17 May 1996 18:32:28   PVENKAT
// Fixed a problem (of not getting socket in one case).
//
//    Rev 1.1   17 May 1996 13:54:00   PVENKAT
// Added functions to read/write a DAT file for NW Server name and
// send message to stored server or all servers. (WINSOCK).
//
//    Rev 1.0   30 Jan 1996 15:54:26   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:40:06   BARRY
// Initial revision.
//
//    Rev 1.5   29 Nov 1994 11:29:04   MFALLEN
// Yeah! It's using the trunk in the W16 project area.
//
//    Rev 1.2   18 Nov 1993 16:25:40   DALLEE
// change InitializeTimeout and TimedOut to match symtime.h prototypes
// (we should get them from the DLL, but that was too much overhead)
//
//    Rev 1.1   20 Sep 1993 15:11:14   MFALLEN
// John Millard made some modification to wait until the SYM_NLM notification IO
// is completed.
//
//    Rev 1.0   12 Sep 1993 02:39:08   BRAD
// Initial revision.
//
//    Rev 1.7   09 Sep 1993 23:30:08   DALLEE
// Renamed NW3_GetConnectionNumber.  Name conflict with SYMNET.LIB.
//
//    Rev 1.6   09 Sep 1993 15:21:22   DALLEE
// Turn off optimizations around inline assembly.
//
//    Rev 1.5   09 Sep 1993 14:03:08   CGUC
// We now free the Ecb memory that was allocated so the NAVTSRW
// doesn't crash on the second send message.  Memory was allocated
// under context of different program so when that program
// exited, the memory was freed.
// Also added cover (no code) for UnInitIpx( );
//
//    Rev 1.4   09 Sep 1993 13:45:22   JMILLARD
// add check for shell loaded before trying to notify
//
//    Rev 1.3   07 Sep 1993 22:36:00   DALLEE
// Check error code before setting static IpxInitialized.
//
//    Rev 1.2   23 Aug 1993 00:39:30   BARRY
// Added MODULE_NAME and fixed MemUnlock arguments for VMM debugging
//
//    Rev 1.1   12 Aug 1993 23:17:52   JMILLARD
// change to notify only one server
// fix bug in InitIpx that caused notify to work the first time
//
//
//    Rev 1.0   12 Aug 1993 11:57:08   MFALLEN
// Initial Revision
//
//    Rev 1.4   23 Jul 1993 06:36:46   JMILLARD
// fix uninitialized wErr for Dos
//
//    Rev 1.3   15 Jul 1993 11:05:28   MFALLEN
// added ifdef for DOS
//
//    Rev 1.2   15 Jul 1993 08:13:28   MFALLEN
// Added an if () statement to verify that the pointers to functions are valid
// before calling them.
//
//    Rev 1.1   09 Jul 1993 17:50:54   JMILLARD
// fix DOS compile problem
// null terminate message, so old Nlm's don't explode
//
//    Rev 1.0   09 Jul 1993 17:43:44   JMILLARD
// Initial revision.
//***************************************************************************

#include "platform.h"                   // Platform independence
#include "symnet.h"
#include "sympipe.h"
#include "symtime.h"

#include "navutil.h"
#include "xapi.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SYM_NLM

#include <nwipxspx.h>       // API prototypes
#include <errno.h>          // ESUCCESS
#include <niterror.h>       // ipx and spx errors
#include <nwmisc.h>         // IntSwap
#include <nwsemaph.h>       // OpenLocalSemaphore, CloseLocalSemaphor
#include <nwbindry.h>       // OT_USER, OT_SERVER
#include <nwconn.h>         // GetObjectConnectionNumbers, GetInternetAddress
#include <nwenvrn.h>        // GetFileServerName

#else       // SYM_WIN or DOS

#ifdef SYM_WIN
    #include <nxtw.h>       // API prototypes
#endif

#ifdef SYM_DOS
    #include <nxtd.h>
#endif

#include <string.h>
//#include <nit.h>            // IntSwap
#include <nwbindry.h>       // ScanBinderyObject
#include <undoc.h>         // For Dos3Call

#endif

#ifdef	SYM_WIN32
    #include	<winsock.h>
    #include	<nspapi.h>
    #include	<wsipx.h>
    #include	<svcguid.h>
#endif

#ifdef __cplusplus
}
#endif

#define _SERVENFY_C
#include "servenfy.h"
#include "navserid.h"

#ifdef   SYM_WIN32
    #include "nlmmsgr.h"                // For CNLMMessenger object.
#endif

#define NO_TIMEOUT              0       // 0 is no timeout

VOID SYM_EXPORT WINAPI InitializeTimeout(
    LPTIMEOUT lpTimeout,
    DWORD dwTimeout );

BOOL SYM_EXPORT WINAPI TimedOut(
    LPTIMEOUT lpTimeout );

BOOL NotifyNLMServer (LPTSTR szVirus, LPTSTR szFile, LPTSTR szUser);

BOOL NotifyNLMServers (LPTSTR lpszVirus, LPTSTR lpszFile, LPTSTR szUser);

MODULE_NAME;

//**********************************************************************
//
//  static data
//
//**********************************************************************

// pointers to all the dynamically loaded functions

STATIC SCANBINDERYOBJECT_FCN    lpfnScanBinderyObject   = NULL;
STATIC READPROPERTYVALUE_FCN    lpfnReadPropertyValue   = NULL;
STATIC IPXINITIALIZE_FCN        lpfnIpxInitialize       = NULL;
STATIC IPXDEINITIALIZE_FCN      lpfnIpxDeinitialize     = NULL;
STATIC IPXRELINQUISHCONTROL_FCN lpfnIpxRelinquishControl= NULL;
STATIC IPXSENDPACKET_FCN        lpfnIpxSendPacket       = NULL;
STATIC IPXGETLOCALTARGET_FCN    lpfnIpxGetLocalTarget   = NULL;
STATIC GETFILESERVERNAME_FCN    lpfnGetFileServerName   = NULL;
STATIC GETPREFERREDCONNECTIONID_FCN
                                lpfnGetPreferredConnectionId
                                                        = NULL;

STATIC DWORD                    dwIpxTaskId;
STATIC BOOL                     bIpxInitialized = FALSE;

/////////////////////////////////////////////////////////////////////
// Global Data

#ifdef SYM_WIN32
PCNLMMessenger pMessenger = NULL;
#endif

#ifdef SYM_WIN

//***************************************************************************
// PickUpMessageTraffic()
//
// Description: runs peek message for the pipe window, and lets IPX run
//
// Parameters:
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 09/09/1993 JMILLARD Function Created.
//***************************************************************************

VOID PickUpMessageTraffic( HWND hWnd )
{

#ifdef SYM_WIN

    MSG     msg;

    // actually PeekMessage until the queue is empty - when things are
    // really busy, there may be several things that need to processed -
    // just changed if (PeekMessage to while (PeekMessage

    while (PeekMessage( &msg, hWnd, 0, 0, PM_REMOVE))
        {

        TranslateMessage( &msg );
        DispatchMessage( &msg );

        }

#endif

    if (lpfnIpxRelinquishControl)
        {
        (*lpfnIpxRelinquishControl)( );
        }

}



//**********************************************************************
//
//  Description: Waits on an ECB to complete
//
//**********************************************************************
//
//  06dec92 jdm - New
//
//**********************************************************************

WORD SYM_EXPORT WINAPI WaitOnECB(       // wait on ECB
    LPIPXECBLIST lpECB,             // ECB to wait on
    DWORD        dwTimeout )        // how long to wait


{
    WORD    wErr = NOERR;


#ifdef SYM_NLM

    (void) dwTimeout;

    while (lpECB->Ecb.status != ECB_AVAILABLE_FOR_USE &&
           lpECB->Ecb.status != STS_SPX_EVENT_CANCELLED)
        {
                                        // check for high bit on (error)
                                        // implementation dependent!
                                        // in NetWare 3.11 status is a ushort
        if (lpECB->Ecb.status & 0x8000)
            break;

        RelinquishControl( );
        }

#else

    TIMEOUT Timeout;                // used to time out operation


    // wait on something to show up - the window will post it

    InitializeTimeout( &Timeout, dwTimeout );
    wErr = NP_ERR_OPERATION_TIMED_OUT;

    // do at least one PeekMessage/IpxRelinquish so that any posted
    // stuff gets processed right now - otherwise posted messages
    // may end up hanging around for a while and will get processed at
    // inopportune times - like after the pipe is closed

    do
        {

        PickUpMessageTraffic( NULL );       // service all windows

        } while (lpECB->Ecb.inUseFlag != ECB_AVAILABLE_FOR_USE &&
           !TimedOut( &Timeout ) );

    if (lpECB->Ecb.inUseFlag == ECB_AVAILABLE_FOR_USE)
        {
        wErr = NOERR;
        }

#endif

    return (wErr);
}

#endif



//***************************************************************************
// PutMessageByte()
//
// Description: Put byte in message, bump length and pointer
//
// Parameters:
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 07/09/1993 JMILLARD Function Created.
//***************************************************************************

VOID WINAPI PutMessageByte( LPBYTE FAR *lplpMessage,
                     LPWORD lpwMessageLen,
                     BYTE byData )
{
    **lplpMessage   = byData;
    *lplpMessage   += 1;
    *lpwMessageLen += 1;
}


//***************************************************************************
// PutMessageString()
//
// Description: Put string in message, terminate with ;,
//              bump length and pointer
//
// Parameters:
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 07/09/1993 JMILLARD Function Created.
//***************************************************************************

VOID WINAPI PutMessageString( LPBYTE FAR *lplpMessage,
                       LPWORD lpwMessageLen,
                       LPSTR szData )
{
    WORD    wStrLen = STRLEN( szData );

    MEMCPY( *lplpMessage, szData, wStrLen );

    *lplpMessage   += wStrLen;
    *lpwMessageLen += wStrLen;

    PutMessageByte( lplpMessage, lpwMessageLen, ';' );
}


//------------------------------------------------------------------------
// NW3_GetConnectionNumber - borrowed from symnet (Thanks, Doug Neal)
//
// This function is called to retrieve the current workstation connection
// number.
//
// Supports NetWare 1000
//
// Returns: DWORD connection number
//------------------------------------------------------------------------
#pragma optimize("", off)

DWORD SYM_EXPORT WINAPI ServenfyNW3_GetConnectionNumber()
    {
    auto    WORD    wID;
#ifndef	SYM_WIN32
    _asm {
        push    si
        push    di
        push    es
        push    ds
        push    bp

        mov     cl, 'X'
        mov     ax,0DC00h

#ifdef  SYM_WIN
        call    DOS3Call
#else
        int     21h
#endif

        cmp     ah, 0DCh
        jne     all_is_well
                                        ; Call failed, clear the 'ah'
                                        ; just in case
        xor     ah, ah
        jmp     GREATER_255

    all_is_well:

        cmp     cl, 'X'
        je      GREATER_255
        xor     ah, ah

    GREATER_255:

        pop     bp
        pop     ds
        pop     es
        pop     di
        pop     si

        mov     [wID],ax
        }

#endif
    return ((DWORD)wID);

    }

#pragma optimize("", on)



//***************************************************************************
// InitIpx()
//
// Description: Initialize Ipx
//
// Parameters: None
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 07/09/1993 JMILLARD Function Created.
//***************************************************************************

WORD WINAPI InitIpx( VOID )
{
    WORD    wErr = NOERR;

    if (!bIpxInitialized)
        {

#ifdef SYM_WIN

        // initialize ipx

        dwIpxTaskId = 0;                // set resources on application basis
        if (lpfnIpxInitialize)
            {
            wErr = (*lpfnIpxInitialize)
                        (&dwIpxTaskId, MAX_IPX_ECBS, MAX_IPX_PACKET_SIZE );
            }
#else

        // initialize ipx

        if (lpfnIpxInitialize)
            {
            wErr = (*lpfnIpxInitialize)( );
            }
#endif

        if (NOERR == wErr)
            {
            bIpxInitialized = TRUE;
            }
        }

    return (wErr);

}


//***************************************************************************
// UnInitIpx()
//
// Description: DeInitialize Ipx
//
// Parameters: None
//
// Return Value:
//
// See Also:
//
//***************************************************************************

VOID UnInitIpx( VOID )
{

#ifdef SYM_WIN

    if (lpfnIpxDeinitialize)
        {
        (*lpfnIpxDeinitialize)( dwIpxTaskId );
        }

#else

    if (lpfnIpxDeinitialize)
        {
        (*lpfnIpxDeinitialize)( );
        }

#endif

    bIpxInitialized = FALSE;

}



//***************************************************************************
// DynamicallyLoadNetworkLibrary()
//
// Description: Load the required network libraries, and set up the
//              entry points
//
// Parameters:
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 07/09/1993 JMILLARD Function Created.
//***************************************************************************

WORD WINAPI DynamicallyLoadNetworkLibrary( VOID )
{

    STATIC BOOL     bNetworkDllLoaded = FALSE;
    WORD            wErr = NOERR;


    if (!bNetworkDllLoaded)
        {

#ifdef SYM_WIN

        OFSTRUCT    ofstruct;
        HINSTANCE   hNetwareInst;
        HINSTANCE   hIpxInst;

        // verify the needed dll's exist, and load the dll

        if (OpenFile (NetwareDll, &ofstruct, OF_EXIST) != HFILE_ERROR)
            {
                                        // Module exists, perform load
                                        // of it into memory...
            hNetwareInst = LoadLibrary (NetwareDll);

                                        // Check for error loading...
#ifdef	SYM_WIN32
            if (hNetwareInst == NULL)
#else
            if (hNetwareInst < HINSTANCE_ERROR)
#endif
                {
                hNetwareInst = NULL;
                wErr = NP_ERR_INIT_DLL_NOT_FOUND;
                }
            }
        else
            {
            wErr = NP_ERR_INIT_DLL_LOAD_ERROR;
            }

        if (OpenFile (IpxDll, &ofstruct, OF_EXIST) != HFILE_ERROR)
            {
                                        // Module exists, perform load
                                        // of it into memory...
            hIpxInst = LoadLibrary (IpxDll);

                                        // Check for error loading...
#ifdef	SYM_WIN32
            if (hIpxInst == NULL)
#else
            if (hIpxInst < HINSTANCE_ERROR)
#endif
                {
                hIpxInst = NULL;
                wErr = NP_ERR_INIT_DLL_NOT_FOUND;
                }
            }
        else
            {
            wErr = NP_ERR_INIT_DLL_LOAD_ERROR;
            }

#endif  // ifdef SYM_WIN

        if (wErr == NOERR)
            {

#ifdef SYM_WIN

            // dynamically link library procedures

            lpfnScanBinderyObject = (SCANBINDERYOBJECT_FCN)
                        GetProcAddress( hNetwareInst,SCANBINDERYOBJECT_NAME );
            lpfnReadPropertyValue = (READPROPERTYVALUE_FCN)
                        GetProcAddress( hNetwareInst,READPROPERTYVALUE_NAME );
            lpfnIpxInitialize     = (IPXINITIALIZE_FCN)
                        GetProcAddress( hIpxInst,IPXINITIALIZE_NAME );
            lpfnIpxDeinitialize   = (IPXDEINITIALIZE_FCN)
                        GetProcAddress( hIpxInst,IPXDEINITIALIZE_NAME );
            lpfnIpxRelinquishControl = (IPXRELINQUISHCONTROL_FCN)
                        GetProcAddress( hIpxInst,IPXRELINQUISHCONTROL_NAME );
            lpfnIpxSendPacket     = (IPXSENDPACKET_FCN)
                        GetProcAddress( hIpxInst,IPXSENDPACKET_NAME );
            lpfnIpxGetLocalTarget = (IPXGETLOCALTARGET_FCN)
                        GetProcAddress( hIpxInst,IPXGETLOCALTARGET_NAME );
            lpfnGetFileServerName = (GETFILESERVERNAME_FCN)
                        GetProcAddress( hNetwareInst,GETFILESERVERNAME_NAME );
            lpfnGetPreferredConnectionId = (GETPREFERREDCONNECTIONID_FCN)
                        GetProcAddress( hNetwareInst,GETPREFERREDCONNECTIONID_NAME );

#else

            // set up pointers to the library functions

            lpfnScanBinderyObject = (SCANBINDERYOBJECT_FCN)
                        &ScanBinderyObject;
            lpfnReadPropertyValue = (READPROPERTYVALUE_FCN)
                        &ReadPropertyValue;
            lpfnIpxInitialize     = (IPXINITIALIZE_FCN)
                        &IPXInitialize;

            // no deinitialize for dos

            lpfnIpxRelinquishControl = (IPXRELINQUISHCONTROL_FCN)
                        &IPXRelinquishControl;
            lpfnIpxSendPacket     = (IPXSENDPACKET_FCN)
                        &IPXSendPacket;
            lpfnIpxGetLocalTarget = (IPXGETLOCALTARGET_FCN)
                        &IPXGetLocalTarget;
            lpfnGetFileServerName = (GETFILESERVERNAME_FCN)
                        &GetFileServerName;
            lpfnGetPreferredConnectionId = (GETPREFERREDCONNECTIONID_FCN)
                        &GetPreferredConnectionID;

#endif
            // verify all the functions arrived

            if (!lpfnIpxInitialize        ||
                !lpfnIpxRelinquishControl ||
                !lpfnIpxSendPacket        ||
                !lpfnScanBinderyObject    ||
                !lpfnIpxGetLocalTarget    ||
                !lpfnReadPropertyValue    ||
                !lpfnGetFileServerName    ||
                !lpfnGetPreferredConnectionId )
                {
                wErr = ERR;
                }
            }

#ifdef SYM_WIN
#ifdef	SYM_WIN32
        if (hNetwareInst != NULL && hIpxInst != NULL)
#else
        if (hNetwareInst > HINSTANCE_ERROR && hIpxInst > HINSTANCE_ERROR)
#endif
            bNetworkDllLoaded = TRUE;
#else
        bNetworkDllLoaded = TRUE;
#endif
        }

    return (wErr);
}


//***************************************************************************
// AllocInitEcb()
//
// Description: Allocate and initialize an Ipx ECB
//
// Parameters:
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 07/09/1993 JMILLARD Function Created.
//***************************************************************************

WORD WINAPI AllocInitEcb(LPHGLOBAL lphEcb, LPIPXECBLIST FAR *lplpEcb)
{
    WORD            wErr = NOERR;

    HGLOBAL         hEcb = NULL;
    LPIPXECBLIST    lpEcb = NULL;


    // allocate memory for an IPX ECB

    hEcb = MemAlloc( GMEM_FIXED, sizeof(IPXECBLIST) );
    if (hEcb == NULL)
        {
        wErr = NP_ERR_INIT_NO_MEM;
        goto BailOut;
        }

    lpEcb = (LPIPXECBLIST) MemLock( hEcb );
    if (lpEcb == NULL)
        {
        wErr = NP_ERR_INIT_NO_LOCK;
        goto BailOut;
        }

#ifdef SYM_WIN

    // lock so it can't be paged out - it will be used at the
    // interrupt level - not very Windows like, but necessary

    wErr = MemPageLock( hEcb ) + 0;
    if (wErr == NO_PAGE_LOCK)
        {
        wErr = NP_ERR_INIT_NO_LOCK;
        goto BailOut;
        }
    else
        {
        wErr = NOERR;
        }
#endif

    // initialize the ECB and Ipx packet itself

#ifdef SYM_NLM

    lpEcb->Ecb.queueHead     = NULL;
    lpEcb->Ecb.status        = 0;
    lpEcb->Ecb.semHandle     = 0;
    lpEcb->Ecb.socket        = 0;

    lpEcb->IpxHeader.destSocket = 0;

    lpEcb->Ecb.fragCount     = 2;

    lpEcb->Ecb.fragList[0].fragAddress =
        &lpEcb->IpxHeader;
    lpEcb->Ecb.fragList[0].fragSize =
        IPX_HEADER_SIZE;

    lpEcb->Ecb.fragList[1].fragAddress =
        &lpEcb->byIpxMessage;
    lpEcb->Ecb.fragList[1].fragSize =
        IPX_BODY_SIZE;

#else   // SYM_WIN or DOS

    lpEcb->Ecb.linkAddress   = NULL;
    lpEcb->Ecb.ESRAddress    = NULL;
    lpEcb->Ecb.inUseFlag     = 0;
    lpEcb->Ecb.socketNumber  = 0;

    lpEcb->Ecb.fragmentCount = 2;

    lpEcb->Ecb.fragmentDescriptor[0].address =
        &lpEcb->IpxHeader;
    lpEcb->Ecb.fragmentDescriptor[0].size =
        IPX_HEADER_SIZE;

    lpEcb->Ecb.fragmentDescriptor[1].address =
        &lpEcb->byIpxMessage;
    lpEcb->Ecb.fragmentDescriptor[1].size =
        IPX_BODY_SIZE;

#endif

BailOut:
    if (wErr)
        {
        hEcb = NULL;
        }

    *lphEcb = hEcb;
    *lplpEcb = lpEcb;

    return (wErr);
}


//***************************************************************************
// FreeEcb()
//
// Description: Free an Ipx ECB
//
// Parameters:
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 07/09/1993 JMILLARD Function Created.
//***************************************************************************

WORD WINAPI FreeEcb(HGLOBAL hEcb)
{
    WORD            wErr = NOERR;
    LPVOID          lpJunk = NULL;


    MemPageUnlock( hEcb );
    MemUnlock( hEcb, lpJunk );
    MemFree( hEcb );

    return (wErr);
}


//***************************************************************************
// NotifyServerOfVirus()
//
// Description: Notifies specified server of virus detected on workstation
//
// Parameters:
//
// Return Value: ERR if server not advertising, or error occurred
//
// See Also:
//
//***************************************************************************
// 07/06/1993 JMILLARD Function Created.
//***************************************************************************

WORD SYM_EXPORT WINAPI NotifyServerOfVirus(
    LPSTR           szServerName,
    LPSTR           szIpxMessage,
    LPIPXECBLIST    lpEcb,
    WORD            wMessageLen )
{
    WORD    wErr = NOERR;

    DWORD   dwObjectId = 0xffffffff;
    WORD    wObjectType;
    BYTE    szObjectHasProperties;
    BYTE    szObjectFlag;
    BYTE    szObjectSecurity;
    BYTE    bMoreSegments;
    BYTE    byPropertyFlags;

    BYTE    szLocalServerName[NETMAXSERVERNAME];

    DWORD   dwErr = 0;
    int     nTransmitTime;
    WORD    wIndex = 0;
    BYTE    byIpxServerAddress[128];


    // notify specified server

    dwObjectId = 0xffffffff;


    // see if server is running SYM_NLM

    if ((*lpfnScanBinderyObject)( (LPSTR) szServerName,
                                  OT_NAV_IPX_SERVER,
                                  &dwObjectId,
                                  (LPSTR) szLocalServerName,
                                  &wObjectType,
                                  &szObjectHasProperties,
                                  &szObjectFlag,
                                  &szObjectSecurity) == NOERR)
        {

        // get the internet address

        wErr = (*lpfnReadPropertyValue)( (LPSTR) szServerName,
                                        OT_NAV_IPX_SERVER,
                                        (LPSTR) "NET_ADDRESS",
                                        1,
                                        (LPBYTE) byIpxServerAddress,
                                        (LPBYTE) &bMoreSegments,
                                        (LPBYTE) &byPropertyFlags );

        // get the routing address

#ifdef SYM_WIN

        wErr = (*lpfnIpxGetLocalTarget)( dwIpxTaskId,
                                        byIpxServerAddress,
                                        lpEcb->Ecb.immediateAddress,
                                        &nTransmitTime );

#else
        wErr = (*lpfnIpxGetLocalTarget)( byIpxServerAddress,
                                        lpEcb->Ecb.immediateAddress,
                                        &nTransmitTime );

#endif

#ifdef SYM_NLM

        // set up destination address

        MEMCPY( &lpEcb->IpxHeader.destNet,
                byIpxServerAddress,
                12 );

        // set up message

        lpEcb->Ecb.fragCount = 2;
        lpEcb->Ecb.queueHead = &lpSendQueue;

        lpEcb->IpxHeader.connectionCtl |=  IPX_END_OF_MESSAGE;  // send it now
        lpEcb->IpxHeader.dataStreamType = IPX_STREAM_DATA;

        // address of data to send

        lpEcb->Ecb.fragList[1].fragSize = wMessageLen;

        nErr = IpxSendPacket( dwIpxTaskId, &lpEcb->Ecb );

#else           // SYM_WIN or DOS

        MEMCPY( &lpEcb->IpxHeader.destination,
                byIpxServerAddress,
                12 );

        MEMCPY( &lpEcb->Ecb.socketNumber,
                lpEcb->IpxHeader.destination.socket,
                2 );

        lpEcb->Ecb.fragmentCount = 2;

        lpEcb->IpxHeader.packetType = 4;        //  IPX

        // address of data to send

        lpEcb->Ecb.fragmentDescriptor[1].size = wMessageLen;

        // ship it!

#ifdef SYM_WIN
        (*lpfnIpxSendPacket)( dwIpxTaskId, &lpEcb->Ecb );
        WaitOnECB( lpEcb, SEND_TIMEOUT );   // wait until it finishes
#else
        (*lpfnIpxSendPacket)( &lpEcb->Ecb );
#endif


#endif
        }
    else
        {
        wErr = ERR;                     // server not present
        }

    return (wErr);

}


//***************************************************************************
// NotifyServersOfVirus()
//
// Description: Notifies servers of virus detected on workstation
//
// Parameters:
//
// Return Value: NOERR if a server was apparently notified
//
// See Also:
//
//***************************************************************************
// 07/06/1993 JMILLARD Function Created.
//***************************************************************************

WORD SYM_EXPORT WINAPI NotifyServersOfVirus(
    LPSTR   szVirusName,
    LPSTR   szUserName,
    LPSTR   szPath )

{
    WORD    wErr = NOERR;
    DWORD   dwErr = 0;
#ifndef	SYM_WIN32
    WORD    wIpxInitialized = FALSE;
    WORD    i;

    char    szServerName[NETMAXSERVERNAME];
    WORD    wPreferredConnection;

    DWORD   dwConnectionId;
    WORD    wIndex = 0;
    LPBYTE  lpIpxMessage;
    WORD    wMessageLen;

    STATIC HGLOBAL         hEcb = NULL;
    STATIC LPIPXECBLIST    lpEcb = NULL;
#endif

#ifdef	SYM_WIN32
    dwErr = NotifyNLMServers (szVirusName, szPath, szUserName);
    if (dwErr)
    	wErr = (WORD) dwErr;

#else
    // first check to see if I am attached to a server (which means the
    // shell is running).

    dwConnectionId = ServenfyNW3_GetConnectionNumber( );
    if (dwConnectionId == 0)
        {
        wErr = ERR;
        goto BailOut;
        }

    wErr = DynamicallyLoadNetworkLibrary( );
    if (wErr)
        {
        goto BailOut;
        }

    wErr = InitIpx( );
    if (wErr)
        {
        goto BailOut;
        }
    else
        {
        wIpxInitialized = TRUE;
        }

    wErr = AllocInitEcb( &hEcb, &lpEcb );
    if (wErr)
        {
        goto BailOut;
        }

    // build the notification message

    if (lpfnReadPropertyValue != NULL && lpfnIpxGetLocalTarget != NULL &&
        lpfnIpxGetLocalTarget != NULL && lpfnScanBinderyObject != NULL)
        {
        lpIpxMessage = lpEcb->byIpxMessage;
        wMessageLen = 0;

        PutMessageByte( &lpIpxMessage, &wMessageLen, IPX_WORKSTATION_VIRUS );
        PutMessageString( &lpIpxMessage, &wMessageLen, "NAV" );
        PutMessageString( &lpIpxMessage, &wMessageLen, szVirusName );
        PutMessageString( &lpIpxMessage, &wMessageLen, szPath );
        PutMessageString( &lpIpxMessage, &wMessageLen, szUserName );
        PutMessageByte( &lpIpxMessage, &wMessageLen, EOS );

        // notify the first server I like, either the preferred or default
        // connection

        wPreferredConnection = (*lpfnGetPreferredConnectionId)( );

        (*lpfnGetFileServerName)( (BYTE) wPreferredConnection, (LPSTR)szServerName );

        if (NotifyServerOfVirus( szServerName,
                                 (LPSTR)lpIpxMessage,
                                 lpEcb,
                                 wMessageLen ) == NOERR )
            {
            goto BailOut;       // it worked, I'm done
            }

        // that didn't work, find the first server in the list that listens

        for (i=1; i<=8; i++)
            {
            if (i != wPreferredConnection)      // already tried him
                {
                (*lpfnGetFileServerName)
                    ( (BYTE) i, (LPSTR)szServerName );

                if (NotifyServerOfVirus( szServerName,
                                         (LPSTR)lpIpxMessage,
                                         lpEcb,
                                         wMessageLen ) == NOERR )
                    {
                    break;
                    }
                }
            }
        }

BailOut:

    if (wIpxInitialized == TRUE)
        {
        UnInitIpx( );
        }

    if (hEcb != NULL)
        {
        FreeEcb(hEcb);
        hEcb = NULL;
        }
#endif

    return (wErr);
}

#ifdef SYM_WIN

//***************************************************************************
// InitializeTimeout()
//
// Description: Initializes a time out
//
// Parameters:
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 05/06/1993 JMILLARD Function Created.
//***************************************************************************

VOID SYM_EXPORT WINAPI InitializeTimeout( LPTIMEOUT lpTimeout, DWORD dwTimeout )
{
    lpTimeout->dwStructSize = sizeof( TIMEOUT );

    // set the current time - this is used to check for wrap of the clock

    lpTimeout->dwInitialTicks = GetTickCount( );

    // calculate when the timer should run out - if this wraps around the
    // limit, it wraps just like GetTickCount does

    lpTimeout->dwExpireTicks = lpTimeout->dwInitialTicks + dwTimeout;

    lpTimeout->bExpireWrapped =
        lpTimeout->dwExpireTicks < lpTimeout->dwInitialTicks;

    lpTimeout->bNoTimeout = dwTimeout == NO_TIMEOUT;

}


//***************************************************************************
// TimedOut()
//
// Description: Checks to see if a timeout has expired
//
// Parameters:
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 05/06/1993 JMILLARD Function Created.
//***************************************************************************

BOOL SYM_EXPORT WINAPI TimedOut( LPTIMEOUT lpTimeout )
{
    BOOL    bExpired = FALSE;
    DWORD   dwCurrTime;


    if (!lpTimeout->bNoTimeout)
        {

        dwCurrTime = GetTickCount( );       // get current time

        if (lpTimeout->bExpireWrapped)
            {

            // if the timer has wrapped, and the current time is past
            // the target time

            if (dwCurrTime <  lpTimeout->dwInitialTicks &&
                dwCurrTime >= lpTimeout->dwExpireTicks)
                {
                bExpired = TRUE;
                }
            }
        else
            {

            // if the time is past the target time, or the timer has wrapped
            // and I didn't see the expiration time

            if (dwCurrTime >= lpTimeout->dwExpireTicks ||
                dwCurrTime < lpTimeout->dwInitialTicks )
                {
                bExpired = TRUE;
                }
            }
        }

    return (bExpired);
}


#endif


//***************************************************************************
// GetNLMDataFile
//
// Description: Loads from the Resource and returns the .DAT file name
//
// Parameters:
//		lpszFileName		LPTSTR		filename
//
// Return Value: 		TRUE on success and FALSE on failure.
//
// See Also:
//
//***************************************************************************
// 05/16/1996 PVENKAT Function Created.
//***************************************************************************
BOOL
GetNLMDataFile (LPTSTR lpszFileName)
{
    TCHAR	szPath[SYM_MAX_PATH + 1];	// For complete path
	int		nChars;						// To check for success
	BOOL	bRet = TRUE;				// Start with success

	// Get the Filename from Resources
   nChars = LoadString (AfxGetInstanceHandle(),
								IDS_NLMDATA_FILE_NAME,
								lpszFileName,
								sizeof (szPath));
	// Check if succeeded
	if (nChars)
	{
		// Get the Directory
      GetStartDir (AfxGetInstanceHandle(), szPath, sizeof (szPath));

		// Append the filename
    	NameAppendFile (szPath, lpszFileName);

		// Copy to the return buffer
		STRCPY (lpszFileName, szPath);
	}
	else
		bRet = FALSE;	// Failed.
    return bRet;
}

//***************************************************************************
// GetNWServerName
//
// Description: Reads the DAT file and returns the Server name
//
// Parameters:
//		lpszNLMServer	LPTSTR		buffer for the servername
//
// Return Value:
//		BOOL			TRUE		on success
//						FALSE		on failure
//
// See Also:
//
//***************************************************************************
// 05/16/1996 PVENKAT Function Created.
//***************************************************************************
BOOL SYM_EXPORT WINAPI
GetNWServerName (LPTSTR lpszNLMServer)
{
	BOOL		bRet = FALSE;
	TCHAR		szDataFile[SYM_MAX_PATH + 1];	// Full pathname
	HFILE 		hFile;							// File handle
	DWORD		dwSize, dwRead;					// Buffer size, Bytes read
	OFSTRUCT	of;

	// Get the DAT file
	GetNLMDataFile (szDataFile);

    *lpszNLMServer = EOS;                       // Start with empty string

	// Open the file for read.
	hFile = OpenFile (szDataFile, &of, OF_READ);
	if (hFile != HFILE_ERROR)
	{
		//
		// Get the size:  Previously we write the filename, so
		// cannot exceed MAX_PATH
		//
		dwSize = GetFileSize ((HANDLE) hFile, NULL);

		if (dwSize != 0xFFFFFFFF)
		{
			// Read the server name
			bRet = ReadFile ((HANDLE) hFile,
								lpszNLMServer,
								dwSize,
								&dwRead,
								NULL);
		}
		_lclose (hFile);
	}
	return bRet;
}

//***************************************************************************
// WriteNWServerName
//
// Description: Writes the provided servername to the file.  And deletes
//				the file, if NULL passed.
//
// Parameters:
//		lpszNLMServer		LPTSTR		server name to write to the DAT file.
//
// Return Value:
//		BOOL				TRUE		on success
//							FALSE		on failure.
//
// See Also:
//
//***************************************************************************
// 05/16/1996 PVENKAT Function Created.
//***************************************************************************
BOOL SYM_EXPORT WINAPI
WriteNWServerName (LPTSTR lpszNLMServer)
{
	BOOL		bRet = TRUE;					// Start with Success
	TCHAR		szDataFile[SYM_MAX_PATH + 1];	// DAT filename
	OFSTRUCT	of;
	DWORD		dwLen, dwWritten;
	HFILE		hFile;

	// Get the DAT file name.
	GetNLMDataFile (szDataFile);

	// Check if valid selection
	if (lpszNLMServer != NULL)
	{
		MEMSET (&of, 0, sizeof (of));
		of.cBytes = sizeof (of);

		// Open it for writing, if it doesnot exist, create it.
		hFile = OpenFile (szDataFile, &of, OF_CREATE | OF_WRITE);
		if (hFile != HFILE_ERROR)
		{
			// Add one TCHAR size for EOS
			dwLen = STRLEN (lpszNLMServer) + sizeof (lpszNLMServer[0]);

			// Write the filename includeing EOS
			bRet = WriteFile ((HANDLE)hFile,
								lpszNLMServer,
								dwLen,
								&dwWritten,
								NULL);
			_lclose (hFile);
		}
        else
            bRet = FALSE;

	}
	else
		bRet = DeleteFile (szDataFile);		// Delete it: No NLM Server selected
	return bRet;
}


// #define     MAX_RECS       100      // # of servers in each type


//***************************************************************************
// NotifyNLMServer
//
// Description: Sends the Virus Notification to the selected
//          NetWare server
//
// Parameters:
//		szVirus		LPTSTR		Virus name
//		szFile		LPTSTR		Filename (full pathname)
//		szUser		LPTSTR		Current User name.
//    szServer    LPTSTR      Server to notify
//
// Return Value:
// 		DWORD		WinSock Error code.
//
// Note:
//    The guts of this function have been moved into the CNLMMessenger
//    that resides in NLMMSGR.CPP.
//
// See Also:
//
//***************************************************************************
// 05/16/1996 PVENKAT Function Created.
// 08/19/1996 DSACKIN Function Re-Worked to use CNLMMessenger
//***************************************************************************
BOOL NotifyNLMServer (LPTSTR szVirus, LPTSTR szFile, LPTSTR szUser, LPTSTR szServer)
{
BOOL bReturn = TRUE;

   if (NULL == pMessenger)
   {
      bReturn = FALSE;
      goto Cleanup;
   }

   if (ERROR_SUCCESS != pMessenger->NotifyNLMServer(
                           szVirus,
                           szFile,
                           szUser,
                           szServer))
   {
      bReturn = FALSE;
   }

Cleanup:

   return bReturn;
}


//***************************************************************************
// NotifyNLMServer
//
// Description: Sends the Virus Notification to the previous selected
//				NetWare server or all servers (if 'All' was selected).
//
// Parameters:
//		szVirus		LPTSTR		Virus name
//		szFile		LPTSTR		Filename (full pathname)
//		szUser		LPTSTR		Current User name.
//
// Return Value:
// 		DWORD		WinSock Error code.
//
// Note:
//    The guts of this function have been moved into the CNLMMessenger
//    that resides in NLMMSGR.CPP.
//
//    The actual server name that we need to notify is retrieved by
//    the call to GetNWServerName.
//
// See Also:
//
//***************************************************************************
// 05/16/1996 PVENKAT Function Created.
// 08/19/1996 DSACKIN Function Re-Worked to use CNLMMessenger
//***************************************************************************
BOOL NotifyNLMServers (LPTSTR lpszVirus, LPTSTR lpszFile, LPTSTR szUser)
{
BOOL  bReturn = TRUE;
int   nRet;
TCHAR szServer[NETMAXSERVERNAME + 1];
TCHAR szAll[SYM_MAX_PATH + 1];

   if (NULL == pMessenger)
   {
      bReturn = FALSE;
      goto Cleanup;
   }

   nRet = LoadString (
             AfxGetInstanceHandle(),
             IDS_SERVER_ALL,
             szAll,
             sizeof (szAll) / sizeof (szAll[0]));

   if (0 == nRet)
   {
      bReturn = FALSE;
      goto Cleanup;
   }

   if (!GetNWServerName (szServer))
      STRCPY (szServer, szAll);

   if (szServer[0] == '\\')
      STRCPY (szServer, &szServer[2]);

   if (STRCMP (szServer, szAll) != 0)
      bReturn = NotifyNLMServer (lpszVirus, lpszFile, szUser, szServer);
   else
   {
      bReturn = pMessenger->NotifyAllNLMServers(
                  lpszVirus,
                  lpszFile,
                  szUser);
   }

Cleanup:

   return bReturn;

}


//***************************************************************************
// NotifyNAVNLMServer
//
// Description:
//    Sends the Virus Notification to the specified NetWare servers.
//    If "(All NetWare Servers)" is specified, the message goes
//    to all NetWare Servers.
//
// Parameters:
//    szVirus     LPSTR      Virus name
//    szFile      LPSTR      Filename (full pathname)
//    szUser      LPSTR      Current User name.
//    szServer    LPSTR      Server to send message to (Possibly All Servers)
//
// Return Value:
//       BOOL     TRUE - Success
//               FALSE - Failure
//
// See Also:
//
//***************************************************************************
// 11/17/96 - DSACKIN - Function Created
//***************************************************************************
BOOL SYM_EXPORT NotifyNAVNLMServer (
   LPSTR lpszVirus,                     // [in] - Virus name
   LPSTR lpszFile,                      // [in] - File containing the virus
   LPSTR szUser,                        // [in] - User who discovered the virus
   LPSTR szServer)                      // [in] - Server to notify.
{
BOOL  bReturn = TRUE;
int   nRet;
TCHAR szAll[SYM_MAX_PATH + 1];

   if (NULL == pMessenger)
   {
      bReturn = FALSE;
      goto Cleanup;
   }

   nRet = LoadString (
             AfxGetInstanceHandle(),
             IDS_SERVER_ALL,
             szAll,
             sizeof (szAll) / sizeof (szAll[0]));

   if (0 == nRet)
   {
      bReturn = FALSE;
      goto Cleanup;
   }

   if (STRCMP (szServer, szAll) != 0)
      bReturn = NotifyNLMServer (lpszVirus, lpszFile, szUser, szServer);
   else
   {
      bReturn = pMessenger->NotifyAllNLMServers(
                  lpszVirus,
                  lpszFile,
                  szUser);
   }

Cleanup:

   return bReturn;

}


//***************************************************************************
// InitMessenger
//
// Description:
//    Starts our CNLMMessenger guy working.
//    If the messenger is already working, that is OK.
//
// Parameters:
//    none.
//
// Return Value:
//    BOOL        TRUE     on success
//                FALSE    on failure
//
// See Also:
//
//***************************************************************************
// 08/14/1996 DSACKIN Function Created.
//***************************************************************************
#ifdef SYM_WIN32

BOOL SYM_EXPORT WINAPI InitNLMMessenger (VOID)
{
BOOL bReturn = TRUE;

                                        // If its not NULL, it is already
                                        // running.

   if (NULL == pMessenger)
   {
      pMessenger = new CNLMMessenger();

      if (NULL == pMessenger)
      {
         bReturn = FALSE;
         goto Cleanup;
      }

      if (ERROR_SUCCESS != pMessenger->Init())
      {
         delete pMessenger;
         bReturn = FALSE;
         goto Cleanup;
      }
   }

Cleanup:

   return bReturn;
}

#endif // SYM_WIN32


//***************************************************************************
// DeInitMessenger
//
// Description:
//    Stops our messenger (Should ONLY be called in the WEP function
//    of the DLL exiting.  This way, we know that ALL dependant processes
//    have exited.
//
// Parameters:
//    none.
//
// Return Value:
//    none.
//
// See Also:
//
//***************************************************************************
// 08/14/1996 DSACKIN Function Created.
//***************************************************************************
#ifdef SYM_WIN32

VOID SYM_EXPORT WINAPI DeInitNLMMessenger (VOID)
{
   if (NULL != pMessenger)
   {
      delete pMessenger;
   }
}

#endif // SYM_WIN32

