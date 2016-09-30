// TerminateProcess.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <time.h>
#include <stdio.h>
#include <direct.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <string.h>
#include "psapi.h"

void writeToLogFile (char *logFilename, char *logBuf);

int main(int argc, char* argv[])
{
	char processName [520];
	char logFilename [520];
	char commandLine [520];
	char currentWorkDir [520];
	char logBuf [5020];
	time_t ltime;

	strcpy (processName, "");
	strcpy (logFilename, argv [0]);
	strcpy (commandLine, argv [0]);

	if (argc > 1)
	{
		strcat (commandLine, " ");
		strcat (commandLine, argv [1]);
	}

	//Form the log filename.
	if(_getcwd(currentWorkDir, sizeof (currentWorkDir)) == NULL )
	{
		strcpy (logFilename, "c:\\TerminateAvisProcess.log");
	}
	else
	{
		strcpy (logFilename, currentWorkDir);
		
		if (logFilename [strlen (logFilename) -1] != '\\' &&
			logFilename [strlen (logFilename) -1] != '/' )
		{
			strcat (logFilename, "\\");
		}

		strcat (logFilename, "TerminateAvisProcess.log");
	}

	if (argc <= 1)
	{
		time (&ltime);
		strcpy (logBuf, "CurrentTime: ");
		strcat (logBuf, ctime (&ltime));
		strcat (logBuf, "User launched this program as \'");
		strcat (logBuf, commandLine);
		strcat (logBuf, "\'.\n");
		strcat (logBuf, "LogFile: ");
		strcat (logBuf, logFilename);
		strcat (logBuf, "\n");
		strcat (logBuf, "Missing process name in the command line argument.\n");
		strcat (logBuf, "=============================================================\n");
		strcat (logBuf, "Usage: TerminateAvisProcess.exe <arg1>\n");
		strcat (logBuf, "<arg1> should be the process name you want to terminate.\n");
		strcat (logBuf, "Example: TerminateAvisProcess.exe wHTTPg.exe\n");
		strcat (logBuf, "=============================================================\n");
		writeToLogFile (logFilename, logBuf);	
		return (0);
	}
	else
	{
		strcpy (processName, argv [1]);
	}

    DWORD aProcesses[2048];
    DWORD cbNeeded;
    DWORD cProcesses;
    UINT i;

    // Get the list of all ongoing processes and look for the name of the process
    // If there is a process already running with the same name then attach to the
    // process
    if (!EnumProcesses (aProcesses, sizeof (aProcesses), &cbNeeded))
	{
		time (&ltime);
		strcpy (logBuf, "CurrentTime: ");
		strcat (logBuf, ctime (&ltime));
		strcat (logBuf, "User launched this program as \'");
		strcat (logBuf, commandLine);
		strcat (logBuf, "\'.\n");
		strcat (logBuf, "LogFile: ");
		strcat (logBuf, logFilename);
		strcat (logBuf, "\n");
		strcat (logBuf, "Unable to enumerate processes using EnumProcess function.\n");
		strcat (logBuf, "Process ");
		strcat (logBuf, processName);
		strcat (logBuf, " was not terminated.\n");
		writeToLogFile (logFilename, logBuf);	
        return (0);
	}

    cProcesses = cbNeeded / sizeof (DWORD);

    for (i = 0; i < cProcesses; i++)
    {
        char szProcessName[MAX_PATH] = "unknown";

        HANDLE hProcess = OpenProcess (
            PROCESS_ALL_ACCESS,
            FALSE,
            aProcesses[i]);

        if (hProcess)
        {
            HMODULE hMod;

            if (EnumProcessModules (hProcess, &hMod, sizeof (hMod), &cbNeeded))
            {
                GetModuleBaseName (hProcess, hMod, szProcessName, sizeof(szProcessName));

                if (!stricmp (szProcessName, processName))
                {
					//We found the process in the task manager.
					//We will try to terminate the process.
					if (TerminateProcess (hProcess, 1003))
					{
						time (&ltime);
						strcpy (logBuf, "CurrentTime: ");
						strcat (logBuf, ctime (&ltime));
						strcat (logBuf, "User launched this program as \'");
						strcat (logBuf, commandLine);
						strcat (logBuf, "\'.\n");
						strcat (logBuf, "LogFile: ");
						strcat (logBuf, logFilename);
						strcat (logBuf, "\n");
						strcat (logBuf, "Process ");
						strcat (logBuf, processName);
						strcat (logBuf, " was successfully terminated.\n");
						writeToLogFile (logFilename, logBuf);	
					}
					else
					{
						time (&ltime);
						strcpy (logBuf, "CurrentTime: ");
						strcat (logBuf, ctime (&ltime));
						strcat (logBuf, "User launched this program as \'");
						strcat (logBuf, commandLine);
						strcat (logBuf, "\'.\n");
						strcat (logBuf, "LogFile: ");
						strcat (logBuf, logFilename);
						strcat (logBuf, "\n");
						strcat (logBuf, "Unable to terminate process using TerminateProcess function.\n");
						strcat (logBuf, "Process ");
						strcat (logBuf, processName);
						strcat (logBuf, " was not terminated.\n");
						writeToLogFile (logFilename, logBuf);	
					}

					break;
				}
			}
		}
	}

	if (i >= cProcesses)
	{
		time (&ltime);
		strcpy (logBuf, "CurrentTime: ");
		strcat (logBuf, ctime (&ltime));
		strcat (logBuf, "User launched this program as \'");
		strcat (logBuf, commandLine);
		strcat (logBuf, "\'.\n");
		strcat (logBuf, "LogFile: ");
		strcat (logBuf, logFilename);
		strcat (logBuf, "\n");
		strcat (logBuf, "There is no process by name \'");
		strcat (logBuf, processName);
		strcat (logBuf, "\' is currently active.\n");
		writeToLogFile (logFilename, logBuf);	
	}

	return 0;
}


void writeToLogFile (char *logFilename, char *logBuf)
{
	printf (logBuf);
	FILE *filePtr = fopen (logFilename, "w");

	if (filePtr ==NULL)
	{
		printf ("Unable to open log file %s.\n", logFilename);
		return;
	}

	fwrite (logBuf, strlen (logBuf), 1, filePtr);
	fclose (filePtr);
}
