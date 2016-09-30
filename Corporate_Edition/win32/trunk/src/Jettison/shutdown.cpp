//-----------------------------------------------------------------------------
//  Shutdown.Cpp
//  A component of the MacNuke Project
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
//  Copyright 1997, 2005 (c) Symantec Corp.. All rights reserved.
//  SYMANTEC CONFIDENTIAL
//
//  Revision History
//
//  Who                     When        What / Why
//
//  tedn@thuridion.com	  12/22/03		Commented out param in RemoveFromAutoExec
//										to remove compiler warning.
//
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "shutdown.h"
#include "errorlog.h"


void Error(CString ErrStr, CString ErrorFilePath)
{
  ErrorLog *ELog = new ErrorLog(ErrorFilePath);
  if (ELog) {

		ELog->WriteError(ErrStr);

		delete ELog;
		ELog = NULL;
	}
}


BOOL SystemShutdown(CString ErrorFilePath)
{
  HANDLE hToken;              // handle to process token
  TOKEN_PRIVILEGES tkp;       // pointer to token structure

  BOOL fResult;               // system shutdown flag

  // Get the current process token handle so we can get shutdown
  // privilege.

  if (!OpenProcessToken(GetCurrentProcess(),
          TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
			Error("OpenProcessToken failed.", ErrorFilePath);

  // Get the LUID for shutdown privilege.

  if(!LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,
          &tkp.Privileges[0].Luid))
		  Error("LookupPrivilegeValue failed.", ErrorFilePath);

  tkp.PrivilegeCount = 1;  // one privilege to set
  tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

  // Get shutdown privilege for this process.

  AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
      (PTOKEN_PRIVILEGES) NULL, 0);

  // Cannot test the return value of AdjustTokenPrivileges.

  if (GetLastError() != ERROR_SUCCESS)
		Error("AdjustTokenPrivileges enable failed.", ErrorFilePath);

  // Display the shutdown dialog box and start the time-out countdown.

  fResult = InitiateSystemShutdown(
      NULL,                                  // shut down local computer
      "Click on the main window and press \
       the Escape key to cancel shutdown.",  // message to user
      20,                                    // time-out period
      TRUE,                                 
      TRUE);                                 // reboot after shutdown

  if (!fResult)
  {
		Error("InitiateSystemShutdown failed.", ErrorFilePath);
  }

  // Disable shutdown privilege.

  tkp.Privileges[0].Attributes = 0;
  AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
          (PTOKEN_PRIVILEGES) NULL, 0);

  if (GetLastError() != ERROR_SUCCESS)
  {
		Error("AdjustTokenPrivileges disable failed.", ErrorFilePath);
  }
  return TRUE;
}
