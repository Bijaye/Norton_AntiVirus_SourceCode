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

#include <stdio.h>
#if !defined(SYM_UNIX)
#include <io.h>
#endif
#if defined(SYM_NLM)
#include <process.h>
#include <conio.h>
#endif

// Symantec core library headers

#include "platform.h"
#include "file.h"

// NAVAPI headers

#include "navapi.h"


/////////////////////////////////////////////////////////////////////////////
// Test Parameters

#define TEST_PHYSICAL_DRIVE     0x80

#define TEST_LOGICAL_DRIVE      'C'

#if defined(SYM_WIN)
#define TEST_CALLBACKS_WINDOWS
#else 
#define TEST_CALLBACKS_QUAKE
#endif


/////////////////////////////////////////////////////////////////////////////
// Options

BOOL gbHelp = FALSE;        // Show help (-h, -?)
BOOL gbVersion = FALSE;     // Show API version string (-v)
BOOL gbInfo = FALSE;        // Show virus database info (-i)
BOOL gbDefInfo = FALSE;     // Show info for last virus def (-l)
BOOL gbTableInfo = FALSE;   // Show info from virus table (-t) 
BOOL gbMemory = FALSE;      // Scan memory (-m)
BOOL gbMasterBoot = FALSE;  // Scan master boot record on disk 1 (-p)
BOOL gbBoot = FALSE;        // Scan boot record on C: (-b)
BOOL gbRepair = FALSE;      // Repair viruses (-r)
BOOL gbDefDir = FALSE;      // Set virus database directory (-d defsdir)
char gszDefDir[256];


/////////////////////////////////////////////////////////////////////////////
// Prototypes

// Test related functions

int main2(int argc, char* argv[]);
int Test(int argc, char* argv[]);
void TestMessage(LPSTR szMsg, LPSTR szTitle);

// NAVAPI file i/o callback functions

DWORD NAVCALLBACK TestFileOpen(LPVOID lpvFileInfo, DWORD dwOpenMode);
BOOL NAVCALLBACK TestFileClose(DWORD dwHandle);
UINT NAVCALLBACK TestFileRead(DWORD dwHandle, LPVOID lpvBuffer, UINT uBytesToRead);
UINT NAVCALLBACK TestFileWrite(DWORD dwHandle, LPVOID lpvBuffer, UINT uBytesToWrite);
BOOL NAVCALLBACK TestFileDelete(LPVOID lpvFileInfo);
BOOL NAVCALLBACK TestFileGetDateTime(DWORD dwHandle, UINT uType, LPWORD lpwDate, LPWORD lpwTime);
BOOL NAVCALLBACK TestFileSetDateTime(DWORD dwHandle, UINT uType, WORD wDate, WORD wTime);

// NAVAPI file i/o callback array delcaration

extern NAVFILEIO gstFileIOCallbacks;


/////////////////////////////////////////////////////////////////////////////
// WinMain()

#if defined(SYM_WIN)

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
    char* argv[100];
    int argc = 0; 
    char* token;

    argv[argc++] = "apitest";

    token = strtok(lpszCmdLine, " ");
    while (argc < 100 && token != NULL)
    {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }

    return main2(argc, argv);
}

#endif


/////////////////////////////////////////////////////////////////////////////
// main()

#if defined(SYM_DOS) || defined(SYM_UNIX)

int main(int argc, char* argv[])
{
    return main2(argc, argv);
}

#elif defined(SYM_NLM)

int main(int argc, char* argv[])
{
    char* argv[10];
    int argc; 

    argv[0] = "test";
    argv[1] = "-d";
    argv[2] = "SYS:\\DAVID\\";
    argv[3] = "-viltr";
    argv[4] = "SYS:\\DAVID\\dir2.com"
    argc = 5;

    return main2(argc, argv);
}

#endif


/////////////////////////////////////////////////////////////////////////////
// main2()

int main2(int argc, char* argv[])
{
    int c;

    if (argc == 1)
        gbHelp = TRUE;

    // While there are aguments beginning with '-'...
    while (--argc > 0 && (*++argv)[0] == '-')
    {
        // If the argument is -d followed by a defsdir name...
        if (argv[0][1] == 'd' && argc > 1)
        {
            // Skip past -d
            --argc;
            ++argv;
            // Copy the name argument
            strcpy(gszDefDir, argv[0]);
            gbDefDir = TRUE;
            continue;
        }

        // While there are letters after the '-'...
        while ((c = *++(argv[0])) != 0)
        {
            switch (c)
            {
                case 'h':
                case '?':
                    gbHelp = TRUE;
                    break;
                case 'v':
                    gbVersion = TRUE;
                    break;
                case 'i':
                    gbInfo = TRUE;
                    break;
                case 'l':
                    gbDefInfo = TRUE;
                    break;
                case 't':
                    gbTableInfo = TRUE;
                    break;
                case 'm':
                    gbMemory = TRUE;
                    break;
                case 'p':
                    gbMasterBoot = TRUE;
                    break;
                case 'b':
                    gbBoot = TRUE;
                    break;
                case 'r':
                    gbRepair = TRUE;
                    break;
                default:
                    printf("illegal option %c\n", c);
                    gbHelp = TRUE;
                    break;
            }
        }
    }

    if (!gbDefDir)
    {
        strcpy(gszDefDir, ".");
        gbDefDir = TRUE;
    }

    if (gbHelp)
    {
        char* szMsg = 
        "Usage: apitest [-d dir] [-hviltmpbr] [file...]\n"
        "-d Set virus database directory\n"
        "-h Show help\n"
        "-v Show API version string\n"
        "-i Show virus database info\n"
        "-l Show info for last virus def\n"
        "-t Show info from virus table\n"
        "-m Scan memory\n"
        "-p Scan master boot record on disk 1\n"
        "-b Scan boot record on C:\n"
        "-r Repair viruses\n";
#if defined(SYM_WIN)
        MessageBox(NULL, szMsg, "apitest", MB_OK);
#else
        printf(szMsg);
#endif
        return 0;
    }

    return Test(argc, argv);
}


/////////////////////////////////////////////////////////////////////////////
// Test()
//
// Test some NAVAPI calls

int Test(int argc, char** argv)
{
    HNAVENGINE hEngine = NULL;
    VIRUSDBINFO dbinfo;
    HNAVVIRUS  hVirus = NULL;
    NAVSTATUS  status = NAV_OK;
    char szInfo[300] = { '\0' };
    DWORD dwInfoSize = 0;
    HNAVVIRUSDEF hDef = NULL;
    HNAVVIRUSDEFTABLE hDefTable = NULL;
    DWORD dwVirusCount = 0;


    if (gbVersion)
    {
        status = NAVGetNavapiVersionString(szInfo, sizeof(szInfo));
        if (status == NAV_OK)
            TestMessage(szInfo, "Version String");
    }

    hEngine = NAVEngineInit(gszDefDir, NULL, &gstFileIOCallbacks, 2, 0, &status);
    if (status != NAV_OK || hEngine == NULL)
    {
        TestMessage("NAVEngineInit Failed", "Engine Test");
        return 0;
    }

    if (gbInfo)
    {
        status = NAVGetVirusDBInfo(hEngine, &dbinfo);
        if (status == NAV_OK)
        {
            sprintf(szInfo, 
                "%d/%d/%d %ld", 
                (int) dbinfo.wYear, 
                (int) dbinfo.wMonth, 
                (int) dbinfo.wDay, 
                dbinfo.dwVersion);
            TestMessage(szInfo, "DB Info Test");
        }
    }

    if (gbDefInfo)
    {
        if (NAVGetVirusDefCount(hEngine, &dwVirusCount) == NAV_OK)
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
    }

    if (gbTableInfo)
    {
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
        }
    }

    if (gbMemory)
    {
        status = NAVScanMemory(hEngine, &hVirus);
        if (status == NAV_OK && hVirus != NULL)
        {
            TestMessage("Virus Found!", "Memory Test");
            NAVFreeVirusHandle(hVirus);
        }
    }

    if (gbMasterBoot)
    {
        status = NAVScanMasterBoot(hEngine, TEST_PHYSICAL_DRIVE, &hVirus);
        if (status == NAV_OK && hVirus != NULL)
        {
            TestMessage("Virus Found!", "Master Boot Test");

            if (gbRepair)
            {
                status = NAVRepairMasterBoot(hEngine, TEST_PHYSICAL_DRIVE);
                if (status == NAV_OK)
                    TestMessage("Virus Repaired!", "Master Boot Test");
            }

            NAVFreeVirusHandle(hVirus);
        }
    }

    if (gbBoot)
    {
        status = NAVScanBoot(hEngine, TEST_LOGICAL_DRIVE, &hVirus);
        if (status == NAV_OK && hVirus != NULL)
        {
            TestMessage("Virus Found!", "Boot Test");

            if (gbRepair)
            {
                status = NAVRepairBoot(hEngine, TEST_LOGICAL_DRIVE);
                if (status == NAV_OK)
                    TestMessage("Virus Repaired!", "Boot Test");

                NAVFreeVirusHandle(hVirus);
            }
        }
    }

    while (argc-- > 0)
    {
        char* pszFile = *argv;
        char* pszExtension = strrchr(pszFile, '.');
        argv++;

        sprintf(szInfo, "Scanning %s...", pszFile);
        TestMessage(szInfo, "File Test");

        status = NAVScanFile(hEngine, pszFile, pszExtension, TRUE, &hVirus);
        if (status == NAV_OK && hVirus != NULL)
        {
            TestMessage("Virus Found!", "File Test");

            dwInfoSize = sizeof(szInfo);
            status = NAVGetVirusInfo(hVirus, NAV_VI_VIRUS_NAME, szInfo, &dwInfoSize);
            TestMessage(szInfo, "File Test");

            if (gbRepair)
            {
                status = NAVRepairFile(hEngine, pszFile, pszExtension);
                if (status == NAV_OK)
                    TestMessage("Virus Repaired!", "File Test");
            }

            NAVFreeVirusHandle(hVirus);
        }
    }

    NAVEngineClose(hEngine);

    return 0;
}


/////////////////////////////////////////////////////////////////////////////
// TestMessage()
//
// Display an output message

void TestMessage(LPSTR szMsg, LPSTR szTitle)
{
#if defined(SYM_DOS) || defined(SYM_UNIX)
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

DWORD NAVCALLBACK TestFileOpen(LPVOID lpvFileInfo, DWORD dwOpenMode)
{
    return -1;
} 

BOOL NAVCALLBACK TestFileClose(DWORD dwHandle)
{
    return FALSE;
}

DWORD NAVCALLBACK TestFileSeek(DWORD dwHandle, LONG lOffset, int nFrom)
{
    return 0;
}

UINT NAVCALLBACK TestFileRead(DWORD dwHandle, LPVOID lpvBuffer, UINT uBytesToRead)
{
    return -1;
}

UINT NAVCALLBACK TestFileWrite(DWORD dwHandle, LPVOID lpvBuffer, UINT uBytesToWrite)
{
    return -1;
}

BOOL NAVCALLBACK TestFileDelete(LPVOID lpvFileInfo)
{
    return FALSE;
}

BOOL NAVCALLBACK TestFileGetDateTime(DWORD dwHandle, UINT uType, LPWORD lpwDate, LPWORD lpwTime)
{
    return FALSE;
}

BOOL NAVCALLBACK TestFileSetDateTime(DWORD dwHandle, UINT uType, WORD wDate, WORD wTime)
{
    return FALSE;
}

#endif


/////////////////////////////////////////////////////////////////////////////
// NAVAPI File I/O Callbacks, Windows Implementation

#if defined(TEST_CALLBACKS_WINDOWS)

DWORD NAVCALLBACK TestFileOpen(LPVOID lpvFileInfo, DWORD dwOpenMode)
{
    OFSTRUCT of;
    return OpenFile((LPCSTR)lpvFileInfo, &of, (UINT)dwOpenMode);
} 
 
BOOL NAVCALLBACK TestFileClose(DWORD dwHandle)
{
    return (_lclose((HFILE)dwHandle) != HFILE_ERROR);
}

DWORD NAVCALLBACK TestFileSeek(DWORD dwHandle, LONG lOffset, int nFrom)
{
    return SetFilePointer((HANDLE)dwHandle, lOffset, 0, nFrom);
}

UINT NAVCALLBACK TestFileRead(DWORD dwHandle, LPVOID lpvBuffer, UINT uBytesToRead)
{
    return _lread((HFILE)dwHandle, lpvBuffer, uBytesToRead);
}

UINT NAVCALLBACK TestFileWrite(DWORD dwHandle, LPVOID lpvBuffer, UINT uBytesToWrite)
{
    return _lwrite((HFILE)dwHandle, (LPCSTR)lpvBuffer, uBytesToWrite);
}

BOOL NAVCALLBACK TestFileDelete(LPVOID lpvFileInfo)
{
    return DeleteFile((LPCSTR)lpvFileInfo);
}

BOOL NAVCALLBACK TestFileGetDateTime(DWORD dwHandle, UINT uType, LPWORD lpwDate, LPWORD lpwTime)
{
    FILETIME ftCreation, ftLastAccess, ftLastWrite;
    if (GetFileTime((HANDLE)dwHandle, &ftCreation, &ftLastAccess, &ftLastWrite) == FALSE)
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

BOOL NAVCALLBACK TestFileSetDateTime(DWORD dwHandle, UINT uType, WORD wDate, WORD wTime)
{
    FILETIME ft;
    if (DosDateTimeToFileTime(wDate, wTime, &ft) == FALSE)
        return FALSE;
    switch (uType)
    {
    case NAV_TIME_LASTWRITE:
        return SetFileTime((HANDLE)dwHandle, NULL, NULL, &ft);
    case NAV_TIME_LASTACCESS:
        return SetFileTime((HANDLE)dwHandle, NULL, &ft, NULL);
    case NAV_TIME_CREATION:
        return SetFileTime((HANDLE)dwHandle, &ft, NULL, NULL);
    default:
        return FALSE;
    }
}

#endif


/////////////////////////////////////////////////////////////////////////////
// NAVAPI File I/O Callbacks, Quake Implementation

#if defined(TEST_CALLBACKS_QUAKE)

DWORD NAVCALLBACK TestFileOpen(LPVOID lpvFileInfo, DWORD dwOpenMode)
{
    DWORD dwHandle;

    dwHandle = (DWORD) FileOpen((LPCSTR)lpvFileInfo, (UINT)dwOpenMode);

#if defined(SYM_NLM)
//  ConsolePrintf("TestFileOpenTest(%s) returns %ld\n", lpvFileInfo, dwHandle);
#endif

    return dwHandle;
} 

BOOL NAVCALLBACK TestFileClose(DWORD dwHandle)
{
    // Return TRUE on success
    return !FileClose((HFILE)dwHandle);
}

DWORD NAVCALLBACK TestFileSeek(DWORD dwHandle, LONG lOffset, int nFrom)
{
    return FileSeek((HFILE)dwHandle, lOffset, nFrom);
}

UINT NAVCALLBACK TestFileRead(DWORD dwHandle, LPVOID lpvBuffer, UINT uBytesToRead)
{
#if defined(SYM_NLM)
    UINT uBytes;
    ThreadSwitch();
    uBytes = read((int)dwHandle, lpvBuffer, (LONG)uBytesToRead);
//  ConsolePrintf("TestFileRead(%ld) returns %ld\n", dwHandle, uBytes);
    return uBytes;
#else
    return FileRead((HFILE)dwHandle, lpvBuffer, uBytesToRead);
#endif
}

UINT NAVCALLBACK TestFileWrite(DWORD dwHandle, LPVOID lpvBuffer, UINT uBytesToWrite)
{
#if defined(SYM_NLM)
    ThreadSwitch();
    return write((int)dwHandle, lpvBuffer, (LONG)uBytesToWrite);
#else
    return FileWrite((HFILE)dwHandle, lpvBuffer, uBytesToWrite);
#endif
}

BOOL NAVCALLBACK TestFileDelete(LPVOID lpvFileInfo)
{
    return FileDelete((LPCSTR)lpvFileInfo);
}

BOOL NAVCALLBACK TestFileGetDateTime(DWORD dwHandle, UINT uType, LPWORD lpwDate, LPWORD lpwTime)
{
    FILETIME ft;
    FileGetTime((HFILE)dwHandle, uType, &ft);
    FileTimeToDosTime(&ft, lpwDate, lpwTime);
    return TRUE;
}

BOOL NAVCALLBACK TestFileSetDateTime(DWORD dwHandle, UINT uType, WORD wDate, WORD wTime)
{
    FILETIME ft;
    if (DosTimeToFileTime(wDate, wTime, &ft) == FALSE)
        return FALSE;
    if (FileSetTime((HFILE)dwHandle, uType, &ft) != NOERR)
        return FALSE;
    return TRUE;
}

#endif


/////////////////////////////////////////////////////////////////////////////
// NAVAPI File I/O Callbacks Array

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


