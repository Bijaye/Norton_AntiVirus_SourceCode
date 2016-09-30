/*
 * Module: drop.c
 *
 * Author: INW, 199906
 *
 * Purpose: Provides a single entry point which will
 *          cause the host computer to shutdown.
 */

/* ----- */
#include <stdio.h>
#include <windows.h>
#include "dropexp.h"
#include "defs.h"

/* ----- */

/*
 * Function: doDrop
 *
 * Parameters: (1) 0 for don't actually shutdown, !0 for do
 *             (2) !0 if we are debugging, 0 if we are not.
 *
 * Returns: !0 on success, 0 on failure.
 *
 * Purpose: Gyrates sufficiently to shutdown the system.
 */
int doDrop(int reallyDoIt, int restart, int debugging)
{
  HANDLE           tokHnd  = INVALID_HANDLE_VALUE;
  int              retCode = 0;
  LUID             luid;
  TOKEN_PRIVILEGES tokPriv;
  OSVERSIONINFO    osVerInfo;
  UINT             exitWindowsFlag = 0;
  FILE *f;
  /*
   * Set up for the version call, and issue it.
   */
  osVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  if (GetVersionEx(&osVerInfo) == FALSE)
  {
	  if (debugging) {
		if ((f =fopen(TRACEFILENAME, "a")) !=NULL)
		{
           fprintf(  f, "Failed to GetVersionEx(), error %d\n", GetLastError());
           fclose(f);
		}
	  }
	  
    goto bail;
  }

  /*
   * If we're on NT, bugger about with security.
   */
  if (osVerInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
  {
    /*
     * Retrieve the info for the privilege we need.
     */
    if (LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &luid) == 0)
    {
      if (debugging)
  		if ((f =fopen(TRACEFILENAME, "a")) !=NULL)
		{
           fprintf(  f, "Failed to LookupPrivilegeValue(), error %d\n", GetLastError());
           fclose(f);
		}

      goto bail;
    }

    /*
     * Fill in the structure with the information we just got,
     * and tag that we want to turn on the privilege.
     */
    tokPriv.PrivilegeCount           = 1;
    tokPriv.Privileges[0].Luid       = luid;
    tokPriv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    /*
     * Open a handle to the process's token.
     */
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &tokHnd) == 0)
    {
      if (debugging)
  		if ((f =fopen(TRACEFILENAME, "a")) !=NULL)
		{
           fprintf(  f,"Failed To OpenProcessToken(), error %d\n", GetLastError());
           fclose(f);
		}
    
      goto bail;
    }

    /*
     * Change that token's privileges.
     */
    if (AdjustTokenPrivileges(tokHnd, FALSE, &tokPriv, 0, NULL, NULL) == 0)
    {
      if (debugging)
  		if ((f =fopen(TRACEFILENAME, "a")) !=NULL)
		{
           fprintf(  f,"Failed to AdjustTokenPrivileges(), error %d\n", GetLastError());
           fclose(f);
		}
      goto bail;
    }

    /*
     * Close the token handle, it's done.
     */
    CloseHandle(tokHnd);
    tokHnd = INVALID_HANDLE_VALUE;

    exitWindowsFlag = EWX_POWEROFF;
  }
  else if (osVerInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
  {
    /*
     * First we have to kill explorer, then we can exit windows.
     */
    dropExplorer();
    if (restart)
		exitWindowsFlag = EWX_REBOOT;
	else
       exitWindowsFlag = EWX_SHUTDOWN;
  }

  /*
   * Attempt to drop the system.
   */

  if ((reallyDoIt) &&  (ExitWindowsEx(exitWindowsFlag | EWX_FORCE, 0) == FALSE))
  {
    if (debugging)
  		if ((f =fopen(TRACEFILENAME, "a")) !=NULL)
		{
           fprintf(  f, "Failed to ExitWindowsEx(), error code %d\n", GetLastError());
           fclose(f);
		}

    goto bail;
  }

  retCode = 1;
  /* FALL THRU INTO BAIL. */

bail :
  if (tokHnd != INVALID_HANDLE_VALUE)
    CloseHandle(tokHnd);
  return retCode;
}
/* ----- Transmission Ends ----- */
