// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

#include "stdafx.h"
#include "install.h"
#include "debug.h"

static HINSTANCE s_hInst;                               // current instance
static const DWORD c_max_path = MAX_PATH*2;

typedef BOOL (* NSE_INSTALLPROC) (LONG lInit);

void HookNotes(void)
{
	static TCHAR	szSourceFile[c_max_path];
	static TCHAR	szDestFile[c_max_path];
	static TCHAR	szNotesDir[c_max_path];
	static TCHAR	szNavDir[c_max_path];

	DebugOut(SM_GENERAL, "NLNHOOK: checking if Notes is installed...");

	// check if notes is not installed, exit!
	if (GetNotesInstallPath(szNotesDir, c_max_path) == NOTES_NOT_INSTALLED)
	{
		DebugOut(SM_GENERAL, "NLNHOOK: Notes is NOT installed; hook skipped");
		return;
	}


	// get the dest file (the nlnvp.dll in the notes dir)
	_tcscpy(szDestFile, szNotesDir);
	_tcscat(szDestFile, "\\");
	_tcscat(szDestFile, "nlnvp.dll");

	DebugOut(SM_GENERAL, "NLNHOOK: copy to '%s'", szDestFile);

    if (GetFileAttributes(szDestFile) != INVALID_FILE_ATTRIBUTES)
	{
		TCHAR szLibrary[c_max_path];
		HINSTANCE hInst	= NULL;
		NSE_INSTALLPROC pfnNSE_Install = NULL;

		_tcscpy(szLibrary, szNotesDir);
		_tcscat(szLibrary, "\\");
		_tcscat(szLibrary, "nlnvp.dll");

		SetCurrentDirectory(szNotesDir);

		DebugOut(SM_GENERAL, "NLNHOOK: loading DLL '%s'", szLibrary);

		if (hInst = LoadLibrary(szLibrary))
		{
			if (pfnNSE_Install = (NSE_INSTALLPROC)GetProcAddress(hInst, "NSE_Install"))
			{
				DebugOut(SM_GENERAL, "NLNHOOK: hooking Notes...");
				DWORD dwRet = pfnNSE_Install(TRUE);
				DebugOut(SM_GENERAL, "NLNHOOK: hook returned %d.", dwRet);
			}

			FreeLibrary(hInst);
		}
		else
		{
			DebugOut(SM_ERROR, "ERROR: NLNHOOK: unable to load library; hook skipped");
		}

		SetCurrentDirectory(szNavDir);
	}
	else
	{
		DebugOut(SM_ERROR, "ERROR: NLNHOOK: unable to copy file; hook skipped");
	}

	return;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    hPrevInstance;
    nCmdShow;

    s_hInst = hInstance;
	g_debug = TRUE;
	HookNotes();

    return 0;
}
