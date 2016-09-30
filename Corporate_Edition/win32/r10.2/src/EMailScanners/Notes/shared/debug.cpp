// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#include "debug.h"
#include "SymSaferStrings.h"

DWORD g_debug = 0;

#ifdef SMRTY_DEBUG
void _LogLine(LPSTR line, DWORD level);
char g_szTextBuffer[MAX_SM_MESSAGE];
#endif

/*===========================================================================*/
// Debug functions
/*===========================================================================*/

/*---------------------------------------------------------------------------*/

//	DebugOut is used to output debug messages.  It also takes a format
//  string and a variable number of arguments which may be referenced in the 
//	format string.

#ifdef SMRTY_DEBUG
void DebugOut(DWORD level, LPSTR pszFormat, ...)
{
	va_list args;

	if (g_debug)
		{
		// use dynamic memory since we are multi-threaded
		// this should be changed to used a fixed pool of strings
		// so that memory fragmentation is lessened
		char *szTextOut = (char*)malloc(1024);
		if (szTextOut)
			{
			va_start(args, pszFormat);
			ssvsnprintf(szTextOut, 1024, pszFormat, args);
			_LogLine(szTextOut, level);
			va_end(args);
			free(szTextOut);
			}
		}
}

/*---------------------------------------------------------------------------*/

//	_LogLine sends a WM_COPYDATA message to a well-known window
//	to display debug information.  This window is the main view
//	window of the notes test application.

void _LogLine(char *line, DWORD level)
{
	// converted to use OutputDebugString
	//HWND hWnd = NULL;
	//COPYDATASTRUCT copyData;

	OutputDebugString(line);

	// Try to send the message to the Real Time Hook Viewer.
	// We look for the window each time, just in case the Smrty viewer is closed!
	//hWnd = FindWindow(NULL, _T("Smrty"));

	/*
	if(hWnd)
	{
		copyData.dwData = level;
		copyData.cbData = strlen(line) +1;
		copyData.lpData = line;

		SendMessage(hWnd, WM_COPYDATA, (WPARAM) NULL, (LPARAM) &copyData);
	}
	*/
}
#endif

/*--- end of source ---*/