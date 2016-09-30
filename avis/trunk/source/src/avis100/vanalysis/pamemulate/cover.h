// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/cover.h_v   1.1   17 May 1996 09:59:54   CNACHEN  $*
//
// Description:
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/cover.h_v  $
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

#include "callfake.h"   // DFile...


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
