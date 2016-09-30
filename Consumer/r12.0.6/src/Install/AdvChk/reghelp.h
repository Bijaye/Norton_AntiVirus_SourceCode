// reghelp.h
//
// registry helper function declaration

#ifndef __REGHELP_H__
#define __REGHELP_H__

#include <shellapi.h>

/////////////////////////////////////////////////////////////////////
// definitions

// defaults
#define DEFAULT_SYMANTEC               _T("Symantec")
#define DEFAULT_SYMANTEC_SHARED        _T("Symantec Shared")
#define DEFAULT_COMMONFILES            _T("Common Files")
#define DEFAULT_PROGRAMFILES           _T("Program Files")
#define DEFAULT_COMMONFILESDIR         DEFAULT_PROGRAMFILESDIR _T("\\") DEFAULT_COMMONFILES

/////////////////////////////////////////////////////////////////////
// function prototypes

LONG RegRecursiveDeleteKey(HKEY hParentKey, LPCTSTR szKeyName);
LONG RegSafeRecursiveDeleteKey(HKEY hParentKey, LPCTSTR szKeyName);
BOOL RegKeyHasValues(HKEY hKeyParent, LPCTSTR szKeyName);
BOOL RegKeyHasSubKeys(HKEY hKeyParent, LPCTSTR szKeyName);
BOOL RegGetProgramFilesDir(LPTSTR pszPath, UINT cbSize);
BOOL RegGetCommonFilesDir(LPTSTR pszPath, UINT cbSize);
BOOL RegGetSymantecDir(LPTSTR pszPath, UINT cbSize);
BOOL RegGetSymantecSharedDir(LPTSTR pszPath, UINT cbSize);

/////////////////////////////////////////////////////////////////////
//Stuff for installshield type processing
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////



LPTSTR BSlash(LPTSTR szString0, LPCTSTR szString1, LPCTSTR szString2);
/////////////////////////////////////////////////////////////////////
//Small addition to TCHAR so that we can use the = operator.
class myTCHAR
{
public:
	myTCHAR() {};
	myTCHAR(LPTSTR szString) { _tcscpy(m_szData, szString); };

	const myTCHAR& operator=(LPCTSTR szString) {_tcscpy(m_szData, szString); return(*this); };
	
	operator LPTSTR() const { return (LPTSTR)m_szData; };
//	const myTCHAR& operator^(TCHAR szString[]);
//	const myTCHAR& operator^(TCHAR szString[]);

private:
	TCHAR m_szData[MAX_PATH];
};

//Defines used for Registry Functions

#define REGDB_STRING REG_SZ
#define REGDB_BINARY REG_BINARY
#define REGDB_NUMBER REG_DWORD

/////////////////////////////////////////////////////////////////////
//Functions for Installshield type processing
void RegDBSetDefaultRoot(HKEY hRoot);

BOOL RegDBCreateKeyEx(LPCTSTR szKey, LPCTSTR szClass);
BOOL RegDBSetKeyValueEx (LPCTSTR szKey, LPCTSTR szName, const int nType, LPCTSTR szValue, int nSize);
BOOL RegDBGetKeyValueEx (LPCTSTR szKey, LPCTSTR szName, DWORD& nvType, LPTSTR svValue, DWORD& nvSize);
BOOL RegDBDeleteKeyValue (LPCTSTR szKey, LPCTSTR szName);
BOOL RegDBDeleteKey(LPCTSTR szKey);

#endif
