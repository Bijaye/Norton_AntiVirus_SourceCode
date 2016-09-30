/*******************************************************************
*                                                                  *
* File name:     appli.c                                           *
*                                                                  *
* Description:   application initialization and termination        *
*                                                                  *
*                                                                  *
* Statement:     Licensed Materials - Property of IBM              *
*                (c) Copyright IBM Corp. 1999                      *
*                                                                  *
*                                                                  *
*                U.S. Government Users Restricted Rights - use,    *
*                duplication or disclosure restricted by GSA ADP   *
*                Schedule Contract with IBM Corp.                  *
*                                                                  *
*                                                                  *
********************************************************************
*                                                                  *
* Function:      Contains function to initialize and terminate     *
*                the application                                   *
*                                                                  *
*******************************************************************/
#include "appli.h"

int kill_app_instance(HWND apphwnd)
{
HANDLE apphndl;
DWORD appprocessid;
long WaitTime = 0;

	GetWindowThreadProcessId(apphwnd, &appprocessid);
	apphndl = OpenProcess(PROCESS_TERMINATE, FALSE, appprocessid);
	if (apphndl == NULL)
	{
		fprintf (stderr, "openprocess failed\n");
		return 1;
	}
	if (TerminateProcess(apphndl, 1) == FALSE)
	{
		fprintf (stderr, "terminateprocess failed\n");
		return 2;
	}
	else
	{
		while(IsWindow(apphwnd) && (WaitTime <= 1000))
			Sleep(200);
	}	
return !IsWindow(apphwnd);
}


int kill_all_app_instances(LPCTSTR AppClassName)
{
HWND apphwnd;
int killed_nb = 0;

	while ((apphwnd = FindWindowEx(0, 0, AppClassName, 0)) != NULL)
    {
		if (kill_app_instance(apphwnd) ==0) killed_nb++;
		else {return 2;}

        Sleep(1000);
//      printf ("elapsed time : %d seconds out of %d\n", i, MaxWait);
    }

	if (killed_nb ==0) return 1;

return 0;
}


HANDLE start_app_instance(char * Path)
{

STARTUPINFO si;
PROCESS_INFORMATION pi;

//initialization of the STARTUPINFO structure

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	
	si.lpReserved = NULL;
	si.lpDesktop = NULL; //crashes with an empty string
	si.lpTitle = NULL;
	si.dwFlags = STARTF_USESHOWWINDOW;
	//si.wShowWindow = SW_SHOWMAXIMIZED;
	si.wShowWindow = SW_SHOW;
	si.cbReserved2 = 0;
	si.lpReserved2 = NULL;

	si.cb = sizeof (si);

	CreateProcess(
		Path, //lpApplication name
		NULL,	// command line
		NULL,	// handle cannot be inherited by child processes
		NULL,	// handle cannot be inherited by threads
		FALSE,	// new process does not inherit handles form the calling process
		CREATE_DEFAULT_ERROR_MODE & CREATE_NEW_PROCESS_GROUP & NORMAL_PRIORITY_CLASS, 
		NULL,	// same environment as the calling process
		NULL,	// same directory as the calling process
		&si,
		&pi);

	return pi.hProcess;
}
