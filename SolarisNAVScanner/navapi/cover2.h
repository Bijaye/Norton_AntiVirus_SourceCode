// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVAPI/VCS/cover2.h_v   1.1   11 Jun 1998 14:36:02   MKEATIN  $*
//
// Description:
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVAPI/VCS/cover2.h_v  $
// 
//    Rev 1.1   11 Jun 1998 14:36:02   MKEATIN
// Defined PAMFilexxx macros to be gstCallBacks.Filexxx calls.
//
//    Rev 1.0   21 May 1998 19:55:34   MKEATIN
// Initial revision.
//************************************************************************

#if !defined ( _COVER2_H )
#   define _COVER2_H

#   define PAMMemoryAlloc  PMemoryAlloc
#   define PAMMemoryFree   PMemoryFree

#   define PAMFileOpen     gstCallBacks.FileOpen
#   define PAMFileClose    gstCallBacks.FileClose
#   define PAMFileRead     gstCallBacks.FileRead
#   define PAMFileWrite    gstCallBacks.FileWrite
#   define PAMFileSeek     gstCallBacks.FileSeek
#   define PAMFileLength   gstCallBacks.FileSize
#   define PAMFileCreate   gstCallBacks.FileCreate

// Cover prototypes...

LPVOID  PMemoryAlloc(DWORD uSize);
void    PMemoryFree(LPVOID lpBuffer);
void    PAMRelinquishControl(void);

#endif                                  // defined _COVER2_H
