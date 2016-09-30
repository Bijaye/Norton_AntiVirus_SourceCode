#ifndef     _ISUTIL_H_
#define     _ISUTIL_H_


/////////////////////////////////////////////////////////////////////////////
//  Misc defines
/////////////////////////////////////////////////////////////////////////////


#define IS_FILE_DELETED         0x00000000
#define IS_FILE_NOT_FOUND       0x00000001
#define IS_FILE_IN_USE          0x00000003

#define IS_DIR_REMOVED          IS_FILE_DELETED
#define IS_DIR_NOT_FOUND        IS_FILE_NOT_FOUND
#define IS_DIR_NOT_EMPTY        IS_FILE_IN_USE

#define RESOURCE_TYPE			_T("BINARYRC")
#define MEMORY_SCAN				_T("MemScan")


/////////////////////////////////////////////////////////////////////////////
//  Function prototypes from ISUtil.CPP
/////////////////////////////////////////////////////////////////////////////

BOOL WINAPI DirectoryExists(LPTSTR szDir);

DWORD WINAPI ISRemoveDirectory(LPTSTR lpDir);

BOOL WINAPI LaunchAndWait(LPSTR lpExe, LPSTR lpParams);

BOOL ExtractFiles(HINSTANCE hInst, LPTSTR szTempFolder);

DWORD WINAPI CreateResourceFile(HINSTANCE hInst, UINT uResourceId,
                                LPTSTR lpType, LPTSTR szFileName, LPTSTR lpOutputFile);

DWORD WINAPI ISDeleteFile(LPTSTR szDir);

/////////////////////////////////////////////////////////////////////////////
//  Inline utility functions
/////////////////////////////////////////////////////////////////////////////

#endif                                  // ifndef _this_file_