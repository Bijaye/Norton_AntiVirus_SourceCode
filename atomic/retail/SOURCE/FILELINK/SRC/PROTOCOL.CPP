/////////////////////////////////////////////////////////////////////
// PROTOCOL.CPP - Created - 12/03/97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/FILELINK/VCS/PROTOCOL.CPv   1.0   17 Mar 1998 19:45:16   SEDWARD  $
//
// Description:
//    This file contains the code that will take a machine name and
//    figure out which protocol to use to contact that machine.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/FILELINK/VCS/PROTOCOL.CPv  $
// 
//    Rev 1.0   17 Mar 1998 19:45:16   SEDWARD
// Initial revision.
// 
//    Rev 1.1   27 Jan 1998 13:21:08   KSACKIN
// Updated the protocol string list to be an LPCTSTR from an LPSTR.
//
//    Rev 1.0   27 Jan 1998 12:41:12   KSACKIN
// Initial revision.
//
//    Rev 1.1   17 Dec 1997 17:30:40   DBuches
// Change _asm int 3 calls to DebugBreak().
//
//    Rev 1.0   03 Dec 1997 19:10:44   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include <platform.h>

#include "LinkCli.h"
#include "LinkVal.h"
#include "RPCProt.h"                    // For the RPC Protocol definitions

#include "LinkInfo.h"

#include "Protocol.h"


#define NUMBER_OF_USED_PROTOCOLS 11

LPCTSTR szProtocols [NUMBER_OF_USED_PROTOCOLS] = {
          _T(PROTOCOL_NCACN_IP_TCP),
          _T(PROTOCOL_NCACN_NB_IPX),
          _T(PROTOCOL_NCACN_SPX),
          _T(PROTOCOL_NCACN_NB_NB),
          _T(PROTOCOL_NCACN_NB_TCP),
          _T(PROTOCOL_NCADG_IP_UDP),
          _T(PROTOCOL_NCADG_IPX),
          _T(PROTOCOL_NCACN_NP),
          _T(PROTOCOL_NCACN_DNET_NSP),
          _T(PROTOCOL_NCACN_VNS_SPP),
          _T(PROTOCOL_NCACN_AT_DSP)
};



/////////////////////////////////////////////////////////////////////
// Macros

#ifdef _DEBUG
#define MY_ASSERT(xP) if( (LPVOID)xP == NULL ) DebugBreak();
#else
#define MY_ASSERT(xP)
#endif


/////////////////////////////////////////////////////////////////////
// BOOL __declspec(dllexport) WINAPI SelectProtocol(
//
// Purpose:
//    This function is used to figure out which protocol to use
//    to contact a target machine
//
// Parameters:
//    PRPC_LINK_INFO pLinkInfo - [in] - Information on how to contact the server
//                                    - This structure needs to have the address
//                                      Already filled out.  This structure
//                                      also has to have the Protocol buffer
//                                      allocated.
//    LPFNPINGFUNCTION lpfnPing - [in] - Ping function to use in contacting
//                                       the desired machine
//
// Returns:
//    TRUE  - A protocol was found and returned in pLinkInfo->pszProtocol
//    FALSE - No protocol was found that could contact the remote machine.
//
// Notes:
//
//
/////////////////////////////////////////////////////////////////////
// 12/02/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL __declspec(dllexport) WINAPI SelectProtocol(
    PRPC_LINK_INFO     pLinkInfo,
    LPFNPINGFUNCTION   lpfnPing)
    {
    BOOL  bReturn  = FALSE;
    DWORD dwReturn = LINK_ERROR;
    DWORD dwCount;

                                        // Validate incoming structure
    MY_ASSERT(pLinkInfo);
    MY_ASSERT(pLinkInfo->pszAddress);
    MY_ASSERT(pLinkInfo->pszProtocol);

    for (dwCount = 0;
         ( (dwCount < NUMBER_OF_USED_PROTOCOLS) &&
           (LINK_ERROR == dwReturn) );
         dwCount++)
         {

                                        // Copy in the next protocol and try to
                                        // connect with it.
         STRCPY (
             pLinkInfo->pszProtocol,
             szProtocols[dwCount]);


         dwReturn = lpfnPing(
                        pLinkInfo);
         }

                                        // If one of the protocols succeeds,
                                        // then we set the success flag.  The
                                        // protocol name will already be in the
                                        // structure ready for return.
    if (ERROR_SUCCESS == dwReturn)
        bReturn = TRUE;

    return bReturn;
    }



