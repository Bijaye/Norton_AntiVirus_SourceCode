//Declarations of functions implemented in AdvChk.cpp

//Helper functions that install/uninstall.

//Helper functions that check installations
DWORD	ProcessCmdLine(LPTSTR szCmdLine);
BOOL	VerifyAdvToolsRegKeys();


//Pre DefinedRegistry Values
#define REGKEY_INSTALLEDAPPS _T("Software\\Symantec\\InstalledApps")
#define REGKEY_UNERASE _T("SOFTWARE\\Symantec\\Norton Utilities\\Norton UnErase")
#define INSTALLDIR _T("InstallDir");
#define UNERASE_DISPLAYNAME _T("Norton UnErase Protection")

//Return Values from ProcessCmdLine
#define INSTALL   1
#define UNINSTALL 2


// Unerase defines
#define UNERASE_PROT_DIR _T("?:\\RECYCLED\\NPROTECT")