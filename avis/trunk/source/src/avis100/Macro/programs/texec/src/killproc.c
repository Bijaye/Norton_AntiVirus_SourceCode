/*
killproc.c, by JMB
*/

#include <stdio.h>
#include <windows.h>
#include <winuser.h>
#include <winbase.h>

#include "error.h"

//tries to terminate a process given its process id
//returns TRUE on success, FALSE otherwise
BOOL KillProc(DWORD dwProcessid)
{
HANDLE hAppProcess;
BOOL bResult = FALSE;

	//try to get a handle to the process with termination rights
	//(the handle is not made heritable by processes spawned by the current process [FALSE argument])
	hAppProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessid);
	if (hAppProcess == NULL)
		PrintErr("OpenProcess failed: ", GetLastError());
	else {
		fprintf(stderr, "Trying to terminate process number %u\n", dwProcessid);
		if (!(TerminateProcess(hAppProcess, 1)))
			PrintErr("TerminateProcess failed: ", GetLastError());
		else {
			fprintf(stderr, "Process %d terminated\n", dwProcessid);
			bResult = 1;
		}
	}
	CloseHandle(hAppProcess);
return bResult;
}
