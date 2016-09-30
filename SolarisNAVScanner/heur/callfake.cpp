// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/HEUR/VCS/callfake.cpv   1.3   03 Jun 1997 11:07:16   CNACHEN  $
//
// Description:
//      Contains file handling callback routines for PAMA.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/HEUR/VCS/callfake.cpv  $
// 
//    Rev 1.3   03 Jun 1997 11:07:16   CNACHEN
// changed lpstrs to lptstrs
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if !defined(SYM_UNIX)
#include <io.h>
#endif

#include "avtypes.h"
#include "callfake.h"

HFILE DFileOpen(LPTSTR lpName, UINT uOpenMode)
{
	char    s[10];
    FILE    *temp;

    if (uOpenMode == 0)
	strcpy(s,"rb");
    if (uOpenMode & 1)
	strcpy(s,"wb");
    if (uOpenMode & 2)
	strcpy(s,"rb+");

	temp = fopen(lpName,s);
	if (temp == NULL)
		return((HFILE)-1);

    return((HFILE)temp);
}

HFILE DFileClose(HFILE hHandle)
{
    fclose((FILE *)hHandle);
    return(hHandle);
}

UINT DFileRead(HFILE hHandle, LPVOID lpBuffer, UINT uBytes)
{
    UINT uRead;

    uRead = fread(lpBuffer,1,uBytes,(FILE *)hHandle);

#if !defined(SYM_UNIX)
    {
	extern DWORD g_dwNumDiskHits;

	g_dwNumDiskHits++;
    }
#endif

    return(uRead);
}

UINT DFileWrite(HFILE hHandle, LPVOID lpBuffer, UINT uBytes)
{
	UINT uWrite;

    if (uBytes != 0)
    {
	uWrite = fwrite(lpBuffer,1,uBytes,(FILE *)hHandle);
	if (uWrite != uBytes)
	    return((UINT)-1);

	return(uWrite);
    }

    return((UINT)-1);
}


DWORD DFileSeek(HFILE hHandle, LONG dwOffset, int nFlag)
{
    if (!fseek((FILE *)hHandle,dwOffset,nFlag))
	return(ftell((FILE *)hHandle));

    return((DWORD)-1);
}

DWORD DFileLength(HFILE hHandle)
{
#if defined(SYM_UNIX)
    DWORD dwCur = ftell((FILE *)hHandle);
    fseek((FILE *)hHandle,0,SEEK_END);
    DWORD dwEnd = ftell((FILE *)hHandle);
    fseek((FILE *)hHandle,dwCur,SEEK_SET);
    return(dwEnd);
#else
    return(filelength(fileno((FILE *)hHandle)));
#endif
}

BOOL DFileDelete(LPTSTR lpFileName)
{
#if defined(SYM_UNIX)
    return(!remove(lpFileName));        /* 1 on success, 0 on failure */
#else
    return(!unlink(lpFileName));        /* 1 on success, 0 on failure */
#endif
}

HFILE DFileCreate(LPTSTR lpFileName, UINT uAttr)
{
    FILE *temp;

	temp = fopen(lpFileName,"wb");
    if (NULL == temp)
	return(NULL);
    fclose(temp);

    temp = (FILE *)DFileOpen(lpFileName,uAttr);

	if (NULL == temp)
		return((HFILE)-1);
	else
	return((HFILE)temp);

}


BOOL CBProgress(WORD wProgress, DWORD dwCookie)
{
	if (dwCookie)
		return(wProgress);
	else
		return(0);
}

LPVOID PMemoryAlloc(DWORD uSize)
{
	LPVOID x;

    x = malloc((UINT)uSize);

	return(x);
}

void PMemoryFree(LPVOID lpBuffer)
{
    free(lpBuffer);

}


void PAMRelinquishControl(void)
{
}

