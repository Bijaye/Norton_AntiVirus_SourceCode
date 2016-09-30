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
    PNAVAP_CONFIG prConfig
    );

DWORD EnterLoadAVAPI (
    PSTR pszPath
    );

DWORD EnterUnloadAVAPI (
    VOID
    );

DWORD EnterControlledInit (
    PNAVAP_CONFIG prConfig
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
