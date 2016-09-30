// Copyright 1991-1997 Symantec Corporation, Peter Norton Product Group
//************************************************************************
//
//  $Header:   S:/IAL/VCS/workset.cpv   1.1   25 Jun 1997 17:24:44   DSACKIN  $
//
//  Description:
//      This file contains the source code for enumerating process and
//      looking for a particular process to end.
//
//************************************************************************
//
//  $Log:   S:/IAL/VCS/workset.cpv  $
// 
//    Rev 1.1   25 Jun 1997 17:24:44   DSACKIN
// Ported from Marconi
// 
//    Rev 1.5   24 Apr 1997 11:06:08   pvenkat
// WaitForProcess, currently looks at Performance Database instead of 
// OpenProcessToken & WaitForSingleObject.
// 
// 
//    Rev 1.4   12 Feb 1997 13:09:54   pvenkat
// Added extra while loop to look for Process again even when we find first time
// For some unknown reason, first time we find process and some times the id 
// seem to be 0 or immediate snapshot reports process being missing.  Looking 
// the for second avoids this problem. (We look for max of 5 times or timeout
// seconds).
// 
// 
// 
// 
//    Rev 1.3   07 Feb 1997 12:32:52   pvenkat
// #79353.  Modified Timeout limit to 120 seconds (from 90 seconds).
// 
// 
//    Rev 1.2   27 Jan 1997 16:41:02   pvenkat
// Decreased time delay between the loops, to 30 seconds (instead for 90 secs).
// 
// 
// 
//    Rev 1.1   24 Jan 1997 13:02:50   pvenkat
// Fixed #79353.  Added more time between getting snapshot of processes.
// 
//
//************************************************************************

#define	WIN32_LEAN_AND_MEAN

#include	<windows.h>
#include	<winperf.h>
#include	<tchar.h>
#include	<stdio.h>
#include	<stdlib.h>

#pragma hdrstop

#include	"perfdata.h"

extern DWORD
WriteLogFile (LPTSTR lpszActivity, BOOL bAppend, LPTSTR lpszMsg);

DWORD   gpId = 0;

BOOL
WaitForProcess (LPTSTR lpszProcess)
{
    BOOL	fFoundProcess = FALSE;
    TCHAR	szProgram[MAX_PATH];

//    #ifdef	UNICODE
//        mbstowcs (szProgram, lpszProcess, sizeof (szProgram));
//    #else
        lstrcpyn (szProgram, lpszProcess, sizeof (szProgram));
//    #endif

    CPerfTitleDatabase	gCounterTitles (PERF_TITLE_COUNTER);
    CPerfSnapshot 		snapshot (&gCounterTitles);

    if (!snapshot.TakeSnapshot (TEXT ("Process")) )
    {
        _tprintf (TEXT ("Failed to make snapshot \n"));
        return 0;
    }

    CPerfObjectList objectList (&snapshot, &gCounterTitles);
    PCPerfObject	pPerfObj = objectList.GetPerfObject (TEXT ("Process"));

    if (!pPerfObj)
    {
        _tprintf (TEXT ("Process Performance object not found"));
        return 0;
    }

    PCPerfObjectInstance pPerfObjInst;

    for (pPerfObjInst = pPerfObj->GetFirstObjectInstance ();
            pPerfObjInst;
            pPerfObjInst = pPerfObj->GetNextObjectInstance ())
    {
        TCHAR		szInstanceName [260];
        unsigned	nSizeInstanceName = sizeof (szInstanceName);

        if (!pPerfObjInst->GetObjectInstanceName (szInstanceName, nSizeInstanceName))
            continue;

        if (lstrcmpi (szInstanceName, szProgram) )
            continue;

        // Process Found
        //		_tprintf (TEXT ("%s found\n"), szProgram);

        CPerfCounter *pCounter;

        pCounter = pPerfObjInst->GetCounterByName (TEXT ("Working Set"));
        if (!pCounter)
        {
            _tprintf (TEXT ("Working set counter not found\n"));
            return 0;
        }
        TCHAR szCounterDataStr[256];
        unsigned nSizeCounterDataStr = sizeof (szCounterDataStr) / sizeof (TCHAR);

        if (pCounter->Format (szCounterDataStr, nSizeCounterDataStr, TRUE) )
        {
        //			_tprintf (TEXT ("Working set : %s bytes\n"), szCounterDataStr);
        }

        delete pCounter;

        pCounter = pPerfObjInst->GetCounterByName (TEXT ("ID Process"));
        if (!pCounter)
        {
            _tprintf (TEXT ("ID Process counter not found\n"));
            return 0;
        }
        //		TCHAR szCounterDataStr[256];
        //		unsigned nSizeCounterDataStr = sizeof (szCounterDataStr) / sizeof (TCHAR);

        if (pCounter->Format (szCounterDataStr, nSizeCounterDataStr, FALSE) )
        {
        //			_tprintf (TEXT ("ID Process : %s \n"), szCounterDataStr);
            gpId = _ttoi (szCounterDataStr);
        }

        delete pCounter;

        fFoundProcess = TRUE;
        break;	// Only one instance is ok, lets exit.
    }
    delete pPerfObj;

    return fFoundProcess;
}


int WaitforInst32 (LPTSTR lpszInst)
{
    BOOL	bFound = FALSE;
    DWORD	dwMaxTime = 120000, dwCur = 0;
    short   nIter;
#ifdef  DEBUG
    SYSTEMTIME  rTime;
    TCHAR       szBuf[512];
#endif

    // Wait for half-a-second
    Sleep (500);

    nIter = 0;

    while (gpId == 0 && nIter < 5)
    {
        bFound = FALSE;
        dwCur = 0;

        // Loop until the process comes up
        while (!bFound)
        {
            // Wait for half a second
            Sleep (500);

            // Look for process.
            bFound = WaitForProcess (lpszInst);

            // Check for time out.
            dwCur += 500;
            if (dwCur > dwMaxTime)
                break;
        }
#ifdef  DEBUG
        if (bFound && gpId == 0)
            MessageBox (NULL, 
                        "Process found with ID 0", 
                        "ERROR", 
                        MB_OK | MB_SETFOREGROUND | MB_TOPMOST);
#endif

        if (!bFound)
            break;
        else
            nIter ++;
    }

#ifdef  DEBUG
    GetLocalTime (&rTime);
    wsprintf (szBuf, 
                TEXT ("%02d-%02d-%04d %02d:%02d:%02d %03d"), 
                rTime.wDay,
                rTime.wMonth,
                rTime.wYear,
                rTime.wHour,
                rTime.wMinute,
                rTime.wSecond,
                rTime.wMilliseconds);
    MessageBox (NULL, szBuf, "After First loop", MB_OK);

    // Give a long sleep (Slow machines to catch up).
//    Sleep (dwMaxTime);
    Sleep (60 * 500);

    dwCur = 0;

    wsprintf (szBuf1,  "ProcessID : %x", gpId);
    MessageBox (NULL, szBuf1, "ProcessID", MB_OK);

    TCHAR   szBuf1[512];
    HANDLE  hProcess;

    if (bFound)
    {
        hProcess = OpenProcess (PROCESS_QUERY_INFORMATION | SYNCHRONIZE, 
                                FALSE, 
                                gpId);

        if (hProcess == NULL)
        {
            wsprintf (szBuf1, "Error in OpenProcess : %d", GetLastError ());
            MessageBox (NULL, szBuf1, "Error in OpenProcess", MB_OK);
        }
        

        WaitForSingleObject (hProcess, INFINITE);
    }
    MessageBox (NULL, "Process Over", "???", MB_OK);
#endif

    // Loop until the process is over.
	while (bFound)
	{
        // Wait for half a sec
		Sleep (500);

        // Look for process
		bFound = WaitForProcess (lpszInst);
	}

#ifdef  DEBUG
    GetLocalTime (&rTime);
    wsprintf (szBuf, 
                TEXT ("%02d-%02d-%04d %02d:%02d:%02d %03d"), 
                rTime.wDay,
                rTime.wMonth,
                rTime.wYear,
                rTime.wHour,
                rTime.wMinute,
                rTime.wSecond,
                rTime.wMilliseconds);
    MessageBox (NULL, szBuf, "After Second loop", MB_OK);
#endif

	return 0;
}
