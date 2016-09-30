// Copyright 1992 Symantec, Peter Norton Group
//**********************************************************************
//
// $Header:   S:/include/VCS/errcode.h_v   1.0   06 Feb 1997 20:52:02   RFULLER  $
//
// Description:
//
// See Also:
//
//**********************************************************************
// $Log:   S:/include/VCS/errcode.h_v  $
// 
//    Rev 1.0   06 Feb 1997 20:52:02   RFULLER
// Initial revision
// 
//    Rev 1.0   16 Apr 1996 19:26:24   TSmith
// Initial revision.
//
//    Rev 1.0   15 Aug 1994 16:13:46   GDZIECI
// Baseline Mario from Vishnu with Win32 awk changes applied
//
//    Rev 1.1   09 Apr 1993 11:36:16   JMILLARD
// Add FAILED
//
//    Rev 1.0   16 Oct 1992 22:38:38   JMILLARD
// Initial revision.
//
//    Rev 1.0   16 Oct 1992 22:37:26   JMILLARD
// Initial revision.
//
/************************************************************************/
#if !defined(_ERRCODE_H)
#define _ERRCODE_H

//  .h files required to include this .h file

//
//  Global err codes
//
#pragma warning(disable: 4005)			// Turn off macro redef warning

#define OK      0       // no error return
#define FAILED  0xffff  // general failure return

#pragma warning(default: 4005)			// Turn on macro redef warning


// defines here for window handles


//**********************************************************************
//
//  Prototypes
//
//**********************************************************************



#endif  // _ERRCODE_H
