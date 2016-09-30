// Copyright 1992-1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVRUNR/VCS/navrunr.cpv   1.8   29 Aug 1997 14:16:18   jtaylor  $
//
// Description:
//      This file parses the cmd line to a file names and runs them in sync
//
// Contains (as exports):
//
// See Also:

//************************************************************************
// $Log:   S:/NAVRUNR/VCS/navrunr.cpv  $
// 
//    Rev 1.8   29 Aug 1997 14:16:18   jtaylor
// -- Made the error messagebox not popup if the error code was ERROR_SUCCESS
// -- Put the flags that Jim just removed back into the code.
//
//    Rev 1.7   29 Aug 1997 11:28:50   JBRENNA
// Put in a fix that will cause us to stop getting the "Success" messagebox.
// Do not understand why we were getting the messagebox. But this seems to
// fix it.
//
//    Rev 1.6   26 Aug 1997 09:48:50   JBRENNA
// Ops. dwNumPathChars calculated to always be 1. Second sizeof needed the [0].
//
//    Rev 1.5   31 Jul 1997 18:00:00   JBRENNA
// Chnage OverrideEnvPath to pre-append the NAVW path. Previously, we would
// just completely overwrite the PATH env var with NAVW's path. This would
// cause problems with NavLu32. NavLu32 needs C:\Windows in the path.
//
//    Rev 1.4   01 Jul 1997 10:03:18   JBRENNA
// 1. Added a /NOWAIT switch. When set, this exe will launch the application
//    and continue. It does not wait for the application to complete.
// 2. Added the ability for this executable to launch itself.
// 3. Changed the command line processing to support ( ) as the command
//    delineators. This allows recursive command lines.
//    Ex. NavRunr.EXE (/nowait NavRunr.EXE (NavLu32.exe) (Nresq32.exe))
//************************************************************************


#include "platform.h"
#include <winbase.h>
#include <string.h>
#include <tchar.h>

#if defined (WIN32)
    #define IS_WIN32 TRUE
#else
    #define IS_WIN32 FALSE
#endif
#define IS_NT      IS_WIN32 && (BOOL)(GetVersion() < 0x80000000)


const char    g_chCmdBegin = '(';
const char    g_chCmdEnd   = ')';


static void OverrideEnvPath(void)
{
    auto  HKEY   hKey           = 0;
    auto  LPTSTR pszValue       = _T("Path");
    auto  LPTSTR pszKey;
    const DWORD  dwPathSize     = 512;
    auto  DWORD  dwPathSizeTmp  = dwPathSize;
    auto  TCHAR  szNavwPath[dwPathSize];
    auto  TCHAR  szOrigPath[dwPathSize];
    auto  DWORD  dwNumPathChars = sizeof (szNavwPath) / sizeof (szNavwPath[0]);

    if (IS_NT)
        pszKey = _T("Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\NAVWNT.EXE");
    else
        pszKey = _T("Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\NAVW32.EXE");

    if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, pszKey, 0, KEY_READ, &hKey))
    {
        if (ERROR_SUCCESS == RegQueryValueEx(hKey, pszValue, 0, 0, (LPBYTE)szNavwPath, &dwPathSizeTmp))
        {
            if (0 == GetEnvironmentVariable (pszValue, szOrigPath, dwNumPathChars))
                *szOrigPath = '\0';

            // Pre-append the NAVW path to our existing path.
            STRCAT (szNavwPath, ";");
            STRCAT (szNavwPath, szOrigPath);

            // Override the PATH environmental variable with the
            // path from the registry.
            SetEnvironmentVariable (pszValue, szNavwPath);
        }
        RegCloseKey(hKey);
    }
}

static int Execute(LPSTR lpCmd, BOOL bWait)
{
    DWORD dwExitCode = 0;
    SECURITY_ATTRIBUTES  saThread, saProcess;
    PROCESS_INFORMATION  ProcessInformation;
    STARTUPINFO          si;

    // Only if we get the default desktop name do the following
    memset (&si,        0, sizeof (si));
    memset (&saProcess, 0, sizeof (saProcess));
    memset (&saThread,  0, sizeof (saThread));

    if (IS_NT)
    {
        SECURITY_DESCRIPTOR  sdSec;

        InitializeSecurityDescriptor (&sdSec, SECURITY_DESCRIPTOR_REVISION);
        SetSecurityDescriptorDacl (&sdSec, TRUE, (PACL) NULL, FALSE);

        si.lpDesktop = TEXT("WinSta0\\Default");
        saThread.lpSecurityDescriptor  = &sdSec;
        saProcess.lpSecurityDescriptor = &sdSec;
    }
    else
    {
        si.lpDesktop = NULL;
        saThread.lpSecurityDescriptor  = NULL;
        saProcess.lpSecurityDescriptor = NULL;
    }

    // Set the startup Info.
    si.cb = sizeof (si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOWNORMAL;

    saProcess.nLength = sizeof (saProcess);
    saProcess.bInheritHandle = TRUE;

    saThread.nLength = sizeof (saThread);
    saThread.bInheritHandle = TRUE;

    //Override the Path environment setting
    OverrideEnvPath();

    dwExitCode = CreateProcess (
                        NULL,
                        lpCmd,
                        NULL,
                        NULL,
                        FALSE,
                        HIGH_PRIORITY_CLASS | CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_PROCESS_GROUP | CREATE_SEPARATE_WOW_VDM,
                        NULL,
                        NULL,
                        &si,
                        &ProcessInformation);

    if (!dwExitCode && GetLastError() != ERROR_SUCCESS)
    {
        LPVOID lpMessageBuffer;

        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                      NULL,
                      GetLastError(),
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), //The user default language
                      (LPTSTR) &lpMessageBuffer,
                      0,
                      NULL);

        //... now display this string
        MessageBox(NULL, (LPCTSTR)lpMessageBuffer, _T("Norton AntiVirus"), MB_ICONINFORMATION | MB_TASKMODAL | MB_OK);

        // Free the buffer allocated by the system
        LocalFree( lpMessageBuffer );
    }
    else
    {
        if (bWait)
            WaitForSingleObject(ProcessInformation.hProcess, INFINITE);
    }
    return (dwExitCode);
}

static LPSTR SkipWhiteSpace (LPSTR lpStr)
{
    // Skip past whitespace
    while (*lpStr && !isleadbyte (*lpStr) && isspace (*lpStr))
        lpStr = CharNext (lpStr);

    return lpStr;
}

//========================================================================
// Look for the first non-recursive ) character. The following shows the
// return values:
//      lpCmd           Returns
//      --------------------------------------------------------
//      abc)            lpCmd + 3 characters, last ) character.
//      a(bc)           NULL
//      a(bc))          lpCmd + 5 characters, last ) character.
//      a)(bc)          lpCmd + 1 characters, first ) character.
//========================================================================
static LPSTR FindCmdEnd (LPSTR lpCmd)
{
    LPSTR lpOut     = lpCmd;
    int   nParamCnt = 0;
    BOOL  bFound    = FALSE;

    for (lpOut = lpCmd; *lpOut && !bFound; )
    {
        // Skip DBCS characters.
        if (isleadbyte (*lpOut))
        {
            lpOut = CharNext (lpOut);
            continue;
        }

        if (*lpOut == g_chCmdEnd)
        {
            if (nParamCnt == 0)
                bFound = TRUE;
            else
                --nParamCnt;
        }
        
        else if (*lpOut == g_chCmdBegin)
            ++nParamCnt;

        // Increment our pointer only when we have not found the
        // ending character.
        if (!bFound)
            lpOut = CharNext (lpOut);
    }

    if (!bFound)
        lpOut = NULL;

    return lpOut;
}

//========================================================================
// Parse the command line. Fetch 1 command off the command line. This
// function also advances lpCmdLine to the beginning of the next command.
//
// Example of understood command lines:
//     (NavLu32.exe) (Nresq32.exe) (NavW32.exe /l /visible)
//          -- Launches each application in sequence. Each app waits for the
//             previous app to finish before being launched.
//
//     (/NOWAIT NavRunR.exe (NavLu32.exe) (Nresq32.exe) (NavW32.exe /l /visible))
//          -- Launches NavRunR.exe with the command line from example 1. Notice
//             that this application is launching itself.
// 
// 
// Parameters:
//   lpCmdLine     i/o - current command line. This is "" if no more commands
//                       to execute.
//   lpCmdOut      out - next parsed command to execute. This is "" if no more
//                       commands to execute.
//   lpWait        out - defaults to FALSE. Is TRUE when /NOWAIT is 
//                       associated with this command.
//========================================================================
static void GetNextCmd (LPSTR *lpCmdLine, LPSTR lpCmdOut, LPBOOL lpbWait)
{
    SYM_ASSERT (lpCmdLine);
    SYM_ASSERT (lpCmdOut);
    SYM_ASSERT (lpbWait);

    auto   LPCSTR  szNoWait     = "/NOWAIT";
    const  int     nNumNoWaitChars = STRLEN (szNoWait);
    auto   LPSTR   lpBegin      = STRCHR(*lpCmdLine, g_chCmdBegin);
    auto   LPSTR   lpEnd;

    // Setup our default return values.
    *lpCmdOut = '\0';
    *lpbWait  = TRUE;

    // No commands
    if (lpBegin == NULL)
    {
        (*lpCmdLine) += STRLEN (*lpCmdLine);
        return;
    }

    // Skip past the begin character.
    ++lpBegin;

    lpBegin = SkipWhiteSpace (lpBegin);

    // Check to see if this command line has a /NOWAIT on it. Advance the
    // lpBegin pointer past the /NOWAIT.
    *lpbWait = !(0 == STRNICMP (lpBegin, szNoWait, nNumNoWaitChars));
    if (!*lpbWait)
        lpBegin += nNumNoWaitChars;
    
    lpBegin = SkipWhiteSpace (lpBegin);

    // Find the ending ')' for this command.
    lpEnd = FindCmdEnd (lpBegin);

    // Didn't find any end for this command. Pretend that
    // that this was the last command.
    if (!lpEnd)
    {
        (*lpCmdLine) += STRLEN (*lpCmdLine);
        return;
    }

    STRNCPY (lpCmdOut, lpBegin, lpEnd - lpBegin);
    *(lpCmdOut + (lpEnd - lpBegin)) = '\0';

    // Advance past this command
    (*lpCmdLine) = lpEnd + 1;
}


static void ParseAndExecute(LPSTR lpCmdLine)
{
    BOOL bWait;
    char szCmd[1024];

    SYM_ASSERT (lpCmdLine);

    while (*lpCmdLine)
    {
        GetNextCmd (&lpCmdLine, szCmd, &bWait);

        if (*szCmd)
            Execute (szCmd, bWait);
    }
}

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    if (strlen(lpCmdLine))
    {
        ParseAndExecute(lpCmdLine);
    }
    return (1);
}
