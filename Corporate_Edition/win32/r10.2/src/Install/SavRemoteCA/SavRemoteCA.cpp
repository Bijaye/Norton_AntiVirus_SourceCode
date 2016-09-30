// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006 Symantec Corporation. All rights reserved.
///////////////////////////////////////////////////////////////////////////
//
// SavRemoteCA - MSI Custom Action DLL
// Description: To provide a generic way to update SAV installer status
//				messages for an external mergemodule.
//
///////////////////////////////////////////////////////////////////////////
// 1/10/06 DKowalyshyn
///////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#pragma warning (disable: 4786) // identifier was truncated to '255' characters in the debug information

#define INITIIDS

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <TCHAR.H> 

#include "vpstrutils.h"
//#include "SymSaferRegistry.h"
#include "Status.h"
#include "ClientReg.h"
#include "SymSaferStrings.h"
#include "SavRemoteCA.h"

HINSTANCE g_hinstDLL;

BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID )
{
	switch (fdwReason)
	{
		case  DLL_PROCESS_ATTACH:
		{
			g_hinstDLL = (HMODULE) hinstDLL;
			break;
		}

		case  DLL_THREAD_ATTACH:
		break;

		case  DLL_THREAD_DETACH:
		break;

		case  DLL_PROCESS_DETACH:
		break;

		default:
		break;
	}
	return  (TRUE);

}  // end of "DllMain"

///////////////////////////////////////////////////////////////////////////
//	
// Function: MsiLogMessage
//
// Description: MsiLogMessage
//
///////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////
UINT MsiLogMessage( MSIHANDLE hInstall, LPCTSTR szString )
{
	UINT uiRetVal = ERROR_SUCCESS;

	MSIHANDLE hRec = MsiCreateRecord(1);
	if (hRec)
	{
		TCHAR szLogLine[MAX_PATH*5]; // * buffer overrun fix

		lstrcpy(szLogLine, "SavRemoteCA: ");
		lstrcat(szLogLine, szString);

		// Use OutputDebugString...
		OutputDebugString(szLogLine);

		// ...as well as the log file.
		MsiRecordClearData(hRec);
		MsiRecordSetString(hRec, 0, szLogLine);
		MsiProcessMessage(hInstall, INSTALLMESSAGE_INFO, hRec);
		MsiCloseHandle(hRec);
	}

	return uiRetVal;
}
///////////////////////////////////////////////////////////////////////////
//
// Function: MSISTS_SLEEP5000
//
// Description: Sleep for a couple to simulate a proper install
//	this is for testing purposes only.
// 
//
///////////////////////////////////////////////////////////////////////////
// 1/10/2006 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
UINT _stdcall MSISTS_SLEEP5000( MSIHANDLE hInstall)
{
	TCHAR szErrorFlags[DOUBLE_MAX_PATH] = {0};
	sssnprintf(szErrorFlags, sizeof(szErrorFlags), "MSISTS_SLEEP5000 5000ms.");
	MsiLogMessage (hInstall, szErrorFlags );
	Sleep(5000);

	return ERROR_SUCCESS;
}
///////////////////////////////////////////////////////////////////////////
//
// Function: MSIStartMessage
//
// Description: Set a get key so that AVServer rollout knows the reporter
//	agent install has started. 
//
///////////////////////////////////////////////////////////////////////////
// 1/11/2006 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
UINT _stdcall MSIStartMessage( MSIHANDLE hInstall )
{
	HKEY		regKey = NULL;
	DWORD		dwDisp = 0;
	DWORD		dwRet = 0;

	// Open RemoteUpgrade Key and set the install key
	if ((dwRet = RegCreateKeyEx(HKEY_LOCAL_MACHINE,INSTALLSTATUS_REGKEY,0,0,0,KEY_WRITE,NULL,&regKey,&dwDisp)) == ERROR_SUCCESS)
	{
		DWORD dwSize = sizeof(DWORD);
		DWORD dwVal = REPORTER_INSTALL_STARTED;
		dwRet = RegSetValueEx (regKey,INSTALL_REPORTER,0,REG_DWORD,(const BYTE *)&dwVal,sizeof (DWORD));
		RegCloseKey(regKey);
		MsiLogMessage(hInstall,"SavRemote: Create Key INSTALL_REPORTER succeeded.");
	}
	else
	{
		MsiLogMessage(hInstall,"SavRemote: Create Key INSTALL_REPORTER failed.");
	}
	RegCloseKey (regKey);
	MsiLogMessage(hInstall,"SavRemote: MSIStartMessage.");
	
	return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
//
// Function: MSIUpdateRemoteStatus
//
// Description: Will write to a registry to keep Rollout updated 
//
// 
//
///////////////////////////////////////////////////////////////////////////
// 8/25/2003 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
UINT _stdcall MSIUpdateRemoteStatus( MSIHANDLE hInstall)
{
	DWORD cchBuff = 256;		// size of computer or user name 
	BOOL bRet = 0;				// returncode
	TCHAR szRegString[DOUBLE_MAX_PATH] = {0};
	LPTSTR lpszSystemInfo;		// pointer to system information string 
	TCHAR tchBuffer2[DOUBLE_MAX_PATH] = {0};  // buffer for concatenated string 
	TCHAR szCompName[DOUBLE_MAX_PATH] = {0};


	lpszSystemInfo = tchBuffer2; 

	bRet = GetComputerName(szCompName, &cchBuff); 
	lstrcpy (szRegString, szCompName);
	lstrcat (szRegString, "_status");

	MsiLogMessage (hInstall, "MSIUpdateRemoteStatus szRegString" );
	MsiLogMessage (hInstall, szRegString );

	WriteRemoteStatusReg (szRegString,STS_INITIAL_MESSAGE);

	return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
//
// Function: MSISTS_INITIAL_MESSAGE
//
// Description: Will to registry for MSISTS_INITIAL_MESSAGE status
//
// 
//
///////////////////////////////////////////////////////////////////////////
// 8/26/2003 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
UINT _stdcall MSISTS_INITIAL_MESSAGE( MSIHANDLE hInstall)
{
	DWORD dwErrorPercent=0;
	DWORD dwErrorFlags=0;
	TCHAR szErrorFlags[DOUBLE_MAX_PATH] = {0};
	dwErrorPercent = 0 << PERCENT_SHIFT;  // percentage done and bitshift
	dwErrorFlags = STS_INITIAL_MESSAGE | dwErrorPercent;
	sssnprintf(szErrorFlags, sizeof(szErrorFlags), "MSISTS_INITIAL_MESSAGE dwErrorFlags %li ",dwErrorFlags);
	MsiLogMessage (hInstall, szErrorFlags );
	CreateRemoteReg (hInstall ,dwErrorFlags);
	return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
//
// Function: MSISTS_STARTED_MESSAGE
//
// Description: Will to registry for MSISTS_STARTED_MESSAGE status
//
// 
//
///////////////////////////////////////////////////////////////////////////
// 8/26/2003 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
UINT _stdcall MSISTS_STARTED_MESSAGE( MSIHANDLE hInstall)
{
	DWORD dwErrorPercent=0;
	DWORD dwErrorFlags=0;
	TCHAR szErrorFlags[DOUBLE_MAX_PATH] = {0};
	dwErrorPercent = 10 << PERCENT_SHIFT;  // percentage done and bitshift
	dwErrorFlags = STS_STARTED_MESSAGE | dwErrorPercent;
	sssnprintf(szErrorFlags, sizeof(szErrorFlags), "MSISTS_STARTED_MESSAGE dwErrorFlags %li ",dwErrorFlags);
	MsiLogMessage (hInstall, szErrorFlags );
	CreateRemoteReg (hInstall ,dwErrorFlags);
	return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
//
// Function: MSISTS_COPYING_FILES
//
// Description: Will to registry for MSISTS_COPYING_FILES status
//
// 
//
///////////////////////////////////////////////////////////////////////////
// 8/26/2003 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
UINT _stdcall MSISTS_COPYING_FILES( MSIHANDLE hInstall)
{
	DWORD dwErrorPercent=0;
	DWORD dwErrorFlags=0;
	TCHAR szErrorFlags[DOUBLE_MAX_PATH] = {0};
	dwErrorPercent = 30 << PERCENT_SHIFT;  // percentage done and bitshift
	dwErrorFlags = STS_COPYING_FILES | dwErrorPercent;
	sssnprintf(szErrorFlags, sizeof(szErrorFlags), "MSISTS_COPYING_FILES dwErrorFlags %li ",dwErrorFlags);
	MsiLogMessage (hInstall, szErrorFlags );
	CreateRemoteReg (hInstall ,dwErrorFlags);
	return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
//
// Function: MSISTS_UPDATING_REGISTRY
//
// Description: Will to registry for MSISTS_UPDATING_REGISTRY status
//
// 
//
///////////////////////////////////////////////////////////////////////////
// 8/26/2003 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
UINT _stdcall MSISTS_UPDATING_REGISTRY( MSIHANDLE hInstall)
{
	DWORD dwErrorPercent=0;
	DWORD dwErrorFlags=0;
	TCHAR szErrorFlags[DOUBLE_MAX_PATH] = {0};
	dwErrorPercent = 50 << PERCENT_SHIFT;  // percentage done and bitshift
	dwErrorFlags = STS_UPDATING_REGISTRY | dwErrorPercent;
	sssnprintf(szErrorFlags, sizeof(szErrorFlags), "MSISTS_UPDATING_REGISTRY dwErrorFlags %li ",dwErrorFlags);
	MsiLogMessage (hInstall, szErrorFlags );
	CreateRemoteReg (hInstall ,dwErrorFlags);
	return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
//
// Function: MSISTS_ADDING_SHARES
//
// Description: Will to registry for MSISTS_ADDING_SHARES status
//
// 
//
///////////////////////////////////////////////////////////////////////////
// 8/26/2003 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
UINT _stdcall MSISTS_ADDING_SHARES( MSIHANDLE hInstall)
{
	DWORD dwErrorPercent=0;
	DWORD dwErrorFlags=0;
	TCHAR szErrorFlags[DOUBLE_MAX_PATH] = {0};
	dwErrorPercent = 60 << PERCENT_SHIFT;  // percentage done and bitshift
	dwErrorFlags = STS_ADDING_SHARES | dwErrorPercent;
	sssnprintf(szErrorFlags, sizeof(szErrorFlags), "MSISTS_ADDING_SHARES dwErrorFlags %li ",dwErrorFlags);
	MsiLogMessage (hInstall, szErrorFlags );
	CreateRemoteReg (hInstall ,dwErrorFlags);
	return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
//
// Function: MSISTS_STARTING_SERVICE
//
// Description: Will to registry for MSISTS_STARTING_SERVICE status
//
// 
//
///////////////////////////////////////////////////////////////////////////
// 8/26/2003 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
UINT _stdcall MSISTS_STARTING_SERVICE( MSIHANDLE hInstall)
{
	DWORD dwErrorPercent=0;
	DWORD dwErrorFlags=0;
	TCHAR szErrorFlags[DOUBLE_MAX_PATH] = {0};
	dwErrorPercent = 70 << PERCENT_SHIFT;  // percentage done and bitshift
	dwErrorFlags = STS_STARTING_SERVICE | dwErrorPercent;
	sssnprintf(szErrorFlags, sizeof(szErrorFlags), "MSISTS_STARTING_SERVICE dwErrorFlags %li ",dwErrorFlags);
	MsiLogMessage (hInstall, szErrorFlags );
	CreateRemoteReg (hInstall ,dwErrorFlags);
	return ERROR_SUCCESS;
}
///////////////////////////////////////////////////////////////////////////
//
// Function: MSISTS_UPDATING_REPORTING
//
// Description: Will to registry for MSISTS_UPDATING_REPORTING status
//
// 
//
///////////////////////////////////////////////////////////////////////////
// 1/10/2006 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
UINT _stdcall MSISTS_UPDATING_REPORTING( MSIHANDLE hInstall)
{
	DWORD dwErrorPercent=0;
	DWORD dwErrorFlags=0;
	TCHAR szErrorFlags[DOUBLE_MAX_PATH] = {0};
	dwErrorPercent = 60 << PERCENT_SHIFT;  // percentage done and bitshift
	dwErrorFlags = STS_INITIAL_MESSAGE | STS_UPDATING_REPORTER | dwErrorPercent;
	sssnprintf(szErrorFlags, sizeof(szErrorFlags), "MSISTS_UPDATING_REPORTING dwErrorFlags %li ",dwErrorFlags);
	MsiLogMessage (hInstall, szErrorFlags );
	CreateRemoteReg (hInstall ,dwErrorFlags);
	return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
//
// Function: MSISTS_FINISHED_REPORTING
//
// Description: Will to registry for MSISTS_FINISHED_REPORTING status
//
///////////////////////////////////////////////////////////////////////////
// 1/10/2006 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
UINT _stdcall MSISTS_FINISHED_REPORTING( MSIHANDLE hInstall)
{
	DWORD dwErrorPercent=0;
	DWORD dwErrorFlags=0;
	TCHAR szErrorFlags[DOUBLE_MAX_PATH] = {0};
	dwErrorPercent = 100 << PERCENT_SHIFT;  // percentage done and bitshift
	dwErrorFlags = dwErrorFlags = STS_INITIAL_MESSAGE | STS_FINISHED_REPORTER | dwErrorPercent;
	sssnprintf(szErrorFlags, sizeof(szErrorFlags), "MSISTS_FINISHED_REPORTING dwErrorFlags %li ",dwErrorFlags);
	MsiLogMessage (hInstall, szErrorFlags );
	CreateRemoteReg (hInstall ,dwErrorFlags);
	return ERROR_SUCCESS;
}
///////////////////////////////////////////////////////////////////////////
//
// Function: MSISTS_UPDATING_AMS2
//
// Description: Will to registry for MSISTS_UPDATING_AMS2 status
//
// 
//
///////////////////////////////////////////////////////////////////////////
// 10/02/2003 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
UINT _stdcall MSISTS_UPDATING_AMS2( MSIHANDLE hInstall)
{
	DWORD dwErrorPercent=0;
	DWORD dwErrorFlags=0;
	TCHAR szErrorFlags[DOUBLE_MAX_PATH] = {0};
	dwErrorPercent = 80 << PERCENT_SHIFT;  // percentage done and bitshift
	dwErrorFlags = STS_UPDATING_AMS2 | dwErrorPercent;
	sssnprintf(szErrorFlags, sizeof(szErrorFlags), "MSISTS_UPDATING_AMS2 dwErrorFlags %li ",dwErrorFlags);
	MsiLogMessage (hInstall, szErrorFlags );
	CreateRemoteReg (hInstall ,dwErrorFlags);
	return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
//
// Function: MSISTS_FINISHED_AMS2
//
// Description: Will to registry for MSISTS_FINISHED_AMS2 status
//
///////////////////////////////////////////////////////////////////////////
// 10/06/2003 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
UINT _stdcall MSISTS_FINISHED_AMS2( MSIHANDLE hInstall)
{
	DWORD dwErrorPercent=0;
	DWORD dwErrorFlags=0;
	TCHAR szErrorFlags[DOUBLE_MAX_PATH] = {0};
	dwErrorPercent = 100 << PERCENT_SHIFT;  // percentage done and bitshift
	dwErrorFlags = dwErrorFlags = FINISH_TYPE | STS_REBOOT_NECESSARY | dwErrorPercent;
	sssnprintf(szErrorFlags, sizeof(szErrorFlags), "MSISTS_FINISHED_AMS2 dwErrorFlags %li ",dwErrorFlags);
	MsiLogMessage (hInstall, szErrorFlags );
	CreateRemoteReg (hInstall ,dwErrorFlags);
	return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
//
// Function: MSISTS_FINISHED_MESSAGE
//
// Description: Will to registry for MSISTS_FINISHED_MESSAGE status
//
// 
//
///////////////////////////////////////////////////////////////////////////
// 8/26/2003 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
UINT _stdcall MSISTS_FINISHED_MESSAGE( MSIHANDLE hInstall)
{
	DWORD dwErrorPercent                    = 0;
	DWORD dwErrorFlags                      = 0;
	TCHAR szErrorFlags[DOUBLE_MAX_PATH]     = {0};
    TCHAR szRebootPropertyValue[MAX_PATH]   = {""};
    DWORD dwBufferLength                    = 0;
    DWORD returnValDW                       = ERROR_OUT_OF_PAPER;
    bool  rebootRequired                    = false;

	dwErrorPercent = 100 << PERCENT_SHIFT;  // percentage done and bitshift
    // Currently, SymEvent is always updated for SymProtect and thus a reboot is always required for
    // SP to be active (AP is OK until then, however).  However, for PM reasons, on server deployment we install
    // with REBOOT=ReallySuppress, meaning Setup32 does NOT report that a reboot is needed...because
    // we've (server install) been told to ignore it, but it is needed.
    // Resolution is for server installs, see if REBOOT=ReallySuppress - if it is, assume a reboot is needed
    // (safe as it always is at the moment) and set the STS_REBOOT_NECESSARY flag for the Setup32 deployment box.
    // REBOOT must be passed via CustomActionData because this CA is system context
    dwBufferLength = sizeof(szRebootPropertyValue)/sizeof(szRebootPropertyValue[0]);
    returnValDW = MsiGetProperty(hInstall, _T("CustomActionData"), szRebootPropertyValue, &dwBufferLength);
    if (_tcsicmp(szRebootPropertyValue, _T("reallysuppress")) == 0)
    {
        rebootRequired = true;
        MsiLogMessage(hInstall, "SavRemoteCA:  Server deployment reporting override - finished with reboot required.");
    }
	// Normal method - Check MSIRUNMODE_REBOOTATEND to see if we need to set the STS_REBOOT_NECESSARY flag
	if ( TRUE == MsiGetMode(
		hInstall,
		MSIRUNMODE_REBOOTATEND))
    {
        MsiLogMessage(hInstall, "SavRemoteCA:  Server deployment detected finished with reboot required.");
        rebootRequired = true;
    }
    else
    {
        MsiLogMessage(hInstall, "SavRemoteCA:  Server deployment detected finished, no reboot required.");
    }

    if (rebootRequired)
	{
		dwErrorFlags = FINISH_TYPE | STS_REBOOT_NECESSARY | dwErrorPercent;
		sssnprintf(szErrorFlags, sizeof(szErrorFlags), "MSISTS_REBOOT_NECESSARY dwErrorFlags %li ",dwErrorFlags);
		MsiLogMessage (hInstall, szErrorFlags );
		CreateRemoteReg (hInstall ,dwErrorFlags);
	}
	else
	{
		dwErrorFlags = FINISH_TYPE | STS_FINISHED_MESSAGE | dwErrorPercent;
		sssnprintf(szErrorFlags, sizeof(szErrorFlags), "MSISTS_FINISHED_MESSAGE dwErrorFlags %li ",dwErrorFlags);
		MsiLogMessage (hInstall, szErrorFlags );
		CreateRemoteReg (hInstall ,dwErrorFlags);
	}
	return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
//
// Function: MSI_ERROR_ROLLBACK
//
// Description: Will to registry for MSI_ERROR_ROLLBACK status
// On a failure and MSI Rollback this function will be called and update
// the failure back to rollout. 
//
///////////////////////////////////////////////////////////////////////////
// 8/27/2003 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
UINT _stdcall MSI_ERROR_ROLLBACK( MSIHANDLE hInstall)
{
	
	if ( TRUE == MsiGetMode(
		hInstall,
		MSIRUNMODE_ROLLBACK))
	{
		DWORD dwErrorFlags=0;
		TCHAR szErrorFlags[DOUBLE_MAX_PATH] = {0};
		//set the error bit to 1 and indicate that we're done, but have an error.
		dwErrorFlags = FINISH_TYPE | STS_FINISHED_MESSAGE | 0x00000001;
		sssnprintf(szErrorFlags, sizeof(szErrorFlags), "MSI_ERROR_ROLLBACK dwErrorFlags %li ",dwErrorFlags);
		MsiLogMessage (hInstall, szErrorFlags );
		CreateRemoteReg (hInstall, dwErrorFlags);
	}
	return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
//
// Function: MSISTS_ERROR_OLDSSC
//
// Description: If we detect a Sever migration over while an old SSC exist
// we'll try to fail and post this message back to Rollout. 
//
///////////////////////////////////////////////////////////////////////////
// 3/01/2005 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
UINT _stdcall MSISTS_ERROR_OLDSSC( MSIHANDLE hInstall)
{
	DWORD dwErrorFlags=0;
	TCHAR szErrorFlags[DOUBLE_MAX_PATH] = {0};
	//set the error bit to 1 and indicate that we're done, but have an error.
	dwErrorFlags = FINISH_TYPE | STS_OLDSSC_MIG_ERROR | 0x00000001;
	sssnprintf(szErrorFlags, sizeof(szErrorFlags), "MSISTS_ERROR_OLDSSC dwErrorFlags %li ",dwErrorFlags);
	MsiLogMessage (hInstall, szErrorFlags );
	CreateRemoteReg (hInstall, dwErrorFlags);
	// Setup the RemoteUpgrade\Error for AVServer to read
	SetOldSSCMessage();

	return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
//
// Function: SetOldSSCMessage
//
// Description: Set a special flag so that AVServer rollout and detect 
//	this error and return a useful code. 
//
///////////////////////////////////////////////////////////////////////////
// 3/19/2005 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
void SetOldSSCMessage()
{
	HKEY hKey;
	TCHAR szBuf[MAX_PATH] ={0}; 
	TCHAR szRegKey_RU[MAX_PATH] ={0}; 
	TCHAR szCompName[MAX_PATH] = {0};
	TCHAR szRegString[MAX_PATH] = {0};
	DWORD cchBuff = MAX_PATH;		// size of computer or user name 

	GetComputerName(szCompName, &cchBuff); 
	lstrcpy (szRegString, szCompName);
	lstrcat (szRegString, "_Error");

	vpsnprintf( szRegKey_RU, sizeof(szRegKey_RU), "%s\\%s\\%s", szReg_Key_Main,szReg_Val_RemoteUpgrade,szReg_Val_Errors);

	DWORD dwVal = STS_OLDSSC_MIG_ERROR;

	if( ERROR_SUCCESS ==  RegCreateKeyEx(
		HKEY_LOCAL_MACHINE,                   
		_T(szRegKey_RU),              
		0,								
		_T(szRegString),				
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&hKey,
		NULL))
	{
		RegSetValueEx( hKey, szRegString, 0, REG_DWORD, (LPBYTE) &dwVal , sizeof(DWORD) );
	}
}

///////////////////////////////////////////////////////////////////////////
//
// Function: CreateRemoteReg
//
// Description: Create the specific registry information to be written
//
// Calling: WriteRemoteStatusReg
//
///////////////////////////////////////////////////////////////////////////
// 8/26/2003 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
UINT CreateRemoteReg (MSIHANDLE hInstall, DWORD dwErrorFlags)
{
	DWORD cchBuff = 256;		// size of computer or user name 
	BOOL bRet = 0;				// returncode
	TCHAR szRegString[DOUBLE_MAX_PATH] = {0};
	LPTSTR lpszSystemInfo;		// pointer to system information string 
	TCHAR tchBuffer2[DOUBLE_MAX_PATH] = {0};  // buffer for concatenated string 
	TCHAR szCompName[DOUBLE_MAX_PATH] = {0};
	lpszSystemInfo = tchBuffer2; 

	bRet = GetComputerName(szCompName, &cchBuff); 
	lstrcpy (szRegString, szCompName);
	lstrcat (szRegString, "_status");

	MsiLogMessage (hInstall, "CreateRemoteReg szRegString" );
	MsiLogMessage (hInstall, szRegString );

	WriteRemoteStatusReg (szRegString,dwErrorFlags);

	return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
//
// Function: WriteRemoteStatusReg
//
// Description: Writing out registry information for Rollout status
//
// 
//
///////////////////////////////////////////////////////////////////////////
// 8/26/2003 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
UINT WriteRemoteStatusReg (LPTSTR szStatusString, DWORD dwStatus)
{
	DWORD dwRet = 0;
	HKEY hKey;
	TCHAR szClassName[] = {0};

//	wsprintf(szStatusStringVal, "%li/0",dwStatus);

	if( ERROR_SUCCESS ==  RegCreateKeyEx(
	  HKEY_LOCAL_MACHINE,                   // handle to open key
	  REMOTE_UPGRADE_PATH_KEY,              // subkey name
	  0,									// reserved
	  szClassName,									// class string
	  REG_OPTION_NON_VOLATILE,              // special options
	  KEY_ALL_ACCESS,                       // desired security access
	  NULL,									// inheritance
	  &hKey,								// key handle 
	  NULL									// disposition value buffer
	  )){
		if( ERROR_SUCCESS ==  RegSetValueEx (hKey,
			szStatusString,
			0,
			REG_DWORD,
			(LPBYTE)&dwStatus,			// pointer to the value data
			sizeof(dwStatus) ))		// length of value data 
		{
			dwRet = ERROR_SUCCESS;
		}
		else
		{
			dwRet =1;
		}
	}
	RegCloseKey( hKey );
	return dwRet;
}
