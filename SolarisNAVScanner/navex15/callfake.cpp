// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVEX/VCS/callfake.cpv   1.1   13 Apr 1998 17:35:18   CEATON  $
//
// Description:
//      Contains file handling callback routines for PAMA.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVEX/VCS/callfake.cpv  $
// 
//    Rev 1.1   13 Apr 1998 17:35:18   CEATON
// Modified memory flags for GlobalAlloc() from GMEM_FIXED to GMEM_MOVEABLE.
// 
// 
//    Rev 1.0   03 Nov 1997 17:13:20   CNACHEN
// Initial revision.
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

#include "platform.h"
#include "ctsn.h"
#include "callback.h"
#include "navex.h"
#include "heurapi.h"
#include "navheur1.h"

#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>

extern NAVEX_HEUR_T                 gstHeur;

HFILE DFileOpen(LPTSTR lpName, UINT uOpenMode)
{
    if (gstHeur.lpstCallBacks != NULL)
        return(gstHeur.lpstCallBacks->FileOpen(lpName,uOpenMode));
    else
    {
#if defined(SYM_WIN16)
        OFSTRUCT    stOF;
        HFILE       hFile;

        hFile =  OpenFile(lpName,
                          &stOF,
                          OF_READ | OF_SHARE_DENY_WRITE);

        return(hFile);
#else
        return(open(lpName,uOpenMode | _O_BINARY));
#endif
    }
}

HFILE DFileClose(HFILE hHandle)
{
    if (gstHeur.lpstCallBacks != NULL)
        return(gstHeur.lpstCallBacks->FileClose(hHandle));
    else
    {
#if defined(SYM_WIN16)
        return(_lclose(hHandle));
#else
        return(close(hHandle));
#endif
    }
}

UINT DFileRead(HFILE hHandle, LPVOID lpBuffer, UINT uBytes)
{
    if (gstHeur.lpstCallBacks != NULL)
        return(gstHeur.lpstCallBacks->FileRead(hHandle,lpBuffer,uBytes));
    else
    {
#if defined(SYM_WIN16)
    return(_lread(hHandle,
                  lpBuffer,
                  uBytes));
#else
        return(read(hHandle,lpBuffer,uBytes));
#endif
    }
}

UINT DFileWrite(HFILE hHandle, LPVOID lpBuffer, UINT uBytes)
{
    if (gstHeur.lpstCallBacks != NULL)
        return(gstHeur.lpstCallBacks->FileWrite(hHandle,lpBuffer,uBytes));
    else
        return(0xFFFFU);
}


DWORD DFileSeek(HFILE hHandle, LONG dwOffset, int nFlag)
{
    if (gstHeur.lpstCallBacks != NULL)
        return(gstHeur.lpstCallBacks->FileSeek(hHandle,dwOffset,nFlag));
    else
    {
#if defined(SYM_WIN16)
    return(_llseek(hHandle,
                   dwOffset,
                   nFlag));
#else
        return(lseek(hHandle,dwOffset,nFlag));
#endif
    }
}

DWORD DFileLength(HFILE hHandle)
{
    if (gstHeur.lpstCallBacks != NULL)
        return(gstHeur.lpstCallBacks->FileSize(hHandle));
    else
    {
        return(0xFFFFFFFF);
    }
}

BOOL DFileDelete(LPTSTR lpFileName)
{
    if (gstHeur.lpstCallBacks != NULL)
        return(gstHeur.lpstCallBacks->FileDelete(lpFileName));
    else
        return(FALSE);
}

HFILE DFileCreate(LPTSTR lpFileName, UINT uAttr)
{
    if (gstHeur.lpstCallBacks != NULL)
        return(gstHeur.lpstCallBacks->FileCreate(lpFileName,uAttr));
    else
        return((HFILE)-1);
}


BOOL CBProgress(WORD wProgress, DWORD dwCookie)
{
    if (gstHeur.lpstCallBacks != NULL)
        return(gstHeur.lpstCallBacks->Progress(wProgress,dwCookie));
    else
        return(FALSE);
}

LPVOID PMemoryAlloc(DWORD uSize)
{
    if (gstHeur.lpstCallBacks != NULL)
        return(gstHeur.lpstCallBacks->PermMemoryAlloc(uSize));
    else
    {
#if defined(SYM_WIN16)
        LPHANDLE        lpHandle;
        HANDLE          hMemory;

        hMemory =  GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT,uSize + sizeof(HANDLE));
        if (hMemory == NULL)
            return(NULL);

        lpHandle = (HANDLE FAR *)GlobalLock(hMemory);
        *lpHandle = hMemory;
        lpHandle++;

        return((LPVOID)lpHandle);
#else
        return((LPVOID)malloc(uSize));
#endif
    }
}

void PMemoryFree(LPVOID lpBuffer)
{
    if (gstHeur.lpstCallBacks != NULL)
        gstHeur.lpstCallBacks->PermMemoryFree(lpBuffer);
    else
    {
#if defined(SYM_WIN16)
        LPHANDLE            lpHandle;
        HANDLE              hHandle;

        lpHandle = (LPHANDLE)lpBuffer;
        lpHandle--;
        hHandle = *lpHandle;

        GlobalUnlock(hHandle);
        GlobalFree(hHandle);
#else
        free(lpBuffer);
#endif
    }
}


