/////////////////////////////////////////////////////////////////////
// LINKINFO.H - Created - 12/02/97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/LINKINFO.H_v   1.0   17 Mar 1998 19:51:42   SEDWARD  $
//
// Description:
//    This file will contain any link information structures.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/INCLUDE/VCS/LINKINFO.H_v  $
// 
//    Rev 1.0   17 Mar 1998 19:51:42   SEDWARD
// Initial revision.
// 
//    Rev 1.0   27 Jan 1998 12:46:58   KSACKIN
// Initial revision.
// 
//    Rev 1.2   04 Dec 1997 13:45:46   DSACKIN
// Added WINAPI to the typedef of LPFNPINGFUNC
//
//    Rev 1.1   03 Dec 1997 19:08:40   DSACKIN
// Added LPFNPINGFUNCTION
//
//    Rev 1.0   02 Dec 1997 10:45:10   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__LINKINFO_H)
#define __LINKINFO_H

/////////////////////////////////////////////////////////////////////
// Structures

typedef struct _RPC_LINK_INFO
    {
    LPTSTR pszProtocol;                 // Protocol to use
    LPTSTR pszAddress;                  // Address to contact
    LPTSTR pszEndpoint;                 // Endpoint to look for
    } RPC_LINK_INFO, *PRPC_LINK_INFO;


typedef  DWORD (WINAPI * LPFNPINGFUNCTION) (PRPC_LINK_INFO);

#endif // if !defined(__LINKINFO_H)
