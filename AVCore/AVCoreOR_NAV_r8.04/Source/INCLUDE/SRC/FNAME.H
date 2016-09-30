// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/fname.h_v   1.2   11 Sep 1997 19:53:20   KSACKIN  $
//
// Description:
//      Filename manipulation and matching routine prototypes.
//
// Contains:
//
// See Also:
//      NAVFAMLY ( or AVAPI1 ) for function definitions.
//************************************************************************
// $Log:   S:/INCLUDE/VCS/fname.h_v  $
// 
//    Rev 1.2   11 Sep 1997 19:53:20   KSACKIN
// Ported changes from QAKG.
//
//    Rev 1.1.1.1   02 Sep 1997 13:46:26   DALLEE
// Prototype for FileNameGetShortPathName().
//
//
//    Rev 1.1   11 May 1997 20:50:46   DALLEE
// Removed problematic GetLongName() from NTK and VXD.
//
//    Rev 1.0   11 May 1997 19:54:30   DALLEE
// Initial revision.
//************************************************************************

#ifndef __FNAME_H
#define __FNAME_H

//************************************************************************
// Prototypes

#ifdef __cplusplus
extern "C" {
#endif

LPTSTR WINAPI FileNameStrip( LPTSTR lpszPath );
LPTSTR WINAPI FileNameAppendSeparator( LPTSTR lpszPath );
LPTSTR WINAPI FileNameCat( LPTSTR lpszPath, LPCTSTR lpszFileName );
LPTSTR WINAPI FileNameSkipVolume( LPCTSTR lpszPath );
LPTSTR WINAPI FileNameSkipDirectory( LPCTSTR lpszPath );
LPTSTR WINAPI FileNameSkipSection( LPCTSTR lpszPath );
LPTSTR WINAPI FileNameToLower ( LPTSTR lpszFile );
LPTSTR WINAPI FileNameToUpper ( LPTSTR lpszFile );
BOOL WINAPI FileNameMatchSection ( LPCTSTR lpszWild, LPCTSTR lpszFile );

#if !defined(SYM_VXD) && !defined(SYM_NTK)
DWORD WINAPI FileNameGetLongPathName( LPCTSTR   lpszShort,
                              LPTSTR    lpszLong,
                              DWORD     cchBuffer );

DWORD WINAPI FileNameGetShortPathName( LPCTSTR   lpszLong,
                                       LPTSTR    lpszShort,
                                       DWORD     cchBuffer );
#endif

#ifdef __cplusplus
}
#endif

#endif // __FNAME_H
