// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/********************************************************
	Authors: Gordon Roylance
			 Randy Templeton

	Copywrite (C) 1997 Intel Corporation
******************************************************/

#ifndef _VP50_
#define _VP50_


#include "Resource.h"
#include "AppInfo.h"
#include "TransmanClass.h"

#define ID_UPDATE				WM_USER + 1

#define DEFAULT_DOMAIN_PASSWORD		"symantec"
#define PASSWORD_KEY				"RaNdYeVaN"


#define MAX_NAME	512
#define MAX_PATHS	MAX_PATH * 2

#define INSTALL				0x0001
#define REINSTALL			0x0010
#define UPDATE				0x0100
#define UNINSTALL			0x1000

#define CLIENT_INSTALLED	0x0001
#define SERVER_INSTALLED	0x0002
#define CONSOLE_INSTALLED	0x0004
#define AMS_INSTALLED		0x0008
#define REMOTE_SERVER		0x0010
#define IMAGE_INSTALLED		0x0020
#define CLIENTS_SKEW		0x0040
#define SHOW_ADVERTISE		0x0080
#define LICENSE_AGREEMENT	0x0100
// RUN_SILENTLY				0x0800 -- see "Ildvpdist.h"
  
struct SetupInfo 
{
	int WizardType;			//specifies which type of install to perform
	int Console;			//- these three integer variables define which action to take
	int Server;				//- for the appropriate files they represent, possible actions:
	int Client;				//- INSTALL, REINSTALL, and UNINSTALL
	int InstType;			//bit flags specify what has been installed: CLIENT,CONSOLE,SERVER
	BOOL AMS;				//set when copying AMS files
	BOOL AutoStart;			//set when automatic startup for services is selected
	BOOL Config;			//determines whether to copy vpregdb.def file for a reinstall
	BOOL WinExecInst;		//set when installing server on local machine
	char VPPath[MAX_PATHS];	//stores (or returns) the path of (for) VP software
	char SvPath[MAX_PATHS]; //returns the server path of the selected server for client install
	int WizDone;			//set when modeless wizard should finish
	HWND Handle;			//parent window handle for modeless wizard
	FARPROC pMake3D;		//function needed for Windows NT 3.51
	BOOL bInstallLDCM;		//TRUE if LDCM integration should be installed
	BOOL bIsLDCMPresent;	//TRUE if LDCM is around
};

int InitSilent();
int GetValueFromFile(LPCTSTR sFileName,LPCTSTR sString,LPTSTR sValue);

BOOL GetRemoteProgsDir(LPTSTR pPath);
void RemoveClientFromServer();

// flags for what products
#define OLD_CLIENT			0x00000001
#define OLD_SERVER			0x00000002
#define OLD_CONSOLE			0x00000004
#define OLD_AMS				0x00000008
#define NEW_CLIENT			0x00000010
#define NEW_SERVER			0x00000020
#define NEW_CONSOLE			0x00000040
#define NEW_AMS				0x00000080
#define EXCHANGE_MAIL		0x00000100
#define CCMAIL_MAIL			0x00000200
#define LOTUS_MAIL			0x00000400

// flags for options
#define DISPLAY_AD			0x01000000
#define CONSOLE_WIZARD		0x04000000
#define SERVER_WIZARD		0x08000000
#define CLIENT_WIZARD		0x10000000
#define RANFROM_LOGIN		0x00010000

#define CLIENT_SUPPORT		0x00000010
#define SERVER_LOCAL		0x00000020
#define CONNECTED_CLIENT	0x00000040


void FindVPProducts(DWORD& dwFlags);
void GetSpecialVariables();
CString GetProgramFilesPath();
CString GetPreviousProductPath();
CString BuildInitialPath();
CString GetCurrentProductPath();

#define ERR_NO_INF_FILE				0x5060
#define ERR_FILES_ONREBOOT			0x5050
#define ERR_USER_CANCELED			0x5070
#define ERR_PREVIOUS_ONREBOOT		0x5080

DWORD TestForReboot(CString sSrc, CString sDest, BOOL &bUpdate);
DWORD AttemptToUpdateFile(CString sSrc, CString sDest, BOOL &bReboot);
DWORD UpdateSystemFiles(BOOL bSilent, BOOL bAllowReboot = TRUE);
DWORD RegisterSetupFiles(CString sSrc, BOOL bSilent, BOOL bRegister = TRUE);

#endif
