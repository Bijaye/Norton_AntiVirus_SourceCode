

//Pre DefinedRegistry Values
#define REGKEY_INSTALLEDAPPS _T("Software\\Symantec\\InstalledApps")

//NU possible installed apps values.
static LPCTSTR NUInstalledAppList[] =
{
    {_T("Norton Utilities")     },
    {_T("NU4.0")                },
    {_T("NU3.0")                },
	NULL
};

#define SI_MAX_PATH             260
#define SI_MAXSTR               1024

BOOL IsOSNT();
VOID CDECL	LTrace( LPCSTR lpszFmt, ... );
BOOL		IsAppInstalled(LPCTSTR szAPPID, LPTSTR szPath, DWORD &dwSize);
BOOL		IsNUInstalled(LPTSTR szPath, DWORD &dwSize);
BOOL		GetModuleDirectory(LPTSTR szPath);

BOOL WINAPI RemoveNProtectDirectory( VOID );
BOOL WINAPI DeleteDirectoryContents( LPTSTR lpDirectory, BOOL bRecursive = FALSE);
#define IS_FILE_DELETED         0x00000000
#define IS_FILE_NOT_FOUND       0x00000001
#define IS_FILE_IN_USE          0x00000003

#define IS_DIR_REMOVED          IS_FILE_DELETED
#define IS_DIR_NOT_FOUND        IS_FILE_NOT_FOUND
#define IS_DIR_NOT_EMPTY        IS_FILE_IN_USE


