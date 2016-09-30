// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SAVSERVERCA_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SAVSERVERCA_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
/*#ifdef SAVSERVERCA_EXPORTS
#define SAVSERVERCA_API __declspec(dllexport)
#else
#define SAVSERVERCA_API __declspec(dllimport)
#endif*/

///////////////////////////////////////////////////////////////////////////////
//
//  Preprocessor
//
//  NOTE: the dllexport attribute eliminates the need for a .DEF file
//
///////////////////////////////////////////////////////////////////////////////
#include <msi.h>
#include <msiquery.h>

#define MBUFFER					512
#define LBUFFER					1024
#define DllImport   __declspec( dllimport )
#define DllExport	__declspec( dllexport )
#define SYM_MAX_PATH            260     /* max. length of full pathname     */
#define MAX_LOG_PATH			1024 // Ensure that we never buffer over run when dealing with strings as large as MAX_PATH
#define DOUBLE_MAX_PATH			(MAX_PATH * 2)
// Used for Server Rollout communication
#define SYMANTEC_INSTALLDIR     "Software\\Symantec\\Symantec Antivirus\\Install\\7.50"
#define SYMANTEC_UNCPATH		"Software\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion"
#define REMOTE_UPGRADE_PATH_KEY		"Software\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion\\RemoteUpgrade"
#define SYMANTEC_SHARE_PATH_BACKUP "C:\\Program Files\\Symantec Antivirus"
#define PERCENT_SHIFT				16
#define _STATUS						"_Status"
#define _ERROR						"_Error"
#define _COMMAND					"_Command"
#define STS_INITIAL_MESSAGE			0x01000000
#define STS_STARTED_MESSAGE			0x02000000
#define STS_FINISHED_MESSAGE		0x03000000
#define STS_WILLNOT_UPGRADE			0x05000000
#define STS_REBOOT_NECESSARY		0x07000000
#define STS_USER_STOPPED			0x08000000
#define STS_COPYING_FILES			0x09000000
#define STS_DELETING_FILES			0x0a000000
#define STS_UPDATING_REGISTRY		0x0b000000
#define STS_CLEANUP_REGISTRY		0x0c000000
#define STS_ADDING_SHARES			0x0d000000
#define STS_REMOVING_SHARES			0x0e000000
#define STS_UPDATING_AMS2			0x0f000000
#define STS_STARTING_SERVICE		0x10000000
#define STS_STOPPING_SERVICE		0x11000000
#define STS_OLDSSC_MIG_ERROR   		0x13000000
#define FINISH_TYPE					0x7000
// UpdateLoginDirFile defines
#define INSTALLED_APPS_KEY					"Software\\Symantec\\InstalledApps"
#define AMS_INSTALLED_APPS_KEY				"Software\\Symantec\\AMS Server"
#define AMS_INSTALLED_REG_PATH				"InstallPath"
#define INTEL_INSTALL_LANGUAGE_CODE			"ENU"
#define SYMANTEC_INSTALL_PATH				"SAVCE"
#define NAV_INSTALL_PATH					"NAVNT"
#define SYMANTEC_INSTALL_DIR				"InstallDir"
#define VPLOGON_BAT_PATH					"\\logon\\vplogon.bat"
#define I2_LDVP_VDB_PATH					"I2_LDVP.VDB"
#define szReg_Val_Errors					"Errors"

#define INSTALLER_SRC_PACKAGES_COUNT 5

static char * pszInstallerSrcPackages[INSTALLER_SRC_PACKAGES_COUNT] =
        {"WIN32",
        "WIN32SCS",
        "WIN64",
        "WIN32VISTA",
        "WIN64VISTA"};
static char * pszSrcPropertiesDectected[INSTALLER_SRC_PACKAGES_COUNT] = 
        {"WIN32INST.2D6B2C77_9DB3_4019_A3E4_3F2892186836",
        "WIN32SCSINST.2D6B2C77_9DB3_4019_A3E4_3F2892186836",
        "WIN64INST.2D6B2C77_9DB3_4019_A3E4_3F2892186836",
        "WIN32VISTAINST.2D6B2C77_9DB3_4019_A3E4_3F2892186836",
        "WIN64VISTAINST.2D6B2C77_9DB3_4019_A3E4_3F2892186836"};

DWORD	ConvertToWide(LPCSTR pMultiStr, LPWSTR &pWideStr);
DWORD   GetMSIProperty( MSIHANDLE hInstall, TCHAR strProperty []);
DWORD	RemoveNTShare( LPTSTR );
UINT	MsiLogMessage( MSIHANDLE , TCHAR* );
UINT	WriteRemoteStatusReg (LPTSTR szStatusString, DWORD dwStatus);
UINT CreateRemoteReg (MSIHANDLE hInstall, DWORD dwErrorFlags);
BOOL DoUpdateLoginDirFile( TCHAR* szTarget, TCHAR* szInfo );
BOOL DeleteDir( MSIHANDLE hInstall, TCHAR* strFolder, TCHAR* strPattern );
int CopyDirRecursive( MSIHANDLE hInstall, TCHAR* strSource, TCHAR* strDest );
void SetOldSSCMessage();
DWORD RegKeyDeleteAll(HKEY hKey, LPCSTR sKeyName);
BOOL Findit (MSIHANDLE hInstall, LPCTSTR szFile, TCHAR* strPath);

extern "C" DllExport	DWORD	__stdcall AddNTShare(LPTSTR szShareName, LPTSTR szSharePath, LPTSTR szShareRemark, DWORD dwAccess, MSIHANDLE hInstall);
extern "C" DllExport DWORD __stdcall RemoveServerShares(MSIHANDLE hInstall);

UINT _stdcall MSIRemoveSAVServerShares	( MSIHANDLE hInstall);
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
UINT _stdcall MSIUnInstallAMSServerLegacy( MSIHANDLE hInstall);
UINT _stdcall UpdateLoginDirFile		( MSIHANDLE hInstall);
UINT _stdcall StreamAMSCustdll			( MSIHANDLE hInstall);
UINT _stdcall EncryptConsolePass		( MSIHANDLE hInstall);
UINT __stdcall ProcessSettingsINI		( MSIHANDLE);
UINT _stdcall MSIRemoveVDB_Dirs			( MSIHANDLE hInstall);
UINT _stdcall MSIBackupServerGroupsDir	( MSIHANDLE hInstall );
UINT _stdcall MSIRestoreServerGroupsDir	( MSIHANDLE hInstall );
UINT _stdcall MSICreateDomainGuid		( MSIHANDLE hInstall );
UINT _stdcall MSISTS_ERROR_OLDSSC		( MSIHANDLE hInstall);
UINT __stdcall MsiClearAddressCache		( MSIHANDLE hInstall );
UINT _stdcall MSIRemoveAmsVerFiles		( MSIHANDLE hInstall );
UINT _stdcall MSIRemoveAmsVerFilesRB	( MSIHANDLE hInstall );
UINT _stdcall MSIRemoveAmsVerFilesRB    ( MSIHANDLE hInstall );
UINT _stdcall MSIDetectSCSInstallers    ( MSIHANDLE hInstall );
