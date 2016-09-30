// PerformanceMonitor.cpp: implementation of the CPerformanceMonitor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PerformanceMonitor.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// The actual code to collect the performance statistics for a remote or
// local NT machine and AIX machines.

CPerformanceMonitor::CPerformanceMonitor(CString &machineName, CString &ntMachineName, CString &userName)
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
    LONG RC;

    memoryLow = FALSE;
    diskSpaceLow = FALSE;

    lpNameStrings = (LPSTR) NULL;
    lpNamesArray = (LPSTR *) NULL;
    m_hKey = (HKEY) NULL;
    firstTime = TRUE;
    PerfData = NULL;
    BufferSize = TOTALBYTES;
    newBufferSize = TOTALBYTES;
    m_MachineName = machineName;
    m_NTMachineName = ntMachineName;
    m_UserName = userName;

    if (ntMachineName.IsEmpty())
        return;

    // For NT machines, open the appropriate registry keys
    // There should be a user on the remote NT machine with the same
    // userid and password as the current user on the machine where this
    // program is being executed.

    if (ntMachineName.GetLength() < 2 ||
        ntMachineName[0] != '\\' ||
        ntMachineName[1] != '\\')
        m_NTMachineName = "\\\\";
    else
        m_NTMachineName = "";

    m_NTMachineName += ntMachineName;

    strcpy (m_CounterString, "");
    if (m_NTMachineName.Compare("\\\\LOCAL"))
    {
        RC = RegConnectRegistry (m_NTMachineName.GetBuffer(m_NTMachineName.GetLength() + 10),
                HKEY_PERFORMANCE_DATA, &m_hKey);
        // Get the name strings through the registry.
        if (RC == ERROR_SUCCESS)
            RC = GetNameStrings( m_CounterString
                ,m_NTMachineName.GetBuffer(m_NTMachineName.GetLength() + 10));
    }
    else
    {
        RC = RegConnectRegistry (NULL, HKEY_PERFORMANCE_DATA, &m_hKey);
        // Get the name strings through the registry.
        if (RC == ERROR_SUCCESS)
            RC = GetNameStrings( m_CounterString , NULL);
    }

    if (RC != ERROR_SUCCESS)
        m_hKey = (HKEY) NULL;

    firstTime = FALSE;
}

CPerformanceMonitor::~CPerformanceMonitor()
{
    if (PerfData)
        free (PerfData);

    if (lpNamesArray)
        free (lpNamesArray);

    if (lpNameStrings)
        free (lpNameStrings);

    if (m_hKey)
        RegCloseKey (m_hKey);
}

// Add any other operations required on this class.
// Look at the Section 'Windows NT Performance Counters' in the MSDN Library
// for details about the performance counters and there formats

PPERF_OBJECT_TYPE 
CPerformanceMonitor::FirstObject( PPERF_DATA_BLOCK PerfData )
{
    return( (PPERF_OBJECT_TYPE)((PBYTE)PerfData + 
        PerfData->HeaderLength) );
}

PPERF_OBJECT_TYPE 
CPerformanceMonitor::NextObject( PPERF_OBJECT_TYPE PerfObj )
{
    return( (PPERF_OBJECT_TYPE)((PBYTE)PerfObj + 
        PerfObj->TotalByteLength) );
}

PPERF_INSTANCE_DEFINITION 
CPerformanceMonitor::FirstInstance( PPERF_OBJECT_TYPE PerfObj )
{
    return( (PPERF_INSTANCE_DEFINITION)((PBYTE)PerfObj + 
        PerfObj->DefinitionLength) );
}

PPERF_INSTANCE_DEFINITION 
CPerformanceMonitor::NextInstance( 
    PPERF_INSTANCE_DEFINITION PerfInst )
{
    PPERF_COUNTER_BLOCK PerfCntrBlk;

    PerfCntrBlk = (PPERF_COUNTER_BLOCK)((PBYTE)PerfInst + 
        PerfInst->ByteLength);

    return( (PPERF_INSTANCE_DEFINITION)((PBYTE)PerfCntrBlk + 
        PerfCntrBlk->ByteLength) );
}

PPERF_COUNTER_DEFINITION 
CPerformanceMonitor::FirstCounter( PPERF_OBJECT_TYPE PerfObj )
{
    return( (PPERF_COUNTER_DEFINITION) ((PBYTE)PerfObj + 
        PerfObj->HeaderLength) );
}

PPERF_COUNTER_DEFINITION 
CPerformanceMonitor::NextCounter( 
    PPERF_COUNTER_DEFINITION PerfCntr )
{
    return( (PPERF_COUNTER_DEFINITION)((PBYTE)PerfCntr + 
        PerfCntr->ByteLength) );
}

/*****************************************************************
 *                                                               *
 * Load the counter and object names from the registry to the    *
 * global variable lpNamesArray.                                 *
 *                                                               *
 *****************************************************************/


LONG 
CPerformanceMonitor::GetNameStrings( char * counterList, char * machine)
{
    HKEY hKeyPerflib;      // handle to registry key
    HKEY hKeyPerflib009;   // handle to registry key
    DWORD dwMaxValueLen;   // maximum size of key values
    DWORD dwMaxValueBufferLen;
    DWORD dwBuffer;        // bytes to allocate for buffers
    DWORD dwBufferSize;    // size of dwBuffer
    LPSTR lpCurrentString; // pointer for enumerating data strings
    DWORD dwCounter;       // current counter index
    HKEY hKey;
    LONG RC;

// Get the number of Counter items.

    RC = RegConnectRegistry (machine, HKEY_LOCAL_MACHINE, &hKey);

    if (RC != ERROR_SUCCESS)
        return RC;

    RC = RegOpenKeyEx( hKey,
        "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Perflib",
        0,
        KEY_READ,
        &hKeyPerflib);
    if (RC != ERROR_SUCCESS)
    {
        RegCloseKey (hKey);
        return RC;
    }

    dwBufferSize = sizeof(dwBuffer);

    RC = RegQueryValueEx( hKeyPerflib,
        "Last Counter",
        NULL,
        NULL,
        (LPBYTE) &dwBuffer,
        &dwBufferSize );

    RegCloseKey( hKeyPerflib );
    if (RC != ERROR_SUCCESS)
    {
        RegCloseKey (hKey);
        return RC;
    }
// Allocate memory for the names array.

    if (lpNamesArray)
        free (lpNamesArray);

    lpNamesArray = (LPSTR *) malloc( (dwBuffer+1) * sizeof(LPSTR) );
    if (!lpNamesArray)
    {
        RegCloseKey (hKey);
        return !ERROR_SUCCESS;
    }

// Open key containing counter and object names.

    RC = RegConnectRegistry (machine, HKEY_PERFORMANCE_DATA, &hKeyPerflib009);

    /*
    RC = RegOpenKeyEx( hKey,
    "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Perflib\\009",
        0,
        KEY_READ,
        &hKeyPerflib009);
    if (RC != ERROR_SUCCESS)
    {
        RegCloseKey (hKey);
        return RC;
    }
    */

// Get the size of the largest value in the key (Counter or Help).

    /*
    RC = RegQueryInfoKey( hKeyPerflib009,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        &dwMaxValueLen,
        NULL,
        NULL);
    if (RC != ERROR_SUCCESS)
    {
        RegCloseKey( hKeyPerflib009 );
        RegCloseKey (hKey);
        return RC;
    }
    */

// Allocate memory for the counter and object names.

    dwMaxValueBufferLen = 0;

    RC = RegQueryValueEx( hKeyPerflib009,
                            "Counter 009",
                            NULL,
                            NULL,
                            NULL,
                            &dwMaxValueBufferLen );

    if (dwMaxValueBufferLen <= 0)
        dwMaxValueBufferLen = TOTALBYTES;

    dwMaxValueLen = dwMaxValueBufferLen;

    if (lpNameStrings)
        free (lpNameStrings);

    lpNameStrings = (LPSTR) malloc( dwMaxValueLen );
    if (!lpNameStrings)
    {
        RegCloseKey( hKeyPerflib009 );
        RegCloseKey (hKey);
        return !ERROR_SUCCESS;
    }
    memset (lpNameStrings, '\0', dwMaxValueLen);

// Read Counter value.

    while (lpNameStrings &&
        ((RC = RegQueryValueEx( hKeyPerflib009,
                                "Counter 009",
                                NULL,
                                NULL,
                                (LPBYTE) lpNameStrings,
                                &dwMaxValueBufferLen )) == ERROR_MORE_DATA) )
    {
        dwMaxValueLen += BYTEINCREMENT;
        dwMaxValueBufferLen = dwMaxValueLen;
        lpNameStrings = (LPSTR) realloc( lpNameStrings, dwMaxValueLen );
        memset (lpNameStrings, '\0', dwMaxValueLen);
    }

    RegCloseKey( hKeyPerflib009 );
    RegCloseKey ( hKey );
    if (RC != ERROR_SUCCESS)
    {
        return RC;
    }

// Load names into an array, by index.

    for( lpCurrentString = lpNameStrings; *lpCurrentString;
         lpCurrentString += (lstrlen(lpCurrentString)+1) )
    {
        dwCounter = atol( lpCurrentString );

        lpCurrentString += (lstrlen(lpCurrentString)+1);

        if (!strcmp(lpCurrentString, "System") ||
            !strcmp(lpCurrentString, "Memory") ||
            !strcmp(lpCurrentString, "LogicalDisk") )
        {
            char tmpStr[128];

            sprintf (tmpStr, "%ld", (LONG) dwCounter);
            if (counterList[0])
                strcat (counterList, " ");
            strcat (counterList, tmpStr);
        }

        if (!strcmp(lpCurrentString, "% Total Processor Time"))
            processorTimeIndex = dwCounter;

        if (!strcmp(lpCurrentString, "% Total User Time"))
            userTimeIndex = dwCounter;

        if (!strcmp(lpCurrentString, "% Total Privileged Time"))
            kernelTimeIndex = dwCounter;

        if (!strcmp(lpCurrentString, "Available Bytes"))
            availableByteIndex = dwCounter;

        if (!strcmp(lpCurrentString, "Committed Bytes"))
            commitedByteIndex = dwCounter;

        if (!strcmp(lpCurrentString, "Commit Limit"))
            commitLimitIndex = dwCounter;

        if (!strcmp(lpCurrentString, "System Up Time"))
            systemUpTimeIndex = dwCounter;

        if (!strcmp(lpCurrentString, "% Free Space"))
            diskFreeIndex = dwCounter;

        if (!strcmp(lpCurrentString, "Free Megabytes"))
            diskMBFreeIndex = dwCounter;

        lpNamesArray[dwCounter] = (LPSTR) lpCurrentString;
    }

    return (ERROR_SUCCESS);
}


void CPerformanceMonitor::GetPerformanceData()
{
    LONG RC = ERROR_SUCCESS;
    LONGLONG newTime;
    LONGLONG newProcessorCount;
    LONGLONG newUserTimeCount;
    LONGLONG newKernelTimeCount;
    PPERF_OBJECT_TYPE PerfObj;
    PPERF_INSTANCE_DEFINITION PerfInst;
    PPERF_COUNTER_DEFINITION PerfCntr;
    PPERF_COUNTER_DEFINITION CurCntr;

    diskSpaceLow = FALSE;
    memoryLow = FALSE;

    if (m_hKey == NULL)
    {
        if (!m_NTMachineName.IsEmpty())
            m_ErrorCode = !ERROR_SUCCESS;
        else
            m_ErrorCode = GetAIXPerformanceData();

        m_DataValid = TRUE;
        return ;
    }

    if (!PerfData)
        PerfData = (PPERF_DATA_BLOCK) malloc( BufferSize );

    while( PerfData && 
           ((RC = RegQueryValueEx( m_hKey,
                                   m_CounterString,
                                   NULL,
                                   NULL,
                                   (LPBYTE) PerfData,
                                   &newBufferSize )) == ERROR_MORE_DATA) )
    {
        // Get a buffer that is big enough.
        BufferSize += BYTEINCREMENT;
        newBufferSize = BufferSize;
        PerfData = (PPERF_DATA_BLOCK) realloc( PerfData, BufferSize );
    }

    if (RC != ERROR_SUCCESS)
    {
        m_ErrorCode = RC;
        m_DataValid = TRUE;
        return ;
    }

    newTime = PerfData->PerfTime100nSec.QuadPart;

    // Get the first object type.
    PerfObj = FirstObject( PerfData );

    diskFreeString.Empty();

    // Process all objects.
    for( UINT i=0; i < PerfData->NumObjectTypes; i++ )
    {
        PerfCntr = FirstCounter( PerfObj );

        if (PerfObj->NumInstances > 0)
        {
            // Get the first instance.

            PerfInst = FirstInstance( PerfObj );

            // Retrieve all instances.
            for( int k=0; k < PerfObj->NumInstances; k++ )
            {
                // Display the instance by name.
                PPERF_COUNTER_BLOCK PerfCntrBlk;
                CString instString;

                instString.Format ("%S", (char *)((PBYTE)PerfInst + PerfInst->NameOffset));
                /*
                if (!instString.Compare("_Total"))
                    instString = "Total";
                */

                if (instString.Compare("_Total"))
                {
                    CurCntr = PerfCntr;

                    diskFreeData = 0;
                    diskFreeBase = 0;

                    // Retrieve all counters.
                    PerfCntrBlk = (PPERF_COUNTER_BLOCK)((PBYTE)PerfInst + 
                        PerfInst->ByteLength);

                    for( UINT j=0; j < PerfObj->NumCounters; j++ )
                    {
                        DWORD * pdwValue = (DWORD *) ((PBYTE) PerfCntrBlk + CurCntr->CounterOffset);

                        if (CurCntr->CounterNameTitleIndex == diskFreeIndex)
                        {
                            if (CurCntr->CounterType == PERF_RAW_FRACTION)
                                diskFreeData = *pdwValue;
                            if (CurCntr->CounterType == PERF_RAW_BASE)
                                diskFreeBase = *pdwValue;
                        }
                        if (CurCntr->CounterNameTitleIndex == diskMBFreeIndex)
                            diskMBFree = *pdwValue;

                        // Get the next counter.
                        CurCntr = NextCounter( CurCntr );
                    }
                    if (diskFreeBase)
                    {
                        CString tmpStr;

                        tmpStr.Format ( "%4ld MB (%3ld %%)",
                            (LONG) diskMBFree,
                            (LONG) ((diskFreeData * 100) / diskFreeBase));

                        diskFreeString += instString;
                        diskFreeString += " ";
                        diskFreeString += tmpStr;
                        diskFreeString += " ";

                        if (((diskFreeData * 100) / diskFreeBase) <= 10)
                            diskSpaceLow = TRUE;
                    }
                }

                // Get the next instance.
                PerfInst = NextInstance( PerfInst );
            }
        }
        else
        {
            PPERF_COUNTER_BLOCK PerfCntrBlk;

            // Get the counter block.
            PerfCntrBlk = (PPERF_COUNTER_BLOCK) ((PBYTE)PerfObj +
                            PerfObj->DefinitionLength );

            for( UINT j=0; j < PerfObj->NumCounters; j++ )
            {
                PBYTE dataPtr;

                dataPtr = (PBYTE) PerfCntrBlk + PerfCntr->CounterOffset;

                if (PerfCntr->CounterNameTitleIndex == availableByteIndex)
                {
                    availableMemoryCount = *((DWORD *) dataPtr);
                }
                else
                if (PerfCntr->CounterNameTitleIndex == commitedByteIndex)
                {
                    commitedMemoryCount = *((DWORD *) dataPtr);
                }
                else
                if (PerfCntr->CounterNameTitleIndex == commitLimitIndex)
                {
                    maxCommitedMemoryCount = *((DWORD *) dataPtr);
                }
                else
                if (PerfCntr->CounterNameTitleIndex == processorTimeIndex)
                {
                    newProcessorCount = *((LONGLONG UNALIGNED *) dataPtr);
                }
                else
                if (PerfCntr->CounterNameTitleIndex == userTimeIndex)
                {
                    newUserTimeCount = *((LONGLONG UNALIGNED *) dataPtr);
                }
                else
                if (PerfCntr->CounterNameTitleIndex == kernelTimeIndex)
                {
                    newKernelTimeCount = *((LONGLONG UNALIGNED *) dataPtr);
                }
                else
                if (PerfCntr->CounterNameTitleIndex == systemUpTimeIndex)
                {
                    rawSystemUpTime = *((LONGLONG UNALIGNED *) dataPtr);
                    systemUpPerfTime = PerfObj->PerfTime.QuadPart;
                    systemUpPerfFreq = PerfObj->PerfFreq.QuadPart;
                }

                // Get the next counter.
                PerfCntr = NextCounter( PerfCntr );
            }
            if ((maxCommitedMemoryCount - commitedMemoryCount) <= (maxCommitedMemoryCount / 20))
            {
                memoryLow = TRUE;
            }
        }
        PerfObj = NextObject( PerfObj );
    }

    if (firstTime != TRUE)
    {
        LONGLONG timeDifference = (newTime - oldTime);
        LONGLONG countDifference = (newProcessorCount - oldProcessorCount);
        LONGLONG userCountDifference = (newUserTimeCount - oldUserTimeCount);
        LONGLONG kernelCountDifference = (newKernelTimeCount - oldKernelTimeCount);
        double eProcessFraction = 0.0;
        double eUserFraction = 0.0;
        double eKernelFraction = 0.0;

        if (timeDifference > 0)
        {
            eProcessFraction = ((double) countDifference / (double) timeDifference);
            eUserFraction = ((double) userCountDifference / (double) timeDifference);
            eKernelFraction = ((double) kernelCountDifference / (double) timeDifference);
        }

        if (eProcessFraction >= 1.0)
            eProcessFraction = 1.0;
        else
        if (eProcessFraction <= 0.0)
            eProcessFraction = 0.0;

        if (eUserFraction >= 1.0)
            eUserFraction = 1.0;
        else
        if (eUserFraction <= 0.0)
            eUserFraction = 0.0;

        if (eKernelFraction >= 1.0)
            eKernelFraction = 1.0;
        else
        if (eKernelFraction <= 0.0)
            eKernelFraction = 0.0;

        processorTime = (int ) (((1.0 - eProcessFraction) * 100.0) + 0.5);
        userTime = (int ) ((eUserFraction * 100.0) + 0.5);
        kernelTime = (int) ((eKernelFraction * 100.0) + 0.5);
        systemUpTime = (LONG) ((LONGLONG) (systemUpPerfTime - rawSystemUpTime) / systemUpPerfFreq);
    }

    COleDateTime curTime = COleDateTime::GetCurrentTime();
    COleDateTimeSpan diffTime(0,0,0,systemUpTime);

    curTime -= diffTime;
    systemUpTimeStr.Format ("%ld-%02ld:%02ld:%02ld",
        diffTime.GetDays(),
        diffTime.GetHours(),
        diffTime.GetMinutes(),
        diffTime.GetSeconds());

    oldTime = newTime;
    oldProcessorCount = newProcessorCount;
    oldUserTimeCount = newUserTimeCount;
    oldKernelTimeCount = newKernelTimeCount;

    m_ErrorCode = RC;
    m_DataValid = TRUE;
    return ;
}

LONG CPerformanceMonitor::GetAIXPerformanceData()
{
    TCHAR cmdLine[1024];
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    FILE *fpAIXStat = (FILE *) NULL;
    FILE *fpTmp = (FILE *) NULL;
    LONG RC = !ERROR_SUCCESS;
    char batchFileName[256];
    char outFileName[256];
    char *ptr = NULL;

    ptr = tempnam (NULL, "ISDF");
    if (ptr)
    {
        fpTmp = fopen (ptr, _T("w"));
        if (fpTmp)
        {
            fclose (fpTmp);
            strcpy (batchFileName, ptr);
            strcpy (outFileName, ptr);
            strcat (batchFileName, ".bat");
            strcat (outFileName, ".out");
            unlink (batchFileName);
            unlink (outFileName);
            fpAIXStat = fopen (batchFileName, _T("w"));
        }
    }

    if (fpAIXStat)
    {
        // Look at the man pages for vmstat, lsps, ps, cut, sed and grep for the meaning of the following long command
        fprintf (fpAIXStat, "rsh %s -l %s \"vmstat 1 2 | grep \\\"[0-9]\\\" | sed -e \\\"s/  */ /g\\\" | cut -f 4-5,15-18 -d \\\" \\\" && lsps -s | grep MB | sed -e \\\"s/  */ /g\\\" | cut -f 2 -d \\\" \\\" | sed -e \\\"s/MB//\\\" && ps -p 0 -o etime | grep \\\"[0-9]\\\" | sed -e \\\"s/ *//g\\\" && df -k -I | grep -v \\\"^Filesystem\\\" | sed -e \\\"s/  */ /g\\\" | cut -f 1,4- -d \\\" \\\" \" > %s",
            m_MachineName,
            m_UserName,
            outFileName);
        fclose (fpAIXStat);

        sprintf (cmdLine, _T(batchFileName));

        memset (&si, 0, sizeof (STARTUPINFO));
        si.cb = sizeof (STARTUPINFO);
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;
        si.lpTitle = cmdLine;
        memset (&pi, 0, sizeof (PROCESS_INFORMATION));
        CreateProcess (NULL,
            cmdLine,
            NULL,
            NULL,
            FALSE,
            0,
            NULL,
            NULL,
            &si,
            &pi);

        WaitForSingleObject(pi.hProcess, 30000); // Exit after 30 seconds
        TerminateProcess (pi.hProcess, 0);
        CloseHandle (pi.hProcess);
        CloseHandle (pi.hThread);

        fpAIXStat = fopen (outFileName, _T("r"));
        if (fpAIXStat)
        {
            char inBuf[1024];
            CString dispStr;

            fgets (inBuf, sizeof(inBuf), fpAIXStat);

            if (fgets (inBuf, sizeof(inBuf), fpAIXStat))
            {
                int idle = 0;
                int wIdle = 0;

                if (inBuf[strlen(inBuf) - 1] == '\n')
                    inBuf[strlen(inBuf) - 1] = '\0';

                sscanf (inBuf, "%ld %ld %d %d %d %d",
                    &commitedMemoryCount,
                    &availableMemoryCount,
                    &userTime,
                    &kernelTime,
                    &idle,
                    &wIdle);

                commitedMemoryCount *= 4096;
                availableMemoryCount *= 4096;
                processorTime = 100 - idle - wIdle;
                if (processorTime < 0)
                    processorTime = 0;
                if (processorTime > 100)
                    processorTime = 100;
            }

            if (fgets (inBuf, sizeof(inBuf), fpAIXStat))
            {
                if (inBuf[strlen(inBuf) - 1] == '\n')
                    inBuf[strlen(inBuf) - 1] = '\0';

                maxCommitedMemoryCount = atol (inBuf);
                maxCommitedMemoryCount *= (1024 * 1024);
            }

            if (fgets (inBuf, sizeof(inBuf), fpAIXStat))
            {
                if (inBuf[strlen(inBuf) - 1] == '\n')
                    inBuf[strlen(inBuf) - 1] = '\0';

                systemUpTimeStr = inBuf;
                RC = ERROR_SUCCESS;
            }
            else
            {
                RC = !ERROR_SUCCESS;
            }

            diskFreeString.Empty();

            while (fgets (inBuf, sizeof(inBuf), fpAIXStat))
            {
                char devName[1024];
                char mountName[1024];
                LONG diskFree;
                LONG usePercent;

                if (inBuf[strlen(inBuf) - 1] == '\n')
                    inBuf[strlen(inBuf) - 1] = '\0';

                sscanf (inBuf, "%s %ld %ld%% %s",
                    devName, &diskFree, &usePercent, mountName);

                dispStr.Format("%s[%s] %4ld MB (%3ld %%)",
                    devName, mountName, (LONG) (diskFree/1024), (LONG) (100 - usePercent));

                diskFreeString += dispStr;
                diskFreeString += " ";
            }

            fclose (fpAIXStat);
        }
        int urc = 1;
        int trycount = 0;

        while (urc != 0)
        {
            urc = remove (outFileName);
            trycount++;

            // If the remote AIX machine is not available, the rsh command may
            // take a long time to complete and come back with an error. During
            // this time, the outfile is kept open by the command and the fil
            // cannot be deleted. We just wait another 3 minutes and keep on 
            // trying to delete the file.
            if (trycount > 180)     // Try for 3 minutes
                break;

            Sleep (1000);
        }
        unlink (batchFileName);
    }

    if (ptr)
    {
        unlink (ptr);
        free (ptr);
    }

    return RC;
}
