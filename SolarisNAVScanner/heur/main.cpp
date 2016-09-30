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
#include <process.h>
#include <memory.h>
#include <ctype.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "heurapi.h"
#include "filefind.h"
#include "callfake.h"

/* BUILD_SET DEFINE! */

BOOL    gbScanAll = FALSE;
BOOL    gbDeleteClean = FALSE;
BOOL    g_bDisplayVirusInfo = FALSE;
BOOL    g_bDisplayCleanInfo = FALSE;
int     g_nFileNum = 0;
int     g_nNumInfections = 0;
int     g_nHeurLevel = 0;

DWORD   g_dwNumDiskHits, g_dwNumIter,
	g_dwTotalDiskHits, g_dwTotalIter;

BOOL    g_bWriteCRC;


void do_file(char *filename, PAMLHANDLE hLocal)
{
	PAMSTATUS   pamStatus;
	HFILE           hFile;
	WORD            wVirusID;
    BOOL        bVirusFound;

	strupr(filename);

    
    g_nFileNum++;

    fprintf(stderr,"Files scanned: %d, Files detected: %d, Detection %%: %.2f%%)\n",
	   g_nFileNum,
	   g_nNumInfections,
	   (float)g_nNumInfections/(float)g_nFileNum * 100);

    printf("------------------------------------------------------------------------------\n");
    printf("%d: %-60s\n",g_nFileNum,filename);
    printf("------------------------------------------------------------------------------\n");

	fprintf(stderr,"Scan: %-60s\r\n",filename);

    g_dwNumDiskHits = g_dwNumIter = 0;

    hFile = DFileOpen(filename,READ_ONLY_FILE);
	if (hFile != (HFILE)-1)
	{
	g_bWriteCRC = TRUE;

	pamStatus = PAMScanFileH(hLocal, hFile, g_nHeurLevel, &bVirusFound, &wVirusID);
		DFileClose(hFile);
	}
	else
		pamStatus = PAMSTATUS_FILE_ERROR;

    if (pamStatus == PAMSTATUS_OK)
    {
	if (bVirusFound == TRUE)
	{
	    g_nNumInfections++;

	    printf("%d:Virus found\n",g_nFileNum);

	    if (g_bDisplayVirusInfo)
		BMPrintBehavior(&hLocal->stBM,g_nFileNum);
	}
	else
	{
	    if (gbDeleteClean == TRUE)
	    {
		printf("File deleted.\n");
//                unlink(filename);
	    }

	    if (g_bDisplayCleanInfo)
		BMPrintBehavior(&hLocal->stBM,g_nFileNum);
	}
    }
}

/*
typedef struct _WIN32_FIND_DATA { // wfd  

    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD    nFileSizeHigh;
    DWORD    nFileSizeLow;
    DWORD    dwReserved0;
    DWORD    dwReserved1;
    TCHAR    cFileName[ MAX_PATH ];
    TCHAR    cAlternateFileName[ 14 ];
} WIN32_FIND_DATA;
*/

void process_directory(char *dir, PAMLHANDLE hLocal)
{
    FILEFINDDATA_T      stFindFileData;
	TCHAR                           szTemp[256], szPath[256];
	BOOL                            bFileDone = FALSE, bOK;

	sprintf(szPath,"%s\\%s",dir,"*.*");

    bOK = FindFirstFile(szPath,&stFindFileData);

	while (bOK == TRUE)
	{
	sprintf(szTemp,"%s\\%s",dir,stFindFileData.szFileName);
		strupr(szTemp);

	if (!(stFindFileData.dwFileAttributes & FF_ATTRIB_DIREC))
		{
	    char *ptr, szTemp2[100];

	    ptr = strrchr(szTemp,'.');
	    if (ptr)
		strcpy(szTemp2,ptr);
	    else
		szTemp2[0] = 0;

	    if (!bFileDone)
			{
				do_file(szTemp,hLocal);
				if (gbScanAll == FALSE)
					bFileDone = TRUE;
			}
		}
		else
		{
	    if (strcmp(stFindFileData.szFileName,".") && strcmp(stFindFileData.szFileName,".."))
				process_directory(szTemp,hLocal);
		}
		
	bOK = FindNextFile(&stFindFileData);
	}

    CloseFileFind(&stFindFileData);
   

}

void parse_options(int argc, char *argv[])
{
	int i;

    gbScanAll = TRUE;
    gbDeleteClean = FALSE;

	for (i=1;i<argc;i++)
	{
	strupr(argv[i]);

	if (!strcmp(argv[i],"/D"))
		{
	    gbDeleteClean = TRUE;
		}

	if (!strncmp(argv[i],"/L",2))
		{
	    g_nHeurLevel = atoi(argv[i]+2);
	}

	if (!strcmp(argv[i],"/V"))
	{
	    g_bDisplayVirusInfo = TRUE;
	}

	if (!strcmp(argv[i],"/C"))
	{
	    g_bDisplayCleanInfo = TRUE;
	}


	}
}


int main(int argc, char *argv[])
{

	PAMGHANDLE      hGlobal;
	PAMLHANDLE      hLocal;
	PAMSTATUS       pamStatus;

    parse_options(argc,argv);

    printf("This program *appends* to a file called CRC.TXT.\n");
    printf("Heuristic level: %d\n",g_nHeurLevel);

    pamStatus = PAMGlobalInit("virscan5.dat",&hGlobal);
	if (pamStatus != PAMSTATUS_OK)
	{
		printf("Something didn't go right during global init: %d\n",pamStatus);
		return 0;
	}

    pamStatus = PAMLocalInit(hGlobal,
							 &hLocal);

	if (pamStatus != PAMSTATUS_OK)
	{
		printf("Something didn't go right during local init: %d\n",pamStatus);
		return 0;
	}

	process_directory(argv[1], hLocal);

	PAMLocalClose(hLocal);
	PAMGlobalClose(hGlobal);

    printf("------------------------------------------------------------------------------\n");
    if (g_nFileNum != 0)
    {
	printf("Summary\nFiles scanned: %d, Files detected: %d, Detection %%: %.2f%%)\n",
	       g_nFileNum,
	       g_nNumInfections,
	       (float)g_nNumInfections/(float)g_nFileNum * 100);
	printf("Average disk hits: %ld, Average iterations: %ld\n",
		g_dwTotalDiskHits/g_nFileNum,g_dwTotalIter/g_nFileNum);
    }
    printf("------------------------------------------------------------------------------\n");

    return 0;
}
