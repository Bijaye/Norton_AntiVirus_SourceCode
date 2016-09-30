#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>
#include <winuser.h>
#include <winbase.h>

#define MaxWaitTime 10
#define SleepTime 500


long KillAppInstance(HWND hAppWin)
{
HANDLE hAppProcess;
DWORD dwProcessid;
DWORD dwErr = 0;
DWORD dwAppRC = STILL_ACTIVE;
double WaitTime = 0;

	GetWindowThreadProcessId(hAppWin, &dwProcessid);
	hAppProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessid);

	if (hAppProcess == NULL)
	{
		dwErr = GetLastError();
		SetLastError(0);
	}

	if (!(TerminateProcess(hAppProcess, 1)))
	{
		dwErr = GetLastError();
		SetLastError(0);
	}
//	else
//	{
//		while ((dwAppRC == STILL_ACTIVE)&&(WaitTime <= MaxWaitTime))
//		{
//			Sleep(SleepTime);
//			WaitTime += SleepTime / 1000;
//			GetExitCodeProcess(hProcess,&dwAppRC);
//		}
//	}
	CloseHandle(hAppProcess);

return dwErr;
}


long KillAllAppInstances(LPCTSTR lpszClass)
{
HWND hAppWin;
HWND hLastAppWin = 0;
int nKilled = 0;
DWORD dwErr;
//long lWaitTime = 0;

	while ((hAppWin = FindWindowEx(0, 0, lpszClass, 0)) != NULL)
    {
		dwErr = KillAppInstance(hAppWin);
//		printf("loop\n");
		if (!dwErr) ++nKilled;
		else return dwErr;

//		hLastAppWin = hAppWin;
//        Sleep(1000);
    }

return 0;
}


long KillAllUserAppInstances(LPCTSTR lpszClass, LPTSTR lpszUserName, LPTSTR lpszPassword)
{
HANDLE hToken;
long lRes;

	if (LogonUser(
        lpszUserName,
        ".",
        lpszPassword,
        LOGON32_LOGON_INTERACTIVE,
        LOGON32_PROVIDER_DEFAULT,
        &hToken))
	{
		ImpersonateLoggedOnUser(hToken); 
		lRes = KillAllAppInstances(lpszClass);
		RevertToSelf();
		return lRes;
	}
	else
		lRes = GetLastError();

	SetLastError(0);
	return lRes;
}
