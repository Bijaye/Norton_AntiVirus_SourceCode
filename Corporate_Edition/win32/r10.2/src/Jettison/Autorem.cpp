//-----------------------------------------------------------------------------
//  AutoRem.Cpp
//  A component of the MacNuke Project
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
//  Copyright 1997, 2005 (c) Symantec Corp.. All rights reserved.
//  SYMANTEC CONFIDENTIAL
//
//  Revision History
//
//  Who                     When        What / Why
//
//  Richard Sadowsky      09/01/97      Created.  
//										Contains code to remove a given program from the Autoexec.bat file.
//
//  tedn@thuridion.com	  12/22/03		Replaced <> with "" around stdafx.h include.
//
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include <stdio.h>
#include "autorem.h"
#include "smalltxt.h"

#define StringHelperMax (1024)

TCHAR GetBootDrive();

int RemoveFromAutoExec(LPCTSTR /*pathname*/, LPCTSTR programName)
{
	int ret = -1;
	TCHAR autoExec[StringHelperMax];
	TCHAR autoExecBak[StringHelperMax];
	const LPCTSTR AutoExecBat = "autoexec.bat";
	const LPCTSTR AutoExecBak = "autoexec.sym";

	autoExec[0] = GetBootDrive();
	autoExec[1] = ':';
	autoExec[2] = '\\';
	autoExec[3] = '\0';
	lstrcpy(autoExecBak, autoExec);
	lstrcat(autoExec, AutoExecBat);
	lstrcat(autoExecBak, AutoExecBak);

	remove(autoExecBak);
	rename(autoExec, autoExecBak);

	CSmallTextFile *autoExecFile = new CSmallTextFile(autoExecBak);
	HANDLE outFile = CreateFile(autoExec, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if ((outFile == INVALID_HANDLE_VALUE) || (!autoExecFile->IsOpen()))
	{
		rename(autoExecBak, autoExec);
		ret = -2;
	}
	else
	{
		TCHAR line[StringHelperMax];
		LPTSTR checkProgramName = new TCHAR[lstrlen(programName) + 1];
		lstrcpy(checkProgramName, programName);
		CharUpper(checkProgramName);
		while (!autoExecFile->IsEOF())
		{
			autoExecFile->ReadLine(line);
			BOOL removeLine = FALSE;
			LPTSTR checkLine = new TCHAR[lstrlen(line) + 1];
			lstrcpy(checkLine, line);
			CharUpper(checkLine);
			LPTSTR p1 = strstr(checkLine, checkProgramName);
			if (p1 != NULL)
				removeLine = TRUE;
			if (!removeLine)
			{
				DWORD written;
				lstrcat(line, "\r\n");
				WriteFile(outFile, line, lstrlen(line), &written, NULL);
			}
		}
	}
	CloseHandle(outFile);
	delete autoExecFile;
	return ret;
}

TCHAR GetBootDrive()
{
	return 'C'; //RSS!!! Kludgerama
}
