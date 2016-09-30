// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#include "notesext.h"
#include "noteswatch.h"
#include "install.h"
#include "SymSaferRegistry.h"
#include "vpstrutils.h"

#define INITIIDS
#include "ccVerifyTrustLoader.h"
#include "ccLibDllLink.h"
#include "ccTrace.h"
#include "ccSymDebugOutput.h"

static ccSym::CDebugOutput  debugOutput(_T("NotesWatch"));
IMPLEMENT_CCTRACE(debugOutput);

static unsigned thrdId = 0;
static const c_max_path = MAX_PATH*2;
extern PSSFUNCTIONS pSSFunctions;

/*---------------------------------------------------------------------------*/

// Determine if the specified file is trusted. Copied and adapted from
// ccSymInterfaceLoader.h, CSymInterfaceTrustedLoader class.

static bool IsTrustedFile (const TCHAR *szPath)
{
	if (szPath == NULL)
		return false;

    ccVerifyTrust::ccVerifyTrust_IVerifyTrust objVerifyTrustLoader;
    ccVerifyTrust::CVerifyTrustPtr ptrVerifyTrust;

    // Load and create the IVerifyTrust interface.
    // If we can not create the trust validation object we will let the load continue.
    if (SYM_SUCCEEDED (objVerifyTrustLoader.CreateObject(ptrVerifyTrust.m_p)) && ptrVerifyTrust != NULL)
    {
        if (ptrVerifyTrust->Create(true /*bLoadTrustPolicy*/) != ccVerifyTrust::eNoError)
            ptrVerifyTrust.Release();
    }

	// Make sure file exists.
	if (::GetFileAttributes (szPath) == INVALID_FILE_ATTRIBUTES)
		return false;

	// Test for valid symantec digital signature.
    if (ptrVerifyTrust != NULL)
    {
		// Only fail in case of eVerifyError
        // We let all other errors continue because eVerifyError is the only guarantee that 
        // the validation did indeed fail and it was not a subsystem or other error
        if (ptrVerifyTrust->VerifyFile (szPath, ccVerifyTrust::eSymantecSignature) == ccVerifyTrust::eVerifyError)
            return false;
    }

	return true;
}

typedef BOOL (* NSE_INSTALLPROC) (LONG lInit);

void CopyNotesDLLToNotesDir(void)
{
	static TCHAR	szSourceFile[c_max_path];
	static TCHAR	szDestFile[c_max_path];
	static TCHAR	szNotesDir[c_max_path];
	static TCHAR	szNavDir[c_max_path];

	TCHAR			*pChar;

	// check if notes is not installed, exit!
	if (GetNotesInstallPath(szNotesDir, c_max_path) == NOTES_NOT_INSTALLED)
	{
		pSSFunctions->dPrintf("NLNHOOK: Notes is NOT installed; hook skipped");
		return;
	}

	// get the source file (the nlnvp.dll file in the nav dir)
	GetModuleFileName(NULL, szNavDir, c_max_path-2);
	szNavDir[c_max_path-2] = 0;
	szNavDir[c_max_path-1] = 0;

	if (!(pChar = _tcsrchr(szNavDir, '\\')))
	{
		pSSFunctions->dPrintf("WARNING: NLNHOOK: unable to determine path; hook skipped");
		return;
	}

	*pChar = 0;

	_tcscpy(szSourceFile, szNavDir);
	_tcscat(szSourceFile, "\\");
	_tcscat(szSourceFile, "nlnvp.dll");

	// get the dest file (the nlnvp.dll in the notes dir)
	_tcscpy(szDestFile, szNotesDir);
	_tcscat(szDestFile, "\\");
	_tcscat(szDestFile, "nlnvp.dll");

	pSSFunctions->dPrintf("NLNHOOK: copy from '%s'", szSourceFile);
	pSSFunctions->dPrintf("NLNHOOK: copy to '%s'", szDestFile);

	// attempt to copy the file...
	// allow overwriting of any previous version
	CopyFile(szSourceFile, szDestFile, FALSE);

	return;
}
/*---------------------------------------------------------------------------*/

void HookNotesIfNeeded( HANDLE userAccessToken )
{
	CopyNotesDLLToNotesDir();

	TCHAR	szNotesDir[c_max_path] = {0};

	// check if notes is not installed, exit!
	if (GetNotesInstallPath(szNotesDir, c_max_path) == NOTES_NOT_INSTALLED)
	{
        pSSFunctions->dPrintf("NLNVP: Notes not installed; hook skipped.\n");
		return;
	}

	// Load up the SAV install directory.
    TCHAR szSavDir[c_max_path] = {0};
	HKEY  hKey = NULL;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szReg_Key_Main, NULL, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
	{
		DWORD dwSize  = sizeof(szSavDir) - sizeof (TCHAR) /*ensure NULL termination*/;
		LONG  lResult = SymSaferRegQueryValueEx(hKey, szReg_Val_HomeDir, NULL, NULL, (LPBYTE)szSavDir, &dwSize);
		RegCloseKey(hKey);
        if (lResult != ERROR_SUCCESS)
		{
            pSSFunctions->dPrintf("ERROR: NLNVP: error reading registry; hook skipped.\n");
            return;
		}
	}

    // construct a non-quoted full path to nlnhook.exe; IsTrustedFile() cannot deal with quoted paths
    TCHAR szCmdLine[c_max_path];
    vpstrncpy        (szCmdLine, szSavDir,          sizeof (szCmdLine));
    vpstrnappendfile (szCmdLine, _T("nlnhook.exe"), sizeof (szCmdLine));
    
    // ensure that the file has a Symantec digital signature before launching it
    if (IsTrustedFile(szCmdLine) == false)
	{
        pSSFunctions->dPrintf("ERROR: NLNVP: untrusted file; hook skipped.\n");
        return;
	}

    // construct a quoted full path to nlnhook.exe
	// the quoting ensures that CreateProcess() only launches our exe and not something else
    szCmdLine[0] = _T('\"');
    szCmdLine[1] = _T('\0');
    vpstrnappend     (szCmdLine, szSavDir,             sizeof (szCmdLine));
    vpstrnappendfile (szCmdLine, _T("nlnhook.exe\""),  sizeof (szCmdLine));

    STARTUPINFO			stStartupInfo = {0};
    PROCESS_INFORMATION stProcessInfo = {0};

    stStartupInfo.cb = sizeof (stStartupInfo);

    pSSFunctions->dPrintf("NLNVP: launching hook...\n");

	// actually launch the hook, but don't wait for it to finish.
    if (userAccessToken != NULL)
    {
        if (CreateProcessAsUser(userAccessToken, NULL, szCmdLine, NULL, NULL, FALSE,
                           NORMAL_PRIORITY_CLASS, NULL, NULL, &stStartupInfo, &stProcessInfo))
        {
            CloseHandle (stProcessInfo.hThread);
            stProcessInfo.hThread = NULL;
            CloseHandle (stProcessInfo.hProcess);
            stProcessInfo.hProcess = NULL;
        }
        else
        {
            pSSFunctions->dPrintf("ERROR: hook launched failed, code %d.\n", GetLastError());
        }
    }
    else
    {
        if (CreateProcess (NULL, szCmdLine, NULL, NULL, FALSE,
                           NORMAL_PRIORITY_CLASS, NULL, NULL, &stStartupInfo, &stProcessInfo))
        {
            CloseHandle (stProcessInfo.hThread);
            stProcessInfo.hThread = NULL;
            CloseHandle (stProcessInfo.hProcess);
            stProcessInfo.hProcess = NULL;
        }
        else
        {
            pSSFunctions->dPrintf("ERROR: hook launched failed, code %d.\n", GetLastError());
        }
    }

	return;
}

/*---------------------------------------------------------------------------*/

unsigned __stdcall watchTimer(void *unused)
{
	HKEY hkey = NULL;
	DWORD cbData = 0;
	DWORD minutes = 480;
	DWORD signal; 
	BOOL bGetOut = FALSE;

	DebugOut(SM_GENERAL, "NLNVP: NotesWatch started...");

	// sleep for three minutes so that someone can edit the registry key if desired...
	Sleep(3*60*1000);

	// begin our loop...
	while (!bGetOut)
		{
		__try
			{
			cbData = sizeof(minutes);

			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGHEADER"\\Storages\\LotusNotes", 0, KEY_READ, &hkey) == ERROR_SUCCESS)
				{

				if (SymSaferRegQueryValueEx(hkey, _T("NotesWatch"), NULL, NULL, (LPBYTE)&minutes, &cbData) != ERROR_SUCCESS)
					minutes = 480;

				RegCloseKey(hkey);
				hkey = NULL;

				// don't allow a delay longer than 24 hours
				if (minutes > 1440)
					minutes = 1440;
				else if (minutes < 3) // or less than three minutes
					minutes = 3;

				// check for Notes installed
				DebugOut(SM_GENERAL, "NLNVP: NotesWatch wakeup...");
                HookNotesIfNeeded(g_hUserAccessToken);
				}

			signal = WaitForSingleObject(g_hShutdown, minutes*60*1000);

			if (signal != WAIT_TIMEOUT)
				bGetOut = TRUE;
			}

		__except(1)
			{
			if (!bGetOut)
				startNotesWatch();
			return 1;
			}
		}

	return 0;
}

/*---------------------------------------------------------------------------*/

void startNotesWatch(void)
{
	HANDLE hThread = 0;

	if (hThread = (HANDLE) _beginthreadex(NULL, 0, watchTimer, NULL, 0, &thrdId))
		CloseHandle(hThread);
}

/*--- end of source ---*/
