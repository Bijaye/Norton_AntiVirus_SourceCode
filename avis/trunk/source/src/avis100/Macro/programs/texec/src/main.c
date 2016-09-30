#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <windows.h>
#include <winnt.h>
#include <winbase.h>

#include "killproc.h"


int StartProcess(char *pgmpath, char *args, int _16bit, int ihp, PPROCESS_INFORMATION pPI) {
SECURITY_ATTRIBUTES SA;
STARTUPINFO SI;
int success;
long errcode;
char *pgmname;
char *pos;
char *cmdline = NULL;
char *exemodule = NULL;
DWORD dwCreationFlags = CREATE_NEW_CONSOLE;

SI.cb = sizeof SI;
SI.lpReserved = NULL;
SI.lpTitle = NULL;
SI.dwFlags = 0;
SI.wShowWindow = SW_SHOWNORMAL;
SI.cbReserved2 = 0;
SI.lpReserved2 = NULL;
SI.lpDesktop = NULL;

SA.nLength = sizeof SA;
SA.lpSecurityDescriptor = NULL;
SA.bInheritHandle = 0;

if (args) {
    if (_16bit) {
      exemodule = NULL;
	  cmdline = malloc((strlen(pgmpath) +strlen(args) +1) * sizeof(*pgmpath));	
	  strcpy(cmdline, pgmpath);
	  strcat(cmdline, args);
	}
	else {
	  exemodule = malloc((strlen(pgmpath) +1) * sizeof(*pgmpath));
      strcpy(exemodule, pgmpath);
   	  pos = strrchr(exemodule, '\\');
	  pgmname = malloc((strlen(pos) +1) * sizeof(*pgmname));
	  strcpy(pgmname, pos +1);
	  cmdline = malloc((strlen(pgmname) +strlen(args) +1) * sizeof(*pgmname));	
	  strcpy(cmdline, pgmname);
	  strcat(cmdline, args);
	}
}
if (ihp) dwCreationFlags |= HIGH_PRIORITY_CLASS;
else dwCreationFlags |= NORMAL_PRIORITY_CLASS;

    success = CreateProcess(exemodule,
                cmdline, 
                &SA, &SA, 
                0, 
				dwCreationFlags, 
                0,
                NULL, 
                &SI, 
                pPI);
    
  if (!success) {
       errcode = GetLastError();
       fprintf(stderr, "CreateProcess failed for %s;%s;%d\n", pgmpath, cmdline, errcode);
    }

  return success;
}


char* getargs(int args, int argc, char* argv[], int addquotes) {
char *cmdline;
int i;

  cmdline = malloc(1 * sizeof( *cmdline));
  *cmdline = 0;
  for (i =args; i < argc; ++i) {
    cmdline = realloc(cmdline, (strlen(cmdline) +1 +1 + strlen(argv[i]) +1 +1) * sizeof(*cmdline));
      strcat(cmdline, " ");
    if (addquotes)
      strcat(cmdline, "\"");
	strcat(cmdline, argv[i]);
    if (addquotes)
      strcat(cmdline, "\"");
  }
return cmdline;
}


void main(int argc, char* argv[]) {
PROCESS_INFORMATION PI;
char *args;
char *pgmpath;
float waittime =0;
int sleeptime = 1000;
int isleeptime = 0;
long timeout = 0;
int iflag = 0;
int i;
int wait = 0;
int kill = 0;
long exitcode = STILL_ACTIVE;
int i16bit = 0;
int ihp = 0;
DWORD waitresult = 0;
DWORD errcode = 0;

	if (argc < 2) {
	  fprintf(stderr, "Usage: texec [-isleep isleeptime] [-sleep sleeptime] [-wait] [-kill timeout] program_path [arguments]\n");
	  fprintf(stderr, "Starts the command line and waits for timeout seconds or the exit of the process before returning.\n");
	  fprintf(stderr, "timeout has to be given in seconds\n");
	  fprintf(stderr, "sleeptime is the sleeping time interval during the wait and has to be given in milliseconds (default: 1000ms).\n");
	  fprintf(stderr, "isleeptime is the time after which the command has to be carried out (in ms).\n");
	  return;
	}
    
    for (i = 1; i <argc; ++i) {
      if (!stricmp("-sleep", argv[i])) {
    	sleeptime = atoi(argv[++i]);
    	if (sleeptime <=0) {
	      fprintf(stderr, "Bad sleeptime value: %s\n", argv[i]);
	      return;
		}
		iflag = i;
	  }
	  else if (!stricmp("-isleep", argv[i])) {
    	isleeptime = atoi(argv[++i]);
    	if (isleeptime <=0) {
	      fprintf(stderr, "Bad initial sleep value: %s\n", argv[i]);
	      return;
		}
		iflag = i;
	  }
	  else if (!stricmp("-wait", argv[i])) {
          wait = 1;
		iflag = i;
	  }
	  else if (!stricmp("-16bit", argv[i])) {
            i16bit = 1;
    		iflag = i;
	  }
	  else if (!stricmp("-hp", argv[i])) {
            ihp = 1;
    		iflag = i;
	  }
	  else if (!stricmp("-kill", argv[i])) {
        kill = 1;
    	timeout = atoi(argv[++i]);
    	if (timeout <=0) {
	      fprintf(stderr, "Bad timeout value: %s\n", argv[i]);
	      return;
		}
		iflag = i;
	  }
	  else {
        break;
	  }
	}

	if (isleeptime > 0) {
      Sleep(isleeptime);
	}

	pgmpath = malloc((strlen(argv[iflag + 1]) +1) * sizeof(*pgmpath));
	strcpy(pgmpath, argv[iflag +1]);
	args = getargs(iflag +2, argc, argv, !i16bit);
	if (StartProcess(pgmpath, args, i16bit, ihp, &PI)) {
		waitresult = WaitForSingleObject(PI.hProcess, timeout * 1000);
	   if (waitresult ==WAIT_FAILED) {
         errcode = GetLastError();
         fprintf(stderr, "WaitForSingleObject failed: %d\n", errcode);
	   }
	   if (kill && (waitresult ==WAIT_TIMEOUT)) {
		 KillProc(PI.dwProcessId);		 
	   }
	}
	if (args) free(args);
	free(pgmpath);
return;
}
