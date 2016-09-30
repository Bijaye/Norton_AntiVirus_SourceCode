// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/EMULATOR/VCS/callfake.cpv   1.0   23 Oct 1996 14:53:20   CNACHEN  $
//
// Description:
//      Contains file handling callback routines for PAMA.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/EMULATOR/VCS/callfake.cpv  $
// 
//    Rev 1.0   23 Oct 1996 14:53:20   CNACHEN
// Initial revision.
// 
//************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>

#include "avtypes.h"
#include "callfake.h"

HFILE DFileOpen(LPSTR lpName, UINT uOpenMode)
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
    extern DWORD gdwPagesRead;

    gdwPagesRead++;

    uRead = fread(lpBuffer,1,uBytes,(FILE *)hHandle);

    return(uRead);
}

UINT DFileWrite(HFILE hHandle, LPVOID lpBuffer, UINT uBytes)
{
	UINT uWrite;

    if (uBytes == 0)
    {
        WORD n = fileno((FILE *)hHandle);

        asm {
            mov ax, 0x4000
            mov cx, 0
            mov bx, n
            int 21h
            };
    }
    else
    {
        uWrite = fwrite(lpBuffer,1,uBytes,(FILE *)hHandle);
        if (uWrite != uBytes)
            return((UINT)-1);
    }

    return(uWrite);
}


DWORD DFileSeek(HFILE hHandle, LONG dwOffset, int nFlag)
{
    if (!fseek((FILE *)hHandle,dwOffset,nFlag))
        return(ftell((FILE *)hHandle));

    return((DWORD)-1);
}

DWORD DFileLength(HFILE hHandle)
{
    return(filelength(fileno((FILE *)hHandle)));
}

BOOL DFileDelete(LPSTR lpFileName)
{
    return(!unlink(lpFileName));        /* 1 on success, 0 on failure */
}

HFILE DFileCreate(LPSTR lpFileName, UINT uAttr)
{
    FILE *temp;

	temp = fopen(lpFileName,"wb");
    if (!temp)
        return (HFILE)0;
	fclose(temp);

	temp = (FILE *)DFileOpen(lpFileName,uAttr);

	if (!temp)
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

//	printf(" !F! ");
}


void PAMRelinquishControl(void)
{
}

