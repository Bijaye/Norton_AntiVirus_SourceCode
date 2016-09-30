// Symevent.cpp - This file contains the class that verifies Symevent is functioning
//

#include "stdafx.h"
#include "Symevent.h"
#include <shellapi.h>

CCheckSymevent::CCheckSymevent() : CDriverCheck(_T("Symevent"))
{
}

bool CCheckSymevent::Install()
{
	// Install Symevent from the Common Files\Symantec Shared directory
	TCHAR szSevinst[MAX_PATH*2] = {0};
	if( !g_CCInfo.GetCCFolder(szSevinst) )
	{
		g_log.Log(_T("Error - CCheckSymevent::Install - Unable to get the CC Folder"));
		return false;
	}

	_tcscat(szSevinst,_T("\\Sevinst.exe"));

	if( -1 == GetFileAttributes(szSevinst) )
	{
		g_log.Log(_T("Error: CCheckSymevent::Install - Sevinst.exe does not exist here: %s"), szSevinst);
		return false;
	}

	SHELLEXECUTEINFO Info = {0};
	Info.fMask = SEE_MASK_NOCLOSEPROCESS;
	Info.cbSize = sizeof(Info);
	Info.hwnd = NULL;
	Info.lpFile = szSevinst;
	Info.lpParameters = _T("/Q -consumer NAV");
	Info.nShow = SW_SHOWNORMAL;

	ShellExecuteEx(&Info);

	// Wait for it to finish
	while (WaitForSingleObject(Info.hProcess, 1000) == WAIT_TIMEOUT)
	{
		MSG msg = {0};
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		};
	}

	g_log.Log ( _T("CCheckSymevent::Install - Finished running Sevinst.exe") );

    // Check the result of the registration.
    //
    DWORD dwExitCode = 0;
    if ( GetExitCodeProcess ( Info.hProcess, &dwExitCode) )
    {
        g_log.Log ( "CCheckSymevent::Install - Sevinst install returned %d", dwExitCode );
    }

    CloseHandle(Info.hProcess);

	// Now update the registry
	TCHAR szSymeventPath[MAX_PATH] = {0};
	if( FAILED( SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES, NULL, 0, szSymeventPath) ) )
	{
		g_log.Log (_T("Error: CCheckSymevent::Install - Could not get the program files directory when attempting to create the symevent registry keys"));
		return false;
	}
	_tcscat(szSymeventPath, _T("\\Symantec\\Symevent.sys"));

	// Make sure that file exists
	if( -1 == GetFileAttributes(szSymeventPath) )
	{
		g_log.Log (_T("Error: CCheckSymevent::Install - The Symevent.sys file %s does not exist"), szSymeventPath);
		return false;
	}

	TCHAR szImagePath[MAX_PATH] = _T("\\??\\");
	_tcscat(szImagePath, szSymeventPath);

	CDriverCheck::Install(szImagePath, 3, 1);

	return true;
}

bool CCheckSymevent::VerifyStartType()
{
	// Symevent should be set to start manually - 3
	DWORD dwStart = 0;
	if( ERROR_SUCCESS != m_ServiceKey.QueryValue(dwStart, _T("Start")) )
	{
		g_log.Log(_T("Error - CCheckSymevent::VerifyStartType() - The Start key could not be opened for Symevent"));
		return false;
	}

	if( dwStart != 3 )
	{
		g_log.Log(_T("Error - CCheckSymevent::VerifyStartType() - The Start mode for Symevent is not 3 (it is %i). Fixing the start mode"),dwStart);
		if( ERROR_SUCCESS != m_ServiceKey.SetValue(3, _T("Start")) )
		{
			g_log.Log(_T("Error - CCheckSymevent::VerifyStartType() - The Start mode for Symevent could not be set to 3."));
			return false;
		}
		g_log.Log(_T("CCheckSymevent::VerifyStartType() - The Start mode for Symevent was changed to 3."));
	}

	return true;
}

bool CCheckSymevent::ExtraChecks()
{
	// Make sure that NAV is registered with Symevent as a client
	CRegKey NavVDDKey;
	if( ERROR_SUCCESS != NavVDDKey.Open(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Symevent\\VDD\\NAV"), KEY_QUERY_VALUE) )
	{
		g_log.Log(_T("Error - CCheckSymevent::ExtraChecks() - The VDD\\NAV registry key could not be found."));
		return false;
	}

	return true;
}