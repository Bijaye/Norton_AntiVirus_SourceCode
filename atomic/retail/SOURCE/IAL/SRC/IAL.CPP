// Copyright 1991-1997 Symantec Corporation, Peter Norton Product Group
//************************************************************************
//
//  $Header:   S:/IAL/VCS/ial.cpv   1.7   20 Nov 1997 18:52:00   vgloste  $
//
//  Description:
//      This file contains the source code for impersonation and launcing
//      setup on specified Admin account context.
//
//************************************************************************
//
//  $Log:   S:/IAL/VCS/ial.cpv  $
// 
//    Rev 1.7   20 Nov 1997 18:52:00   vgloste
// This program was doing a case-sensitive comparison of the regkey value
// that is used to determine whether a system is Windows NT server or 
// workstation.  But the case of the value differs under NT 3.51 and NT 
// 4.0.  Under 4.0 the value is WinNT and under 3.51 it is Winnt.  I made 
// the compare case-insensitive.
// 
// 
//    Rev 1.6   19 Aug 1997 20:27:30   DSACKIN
// Updated IAL to match the new NNS file strings.
//
//    Rev 1.5   22 Jul 1997 14:40:46   DSACKIN
// Switched the CreateProcess flags so that we will wait for the correct
// program to complete.
//
//    Rev 1.4   07 Jul 1997 13:50:28   TIVANOV
// change the path name to a short path name and call CreateProcess with normal
// priority class
//
//    Rev 1.3   30 Jun 1997 15:10:32   TIVANOV
// check if the OS is NT server or workstation
//
//    Rev 1.2   25 Jun 1997 17:17:44   DSACKIN
// Ported from Marconi
//
//    Rev 1.11   24 Apr 1997 11:14:14   pvenkat
// Added the NNS file passed to IAL in the command line for NAV32UP.
//
//
//    Rev 1.10   16 Apr 1997 16:19:22   pvenkat
// Complete solution for Impersonation.
//
//
//    Rev 1.9   30 Jan 1997 13:03:30   pvenkat
// Fixed #80145, #80141.  If unable to read from NNS file, we donot bailout now,
//     And we log different error msgs if we could not launch setup & nav32up.
//
//
//
//    Rev 1.8   27 Jan 1997 16:40:16   pvenkat
// Fixed #78746 rework.  Modified user name length to 15 chars.
//
//
//
//    Rev 1.7   24 Jan 1997 14:08:58   pvenkat
// Fixed #79755. Moved Constant strings to RC file.
//
//
//    Rev 1.6   24 Jan 1997 13:02:06   pvenkat
// Fixed #79353.  Added more time between getting snapshot of process.
//
//
//************************************************************************


#include "windows.h"
#include <stdio.h>
#include <tchar.h>

#include "ialcons.h"

typedef struct tagOPTIONS
{
    TCHAR    szNNSPath[MAX_PATH];
    int     nImpersonate;
    TCHAR    szImpersonationUserName[MAX_PATH];
    TCHAR    szImpersonationDomain[MAX_PATH];
    TCHAR    szImpersonationPassword[MAX_PATH];
    TCHAR    szSymantecSetupPath[MAX_PATH];
    TCHAR    szSymantecSetupParameters[MAX_PATH];
    short   nActivityLog;
    TCHAR    szLogLocation[MAX_PATH + 1];
    short   nAppendLog;
} OPTIONS, * LPOPTIONS;

BOOL Exists (LPTSTR lpszNNSPath);
BOOL GetOptionsFromNNS(LPOPTIONS lpOptions);
BOOL ImpersonateUserAndLaunchInstall(LPOPTIONS lpOptions);

DWORD LogEvent (HINSTANCE hInst, LPOPTIONS lpOptions, DWORD nId);
DWORD WriteRecordToLog (HANDLE hFile, LPTSTR lpszMsg);


extern int WaitforInst32 (LPTSTR lpszInst);
extern HANDLE WINAPI Spawn (LPOPTIONS lpOptions);

// Globals
HINSTANCE       ghInst;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow)
{
    OPTIONS rOptions;
    int     nRet = -1;

    ghInst = hInstance;

    ZeroMemory (&rOptions, sizeof (OPTIONS));

    strcpy (rOptions.szNNSPath, lpCmdLine);

    if (Exists(rOptions.szNNSPath))
    {
        if (GetOptionsFromNNS(&rOptions))
        {
            if (!ImpersonateUserAndLaunchInstall(&rOptions))
            {
#ifdef  IMPERSONATE
                LogEvent (hInstance, &rOptions, IDS_ERR_IMPERSONATION_FAILED);
#endif
            }
        else
            nRet = 0;   // Success
        }
#ifdef  IMPERSONATE
        else
            LogEvent (hInstance, &rOptions, IDS_ERR_UNABLE_TO_READ_OPTIONS);
#endif
    }
#ifdef  IMPERSONATE
    else
        LogEvent (hInstance, &rOptions, IDS_ERR_NNS_FILE_NOT_FOUND);
#endif

    return (nRet);
}

//***************************************************************************
//*                                                                         *
//* IsRunningAsAdministrator                                                *
//*                                                                         *
//*     To check whether the current Process is running on Administrator    *
//*     Context.  We browse the list of Groups that current user belongs and*
//*     compare each of them against Admin Group.                           *
//*                                                                         *
//* Parameters:                                                             *
//*     - None -                                                            *
//*                                                                         *
//* Returns:                                                                *
//*     BOOL    TRUE  if current user belongs to ADMIN group                *
//*             FALSE if not.                                               *
//*                                                                         *
//***************************************************************************
//* 1/14/97     PVENKAT     Function created.								*
//***************************************************************************
BOOL
IsRunningAsAdministrator (VOID)
{
    HANDLE                      hProcess;           // Current Process
    TOKEN_GROUPS                *pTG = NULL;        // Token Groups
    DWORD                       cbTokenGroups;      // # of Groups
    DWORD                       dwGroup;            // for Index
    PSID                        psidAdmin;          // Administrator SID
    SID_IDENTIFIER_AUTHORITY    SystemSidAuthority = SECURITY_NT_AUTHORITY;
    BOOL                        fAdmin = FALSE;     // Return Value

    // First we must open a handle to tha access token for this thread.
    if (OpenProcessToken (GetCurrentProcess (), TOKEN_QUERY, &hProcess))
    {
        // We must query the size of group information associated with
        // the token.  Note that we expect a FALSE result from
        // GetTokenInformation because we've given it a NULL buffer.
        // On Exit cbTokenGroups will tell us the size of Group Information.

        if (!GetTokenInformation (hProcess, TokenGroups, NULL, 0, &cbTokenGroups))
        {
            // Allocate a buffer for the Group Info
            if ( (pTG = (TOKEN_GROUPS *) GlobalAlloc (GPTR,
                                            cbTokenGroups))
                            != NULL)
            {
                // Get the Group Info.
                if (GetTokenInformation (hProcess,           // Process Handle
                                            TokenGroups,     // Group - Token
                                            pTG,             // buffer
                                            cbTokenGroups,   // buffer size
                                            &cbTokenGroups)) // bytes returned
                {
                    // Create SID for Admin Group
                    if (AllocateAndInitializeSid (&SystemSidAuthority,
                                                2,
                                                SECURITY_BUILTIN_DOMAIN_RID,
                                                DOMAIN_ALIAS_RID_ADMINS,
                                                0,
                                                0,
                                                0,
                                                0,
                                                0,
                                                0,
                                                &psidAdmin) )
                    {
                        // Go thru all groups until we find that we are a
                        // member of Admin.
                        for (dwGroup = 0;
                                !fAdmin && dwGroup < pTG->GroupCount;
                                dwGroup++)
                        {
                            // Compare the Current Group SID with Admin SID
                            if (EqualSid (pTG->Groups[dwGroup].Sid, psidAdmin))
                                fAdmin = TRUE;	// We are member of Admin Group.
                        }
                        FreeSid (psidAdmin);    // Clean up SID buffer
                    }
                }
                GlobalFree (pTG);   // Clean up the Token Group Buffer.
            }
        }
    }
    return fAdmin;
}

BOOL
EnablePrivilege (LPTSTR lpszPrivilege)
{
    BOOL                bSuccess = TRUE;
    HANDLE              hToken;
    LUID                luid;
    TOKEN_PRIVILEGES    tokenPrivileges;
    DWORD               dwErr = ERROR_SUCCESS;

    if (OpenProcessToken (GetCurrentProcess (),
                            TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
                            &hToken))
    {
        if (LookupPrivilegeValue (NULL, lpszPrivilege, &luid))
        {
            tokenPrivileges.PrivilegeCount = 1;
            tokenPrivileges.Privileges[0].Luid = luid;
            tokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

            AdjustTokenPrivileges (hToken, FALSE, &tokenPrivileges, 0, 0, 0);
            if (GetLastError () != ERROR_SUCCESS)
                bSuccess = FALSE;
        }
        else
            bSuccess = FALSE;
    }
    else
        bSuccess = FALSE;

    dwErr = GetLastError ();

    return bSuccess;
}


BOOL ImpersonateUserAndLaunchInstall(LPOPTIONS lpOptions)
{
#ifdef  IMPERSONATE
    HANDLE hToken;
#endif
    STARTUPINFO rStartupInfo = {0};
    PROCESS_INFORMATION rProcessInfo = {0};
    TCHAR szExecutionCommand[MAX_PATH];
    BOOL bAdmin = FALSE;
    DWORD   dwWhatFailed = 1;   // Assume SETUP.EXE failed to launch;
    LPTSTR  p;


    // PLEASE NOTE!
    // ---------------------------------------------------------------
    // In order for this function to properly impersonate the security
    // of another user and execute the specified program under the
    // security context of the impersonated account, the following
    // Advanced User Rights need to be granted to the user (or group)
    // executing this program:
    //
    // * Act As Part Of The Operating System
    // * Create A Token Object
    // * Increase Quotas
    // * Replace Process Level Token
    // * Logon As A Batch Job
    // ---------------------------------------------------------------


    bAdmin = IsRunningAsAdministrator ();

#ifdef  IMPERSONATE
    if (!bAdmin && (lpOptions->nImpersonate == 1))
    {

        // attempt to retrieve an impersonation handle
        hToken = Spawn (lpOptions);
    }

    EnablePrivilege (SE_TCB_NAME);
    EnablePrivilege (SE_SECURITY_NAME);
    EnablePrivilege (SE_SYSTEM_PROFILE_NAME);
#endif

    // set up our startup info structure:
    // the lpDesktop field must be set in order for
    // the child process (Symantec Setup) to be able to
    // interact with the desktop

    rStartupInfo.cb = sizeof(STARTUPINFO);
    rStartupInfo.lpDesktop = TEXT ("Winsta0\\default");

    // build our execution string

    lstrcpy(szExecutionCommand, lpOptions->szSymantecSetupPath);
    GetShortPathName(szExecutionCommand, szExecutionCommand, sizeof(szExecutionCommand));

    lstrcat(szExecutionCommand, TEXT ("\\"));
    lstrcat(szExecutionCommand, lpOptions->szSymantecSetupParameters);

    if (bAdmin)
    {
        if (!CreateProcess (NULL,
                        szExecutionCommand,
                        NULL,
                        NULL,
                        FALSE,
                        NORMAL_PRIORITY_CLASS |
                        CREATE_DEFAULT_ERROR_MODE |
                        CREATE_NEW_PROCESS_GROUP |
                        CREATE_SEPARATE_WOW_VDM,
                        NULL,
                        NULL,
                        &rStartupInfo,
                        &rProcessInfo))
        {
            goto bailout;
        }
    }
    else
    {
        if (!CreateProcessAsUser (hToken,
                        NULL,
                        szExecutionCommand,
                        NULL,
                        NULL,
                        FALSE,
                        NORMAL_PRIORITY_CLASS |
                        CREATE_DEFAULT_ERROR_MODE |
                        CREATE_NEW_PROCESS_GROUP |
                        CREATE_SEPARATE_WOW_VDM,
                        NULL,
                        NULL,
                        &rStartupInfo,
                        &rProcessInfo))
        {
            goto bailout;
        }
    }

    if (!bAdmin)
        CloseHandle (hToken);

    // Setup succeeded!, now we may fail to launch NAV32UP.EXE
    dwWhatFailed ++;

    //WaitforInst32 (TEXT ("INST32"));
    WaitForSingleObject(rProcessInfo.hProcess, INFINITE);

#ifdef  IMPERSONATE
    // From now on, we donot need to be an Admin.
    RevertToSelf ();
#endif

    GetModuleFileName (GetModuleHandle (NULL),
                        szExecutionCommand,
                        sizeof (szExecutionCommand));

    p = strrchr (szExecutionCommand, TEXT ('\\'));
    *p = TEXT ('\0');
    lstrcat (szExecutionCommand, TEXT ("\\NAV32UP.EXE "));
    lstrcat (szExecutionCommand, lpOptions->szNNSPath);
    lstrcat (szExecutionCommand, TEXT (" <NAV INSTALLED> "));

#if 0
    DWORD i, nLen;

    nLen = lstrlen (szExecutionCommand);
    for (i = nLen; i > 0; i --)
    {
        if (szExecutionCommand[i - 1] == '\\')
            break;
    }
    szExecutionCommand[i] = '\0';
    lstrcat (szExecutionCommand, TEXT ("\\NAV32UP.EXE"));
    lstrcat (szExecutionCommand, TEXT (" <NAV INSTALLED> "));
#endif

    if (!CreateProcess (NULL,
                            szExecutionCommand,
                            NULL,
                            NULL,
                            FALSE,
                            CREATE_NEW_CONSOLE,
                            NULL,
                            NULL,
                            &rStartupInfo,
                            &rProcessInfo))
    {
        goto bailout;
    }

    return(TRUE);

bailout:
    LogEvent (ghInst, lpOptions, IDS_ERR_LAUNCH_FAILED_BASE + dwWhatFailed);
    return(FALSE);

}

void
Decrypt (LPTSTR pszPlain, LPTSTR pszEncrypted, LPTSTR pszKey)
{
    int len = lstrlen (pszEncrypted) * sizeof (TCHAR) ;
    LPSTR pTemp = (LPSTR) pszPlain;
    LPSTR lpszEncrypted = (LPSTR) pszEncrypted;
    LPSTR lpszKey = (LPSTR) pszKey;
    LPSTR lpszPlain = (LPSTR) pszPlain;

    for (int i = 0; i < len; i++)
    {
        char c1 = *lpszEncrypted;
        char c2 = *lpszKey;
        char c3 = c1 - c2;

        *lpszPlain++ = c3;
        lpszEncrypted++;
        lpszKey++;
    }
    *lpszPlain++ = TEXT ('\0');
    *lpszPlain = TEXT ('\0');
}


// Key
TCHAR szKey[] = TEXT ("NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221");

BOOL IsNTServer()
{
    HKEY hKey = NULL;
    BOOL fRet = FALSE;

    if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, "System\\CurrentControlSet\\Control\\ProductOptions", 0, KEY_QUERY_VALUE, &hKey))
    {
        DWORD Type;
        BYTE  buff[100];
        DWORD dW = sizeof(buff);
        memset(buff, 0, dW);

        if (ERROR_SUCCESS == RegQueryValueEx(hKey, "Producttype", 0, &Type, buff, &dW))
            if (stricmp((LPCTSTR)buff, _T("WinNT")))
                fRet = TRUE;
    }
    return (fRet);
}

BOOL GetOptionsFromNNS(LPOPTIONS lpOptions)
{
#ifdef  IMPERSONATE
    int     nEncrypted;
    TCHAR   szBuf[MAX_PATH + 1];
    char    szIniFile[MAX_PATH + 1];

	// get impersonation setting: whether we will or whether we won't
	lpOptions->nImpersonate = GetPrivateProfileInt(TEXT ("Install/Update"),
												   TEXT ("Impersonate"),
												   0,
												   lpOptions->szNNSPath);

    nEncrypted = GetPrivateProfileInt (TEXT ("Install/Update"),
                                        TEXT ("ImpersonationModified"),
                                        0,
                                        lpOptions->szNNSPath);
    if (nEncrypted == 0)
    {
        strcpy (szIniFile, lpOptions->szNNSPath);

	    // get the username of who we want to impersonate
	    GetPrivateProfileStringA ("Install/Update",
								    "ImpersonationUserName",
								    "",
								    lpOptions->szImpersonationUserName,
								    sizeof(lpOptions->szImpersonationUserName),
								    szIniFile);

	    // get the domain of the user we want to impersonate
	    GetPrivateProfileStringA ("Install/Update",
								    "ImpersonationDomain",
								    "",
                                    lpOptions->szImpersonationDomain,
                                    sizeof (lpOptions->szImpersonationDomain),
								    szIniFile);

	    // get the password of the user we want to impersonate
	    GetPrivateProfileStringA ("Install/Update",
								    "ImpersonationPassword",
								    "",
                                    lpOptions->szImpersonationPassword,
                                    sizeof (lpOptions->szImpersonationPassword),
								    szIniFile);
    }
    else
    {
	    // get the username of who we want to impersonate
	    GetPrivateProfileString (TEXT ("Install/Update"),
								    TEXT ("ImpersonationUserName"),
								    TEXT (""),
								    szBuf,
								    sizeof(szBuf),
								    lpOptions->szNNSPath);
        Decrypt (lpOptions->szImpersonationUserName, szBuf, szKey);

	    // get the domain of the user we want to impersonate
	    GetPrivateProfileString (TEXT ("Install/Update"),
								    TEXT ("ImpersonationDomain"),
								    TEXT (""),
								    szBuf,
								    sizeof(szBuf),
								    lpOptions->szNNSPath);
        Decrypt (lpOptions->szImpersonationDomain, szBuf, szKey);

	    // get the password of the user we want to impersonate
	    GetPrivateProfileString (TEXT ("Install/Update"),
								    TEXT ("ImpersonationPassword"),
								    TEXT (""),
								    szBuf,
								    sizeof (szBuf),
								    lpOptions->szNNSPath);
        Decrypt (lpOptions->szImpersonationPassword, szBuf, szKey);
    }
#endif

	// get the network path of where Symantec Setup lives
    if (IsNTServer())
        GetPrivateProfileString (TEXT ("Install/Update"),
                                    TEXT ("NetworkNTSRV"),
                                    TEXT (""),
                                    lpOptions->szSymantecSetupPath,
                                    sizeof(lpOptions->szSymantecSetupPath),
                                    lpOptions->szNNSPath);
    else
        GetPrivateProfileString (TEXT ("Install/Update"),
                                    TEXT ("NetworkNTWKS"),
                                    TEXT (""),
                                    lpOptions->szSymantecSetupPath,
                                    sizeof(lpOptions->szSymantecSetupPath),
                                    lpOptions->szNNSPath);

	// get the Symantec Setup executable name and parameters
	GetPrivateProfileString (TEXT ("Install/Update"),
								TEXT ("NAVNTInstall"),
								TEXT (""),
								lpOptions->szSymantecSetupParameters,
								sizeof(lpOptions->szSymantecSetupParameters),
								lpOptions->szNNSPath);

    lpOptions->nActivityLog = GetPrivateProfileInt (TEXT ("Install/Update"),
                                                    TEXT ("ActivityLog"),
                                                    0,
                                                    lpOptions->szNNSPath);
    lpOptions->nAppendLog = GetPrivateProfileInt ( TEXT ("Install/Update"),
                                                    TEXT ("AppendToLog"),
                                                    0,
                                                    lpOptions->szNNSPath);
    GetPrivateProfileString (TEXT ("Install/Update"),
                                TEXT ("LogLocation"),
                                TEXT (""),
                                lpOptions->szLogLocation,
                                sizeof (lpOptions->szLogLocation),
                                lpOptions->szNNSPath);
	return(TRUE);
}



BOOL Exists (LPTSTR pszNNSPath)
{
	DWORD dwResult = {0};

	// using the GetFileAttributes to test existence of
	// the NNS file

	if (pszNNSPath == NULL)
	{
		goto bailout;
	}
	else
	{
		dwResult = GetFileAttributes(pszNNSPath);

		if (dwResult == 0xFFFFFFFF)
		{
			goto bailout;
		}
	}

	return(TRUE);

bailout:

	return(FALSE);

}


#define     MAX_RETRIES         (10)
#define     MAX_BUF_SIZE        (1024)

DWORD
WriteLogFile (LPTSTR lpszActivity, BOOL bAppend, LPTSTR lpszMsg)
{
    HANDLE  hFile;
    DWORD   dwCur;
    DWORD   dwCreate = Exists (lpszActivity) ? OPEN_EXISTING : CREATE_NEW;
    DWORD   dwErr = NO_ERROR;

    // If we could not read the options, the Activity-Filename will be
    // empty string, in which case no need to log

    if (lstrlen (lpszActivity) == 0)
        return 0;

    hFile = CreateFile (lpszActivity,
                        GENERIC_WRITE,
                        0,
                        NULL,
                        dwCreate,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        if (bAppend)
        {
            LONG    lDist = 0L, lDistHigh = 0;

            dwCur = SetFilePointer (hFile, lDist, &lDistHigh, FILE_END);
            if (dwCur == 0xFFFFFFFF)
                dwErr = GetLastError ();
        }
        if (dwErr == NO_ERROR)
        {
            dwErr = WriteRecordToLog (hFile, lpszMsg);
        }

        CloseHandle (hFile);
    }
    return dwErr;
}

DWORD
WriteRecordToLog (HANDLE hFile, LPTSTR lpszMsg)
{
    SYSTEMTIME  rTime;
    DWORD       dwErr = NO_ERROR;
    DWORD       dwLen, dwWritten = 0L;
    TCHAR       szBuf[MAX_BUF_SIZE + 1];
    short       nTries;
    TCHAR       szUser[MAX_PATH + 1];

    dwLen = sizeof (szUser);
    GetUserName (szUser, &dwLen);

    GetLocalTime (&rTime);
    wsprintf (szBuf,
                TEXT ("%02d-%02d-%04d  %02d:%02d:%02d  %-15s  %s%c%c"),
                rTime.wMonth,
                rTime.wDay,
                rTime.wYear,
                rTime.wHour,
                rTime.wMinute,
                rTime.wSecond,
                szUser,
                lpszMsg,
                0x0D,
                0x0A);

    dwLen = lstrlen (szBuf);
    nTries = 0;
    while (nTries < MAX_RETRIES)
    {
        if (!WriteFile (hFile, szBuf, dwLen, &dwWritten, NULL))
            nTries++;
		else
			break;
    }

    dwErr = GetLastError ();
    return dwErr;
}

DWORD
WriteToActivityLog (LPBOOL lpbInitialized, LPTSTR lpszActivity, BOOL bAppend, LPTSTR lpszMsg)
{
    return WriteLogFile (lpszActivity, (*lpbInitialized) ? bAppend : TRUE, lpszMsg);
}

DWORD
LogEvent (HINSTANCE hInstance, LPOPTIONS lpOptions, DWORD nIDErr)
{
    static bInitialized = FALSE;
    TCHAR       szErrMsg[512];

    // Get the error string from resource file.
    LoadString (hInstance, nIDErr, szErrMsg, sizeof (szErrMsg));

    // Log only set so.
    if (lpOptions->nActivityLog > 0)
        return WriteToActivityLog (&bInitialized,
                        lpOptions->szLogLocation,
                        lpOptions->nAppendLog ? TRUE : FALSE,
                        szErrMsg);
    else
        return 0L;  // default success.
}

