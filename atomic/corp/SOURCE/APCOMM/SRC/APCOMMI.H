// Copyright 1997 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/APCOMM/VCS/APCOMMI.H_v   1.1   17 Feb 1998 14:28:00   RStanev  $
//
// Description:
//      This file prototypes the APCOMM functions.  Some of these functions
//      are exported to other NAVAP modules.
//
// Contains:
//      NAVAPServiceCallback()
//      NAVAPCancelCallback()
//
//      EnterEnableDisable()
//      EnterReloadConfig()
//      EnterLoadAVAPI()
//      EnterUnloadAVAPI()
//      EnterControlledInit()
//      EnterControlledDeInit()
//
//      Int2FChainCallback()                (SYM_VXD only)
//
// See also:
//      apcomm.h(INCLUDE)
//
//***************************************************************************
// $Log:   S:/APCOMM/VCS/APCOMMI.H_v  $
// 
//    Rev 1.1   17 Feb 1998 14:28:00   RStanev
// Updated copyright.
//
//    Rev 1.0   15 May 1997 00:25:42   RStanev
// Initial revision.
//
//***************************************************************************

#ifndef _APCOMMI_H_
#define _APCOMMI_H_

//---------------------------------------------------------------------------
//
// Prototype DriverComm callback functions.
//
//---------------------------------------------------------------------------
VOID NAVAPServiceCallback (
    DWORD  dwServiceCode,
    PVOID  pvInBuffer,
    DWORD  dwInBufferSize,
    PVOID  pvOutBuffer,
    DWORD  dwOutBufferSize,
    PDWORD pdwBytesReturned
    );

VOID NAVAPCancelCallback (
    DWORD dwServiceCode,
    PVOID pvInBuffer,
    DWORD dwInBufferSize
    );

//---------------------------------------------------------------------------
//
// Prototype helper functions for servicing user mode requests.
//
//---------------------------------------------------------------------------
DWORD EnterEnableDisable (
    BOOL bNewStatus
    );

DWORD EnterReloadConfig (
    VOID
    );

DWORD EnterLoadAVAPI (
    VOID
    );

DWORD EnterUnloadAVAPI (
    VOID
    );

DWORD EnterControlledInit (
    VOID
    );

DWORD EnterControlledDeInit (
    VOID
    );

//---------------------------------------------------------------------------
//
// Miscellaneous helper functions.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

extern "C" {

BOOL WINAPI Int2FChainCallback (
    PCRS prCRS
    );

}

#endif // #if defined(SYM_VXD)

#endif // #ifndef _APCOMM_H_
