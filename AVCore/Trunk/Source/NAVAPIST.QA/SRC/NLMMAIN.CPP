// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVAPIST.QA/VCS/nlmmain.cpv   1.1   20 Aug 1998 18:27:36   NMILLER  $
//
// Description:
//
//************************************************************************
// $Log:   S:/NAVAPIST.QA/VCS/nlmmain.cpv  $
// 
//    Rev 1.1   20 Aug 1998 18:27:36   NMILLER
// Removed SYMNAV dependency
// 
//    Rev 1.0   19 Aug 1998 14:34:26   NMILLER
// Initial revision.
// 
//************************************************************************


#include "platform.h"
#include "quakever.h"
#include "file.h"

#include "navapi.h"
#include <stdio.h>
//#include "nlmlog.h"		Disable logging
//#include "nwserver.h"		Disable file server

#define		STSCAN_LOG_FILENAME		_T("stscan.log")


// Cringe -- globals

HNAVVIRUS * lphVirus = NULL;
HNAVENGINE hEngine = NULL;




/////////////////////////////////////////////////////////////////////////////
// NAVENG File I/O Callbacks, Quake Implementation
// by Nathan Miller and David Hertel


DWORD MyOpen(LPVOID lpvFileInfo, DWORD dwOpenMode)
{
    DWORD dwReturn = FileOpen((LPCTSTR)lpvFileInfo, (UINT)dwOpenMode);

	return dwReturn;
} 
 
BOOL MyClose(DWORD dwHandle)
{
	return FileClose( (HFILE) dwHandle );
//    return FileClose(((HFILE) dwHandle) != HFILE_ERROR);
}

DWORD MySeek(DWORD dwHandle, LONG lOffset, int nFrom)
{
    return FileSeek((HFILE)dwHandle, lOffset, nFrom);
}

UINT MyRead(DWORD dwHandle, LPVOID lpvBuffer, UINT uBytesToRead)
{
    return FileRead((HFILE)dwHandle, lpvBuffer, uBytesToRead);
}

UINT MyWrite(DWORD dwHandle, LPVOID lpvBuffer, UINT uBytesToWrite)
{
    return FileWrite((HFILE)dwHandle, lpvBuffer, uBytesToWrite);
}

BOOL MyDelete(LPVOID lpvFileInfo)
{
    return FileDelete((LPCSTR)lpvFileInfo);
}

BOOL MyGetDateTime(DWORD dwHandle, UINT uType, LPWORD lpwDate, LPWORD lpwTime)
{
    UINT uQuakeType;
    FILETIME ft;
    switch (uType)
    {
    case NAV_TIME_LASTWRITE:
        uQuakeType = TIME_LASTWRITE;
        break;
    case NAV_TIME_LASTACCESS:
        uQuakeType = TIME_LASTACCESS;
        break;
    case NAV_TIME_CREATION:
        uQuakeType = TIME_CREATION;
        break;
    default:
        return FALSE;
    }
    if (FileGetTime((HFILE)dwHandle, uQuakeType, &ft) == FALSE)
        return FALSE;
    return FileTimeToDosTime(&ft, lpwDate, lpwTime);
} 

BOOL MySetDateTime(DWORD dwHandle, UINT uType, WORD wDate, WORD wTime)
{
    FILETIME ft;
    if (DosTimeToFileTime(wDate, wTime, &ft) == FALSE)
        return FALSE;
    switch (uType)
    {
    case NAV_TIME_LASTWRITE:
        return FileSetTime((HFILE)dwHandle, TIME_LASTWRITE, &ft);
    case NAV_TIME_LASTACCESS:
        return FileSetTime((HFILE)dwHandle, TIME_LASTACCESS, &ft);
    case NAV_TIME_CREATION:
        return FileSetTime((HFILE)dwHandle, TIME_CREATION, &ft);
    default:
        return FALSE;
    }
}

/*

// Functions for Scan and Repair of a directory

void	ScanAndRepairFile(LPTSTR lpszFileName, BOOL bDeleteUnrepairable)
{
	NAVSTATUS	dwReturn;
	DWORD		dwStringSize;
	TCHAR		szVirusName[SYM_MAX_PATH];
	TCHAR		szLogMessage[SYM_MAX_PATH];
	
	dwReturn	=	NAVScanFile(hEngine, (LPVOID) lpszFileName, TRUE, TRUE, lphVirus);

	sprintf(szLogMessage, _T("ScanFile %s returned %d"), lpszFileName, dwReturn);
	LogEvent(szLogMessage);
	printf("%s\n", szLogMessage);

	dwStringSize = SYM_MAX_PATH;

	dwReturn	= NAVGetVirusInfo(*lphVirus, (NAVVIRINFO) 0, szVirusName, &dwStringSize);

	if (dwReturn == 0)
		sprintf(szLogMessage, _T("Virus Name -  %s"), szVirusName);
	else
		sprintf(szLogMessage,_T("No virus detected"));
	LogEvent(szLogMessage);
	printf("%s\n", szLogMessage);

	NAVFreeVirusHandle(*lphVirus);

	dwReturn	=	NAVRepairFile(hEngine, (LPVOID) lpszFileName, TRUE);

	sprintf(szLogMessage, _T("RepairFile %s returned %d"), lpszFileName, dwReturn);
	LogEvent(szLogMessage);
	printf("%s\n", szLogMessage);

	if ( ( NAV_CANT_REPAIR == (NAVSTATUS) dwReturn ) && bDeleteUnrepairable )
	{
		dwReturn	=	NAVDeleteFile(hEngine, (LPVOID) lpszFileName, TRUE);

		sprintf(szLogMessage, _T("Unrepairable virus.  DeleteFile %s returned %d"), lpszFileName, dwReturn);
		LogEvent(szLogMessage);
		printf("%s\n", szLogMessage);
	}
	else
	{

		dwReturn	=	NAVScanFile(hEngine, (LPVOID) lpszFileName, TRUE, TRUE, lphVirus);

		sprintf(szLogMessage, _T("ScanFile %s returned %d"), lpszFileName, dwReturn);
		LogEvent(szLogMessage);
			printf("%s\n", szLogMessage);

		dwStringSize = SYM_MAX_PATH;

		dwReturn	= NAVGetVirusInfo(*lphVirus, (NAVVIRINFO) 0, szVirusName, &dwStringSize);

		if (dwReturn == 0)
			sprintf(szLogMessage, _T("Virus Name -  %s"), szVirusName);
		else
			sprintf(szLogMessage, _T("No virus detected"));
		LogEvent(szLogMessage);
		printf("%s\n", szLogMessage);

		NAVFreeVirusHandle(*lphVirus);
	}
}


void	ScanAndRepairDirectory(BOOL bDeleteUnrepairable)
{
	char szRootDir[SYM_MAX_PATH];
	char *	lpszReturnedFile = NULL;
	InitLog(STSCAN_LOG_FILENAME);

	printf("Root directory : ");
	scanf("%s", szRootDir);
	printf("Getting directory listing...\n");

	InitFileServer(szRootDir);

	printf("Commencing scan and repair\n");

	// request the first file
	if (NULL != (lpszReturnedFile = RequestFile(FALSE)))
	{
		ScanAndRepairFile(lpszReturnedFile, bDeleteUnrepairable);
		free(lpszReturnedFile);
	}

	while (NULL != (lpszReturnedFile = RequestFile(TRUE)))
	{
		ScanAndRepairFile(lpszReturnedFile, bDeleteUnrepairable);
		free(lpszReturnedFile);
	}

	CloseLog();

	return;
}

*/

char *	ReadLineFromFile(FILE * lpFile)
{
	char * szStringRead = malloc(SYM_MAX_PATH);

	if (lpFile)
		return fgets(szStringRead, SYM_MAX_PATH, lpFile);
	else
		return NULL;
}



// This is code I hope nobody ever sees.  Particularly the huge switch statement
// which makes me somewhat queasy to think that I wrote it.

int main(void)
{
	FILE * lpFile;
	char * szFileLine;
	char cCommand;
	NAVSTATUS			dwReturn;
	NAVFILEIO			stCallbacks;
	BOOL				bDone				= FALSE;
	TCHAR				cMenuOpt			= 'Z';
	HNAVVIRUSDEF		hVirusEntry			= NULL;
	HNAVVIRUSDEFTABLE	hVirusDefTable		= NULL;
	LPTSTR				lpszResultBuffer	= malloc(SYM_MAX_PATH);

	hEngine		= NULL;
	lphVirus	= malloc(sizeof(HNAVVIRUS));
	*lphVirus	= NULL;

	stCallbacks.Open		= MyOpen;
	stCallbacks.Close		= MyClose;
	stCallbacks.Seek		= MySeek;
	stCallbacks.Read		= MyRead;
	stCallbacks.Write		= MyWrite;
	stCallbacks.Delete		= MyDelete;
	stCallbacks.GetDateTime = MyGetDateTime;
	stCallbacks.SetDateTime = MySetDateTime;

	lpFile = fopen("sys:\\stscan\\script.txt", "r");

	if (lpFile)
	{
		szFileLine = ReadLineFromFile(lpFile);
		while (szFileLine)
		{
			strupr(szFileLine);

			switch (szFileLine[0])
			{
			case 'A':
				{
					LPTSTR szDefsPath = malloc(SYM_MAX_PATH);
					LPTSTR szNavexInf = malloc(SYM_MAX_PATH);
					WORD wHeuristicLevel = 0;
					WORD wFlags = 0;
					NAVSTATUS dwNAVStatus;

					sscanf(szFileLine, "%c %s %s %d %d", &cCommand, szDefsPath, szNavexInf, &wHeuristicLevel, &wFlags);

					if (szNavexInf[0] == '0')
						szNavexInf = NULL;

					hEngine = NAVEngineInit(szDefsPath, szNavexInf, &stCallbacks, wHeuristicLevel, 	
						wFlags, &dwNAVStatus);

					printf("NAVEngineInit(%s, NULL, &stCallbacks, %d, %d, &dwNAVStatus) returned 0x%x\n",
						szDefsPath, wHeuristicLevel, wFlags, hEngine);
					printf("dwNAVStatus points to %d\n", dwNAVStatus);

					free(szDefsPath);
					free(szNavexInf);
				}
				break;
			case 'B':
				{
					dwReturn = NAVEngineClose(hEngine);

					printf("NAVEngineClose(0x%x) returned %d\n", hEngine, dwReturn);
				}
				break;
			case 'C':
				{
					TCHAR cDriveLetter[5];

					sscanf(szFileLine, "%c %s", &cCommand, cDriveLetter);

					dwReturn = NAVScanBoot(hEngine, cDriveLetter[0], lphVirus);

					printf("NAVScanBoot(0x%x, %c, 0x%x) returned %d\n",
						hEngine, cDriveLetter[0], lphVirus, dwReturn);
				}
				break;
			case 'D':
				{
					UINT uPhysDriveNum = 0;

					sscanf(szFileLine, "%c %d", &cCommand, &uPhysDriveNum);

					dwReturn = NAVScanMasterBoot(hEngine, uPhysDriveNum, lphVirus);

					printf("NAVScanMasterBoot(0x%x, %d, 0x%x) returned %d\n",
						hEngine, uPhysDriveNum, lphVirus, dwReturn);
				}
				break;
			case 'E':
				{
					dwReturn = NAVScanMemory(hEngine, lphVirus);

					printf("NAVScanMemory(0x%x, 0x%x) returned %d\n",
						hEngine, lphVirus, dwReturn);
				}
				break;
			case 'F':
				{
					TCHAR cDriveLetter = 'c';

					sscanf(szFileLine, "%c %c", &cCommand, &cDriveLetter);

					dwReturn = NAVRepairBoot(hEngine, cDriveLetter);

					printf("NAVRepairBoot(0x%x, %c) returned %d\n",
						hEngine, cDriveLetter, dwReturn);
				}
				break;
			case 'G':
				{
					UINT uPhysDriveNum = 0;

					sscanf(szFileLine, "%c %d", &cCommand, &uPhysDriveNum);

					dwReturn = NAVRepairMasterBoot(hEngine, uPhysDriveNum);

					printf("NAVRepairMasterBoot(0x%x, %d) returned %d\n",
						hEngine, uPhysDriveNum, dwReturn);
				}
				break;
			case 'H':
				{
					dwReturn = NAVFreeVirusHandle(*lphVirus);

					printf("NAVFreeVirusHandle(0x%x) returned %d\n",
						*lphVirus, dwReturn);
				}
				break;
			case 'I':
				{
					DWORD	dwNumEntries = 0;

					dwReturn = NAVGetVirusDefCount(hEngine, &dwNumEntries);

					printf("NAVGetVirusDefCount(0x%x, 0x%x) returned %d\n",
						hEngine, &dwNumEntries, dwReturn);
					printf("lpdwNumEntries points to %d\n", dwNumEntries);
				}
				break;
			case 'J':
				{
					UINT	uEntryNum = 0;

					sscanf(szFileLine, "%c %d", &cCommand, &uEntryNum);

					hVirusEntry = NAVLoadVirusDef(hEngine, uEntryNum);

					printf("NAVLoadVirusDef(0x%x, %d) returned 0x%x\n",
						hEngine, uEntryNum, hVirusEntry);
				}
				break;
			case 'K':
				{
					NAVReleaseVirusDef(hVirusEntry);

					printf("NAVReleaseVirusDef(0x%x) called (no return type)\n", hVirusEntry);
				}
				break;
			case 'L':
				{
					long	nVirusInfo = 0;
					DWORD	dwBufferSize = SYM_MAX_PATH;
					
					sscanf(szFileLine, "%c %d", &cCommand, &nVirusInfo);

					dwReturn = NAVGetVirusDefInfo(hVirusEntry, (NAVVIRINFO) nVirusInfo, lpszResultBuffer, &dwBufferSize);

					printf("NAVGetVirusDefInfo(0x%x, %d) returned %d\n",
						hVirusEntry, nVirusInfo, dwReturn);
					printf("Result Buffer contains '%s'\n", lpszResultBuffer);

					lpszResultBuffer[0] = NULL;
				}
				break;
			case 'M':
				{
					TCHAR	szFileName[SYM_MAX_PATH];

					sscanf(szFileLine, "%c %s", &cCommand, szFileName);

					dwReturn = NAVScanFile(hEngine, (LPVOID) szFileName, TRUE, TRUE, lphVirus);


					printf("NAVScanFile(0x%x, %s, 0x%x) returned %d\n",
						hEngine, szFileName, lphVirus, dwReturn);
				}
				break;
			case 'N':
				{
					TCHAR	szFileName[SYM_MAX_PATH];

					sscanf(szFileLine, "%c %s", &cCommand, szFileName);

					dwReturn = NAVRepairFile(hEngine, (LPVOID) szFileName, TRUE);

					printf("NAVRepairFile(0x%x, %s) returned %d\n",
						hEngine, szFileName, dwReturn);
				}
				break;
			case 'O':
				{
					long	nVirusInfo = 0;
					DWORD	dwBufferSize = SYM_MAX_PATH;
					
					sscanf(szFileLine, "%c %d", &cCommand, &nVirusInfo);

					dwReturn = NAVGetVirusInfo(*lphVirus, (NAVVIRINFO) nVirusInfo, lpszResultBuffer, &dwBufferSize);


					printf("NAVGetVirusInfo(0x%x, %d) returned %d\n", lphVirus, nVirusInfo, dwReturn);
					printf("Result Buffer contains '%s'\n", lpszResultBuffer);

					lpszResultBuffer[0] = 0;
				}
				break;
			case 'P':
				{
					hVirusDefTable = NAVLoadVirusDefTable( hEngine );

					printf("NAVLoadVirusDefTable(0x%x) returned 0x%x\n", hEngine, hVirusDefTable);
				}
				break;
			case 'Q':
				{
					NAVReleaseVirusDefTable(hVirusDefTable);

					printf("NAVReleaseVirusDefTable(0x%x) called (no return type)\n", hVirusDefTable);
				}
				break;
			case 'R':
				{
					DWORD	dwNumEntries = 0;

					dwReturn = NAVGetVirusDefTableCount(hVirusDefTable, &dwNumEntries);

					printf("NAVGetVirusDefCount(0x%x) returned %d\n", hVirusDefTable, dwReturn);
					printf("lpdwNumEntries points to %d.\n", dwNumEntries);
				}
				break;
			case 'S':
				{
					long	nVirusInfo = 0, nIndex = 0;
					DWORD	dwBufferSize = SYM_MAX_PATH;

					sscanf(szFileLine, "%c %d %d", &cCommand, &nIndex, &nVirusInfo);

					dwReturn = NAVGetVirusDefTableInfo(hVirusDefTable, nIndex, 
										(NAVVIRINFO) nVirusInfo, lpszResultBuffer, &dwBufferSize);

					printf("NAVGetVirusDefTableInfo(0x%x, %d, %d) returned %d\n",
						hVirusDefTable, nIndex, nVirusInfo, dwReturn);
					printf("Result Buffer contains '%s'\n", lpszResultBuffer);

					lpszResultBuffer[0] = 0;
				}
				break;
/*			case 'T':
				{
					ScanAndRepairDirectory(FALSE);
				}
				break;
			case 'U':
				{
					ScanAndRepairDirectory(TRUE);
				}
				break;
*/
			case 'V':
				{
					dwReturn = NAVGetNavapiVersionString(lpszResultBuffer, SYM_MAX_PATH);

					printf("NAVGetNavapiVersionString() returned %d\n", dwReturn);
					printf("Result Buffer contains %s\n", lpszResultBuffer);

					lpszResultBuffer[0] = 0;
				}
				break;
			case 'W':
				{
					VIRUSDBINFO sVirusDBInfo;

					dwReturn = NAVGetVirusDBInfo(hEngine, &sVirusDBInfo);

					printf("NAVGetVirusDBInfo(0x%x, &sVirusDBInfo) returned %d\n", 
						hEngine, dwReturn);
					printf("VIRUSDBINFO contains Version %d, %d/%d/%d.\n", 
						sVirusDBInfo.dwVersion, sVirusDBInfo.wDay, sVirusDBInfo.wMonth,
						sVirusDBInfo.wYear);
				}
				break;
			default:
				{
					printf("Unrecognized command.  Skipping line.\n");
				}
				break;
			}

			free(szFileLine);
			szFileLine = ReadLineFromFile(lpFile);
		}

		fclose(lpFile);
	}
	else
		printf("No script file!\n");
	
	// Deallocate memory and bail.

	free(lpszResultBuffer);
	free(lphVirus);


	return 0;
}
