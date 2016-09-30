// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/ENGINE15/VCS/callfake.cpv   1.1   08 Sep 1997 11:09:28   CNACHEN  $
//
// Description:
//      Contains file handling callback routines for PAMA.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/ENGINE15/VCS/callfake.cpv  $
// 
//    Rev 1.1   08 Sep 1997 11:09:28   CNACHEN
// Added DRelinquishControl function.
// 
//    Rev 1.0   04 Jun 1997 11:39:04   CNACHEN
// Initial revision.
// 
//    Rev 1.2   27 May 1997 14:17:44   CNACHEN
// 
//    Rev 1.1   27 May 1997 14:11:44   CNACHEN
// Removed write of 0 byte code; no truncation done anyway.
// 
//    Rev 1.0   14 May 1997 17:22:58   CNACHEN
// Initial revision.
// 
//    Rev 1.1   31 Jan 1997 17:20:12   CNACHEN
// Added proper truncation for file write (0)
// 
//    Rev 1.0   23 Oct 1996 14:53:20   CNACHEN
// Initial revision.
// 
//************************************************************************

//#include "platform.h"			// NEW_UNIX
#include "avtypes.h"			// NEW_UNIX
//#include "ctsn.h"			// NEW_UNIX
#include "n30type.h"			// NEW_UNIX
#include "callback.h"
#include "navex.h"
#include "heurapi.h"
#include "navheur.h"

extern NAVEX_HEUR_T                 gstHeur;

HFILE DFileOpen(LPTSTR lpName, UINT uOpenMode)
{
    return(gstHeur.lpstCallBacks->FileOpen(lpName,uOpenMode));
}

HFILE DFileClose(HFILE hHandle)
{
    return(gstHeur.lpstCallBacks->FileClose(hHandle));
}

UINT DFileRead(HFILE hHandle, LPVOID lpBuffer, UINT uBytes)
{
    return(gstHeur.lpstCallBacks->FileRead(hHandle,lpBuffer,uBytes));
}

UINT DFileWrite(HFILE hHandle, LPVOID lpBuffer, UINT uBytes)
{
    return(gstHeur.lpstCallBacks->FileWrite(hHandle,lpBuffer,uBytes));
}


DWORD DFileSeek(HFILE hHandle, LONG dwOffset, int nFlag)
{
    return(gstHeur.lpstCallBacks->FileSeek(hHandle,dwOffset,nFlag));
}

DWORD DFileLength(HFILE hHandle)
{
    return(gstHeur.lpstCallBacks->FileSize(hHandle));
}

BOOL DFileDelete(LPTSTR lpFileName)
{
    return(gstHeur.lpstCallBacks->FileDelete(lpFileName));
}

HFILE DFileCreate(LPTSTR lpFileName, UINT uAttr)
{
    return(gstHeur.lpstCallBacks->FileCreate(lpFileName,uAttr));
}


BOOL CBProgress(WORD wProgress, DWORD dwCookie)
{
    return(gstHeur.lpstCallBacks->Progress(wProgress,dwCookie));
}

LPVOID PMemoryAlloc(DWORD uSize)
{
    return(gstHeur.lpstCallBacks->PermMemoryAlloc(uSize));
}

void PMemoryFree(LPVOID lpBuffer)
{
    gstHeur.lpstCallBacks->PermMemoryFree(lpBuffer);
}

#ifdef SYM_NLM

void DRelinquishControl(void)
{
    gstHeur.lpstCallBacks->RelinquishControl();
}

#endif
