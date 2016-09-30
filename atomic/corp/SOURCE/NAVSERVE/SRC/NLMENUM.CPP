/////////////////////////////////////////////////////////////////////
// NLMENUM.CPP - Created - 12-12-96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/NAVSERVE/VCS/nlmenum.cpv   1.3   13 Feb 1998 18:38:52   MKEATIN  $
//
// Description:
//    This is where the NLM enumeration functions will go.  This is
//    going to be called by all of the people that are going to need
//    a list of NLM servers or by people that want to get a list of
//    all of the NLM servers that are currently running NAVNLM.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/NAVSERVE/VCS/nlmenum.cpv  $
// 
//    Rev 1.3   13 Feb 1998 18:38:52   MKEATIN
// Roswell Port.
// 
//    Rev 1.3   14 Jan 1998 17:48:46   CEATON
// Further changes to load Winsock DLL "just-in-time".
// 
// 
//    Rev 1.2   22 Dec 1997 17:30:34   CEATON
// GetAddressByName() now called via function pointer.
// 
// 
//    Rev 1.1   18 Apr 1997 14:02:02   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:14:06   RFULLER
// Initial revision
// 
//    Rev 1.1   21 Dec 1996 01:21:52   ksackin
// Fixed some problems with the Novell 32 bit client for Windows NT.  We now
// handle the dynamic sizing of the resource buffer for the Network Enumeration
// calls.  This was done because Novell did not furnish the API's in the same
// fashion that Microsoft does, and therefore need HUGE buffers to do the same
// thing that Microsoft did with small ones.  Also handle the ERROR_MORE_DATA
// error message when enumerating the network, and properly resize our buffer
// accordingly.
//
//    Rev 1.0   13 Dec 1996 13:57:58   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

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

typedef INT ( APIENTRY *GETADDRESSBYNAME )(
    IN     DWORD                dwNameSpace,
    IN     LPGUID               lpServiceType,
    IN     LPSTR              lpServiceName OPTIONAL,
    IN     LPINT                lpiProtocols OPTIONAL,
    IN     DWORD                dwResolution,
    IN     LPSERVICE_ASYNC_INFO lpServiceAsyncInfo OPTIONAL,
    IN OUT LPVOID               lpCsaddrBuffer,
    IN OUT LPDWORD              lpdwBufferLength,
    IN OUT LPSTR              lpAliasBuffer OPTIONAL,
    IN OUT LPDWORD              lpdwAliasBufferLength OPTIONAL
    );

/////////////////////////////////////////////////////////////////////
// Global Variables


/////////////////////////////////////////////////////////////////////
// VOID EnumNAVNLMServers
//
// Purpose:
//    This function should be called by an application that is looking
//    for netware servers.
//
// Note:
//    If the address parameter is requested, only NetWare servers that
//    are currently running the NAV NLM will be returned.  If the
//    address parameter is not requested, this function returns all
//    NetWare servers.
//
// Parameters:
//    NAVNLMENUMERATIONPROC lpfnEnumProc - [in] - Function to send our
//                                                enumerated servers to.
//    BOOL bReturnAddress - [in] - Should we do a name lookup on the
//                               - servers that we find?
//
//
// Returns:
//    BOOL - TRUE  - Operation completed successfully
//         - FALSE - the callback function cancelled the operation
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 12-12-96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL SYM_EXPORT WINAPI EnumNAVNLMServers(
   NAVNLMENUMERATIONPROC lpfnEnumProc,  // Callback procedure
   BOOL                  bReturnAddresses, // Should we get addresses?
   LPNETRESOURCE         lpCurrentResource,// Always start with NULL!
   LPVOID                lpExtra)       // Will be passed back to callback.
{
HINSTANCE       hWinsockLibrary = NULL;
GETADDRESSBYNAME lpGetAddressByName;
BOOL            bReturn = TRUE;
DWORD           dwErr;

HANDLE          hEnum;
BOOL            bEnumOpen = FALSE;
LPNETRESOURCE   lpResources = NULL;
DWORD           dwResourceBufferSize = 4096;
DWORD           dwCount;
DWORD           dwTotalObjects;

CSADDR_INFO     rAddress[MAX_ADDRESSES];    // Server Addresses
DWORD           dwSize;

LPSTR           lpszServer, lpszTempName;

GUID            guid;
int             anProtocols[MAX_PROTOCOLS];
DWORD           dwProtocolSize;
DWORD           dwAddressCount, dwRet;


/////////////////////////////////////////////////////////////////////
// Start off with initialization

   if (bReturnAddresses)
   {
                                        // Fill in the GUID
                                        // NAV NLM uses GUID 0x0531
      guid.Data1    = MAKELONG (MAKEWORD (0x31, 0x05), 0x000B);
      guid.Data2    = 0;
      guid.Data3    = 0;
      guid.Data4[0] = 0xC0;
      guid.Data4[1] = 0;
      guid.Data4[2] = 0;
      guid.Data4[3] = 0;
      guid.Data4[4] = 0;
      guid.Data4[5] = 0;
      guid.Data4[6] = 0;
      guid.Data4[7] = 0x46;

                                        // zero out the protocols.
      MEMSET (anProtocols, 0, sizeof (anProtocols));

      anProtocols[0] = NSPROTO_IPX;     // only IPX
                                        // set size based on
                                        // SOCKADDR_IPX (of our interest)
      dwProtocolSize = MAX_PROTOCOLS * sizeof (SOCKADDR_IPX);
   }


/////////////////////////////////////////////////////////////////////
// First, we want to go through any containers like NetWare Servers
// to get to the actual servers.

                                        // First, go through the list of
                                        // network type containers.

                                        // Starting from the top?
   dwErr = WNetOpenEnum (
               RESOURCE_GLOBALNET,
               RESOURCETYPE_ANY,
               RESOURCEUSAGE_CONTAINER,
               lpCurrentResource,
               &hEnum);

   if (NO_ERROR != dwErr)
   {
      bReturn = FALSE;
      goto Cleanup;
   }

                                        // Signal to clean up when we are done
   bEnumOpen = TRUE;

                                        // Allocate the initial size of our
                                        // enumeration buffer, and check for
                                        // NULL case.
   if ( NULL == (lpResources = (LPNETRESOURCE) malloc( dwResourceBufferSize ) ) )
   {
                                        // Exit this function if we have an
                                        // allocation error.
      goto Cleanup;
   }


   if ( ( hWinsockLibrary = LoadLibrary( "WSOCK32.DLL" ) ) != NULL )
       lpGetAddressByName  =( GETADDRESSBYNAME )  GetProcAddress( hWinsockLibrary,
                                      "GetAddressByNameA" );

                                        // Keep getting servers while there
                                        // is no error.  We will eventually
                                        // get a ERROR_NO_MORE_ITEMS
   while (NO_ERROR == dwErr)
   {
                                        // This flag tells WNetEnumResource
                                        // to return as many as possible.
      dwTotalObjects = 0xFFFFFFFF;

      dwErr = WNetEnumResource (
                  hEnum,
                  &dwTotalObjects,
                  lpResources,
                  &dwResourceBufferSize);

                                        // Not enough information for one
                                        // record...
      if ( dwErr == ERROR_MORE_DATA )
      {
                                        // Free the current resource buffer.
         free( lpResources );

                                        // Increment this number for safety, as
                                        // well as to work around a bug with
                                        // the Novell NT 3.51 client for NT.
                                        // (Bogus return value. Too small)
         dwResourceBufferSize += 100;

                                        // Reallocate the resource buffer to be
                                        // as big as we need it for this
                                        // enumeration.  Also check for NULL.
         if ( NULL == (lpResources = (LPNETRESOURCE) malloc( dwResourceBufferSize )) )
         {
                                        // Exit this function if we have an
                                        // allocation error.
         goto Cleanup;
         }
                                        // Since we only needed to resize
                                        // our buffer, no error really occurred.
                                        // Setting this value to NO_ERROR will
                                        // allow this enumeration loop to continue.
         dwErr = NO_ERROR;
      }
                                        // OK.. Go through all object resources
      for (dwCount = 0;
            (NO_ERROR == dwErr) &&
               (dwCount < dwTotalObjects);
            dwCount++)
      {
                                        // If they don't give us a
                                        // provider name, don't check the
                                        // resources.
         if (NULL == lpResources[dwCount].lpProvider)
            continue;

                                        // We are looking only for NetWare
         if (NULL == STRSTR (lpResources[dwCount].lpProvider, SZ_NETWARE))
            continue;

                                        // The type share is a hack because of
                                        // the way the new NetWare drivers
                                        // change the NWNetEnum functions.
         if ( (RESOURCEDISPLAYTYPE_GENERIC == lpResources[dwCount].dwDisplayType) ||
              (RESOURCEDISPLAYTYPE_NETWORK == lpResources[dwCount].dwDisplayType) ||
              (RESOURCEDISPLAYTYPE_SHARE   == lpResources[dwCount].dwDisplayType) )
         {
                                        // Only continue to enumerate further
                                        // if we get a generic type or a
                                        // network type
            EnumNAVNLMServers(
               lpfnEnumProc,
               bReturnAddresses,
               &lpResources[dwCount],
               lpExtra);

         }
         else if (RESOURCEDISPLAYTYPE_SERVER == lpResources[dwCount].dwDisplayType)
         {
                                        // Don't add NULL names!!!
            if (NULL == lpResources[dwCount].lpRemoteName)
               continue;

            lpszServer = lpResources[dwCount].lpRemoteName;

            if (!bReturnAddresses)
            {
                                        // If we don't have to return
                                        // addresses, we have all of the
                                        // information that we need at this
                                        // point so send it back.

               lpfnEnumProc(
                  lpszServer,
                  (LPVOID) NULL,
                  lpExtra);
            }
            else
            {

                                        // Use a dummy variable to get pas the
                                        // back-slashes
               lpszTempName = lpszServer;

                                        // OK... they want the addresses.  Lets
                                        // get it for them.
                                        // Set our pointer past the double backslash
               while ( (*lpszTempName == ' ') || (*lpszTempName == '\\') )
                  lpszTempName++;

                                        // Now that we have the name, we need
                                        // to get what we came for: the address
               dwSize = sizeof (rAddress);
               dwRet = ( *lpGetAddressByName )(
                        NS_SAP,          // Name Space
                        &guid,           // NLM GUID Identifier
                        lpszTempName,    // Server Name
                        anProtocols,     // Protocols
//                        NULL,            // Solution on NT 3.51 with Novell client 32 (Slow as heck!)
                        RES_SOFT_SEARCH, // Resolution
                        NULL,            // Reserved
                        &rAddress,       // Buffer
                        &dwSize,         // Bufsize - Returns number of byte received
                        NULL,            // Alias Buffer
                        NULL);           // Alias buf size

                                        // Return them if we have succeeded
               if (SOCKET_ERROR != dwRet)
               {
                  for (dwAddressCount = 0;
                     dwAddressCount < dwRet;
                     dwAddressCount++)
                  {
                                          // Send servers back to the callback
                     lpfnEnumProc(
                        lpszServer,
                        (LPVOID) (rAddress[dwAddressCount].RemoteAddr.lpSockaddr),
                        lpExtra);
                  }
               }

            }

         }
      }
   }

   if (NO_ERROR != dwErr)
   {
                                        // ERROR_NO_MORE_ITEMS is not really an
                                        // error and we should treat it as such
      if (ERROR_NO_MORE_ITEMS == dwErr)
      {
         dwErr = NO_ERROR;
      }
      else
      {
                                        // Oops... We got a real error
         bReturn = FALSE;
         goto Cleanup;
      }
   }

Cleanup:

   if (bEnumOpen)
   {
      WNetCloseEnum(hEnum);
      bEnumOpen = FALSE;
   }

   if ( hWinsockLibrary != NULL )
       FreeLibrary( hWinsockLibrary );

                                        // If we still have a resource buffer...
   if ( NULL != lpResources )
   {
                                        // Free the resource buffer.
      free( lpResources );
   }
   return bReturn;
}
