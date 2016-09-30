/****************************************************************
Uninstav.Cpp
A component of the MacNuke Project
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
Copyright 1997, 2005 (c) Symantec Corp.. All rights reserved.
SYMANTEC CONFIDENTIAL

Author:		Richard Sadowsky
Created:	September 1, 1997

Purpose: 
Contains the WinMain for the AVUNINST program for the "Make
the Break" marketing campaign. Includes command line processing.
****************************************************************/
#include <windows.h>
#include "cmdline.h"
#include "uninstav.h"
#include "uninstall.h"
#include "tchar.h"

BOOL SystemShutdown();
BOOL IsNT();
BOOL gDebug = FALSE;
BOOL gForceReboot = FALSE;
TCHAR OurFullPathSpace[1024];
LPTSTR OurFullPath = OurFullPathSpace;

int FAR PASCAL WinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPSTR   lpszCmdLine,
                       int     nCmdShow)
{
	int ret;
  int SecondRun = FALSE;
	BOOL silent = FALSE;
	unsigned int products = AV_None;

  //Setup our full path to be used for some uninstalls (eg: ibm)
  lstrcpy(OurFullPath, GetCommandLine());
  _tcsupr(OurFullPath);
  LPTSTR ptr = OurFullPath;
  while (*ptr) {
    if (*ptr == ' ') {
      *ptr = 0;
      break;
    }
  	ptr = CharNext(ptr);
  }

  //Determine if we are uninstalling IBM (and we are on the second run)
  if (IsNT()) {
    CRegistryCurrentUser *program= new CRegistryCurrentUser("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows");
    TCHAR tempstr2[1024];
    if (!program->GetRunString(tempstr2))
    {
      //error clearing value
    }
    _tcsupr(tempstr2);
    if (_tcsstr(tempstr2, OurFullPath)) {
      SecondRun = TRUE;
    }
	  delete program;
  }

  //If nothing
  if (!lpszCmdLine[0]) {
    silent = TRUE;
  	products |= AV_IBM;
  }

  CCommandLine commandLine(lpszCmdLine, TRUE);
	
	while (commandLine.AreMoreSwitches())
	{
		TCHAR param[MaxString];
		TCHAR command = commandLine.GetNextSwitch(param);
		switch (command)
		{
			case 'A' :
				products = AV_All;
				break;
			case 'M':
				products |= AV_McAfee;
				break;
			case 'I' : 
				products |= AV_IBM;
				break;

			case 'T' :
				products |= AV_TrendMicro;
				break;
			case 'N' :
				products |= AV_Innoculan;
				break;
			case 'F' :
				products |= AV_FPROT;
				break;
			case 'C' :
				products |= AV_Cheyenne;
				break;
			case 'O' :
				products |= AV_DrSolomon;
				break;
			case 'S' :
				silent = TRUE;
				break;
			case 'D' : 
				gDebug = TRUE;
				break;
			case 'R' :
				gForceReboot = TRUE;
				break;
			case '*' :
				RunAfterReboot(products, param);
        ret = -1;
				return ret;
		}
	}

  if (IsNT()) {
		ret = WinNTUninstallAV(products, silent, SecondRun);
    if (!SecondRun) {
	    if ((products & AV_IBM) > 0)
	    {
        if (SystemShutdown()) {
        }
      }
    }
  }	else {
		ret = Win95UninstallAV(products, silent);
  }


	return ret;
}

BOOL IsNT()
{
	OSVERSIONINFO os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	BOOL result = GetVersionEx(&os);
	//SAVASSERT(result != 0);
	return (os.dwPlatformId == VER_PLATFORM_WIN32_NT);
}
