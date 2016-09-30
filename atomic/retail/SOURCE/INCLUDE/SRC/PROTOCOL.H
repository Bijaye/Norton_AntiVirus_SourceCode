/////////////////////////////////////////////////////////////////////
// PROTOCOL.H - Created - 08/01/97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/PROTOCOL.H_v   1.0   17 Mar 1998 19:50:02   SEDWARD  $
//
// Description:
//    This file contains the prototype for the protocol detection function
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/INCLUDE/VCS/PROTOCOL.H_v  $
// 
//    Rev 1.0   17 Mar 1998 19:50:02   SEDWARD
// Initial revision.
// 
//    Rev 1.0   27 Jan 1998 12:46:56   KSACKIN
// Initial revision.
// 
//    Rev 1.0   03 Dec 1997 19:10:16   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__PROTOCOL_H)
#define __PROTOCOL_H

#if !defined(__LINKINFO_H)
#   include "linkinfo.h"
#endif // #if !defined(__LINKINFO_H)


BOOL __declspec(dllexport) WINAPI SelectProtocol(
    PRPC_LINK_INFO     pLinkInfo,
    LPFNPINGFUNCTION   lpfnPing);


#endif // if !defined(__PROTOCOL_H)
