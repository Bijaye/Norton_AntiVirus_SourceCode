// Copyright 1998 Symantec Corporation. 
//*************************************************************************
// copystream.h - created 6/10/98 12:22:51 PM
//
// $Header:   S:/NAVNTUTL/VCS/copystream.h_v   1.0   10 Jun 1998 15:48:00   DBuches  $
//
// Description: Contains function delcarations for CopyStream routines.
//
//*************************************************************************
// $Log:   S:/NAVNTUTL/VCS/copystream.h_v  $
// 
//    Rev 1.0   10 Jun 1998 15:48:00   DBuches
// Initial revision.
//*************************************************************************

#ifndef _COPYSTREAM_H_
#define _COPYSTREAM_H_

//*************************************************************************


ULONG CopyStream( IN LPCWSTR lpwszStreamName, 
                  IN ULONG ulStreamNameLength,
                  IN ULONG ulStreamSize, 
                  IN LPCTSTR lpszSourceName, 
                  IN LPCTSTR lpszDestName );

//*************************************************************************

#endif
