// Copyright 1998 Symantec Corporation. 
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

extern "C" {
//*************************************************************************

ULONG DECLSPEC_NAVNUTIL FileHasAlternateDataStreams( HANDLE  hSourceFile );
typedef ULONG (*PFNFILEHASALTERNATEDATASTREAMS) ( HANDLE hSourceFile );

ULONG DECLSPEC_NAVNUTIL CopyAlternateDataStreams( HANDLE hSourceFile, LPCTSTR lpszSourceName, LPCTSTR lpszDestName );
typedef ULONG (*PFNCOPYALTERNATEDATASTREAMS) ( HANDLE hSourceFile, LPCTSTR lpszSourceName, LPCTSTR lpszDestName );


//*************************************************************************
};
#endif
