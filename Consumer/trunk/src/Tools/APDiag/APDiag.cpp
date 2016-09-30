////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// APDiag.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

HINSTANCE g_hInstance;
CLog g_log;
COSInfo g_OSInfo;
CNAVInfo g_NAVInfo;
CCommonClientInfo g_CCInfo;
bool g_bReboot = false;
bool g_bRestartNavapsvc = false; // Flag to indicate the service needs restarting

void ShowErrorMsg(UINT uID)
{
	TCHAR szMsg[MAX_PATH*2] = {0};
	TCHAR szTitle[50] = {0};

	LoadString(g_hInstance, IDS_TITLE, szTitle, 50);
	LoadString(g_hInstance, uID, szMsg, MAX_PATH*2);
	MessageBox(NULL, szMsg, szTitle, MB_OK | MB_ICONERROR);
}

/////////////////////////////////////////////////////////////
//Functions to check the NAV version
bool CheckNavVersion();
typedef struct VERSION
{
	int nMajor;
	int nMinor;
	int nBuild;
} *PVERSION;

BOOL ConvertNavVerStr(LPTSTR szVerStr, PVERSION pstVer);
int CompareNavVersions(PVERSION pstVer, PVERSION pstVer2);

// End NAV version functions
/////////////////////////////////////////////////////////////

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	g_hInstance = hInstance;

	// Create a log file to dump all logging to
	g_log.CreateLog(_T("diagnostic.log"));
	g_log.Log(_T("=== Log Begin ==="));

	// Check to make sure this is NT
	if ( !g_OSInfo.IsWinNT() )
	{
		g_log.Log(_T("Error: This is not an NT platform. Bailing Out"));
		ShowErrorMsg(IDS_NOT_NT);
		return -1;
	}

	// Check for administrative rights
	if( !g_OSInfo.IsAdminProcess() )
	{
		g_log.Log(_T("Error: User is not running as an administrator. Bailing Out"));
		ShowErrorMsg(IDS_NOT_ADMIN);
		return -1;
	}

	// Make sure the version of NAV is kosher
	if( !CheckNavVersion() )
	{
		g_log.Log(_T("Error: NAV version checking failed. Bailing Out"));
		return -1;
	}

	// Check Symevent
	CCheckSymevent Symevent;
	if( !Symevent.Verify() )
	{
		g_log.Log(_T("Error: The verification for Symevent failed. Attempting re-installation"));
		Symevent.Install();

		// Try again
		if( !Symevent.Verify() )
		{
			g_log.Log(_T("Error: The verification for Symevent failed after re-installation. Bailing Out"));
			return -1;
		}

		// Set the reboot flag and restart service flag
		g_bReboot = true;
		g_bRestartNavapsvc = true;
	}

	CCheckNaveng Naveng;
	if( !Naveng.Verify() )
	{
		g_log.Log(_T("Error: The verification for Naveng failed. Attempting re-installation"));
		Naveng.Install();

		// Try again
		if( !Naveng.Verify() )
		{
			g_log.Log(_T("Error: The verification for Naveng failed after re-installation. Bailing Out"));
			return -1;
		}

		// Set the restart service flag
		g_bRestartNavapsvc = true;
	}

	CCheckNavex Navex;
	if( !Navex.Verify() )
	{
		g_log.Log(_T("Error: The verification for Navex failed. Attempting re-installation"));
		Navex.Install();

		// Try again
		if( !Navex.Verify() )
		{
			g_log.Log(_T("Error: The verification for Navex failed after re-installation. Bailing Out"));
			return -1;
		}

		// Set the restart service flag
		g_bRestartNavapsvc = true;
	}

    CCheckSavrt Savrt;
	if( !Savrt.Verify() )
	{
		g_log.Log(_T("Error: The verification for Savrt failed. Attempting re-installation"));
		Savrt.Install();

		// Try again
		if( !Savrt.Verify() )
		{
			g_log.Log(_T("Error: The verification for Savrt failed after re-installation. Bailing Out"));
			return -1;
		}

		// Set the restart service flag
		g_bRestartNavapsvc = true;
	}

    CCheckSavrtPel SavrtPel;
	if( !SavrtPel.Verify() )
	{
		g_log.Log(_T("Error: The verification for SavrtPel failed. Attempting re-installation"));
		SavrtPel.Install();

		// Try again
		if( !SavrtPel.Verify() )
		{
			g_log.Log(_T("Error: The verification for SavrtPel failed after re-installation. Bailing Out"));
			return -1;
		}

		// Set the reboot flag
		g_bReboot = true;
	}

	CCheckNavapsvc Navapsvc;
	if( !Navapsvc.Verify() )
	{
		g_log.Log(_T("Error: The verification for Navapsvc failed. Attempting re-installation"));
		Navapsvc.Install();

		// Try again
		if( !Navapsvc.Verify() )
		{
			g_log.Log(_T("Error: The verification for Navapsvc failed after re-installation. Bailing Out"));
			return -1;
		}

		// Set the restart service flag
		g_bRestartNavapsvc = true;
	}

	if( !CheckAPFiles() )
	{
		g_log.Log(_T("Error: The verification for Navapw32.dll and Navapscr.dll failed."));
	}

	if( !IsAPServiceRunning() )
	{
		Navapsvc.StartService();
	}

	if( !IsAPAgentRunning() )
	{
		// The Agent is not running, is ccapp?
		HWND hWnd = NULL;

		hWnd = FindWindow("ccAppWindow", NULL);
		if (hWnd == NULL)
		{
			g_log.Log(_T("Error: No ccapp window was found. Ccapp.exe is not running"));
		}
		g_log.Log(_T("The ccapp windows was found. Ccapp.exe is running"));
	}

	if( g_bRestartNavapsvc )
	{
		if( !Navapsvc.StopService() )
			g_log.Log(_T("Error: Unable to stop navapsvc"));
		if( !Navapsvc.StartService() )
			g_log.Log(_T("Error: Unable to start navapsvc"));
		else
			g_log.Log(_T("Successfully started navapsvc"));
	}

	if( g_bReboot )
	{
		TCHAR szMsg[MAX_PATH*2] = {0};
		TCHAR szTitle[50] = {0};

		LoadString(g_hInstance, IDS_TITLE, szTitle, 50);
		LoadString(g_hInstance, IDS_NEED_REBOOT, szMsg, MAX_PATH*2);
		MessageBox(NULL, szMsg, szTitle, MB_OK | MB_ICONERROR);
	}

	g_log.Log(_T("=== Log End ==="));
	return 0;
}


/*
This checks version.dat to ensure the version of NAV is 9.00.67 or higher
It will also check cfgwiz.exe for versioning if the version.dat check fails
*/
bool CheckNavVersion()
{
//[Versions]
//PublicVersion=9.00
//PublicRevision=9.00.67
//InternalVersion=9.00
//InternalRevision=9.00.67

	TCHAR szNavDir[MAX_PATH] = {0};
	_tcscpy(szNavDir, g_NAVInfo.GetNAVDir());

	if(!_tcslen(szNavDir))
	{
		// Could not get the installed apps key recommend a re-install
		g_log.Log(_T("Error: Unable to get the installed apps NAV key!"));
		ShowErrorMsg(IDS_REINSTALL_MISSING_COMPONENTS);
		return false;
	}

	// Get version.dat
	TCHAR szVersionDat[MAX_PATH] = {0};
	_tcscpy(szVersionDat,szNavDir);
	_tcscat(szVersionDat, _T("\\version.dat"));

	// Check for a beta/pre-release version
	TCHAR szMinVer[MAX_PATH] = {0};
	TCHAR szVersion[MAX_PATH] = {0};
	TCHAR szCfgWizVer[MAX_PATH] = {0};

	GetPrivateProfileString(_T("Versions"), _T("PublicRevision"), _T(""), szVersion, MAX_PATH, szVersionDat);
	LoadString(g_hInstance, IDS_MIN_VER, szMinVer, MAX_PATH);

	VERSION vMin, vCur, vCfgWiz;

	// we have a version so let's compare
	if(ConvertNavVerStr(szVersion, &vCur) && ConvertNavVerStr(szMinVer, &vMin))
	{
		if(0 < CompareNavVersions(&vCur, &vMin))
		{
			// Version.dat reports an older version, now check cfgwiz.exe to be sure
			g_log.Log(_T("Error: The version in version.dat is %s. Now checking the version of cfgwiz.exe"),szVersion);

			TCHAR szCfgWizEXE[MAX_PATH] = {0};
			_tcscpy(szCfgWizEXE,szNavDir);
			_tcscat(szCfgWizEXE, _T("\\CfgWiz.exe"));
			if( -1 == GetFileAttributes(szCfgWizEXE) )
			{
				// CfgWiz.exe does not exist!!!
				g_log.Log(_T("Error: CfgWiz.exe does not exist!"));
				ShowErrorMsg(IDS_REINSTALL_MISSING_COMPONENTS);
				return false;
			}

			if( !GetFileVersion(szCfgWizEXE, szCfgWizVer, MAX_PATH) )
			{
				g_log.Log(_T("Error: Failed to get the version info for cfgwiz.exe!"));
				return false;
			}

			// Convert the cfgwiz version to a Version struct
			if( !ConvertNavVerStr(szCfgWizVer, &vCfgWiz) )
			{
				g_log.Log(_T("Error: Failed to convert the cfgwiz.exe version number! (%s)."),szCfgWizVer);
				return false;
			}

			// If major version is not 9 then fail the check
			if( vCfgWiz.nMajor < vCur.nMajor )
			{
				g_log.Log(_T("Error: The major version of CfgWiz.exe is: %n"), vCfgWiz.nMajor);
				ShowErrorMsg(IDS_NOT_NAV_2003);
				return false;
			}

			// So we know it's 9.xx.xx or higher so check the rest of the version with
			// the minimum version to see if it is pre-release
			if(0 < CompareNavVersions(&vCfgWiz, &vMin))
			{
				g_log.Log(_T("Error: The version of CfgWiz.exe is: %s. This is determined to be a pre-release or beta build of NAV."), szCfgWizVer);
				ShowErrorMsg(IDS_BETA_VERSION);
				return false;
			}

			//
			// This means they really have the correct version so we need to update version.dat
			//
			g_log.Log(_T("The version of cfgwiz.exe: %s, is a released version, updating the version.dat to reflect this version."), szCfgWizVer);
			WritePrivateProfileString(_T("Versions"), _T("PublicRevision"), szCfgWizVer, szVersionDat);
			WritePrivateProfileString(_T("Versions"), _T("InternalRevision"), szCfgWizVer, szVersionDat);
			return true;
		}

		return true;
	}
	else
	{
		g_log.Log(_T("Error: Unable to convert the version from version.dat: %s to a version struct. Allowing execution to continue."),szVersion);
	}

	return true;
}

// Converts version string of form "w.xy.z" to three int's.
BOOL ConvertNavVerStr(LPTSTR szVerStr, PVERSION pstVer)
{
	TCHAR szVer[3][MAX_PATH] = {0};
	int nPart = 0, nChar = 0;
	BOOL bDone = FALSE;
	TCHAR szPiece[MAX_PATH] = {0};

	LPTSTR szTemp = szVerStr;

	// Step thrugh the string
	while (!bDone)
	{
		// '.' and NULL are separators (NULL for the last part).
		if ((szTemp[0] == '.') || (szTemp[0] == NULL))
		{
			strcpy(szVer[nPart], szPiece);
			if (++nPart == 3)
				bDone = TRUE;
			else
			{
				szTemp++;
				nChar=0;
				ZeroMemory(szPiece, sizeof(szPiece));
			}
			continue;
		}
		szPiece[nChar] = szTemp[0];
		nChar++;
		szTemp++;
	}

	pstVer->nMajor = atoi(szVer[0]);
	pstVer->nMinor = atoi(szVer[1]);
	pstVer->nBuild = atoi(szVer[2]);

	if(!pstVer->nMajor && !pstVer->nMinor && !pstVer->nBuild)
		return FALSE;

	return TRUE;
}

// Compares two NAV versions
int CompareNavVersions(PVERSION pstVer, PVERSION pstVer2)
{
	if(pstVer->nMajor < pstVer2->nMajor)
		return 1;
	else if(pstVer->nMajor > pstVer2->nMajor)
		return -1;
	if(pstVer->nMinor < pstVer2->nMinor)
		return 1;
	else if(pstVer->nMinor > pstVer2->nMinor)
		return -1;
	if(pstVer->nBuild < pstVer2->nBuild)
		return 1;
	else if(pstVer->nBuild > pstVer2->nBuild)
		return -1;

	return 0;
}
