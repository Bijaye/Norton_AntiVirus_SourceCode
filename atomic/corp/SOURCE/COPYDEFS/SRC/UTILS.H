//************************************************************************
// Copyright 1997 Symantec Corporation
// _______________________________________________________________________
//
// $Header:   S:/COPYDEFS/VCS/utils.h_v   1.0   19 Feb 1998 10:11:38   mdunn  $
//
// Description: Header file with prototypes of utility functions.
//
//************************************************************************
//  $Log:   S:/COPYDEFS/VCS/utils.h_v  $
// 
//    Rev 1.0   19 Feb 1998 10:11:38   mdunn
// Initial revision.
// 
//    Rev 1.0   25 Sep 1997 19:09:58   mdunn
// Initial revision.
//************************************************************************

LPTSTR  LLoadString         ( int nIDS );

LPTSTR  LNameAppendFile     ( LPTSTR szPath, LPCTSTR szFile );

LPTSTR  LNameStripFile      ( LPTSTR szPath );

LPTSTR  LNameReturnFile     ( LPTSTR szPath );

LPTSTR  LNameStripExtension ( LPTSTR szPath );

void    PrintMessage        ( int nIDS );

BOOL    GetNAVDir           ( LPTSTR szDir, DWORD dwSize );
