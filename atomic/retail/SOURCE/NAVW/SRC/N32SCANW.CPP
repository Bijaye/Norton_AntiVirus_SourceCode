// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/navw/VCS/n32scanw.cpv   1.0   06 Feb 1997 21:01:24   RFULLER  $
//
// Description:
//  N32SCANW.EXE -- Simple exe which maps the old
//  Norton AntiVirus Scanner for NT name with the new one. This app is for
//  keeping the link with rest of the Norton applications which are still
//  calling n32scanw instead the new name navwnt.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/navw/VCS/n32scanw.cpv  $
// 
//    Rev 1.0   06 Feb 1997 21:01:24   RFULLER
// Initial revision
// 
//    Rev 1.4   23 Sep 1996 10:40:54   DALLEE
// Ported missed change from Gryphon - if not compiling UNICODE, 
// MultiByteToWide call changes to simple STRCPY.
// 
//    Rev 1.3   12 Sep 1996 16:40:10   PVENKAT
// Removed #error on non-UNICODE compilation.
// 
//    Rev 1.2   06 Sep 1996 15:21:32   RPULINT
// MOved code over from gryphon: Added check to see if running under win95
// 
//    Rev 1.1   21 Aug 1996 15:13:18   DALLEE
// Added header and guarantee UNICODE is defined.
//************************************************************************

#include "platform.h"
#include <malloc.h>
#include <fcntl.h>
#include "navwstb.h"                    // NAV string table entries
#include "xapi.h"

void GetPath(TCHAR* szTempW, int size)
{
    HKEY hKey= 0;

    MEMSET (szTempW, 0, size);

    if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Symantec\\InstalledApps"), 0, KEY_READ, &hKey))
    {
        if (ERROR_SUCCESS == RegQueryValueEx(hKey, TEXT("NAVNT"), 0, 0, (LPBYTE)szTempW, (LPDWORD)&size))
        {
            // Get Unicode string for command line.
            STRCAT(szTempW, TEXT("\\navwnt.exe"));

            RegCloseKey(hKey);
        }
    }
}

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    DWORD dwExitCode = 0;

    SECURITY_DESCRIPTOR  sdSec;

#ifdef NAVWNT
    // if we're building for NT but executing in W95, exit gracefully
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_95)
    {
        TCHAR   szErrorStarting[500];
        TCHAR   szNotFor95[2000];

        LoadString(hInstance,
                        IDS_ERROR_STARTING,
                        szErrorStarting,
                        sizeof(szErrorStarting) / sizeof (szErrorStarting[0]));

        LoadString(hInstance,
                        IDS_NOTFORWIN95,
                        szNotFor95,
                        sizeof(szNotFor95) / sizeof (szNotFor95[0]));
        MessageBox(NULL, szNotFor95, szErrorStarting, MB_OK | MB_ICONEXCLAMATION);

        return ( dwExitCode );
    }
#endif

    if (InitializeSecurityDescriptor (&sdSec, SECURITY_DESCRIPTOR_REVISION))
    {
        if (SetSecurityDescriptorDacl (&sdSec, TRUE, (PACL) NULL, FALSE))
        {
            SECURITY_ATTRIBUTES  saThread, saProcess;
            PROCESS_INFORMATION  ProcessInformation;
            STARTUPINFO          si;
            TCHAR szTempW[1024];

            // Only if we get the default desktop name do the following
            MEMSET (&si,        0, sizeof (si));
            MEMSET (&saProcess, 0, sizeof (saProcess));
            MEMSET (&saThread,  0, sizeof (saThread));

            // Set the startup Info.
            si.cb = sizeof (si);
            si.lpDesktop = TEXT("WinSta0\\Default");
            si.dwFlags = 0;
            si.wShowWindow = SW_SHOWNORMAL;

            saProcess.nLength = sizeof (saProcess);
            saProcess.lpSecurityDescriptor = &sdSec;
            saProcess.bInheritHandle = FALSE;

            saThread.nLength = sizeof (saThread);
            saThread.lpSecurityDescriptor = &sdSec;
            saThread.bInheritHandle = FALSE;

            GetPath(szTempW, sizeof(szTempW) / sizeof(szTempW[0]));

            TCHAR szCmdL[50];
#ifdef UNICODE
            //convert the comand line to wide carecters
            MultiByteToWideChar (
                        CP_ACP,
                        MB_PRECOMPOSED,
                        lpCmdLine,
                        -1,
                        (LPWSTR)szCmdL,
                        sizeof(szCmdL) / sizeof(szCmdL[0]));
#else
            STRCPY(szCmdL, lpCmdLine);
#endif

            STRCAT (szTempW, TEXT(" "));
            STRCAT (szTempW, szCmdL);

            if (CreateProcess ( NULL,
                                szTempW,
                                &saProcess,
                                &saThread,
                                FALSE,
                                HIGH_PRIORITY_CLASS | CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_PROCESS_GROUP,
                                NULL,
                                NULL,
                                &si,
                                &ProcessInformation))
            {
                HANDLE hProcess = ProcessInformation.hProcess;

                if (WaitForSingleObject(hProcess, INFINITE) != 0xFFFFFFFF)
                {
                        // The process terminated.
                        GetExitCodeProcess(hProcess, &dwExitCode);
                }

                // Close our handles to the process & its primary thread.
                CloseHandle(ProcessInformation.hThread);
                CloseHandle(hProcess);
            }
        }
    }
    return (dwExitCode);
}
