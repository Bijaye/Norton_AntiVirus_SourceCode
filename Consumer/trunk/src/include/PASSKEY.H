////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// $Header:   S:/passkey/VCS/passkey.CPv   1.1   06 Aug 1997 21:19:28   bsobel  $
//
// Description:
//
//     Library for determining passkey codes stored in the registry (or elsewhere)
//     as a mechanism for an ad-hok copy protection
// Contains:
//
// See Also:
//
//////////////////////////////////////////////////////////////////////////
// $Log:   S:/passkey/VCS/passkey.CPv  $
//
#ifndef PASSKEY_H
#define PASSKEY_H



#if defined(__cplusplus)
extern "C" {
#endif

#define   PCOOKIE_F_SUCCESS   1     // success in aquiring 

// NOTE:  This structure is OPAQUE! I.E., It is merely a cookie that is passed back to the 
// password verification code.  Please do not do anything with or make any assumptions about 
// the structure.  Development of the passkey code must be free to change this at any time.

typedef struct _PCOOKIE
{
    DWORD   dwcSize;
    DWORD   dwComponent1;
    DWORD   dwComponent2;
    DWORD   dwComponent3;
    DWORD   dwSuccess;

} PASSKEYCOOKIE, * PPASSKEYCOOKIE;



BOOL        SYM_EXPORT WINAPI  PASSKEYInstall(DWORD password,DWORD dwSeed);
BOOL		SYM_EXPORT WINAPI  PASSKEYInstallEx(LPCTSTR szDllName);
BOOL        SYM_EXPORT WINAPI  PASSKEYUnInstall();

BOOL        SYM_EXPORT WINAPI  GetPASSKEYCookie( PASSKEYCOOKIE * pCookie);
BOOL        SYM_EXPORT WINAPI  TestPASSKEYCookie (PASSKEYCOOKIE * pCookie,DWORD dwSeed);


#if defined(__cplusplus)
}
#endif


#endif