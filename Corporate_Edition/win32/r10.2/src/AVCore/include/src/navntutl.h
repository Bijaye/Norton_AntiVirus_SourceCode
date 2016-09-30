// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1998 - 2003, 2005 Symantec Corporation. All rights reserved.. 
//*************************************************************************
// navntutl.h - created 6/9/98 6:01:50 PM
//
// $Header:   S:/INCLUDE/VCS/navntutl.h_v   1.1   11 Jun 1998 13:33:36   DBuches  $
//
// Description: Contains exported functions from NAVNTUTL.DLL
//
//  Note that all functions declared here should also have an up to date
//  function pointer prototype declared with it.
//  
//*************************************************************************
// $Log:   S:/INCLUDE/VCS/navntutl.h_v  $
// 
//    Rev 1.1   11 Jun 1998 13:33:36   DBuches
// Added extern C.
// 
//    Rev 1.0   10 Jun 1998 15:48:48   DBuches
// Initial revision.
//*************************************************************************
#ifndef _NAVNTUTIL_H_
#define _NAVNTUTIL_H_

//*************************************************************************

#ifdef BUILDING_NAVNUTIL
#define DECLSPEC_NAVNUTIL __declspec( dllexport )
#else
#define DECLSPEC_NAVNUTIL __declspec( dllimport )
#endif

#ifdef __cplusplus
extern "C" {
#endif
//*************************************************************************

ULONG DECLSPEC_NAVNUTIL FileHasAlternateDataStreams( HANDLE  hSourceFile );
typedef ULONG (*PFNFILEHASALTERNATEDATASTREAMS) ( HANDLE hSourceFile );

ULONG DECLSPEC_NAVNUTIL CopyAlternateDataStreams( HANDLE hSourceFile, LPCTSTR lpszSourceName, LPCTSTR lpszDestName );
typedef ULONG (*PFNCOPYALTERNATEDATASTREAMS) ( HANDLE hSourceFile, LPCTSTR lpszSourceName, LPCTSTR lpszDestName );

//added to allow people to XOR ADS stream in quarantine...
ULONG DECLSPEC_NAVNUTIL CopyAlternateDataStreams2( HANDLE hSourceFile, LPCTSTR lpszSourceName, LPCTSTR lpszDestName, BOOL bXORStreams );
typedef ULONG (*PFNCOPYALTERNATEDATASTREAMS2) ( HANDLE hSourceFile, LPCTSTR lpszSourceName, LPCTSTR lpszDestName, BOOL bXORStreams );

ULONG DECLSPEC_NAVNUTIL PolicyCheckHideDrives( ULONG * lpdwDrives );
ULONG DECLSPEC_NAVNUTIL PolicyCheckRunOnly( LPTSTR lpszAppName, ULONG * lpbAllowed );
ULONG DECLSPEC_NAVNUTIL PolicyCheckNoEntireNetwork( ULONG * lpbPolicyFlag );

ULONG DECLSPEC_NAVNUTIL ProcessIsNormalUser( ULONG * lpbNormalUser );
ULONG DECLSPEC_NAVNUTIL ProcessCanUpdateRegKey( HANDLE hProcessToken, HANDLE hMainKey, LPCTSTR lpszRegKey, ULONG * lpbCanUpdate );
ULONG DECLSPEC_NAVNUTIL ProcessSetPrivilege ( HANDLE hToken, LPCTSTR lpszPrivilege, USHORT bEnablePriv ); 
ULONG DECLSPEC_NAVNUTIL ProcessTakeFileOwnership(LPTSTR lpFileName); 
ULONG DECLSPEC_NAVNUTIL ProcessIsInUserContext( ULONG * lpbNormalUser );
//*************************************************************************
#ifdef __cplusplus
};
#endif
#endif
