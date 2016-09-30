// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/HEUR/VCS/main.cpv   1.5   10 Jun 1998 14:50:58   CNACHEN  $
//
// Description:
//
//  Contains code for the bloodhound test executable.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/HEUR/VCS/main.cpv  $
// 
//    Rev 1.5   10 Jun 1998 14:50:58   CNACHEN
// 
//    Rev 1.4   12 Nov 1997 18:00:28   CNACHEN
// Added g_bWriteCRC variable for heurc.
// 
//    Rev 1.3   12 Nov 1997 17:30:10   CNACHEN
// 
//    Rev 1.3   23 Jun 1997 12:02:12   CNACHEN
// virscan2 ->virscan5
// 
//    Rev 1.2   29 May 1997 13:30:30   CNACHEN
// Added support for heuristic sensitivity level
// 
//    Rev 1.1   27 May 1997 14:45:30   CNACHEN
// 
//    Rev 1.0   14 May 1997 17:23:16   CNACHEN
// Initial revision.
// 
//************************************************************************

#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "avtypes.h"
#include "callback.h"
#include "n30type.h"
#include "navex.h"
#include "callfake.h"


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
//#include <io.h>		

#include "avtypes.h"
#include "callfake.h"

HFILE DDFileOpen(LPTSTR lpName, UINT uOpenMode)
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

HFILE DDFileClose(HFILE hHandle)
{
    fclose((FILE *)hHandle);
    return(hHandle);
}

UINT DDFileRead(HFILE hHandle, LPVOID lpBuffer, UINT uBytes)
{
    UINT uRead;

    uRead = fread(lpBuffer,1,uBytes,(FILE *)hHandle);

    return(uRead);
}

UINT DDFileWrite(HFILE hHandle, LPVOID lpBuffer, UINT uBytes)
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


DWORD DDFileSeek(HFILE hHandle, LONG dwOffset, int nFlag)
{
    if (!fseek((FILE *)hHandle,dwOffset,nFlag))
        return(ftell((FILE *)hHandle));

    return((DWORD)-1);
}

DWORD DDFileSize(HFILE hHandle)
{
	DWORD dwCur = ftell((FILE *)hHandle);
	fseek((FILE *)hHandle,0,SEEK_END);
	DWORD dwEnd = ftell((FILE *)hHandle);
	fseek((FILE *)hHandle,dwCur,SEEK_SET);
	return(dwEnd);
}

BOOL DDFileDelete(LPTSTR lpFileName)
{
    return(!remove(lpFileName));        /* 1 on success, 0 on failure */
}

HFILE DDFileCreate(LPTSTR lpFileName, UINT uAttr)
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


BOOL DDProgress(WORD wProgress, DWORD dwCookie)
{
	if (dwCookie)
		return(wProgress);
	else
		return(0);
}

LPVOID DDMemoryAlloc(DWORD uSize)
{
	LPVOID x;

    x = malloc((UINT)uSize);

	return(x);
}

void DDMemoryFree(LPVOID lpBuffer)
{
    free(lpBuffer);

}


void DDRelinquishControl(void)
{
}

void DDGetConfigInfo
(
	LPTSTR		lpszKey,
	LPVOID		lpvResult,
	UINT		uMaxLen,
	LPBOOL		lpbImp,
	LPVOID		lpvCookie
)
{
	*lpbImp = FALSE;
}


/* BUILD_SET DEFINE! */

void do_file(char *filename,LPCALLBACKREV1 lpstCallBacks)
{
    EXTSTATUS   extStatus;
	HFILE		hFile;
	WORD		wVirusID;
    BYTE        abyInfection[1024], abyWork[4096];

    hFile = lpstCallBacks->FileOpen(filename,READ_ONLY_FILE);
	if (hFile != (HFILE)-1)
	{
        extStatus = EXTScanFile(lpstCallBacks,
                                filename,
                                hFile,
                                abyInfection,
                                abyWork,
                                3,
                                &wVirusID);

        lpstCallBacks->FileClose(hFile);
	}
	else
        extStatus = EXTSTATUS_FILE_ERROR;

    if (extStatus == EXTSTATUS_VIRUS_FOUND)
    {
        printf("Virus found\n");
    }

    printf("Status was %d\n",extStatus);
}

int main(int argc, char *argv[])
{
    EXTSTATUS           extStatus;

    printf("Hello!\n");
    fflush(stdout);

    CALLBACKREV1        stCB = {
                                DDFileOpen,
                                DDFileClose,
                                DDFileRead,
                                DDFileWrite,
                                DDFileSeek,
                                DDFileSize,
				NULL,
				NULL,
                                DDFileDelete,
				NULL,
				NULL,
                                DDFileCreate,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                DDMemoryAlloc,
                                DDMemoryFree,
                                DDMemoryAlloc,
                                DDMemoryFree,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
				NULL,
				NULL,
				NULL,
				NULL,
				NULL,
				DDGetConfigInfo
                               };
    WORD    wVersion;

	printf("start up call!\n");
	fflush(stdout);

    extStatus = EXTStartUp(&stCB,3,NULL,".","navex15.inf",&wVersion);

    printf("EXTStartUp = %d\n",extStatus);

    if (extStatus != EXTSTATUS_OK)
        return 0;

    do_file(argv[1], &stCB);

    EXTShutDown(&stCB);
    return 0;
}
