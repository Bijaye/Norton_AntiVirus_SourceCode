// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006 Symantec Corporation. All rights reserved.

#include <msi.h>
#include <msiquery.h>
#include <string>

#ifdef UNICODE
#define tstring std::w_string
#else
#define tstring std::string
#endif

#define FINISH_TYPE					0x7000
#define szReg_Val_Errors			"Errors"
#define REMOTE_UPGRADE_PATH_KEY		"Software\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion\\RemoteUpgrade"
#define DOUBLE_MAX_PATH				(MAX_PATH * 2)

UINT	CreateRemoteReg (MSIHANDLE hInstall, DWORD dwErrorFlags);
UINT	WriteRemoteStatusReg (LPTSTR szStatusString, DWORD dwStatus);
void	SetOldSSCMessage();

UINT _stdcall MSIUpdateRemoteStatus		( MSIHANDLE hInstall);
UINT _stdcall MSISTS_INITIAL_MESSAGE	( MSIHANDLE hInstall);
UINT _stdcall MSISTS_STARTED_MESSAGE	( MSIHANDLE hInstall);
UINT _stdcall MSISTS_COPYING_FILES		( MSIHANDLE hInstall);
UINT _stdcall MSISTS_UPDATING_REGISTRY	( MSIHANDLE hInstall);
UINT _stdcall MSISTS_ADDING_SHARES		( MSIHANDLE hInstall);
UINT _stdcall MSISTS_UPDATING_AMS2		( MSIHANDLE hInstall);
UINT _stdcall MSISTS_STARTING_SERVICE	( MSIHANDLE hInstall);
UINT _stdcall MSISTS_FINISHED_MESSAGE	( MSIHANDLE hInstall);
UINT _stdcall MSI_ERROR_ROLLBACK		( MSIHANDLE hInstall);
UINT _stdcall MSISTS_FINISHED_AMS2		( MSIHANDLE hInstall);
UINT _stdcall MSISTS_UPDATING_REPORTING	( MSIHANDLE hInstall);
UINT _stdcall MSISTS_FINISHED_REPORTING	( MSIHANDLE hInstall);
UINT _stdcall MSIStartMessage			( MSIHANDLE hInstall);
UINT _stdcall MSISTS_SLEEP5000			( MSIHANDLE hInstall);
