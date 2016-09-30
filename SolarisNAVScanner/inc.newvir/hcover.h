// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/hcover.h_v   1.8   08 Jun 1998 19:07:30   AOONWAL  $*
//
// Description:
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/hcover.h_v  $
// 
//    Rev 1.8   08 Jun 1998 19:07:30   AOONWAL
// revert back to 1.4
// 
//    Rev 1.4   05 Jun 1998 19:58:56   DCHI
// Reverted to previous version because of NAVEX breakage.
// 
//    Rev 1.3   05 Jun 1998 16:58:38   MKEATIN
// Let's try this once again.
//
//    Rev 1.0   03 Jun 1997 17:45:48   CNACHEN
// Initial revision.
//
//    Rev 1.0   14 May 1997 17:14:36   CNACHEN
// Initial revision.
//
//    Rev 1.1   17 May 1996 09:59:54   CNACHEN
// Changed MemoryAlloc/MemoryFree to PMemoryAlloc/PMemoryFree to avoid
// collisions with NTK #includes.
//
//    Rev 1.0   27 Feb 1996 18:44:54   CNACHEN
// Initial revision.
//
//    Rev 1.0   27 Feb 1996 18:44:14   CNACHEN
// Initial revision.
//
//************************************************************************

#if !defined ( _COVER_H )
#   define _COVER_H

#   if !defined ( SYM_NLM )
#       define PAMMemoryAlloc  PMemoryAlloc
#       define PAMMemoryFree   PMemoryFree

#       if defined ( QUAKE2 )

#           if defined ( __cplusplus )
extern "C" {
#           endif

HFILE SYM_EXPORT WINAPI    NFileOpen (LPCTSTR lpFileName, UINT uMode);

#           if defined ( __cplusplus )
}
#           endif

#           define PAMFileOpen     NFileOpen

#       else

#           define PAMFileOpen     DFileOpen

#       endif

#       define PAMFileClose    DFileClose
#       define PAMFileRead     DFileRead
#       define PAMFileWrite    DFileWrite
#       define PAMFileSeek     DFileSeek
#       define PAMFileLength   DFileLength
#       define PAMFileCreate   DFileCreate

#   else

HFILE SYM_EXPORT WINAPI NV_FileOpen (LPTSTR lpName,UINT uOpenMode );

#       define PAMMemoryAlloc  PMemoryAlloc
#       define PAMMemoryFree   PMemoryFree
#       define PAMFileOpen     DFileOpen
#       define PAMFileClose    DFileClose
#       define PAMFileRead     DFileRead
#       define PAMFileWrite    DFileWrite
#       define PAMFileSeek     DFileSeek
#       define PAMFileLength   DFileLength
#       define PAMFileCreate   DFileCreate

#   endif

// Cover prototypes...

LPVOID  PMemoryAlloc(DWORD uSize);
void    PMemoryFree(LPVOID lpBuffer);
void    PAMRelinquishControl(void);

#endif
