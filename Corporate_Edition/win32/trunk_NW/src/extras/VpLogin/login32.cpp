// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//
//	Login32.c
//		Source for 32 bit login program for Symantec AntiVirus Corporate Edition
//		This is called from the logon scripts.
//
//	Evan Simper April 1997 - Created.
//  Brian Aljian December 2002 - Converted from a console application to a
//                               Windows application.  Removed old scan code
//                               because this program is no longer used for
//                               logon scanning.
//
//  DAllee Dec 2003 - Commented out Administrator checks.
//                    Install will now be attempted on NT regardless of user account's permissions.
//                    This is to handle the various cases where normal users can run install with
//                    elevated privileges. Elevated privileges may be granted on a machine, user, or
//                    install package basis -- disabling checking here and relying on install to
//                    check for the correct privileges.
//                    Prior code left for reference.

#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <io.h>
#include <time.h>
#include "resource.h"

#include "nts.h"
#include "symnts.h"
#include "vpstrutils.h"
#include "SymSaferRegistry.h"
#include "DarwinResCommon.h"

#ifdef __BORLANDC__
#define _access access
#define _strupr strupr
#endif

#define NO_ADMIN_RIGHTS     4
#define SERVER_INSTALLED	3
#define NOT_INSTALLED_YET	2
#define NEED_UPDATE			1
#define CURRENT_VERSION		0

#define VERSION_KEY			"SOFTWARE\\Intel\\LANDesk\\VirusProtect6\\CurrentVersion"
#define VERSION_VALUE		"ProductVersion"

#define CLIENTTYPE_KEY							"ClientType"
#define CLIENTTYPE_CONNECTED					0x00000001
#define CLIENTTYPE_STANDALONE					0x00000002
#define CLIENTTYPE_SERVER						0x00000004

#define MAX_PARAMETERS 10

#define LOGIN_LOG			"c:\\NAV_LOG.TXT"
//
// Environment variable definitions for NT
// NOTE: These are case sensitive!
//
#define IDS_NT_COMSPEC          "ComSpec"
#define IDS_DOS_COMSPEC         "COMSPEC"
#define IDS_NT_COMPROC			"CMD.EXE"
#define IDS_DOS_COMPROC			"COMMAND.COM"

#define dprintf if (debug) RealDprintf
#define TIME_STAMP(m) {if(debug) {time_t t=time(NULL);	dprintf("%s: %s",m,ctime(&t)); } }

DARWIN_DECLARE_RESOURCE_LOADER(_T("vp_log32Res.dll"), _T("vp_log32"))

int CheckInstalled(int NewVersion);
BOOL AskAbout (WORD wMsgID);
BOOL LaunchProgram(LPTSTR szFile);
BOOL ReadIniFile (char *inifile);
int CheckForNT(void);
void PerformInstall(void);
BOOL IsServerInstalled();
//DWORD IsUserAdmin( BOOL *pbAdminUser ) ;

void gcbPrintf(WORD wMsgID, ...);
void RealDprintf(char *format,...);
void PrintVersionString(char * string, DWORD version);

void ParseCommandLine(LPSTR lpCmdLine);
void StartInstall(void *params);
void ErrorMessage(LPTSTR szErrorMessage);
INT_PTR CALLBACK OutputDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

HINSTANCE g_hInst = NULL;		// Current instance
HWND g_hOutputWnd = NULL;		// Handle to debug dialog box
HWND g_list = NULL;				// Handle to list control in debug dialog box
int g_argc = 0;					// Argument count
char *g_argv[MAX_PARAMETERS];	// Argument list
char g_iniPath[MAX_PATH];		// Path to ini file
HANDLE g_hYesEvent = NULL;		// Signalled event when Yes button is pressed.
HANDLE g_hNoEvent = NULL;		// Signalled event when No button is pressed
BOOL g_bIsSAVDeploy = TRUE;		// By default, deploying a SAV install

BOOL bSilentW32Inst=TRUE;
int NewestVersion=0,Installed=0, debug=0,logging=0;
char InstallOp[20],InstallProg[MAX_PATH];
char OSType[10], szPath[MAX_PATH];

// dwr, 1-20V18W, additional options
char SilentInstallOp[32];
void HandleForceInstall(BOOL bSilentW32Inst);

///////////////////////////////////////////////////////////////////////////////
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	HWND hWnd = NULL;
	MSG msg;
	BOOL bRet;
	ULONG hThread;

	if(SUCCEEDED(g_ResLoader.Initialize()))
	{
		g_hInst = g_ResLoader.GetResourceInstance();
	}
	else
	{
		return FALSE;
	}

	// Create dialog box for messages
	g_hOutputWnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_OUTPUT), NULL, (DLGPROC)OutputDialogProc);
	if (g_hOutputWnd)
	{
		g_list = GetDlgItem( g_hOutputWnd, IDC_LIST);
		if (g_list)
		{
			ShowWindow(g_hOutputWnd, SW_SHOW);
		}
		else
		{
			ErrorMessage("Could not get handle to list control.");
			DestroyWindow(g_hOutputWnd);
			return 0;
		}

		if (!g_bIsSAVDeploy)
		{
			char szTemp[256];
			LoadString(g_hInst, IDS_WIN_TITLE_SCS, szTemp, 256);
			SetWindowText(g_hOutputWnd, szTemp);
		}
	}
	else
	{
		ErrorMessage("Could not open window.");
		return 0;
	}

	// Disable all the buttons for now
	EnableWindow(GetDlgItem(g_hOutputWnd, IDC_BUTTON_YES), FALSE);
	EnableWindow(GetDlgItem(g_hOutputWnd, IDC_BUTTON_NO), FALSE);
	EnableWindow(GetDlgItem(g_hOutputWnd, IDC_BUTTON_CLOSE), FALSE);

	g_hYesEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	g_hNoEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (!g_hYesEvent || !g_hNoEvent)
	{
		ErrorMessage("Could not create event handles.");
		DestroyWindow(g_hOutputWnd);
		return 0;
	}

	ParseCommandLine(lpCmdLine);

	// Start a thread to do the actual install work
	hThread = NTxBeginThread(StartInstall, NULL);
	if (hThread == -1)
	{
		ErrorMessage("Could not start install thread.");
		DestroyWindow(g_hOutputWnd);
		return 0;
	}

	// Main message loop
	while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (bRet == -1)
		{
			return 0;
		}
		else if (!IsWindow(g_hOutputWnd) || !IsDialogMessage(g_hOutputWnd, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	CloseHandle(g_hYesEvent);
	CloseHandle(g_hNoEvent);

	return msg.wParam;
}

void ParseCommandLine(LPSTR lpCmdLine)
{
	int		a;
	int		j;
	char	*chptr;
	char	szBuf[MAX_PATH];
	char	seps[] = " \t";
	char	*token = NULL;

	// Process up to MAX_PARAMETERS input parameters.  Convert them to standard
	// argc/argv format.  This is so we don't have to disturb the existing
	// argument processing below when converting from a console application to
	// a windows application.
	if (lpCmdLine)
	{
		token = strtok(lpCmdLine, seps);
		while ((token != NULL) && (g_argc < 10))
		{
			a = strlen(token);
			g_argv[g_argc] = (char*)malloc(a+1);
			memset(g_argv[g_argc], 0, a+1);
			strcpy(g_argv[g_argc], token);
			g_argc++;
			token = strtok(NULL, seps);
		}
	}

	memset (OSType,0,10);
	memset (InstallOp,0,sizeof (InstallOp));
	memset (InstallProg,0,sizeof (InstallProg));
	szPath[0] = '\0';

	if (g_argc > 0)
	{
		for (a=0; a < g_argc; a++)
		{
			vpstrncpy(szBuf, g_argv[a], sizeof (szBuf));
			strupr(szBuf);
			j = 0;
			if (szBuf[0] == '/')
				j++;

			switch (szBuf[j])
			{
				case 'P':
					chptr = strchr(&g_argv[a][j], '=');
					if (chptr)
                    {
						chptr++;
						vpstrncpy   (szPath,    chptr,           sizeof (szPath));
						vpstrncpy   (g_iniPath, szPath,          sizeof (szPath));
						vpstrnappend(g_iniPath,"\\VP_LOGIN.INI", sizeof (szPath));
						if (_access(szPath,0))
							szPath[0] = '\0';
						}
					break;

				case 'D': //debug
					if(!strnicmp( &g_argv[a][j], "DEBUG", strlen(&g_argv[a][j])))
						debug = 1;
					break;

				case 'L': // log
					if(!strnicmp( &g_argv[a][j], "LOG", strlen(&g_argv[a][j])))
						logging = 1;
					unlink (LOGIN_LOG);
					break;
			}
		}
	}
}

void StartInstall(void *params)
{
	TIME_STAMP("Starting Login processing");

	if ( CheckForNT())					// see if it's Win NT
		strcpy (OSType,"WinNT");
	else
		strcpy (OSType,"Win95");

	dprintf("OSType is %s",OSType);
	
	if (szPath[0] == '\0')
	{
		char szTemp[256];

		// No path was specified.  Pop up error message.
		LoadString(g_hInst, IDS_SPECIFY_PATH, szTemp, 256);
		dprintf(szTemp);
		ErrorMessage(szTemp);
		goto done;
	}

	dprintf("Path to VP_LOGIN.INI  = [%s]", g_iniPath);

	if (!ReadIniFile(g_iniPath))
	{
		char szTemp[256];
		char szBuffer[256];

		// INI file could not be found in given path
		LoadString(g_hInst, IDS_COULDNT_FIND, szTemp, 256);
		sssnprintf(szBuffer, sizeof(szBuffer), szTemp, g_iniPath);
		dprintf(szBuffer);
		ErrorMessage(szBuffer);
		goto done;
	}

	// Do the install
	PerformInstall();

done:
	TIME_STAMP("Done processing Login");

	if (debug)
	{
		// Leave the window open if debug=1 so you can see the messages.
		EnableWindow(GetDlgItem(g_hOutputWnd, IDC_BUTTON_CLOSE), TRUE);
		SendMessage(g_list, LB_ADDSTRING, 0, (LPARAM) "Close this window when finished.");
	}
	else
	{
		SendMessage(g_hOutputWnd, WM_CLOSE, 0, 0);
	}

	_endthread();
}

void PerformInstall(void)
{
	if (debug)
	{
		switch(Installed)
		{
			case CURRENT_VERSION:
				dprintf("Installed Value = [%u] --> Client is Current",Installed);
				break;
			case NEED_UPDATE:
				dprintf("Installed Value = [%u] --> Client needs an update",Installed);
				break;
			case NOT_INSTALLED_YET:
				dprintf("Installed Value = [%u] --> Client is not installed",Installed);
				break;
			case SERVER_INSTALLED:
				dprintf ( "Installed Value = [%u] --> Server is already installed", Installed );
				break;
			case NO_ADMIN_RIGHTS:
				dprintf ( "Installed Value = [%u] --> User does not have Administrator rights", Installed );
				break;
		}

		dprintf("Install Options = [%s]",InstallOp);
	}

	if (_access(InstallProg,0)) // if we can't execute the install program then we can't install
	{
		dprintf("Unable to access the Install program: %s",InstallProg);
		return;
	}

	switch (Installed)
	{
		case CURRENT_VERSION:
			break;

		case NEED_UPDATE:
			if (g_bIsSAVDeploy)
				gcbPrintf (IDS_LDVP_NOT_CURRENT);
			else gcbPrintf(IDS_LDVP_NOT_CURRENT_SCS);

			if (!strcmp (InstallOp,"FORCE"))			// Force update
			{
				gcbPrintf (IDS_UPDATE_NOW);

				// dwr, 1-20V18W, add new function
				HandleForceInstall(!debug && bSilentW32Inst);
			}
			else if (!strcmp (InstallOp,"OPTIONAL"))	// Ask the user if they want to update
				{
				if (AskAbout (IDS_ASK_UPDATE))
				{
					gcbPrintf (IDS_YES);
					gcbPrintf (IDS_UPDATE_NOW);

					LaunchProgram(InstallProg);
				}
				else
				{
					gcbPrintf (IDS_NO);
					gcbPrintf (IDS_NO_UPDATE);
				}
			}
			else if (!strcmp (InstallOp,"NONE"))		// Inform the user, but do nothing
			{
				dprintf ("Not Updating.");
			}
			else
			{
				dprintf ("UpdateErr:%s",InstallOp);
			}
		break;
			
		case NOT_INSTALLED_YET:
			if (g_bIsSAVDeploy)
				gcbPrintf (IDS_NOT_INSTALLED);
			else gcbPrintf (IDS_NOT_INSTALLED_SCS);
			
			if (!strcmp (InstallOp,"FORCE"))			// Force install
			{
				gcbPrintf (IDS_INSTALL_NOW);
				if( !debug && bSilentW32Inst )
				{
					_tcscat(InstallProg," /s /v\"/qn REBOOT=ReallySuppress\" ");
				}

				LaunchProgram(InstallProg);
			}
			else if (!strcmp (InstallOp,"OPTIONAL"))	// Ask the user if they want to install
			{
				if (AskAbout (IDS_ASK_INSTALL))
				{
					gcbPrintf (IDS_YES);
					gcbPrintf (IDS_INSTALL_NOW);

					LaunchProgram(InstallProg);
				}
				else
				{
					gcbPrintf (IDS_NO);
					gcbPrintf (IDS_NO_INSTALL);
				}
			}
			else if (!strcmp (InstallOp,"NONE"))		// Inform the user, but do nothing
			{
				dprintf ("Not Installing.");
			}
			else
			{
				dprintf ("InstallErr:%s",InstallOp);
			}

			break;

   		case SERVER_INSTALLED:
            // if we were not supposed to install anyway, suppress error
            if (strcmp (InstallOp,"NONE")) 
            {
				if (g_bIsSAVDeploy)
					gcbPrintf ( IDS_SERVER_INSTALLED );
				else gcbPrintf ( IDS_SERVER_INSTALLED_SCS );
                gcbPrintf ( IDS_NO_INSTALL );
				EnableWindow(GetDlgItem(g_hOutputWnd, IDC_BUTTON_CLOSE), TRUE);
                Sleep ( 10000 );
            }

			break;
   
   		case NO_ADMIN_RIGHTS:
            // if we were not supposed to install anyway, suppress error
            if (strcmp (InstallOp,"NONE")) 
            {
				if (g_bIsSAVDeploy)
					gcbPrintf ( IDS_NO_ADMIN_RIGHTS );
				else gcbPrintf ( IDS_NO_ADMIN_RIGHTS_SCS );
                gcbPrintf ( IDS_NO_INSTALL );
				EnableWindow(GetDlgItem(g_hOutputWnd, IDC_BUTTON_CLOSE), TRUE);
                Sleep ( 10000 );
            }

   			break;
	}
}

///////////////////////////////////////////////////////////////////////////////
void HandleForceInstall (BOOL bSilentW32Inst)
{
	BOOL bYesBootNoMsg	= FALSE;
	BOOL bYesBootYesMsg	= FALSE;
	BOOL bNoBootYesMsg	= FALSE;

	// dwr, 1-20V18W, translate boot options
	dprintf ("SilentInstallOp:%s",SilentInstallOp);
	if( !strcmp (SilentInstallOp,		"YESBOOTNOMSG"))
		bYesBootNoMsg = TRUE;
	else if( !strcmp (SilentInstallOp,	"YESBOOTYESMSG"))
		bYesBootYesMsg = TRUE;
	else if( !strcmp (SilentInstallOp,	"NOBOOTYESMSG"))
		bNoBootYesMsg = TRUE;
	else
		bNoBootYesMsg = TRUE;		// new default action

	if( bSilentW32Inst )
	{
		// dwr, 1-20V18W, selective boot options
		if (bYesBootNoMsg || bYesBootYesMsg)
			_tcscat(InstallProg," /s /v\"/qn REBOOT=Force\" ");
		else
			_tcscat(InstallProg," /s /v\"/qn REBOOT=ReallySuppress\" ");
	}

	LaunchProgram(InstallProg);

	// dwr, 1-20V18W, selective message options
	if( bSilentW32Inst )
	{
		char szError[256];
		UINT nError = 0;

		if (bYesBootYesMsg)
			nError = IDS_YES_BOOT_WARN_MSG;
		else if (bNoBootYesMsg)
			nError = IDS_NO_BOOT_WARN_MSG;

		// inform user
		if (0 != nError)
		{
			LoadString(g_hInst, nError, szError, sizeof(szError));
			dprintf(szError);
			ErrorMessage(szError);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
BOOL ReadIniFile (char *inifile)
{
	FILE *ini;
	char *q,line[256];
	int state=0;

	ini=fopen (inifile,"rt");

	// dwr - 1-29WSEK, port of 1-20AN6K below with additional vplogon.bat mods
	if (ini == NULL)
	{
		// 1-20AN6K: Very rarely the file is not accessible right after mapping drive due to network delays.
		// In case that happend sleep 10 seconds before retrying to open the ini file. 
		Sleep(10000);
		ini = fopen(inifile, "rt");
		if(ini == NULL)
			return FALSE;
	}

	while (!feof (ini)) {
		fgets (line,256,ini);
		if (line[0]==';' || line[0]=='#' || line[0]=='\n') continue;
		if (line[strlen(line)-1] == '\n')
			line[strlen(line)-1] = 0;
		while (strlen(line) && (line[strlen(line)-1] == '\t' || line[strlen(line)-1] == ' '))
			line[strlen(line)-1] = 0;
		if (line[0]=='[') {
			if (!strnicmp(line,"[Installer]",11)) 			{ state=1; }
			else if (!strnicmp(line,"[InstallOptions]",15))	{ state=2; }
			else if (!strnicmp(line,"[ClientNumber]",14)) 	{ state=3; }
			else state=0;
			continue;
		}
		q = _tcschr(line,'=');
		if (q) {
			*q=0;
			q++;
			switch (state) {
				case 0: continue;
				case 1:
					if (!stricmp (line,"Win32"))
					{
						strcpy (InstallProg,q);
						dprintf("InstallProg = [%s]",InstallProg);
					
						TCHAR *szLastBackSlash = _tcsrchr(InstallProg, '\\');
						if (szLastBackSlash)
						{
							TCHAR szInstallPath[1024];
							DWORD dwSizeToCopy = _tcslen(InstallProg) - _tcslen(szLastBackSlash);
							_tcsncpy(szInstallPath, InstallProg, dwSizeToCopy);
							szInstallPath[dwSizeToCopy] = '\0';
							TCHAR szSCSInstallMSIFile[256];
							LoadString(g_hInst, IDS_SCS_MSI_FILE, szSCSInstallMSIFile, 256);
							_tcscat(szInstallPath, _T("\\"));
							_tcscat(szInstallPath, szSCSInstallMSIFile);

							if (_access(szInstallPath,0)) // if we can't execute the install program then we can't install
							{
								// Leave as SAV.
							}
							else
							{
								// Found SCS msi
								g_bIsSAVDeploy = FALSE;
								char szTemp[256];
								LoadString(g_hInst, IDS_WIN_TITLE_SCS, szTemp, 256);
								SetWindowText(g_hOutputWnd, szTemp);
							}
						}
					}
					if (!stricmp (line,"Silent"))
					{
                        bSilentW32Inst = atoi(q);
						dprintf("bSilentW32Inst = [%d]",bSilentW32Inst);
					}
					break;
				case 2:
					if (!stricmp(OSType,line))
					{
						strcpy (InstallOp,q);
						dprintf("InstallOp = [%s]",InstallOp);
					}
					else if (!stricmp(line,"SilentInstall"))
					{
						// dwr, 1-20V18W, options to requested user prompt on login script upgrade when boot needed
						strcpy (SilentInstallOp,q);
						dprintf("SilentInstallOp = [%s]",SilentInstallOp);
					}
					break;
				case 3:
					if (!stricmp("BuildNumber",line)) {
						NewestVersion=strtol (q,NULL,16);
						dprintf("BuildVersion = 0x%08x",NewestVersion);
						if(NewestVersion && debug)
						{
							PrintVersionString("Server build = ",NewestVersion);
						}
					}
					break;
			}
		}
	}

	fclose(ini);

	Installed=CheckInstalled(NewestVersion);
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
BOOL AskAbout (WORD wMsgID)
{
	HANDLE hAsk = NULL;
	HANDLE hEvents[2]; 
	DWORD dwRet;
	BOOL bReturn = FALSE;

	gcbPrintf(wMsgID);

	EnableWindow(GetDlgItem(g_hOutputWnd, IDC_BUTTON_YES), TRUE);
	EnableWindow(GetDlgItem(g_hOutputWnd, IDC_BUTTON_NO), TRUE);

	hEvents[0] = g_hYesEvent;
	hEvents[1] = g_hNoEvent;

	dwRet = WaitForMultipleObjects(2, hEvents, FALSE, 20000);
	if (dwRet == WAIT_OBJECT_0)
	{
		bReturn = TRUE;
	}
	else if (dwRet == WAIT_TIMEOUT)
	{
		dprintf("No user input due to timeout.  Assuming No.");
	}

	EnableWindow(GetDlgItem(g_hOutputWnd, IDC_BUTTON_YES), FALSE);
	EnableWindow(GetDlgItem(g_hOutputWnd, IDC_BUTTON_NO), FALSE);

	return bReturn;
}

///////////////////////////////////////////////////////////////////////////////
int CheckInstalled(int NewVersion)
{
	HKEY hkey=0;
	LONG ccode;
	DWORD type=REG_DWORD,size=sizeof(DWORD),version=0,number=0;
    DWORD dwServerProdVersion = 0;
    DWORD dwClientProdVersion = 0;
	DWORD dwServerBuildNum = 0;
    DWORD dwClientBuildNum = 0;

	BOOL bAdminUser = FALSE ;

	// Bail out if we currently have NAVCE Server installed
	if ( TRUE == IsServerInstalled() )
		return SERVER_INSTALLED;

	// open the key where the ProductVersion is stored
	dprintf("Open Key %s",VERSION_KEY);
	ccode=RegOpenKey(HKEY_LOCAL_MACHINE,VERSION_KEY,&hkey);
	dprintf("returned ccode = [%x]",ccode);

	// if we couldn't open the key then assume it hasn't been installed yet
	if (ccode != ERROR_SUCCESS) return NOT_INSTALLED_YET;

	ccode=SymSaferRegQueryValueEx (hkey,"Type",NULL,&type,(LPBYTE)&version,&size);
	if (ccode != ERROR_SUCCESS) return NOT_INSTALLED_YET;

	size=4;
	version=0;
	type=REG_DWORD;
 	// read the value containing the client version current installed on this machine
	ccode=SymSaferRegQueryValueEx (hkey,VERSION_VALUE,NULL,&type,(LPBYTE)&version,&size);
	dprintf("[0x%x]Version installed on Client: 0x%x",ccode,version);
	if(!ccode && version && debug)
	{
		PrintVersionString("Client build = ",version);
	}

    // Obtain productversion+inline number from the version DWORD
    // return update necessary, if theres a newer version on the server
    dwServerProdVersion = LOWORD (NewVersion);
    dwClientProdVersion = LOWORD (version);

    if ( dwServerProdVersion > dwClientProdVersion )
    {
        // Close all registry handles opened at this point
    	RegCloseKey(hkey);
        return NEED_UPDATE;
    }
    
		// (1-GN0GG) if the version numbers match, we also need to look at build numbers
    if ( dwServerProdVersion == dwClientProdVersion )
    {
		dwServerBuildNum = HIWORD (NewVersion);
		dwClientBuildNum = HIWORD (version);

		// some older builds contain a leading '9' in the build number - remove this before comparing
		if (dwServerBuildNum > 9000)
			dwServerBuildNum -= 9000;
		if (dwClientBuildNum > 9000)
			dwClientBuildNum -= 9000;

		if (dwServerBuildNum > dwClientBuildNum)
		{
	        // Close all registry handles opened at this point
	    	RegCloseKey(hkey);
	        return NEED_UPDATE;
 		}
    }

	RegCloseKey(hkey);
    hkey = NULL ;

	// otherwise assume that the most current version is on the server
	return CURRENT_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
BOOL LaunchProgram(LPTSTR szFile)
{
	STARTUPINFO	startupinfo;
	PROCESS_INFORMATION	processinfo;
	char szPath[MAX_PATH];
	char *pDir = NULL;
	char *pStart;
	BOOL ret;
	DWORD dwExitCode=0;


	memset(&startupinfo, 0, sizeof(STARTUPINFO));
	startupinfo.cb = sizeof(STARTUPINFO);

	_tcscpy(szPath,szFile);
	pStart = _tcsrchr(szPath,'\\');
	if (pStart) {
		*pStart = '\0';
		pDir = szPath;
		}


	// The following is the fix for SCR22819 to copy OLEAUT32.DLL to the win95 system directory
	if( pDir ) {
		char szSrcPath[MAX_PATH];
		char szSysPath[MAX_PATH];

		GetSystemDirectory( szSysPath, MAX_PATH );
		strcat( szSysPath, "\\OLEAUT32.DLL" );

		sssnprintf( szSrcPath, sizeof(szSrcPath), "%s\\OLEAUT32.DLL", pDir );

		CopyFile( szSrcPath, szSysPath, TRUE );
	}
	//End of Fix for SCR22819

	dprintf("Launch Setup Program = [%s]",szFile);

	TIME_STAMP("Start Setup");

	// double quote command to prevent possible execution of unexpected program
	if (! _tcschr( szFile, _T('\"')))
		sssnprintf( szPath, sizeof(szPath), "\"%s\"", szFile );
	else
		strcpy( szPath, szFile );

	ret = CreateProcess(NULL, szPath, NULL, NULL, FALSE, 0, NULL, pDir, &startupinfo, &processinfo);
	if (ret) 
	{
		DWORD dwStart = GetTickCount();
		DWORD dwTimeOut = 1000*900;

        // We need to wait until setup.exe process is done with its job
		if (WaitForSingleObject(processinfo.hProcess, dwTimeOut) == WAIT_TIMEOUT)
		{
			dprintf("setup timed out after %d seconds", dwTimeOut/1000);
		}

		TIME_STAMP("Setup program done");

		if(GetExitCodeProcess( processinfo.hProcess, &dwExitCode )) 
		{
			dprintf("setup returned %d",dwExitCode);

			if ( 0 == stricmp (OSType,"Win95")) 
				ret = TRUE;
			else
				ret = ( dwExitCode == 0 );
		}
		
		CloseHandle(processinfo.hProcess);
		CloseHandle(processinfo.hThread);
	}
	else 
	{
		dprintf("Setup program didn't run (return code %d).", GetLastError());
	}

 	return ret;
}

///////////////////////////////////////////////////////////////////////////////
int CheckForNT()
{
	char buff[1024];
	char* cptr;
	int rc = 0;

	cptr = getenv( IDS_NT_COMSPEC );
	if ( !cptr )
		cptr = getenv( IDS_DOS_COMSPEC );

	if ( cptr )
	{          
		strncpy( buff, cptr, sizeof(buff)-1 );
		buff[sizeof(buff)-1] = '\0';

		_strupr( buff );
		dprintf( "NT Buff1 = %s", buff );
		if ( strstr( buff, IDS_NT_COMPROC ) )
		{
			rc = 1;
			goto cleanup;
		}
		
		cptr = buff;
		strcpy( cptr, IDS_NT_COMPROC );   
		dprintf( "NT Buff2 = %s", buff );   
		rc = ( _access( buff, 0 ) ? 0 : 1 );
	}

cleanup:
	return rc;
}

///////////////////////////////////////////////////////////////////////////////
void LogLine(char* line)
{
	FILE *LogFile = fopen(LOGIN_LOG,"at");
	if(LogFile)
	{
		fprintf(LogFile,line);
		fprintf(LogFile,"\n");
		fclose(LogFile);
	}
}

///////////////////////////////////////////////////////////////////////////////
void gcbPrintf(WORD wMsgID, ...)
{
    va_list	argList;
	char szTemp[256];                
	char szFinal[256];

    LoadString(g_hInst, wMsgID, szTemp, 256);
    va_start(argList, wMsgID);
    ssvsnprintf((LPTSTR)szFinal, sizeof(szFinal), (LPCTSTR)szTemp, argList); 
    va_end(argList);
    
	SendMessage( g_list, LB_ADDSTRING, 0, (LPARAM) szFinal );

	if (logging)
	{
		LogLine(szFinal);
	}

}//gcbPrintf

///////////////////////////////////////////////////////////////////////////////
void RealDprintf(char *format,...)
{
	va_list marker;
	char line[1024];

	va_start(marker, format);
	ssvsnprintf(line,sizeof(line),format,marker);
	va_end(marker);

	SendMessage( g_list, LB_ADDSTRING, 0, (LPARAM) line );

	if (logging)
	{
		LogLine(line);
	}
}

///////////////////////////////////////////////////////////////////////////////
void PrintVersionString(char * string, DWORD version)
{
	DWORD major, minor, build;

	major = version & 0xffff;
	minor = major % 100;
	major /= 100;
	build = (version & 0xffff0000) >> 16;
	dprintf("%s %d.%d.%d",string,major,minor,build);
}

///////////////////////////////////////////////////////////////////////////////
BOOL IsServerInstalled()
{
	HKEY	hRegKey = 0;
	LONG	lRet = 0;
	DWORD	dwType = REG_DWORD;
	DWORD	dwSize = sizeof(DWORD);
	DWORD	dwNav = 0;
	BOOL	bReturn = 0;

	// Check for an installed NAVCE
	if ( ERROR_SUCCESS == RegOpenKeyEx ( HKEY_LOCAL_MACHINE, VERSION_KEY, 0, KEY_READ, &hRegKey ) )
	{
		if ( ERROR_SUCCESS != SymSaferRegQueryValueEx ( hRegKey, CLIENTTYPE_KEY, NULL, &dwType, (LPBYTE)&dwNav, &dwSize ) )
		{
			RegCloseKey ( hRegKey );
			return NOT_INSTALLED_YET;
		}
	}

	// Determine if it's a Server
	switch ( dwNav )
	{
	case CLIENTTYPE_CONNECTED:
		bReturn = FALSE;
		break;
	case CLIENTTYPE_STANDALONE:
		bReturn = FALSE;
		break;
	case CLIENTTYPE_SERVER:
		bReturn = TRUE;
		break;
	default:
		break;
	}	

    RegCloseKey ( hRegKey );

    return ( bReturn );
}

#if 0   // DALLEE 12/2003 Removed admin check -- install may be configured to run with elevated privileges
/*
////////////////////////////////////////////////////////////////////////////
// Function name    : IsUserAdmin
//
// Description      : Check to see if this process is a admin user. We
//                    get an access token and go through the list of groups
//                    to see if it belongs to Adminstrators/BUILTINS
//
//                    The process is considered a normal user if it is NOT
//                    a member of Adminstrators/BUILTINS.
//
// Return type		: DWORD - Standard error codes
//
// Argument         : ULONG * pbAdminUser - Pointer to a BOOL 
//
////////////////////////////////////////////////////////////////////////////
// 2/1/2000 TCASHIN - Function created / Header added.
// 8/19/2000 RPULINT - Got this from NAVNTUTL
// 12/17/2003 DALLEE - Commented out Admin checks. This is unreliable -- install
//      may be configured to run with elevated privileges for a machine, user
//      or individual install package.
////////////////////////////////////////////////////////////////////////////
DWORD IsUserAdmin( BOOL *pbAdminUser )
{ 
    BOOL    bIsMember       = FALSE;
    DWORD   dwRetValue      = ERROR_SUCCESS;
    DWORD   dwSizeNeeded    = 0;
    HANDLE  hCurrentProcess = NULL;
    HANDLE  hCurrProcToken  = NULL;
    int     i;

    TCHAR   szBuffer1[MAX_PATH] = {0};
    TCHAR   szBuffer2[MAX_PATH] = {0};
    DWORD   dwBuffer1Size       = sizeof(szBuffer1);
    DWORD   dwBuffer2Size       = sizeof(szBuffer2);

    SID_NAME_USE                sidType;
    TOKEN_INFORMATION_CLASS     stTokenInfo     = TokenGroups;
    PTOKEN_GROUPS               lpTokenGroups   = NULL;
    SID_IDENTIFIER_AUTHORITY    SIDAuth         = SECURITY_NT_AUTHORITY;
    BYTE sidBuffer[100];
    PSID lpSID = (PSID)&sidBuffer;

    //
    // See if we have valid parameters
    //
    if ( !pbAdminUser )
    {
        dwRetValue = ERROR_INVALID_PARAMETER;
        goto All_Done;
    }

    //
    // Get the handle of the current process
    //
    hCurrentProcess = OpenProcess( PROCESS_QUERY_INFORMATION, FALSE, GetCurrentProcessId() );
    if ( !hCurrentProcess )
    {
        dwRetValue = GetLastError();
        goto All_Done;
    }

    //
    // Get a handle for the access token for the current process
    //
    if ( !OpenProcessToken( hCurrentProcess, TOKEN_QUERY, &hCurrProcToken) )
    {
        dwRetValue = GetLastError();
        goto Clean_Up;
    }

    //
    // Call to get the size of the buffer we'll need 
    //
    GetTokenInformation(hCurrProcToken, stTokenInfo, NULL, 0, &dwSizeNeeded);

    //
    // Allocate a buffer for the security descriptor
    //
    lpTokenGroups = (TOKEN_GROUPS *) GlobalAlloc (GPTR, dwSizeNeeded);
    if ( !lpTokenGroups )
    {
        dwRetValue = GetLastError();
        goto Clean_Up;
    }

    //
    // Get the list of groups from the token
    //
    if ( !GetTokenInformation(hCurrProcToken, stTokenInfo, lpTokenGroups, dwSizeNeeded, &dwSizeNeeded) )
    {
        dwRetValue = GetLastError();
        goto Clean_Up;
    }

    //
    // Create a SID for the BUILTIN\Administrators group.
    //
    if(! AllocateAndInitializeSid( &SIDAuth, 2,
                                   SECURITY_BUILTIN_DOMAIN_RID,
                                   DOMAIN_ALIAS_RID_ADMINS,
                                   0, 0, 0, 0, 0, 0,
                                   &lpSID) ) 
    {
        dwRetValue = GetLastError();
        goto Clean_Up;
    }

    //
    // Set the default to a normal user
    //
    *pbAdminUser = FALSE;

    //
    // Loop through the list of group SIDs
    //
    for ( i=0; i<(int)lpTokenGroups->GroupCount; i++ )
    {
        //
        // Reset the buffer sizes
        //
        dwBuffer1Size = sizeof(szBuffer1);
        dwBuffer2Size = sizeof(szBuffer2);

        //
        // Get the name and domain from the groups SID
        //
        if ( LookupAccountSid( NULL, 
                                lpTokenGroups->Groups[i].Sid, 
                                szBuffer1, 
                                &dwBuffer1Size, 
                                szBuffer2, 
                                &dwBuffer1Size, 
                                &sidType) )
        {
            //
            // If the current SID is the admins and it is enabled,
            // then we're good to go.
            //

            if ( EqualSid( lpSID, lpTokenGroups->Groups[i].Sid) && 
                 (lpTokenGroups->Groups[i].Attributes & SE_GROUP_ENABLED) )
            {
                *pbAdminUser = TRUE;
            }
        }
    }

Clean_Up:

    //
    // Free the memory we allocated for the token info
    //
    if( lpTokenGroups && GlobalFree(lpTokenGroups) )
    {
        dwRetValue = GetLastError();
    }
    
    //
    // Close the handle for the current process
    //
    if ( hCurrentProcess )
    {
        CloseHandle( hCurrentProcess );
        hCurrentProcess = NULL;
    }

    //
    // Free the SID we allocated
    //
    if ( lpSID )
        FreeSid( lpSID );

All_Done:

    return dwRetValue;
}
*/
#endif


INT_PTR CALLBACK OutputDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int a;

    switch (uMsg) 
    {
		case WM_COMMAND:
			switch (wParam)
			{
				case IDC_BUTTON_YES:
					SetEvent(g_hYesEvent);
					break;
				case IDC_BUTTON_NO:
					SetEvent(g_hNoEvent);
					break;
				case IDC_BUTTON_CLOSE:
					DestroyWindow(hwndDlg);
					break;
			}
			break;
		case WM_CLOSE:
			DestroyWindow(hwndDlg);
			break;
		case WM_DESTROY:
			// Free any arguments that we created
			if (g_argc > 0)
			{
				for (a = 0; a < g_argc; a++)
				{
					free(g_argv[a]);
					g_argv[a] = NULL;
				}
			}

			PostQuitMessage(0);
			break;
	}

	return FALSE;
}

void ErrorMessage(LPTSTR szErrorMessage)
{
	char szTemp[256];
	if (g_bIsSAVDeploy)
		LoadString(g_hInst, IDS_WIN_TITLE, szTemp, 256);
	else LoadString(g_hInst, IDS_WIN_TITLE_SCS, szTemp, 256);

	MessageBox(g_hOutputWnd, szErrorMessage, szTemp, MB_OK | MB_ICONEXCLAMATION);
}
