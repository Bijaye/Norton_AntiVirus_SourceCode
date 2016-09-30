/////////////////////////////////////////////////////////////////////////////
//
// test.cpp
// NAVAPI test program
// Copyright 1998 by Symantec Corporation.  All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////
//
// $Header:   S:/NAVAPI/VCS/test.cpv   1.8   27 Aug 1998 20:06:20   dhertel  $
// $Log:   S:/NAVAPI/VCS/test.cpv  $
//
//    Rev 1.8   27 Aug 1998 20:06:20   dhertel
//
//    Rev 1.7   25 Aug 1998 13:51:52   dhertel
//
//    Rev 1.6   20 Aug 1998 15:03:06   DHERTEL
// Removed unneeded headers.
//
//    Rev 1.5   12 Aug 1998 17:16:12   dhertel
// Fixed return value in FileClose callback
//
//    Rev 1.4   11 Aug 1998 12:57:22   DHERTEL
// Added calls to new API functions
//
//    Rev 1.3   10 Aug 1998 13:05:28   DHERTEL
// Added some error checking
//
//    Rev 1.2   31 Jul 1998 19:09:44   DHERTEL
// Brought virus info code online for NLM platform.
//
//    Rev 1.1   30 Jul 1998 21:52:44   DHERTEL
// Changes for NLM packaging of NAVAPI.
//
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Headers

// Standard library headers
#include "platform.h"
#include "file.h"

#include <stdlib.h>
#ifdef SYM_WIN32
  #include <conio.h>
#endif
#include <stdio.h>
#include <time.h>
#ifndef SYM_UNIX
  #include <io.h>
#endif
#if defined(SYM_NLM)
  #include <process.h>
  #include <conio.h>
#endif

// Symantec core library headers


// NAVAPI headers

#include "navapi.h"


/////////////////////////////////////////////////////////////////////////////
// Test Parameters

#if defined(SYM_NLM)
#define TEST_DEFS               "SYS:\\DAVID\\"
#elif defined(SYM_DOS) || defined(SYM_WIN16)
#elif defined(SYM_WIN)
#define TEST_DEFS               "C:\\DEFS"
#elif defined(SYM_UNIX)
#define TEST_DEFS               "/tmp/defs"
#endif

#define TEST_PHYSICAL_DRIVE     0x80

#define TEST_LOGICAL_DRIVE      'C'

#if defined(SYM_NLM)
#define TEST_FILE               "SYS:\\DAVID\\dir2.com"
#else
#define TEST_FILE               "C:\\a\\cascade.com"
#endif

// Define ONE of the following:
//#define TEST_CALLBACKS_EMPTY
//#define TEST_CALLBACKS_WINDOWS
//#define TEST_CALLBACKS_QUAKE
#define TEST_CALLBACKS_NULL


/////////////////////////////////////////////////////////////////////////////
// Prototypes

// Test related functions

int Test(char *pszTestFile);
void TestMessage(LPSTR szMsg, LPSTR szTitle);

// NAVAPI file i/o callback functions

BOOL NAVCALLBACK TestFileOpen(LPVOID lpvFileInfo, DWORD dwOpenMode, LPVOID FAR *lpvHandle);
BOOL NAVCALLBACK TestFileClose(LPVOID lpvHandle);
UINT NAVCALLBACK TestFileRead(LPVOID lpvHandle, LPVOID lpvBuffer, UINT uBytesToRead);
UINT NAVCALLBACK TestFileWrite(LPVOID lpvHandle, LPVOID lpvBuffer, UINT uBytesToWrite);
BOOL NAVCALLBACK TestFileDelete(LPVOID lpvFileInfo);
BOOL NAVCALLBACK TestFileGetDateTime(LPVOID lpvHandle, UINT uType, LPWORD lpwDate, LPWORD lpwTime);
BOOL NAVCALLBACK TestFileSetDateTime(LPVOID lpvHandle, UINT uType, WORD wDate, WORD wTime);

// NAVAPI file i/o callback array delcaration

extern NAVFILEIO gstFileIOCallbacks;


/////////////////////////////////////////////////////////////////////////////
// WinMain()

#ifdef SYM_WIN16

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
    return Test(lpszCmdLine);
}

#endif


/////////////////////////////////////////////////////////////////////////////
// main()

#if defined(SYM_DOS) || defined(SYM_NLM) || defined(SYM_WIN32) || defined(SYM_UNIX)

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("Wrong number of arguments\n");
        return 1;
    }

    return Test(argv[1]);
}

#endif


/////////////////////////////////////////////////////////////////////////////
// Test()
//
// Test some NAVAPI calls

int Test(char *pszTestFile)
{
    HNAVENGINE hEngine = NULL;
    NAVVIRUSDBINFOEX dbinfo;
    HNAVVIRUS  hVirus = NULL;
    NAVSTATUS  status = NAV_OK;
    char szInfo[300] = { '\0' };
    DWORD dwInfoSize = 0;
    HNAVVIRUSDEF hDef = NULL;
    HNAVVIRUSDEFTABLE hDefTable = NULL;
    DWORD dwVirusCount = 0;
    BOOL        bInfectable = TRUE;
    char* pszTestFileExt = NULL;

    status = NAVGetNavapiVersionString(szInfo, sizeof(szInfo));
    if (status == NAV_OK)
        TestMessage(szInfo, "Version String");

#ifdef TEST_CALLBACKS_NULL
    hEngine = NAVEngineInit(TEST_DEFS, NULL, NULL, 2, 0, &status);
#else
    hEngine = NAVEngineInit(TEST_DEFS, NULL, &gstFileIOCallbacks, 2, 0, &status);
#endif
    if (status != NAV_OK || hEngine == NULL)
    {
        TestMessage("NAVEngineInit Failed", "Engine Test");
        return 0;
    }

#ifdef TEST_DEF_TABLE
    // The folling is an optimazation test. For Odyssey...
    {
        const DWORD             c_dwFooBytes = 302;
        auto  HNAVVIRUSDEFTABLE hTable       = NAVLoadVirusDefTable (hEngine);
        auto  DWORD             dwFooBytes   = c_dwFooBytes;
        auto  char              szFoo[256];
        auto  DWORD             dwNumDefs;
        auto  DWORD             dwDefIndex;

        if ( !hTable )
        {
            TestMessage ("NAVLoadVirusDefTable() failed!", "Table Test");
            return 0;
        }

        if ( NAV_OK != NAVGetVirusDefTableCount ( hTable, &dwNumDefs ) )
        {
            TestMessage ( "NAVGetVirusDefTableCount() failed!", "Table Test" );
            NAVReleaseVirusDefTable ( hTable );
            return 0;
        }

        FILE *outFile = fopen ("VirInfo.txt", "w");

        for ( dwDefIndex = 0; dwDefIndex < dwNumDefs; dwDefIndex++ )
        {
            dwFooBytes = c_dwFooBytes;
            NAVGetVirusDefTableInfo ( hTable, dwDefIndex, NAV_VI_VIRUS_NAME         , szFoo, &dwFooBytes );
            fprintf (outFile, "%20s", szFoo);

            dwFooBytes = c_dwFooBytes;
            NAVGetVirusDefTableInfo ( hTable, dwDefIndex, NAV_VI_VIRUS_ALIAS        , szFoo, &dwFooBytes );
            fprintf (outFile, " %s\n", szFoo);

            // Get various BOOLs...
            BOOL bWild           = NAVGetVirusDefTableInfo ( hTable, dwDefIndex, NAV_VI_BOOL_INFECT_WILD   , szFoo, &dwFooBytes );
            BOOL bInfectFiles    = NAVGetVirusDefTableInfo ( hTable, dwDefIndex, NAV_VI_BOOL_INFECT_FILES  , szFoo, &dwFooBytes );
            BOOL bInfectBoot     = NAVGetVirusDefTableInfo ( hTable, dwDefIndex, NAV_VI_BOOL_INFECT_MBR    , szFoo, &dwFooBytes );

            if ( !bInfectBoot )
            {
                bInfectBoot = NAVGetVirusDefTableInfo ( hTable, dwDefIndex, NAV_VI_BOOL_INFECT_FLOPPY_BOOT, szFoo, &dwFooBytes );
            }

            if ( !bInfectBoot )
            {
                bInfectBoot = NAVGetVirusDefTableInfo ( hTable, dwDefIndex, NAV_VI_BOOL_INFECT_HD_BOOT, szFoo, &dwFooBytes );
            }

            BOOL bStealth = NAVGetVirusDefTableInfo ( hTable, dwDefIndex, NAV_VI_BOOL_SIZE_STEALTH, szFoo, &dwFooBytes );

            if ( !bStealth )
            {
                bStealth    = NAVGetVirusDefTableInfo ( hTable, dwDefIndex, NAV_VI_BOOL_FULL_STEALTH  , szFoo, &dwFooBytes );
            }

            BOOL bPolymorphic    = NAVGetVirusDefTableInfo ( hTable, dwDefIndex, NAV_VI_BOOL_POLYMORPHIC   , szFoo, &dwFooBytes );
            BOOL bMultipartite   = NAVGetVirusDefTableInfo ( hTable, dwDefIndex, NAV_VI_BOOL_MULTIPART     , szFoo, &dwFooBytes );
            BOOL bMacro          = NAVGetVirusDefTableInfo ( hTable, dwDefIndex, NAV_VI_BOOL_MACRO_VIRUS   , szFoo, &dwFooBytes );
            BOOL bWindows        = NAVGetVirusDefTableInfo ( hTable, dwDefIndex, NAV_VI_BOOL_WINDOWS_VIRUS , szFoo, &dwFooBytes );
            BOOL bEncrypting     = NAVGetVirusDefTableInfo ( hTable, dwDefIndex, NAV_VI_BOOL_ENCRYPTING    , szFoo, &dwFooBytes );
            BOOL bAgent          = NAVGetVirusDefTableInfo ( hTable, dwDefIndex, NAV_VI_BOOL_AGENT_VIRUS   , szFoo, &dwFooBytes );
        }// end for ( dwDefIndex = 0, nCnt = 0; dwDefIndex < dwNumDefs; dwDefIndex++ )

        NAVReleaseVirusDefTable ( hTable );

        fclose (outFile);
    }
#endif // TEST_DEF_TABLE

#ifndef SYM_UNIX
    dbinfo.dwSize = sizeof (dbinfo);
    status = NAVGetVirusDBInfoEx(hEngine, &dbinfo);
    if (status == NAV_OK)
    {
        sprintf(szInfo,
            "%d/%d/%d v%ld - seq# %ld",
            (int) dbinfo.wYear,
            (int) dbinfo.wMonth,
            (int) dbinfo.wDay,
            dbinfo.dwVersion,
            dbinfo.dwSequence);
        TestMessage(szInfo, "DB Info Test");
    }

    status = NAVScanMemory(hEngine, &hVirus);
    if (status == NAV_OK && hVirus != NULL)
    {
        TestMessage("Virus Found!", "Memory Test");
        NAVFreeVirusHandle(hVirus);
    }


    /*status = NAVScanMasterBoot(hEngine, TEST_PHYSICAL_DRIVE, &hVirus);
    if (status == NAV_OK && hVirus != NULL)
    {
        TestMessage("Virus Found!", "Master Boot Test");

        status = NAVRepairMasterBoot(hEngine, TEST_PHYSICAL_DRIVE);
        if (status == NAV_OK)
            TestMessage("Virus Repaired!", "Master Boot Test");

        NAVFreeVirusHandle(hVirus);
    }


    status = NAVScanBoot(hEngine, TEST_LOGICAL_DRIVE, &hVirus);
    if (status == NAV_OK && hVirus != NULL)
    {
        TestMessage("Virus Found!", "Boot Test");

        status = NAVRepairBoot(hEngine, TEST_LOGICAL_DRIVE);
        if (status == NAV_OK)
            TestMessage("Virus Repaired!", "Boot Test");

        NAVFreeVirusHandle(hVirus);
    } */
#endif


    // Find the file extension.
    pszTestFileExt = strrchr(pszTestFile, '.');
    if (pszTestFileExt == NULL)
        pszTestFileExt = "dat";
    else
        pszTestFileExt++;

    status = NAVScanFile(hEngine, pszTestFile, pszTestFileExt, TRUE, &hVirus);

    if (status == NAV_OK && hVirus != NULL)
    {
        BOOL           bHeuristic;
        BOOL           bSubmitable;
        struct tm FAR *pstTm;
        time_t         TimeValue;

        TestMessage("Virus Found!", "File Test");

        dwInfoSize = sizeof(szInfo);
        status = NAVGetVirusInfo(hVirus, NAV_VI_VIRUS_NAME, szInfo, &dwInfoSize);
        TestMessage(szInfo, "File Test");

        bHeuristic  = NAVGetVirusInfo(hVirus, NAV_VI_BOOL_HEURISTIC,  NULL, 0);
        if (bHeuristic)
            TestMessage ("IsHeuristic", "Heuristic Test");

        bSubmitable = NAVGetVirusInfo(hVirus, NAV_VI_BOOL_SUBMITABLE, NULL, 0);
        if (!bSubmitable)
            TestMessage ("Not submitable", "Submit Test");

        dwInfoSize = sizeof(szInfo);
        status = NAVGetVirusInfo(hVirus, NAV_VI_DETECT_DATE_TIME, szInfo, &dwInfoSize);
        if (status == NAV_OK)
        {
            char *pszStr;
            TimeValue = atol (szInfo);
            pstTm = localtime (&TimeValue);
            pszStr = asctime (pstTm);
            TestMessage (pszStr, "Detect Date/Time");
        }

        status = NAVRepairFile(hEngine, pszTestFile, pszTestFileExt);
        if (status == NAV_OK)
            TestMessage("Virus Repaired!", "File Test");

        NAVFreeVirusHandle(hVirus);
    }


 /*   if (NAVGetVirusDefCount(hEngine, &dwVirusCount) == NAV_OK)
    {
        hDef = NAVLoadVirusDef(hEngine, dwVirusCount-1);
        if (hDef != NULL)
        {
            dwInfoSize = sizeof(szInfo);
            if (NAVGetVirusDefInfo(hDef, NAV_VI_VIRUS_NAME, szInfo, &dwInfoSize) == NAV_OK)
                TestMessage(szInfo, "Last Virus Info Test");
            NAVReleaseVirusDef(hDef);
        }
    }

    if (NAVGetVirusDefCount(hEngine, &dwVirusCount) == NAV_OK)
    {
        DWORD dwVirusMaxCount;
        DWORD dwVirusDetectCount;
        if (NAVGetVirusDefMaxCount (hEngine, &dwVirusMaxCount) == NAV_OK)
        {
            if (NAVGetVirusDefDetectCount (hEngine, &dwVirusDetectCount) == NAV_OK)
            {
                sprintf (szInfo, "Def Count: %ld, MaxCount: %ld, DetectCount: %ld.",
                         dwVirusCount, dwVirusMaxCount, dwVirusDetectCount);
                TestMessage(szInfo, "Def Count Test");
            }
        }
    }


    hDefTable = NAVLoadVirusDefTable(hEngine);
    if (hDefTable != NULL)
    {
        if (NAVGetVirusDefTableCount(hDefTable, &dwVirusCount) == NAV_OK)
        {
            sprintf(szInfo, "Virus Count is %lu", dwVirusCount);
            TestMessage(szInfo, "Virus Table Info Test");

            dwInfoSize = sizeof(szInfo);
            if (NAVGetVirusDefTableInfo(hDefTable, 7000, NAV_VI_VIRUS_NAME, szInfo, &dwInfoSize) == NAV_OK)
                TestMessage(szInfo, "First Virus Table Info Test");

            dwInfoSize = sizeof(szInfo);
            if (NAVGetVirusDefTableInfo(hDefTable, dwVirusCount-1, NAV_VI_VIRUS_NAME, szInfo, &dwInfoSize) == NAV_OK)
                TestMessage(szInfo, "Last Virus Table Info Test");
        }
        NAVReleaseVirusDefTable(hDefTable);
    }*/

#ifdef SYM_WIN32

    status = NAVIsInfectableFile(hEngine, pszTestFile, pszTestFileExt, TRUE, &bInfectable);

    if (status == NAV_OK)
    {
        if (bInfectable)
            sprintf(szInfo, "%s is infectable.", pszTestFile);
        else
            sprintf(szInfo, "%s is not infectable.", pszTestFile);
    }
    else
        sprintf(szInfo, "NAVIsInfectableFile() failed, status %x", status);

    TestMessage(szInfo, "Is Infectable Test");

#endif  // #ifdef SYM_WIN32

    NAVEngineClose(hEngine);

    return 0;
}


/////////////////////////////////////////////////////////////////////////////
// TestMessage()
//
// Display an output message

void TestMessage(LPSTR szMsg, LPSTR szTitle)
{
#if defined(SYM_DOS) || defined (SYM_WIN32) || defined (SYM_UNIX)
    printf("%s: %s\n", szTitle, szMsg);
#elif defined(SYM_NLM)
    ConsolePrintf("%s: %s\n", szTitle, szMsg);
#else
    MessageBox(NULL, szMsg, szTitle, MB_OK);
#endif
}


/////////////////////////////////////////////////////////////////////////////
// NAVAPI File I/O Callbacks, Empty Implementation

#if defined(TEST_CALLBACKS_EMPTY)

BOOL NAVCALLBACK TestFileOpen(LPVOID lpvFileInfo, DWORD dwOpenMode, LPVOID FAR *lplpvHandle)
{
    return FALSE;
}

BOOL NAVCALLBACK TestFileClose(LPVOID lpvHandle)
{
    return FALSE;
}

DWORD NAVCALLBACK TestFileSeek(LPVOID lpvHandle, LONG lOffset, int nFrom)
{
    return 0;
}

UINT NAVCALLBACK TestFileRead(LPVOID lpvHandle, LPVOID lpvBuffer, UINT uBytesToRead)
{
    return -1;
}

UINT NAVCALLBACK TestFileWrite(LPVOID lpvHandle, LPVOID lpvBuffer, UINT uBytesToWrite)
{
    return -1;
}

BOOL NAVCALLBACK TestFileDelete(LPVOID lpvFileInfo)
{
    return FALSE;
}

BOOL NAVCALLBACK TestFileGetDateTime(LPVOID lpvHandle, UINT uType, LPWORD lpwDate, LPWORD lpwTime)
{
    return FALSE;
}

BOOL NAVCALLBACK TestFileSetDateTime(LPVOID lpvHandle, UINT uType, WORD wDate, WORD wTime)
{
    return FALSE;
}

#endif


/////////////////////////////////////////////////////////////////////////////
// NAVAPI File I/O Callbacks, Windows Implementation

#if defined(TEST_CALLBACKS_WINDOWS)

BOOL NAVCALLBACK TestFileOpen(LPVOID lpvFileInfo, DWORD dwOpenMode, LPVOID FAR *lplpvHandle)
{
    OFSTRUCT of;
    HFILE    hFile = OpenFile((LPCSTR)lpvFileInfo, &of, (UINT)dwOpenMode);

    if (hFile == HFILE_ERROR)
        return FALSE;

    *lplpvHandle = (LPVOID)hFile;
    return TRUE;
}

BOOL NAVCALLBACK TestFileClose(LPVOID lpvHandle)
{
    return (_lclose((HFILE)lpvHandle) != HFILE_ERROR);
}

DWORD NAVCALLBACK TestFileSeek(LPVOID lpvHandle, LONG lOffset, int nFrom)
{
    return SetFilePointer((HANDLE)lpvHandle, lOffset, 0, nFrom);
}

UINT NAVCALLBACK TestFileRead(LPVOID lpvHandle, LPVOID lpvBuffer, UINT uBytesToRead)
{
    return _lread((HFILE)lpvHandle, lpvBuffer, uBytesToRead);
}

UINT NAVCALLBACK TestFileWrite(LPVOID lpvHandle, LPVOID lpvBuffer, UINT uBytesToWrite)
{
    return _lwrite((HFILE)lpvHandle, (LPCSTR)lpvBuffer, uBytesToWrite);
}

BOOL NAVCALLBACK TestFileDelete(LPVOID lpvFileInfo)
{
    return DeleteFile((LPCSTR)lpvFileInfo);
}

BOOL NAVCALLBACK TestFileGetDateTime(LPVOID lpvHandle, UINT uType, LPWORD lpwDate, LPWORD lpwTime)
{
    FILETIME ftCreation, ftLastAccess, ftLastWrite;
    if (GetFileTime((HANDLE)lpvHandle, &ftCreation, &ftLastAccess, &ftLastWrite) == FALSE)
        return FALSE;
    FILETIME ft;
    switch (uType)
    {
    case NAV_TIME_LASTWRITE:
        ft = ftLastWrite;
        break;
    case NAV_TIME_LASTACCESS:
        ft = ftLastAccess;
        break;
    case NAV_TIME_CREATION:
        ft = ftCreation;
        break;
    default:
        return FALSE;
    }
    return FileTimeToDosDateTime(&ft, lpwDate, lpwTime);
}

BOOL NAVCALLBACK TestFileSetDateTime(LPVOID lpvHandle, UINT uType, WORD wDate, WORD wTime)
{
    FILETIME ft;
    if (DosDateTimeToFileTime(wDate, wTime, &ft) == FALSE)
        return FALSE;
    switch (uType)
    {
    case NAV_TIME_LASTWRITE:
        return SetFileTime((HANDLE)lpvHandle, NULL, NULL, &ft);
    case NAV_TIME_LASTACCESS:
        return SetFileTime((HANDLE)lpvHandle, NULL, &ft, NULL);
    case NAV_TIME_CREATION:
        return SetFileTime((HANDLE)lpvHandle, &ft, NULL, NULL);
    default:
        return FALSE;
    }
}

#endif


/////////////////////////////////////////////////////////////////////////////
// NAVAPI File I/O Callbacks, Quake Implementation

#if defined(TEST_CALLBACKS_QUAKE)

BOOL NAVCALLBACK TestFileOpen(LPVOID lpvFileInfo, DWORD dwOpenMode, LPVOID FAR *lplpvHandle)
{
    HFILE dwHandle;

    dwHandle = FileOpen((LPCSTR)lpvFileInfo, (UINT)dwOpenMode);
    if (dwHandle == HFILE_ERROR)
        return FALSE;

#if defined(SYM_NLM)
//  ConsolePrintf("TestFileOpenTest(%s) handle %ld\n", lpvFileInfo, dwHandle);
#endif

    *lplpvHandle = (LPVOID)dwHandle;
    return TRUE;
}

BOOL NAVCALLBACK TestFileClose(LPVOID lpvHandle)
{
    // Return TRUE on success
    return !FileClose((HFILE)lpvHandle);
}

DWORD NAVCALLBACK TestFileSeek(LPVOID lpvHandle, LONG lOffset, int nFrom)
{
    return FileSeek((HFILE)lpvHandle, lOffset, nFrom);
}

UINT NAVCALLBACK TestFileRead(LPVOID lpvHandle, LPVOID lpvBuffer, UINT uBytesToRead)
{
#if defined(SYM_NLM)
    UINT uBytes;
    ThreadSwitch();
    uBytes = read((int)lpvHandle, lpvBuffer, (LONG)uBytesToRead);
//  ConsolePrintf("TestFileRead(%ld) returns %ld\n", dwHandle, uBytes);
    return uBytes;
#else
    return FileRead((HFILE)lpvHandle, lpvBuffer, uBytesToRead);
#endif
}

UINT NAVCALLBACK TestFileWrite(LPVOID lpvHandle, LPVOID lpvBuffer, UINT uBytesToWrite)
{
#if defined(SYM_NLM)
    ThreadSwitch();
    return write((int)lpvHandle, lpvBuffer, (LONG)uBytesToWrite);
#else
    return FileWrite((HFILE)lpvHandle, lpvBuffer, uBytesToWrite);
#endif
}

BOOL NAVCALLBACK TestFileDelete(LPVOID lpvFileInfo)
{
    return FileDelete((LPCSTR)lpvFileInfo);
}

BOOL NAVCALLBACK TestFileGetDateTime(LPVOID lpvHandle, UINT uType, LPWORD lpwDate, LPWORD lpwTime)
{
    FILETIME ft;
    FileGetTime((HFILE)lpvHandle, uType, &ft);
    FileTimeToDosTime(&ft, lpwDate, lpwTime);
    return TRUE;
}

BOOL NAVCALLBACK TestFileSetDateTime(LPVOID lpvHandle, UINT uType, WORD wDate, WORD wTime)
{
    FILETIME ft;
    if (DosTimeToFileTime(wDate, wTime, &ft) == FALSE)
        return FALSE;
    if (FileSetTime((HFILE)lpvHandle, uType, &ft) != NOERR)
        return FALSE;
    return TRUE;
}

#endif


/////////////////////////////////////////////////////////////////////////////
// NAVAPI File I/O Callbacks Array

#ifndef TEST_CALLBACKS_NULL
NAVFILEIO gstFileIOCallbacks =
{
    TestFileOpen,
    TestFileClose,
    TestFileSeek,
    TestFileRead,
    TestFileWrite,
    TestFileDelete,
    TestFileGetDateTime,
    TestFileSetDateTime,
};
#endif // ifndef TEST_CALLBACKS_NULL
