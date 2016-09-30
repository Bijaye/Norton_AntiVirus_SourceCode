//Header file for Nprotect specific functions

#ifndef __NPROTECT_H__
#define __NPROTECT_H__

//Defines used for installing Nprotect
#define NPROTECT_SERVICE_NAME   _T("NProtectService")
#define NPROTECT_SERVICE_FILE	_T("NPROTECT.EXE")
#define NPROTECT_SERVICE_DISPLAY_NAME _T("Norton Unerase Protection")

#define NPROTECT_DRIVER_NAME	_T("NPDriver")
#define NPROTECT_DRIVER_FILE	_T("NPDRIVER.SYS")
#define NPROTECT_DRIVER_DISPLAY_NAME _T("Norton Unerase Protection Driver")


BOOL	IsNUInstalledWithNProtect(LPTSTR szFullPath, DWORD &dwSize);
BOOL    CheckForBonsaiShellExtension();

DWORD	Install_NProtect();
DWORD	UnInstall_NProtect();
BOOL	VerifyNProtService();

BOOL	InstallNPDriver(BOOL bStartService);
BOOL	UnInstallNPDriver();

BOOL ShutdownUnerase9( void );
BOOL ShutdownNprotect95( void );
BOOL StopNprotectService(void);
BOOL StartNprotectService(void);
BOOL WINAPI RestoreRecycleBin( void );

BOOL	CopyDriverToDriversDirectory();
BOOL	DeleteDriverFromDriversDirectory();

void LaunchFileInMyDirectory(LPTSTR szFileName, LPTSTR szCmdLine, BOOL bWait);
#endif