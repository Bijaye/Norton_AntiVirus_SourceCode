// Copyright (C) 1997 Symantec Corporation
//*****************************************************************************
// $Header:   S:/INCLUDE/VCS/AdminLink.h_v   1.3   02 Jul 1997 12:41:14   TSmith  $
//
// Description: 
//
//*****************************************************************************
// $Log:   S:/INCLUDE/VCS/AdminLink.h_v  $
// 
//    Rev 1.3   02 Jul 1997 12:41:14   TSmith
// 
//    Rev 1.2   16 Jun 1997 16:26:26   TSmith
// Added function headers.
// 
//    Rev 1.1   12 Jun 1997 13:26:08   TSmith
// Removed overloaded (wide) function versions.
// 
//    Rev 1.0   11 Jun 1997 14:21:58   TSmith
// Initial revision.
//*****************************************************************************
#if !defined( __AdminLink_h__ )
#define       __AdminLink_h__

#include "JobStatus.h"
#include "DistribCommand.h"

typedef struct _RPC_LINK_INFO
    {
    LPTSTR pszProtocol;                 // Protocol to use
    LPTSTR pszAddress;                  // Address to contact
    LPTSTR pszEndpoint;                 // Endpoint to look for
    } RPC_LINK_INFO, *PRPC_LINK_INFO;

/*****************************************************************************
FUNCTION:
    DWORD __declspec(dllexport) WINAPI AdminSendStatus
    
DESCRIPTION:
    Transmits a status packet structure via RPC to the address/endpoint
    combination specified in a RPC_LINK_INFO structure.

PARAMETERS:
    PRPC_LINK_INFO     pLinkInfo - RPC address/endpoint info.
    PJOB_STATUS_PACKET pStatus   - Pointer to the data block to transmit.

RETURN VALUE:
    LINK_ERROR on RPC failure. Call 'GetLastError' for specific error code.
    Value of 'GetLastError' on remote system on successful RPC.
******************************************************************************
6/10/1997 TSmith  - Function created.
*****************************************************************************/
DWORD __declspec(dllexport) WINAPI AdminSendStatus(
    PRPC_LINK_INFO      pLinkInfo,
    PJOB_STATUS_PACKET  pStatus
    );


/*****************************************************************************
FUNCTION:
    DWORD __declspec(dllexport) WINAPI AdminSendCommand
    
DESCRIPTION:
    Transmits a command packet structure via RPC to the address/endpoint
    combination specified in pLinkInfo.
    
PARAMETERS:
    PRPC_LINK_INFO     pLinkInfo - RPC address/endpoint info.
    PJOB_STATUS_PACKET pCommand  - Pointer to the data block to transmit.
    
RETURN VALUE:
    LINK_ERROR on RPC failure. Call 'GetLastError' for specific error code.
    Value of 'GetLastError' on remote system on successful RPC.
******************************************************************************
6/11/1997 TSmith  - Function created.
*****************************************************************************/
DWORD __declspec(dllexport) WINAPI AdminSendCommand(
    PRPC_LINK_INFO           pLinkInfo,
    PDISTRIB_COMMAND_PACKET  pCommand
    );
    
#endif     // __AdminLink_h__
