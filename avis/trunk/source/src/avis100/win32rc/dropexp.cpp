/*
 * Module: dropexp.c
 *
 * Author: CT 1998??, INW 19990701
 *
 * Purpose: Contains a single entry point -- dropExplorer(),
 *          which works on Win9x only.  It locates the
 *          currently executing explorer process, and
 *          kills it.
 *
 *          This is necessary in order for ExitWindowsEx() to
 *          then bring the system down.
 */

/* ----- */

#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>

#include "dropexp.h"
#include "defs.h"

/* ----- */

const char * const envVarName = "windir";

/* ----- */

/*
 * Function: dropExplorer
 *
 * Parameters: None
 *
 * Returns: 0 on failure, !0 on success.
 *
 * Purpose: Kills explorer as discussed above.
 */
int dropExplorer(void)
{
  HINSTANCE      hInstLib;
  HANDLE         hSnapShot;
  HANDLE         procHnd;
  PROCESSENTRY32 procentry;
  BOOL           bFlag;
  char           windirName[1001];
  int            retVal  = 0;
  FILE *f;
  HANDLE (WINAPI *lpfCreateToolhelp32Snapshot)(DWORD,DWORD);
  BOOL (WINAPI *lpfProcess32First)(HANDLE,LPPROCESSENTRY32);
  BOOL (WINAPI *lpfProcess32Next)(HANDLE,LPPROCESSENTRY32);
   
  retVal = GetEnvironmentVariable(envVarName, windirName, 1000);
  if ((retVal == 0) || (retVal > 1000))
    return 0;
  strcat(windirName, "\\EXPLORER.EXE");
  strupr(windirName);

  if ((hInstLib = LoadLibraryA("Kernel32.DLL")) == NULL)
    return 0;

   //
   // Get procedure addresses.
   // We are linking to these functions of Kernel32
   // explicitly, because otherwise a module using
   // this code would fail to load under Windows NT,
   // which does not have the Toolhelp32
   // functions in the Kernel 32.
   //

  lpfCreateToolhelp32Snapshot = (HANDLE(WINAPI *)(DWORD,DWORD))GetProcAddress(hInstLib, "CreateToolhelp32Snapshot");
  lpfProcess32First           = (BOOL(WINAPI *)(HANDLE,LPPROCESSENTRY32))GetProcAddress(hInstLib, "Process32First");
  lpfProcess32Next            = (BOOL(WINAPI *)(HANDLE,LPPROCESSENTRY32))GetProcAddress(hInstLib, "Process32Next");

  if ((lpfProcess32Next            == NULL)  ||
      (lpfProcess32First           == NULL) ||
      (lpfCreateToolhelp32Snapshot == NULL))
  {
    FreeLibrary(hInstLib);
    return 0;
  }

  // Get a handle to a Toolhelp snapshot of the systems
  // processes.
  if ((hSnapShot = lpfCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)) == INVALID_HANDLE_VALUE)
  {
    FreeLibrary(hInstLib);
    return 0;
  }

  // Get the first process' information.
  procentry.dwSize = sizeof(PROCESSENTRY32);

  bFlag = lpfProcess32First(hSnapShot, &procentry);

  retVal = 1;
  while (bFlag)
  {
    if (!strcmp(windirName, procentry.szExeFile))
    {
      if ((procHnd = OpenProcess(PROCESS_TERMINATE, FALSE, procentry.th32ProcessID)) == NULL)
      {
  		if ((f =fopen(TRACEFILENAME, "a")) !=NULL)
		{
           fprintf(  f,"Failed to OpenProcess() [%d]\n" , GetLastError());
           fclose(f);
		}

        retVal = 0;
        break;
      }

      if (TerminateProcess(procHnd, 0) == 0)
      {
  		if ((f =fopen(TRACEFILENAME, "a")) !=NULL)
		{
           fprintf(  f,"Failed to TerminateProcess() [%d]\n", GetLastError());
           fclose(f);
		}

        retVal = 0;
        break;
      }

      /*
       * If we get here, we killed it!
       */
      retVal = 1;
      break;
    }
    procentry.dwSize = sizeof(PROCESSENTRY32);
    bFlag = lpfProcess32Next(hSnapShot, &procentry);
  }

  FreeLibrary(hInstLib);

  return retVal;
}
/* ----- Transmission Ends ----- */
