// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVAPIST.QA/VCS/main.cpv   1.31   02 Dec 1998 19:11:26   cwong  $
//
// Description: Single threaded test app for NAVAPI
//
//************************************************************************
// $Log:   S:/NAVAPIST.QA/VCS/main.cpv  $
// 
//    Rev 1.31   02 Dec 1998 19:11:26   cwong
// Backed out rev 1.30 changes.  This is just 1.29 again.
// DefTable functions have been re-added to NAVAPI.NLM.
// 
//    Rev 1.29   17 Sep 1998 16:27:34   NMILLER
// Removed an extraneous ')'
// 
//    Rev 1.28   17 Sep 1998 15:20:56   NMILLER
// CARLIN changed bExecutable parameters to lpszExtensions as an immediate
// and speedy response to the recent API change.  Good work, CARLIN!!
// END CARLIN
// 
//    Rev 1.27   10 Sep 1998 15:23:46   mmendon
// Removed a function that was no being called.
//
//    Rev 1.26   10 Sep 1998 15:17:52   mmendon
// Consolidated multithread functions.
//
//    Rev 1.25   08 Sep 1998 16:48:18   NMILLER
// Flips the quake return codes on the Get/Set time calls to get a boolean
// value (Quake's TRUE is 0... grumble...)
//
//    Rev 1.24   04 Sep 1998 18:20:40   NMILLER
// Added an IsExecutable() check in a few more places (in the Multithread tests)
//
//    Rev 1.23   04 Sep 1998 18:14:24   NMILLER
// Also check the extension of the file for NAVRepairFile and NAVDeleteFile
//
//    Rev 1.22   02 Sep 1998 16:03:30   NMILLER
// Added code to check the extension of the file we are scanning, and pass
// in the correct value for bExecutable to NAVScanFile.
//
//    Rev 1.21   01 Sep 1998 17:01:52   NMILLER
// Adjusted some memory allocation techniques.
//
//    Rev 1.20   01 Sep 1998 08:06:50   mmendon
// Added multithreading for BR, MBR and memory
//
//    Rev 1.19   24 Aug 1998 22:27:12   NMILLER
// Fixed a typo.
//
//    Rev 1.18   24 Aug 1998 22:21:40   NMILLER
// Made changes to scan/repair directory functions to only attempt a
// repair on infected files, and to keep running totals of the number
// of files scanned, infections, repairs, and files deleted.
// Also fixed two bugs for NLM involving reading from the script file and
// closing the log file.
//
//    Rev 1.17   21 Aug 1998 18:23:16   NMILLER
// Added NLM support.
//
//    Rev 1.16   20 Aug 1998 18:27:26   NMILLER
// Removed SYMNAV dependency
//
//    Rev 1.15   20 Aug 1998 17:45:16   NMILLER
// Fixed a bug with Multi-threaded scans.
//
//    Rev 1.14   20 Aug 1998 13:57:14   NMILLER
// Compacted the main menu.
//
//    Rev 1.13   20 Aug 1998 10:40:14   NMILLER
// Added export types for the Win16 callbacks.
// This was required due to a change in NAVAPI.H
//
//    Rev 1.12   18 Aug 1998 17:58:34   mmendon
// Moved bracket out if #if defined block
//
//    Rev 1.11   18 Aug 1998 17:24:04   mmendon
// Added multithreading for file processing
//
//    Rev 1.10   17 Aug 1998 16:33:08   NMILLER
// Ripped out callback DLL stuff
//
//    Rev 1.9   17 Aug 1998 14:45:22   NMILLER
// Added logging and script files, fixed bug with calling into NAVAPI DLL
//
//    Rev 1.8   14 Aug 1998 12:13:26   NMILLER
// Added parsing of the command line for loading a custom callback DLL
//
//    Rev 1.7   13 Aug 1998 14:25:18   mmendon
// Added suport to dynamically load the callbacks dlls
//
//    Rev 1.6   13 Aug 1998 14:00:28   mmendon
// Added load library support for NAVAPI DLL
//
//    Rev 1.5   12 Aug 1998 17:18:28   NMILLER
// Fixed a problem with the usage of the Quake FileClose routine.
//
//    Rev 1.4   12 Aug 1998 17:00:22   NMILLER
// Added Delete all files in a directory, print out error codes in English
// instead of DWORDs, Removed dependency on APICall library (to support
// dynamic loading of NAVAPIXX.DLL), added support for NAVEX.INF files.
//
//    Rev 1.3   11 Aug 1998 14:01:46   NMILLER
// Fixed initialization of result buffer string.
//
//    Rev 1.2   10 Aug 1998 16:44:52   NMILLER
// Changed some %ld's back to %d's.
//
//    Rev 1.1   10 Aug 1998 14:14:50   NMILLER
// Changed %x and %d to %lx and %ld for DX compatibility.
//
//    Rev 1.0   10 Aug 1998 12:24:50   NMILLER
// Initial revision.
//
//************************************************************************

#include "platform.h"
#include "quakever.h"
#include "file.h"
#include <stdio.h>
#include <stdlib.h>
#include "navapi.h"

#ifndef SYM_NLM
#include "FileServ.h"
#include "main.h"
#endif


#ifdef SYM_WIN
    #include "windows.h"
#endif

#define         STSCAN_LOG_FILENAME             _T("stscan.log")

#ifdef SYM_WIN32
    #include <process.h>
    typedef unsigned (WINAPI *PBEGINTHREADEX_THREADFUNC)(LPVOID lpThreadParam);
    typedef unsigned *PBEGINTHREADEX_THREADID;
#endif

// To get OpenFile to work under netware ? 
#ifdef SYM_NLM
   #define OpenFile open
#endif


/////////////////////////////////////////////////////////////////////////////
// NAVENG File I/O Callbacks, Quake Implementation
// by Nathan Miller and David Hertel
BOOL NAVCALLBACK MyOpen(LPVOID lpvFileInfo, DWORD dwOpenMode, LPVOID FAR* lplpvHandle)
{
	HFILE hFile = FileOpen((LPCSTR)lpvFileInfo, (UINT)dwOpenMode);

	if (hFile == HFILE_ERROR)
	    return FALSE;

	*lplpvHandle = (LPVOID)hFile;
	return TRUE;
}
// returns a non-zero value for success

BOOL NAVCALLBACK MyClose(LPVOID lpvHandle)
{
	return (FileClose( (HFILE) lpvHandle) == NOERR ? TRUE : FALSE);
}

DWORD NAVCALLBACK MySeek(LPVOID lpvHandle, LONG lOffset, int nFrom)
{
    return FileSeek((HFILE)lpvHandle, lOffset, nFrom);
}

UINT NAVCALLBACK MyRead(LPVOID lpvHandle, LPVOID lpvBuffer, UINT uBytesToRead)
{
    return FileRead((HFILE)lpvHandle, lpvBuffer, uBytesToRead);
}

UINT NAVCALLBACK MyWrite(LPVOID lpvHandle, LPVOID lpvBuffer, UINT uBytesToWrite)
{
    return FileWrite((HFILE)lpvHandle, lpvBuffer, uBytesToWrite);
}

BOOL NAVCALLBACK MyDelete(LPVOID lpvFileInfo)
{
    return ( FileDelete((LPCSTR)lpvFileInfo) == NOERR ? TRUE : FALSE );
}

BOOL NAVCALLBACK MyGetDateTime(LPVOID lpvHandle, UINT uType, LPWORD lpwDate, LPWORD lpwTime)
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
    if (FileGetTime((HFILE)lpvHandle, uQuakeType, &ft) != NOERR)
	return FALSE;
    return FileTimeToDosTime(&ft, lpwDate, lpwTime);
}

BOOL NAVCALLBACK MySetDateTime(LPVOID lpvHandle, UINT uType, WORD wDate, WORD wTime)
{
    FILETIME ft;
    if (DosTimeToFileTime(wDate, wTime, &ft) == FALSE)
	return FALSE;
    switch (uType)
    {
    case NAV_TIME_LASTWRITE:
	return !( FileSetTime((HFILE)lpvHandle, TIME_LASTWRITE, &ft));
    case NAV_TIME_LASTACCESS:
	return !( FileSetTime((HFILE)lpvHandle, TIME_LASTACCESS, &ft));
    case NAV_TIME_CREATION:
	return !( FileSetTime((HFILE)lpvHandle, TIME_CREATION, &ft));
    default:
	return FALSE;
    }
}


#ifndef SYM_NLM
void    OutputMessage(LPTSTR lpszOutputString, CLog * pLog, BOOL bSuppressConsole = FALSE)
#else
void    OutputMessage(LPTSTR lpszOutputString, FILE * pLog, BOOL bSuppressConsole)
#endif
{
	if (pLog)
#ifndef SYM_NLM
		pLog->LogEvent(lpszOutputString);
#else
		fputs(lpszOutputString, pLog);
		fputs("\n", pLog);
#endif
	if (!bSuppressConsole)
	{
		printf(lpszOutputString);
		printf("\n");
	}
}


BOOL    NavstatusToString(NAVSTATUS dwStatus, LPTSTR lpszStringEquivalent)
{
	switch (dwStatus)
	{
	case NAV_OK:
		STRCPY(lpszStringEquivalent, "NAV_OK");
		break;
	case NAV_ERROR:
		STRCPY(lpszStringEquivalent, "NAV_ERROR");
		break;
	case NAV_INSUFFICIENT_BUFFER:
		STRCPY(lpszStringEquivalent, "NAV_INSUFFICIENT_BUFFER");
		break;
	case NAV_INVALID_ARG:
		STRCPY(lpszStringEquivalent, "NAV_INVALID_ARG");
		break;
	case NAV_MEMORY_ERROR:
		STRCPY(lpszStringEquivalent, "NAV_MEMORY_ERROR");
		break;
	case NAV_NO_ACCESS:
		STRCPY(lpszStringEquivalent, "NAV_NO_ACCESS");
		break;
	case NAV_CANT_REPAIR:
		STRCPY(lpszStringEquivalent, "NAV_CANT_REPAIR");
		break;
	case NAV_CANT_DELETE:
		STRCPY(lpszStringEquivalent, "NAV_CANT_DELETE");
		break;
	case NAV_ENGINE_IN_USE:
		STRCPY(lpszStringEquivalent, "NAV_ENGINE_IN_USE");
		break;
	case NAV_VXD_INIT_FAILURE:
		STRCPY(lpszStringEquivalent, "NAV_VXD_INIT_FAILURE");
		break;
	case NAV_DEFINITIONS_ERROR:
		STRCPY(lpszStringEquivalent, "NAV_DEFINITIONS_ERROR");
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

// **** CARLIN ****
// Returns extension of filename lpszFilename in lpszExt.

DWORD GetExtension(LPTSTR lpszExt, LPTSTR lpszFilename)
{
	int iIndex = STRLEN(lpszFilename);

	STRUPR(lpszFilename);

	while ( (iIndex > 0) && ( *(lpszFilename + iIndex) != '.') )
		iIndex--;
	
	if (iIndex > 0)
	{
		strcpy(lpszExt, lpszFilename + iIndex + 1);
		return 0;
	}
	else
		return -1;
}
// **** End CARLIN ****


// Starts at the end of the string, finds the ".", takes the extension
// and returns TRUE if it is an executable type

// No longer needed --NM

BOOL IsExecutable(LPTSTR lpszFilename)
{
	int     iIndex = STRLEN(lpszFilename);

	STRUPR(lpszFilename);

	while ( (iIndex > 0) && ( *(lpszFilename + iIndex) != '.') )
		iIndex--;

	if (strcmp (lpszFilename + iIndex + 1, "EXE") == 0)
		return TRUE;

	if (strcmp (lpszFilename + iIndex + 1, "COM") == 0)
		return TRUE;

	return FALSE;
}


#if defined(SYM_WIN32)

//////////////////////////////////////////////////////////////////////////
//
//    Multithreading functionality for Win32
//
//////////////////////////////////////////////////////////////////////////

//Initialize the global variables for keeping track of boot records and MBRs

void InitDriveLetterCounter()
{
    cCriticalSectionLock csl(&CritSectionDLetter);
    gwDriveLetterCounter = 0;
}

void InitDriveNumberCounter()
{
    cCriticalSectionLock csl(&CritSectionDNumber);
    gwDriveNumberCounter = 0;
}



void MTLaunchThreads(LPMT_FUNC_PARAMS lpFuncParams,
            DWORD NumThreads, LPVOID MTFunction)
{
    // This is not the most elegant way to do multi-threading, but
    // it works within the design and the time I had to implement this.
    // TODO: make this nicer.

    //Event object for signalling threads when okay to start processing files

    hMTSignal = NULL;
    hMTSignal = CreateEvent(NULL, true, false, NULL);
    if(!hMTSignal)
    {
        printf("Unable to create event object.");
        return;
    }

    //Because WaitForMultipleObjects can wait for a max of 64 threads
    //there are 2 thread arrays so up to 128 threads can be run
    HANDLE hThreads1[MAXIMUM_WAIT_OBJECTS];
	HANDLE hThreads2[MAXIMUM_WAIT_OBJECTS];
	int ThreadCounter = 0;
	int NumThreads1 = 0, NumThreads2 = 0;
    DWORD ThreadId1, ThreadId2;
    DWORD WaitResult1, WaitResult2;

      //init the thread array
      for(int i = 0; i < MAXIMUM_WAIT_OBJECTS; ++i)
	  {
	    hThreads1[i] = NULL;
		hThreads2[i] = NULL;
	  }

      //go through all of the files in the list

      while(ThreadCounter < NumThreads)
	  {
			// start an available thread
	    printf("Creating thread %d.\n", ThreadCounter);
	    hThreads1[NumThreads1] = (HANDLE)_beginthreadex(NULL, 0, (PBEGINTHREADEX_THREADFUNC)MTFunction,
						   (LPVOID)lpFuncParams,
						  0, (PBEGINTHREADEX_THREADID)&ThreadId1);
			if(!hThreads1[NumThreads1])
			{
		printf("Failed to create thread %d.\n", ThreadCounter);
				break;
			}
			++ThreadCounter;
			++NumThreads1;

			if(ThreadCounter < NumThreads)
			{       // start an available thread
		printf("Creating thread %d.\n", ThreadCounter);
		hThreads2[NumThreads2] = (HANDLE)_beginthreadex(NULL, 0, (PBEGINTHREADEX_THREADFUNC)MTFunction,
						    (LPVOID)lpFuncParams,
						   0, (PBEGINTHREADEX_THREADID)&ThreadId2);
				if(!hThreads2[NumThreads2])
				{
		    printf("Failed to create thread %d.\n", ThreadCounter);
					break;
				}
				++ThreadCounter;
				++NumThreads2;
			}
      }

      //Signal waiting threads to start processing
      SetEvent(hMTSignal);

      //wait for all threads to finish
      WaitResult1 = WaitForMultipleObjects(NumThreads1, hThreads1, TRUE, INFINITE);
	  WaitResult2 = WaitForMultipleObjects(NumThreads2, hThreads2, TRUE, INFINITE);

	  //close all handles
      int j;
      for(j= 0; j < NumThreads1; j++)
      {
	   if(hThreads1[j])
	   {
	       CloseHandle(hThreads1[j]);
	       hThreads1[j] = NULL;
	   }
      }
	  //close all handles
      for(j= 0; j < NumThreads2; j++)
      {
	   if(hThreads2[j])
	   {
	       CloseHandle(hThreads2[j]);
	       hThreads2[j] = NULL;
	   }
      }

    if(hMTSignal)
        CloseHandle(hMTSignal);
	return;
}



void MTReadyParams(HNAVENGINE hEngine, LPTSTR szRootDir, LPTSTR szOutputFile, LPNAVAPI_FUNC lpNavapiFuncs,
		    DWORD NumThreadReq, WORD FuncOption)
{

    if(!hEngine || !lpNavapiFuncs)
    {
        printf("Engine handle or function pointer is null\n");
        return;
    }

    LPMT_FUNC_PARAMS lpFuncParams = NULL;
    DWORD TotalThreads;
    DWORD NumThreads;
    LPVOID MTFunction;
    bool bFileOp = false;

    //Get function to be called
    switch(FuncOption)
    {
        case  SCAN_FILES:       MTFunction = MTScanFile;
                                bFileOp = true;
                                break;
        case  REPAIR_FILES:     MTFunction = MTRepairFile;
                                bFileOp = true;
                                break;
        case  DELETE_FILES:     MTFunction = MTDeleteFile;
                                bFileOp = true;
                                break;
        case  SCAN_BR:          MTFunction = MTScanBR;
                                InitDriveLetterCounter();
                                break;
        case  REPAIR_BR:        MTFunction = MTRepairBR;
                                InitDriveLetterCounter();
                                break;
        case  SCAN_MBR:         MTFunction = MTScanMBR;
                                InitDriveNumberCounter();
                                break;
        case  REPAIR_MBR:       MTFunction = MTRepairMBR;
                                InitDriveNumberCounter();
                                break;
        case  SCAN_MEMORY:      MTFunction = MTScanMemory;
                                break;
        default:                //error, so cleanup and return
                                printf("Invalid multithread function option.\n");
                                goto Cleanup;
    }

    //Init params for thread function
    lpFuncParams = new MT_FUNC_PARAMS;
    if(!lpFuncParams)
        goto Cleanup;  //error, so cleanup and return

    lpFuncParams->hEngine = hEngine;
    lpFuncParams->lpNavapiFuncs = lpNavapiFuncs;
    lpFuncParams->pLog = NULL;
    lpFuncParams->lpFileServer = NULL;

    //CLog is threadsafe so we can safely open a 2nd session to it
    if (szOutputFile)
        lpFuncParams->pLog  =       new CLog(szOutputFile);
	else
        lpFuncParams->pLog  =       new CLog(STSCAN_LOG_FILENAME);

    if(!lpFuncParams->pLog)
        goto Cleanup;  //error, so cleanup and return

    // Only need file server when files are being operated on
    if(bFileOp)
    {
        if(szRootDir)
        {
            printf("Getting directory listing...\n");
            lpFuncParams->lpFileServer = new CFileServer();
            if(!lpFuncParams->lpFileServer)
                goto Cleanup;  //error, so cleanup and return
            lpFuncParams->lpFileServer->ServerInit(szRootDir);
        }
        else  //directory is null, so clean up and return
        {
            printf("File directory is null.\n");
            goto Cleanup;
        }
    }

    //get the number of threads (least of requested, needed, and max allowed)
    TotalThreads = max(NumThreadReq, 1);
    NumThreads = min(TotalThreads, MAXIMUM_WAIT_OBJECTS * 2);
    if(bFileOp)
    {
        DWORD TotalFiles = lpFuncParams->lpFileServer->GetCount();
        NumThreads = min(NumThreads, TotalFiles);
    }

    MTLaunchThreads(lpFuncParams, NumThreads, MTFunction);

Cleanup:

    if(lpFuncParams)
    {
        if(lpFuncParams->pLog)
            delete lpFuncParams->pLog;
        if(lpFuncParams->lpFileServer)
            delete lpFuncParams->lpFileServer;
        delete lpFuncParams;
    }
	return;
}



void WINAPI MTScanFile(LPVOID lpFuncParams)
{
	NAVSTATUS       dwReturn;
    DWORD           dwBufferSize = SYM_MAX_PATH;
    TCHAR           szVirusName[SYM_MAX_PATH];
	TCHAR           szLogMessage[SYM_MAX_PATH];
    TCHAR           szStatusMsg[SYM_MAX_PATH];
	HNAVVIRUS       hVirus = NULL;
    LPTSTR  lpszReturnedFile = NULL;
	LPTSTR			lpszExt;
    LPMT_FUNC_PARAMS lpParams = (LPMT_FUNC_PARAMS)lpFuncParams;

    //wait for all threads to be created before starting processing
    WaitForSingleObject(hMTSignal, 10000);

	lpszExt = (LPTSTR) malloc(SYM_MAX_PATH);
	lpszExt[0] = NULL;

    //The thread will continue to grab files until the end of the list is reached
    //scans file, logs results, then cleans up

    while (NULL != (lpszReturnedFile = lpParams->lpFileServer->MTRequestFile()))
	{
		if (GetExtension(lpszExt, lpszReturnedFile) != 0)
			lpszExt[0] = NULL;
        
		dwReturn  = lpParams->lpNavapiFuncs->lpfnScanFile(lpParams->hEngine, (LPVOID) lpszReturnedFile,
                                                          lpszExt , TRUE, &hVirus);

        NavstatusToString(dwReturn, szStatusMsg);
        dwReturn = lpParams->lpNavapiFuncs->lpfnGetVirusInfo(hVirus, NAV_VI_VIRUS_NAME, szVirusName, &dwBufferSize);
        if (dwReturn == NAV_OK)
            sprintf(szLogMessage, _T("ScanFile %s returned %s. Virus Name -  %s"), lpszReturnedFile, szStatusMsg, szVirusName);
        else
            sprintf(szLogMessage, _T("ScanFile %s returned %s. No virus detected"), lpszReturnedFile, szStatusMsg);
        OutputMessage(szLogMessage, lpParams->pLog, TRUE);
        lpParams->lpNavapiFuncs->lpfnFreeVirusHandle(hVirus);
        delete [] lpszReturnedFile;
        dwBufferSize = SYM_MAX_PATH;
    }

	free(lpszExt);	
}


void WINAPI MTRepairFile(LPVOID lpFuncParams)
{
	NAVSTATUS       dwReturn;
    DWORD           dwBufferSize = SYM_MAX_PATH;
    TCHAR           szVirusName[SYM_MAX_PATH];
	TCHAR           szLogMessage[SYM_MAX_PATH];
    TCHAR           szStatusMsg[SYM_MAX_PATH];
	HNAVVIRUS       hVirus = NULL;
    LPTSTR  lpszReturnedFile = NULL;
	LPTSTR			lpszExt;	
    LPMT_FUNC_PARAMS lpParams = (LPMT_FUNC_PARAMS)lpFuncParams;

    //wait for all threads to be created before starting processing
    WaitForSingleObject(hMTSignal, 10000);

	lpszExt = (LPTSTR) malloc(SYM_MAX_PATH);
	lpszExt[0] = NULL;


    //The thread will continue to grab files until the end of the list is reached
    while (NULL != (lpszReturnedFile = lpParams->lpFileServer->MTRequestFile()))
	{
		if (GetExtension(lpszExt, lpszReturnedFile) != 0)
			lpszExt[0] = NULL;

		// attempt to repair file
		dwReturn = lpParams->lpNavapiFuncs->lpfnRepairFile(lpParams->hEngine, (LPVOID) lpszReturnedFile, lpszExt);
        NavstatusToString(dwReturn, szStatusMsg);
        sprintf(szLogMessage, _T("RepairFile %s returned %s"), lpszReturnedFile, szStatusMsg);
        OutputMessage(szLogMessage, lpParams->pLog, TRUE);

        // scan file after repair
        if(dwReturn == NAV_OK)
        {
            dwReturn  = lpParams->lpNavapiFuncs->lpfnScanFile(lpParams->hEngine, (LPVOID) lpszReturnedFile,
                                                              lpszExt, TRUE, &hVirus);
//			dwReturn  = lpParams->lpNavapiFuncs->lpfnScanFile(lpParams->hEngine, (LPVOID) lpszReturnedFile,
//                                                              IsExecutable(lpszReturnedFile), TRUE, &hVirus);

            NavstatusToString(dwReturn, szStatusMsg);
            dwReturn = lpParams->lpNavapiFuncs->lpfnGetVirusInfo(hVirus, NAV_VI_VIRUS_NAME, szVirusName, &dwBufferSize);
            if (dwReturn == NAV_OK)
                sprintf(szLogMessage, _T("ScanFile %s returned %s. Virus Name -  %s"), lpszReturnedFile,
                                            szStatusMsg, szVirusName);
            else
            sprintf(szLogMessage, _T("ScanFile %s returned %s. No virus detected"), lpszReturnedFile, szStatusMsg);
        OutputMessage(szLogMessage, lpParams->pLog, TRUE);
	    lpParams->lpNavapiFuncs->lpfnFreeVirusHandle(hVirus);
			dwBufferSize = SYM_MAX_PATH;
        }
        delete [] lpszReturnedFile;
    }  //while

	free(lpszExt);		
}

void WINAPI MTDeleteFile(LPVOID lpFuncParams)
{
	NAVSTATUS       dwReturn;
	DWORD                   dwBufferSize = SYM_MAX_PATH;

	TCHAR           szLogMessage[SYM_MAX_PATH];
	TCHAR                   szStatusMsg[SYM_MAX_PATH];
    LPTSTR  lpszReturnedFile = NULL;
	LPSTR			lpszExt;	
    LPMT_FUNC_PARAMS lpParams = (LPMT_FUNC_PARAMS)lpFuncParams;

    //wait for all threads to be created before starting processing
    WaitForSingleObject(hMTSignal, 10000);

	lpszExt = (LPTSTR) malloc(SYM_MAX_PATH);
	lpszExt[0] = NULL;

    //The thread will continue to grab files until the end of the list is reached
    //deletes file, logs results, then cleans up
    while (NULL != (lpszReturnedFile = lpParams->lpFileServer->MTRequestFile()))
	{
		if (GetExtension(lpszExt, lpszReturnedFile) != 0)
			lpszExt[0] = NULL;

        dwReturn  = lpParams->lpNavapiFuncs->lpfnDeleteFile(lpParams->hEngine, (LPVOID) lpszReturnedFile,
                                                            lpszExt);
        NavstatusToString(dwReturn, szStatusMsg);
        sprintf(szLogMessage, _T("DeleteFile %s returned %s."), lpszReturnedFile, szStatusMsg);
        OutputMessage(szLogMessage, lpParams->pLog, TRUE);
        delete [] lpszReturnedFile;
    }

	free(lpszExt);	
}


BOOL GetNextDriveLetter(LPTSTR lpDrive)
{
   // threadsafe function for retrieving drive letters from the list
    cCriticalSectionLock csl(&CritSectionDLetter);
    if(gwDriveLetterCounter > gwMaxDriveLetters)
	return(FALSE);
    lpDrive[0] = gDriveLetters[gwDriveLetterCounter++];
    return(TRUE);
}


UINT GetNextDriveNumber(UINT * lpDriveNum)
{
    // threadsafe function for retrieving drive numbers from the list
    cCriticalSectionLock csl(&CritSectionDNumber);
    if(gwDriveLetterCounter > gwMaxDriveNumbers)
	return(FALSE);
    *lpDriveNum= gDriveNumbers[gwDriveNumberCounter++];
    return(TRUE);
}


void WINAPI MTScanBR(LPVOID lpFuncParams)
{
	NAVSTATUS       dwReturn;
    DWORD           dwBufferSize = SYM_MAX_PATH;
    HNAVVIRUS       hVirus = NULL;
	TCHAR           szLogMessage[SYM_MAX_PATH];
    TCHAR           szVirusName[SYM_MAX_PATH];
    TCHAR           szStatusMsg[SYM_MAX_PATH];
    TCHAR           cDriveLetter;

    LPMT_FUNC_PARAMS lpParams = (LPMT_FUNC_PARAMS)lpFuncParams;

    //get the letter of the drive boot record to be scanned
    if(GetNextDriveLetter(&cDriveLetter))
    {
        //wait for all threads to be created before starting processing
        WaitForSingleObject(hMTSignal, 10000);

        //scan the boot record, output the results, then clean up
        dwReturn = lpParams->lpNavapiFuncs->lpfnScanBoot(lpParams->hEngine, cDriveLetter, &hVirus);
        NavstatusToString(dwReturn, szStatusMsg);
        dwReturn = lpParams->lpNavapiFuncs->lpfnGetVirusInfo(hVirus, NAV_VI_VIRUS_NAME, szVirusName, &dwBufferSize);
        if (dwReturn == NAV_OK)
            sprintf(szLogMessage, _T("Boot scan for drive %c returned %s. Virus Name -  %s"), cDriveLetter,
                                      szStatusMsg, szVirusName);
        else
            sprintf(szLogMessage, _T("Boot scan %c returned %s. No virus detected"), cDriveLetter, szStatusMsg);
        OutputMessage(szLogMessage, lpParams->pLog, FALSE);
        lpParams->lpNavapiFuncs->lpfnFreeVirusHandle(hVirus);
    }
}



void WINAPI MTRepairBR(LPVOID lpFuncParams)
{
	NAVSTATUS       dwReturn;
    TCHAR           cDriveLetter;
	TCHAR           szLogMessage[SYM_MAX_PATH];
    TCHAR           szStatusMsg[SYM_MAX_PATH];

    LPMT_FUNC_PARAMS lpParams = (LPMT_FUNC_PARAMS)lpFuncParams;

    //get the letter of a drive to be repaired
    if(GetNextDriveLetter(&cDriveLetter))
    {
        //wait for all threads to be created before starting processing
        WaitForSingleObject(hMTSignal, 10000);

        //call repair then output the results
        dwReturn = lpParams->lpNavapiFuncs->lpfnRepairBoot(lpParams->hEngine, cDriveLetter);
        NavstatusToString(dwReturn, szStatusMsg);
        sprintf(szLogMessage, _T("Boot repair for drive %c scan returned %s."), cDriveLetter, szStatusMsg);
        OutputMessage(szLogMessage, lpParams->pLog, FALSE);
    }
}

void WINAPI MTScanMBR(LPVOID lpFuncParams)
{
	NAVSTATUS       dwReturn;
    DWORD           dwBufferSize = SYM_MAX_PATH;
    HNAVVIRUS       hVirus = NULL;
	TCHAR           szLogMessage[SYM_MAX_PATH];
    TCHAR           szVirusName[SYM_MAX_PATH];
    TCHAR           szStatusMsg[SYM_MAX_PATH];
    UINT            nDriveNum;

    LPMT_FUNC_PARAMS lpParams = (LPMT_FUNC_PARAMS)lpFuncParams;

    //get the number of a drive to be scanned
    if(GetNextDriveNumber(&nDriveNum))
    {
        //wait for all threads to be created before starting processing
        WaitForSingleObject(hMTSignal, 10000);

        //scan the MBR, output the results, then clean up
        dwReturn = lpParams->lpNavapiFuncs->lpfnScanMasterBoot(lpParams->hEngine, nDriveNum, &hVirus);
        NavstatusToString(dwReturn, szStatusMsg);
        dwReturn = lpParams->lpNavapiFuncs->lpfnGetVirusInfo(hVirus, NAV_VI_VIRUS_NAME, szVirusName, &dwBufferSize);
        if (dwReturn == NAV_OK)
            sprintf(szLogMessage, _T("MBR scan for drive %ld returned %s. Virus Name -  %s"), nDriveNum, szStatusMsg,
            szVirusName);
        else
            sprintf(szLogMessage, _T("MBR scan %ld returned %s. No virus detected"), nDriveNum, szStatusMsg);
        OutputMessage(szLogMessage, lpParams->pLog, FALSE);
        lpParams->lpNavapiFuncs->lpfnFreeVirusHandle(hVirus);
    }
}



void WINAPI MTRepairMBR(LPVOID lpFuncParams)
{
	NAVSTATUS       dwReturn;
    UINT            nDriveNum;
	TCHAR           szLogMessage[SYM_MAX_PATH];
    TCHAR           szStatusMsg[SYM_MAX_PATH];
    LPMT_FUNC_PARAMS lpParams = (LPMT_FUNC_PARAMS)lpFuncParams;

    //get the number of a drive to be repaired
    if(GetNextDriveNumber(&nDriveNum))
    {
        //wait for all threads to be created before starting processing
        WaitForSingleObject(hMTSignal, 10000);

        // attempt to repair MBR then output results
        dwReturn = lpParams->lpNavapiFuncs->lpfnRepairMasterBoot(lpParams->hEngine, nDriveNum);
        NavstatusToString(dwReturn, szStatusMsg);
        sprintf(szLogMessage, _T("MBR repair for drive %ld scan returned %s."), nDriveNum, szStatusMsg);
        OutputMessage(szLogMessage, lpParams->pLog, FALSE);
    }
}


void WINAPI MTScanMemory(LPVOID lpFuncParams)
{
	NAVSTATUS       dwReturn;
    DWORD           dwBufferSize = SYM_MAX_PATH;
    HNAVVIRUS       hVirus = NULL;
	TCHAR           szLogMessage[SYM_MAX_PATH];
    TCHAR           szVirusName[SYM_MAX_PATH];
    TCHAR           szStatusMsg[SYM_MAX_PATH];

    LPMT_FUNC_PARAMS lpParams = (LPMT_FUNC_PARAMS)lpFuncParams;

    //wait for all threads to be created before starting processing
    WaitForSingleObject(hMTSignal, 10000);

    //scan memory, output results, then clean up
    dwReturn = lpParams->lpNavapiFuncs->lpfnScanMemory(lpParams->hEngine, &hVirus);
    NavstatusToString(dwReturn, szStatusMsg);
    dwReturn = lpParams->lpNavapiFuncs->lpfnGetVirusInfo(hVirus, NAV_VI_VIRUS_NAME, szVirusName, &dwBufferSize);
    if (dwReturn == NAV_OK)
        sprintf(szLogMessage, _T("Memory scan returned %s. Virus Name -  %s"), szStatusMsg, szVirusName);
    else
        sprintf(szLogMessage, _T("Memory scan returned %s. No virus detected"),  szStatusMsg);
    OutputMessage(szLogMessage, lpParams->pLog, FALSE);
    lpParams->lpNavapiFuncs->lpfnFreeVirusHandle(hVirus);

}


void GetMTFileParams(HNAVENGINE hEngine, LPTSTR szOutputFile, LPNAVAPI_FUNC lpNavapiFuncs)
{
      //get the input for multi-threaded file scans, then call function to launch threads

      WORD wFuncOpt = 0, wInOpt = 0;
      TCHAR   szRootDir[SYM_MAX_PATH];
      DWORD dwThreads = 1;

      printf("Root directory : ");
      scanf("%s", szRootDir);
      printf("Number of threads (1 to 128): ");
      scanf("%ld", &dwThreads);
      printf("Scan (=0), Repair (=1), Delete (=2) files: ");
      scanf("%d", &wInOpt);
      switch(wInOpt)
      {
        case  0:    wFuncOpt = SCAN_FILES;
                    break;
        case  1:    wFuncOpt = REPAIR_FILES;
                    break;
        case  2:    wFuncOpt = DELETE_FILES;
                    break;
        default:    printf("Invalid file option.\n");
                    return;
      }
      MTReadyParams(hEngine, szRootDir, szOutputFile, lpNavapiFuncs, dwThreads, wFuncOpt);
}



void GetMTBRParams(HNAVENGINE hEngine, LPTSTR szOutputFile, LPNAVAPI_FUNC lpNavapiFuncs)
{
      //get the input for multi-threaded boot record scans, then call function to launch threads
      WORD wFuncOpt = 0, wInOpt = 0;

      CritSectionDLetter.Enter();
	    gwMaxDriveLetters = 0;
        while(gwMaxDriveLetters < MAX_DRIVES)
	    {
            _flushall();
            printf("Drive Letter (0 to end input) : ");
            scanf("%c", &gDriveLetters[gwMaxDriveLetters]);
            if(gDriveLetters[gwMaxDriveLetters] == '0')
                break;
            ++gwMaxDriveLetters;
	    }
      CritSectionDLetter.Leave();

      printf("Scan (=0), Repair (=1) boot records: ");
      scanf("%d", &wInOpt);
      switch(wInOpt)
      {
        case  0:    wFuncOpt = SCAN_BR;
                    break;
        case  1:    wFuncOpt = REPAIR_BR;
                    break;
        default:    printf("Invalid boot record option.\n");
                    return;
      }
      MTReadyParams(hEngine, NULL, szOutputFile, lpNavapiFuncs, gwMaxDriveLetters, wFuncOpt);

}

void GetMTMBRParams(HNAVENGINE hEngine, LPTSTR szOutputFile, LPNAVAPI_FUNC lpNavapiFuncs)
{
      //get the input for multi-threaded MBR scans, then call function to launch threads

      WORD wFuncOpt = 0, wInOpt = 0;

      CritSectionDNumber.Enter();
        gwMaxDriveNumbers = 0;

        while(gwMaxDriveNumbers < MAX_DRIVES)
	    {
            _flushall();
            printf("Physical Drive Number (128 for C:, 0 to end input) : ");
            scanf("%ld", &gDriveNumbers[gwMaxDriveNumbers]);
            if(gDriveNumbers[gwMaxDriveNumbers] == 0)
                break;
            ++gwMaxDriveNumbers;
	    }
      CritSectionDNumber.Leave();

      printf("Scan (=0), Repair (=1) MBRs: ");
      scanf("%d", &wInOpt);

      switch(wInOpt)
      {
        case  0:    wFuncOpt = SCAN_MBR;
                    break;
        case  1:    wFuncOpt = REPAIR_MBR;
                    break;
        default:    printf("Invalid MBR option.\n");
                    return;
      }
      MTReadyParams(hEngine, NULL, szOutputFile, lpNavapiFuncs, gwMaxDriveNumbers, wFuncOpt);
}



void GetMTMemoryParams(HNAVENGINE hEngine, LPTSTR szOutputFile, LPNAVAPI_FUNC lpNavapiFuncs)
{
      //get the input for multi-threaded memory scans, then call function to launch threads

      DWORD dwThreads = 1;

      printf("Number of threads (1 to 128): ");
      scanf("%ld", &dwThreads);
      MTReadyParams(hEngine, NULL, szOutputFile, lpNavapiFuncs, dwThreads, SCAN_MEMORY);
}

// end of multi-threaded functions

#endif  //SYM_WIN32



// Functions for Scan and Repair of a directory

#define NO_INFECTION    0
#define REPAIRED                1
#define CANT_REPAIR             2
#define DELETED                 3
#define CANT_DELETE             4

#ifndef SYM_NLM


/////////////////////////////////////////////////////////////////////
// void PathAppendName(
//
// Purpose:
//    This function will take a path and a filename and append the
//    filename to the path adding a backslash if neccessary.
//
// Parameters:
//    LPTSTR lpszFullPath - [in/out] - Starting path and also full
//                                     filename upon exit
//    LPTSTR lpszFileName - [in]     - Filename to append.
//
// Returns:
//    none.
//
// Notes:
//    It is ASSUMED that there is enough space in lpszFullPath
//    for the full path and any filename appended to it.
//
/////////////////////////////////////////////////////////////////////
// 01-05-98 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
// 10-14-99 - THWANG - copied from fileserv.cpp and renamed with "_"
/////////////////////////////////////////////////////////////////////
void _PathAppendName(
    LPTSTR lpszFullPath,
    LPTSTR lpszFileName)
{
    LPTSTR  lpszPtr;
    BOOL    bBackSlash = FALSE;

    lpszPtr = lpszFullPath;

    while (*lpszPtr)
	{
        bBackSlash = (_T('\\') == *lpszPtr);
        lpszPtr = AnsiNext(lpszPtr); 
	}

    if (!bBackSlash)
        STRCAT(lpszFullPath, _T("\\"));

    STRCAT(lpszFullPath, lpszFileName);
}


#if defined(SYM_WIN)
    int    ScanAndRepairFile(LPTSTR lpszFileName, HNAVENGINE hEngine, CLog * pLog, BOOL bDeleteUnrepairable, LPNAVAPI_FUNC lpNavapiFuncs)
#else
    int    ScanAndRepairFile(LPTSTR lpszFileName, HNAVENGINE hEngine, CLog * pLog, BOOL bDeleteUnrepairable)
#endif
{
	NAVSTATUS       dwReturn;
	DWORD                   dwBufferSize = SYM_MAX_PATH;
	BOOL            bVirus = FALSE;
	TCHAR           szVirusName[SYM_MAX_PATH];
	TCHAR           szLogMessage[SYM_MAX_PATH];
	TCHAR                   szStatusMsg[SYM_MAX_PATH];
	HNAVVIRUS               hVirus = NULL;
	int                             nReturn = -1;
	LPTSTR			lpszExt;

	lpszExt = (LPTSTR) malloc(SYM_MAX_PATH);
	lpszExt[0] = NULL;

	if (GetExtension(lpszExt, lpszFileName) != 0)
		lpszExt[0] = NULL;

#if defined(SYM_WIN)
    dwReturn        =       lpNavapiFuncs->lpfnScanFile(hEngine, (LPVOID) lpszFileName, lpszExt, TRUE, &hVirus);
#else
	dwReturn        =       NAVScanFile(hEngine, (LPVOID) lpszFileName, lpszExt, TRUE, &hVirus);
#endif

	NavstatusToString(dwReturn, szStatusMsg);
	sprintf(szLogMessage, _T("ScanFile %s returned %s"), lpszFileName, szStatusMsg);
	OutputMessage(szLogMessage, pLog);

#if defined(SYM_WIN)
    dwReturn        =       lpNavapiFuncs->lpfnGetVirusInfo(hVirus, NAV_VI_VIRUS_NAME, szVirusName, &dwBufferSize);
#else
    dwReturn        =       NAVGetVirusInfo(hVirus, NAV_VI_VIRUS_NAME, szVirusName, &dwBufferSize);
#endif

	if (dwReturn == 0)
	{
		sprintf(szLogMessage, _T("Virus Name -  %s"), szVirusName);
		bVirus = TRUE;
	}
	else
	{
		sprintf(szLogMessage,_T("No virus detected"));
		nReturn = NO_INFECTION;
	}
	OutputMessage(szLogMessage, pLog);

	if (bVirus)
	{

#if defined(SYM_WIN)
		lpNavapiFuncs->lpfnFreeVirusHandle(hVirus);
		dwReturn        =       lpNavapiFuncs->lpfnRepairFile(hEngine, (LPVOID) lpszFileName, lpszExt);
#else
		NAVFreeVirusHandle(hVirus);
		dwReturn        =       NAVRepairFile(hEngine, (LPVOID) lpszFileName, lpszExt);
#endif

		NavstatusToString(dwReturn, szStatusMsg);
		sprintf(szLogMessage, _T("RepairFile %s returned %s"), lpszFileName, szStatusMsg);
		OutputMessage(szLogMessage, pLog);

		if ( ( NAV_CANT_REPAIR == (NAVSTATUS) dwReturn ) && bDeleteUnrepairable)
		{

#if defined(SYM_WIN)
			dwReturn        =       lpNavapiFuncs->lpfnDeleteFile(hEngine, (LPVOID) lpszFileName, lpszExt);
#else
			dwReturn        =       NAVDeleteFile(hEngine, (LPVOID) lpszFileName, lpszExt);
#endif



			NavstatusToString(dwReturn, szStatusMsg);
			sprintf(szLogMessage, _T("Unrepairable virus.  DeleteFile %s returned %s"), lpszFileName, szStatusMsg);
			OutputMessage(szLogMessage, pLog);
			if ( NAV_CANT_DELETE == (NAVSTATUS) dwReturn)
				nReturn = CANT_DELETE;
			else
				nReturn = DELETED;
		}
		else
		{
			if  ( NAV_OK == (NAVSTATUS) dwReturn )
				nReturn = REPAIRED;             //we repaired it
			else
				nReturn = CANT_REPAIR;

#if defined(SYM_WIN)
			dwReturn        =       lpNavapiFuncs->lpfnScanFile(hEngine, (LPVOID) lpszFileName, lpszExt, TRUE, &hVirus);
//			dwReturn        =       lpNavapiFuncs->lpfnScanFile(hEngine, (LPVOID) lpszFileName, IsExecutable(lpszFileName), TRUE, &hVirus);
#else
			dwReturn        =       NAVScanFile(hEngine, (LPVOID) lpszFileName, lpszExt, TRUE, &hVirus);
//			dwReturn        =       NAVScanFile(hEngine, (LPVOID) lpszFileName, IsExecutable(lpszFileName), TRUE, &hVirus);
#endif



			NavstatusToString(dwReturn, szStatusMsg);
			sprintf(szLogMessage, _T("ScanFile %s returned %s"), lpszFileName, szStatusMsg);
			OutputMessage(szLogMessage, pLog);

			dwBufferSize    = SYM_MAX_PATH;

#if defined(SYM_WIN)
		    dwReturn        =       lpNavapiFuncs->lpfnGetVirusInfo(hVirus, NAV_VI_VIRUS_NAME, szVirusName, &dwBufferSize);
#else
			dwReturn        =       NAVGetVirusInfo(hVirus, NAV_VI_VIRUS_NAME, szVirusName, &dwBufferSize);
#endif




			if (dwReturn == 0)
				sprintf(szLogMessage, _T("Virus Name -  %s"), szVirusName);
			else
				sprintf(szLogMessage, _T("No virus detected"));
			OutputMessage(szLogMessage, pLog);

#if defined(SYM_WIN)
			lpNavapiFuncs->lpfnFreeVirusHandle(hVirus);
#else
			NAVFreeVirusHandle(hVirus);
#endif

		}

	}

	free(lpszExt);	
	return nReturn;
}


// Recursive function used to scan and repair a directory.
#if defined(SYM_WIN)
    void    ScanAndRepairDirectoryRec(HNAVENGINE hEngine, LPTSTR szRootDir, BOOL bDeleteUnrepairable, LPTSTR szOutputFile, 
		LPNAVAPI_FUNC lpNavapiFuncs, long *nScanned, long *nRepaired, long *nInfected, long *nDeleted)
#else
    void    ScanAndRepairDirectoryRec(HNAVENGINE hEngine, LPTSTR szRootDir, BOOL bDeleteUnrepairable, LPTSTR szOutputFile,
		long *nScanned, long *nRepaired, long *nInfected, long *nDeleted)
#endif
{
	CLog * pLog;
    FINDDATA        rFindData;
    HFIND           hFindHandle;
	BOOL            bContinue = TRUE;

	int nAction;
	TCHAR szLogMessage[SYM_MAX_PATH];
	TCHAR szFileName[SYM_MAX_PATH];

	if (szOutputFile)
		pLog  =       new CLog(szOutputFile);		// CLog is threadsafe so we can 
													// safely open a second session to it
	else
		pLog  =       new CLog(STSCAN_LOG_FILENAME);

    // Take our local copy first.
    STRCPY(szFileName, szRootDir);

	// Add the wildcard character to the filename
    _PathAppendName(szFileName, _T("*.*"));

    hFindHandle = FileFindFirst(szFileName, &rFindData);
	
    if ( ((HFIND)INVALID_HANDLE_VALUE) == hFindHandle)
	{
		// Close the handle.
		if (INVALID_HANDLE_VALUE != hFindHandle)
		{
			FileFindClose(hFindHandle);
			hFindHandle = (HFIND)INVALID_HANDLE_VALUE;
		}
	}
                     
	// Make sure that we are to continue, and that we are still successful.
    while (bContinue)
    {
		// Construct the file to delete
        STRCPY (szFileName, szRootDir);

        _PathAppendName(szFileName, rFindData.szFileName);

		// Check to see if it is a directory, and if it is, recurse.
        if ((FA_DIR & rFindData.dwFileAttributes) == FA_DIR)
		{
			// Don't do anything with the "DOTS"
            if (STRCMP(rFindData.szFileName, _T(".")) && STRCMP(rFindData.szFileName, _T("..")))
#if defined(SYM_WIN)
			    ScanAndRepairDirectoryRec(hEngine, szFileName, bDeleteUnrepairable,
					szOutputFile, lpNavapiFuncs, nScanned, nRepaired, nInfected, nDeleted);
#else
				ScanAndRepairDirectoryRec(hEngine, szFileName, bDeleteUnrepairable,
					szOutputFile, nScanned, nRepaired, nInfected, nDeleted);
#endif
		}
        else
        {
#if defined(SYM_WIN)
			nAction = ScanAndRepairFile(szFileName, hEngine, pLog, bDeleteUnrepairable, lpNavapiFuncs);
#else
			nAction = ScanAndRepairFile(szFileName, hEngine, pLog, bDeleteUnrepairable);
#endif
			(*nScanned)++;

			if (REPAIRED == nAction)
			{
				(*nRepaired)++;
				(*nInfected)++;
			}
			else if (CANT_REPAIR == nAction)
				(*nInfected)++;
			else if (CANT_DELETE == nAction)
				(*nInfected)++;
			else if (DELETED == nAction)
			{
				(*nInfected)++;
				(*nDeleted)++;
			}
			sprintf(szLogMessage, "%ld files scanned, %ld files infected, %ld files repaired, %ld files deleted.", *nScanned, *nInfected, *nRepaired, *nDeleted);
			OutputMessage(szLogMessage, pLog);
        }

        bContinue = FileFindNext(hFindHandle, &rFindData);
	}

	// Close the handle.
    if (INVALID_HANDLE_VALUE != hFindHandle)
    {
        FileFindClose(hFindHandle);
        hFindHandle = (HFIND)INVALID_HANDLE_VALUE;
    }
}


#if defined(SYM_WIN)
	void    ScanAndRepairDirectory(HNAVENGINE hEngine, LPTSTR szRootDir, BOOL bDeleteUnrepairable, LPTSTR szOutputFile, 
		LPNAVAPI_FUNC lpNavapiFuncs)
#else
    void    ScanAndRepairDirectory(HNAVENGINE hEngine, LPTSTR szRootDir, BOOL bDeleteUnrepairable, LPTSTR szOutputFile)
#endif
{
	long nScanned = 0, nRepaired = 0, nInfected = 0, nDeleted = 0;

	printf("Commencing scan and repair\n");

#if defined(SYM_WIN)
    ScanAndRepairDirectoryRec(hEngine, szRootDir, bDeleteUnrepairable, szOutputFile, 
		lpNavapiFuncs, &nScanned, &nRepaired, &nInfected, &nDeleted);
#else
    ScanAndRepairDirectoryRec(hEngine, szRootDir, bDeleteUnrepairable, szOutputFile,
		&nScanned, &nRepaired, &nInfected, &nDeleted);
#endif

	return;
}


// Recursive function used to delete a directory.
#if defined (SYM_WIN)
    void   DeleteDirectoryRec(HNAVENGINE hEngine, LPTSTR szRootDir, LPTSTR szOutputFile, LPNAVAPI_FUNC lpNavapiFuncs)
#else
    void   DeleteDirectoryRec(HNAVENGINE hEngine, LPTSTR szRootDir, LPTSTR szOutputFile)
#endif
{
	CLog * pLog;
    FINDDATA        rFindData;
    HFIND           hFindHandle;
	BOOL            bContinue = TRUE;
	NAVSTATUS dwReturn;

	TCHAR szLogMessage[SYM_MAX_PATH];
	TCHAR szFileName[SYM_MAX_PATH];
	TCHAR szStatusMsg[SYM_MAX_PATH];
	LPTSTR lpszExt;			

	lpszExt = (LPTSTR) malloc(SYM_MAX_PATH);
	lpszExt[0] = NULL;

	if (szOutputFile)
		pLog  =       new CLog(szOutputFile);		// CLog is threadsafe so we can 
													// safely open a second session to it
	else
		pLog  =       new CLog(STSCAN_LOG_FILENAME);

    // Take our local copy first.
    STRCPY(szFileName, szRootDir);

	// Add the wildcard character to the filename
    _PathAppendName(szFileName, _T("*.*"));

    hFindHandle = FileFindFirst(szFileName, &rFindData);
	
    if ( ((HFIND)INVALID_HANDLE_VALUE) == hFindHandle)
	{
		// Close the handle.
		if (INVALID_HANDLE_VALUE != hFindHandle)
		{
			FileFindClose(hFindHandle);
			hFindHandle = (HFIND)INVALID_HANDLE_VALUE;
		}
	}
                     
	// Make sure that we are to continue, and that we are still successful.
    while (bContinue)
    {
		// Construct the file to delete
        STRCPY (szFileName, szRootDir);

        _PathAppendName(szFileName, rFindData.szFileName);

		// Check to see if it is a directory, and if it is, recurse.
        if ((FA_DIR & rFindData.dwFileAttributes) == FA_DIR)
		{
			// Don't do anything with the "DOTS"
			if (STRCMP(rFindData.szFileName, _T(".")) && STRCMP(rFindData.szFileName, _T("..")))
#if defined (SYM_WIN)
				DeleteDirectoryRec(hEngine, szFileName, szOutputFile, lpNavapiFuncs);
#else
				DeleteDirectoryRec(hEngine, szFileName, szOutputFile);
#endif
		}
        else
        {
			if (GetExtension(lpszExt, szFileName) != 0)
				lpszExt[0] = NULL;

#if defined(SYM_WIN)
			dwReturn = lpNavapiFuncs->lpfnDeleteFile(hEngine, (LPVOID) szFileName, lpszExt);
#else
			dwReturn = NAVDeleteFile(hEngine, (LPVOID) szFileName, lpszExt);
#endif
			NavstatusToString(dwReturn, szStatusMsg);
			sprintf(szLogMessage, _T("DeleteFile(%s) returned %s"), szFileName, szStatusMsg);
			OutputMessage(szLogMessage, pLog);
        }

        bContinue = FileFindNext(hFindHandle, &rFindData);
	}

	// Close the handle.
    if (INVALID_HANDLE_VALUE != hFindHandle)
    {
        FileFindClose(hFindHandle);
        hFindHandle = (HFIND)INVALID_HANDLE_VALUE;
    }
}


#if defined (SYM_WIN)
    void   DeleteDirectory(HNAVENGINE hEngine, LPTSTR szRootDir, LPTSTR szOutputFile, LPNAVAPI_FUNC lpNavapiFuncs)
#else
    void   DeleteDirectory(HNAVENGINE hEngine, LPTSTR szRootDir, LPTSTR szOutputFile)
#endif
{
	printf("Commencing directory delete...\n");

#if defined(SYM_WIN)
    DeleteDirectoryRec(hEngine, szRootDir, szOutputFile, lpNavapiFuncs);
#else
    DeleteDirectoryRec(hEngine, szRootDir, szOutputFile);
#endif

	return;
}


#endif          // #ifndef SYM_NLM


// Main Menu - pops up the options, returns the user's choice as a TCHAR

TCHAR MainMenu(void)
{
	TCHAR szIn[SYM_MAX_PATH];

	printf("A. NAVEngineInit            ");
	printf("B. NAVEngineClose\n");
#ifdef SYM_WIN32
	printf("C. NAVIsInfectableFile      ");
#endif
	printf("D. NAVScanFile\n");
#ifndef SYM_NLM
	printf("E. NAVScanBoot              ");
	printf("F. NAVScanMasterBoot\n");
#endif
	printf("G. NAVScanMemory            ");
	printf("H. NAVRepairFile\n");
#ifndef SYM_NLM
	printf("I. NAVRepairBoot            ");
	printf("J. NAVRepairMasterBoot\n");
#endif
	printf("K. NAVDeleteFile            ");
	printf("L. NAVFreeVirusHandle\n");
	printf("M. NAVGetVirusInfo          ");
	printf("N. NAVGetVirusDefCount\n");
	printf("O. NAVGetVirusDefMaxCount   ");
	printf("P. NAVGetVirusDefDetectCount\n");
	printf("Q. NAVLoadVirusDef          ");
	printf("R. NAVReleaseVirusDef\n");
	printf("S. NAVGetVirusDefInfo       ");
	printf("T. NAVLoadVirusDefTable\n");
	printf("U. NAVReleaseVirusDefTable  ");
	printf("V. NAVGetVirusDefTableCount\n");
	printf("W. NAVGetVirusDefTableInfo  ");
	printf("X. NAVGetNavapiVersionString\n");
	printf("Y. NAVGetVirusDBInfo        ");
	printf("Z. NAVGetVirusDBInfoEx\n");
	printf("0. DefTableInfo (Entire table test)\n");
#ifndef SYM_NLM
	printf("1. Scan and Repair directory\n");
	printf("2. Scan and Repair directory (delete unrepairable files)\n");
    printf("3. Multithreading\n");
#endif
#ifndef SYM_NLM
	printf("4. Delete all files in a directory\n");
#endif
	printf("5. Quit\n");
	printf("Your command:");

	scanf("%s", szIn);
	STRUPR(szIn);
	return szIn[0];
}

#ifdef SYM_WIN

BOOL  GetNavapiAddresses(HINSTANCE hDLL, LPNAVAPI_FUNC lpFunctions)
{
    lpFunctions->lpfnEngineInit = (FUNC_ENGINE_INIT*)GetProcAddress(hDLL, PROC_ADDR_ENG_INIT);
    if(!lpFunctions->lpfnEngineInit)
        return(FALSE);

    lpFunctions->lpfnEngineClose = (FUNC_ENGINE_CLOSE*)GetProcAddress(hDLL, PROC_ADDR_ENG_CLOSE);
    if(!lpFunctions->lpfnEngineClose)
        return(FALSE);

#ifdef SYM_WIN32
    lpFunctions->lpfnIsInfectableFile = (FUNC_IS_INFECTABLE_FILE*)GetProcAddress(hDLL, PROC_ADDR_IS_INFECTABLE_FILE);
    if(!lpFunctions->lpfnIsInfectableFile)
        return(FALSE);
#endif

    lpFunctions->lpfnScanFile = (FUNC_SCAN_FILE*)GetProcAddress(hDLL, PROC_ADDR_SCAN_FILE);
    if(!lpFunctions->lpfnScanFile)
        return(FALSE);

    lpFunctions->lpfnScanBoot = (FUNC_SCAN_BOOT*)GetProcAddress(hDLL, PROC_ADDR_SCAN_BOOT);
    if(!lpFunctions->lpfnScanBoot)
        return(FALSE);

    lpFunctions->lpfnScanMasterBoot = (FUNC_SCAN_MASTER_BOOT*)GetProcAddress(hDLL, PROC_ADDR_SCAN_MAST_BOOT);
    if(!lpFunctions->lpfnScanMasterBoot)
        return(FALSE);

    lpFunctions->lpfnScanMemory = (FUNC_SCAN_MEMORY*)GetProcAddress(hDLL, PROC_ADDR_SCAN_MEM);
    if(!lpFunctions->lpfnScanMemory)
        return(FALSE);

    lpFunctions->lpfnRepairFile = (FUNC_REPAIR_FILE*)GetProcAddress(hDLL, PROC_ADDR_REP_FILE);
    if(!lpFunctions->lpfnRepairFile)
        return(FALSE);

    lpFunctions->lpfnRepairBoot = (FUNC_REPAIR_BOOT*)GetProcAddress(hDLL, PROC_ADDR_REP_BOOT);
    if(!lpFunctions->lpfnRepairBoot)
        return(FALSE);

    lpFunctions->lpfnRepairMasterBoot = (FUNC_REPAIR_MASTER_BOOT*)GetProcAddress(hDLL, PROC_ADDR_REP_MAST_BOOT);
    if(!lpFunctions->lpfnRepairMasterBoot)
        return(FALSE);

    lpFunctions->lpfnDeleteFile = (FUNC_DELETE_FILE*)GetProcAddress(hDLL, PROC_ADDR_DEL_FILE);
    if(!lpFunctions->lpfnDeleteFile)
        return(FALSE);

    lpFunctions->lpfnFreeVirusHandle = (FUNC_FREE_VIRUS_HANDLE*)GetProcAddress(hDLL, PROC_ADDR_FREE_VIR_HANDLE);
    if(!lpFunctions->lpfnFreeVirusHandle)
        return(FALSE);

    lpFunctions->lpfnGetVirusInfo = (FUNC_GET_VIRUS_INFO*)GetProcAddress(hDLL, PROC_ADDR_GET_VIR_INFO);
    if(!lpFunctions->lpfnGetVirusInfo)
        return(FALSE);

    lpFunctions->lpfnGetVirusDefCount = (FUNC_GET_VIRUS_DEF_COUNT*)GetProcAddress(hDLL, PROC_ADDR_GET_VIR_CNT);
    if(!lpFunctions->lpfnGetVirusDefCount)
        return(FALSE);

    lpFunctions->lpfnGetVirusDefMaxCount = (FUNC_GET_VIRUS_DEF_MAX_COUNT*)GetProcAddress(hDLL, PROC_ADDR_GET_VIR_MAX_CNT);
    if(!lpFunctions->lpfnGetVirusDefMaxCount)
        return(FALSE);

    lpFunctions->lpfnGetVirusDefDetectCount = (FUNC_GET_VIRUS_DEF_DETECT_COUNT*)GetProcAddress(hDLL, PROC_ADDR_GET_VIR_DETECT_CNT);
    if(!lpFunctions->lpfnGetVirusDefDetectCount)
        return(FALSE);

    lpFunctions->lpfnLoadVirusDef = (FUNC_LOAD_VIRUS_DEF*)GetProcAddress(hDLL, PROC_ADDR_LOAD_VIR_DEF);
    if(!lpFunctions->lpfnLoadVirusDef)
        return(FALSE);

    lpFunctions->lpfnReleaseVirusDef = (FUNC_RELEASE_VIRUS_DEF*)GetProcAddress(hDLL, PROC_ADDR_REL_VIR_DEF);
    if(!lpFunctions->lpfnReleaseVirusDef)
        return(FALSE);

    lpFunctions->lpfnGetVirusDefInfo = (FUNC_GET_VIRUS_DEF_INFO*)GetProcAddress(hDLL, PROC_ADDR_GET_VIR_DEF_INFO);
    if(!lpFunctions->lpfnGetVirusDefInfo)
        return(FALSE);

    lpFunctions->lpfnLoadVirusDefTable = (FUNC_LOAD_VIRUS_DEF_TABLE*)GetProcAddress(hDLL, PROC_ADDR_LOAD_VIR_DEF_TBL);
    if(!lpFunctions->lpfnLoadVirusDefTable)
        return(FALSE);

    lpFunctions->lpfnReleaseVirusDefTable = (FUNC_RELEASE_VIRUS_DEF_TABLE*)GetProcAddress(hDLL, PROC_ADDR_REL_VIR_DEF_TBL);
    if(!lpFunctions->lpfnReleaseVirusDefTable)
        return(FALSE);

    lpFunctions->lpfnGetVirusDefTableCount = (FUNC_GET_VIRUS_DEF_TABLE_COUNT*)GetProcAddress(hDLL, PROC_ADDR_GET_VIR_DEF_TBL_CNT);
    if(!lpFunctions->lpfnGetVirusDefTableCount)
        return(FALSE);

    lpFunctions->lpfnGetVirusDefTableInfo = (FUNC_GET_VIRUS_DEF_TABLE_INFO*)GetProcAddress(hDLL, PROC_ADDR_GET_VIR_DEF_TBL_INFO);
    if(!lpFunctions->lpfnGetVirusDefTableInfo)
        return(FALSE);

    lpFunctions->lpfnGetNavapiVerString = (FUNC_GET_NAVAPI_VER_STRING*)GetProcAddress(hDLL, PROC_ADDR_GET_NAVAPI_VER_STR);
    if(!lpFunctions->lpfnGetNavapiVerString)
        return(FALSE);

    lpFunctions->lpfnGetVirDbInfo = (FUNC_GET_VIR_DB_INFO*)GetProcAddress(hDLL, PROC_ADDR_GET_VIR_DB_INFO);
    if(!lpFunctions->lpfnGetVirDbInfo)
        return(FALSE);

    lpFunctions->lpfnGetVirDbInfoEx = (FUNC_GET_VIR_DB_INFO_EX*)GetProcAddress(hDLL, PROC_ADDR_GET_VIR_DB_INFO_EX);
    if(!lpFunctions->lpfnGetVirDbInfoEx)
        return(FALSE);

    return(TRUE);
}



#endif  //SYM_WIN


void ParseCommandLine(int argc, char *argv[], LPTSTR * hScriptFile,
					  LPTSTR * hOutputFile)
{
	int nArgNo;

	for (nArgNo = 1; nArgNo < argc; nArgNo++)
	{
		// Make sure we have a valid command line switch before referencing its data
		if ( (strlen(argv[nArgNo]) > 1) && (argv[nArgNo][0] == '/') )
		{
			strupr(argv[nArgNo]);
			switch (argv[nArgNo][1])
			{
			case 'S':
				*hScriptFile = (LPTSTR) malloc(SYM_MAX_PATH);
				strcpy(*hScriptFile, argv[nArgNo] + 3);
				break;
			case 'O':
				*hOutputFile = (LPTSTR) malloc(SYM_MAX_PATH);
				strcpy(*hOutputFile, argv[nArgNo] + 3);
				break;
			default:
				break;
			}
		}
	}
}




LPTSTR  ReadLineFromFile(FILE * lpFile)
{
	TCHAR *  szStringRead = (TCHAR *) malloc(SYM_MAX_PATH);

	if (lpFile)
		return fgets(szStringRead, SYM_MAX_PATH, lpFile);
	else
	{
		free (szStringRead);
		return NULL;
	}
}


TCHAR GetMenuItem(FILE * lpFile, LPTSTR * szScriptLine)
{
	if (lpFile)
	{
		*szScriptLine = ReadLineFromFile(lpFile);
		if (*szScriptLine)
		{
			strupr(*szScriptLine);
			return (*szScriptLine[0]);
		}
		return 0;
	}
	else
#ifdef SYM_NLM
		return 'Z';
#else
		return MainMenu();
#endif
}


// This is code I hope nobody ever sees.  Particularly the huge switch statement
// which makes me somewhat queasy to think that I wrote it.

// 8/13/98 - Now that Melissa has added the DLL stuff with all its #ifdefs I don't
// feel so bad about the switch statement mess.  ;)

int  main( int argc, char *argv[] )
{
	NAVSTATUS               dwReturn;
	NAVFILEIO               stCallbacks;
	BOOL                    bDone                   = FALSE;
	TCHAR                   cMenuOpt                = 'Z';
	HNAVVIRUSDEF            hVirusEntry             = NULL;
	HNAVVIRUSDEFTABLE       hVirusDefTable          = NULL;
	LPTSTR                  lpszResultBuffer;
	LPTSTR					lpszExt;				
#ifndef SYM_NLM
	CLog *                                  pLog                                    = NULL;
#else
	FILE *                                  pLog                                    = NULL;
#endif

	TCHAR                                   szOutputBuffer[SYM_MAX_PATH];

	// Storage of optional commandline arguments
	LPTSTR szScriptFile = NULL;
	LPTSTR szOutputFile = NULL;

	LPTSTR szScriptLine = NULL;
	FILE * lpFile = NULL;

	HNAVVIRUS hVirus = NULL;
	HNAVENGINE hEngine = NULL;
	TCHAR szStatusMsg[SYM_MAX_PATH];

	lpszResultBuffer = (LPTSTR) malloc(SYM_MAX_PATH);
	lpszResultBuffer[0] = NULL;
	

	lpszExt = (LPTSTR) malloc(SYM_MAX_PATH);
	lpszExt[0] = NULL;


	ParseCommandLine(argc, argv, &szScriptFile, &szOutputFile);

	// Do we want to log?
	if (szOutputFile)
#ifndef SYM_NLM
		pLog = new CLog(szOutputFile);
#else
		pLog = fopen(szOutputFile, "a");
#endif


	// Do we want to use a script file?
	if (szScriptFile)
		lpFile = fopen(szScriptFile, "r");


// load library for navapi dll

#ifdef SYM_WIN

    HINSTANCE hDLL = NULL;
    hDLL = LoadLibrary(NAVAPI_DLL);

#ifdef SYM_WIN32
    if(hDLL == NULL)
#elif defined SYM_WIN16
    if(hDLL < HINSTANCE_ERROR)
#endif
    {
	printf("Unable to load %s\n", NAVAPI_DLL);
	delete [] lpszResultBuffer;
	return 0;
    }

    NAVAPI_FUNC NavapiFuncs;
    if(!GetNavapiAddresses(hDLL, &NavapiFuncs))
    {
	printf("Unable to get proc addresses\n");
	FreeLibrary(hDLL);
	delete [] lpszResultBuffer;
	return(0);
    }
#endif //SYM_WIN


	stCallbacks.Open            = MyOpen;
	stCallbacks.Close           = MyClose;
	stCallbacks.Seek            = MySeek;
	stCallbacks.Read            = MyRead;
	stCallbacks.Write           = MyWrite;
	stCallbacks.Delete          = MyDelete;
	stCallbacks.GetDateTime         = MyGetDateTime;
	stCallbacks.SetDateTime         = MySetDateTime;

	while (!bDone)
	{
		switch ( GetMenuItem(lpFile, &szScriptLine) )
		{
		case '#':
			{		// Echo's a line from the input script file to the output script file, useful for placing comments.
					// Usage:
					// A '#' indicates the start of a line of comment.
					// Example:
					// # This will output everything in this sentence to the output file.
					if (lpFile)
					{
						OutputMessage(szScriptLine, pLog, FALSE);
					}
					else
					{
						OutputMessage("Error, comments can only be used in scripts.\n", pLog, FALSE);
					}
			}
			break;
		case '0':                     // Special test, test loading all virus definitions.
			{
				HNAVVIRUSDEFTABLE hTable;
				DWORD             dwBufSize;
				DWORD             dwMaxName  = 0;
				DWORD             dwMaxAlias = 0;
				DWORD             dwMaxInfo  = 0;
				DWORD             dwNumDefs;
				TCHAR             szFoo[256];
				int               nCnt;
				DWORD             u;
				char              sTemp[256];

				// bBoolVals will hold the previous set of BOOL values in bBoolVals[2] and
				// the current set in bBoolVals[1].  bBoolVals[0] will each be set to [1] as
				// a particular value is found to be different at any time.  This is to
				// check to make sure a particular value is not always returning the
				// exact same value.
				BOOL              bBoolVals[3][55];
				int               nTemp1, nTemp2;
				BOOL              bFirst=TRUE;


				// Initialize the bBoolVals to all zeros.

				for (nTemp1=0; nTemp1<3; nTemp1++)
				   for (nTemp2=0; nTemp2<55; nTemp2++)
				      bBoolVals[nTemp1][nTemp2] = 0;


#if defined(SYM_WIN)
				hTable = NavapiFuncs.lpfnLoadVirusDefTable( hEngine );
#else
				hTable = NAVLoadVirusDefTable ( hEngine );
#endif

				if ( !hTable )
				{
					printf ( "NAVLoadVirusDefTable() failed!\n" ); break;
				}

#if defined(SYM_WIN)
				if ( NAV_OK != NavapiFuncs.lpfnGetVirusDefTableCount ( hTable, &dwNumDefs ) )
#else
				if ( NAV_OK != NAVGetVirusDefTableCount ( hTable, &dwNumDefs ) )
#endif
				{
					printf ( "NAVGetVirusDefTableCount() failed!\n" );
#if defined(SYM_WIN)
					NavapiFuncs.lpfnReleaseVirusDefTable ( hTable );
#else
					NAVReleaseVirusDefTable ( hTable );
#endif
					break;
				}

				for ( u = 0, nCnt = 0; u < dwNumDefs; u++ )
				{
					if (nCnt == 400)
					{
						printf(".");
						nCnt = 0;
					}
					else
					{
						nCnt++;
					}

					dwBufSize = 256;
#if defined(SYM_WIN)
					NavapiFuncs.lpfnGetVirusDefTableInfo ( hTable, u, NAV_VI_VIRUS_NAME, sTemp, &dwBufSize );
					NavapiFuncs.lpfnGetVirusDefTableInfo ( hTable, u, NAV_VI_VIRUS_ALIAS, sTemp, &dwBufSize );
					NavapiFuncs.lpfnGetVirusDefTableInfo ( hTable, u, NAV_VI_VIRUS_INFO, sTemp, &dwBufSize );
					NavapiFuncs.lpfnGetVirusDefTableInfo ( hTable, u, NAV_VI_VIRUS_SIZE, sTemp, &dwBufSize );

					// Note that the end of this for-loop is based on the enumeration in Navapi.h.
					// NAV_VI_BOOL_REPAIRED comes immediately after the last general virus info item.
					// If any changes occur in the enumeration, this may need to be changed.
					// This loop is used five times in this function.
					for (nTemp1 = (int) NAV_VI_BOOL_FIRST+1; nTemp1 < (int) NAV_VI_BOOL_REPAIRED; nTemp1++)
						bBoolVals[1][nTemp1] = 
						   NavapiFuncs.lpfnGetVirusDefTableInfo (hTable, u, (NAVVIRINFO) nTemp1, szFoo, &dwBufSize );
					
#else
					NAVGetVirusDefTableInfo ( hTable, u, NAV_VI_VIRUS_NAME, sTemp, &dwBufSize );
					NAVGetVirusDefTableInfo ( hTable, u, NAV_VI_VIRUS_ALIAS, sTemp, &dwBufSize );
					NAVGetVirusDefTableInfo ( hTable, u, NAV_VI_VIRUS_INFO, sTemp, &dwBufSize );
					NAVGetVirusDefTableInfo ( hTable, u, NAV_VI_VIRUS_SIZE, sTemp, &dwBufSize );

					// Note that the end of this for-loop is based on the enumeration in Navapi.h.
					// NAV_VI_BOOL_REPAIRED comes immediately after the last general virus info item.
					// If any changes occur in the enumeration, this may need to be changed.
					// This loop is used five times in this function.
					for (nTemp1 = (int) NAV_VI_BOOL_FIRST+1; nTemp1 < (int) NAV_VI_BOOL_REPAIRED; nTemp1++)
						bBoolVals[1][nTemp1] = 
						   NAVGetVirusDefTableInfo (hTable, u, (NAVVIRINFO) nTemp1, szFoo, &dwBufSize );


#endif  // #if defined(SYM_WIN)
					// Now we must note any of the booleans that have changed since the last virus.
					// These changes will be noted in bBoolVals[0].  A '1' will indicate that this item has
					// changed at one point (once it changes to '1', it stays set that way through the
					// entire check).  Only if a value is the same throughout the whole table will a
					// position remain '0' at the end of this for-loop.

					// Note that the end of this for-loop is based on the enumeration in Navapi.h.
					// NAV_VI_BOOL_REPAIRED comes immediately after the last general virus info item.
					// If any changes occur in the enumeration, this may need to be changed.
					// This loop is used five times in this function.

					if (TRUE == bFirst)
					   bFirst = FALSE;
					else
					   for (nTemp1 = (int) NAV_VI_BOOL_FIRST+1; nTemp1 < (int) NAV_VI_BOOL_REPAIRED; nTemp1++)
					      if (0 == bBoolVals[0][nTemp1])
					         if (bBoolVals[1][nTemp1] != bBoolVals[2][nTemp1])
					            bBoolVals[0][nTemp1] = 1;

					// Now we need to copy all of the current values to the "previous" slot
					// in bBoolVals[2].

					// Note that the end of this for-loop is based on the enumeration in Navapi.h.
					// NAV_VI_BOOL_REPAIRED comes immediately after the last general virus info item.
					// If any changes occur in the enumeration, this may need to be changed.
					// This loop is used five times in this function.

					for (nTemp1 = (int) NAV_VI_BOOL_FIRST+1; nTemp1 < (int) NAV_VI_BOOL_REPAIRED; nTemp1++)
					   bBoolVals[2][nTemp1] = bBoolVals[1][nTemp1];

				}// end for ( u = 0, nCnt = 0; u < dwNumDefs; u++ )

#if defined(SYM_WIN)
				NavapiFuncs.lpfnReleaseVirusDefTable ( hTable );
#else
				NAVReleaseVirusDefTable ( hTable );
#endif

				for (nTemp1 = (int) NAV_VI_BOOL_FIRST+1; nTemp1 < (int) NAV_VI_BOOL_REPAIRED; nTemp1++)
				   if (0 == bBoolVals[0][nTemp1])
				      printf("GetVirusDefTableInfo item %d returned identical results on all lookups.\n",
				         nTemp1);

				printf("Special function: %ld entries processed.\n", dwNumDefs);
			}
			break;
		case 'A':                                       // InitEngine
			{
				TCHAR szDefsPath[SYM_MAX_PATH];
				TCHAR szNavexInf[SYM_MAX_PATH];
				NAVSTATUS dwStatus = 0;
				WORD wHeuristicLevel = 0;
				WORD wFlags = 0;

				HNAVENGINE      hOldEngine      =       hEngine;                // Save old engine handle

				if (lpFile)
				{
					sscanf(szScriptLine, "%c %s %s %d %d", &cMenuOpt, szDefsPath, szNavexInf, &wHeuristicLevel, &wFlags);
				}
				else
				{
					printf("Defs Path: ");
					scanf("%s", szDefsPath);
					printf("Navex.inf Path ('0' for no Navex.inf) : ");
					scanf("%s", szNavexInf);
					printf("Heuristic Level: ");
					scanf("%d", &wHeuristicLevel);
					printf("Flags: ");
					scanf("%d", &wFlags);
				}

				if ('0' == szNavexInf[0])
				{

#if defined(SYM_WIN)
		    hEngine = NavapiFuncs.lpfnEngineInit(szDefsPath, NULL, &stCallbacks, wHeuristicLevel, wFlags, &dwStatus);
#else
		    hEngine = NAVEngineInit(szDefsPath, NULL, &stCallbacks, wHeuristicLevel, wFlags, &dwStatus);
#endif


					sprintf(szOutputBuffer, "NAVEngineInit(%s, NULL, &stCallbacks, %d, %d, &dwStatus) returned 0x%lx",
						szDefsPath, wHeuristicLevel, wFlags, hEngine);
				}
				else
				{
#if defined(SYM_WIN)
		    hEngine = NavapiFuncs.lpfnEngineInit(szDefsPath, szNavexInf, &stCallbacks, wHeuristicLevel,
						wFlags, &dwStatus);
#else
					hEngine = NAVEngineInit(szDefsPath, szNavexInf, &stCallbacks, wHeuristicLevel,
						wFlags, &dwStatus);
#endif
					sprintf(szOutputBuffer, "NAVEngineInit(%s, %s, &stCallbacks, %d, %d, &dwStatus) returned 0x%lx",
						szDefsPath, szNavexInf, wHeuristicLevel, wFlags, hEngine);
				}
				OutputMessage(szOutputBuffer, pLog, FALSE);

				if (NAV_OK != dwStatus)
					hEngine = hOldEngine;                   // restore old engine handle

				NavstatusToString(dwStatus, szStatusMsg);
				sprintf(szOutputBuffer, "dwStatus contains %s", szStatusMsg);
				OutputMessage(szOutputBuffer, pLog, FALSE);
			}
			break;
		case 'B':
			{
#if defined(SYM_WIN)
		dwReturn = NavapiFuncs.lpfnEngineClose(hEngine);
#else
		dwReturn = NAVEngineClose(hEngine);
#endif
				NavstatusToString(dwReturn, szStatusMsg);
				sprintf(szOutputBuffer, "NAVEngineClose(0x%lx) returned %s", hEngine, szStatusMsg);
				OutputMessage(szOutputBuffer, pLog, FALSE);
			}
			break;
#ifdef SYM_WIN32
		case 'C':
			{
				TCHAR   szFileName[SYM_MAX_PATH];
				BOOL    bIsInfectable = FALSE;
				TCHAR   szIsInfectable[10];

				if (lpFile)
				{
					sscanf(szScriptLine, "%c %s", &cMenuOpt, szFileName);
				}
				else
				{
					printf("File to test: ");
					scanf("%s", szFileName);
				}

				if (GetExtension(lpszExt, szFileName) != 0)
					lpszExt[0] = NULL;

#if defined(SYM_WIN)
		dwReturn = NavapiFuncs.lpfnIsInfectableFile(hEngine, (LPVOID) szFileName, lpszExt, TRUE, &bIsInfectable);
#else
				dwReturn = NAVIsInfectableFile(hEngine, (LPVOID) szFileName, lpszExt, TRUE, &bIsInfectable);
#endif
				
				NavstatusToString(dwReturn, szStatusMsg);
				if (bIsInfectable)
					sprintf(szIsInfectable, "TRUE");
				else
					sprintf(szIsInfectable, "FALSE");
				sprintf(szOutputBuffer, "NAVIsInfectableFile(0x%lx, %s, %s) returned %s",
					hEngine, szFileName, lpszExt, szStatusMsg);
				OutputMessage(szOutputBuffer, pLog, FALSE);
				sprintf(szOutputBuffer, "bIsInfectable = %s", szIsInfectable);
				OutputMessage(szOutputBuffer, pLog, FALSE);
			}
			break;
#endif  // #ifdef SYM_WIN32
		case 'D':
			{
				TCHAR   szFileName[SYM_MAX_PATH];

				if (lpFile)
				{
					sscanf(szScriptLine, "%c %s", &cMenuOpt, szFileName);
				}
				else
				{
					printf("File to scan: ");
					scanf("%s", szFileName);
				}

				if (GetExtension(lpszExt, szFileName) != 0)
					lpszExt[0] = NULL;


#if defined(SYM_WIN)
		dwReturn = NavapiFuncs.lpfnScanFile(hEngine, (LPVOID) szFileName, lpszExt, TRUE, &hVirus);
#else
				dwReturn = NAVScanFile(hEngine, (LPVOID) szFileName, lpszExt, TRUE, &hVirus);
#endif

				NavstatusToString(dwReturn, szStatusMsg);
				sprintf(szOutputBuffer, "NAVScanFile(0x%lx, %s, %s, &hVirus) returned %s",
					hEngine, szFileName, lpszExt, szStatusMsg);
				OutputMessage(szOutputBuffer, pLog, FALSE);
			}
			break;
#ifndef SYM_NLM
		case 'E':
			{
				TCHAR cDriveLetter[1];

				if (lpFile)
				{
					sscanf(szScriptLine, "%c %s", &cMenuOpt, cDriveLetter);
				}
				else
				{
					printf("Drive Letter: ");
					scanf("%s", cDriveLetter);
				}

#if defined(SYM_WIN)
		dwReturn = NavapiFuncs.lpfnScanBoot(hEngine, cDriveLetter[0], &hVirus);
#else
		dwReturn = NAVScanBoot(hEngine, cDriveLetter[0], &hVirus);
#endif

				NavstatusToString(dwReturn, szStatusMsg);
				sprintf(szOutputBuffer, "NAVScanBoot(0x%lx, %c, &hVirus) returned %s",
					hEngine, cDriveLetter[0], szStatusMsg);
				OutputMessage(szOutputBuffer, pLog, FALSE);
			}
			break;
		case 'F':
			{
				UINT uPhysDriveNum = 0;

				if (lpFile)
				{
					sscanf(szScriptLine, "%c %d", &cMenuOpt, &uPhysDriveNum);
				}
				else
				{
					printf("Physical Drive Number (128 for C:) : ");
					scanf("%ld", &uPhysDriveNum);
				}
#if defined(SYM_WIN)
		dwReturn = NavapiFuncs.lpfnScanMasterBoot(hEngine, uPhysDriveNum, &hVirus);
#else
		dwReturn = NAVScanMasterBoot(hEngine, uPhysDriveNum, &hVirus);
#endif

				NavstatusToString(dwReturn, szStatusMsg);
				sprintf(szOutputBuffer, "NAVScanMasterBoot(0x%lx, %d, &hVirus) returned %s",
					hEngine, uPhysDriveNum, szStatusMsg);
				OutputMessage(szOutputBuffer, pLog, FALSE);
			}
			break;
#endif          // #ifndef SYM_NLM
		case 'G':
			{
#if defined(SYM_WIN)
		dwReturn = NavapiFuncs.lpfnScanMemory(hEngine, &hVirus);
#else
		dwReturn = NAVScanMemory(hEngine, &hVirus);
#endif

				NavstatusToString(dwReturn, szStatusMsg);
				sprintf(szOutputBuffer, "NAVScanMemory(0x%lx, &hVirus) returned %s",
					hEngine, szStatusMsg);
				OutputMessage(szOutputBuffer, pLog, FALSE);
			}
			break;
		case 'H':
			{
				TCHAR   szFileName[SYM_MAX_PATH];

				if (lpFile)
				{
					sscanf(szScriptLine, "%c %s", &cMenuOpt, szFileName);
				}
				else
				{
					printf("File to repair: ");
					scanf("%s", szFileName);
				}

				if (GetExtension(lpszExt, szFileName) != 0)
					lpszExt[0] = NULL;

#if defined(SYM_WIN)
		dwReturn = NavapiFuncs.lpfnRepairFile(hEngine, (LPVOID) szFileName, lpszExt);
#else
		dwReturn = NAVRepairFile(hEngine, (LPVOID) szFileName, lpszExt);
#endif

				NavstatusToString(dwReturn, szStatusMsg);
				sprintf(szOutputBuffer, "NAVRepairFile(0x%lx, %s, %s) returned %s",
					hEngine, szFileName, lpszExt, szStatusMsg);
				OutputMessage(szOutputBuffer, pLog, FALSE);
			}
			break;
#ifndef SYM_NLM
		case 'I':
			{
				TCHAR cDriveLetter[1];

				if (lpFile)
				{
					sscanf(szScriptLine, "%c %c", &cMenuOpt, &cDriveLetter);
				}
				else
				{
					printf("Drive Letter: ");
					scanf("%s", cDriveLetter);
				}
#if defined(SYM_WIN)
		dwReturn = NavapiFuncs.lpfnRepairBoot(hEngine, cDriveLetter[0]);
#else
		dwReturn = NAVRepairBoot(hEngine, cDriveLetter[0]);
#endif

				NavstatusToString(dwReturn, szStatusMsg);
				sprintf(szOutputBuffer, "NAVRepairBoot(0x%lx, %c) returned %s",
					hEngine, cDriveLetter[0], szStatusMsg);
				OutputMessage(szOutputBuffer, pLog, FALSE);
			}
			break;
		case 'J':
			{
				UINT uPhysDriveNum = 0;

				if (lpFile)
				{
					sscanf(szScriptLine, "%c %d", &cMenuOpt, &uPhysDriveNum);
				}
				else
				{
					printf("Physical Drive Number (128 for C:) : ");
					scanf("%ld", &uPhysDriveNum);
				}
#if defined(SYM_WIN)
		dwReturn = NavapiFuncs.lpfnRepairMasterBoot(hEngine, uPhysDriveNum);
#else
		dwReturn = NAVRepairMasterBoot(hEngine, uPhysDriveNum);
#endif

				NavstatusToString(dwReturn, szStatusMsg);
				sprintf(szOutputBuffer, "NAVRepairMasterBoot(0x%lx, %d) returned %s",
					hEngine, uPhysDriveNum, szStatusMsg);
				OutputMessage(szOutputBuffer, pLog, FALSE);
			}
			break;
#endif                  // #ifndef SYM_NLM
		case 'K':
			{
				TCHAR   szFileName[SYM_MAX_PATH];

				if (lpFile)
				{
					sscanf(szScriptLine, "%c %s", &cMenuOpt, szFileName);
				}
				else
				{
					printf("File to delete: ");
					scanf("%s", szFileName);
				}

				if (GetExtension(lpszExt, szFileName) != 0)
					lpszExt[0] = NULL;

#if defined(SYM_WIN)
		dwReturn = NavapiFuncs.lpfnDeleteFile(hEngine, (LPVOID) szFileName, lpszExt);
#else
				dwReturn = NAVDeleteFile(hEngine, (LPVOID) szFileName, lpszExt);
#endif	

				NavstatusToString(dwReturn, szStatusMsg);
				sprintf(szOutputBuffer, "NAVDeleteFile(0x%lx, %s, %s, &hVirus) returned %s",
					hEngine, szFileName, lpszExt, szStatusMsg);
				OutputMessage(szOutputBuffer, pLog, FALSE);
			}
			break;
		case 'L':
			{
#if defined(SYM_WIN)
		dwReturn = NavapiFuncs.lpfnFreeVirusHandle(hVirus);
#else
		dwReturn = NAVFreeVirusHandle(hVirus);
#endif

				NavstatusToString(dwReturn, szStatusMsg);
				sprintf(szOutputBuffer, "NAVFreeVirusHandle(0x%lx) returned %s",
					hVirus, szStatusMsg);
				OutputMessage(szOutputBuffer, pLog, FALSE);

				hVirus = NULL;
			}
			break;
		case 'M':
			{
				long    nVirusInfo = 0;
				DWORD   dwBufferSize = SYM_MAX_PATH;

				if (lpFile)
				{
					sscanf(szScriptLine, "%c %d", &cMenuOpt, &nVirusInfo);
				}
				else
				{
					printf("Virus Info Desired: ");
					scanf("%ld", &nVirusInfo);
				}
#if defined(SYM_WIN)
		dwReturn = NavapiFuncs.lpfnGetVirusInfo(hVirus, (NAVVIRINFO) nVirusInfo, lpszResultBuffer, &dwBufferSize);
#else
		dwReturn = NAVGetVirusInfo(hVirus, (NAVVIRINFO) nVirusInfo, lpszResultBuffer, &dwBufferSize);
#endif
				NavstatusToString(dwReturn, szStatusMsg);
				sprintf(szOutputBuffer, "NAVGetVirusInfo(0x%lx, %ld) returned %s", hVirus, nVirusInfo,
					szStatusMsg);
				OutputMessage(szOutputBuffer, pLog, FALSE);
				if (dwReturn)
					sprintf(szOutputBuffer, "Boolean value of: TRUE");
				else
					sprintf(szOutputBuffer, "Boolean value of: FALSE");
				sprintf(szOutputBuffer, "");
				OutputMessage(szOutputBuffer, pLog, FALSE);
				if (NULL != lpszResultBuffer)
				{
					sprintf(szOutputBuffer, "Result Buffer contains '%s'", lpszResultBuffer);
					lpszResultBuffer[0] = 0;
				}
				else
					sprintf(szOutputBuffer, "Result Buffer is NULL");
				OutputMessage(szOutputBuffer, pLog, FALSE);
			}
			break;
		case 'N':
			{
				DWORD   dwNumEntries = 0;
#if defined(SYM_WIN)
		dwReturn = NavapiFuncs.lpfnGetVirusDefCount(hEngine, &dwNumEntries);
#else
		dwReturn = NAVGetVirusDefCount(hEngine, &dwNumEntries);
#endif

				NavstatusToString(dwReturn, szStatusMsg);
				sprintf(szOutputBuffer, "NAVGetVirusDefCount(0x%lx, 0x%lx) returned %s",
					hEngine, &dwNumEntries, szStatusMsg);
				OutputMessage(szOutputBuffer, pLog, FALSE);
				sprintf(szOutputBuffer, "lpdwNumEntries points to %ld", dwNumEntries);
				OutputMessage(szOutputBuffer, pLog, FALSE);
			}
			break;
		case 'O':
			{
				DWORD   dwNumEntries = 0;
#if defined(SYM_WIN)
		dwReturn = NavapiFuncs.lpfnGetVirusDefMaxCount(hEngine, &dwNumEntries);
#else
		dwReturn = NAVGetVirusDefMaxCount(hEngine, &dwNumEntries);
#endif

				NavstatusToString(dwReturn, szStatusMsg);
				sprintf(szOutputBuffer, "NAVGetVirusDefMaxCount(0x%lx, 0x%lx) returned %s",
					hEngine, &dwNumEntries, szStatusMsg);
				OutputMessage(szOutputBuffer, pLog, FALSE);
				sprintf(szOutputBuffer, "lpdwNumEntries points to %ld", dwNumEntries);
				OutputMessage(szOutputBuffer, pLog, FALSE);
			}
			break;
		case 'P':
			{
				DWORD   dwNumEntries = 0;
#if defined(SYM_WIN)
		dwReturn = NavapiFuncs.lpfnGetVirusDefDetectCount(hEngine, &dwNumEntries);
#else
		dwReturn = NAVGetVirusDefDetectCount(hEngine, &dwNumEntries);
#endif

				NavstatusToString(dwReturn, szStatusMsg);
				sprintf(szOutputBuffer, "NAVGetVirusDefDetectCount(0x%lx, 0x%lx) returned %s",
					hEngine, &dwNumEntries, szStatusMsg);
				OutputMessage(szOutputBuffer, pLog, FALSE);
				sprintf(szOutputBuffer, "lpdwNumEntries points to %ld", dwNumEntries);
				OutputMessage(szOutputBuffer, pLog, FALSE);
			}
			break;
		case 'Q':
			{
				UINT    uEntryNum = 0;

				if (lpFile)
				{
					sscanf(szScriptLine, "%c %d", &cMenuOpt, &uEntryNum);
				}
				else
				{
					printf("Entry number : ");
					scanf("%d", &uEntryNum);
				}

#if defined(SYM_WIN)
		hVirusEntry = NavapiFuncs.lpfnLoadVirusDef(hEngine, uEntryNum);
#else
		hVirusEntry = NAVLoadVirusDef(hEngine, uEntryNum);
#endif

				sprintf(szOutputBuffer, "NAVLoadVirusDef(0x%lx, %d) returned 0x%lx",
					hEngine, uEntryNum, hVirusEntry);
				OutputMessage(szOutputBuffer, pLog, FALSE);
			}
			break;
		case 'R':
			{
#if defined(SYM_WIN)
		NavapiFuncs.lpfnReleaseVirusDef(hVirusEntry);
#else
		NAVReleaseVirusDef(hVirusEntry);
#endif

				sprintf(szOutputBuffer, "NAVReleaseVirusDef(0x%lx) called (no return type).", hVirusEntry);
				OutputMessage(szOutputBuffer, pLog, FALSE);

				hVirusEntry = NULL;
			}
			break;
		case 'S':
			{
				long    nVirusInfo = 0;
				DWORD dwBufferSize = SYM_MAX_PATH;

				if (lpFile)
				{
					sscanf(szScriptLine, "%c %d", &cMenuOpt, &nVirusInfo);
				}
				else
				{
					printf("Virus Info Desired: ");
					scanf("%ld", &nVirusInfo);
				}
#if defined(SYM_WIN)
		dwReturn = NavapiFuncs.lpfnGetVirusDefInfo(hVirusEntry, (NAVVIRINFO) nVirusInfo, lpszResultBuffer, &dwBufferSize);
#else
		dwReturn = NAVGetVirusDefInfo(hVirusEntry, (NAVVIRINFO) nVirusInfo, lpszResultBuffer, &dwBufferSize);
#endif

				NavstatusToString(dwReturn, szStatusMsg);
				sprintf(szOutputBuffer, "NAVGetVirusDefInfo(0x%lx, %ld) returned %s",
					hVirusEntry, nVirusInfo, szStatusMsg);
				OutputMessage(szOutputBuffer, pLog, FALSE);
				if (dwReturn)
					sprintf(szOutputBuffer, "Boolean value of: TRUE");
				else
					sprintf(szOutputBuffer, "Boolean value of: FALSE");
				OutputMessage(szOutputBuffer, pLog, FALSE);
				if (NULL != lpszResultBuffer)
				{
					sprintf(szOutputBuffer, "Result Buffer contains '%s'", lpszResultBuffer);
					lpszResultBuffer[0] = NULL;
				}
				else
					sprintf(szOutputBuffer, "Result Buffer is NULL");
				OutputMessage(szOutputBuffer, pLog, FALSE);
			}
			break;
		case 'T':
			{
#if defined(SYM_WIN)
		hVirusDefTable = NavapiFuncs.lpfnLoadVirusDefTable( hEngine );
#else
		hVirusDefTable = NAVLoadVirusDefTable( hEngine );
#endif
		sprintf(szOutputBuffer, "NAVLoadVirusDefTable(0x%lx) returned 0x%lx", hEngine, hVirusDefTable);
				OutputMessage(szOutputBuffer, pLog, FALSE);
			}
			break;
		case 'U':
			{
#if defined(SYM_WIN)
		NavapiFuncs.lpfnReleaseVirusDefTable(hVirusDefTable);
#else
		NAVReleaseVirusDefTable(hVirusDefTable);
#endif

				sprintf(szOutputBuffer, "NAVReleaseVirusDefTable(0x%lx) called (no return type)", hVirusDefTable);
				OutputMessage(szOutputBuffer, pLog, FALSE);

				hVirusDefTable = NULL;
			}
			break;
		case 'V':
			{
				DWORD   dwNumEntries = 0;
#if defined(SYM_WIN)
		dwReturn = NavapiFuncs.lpfnGetVirusDefTableCount(hVirusDefTable, &dwNumEntries);
#else
		dwReturn = NAVGetVirusDefTableCount(hVirusDefTable, &dwNumEntries);
#endif

				NavstatusToString(dwReturn, szStatusMsg);
				sprintf(szOutputBuffer, "NAVGetVirusDefTableCount(0x%lx) returned %s", hVirusDefTable, szStatusMsg);
				OutputMessage(szOutputBuffer, pLog, FALSE);
				sprintf(szOutputBuffer, "lpdwNumEntries points to %ld.", dwNumEntries);
				OutputMessage(szOutputBuffer, pLog, FALSE);
			}
			break;
		case 'W':
			{
				long    nVirusInfo = 0, nIndex = 0;
				DWORD   dwBufferSize = SYM_MAX_PATH;

				if (lpFile)
				{
					sscanf(szScriptLine, "%c %d %d", &cMenuOpt, &nIndex, &nVirusInfo);
				}
				else
				{
					printf("Virus Table Index : ");
					scanf("%ld", &nIndex);
					printf("Virus Info Desired: ");
					scanf("%ld", &nVirusInfo);
				}
#if defined(SYM_WIN)
		dwReturn = NavapiFuncs.lpfnGetVirusDefTableInfo(hVirusDefTable, nIndex,
									(NAVVIRINFO) nVirusInfo, lpszResultBuffer, &dwBufferSize);
#else
				dwReturn = NAVGetVirusDefTableInfo(hVirusDefTable, nIndex,
									(NAVVIRINFO) nVirusInfo, lpszResultBuffer, &dwBufferSize);
#endif
				NavstatusToString(dwReturn, szStatusMsg);
				sprintf(szOutputBuffer, "NAVGetVirusDefTableInfo(0x%lx, %ld, %ld) returned %s",
					hVirusDefTable, nIndex, nVirusInfo, szStatusMsg);
				OutputMessage(szOutputBuffer, pLog, FALSE);
				if (dwReturn)
					sprintf(szOutputBuffer, "Boolean value of: TRUE");
				else
					sprintf(szOutputBuffer, "Boolean value of: FALSE");
				OutputMessage(szOutputBuffer, pLog, FALSE);
				if (NULL != lpszResultBuffer)
				{
					sprintf(szOutputBuffer, "Result Buffer contains '%s'", lpszResultBuffer);
					lpszResultBuffer[0] = 0;
				}
				else
					sprintf(szOutputBuffer, "Result Buffer is NULL");
				OutputMessage(szOutputBuffer, pLog, FALSE);
			}
			break;
		case 'X':
			{
#if defined(SYM_WIN)
		dwReturn = NavapiFuncs.lpfnGetNavapiVerString(lpszResultBuffer, SYM_MAX_PATH);
#else
		dwReturn = NAVGetNavapiVersionString(lpszResultBuffer, SYM_MAX_PATH);
#endif

				NavstatusToString(dwReturn, szStatusMsg);
				sprintf(szOutputBuffer, "NAVGetNavapiVersionString() returned %s", szStatusMsg);
				OutputMessage(szOutputBuffer, pLog, FALSE);

				if (NULL != lpszResultBuffer)
				{
					sprintf(szOutputBuffer, "Result Buffer contains %s", lpszResultBuffer);
					lpszResultBuffer[0] = 0;
				}
				else
					sprintf(szOutputBuffer, "Result Buffer is NULL");
				OutputMessage(szOutputBuffer, pLog, FALSE);
			}
			break;
		case 'Y':
			{
				NAVVIRUSDBINFO sVirusDBInfo;
#if defined(SYM_WIN)
		dwReturn = NavapiFuncs.lpfnGetVirDbInfo(hEngine, &sVirusDBInfo);
#else
		dwReturn = NAVGetVirusDBInfo(hEngine, &sVirusDBInfo);
#endif

				NavstatusToString(dwReturn, szStatusMsg);
				sprintf(szOutputBuffer, "NAVGetVirusDBInfo(0x%lx, &sVirusDBInfo) returned %s",
					hEngine, szStatusMsg);
				OutputMessage(szOutputBuffer, pLog, FALSE);
				sprintf(szOutputBuffer, "VIRUSDBINFO contains Version %ld, %d/%d/%d.",
					sVirusDBInfo.dwVersion, sVirusDBInfo.wMonth, sVirusDBInfo.wDay,
					sVirusDBInfo.wYear);
				OutputMessage(szOutputBuffer, pLog, FALSE);
			}
			break;
		case 'Z':
			{
				NAVVIRUSDBINFOEX sVirusDBInfoEx;

				sVirusDBInfoEx.dwSize = sizeof (sVirusDBInfoEx);
#if defined(SYM_WIN)
		dwReturn = NavapiFuncs.lpfnGetVirDbInfoEx(hEngine, &sVirusDBInfoEx);
#else
		dwReturn = NAVGetVirusDBInfoEx(hEngine, &sVirusDBInfoEx);
#endif

				NavstatusToString(dwReturn, szStatusMsg);
				sprintf(szOutputBuffer, "NAVGetVirusDBInfoEx(0x%lx, &sVirusDBInfoEx) returned %s",
					hEngine, szStatusMsg);
				OutputMessage(szOutputBuffer, pLog, FALSE);
				sprintf(szOutputBuffer, "VIRUSDBINFO contains Version %ld, %d/%d/%d, sequence %ld.",
					sVirusDBInfoEx.dwVersion, (int) sVirusDBInfoEx.wMonth, (int) sVirusDBInfoEx.wDay,
					(int) sVirusDBInfoEx.wYear, sVirusDBInfoEx.dwSequence);
				OutputMessage(szOutputBuffer, pLog, FALSE);
			}
			break;
#ifndef SYM_NLM
		case '1':
			{
				TCHAR   szRootDir[SYM_MAX_PATH];

				if (lpFile)
				{
					sscanf(szScriptLine, "%c %s", &cMenuOpt, szRootDir);
				}
				else
				{
					printf("Root directory : ");
					scanf("%s", szRootDir);
				}


#if defined(SYM_WIN)
		ScanAndRepairDirectory(hEngine, szRootDir, FALSE, szOutputFile, &NavapiFuncs);
#else
		ScanAndRepairDirectory(hEngine, szRootDir, FALSE, szOutputFile);
#endif
			}
			break;
		case '2':
			{

				TCHAR   szRootDir[SYM_MAX_PATH];

				if (lpFile)
				{
					sscanf(szScriptLine, "%c %s", &cMenuOpt, szRootDir);
				}
				else
				{
					printf("Root directory : ");
					scanf("%s", szRootDir);
				}


#if defined(SYM_WIN)
		ScanAndRepairDirectory(hEngine, szRootDir, TRUE, szOutputFile, &NavapiFuncs);
#else
		ScanAndRepairDirectory(hEngine, szRootDir, TRUE, szOutputFile);
#endif
			}
			break;
#endif          // #ifndef SYM_NLM
	case '3':
			{

#if defined(SYM_WIN32)

                //Multi-threading option
                WORD wFuncOpt = 0;
                printf("File (=0), BR (=1), MBR(=2), Memory(=3): ");
                scanf("%d", &wFuncOpt);

                switch (wFuncOpt)
                {
                    case  0:   GetMTFileParams(hEngine, szOutputFile, &NavapiFuncs);
                               break;
                    case  1:   GetMTBRParams(hEngine, szOutputFile, &NavapiFuncs);
                               break;
                    case  2:   GetMTMBRParams(hEngine, szOutputFile, &NavapiFuncs);
                               break;
                    case  3:   GetMTMemoryParams(hEngine, szOutputFile, &NavapiFuncs);
                               break;
                    default:   printf("Invalid multithreading option.\n");
                }
#else
                printf("Multi-threading not implemented on this platform.\n");
#endif
            }
            break;
#ifndef SYM_NLM
		case '4':
			{
				TCHAR   szRootDir[SYM_MAX_PATH];

				if (lpFile)
				{
					sscanf(szScriptLine, "%c %s", &cMenuOpt, szRootDir);
				}
				else
				{
					printf("Root directory : ");
					scanf("%s", szRootDir);
				}

#if defined(SYM_WIN)
		DeleteDirectory(hEngine, szRootDir, szOutputFile, &NavapiFuncs);
#else
		DeleteDirectory(hEngine, szRootDir, szOutputFile);
#endif
			}
			break;
#endif          // #ifndef SYM_NLM

		case '5':
			bDone = TRUE;

		default:
			;
		}
	if (szScriptLine)
		free(szScriptLine);
	}

	// Deallocate memory and bail.

	free (lpszResultBuffer);
	free (lpszExt);				

	if ( szScriptFile != NULL)
		free (szScriptFile);
	if ( szOutputFile != NULL)
		free (szOutputFile);

#ifdef SYM_WIN
    FreeLibrary(hDLL);
#endif

#ifndef SYM_NLM
	if (pLog)
		delete pLog;
#else
	if (pLog)
		fclose(pLog);
#endif

	return 0;
}
