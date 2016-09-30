// reghelp.cpp
//
// registry helper function implementation

#include "stdafx.h"
#include "reghelp.h"
#include "resource.h"
#include "..\advchkres\resource.h"
#include "regstr.h"

extern HINSTANCE ghInstance;     // dllmain.cpp
extern CAtlBaseModule _Module;
HKEY g_DefaultRoot = HKEY_LOCAL_MACHINE;

void RegDBSetDefaultRoot(HKEY hRoot)
{
	g_DefaultRoot = hRoot;
};

/////////////////////////////////////////////////////////////////////
// recursive delete key
LONG RegRecursiveDeleteKey(HKEY hParentKey, LPCTSTR szKeyName)
{
   TCHAR szSubKeyName[MAX_PATH] = {0};
   HKEY  hCurrentKey;
   LONG  lResult;

   // open specified key to check for subs
   if ((lResult = RegOpenKey(hParentKey, szKeyName, &hCurrentKey)) == ERROR_SUCCESS)
   {
      // Remove all subkeys of the key to delete
      while ((lResult = RegEnumKey(hCurrentKey, 0, szSubKeyName, sizeof(szSubKeyName))) ==	ERROR_SUCCESS)
      {
         if ((lResult = RegRecursiveDeleteKey(hCurrentKey, szSubKeyName)) != ERROR_SUCCESS)
            break;
      };

      // If all went well, we should now be able to delete the requested key
      if ((lResult == ERROR_NO_MORE_ITEMS) || (lResult == ERROR_BADKEY))
         lResult = RegDeleteKey(hParentKey, szKeyName);

	  // close up to prevent leaks
	  RegCloseKey(hCurrentKey);
   }


   return lResult;
};
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// recursive delete key (all subkeys only if no values)
LONG RegSafeRecursiveDeleteKey(HKEY hParentKey, LPCTSTR szKeyName)
{
   TCHAR   szSubKeyName[MAX_PATH] = {0};
   BOOL    fContinue = FALSE;
   LONG    lResult;
   HKEY    hCurrentKey;

   // open specified key to check for subs
   if ((lResult = RegOpenKey(hParentKey, szKeyName, &hCurrentKey)) == ERROR_SUCCESS)
   {
      // Remove all subkeys of the key to delete
      while ((lResult = RegEnumKey(hCurrentKey, 0, szSubKeyName, sizeof(szSubKeyName))) ==	ERROR_SUCCESS)
      {
         if ((lResult = RegSafeRecursiveDeleteKey(hCurrentKey, szSubKeyName)) != ERROR_SUCCESS)
            break;
      };

      // check that there are no values on the current key
      if (((lResult == ERROR_NO_MORE_ITEMS) ||
           (lResult == ERROR_BADKEY)) &&
          !RegKeyHasValues(hParentKey, szKeyName) &&
          !RegKeyHasSubKeys(hParentKey, szKeyName))
      {
         RegDeleteKey(hParentKey, szKeyName);
      };

	  // close up to prevent leaks
	  RegCloseKey(hCurrentKey);
   }


   return lResult;
};
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// determine if a registry key has values
BOOL RegKeyHasValues(HKEY hParentKey, LPCTSTR szKeyName)
{
   BOOL fResult = FALSE;
   HKEY hKey = NULL;
   if (RegOpenKey(hParentKey, szKeyName, &hKey) == ERROR_SUCCESS)
   {
      TCHAR szName[255];
      DWORD cbName = sizeof(szName);
      fResult = (RegEnumValue(hKey, 0L, szName, &cbName, NULL, NULL, NULL, NULL) == ERROR_SUCCESS);
	  RegCloseKey(hKey);
   };

   return fResult;
};
/////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////
// determine if a registry key has subkeys
BOOL RegKeyHasSubKeys(HKEY hParentKey, LPCTSTR szKeyName)
{
   BOOL fResult = FALSE;
   HKEY hKey = NULL;
   if (RegOpenKey(hParentKey, szKeyName, &hKey) == ERROR_SUCCESS)
   {
      TCHAR szName[255];
      fResult = (RegEnumKey(hKey, 0L, szName, sizeof(szName)) == ERROR_SUCCESS);
      RegCloseKey(hKey);
   };
   return fResult;
};
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// retrieve windows program files directory

BOOL RegGetProgramFilesDir(LPTSTR pszPath, UINT cbSize)
{
   TCHAR szTemp[ MAX_PATH ] = {0};
   HKEY hKey = NULL;
   LONG lResult = 0L;

   // get CurrentVersion location
   lResult = RegOpenKey(HKEY_LOCAL_MACHINE, REGSTR_PATH_SETUP, &hKey);
   if (lResult == ERROR_SUCCESS)
   {
      if (LoadString(_Module.GetResourceInstance(), REGVAL_PROGRAMFILESDIR, szTemp, sizeof(szTemp)) > 0)
      {
         DWORD dwSize = cbSize;
         lResult = RegQueryValueEx(hKey, szTemp, NULL, NULL, LPBYTE(pszPath), &dwSize);
      }
      else
      {  // bad data
         lResult = ERROR_INVALID_DATA;
      };

	  RegCloseKey(hKey);
   };

   // manufacture if not good to here
   if (lResult != ERROR_SUCCESS)
   {
      // manufacture program files location
      TCHAR szDirName[ MAX_PATH ] = {0};
      GetWindowsDirectory(szDirName, sizeof(szDirName)); //lint !e534
      lstrcpy(szDirName + 3, DEFAULT_PROGRAMFILES);

      // add key by open/create
      if (CreateDirectory(szDirName, NULL))
      {
         lResult = RegCreateKey(HKEY_LOCAL_MACHINE, REGSTR_PATH_SETUP, &hKey);
         if (lResult == ERROR_SUCCESS)
         {
            LoadString(_Module.GetResourceInstance(), REGVAL_PROGRAMFILESDIR, szTemp, sizeof(szTemp));
            lResult = RegSetValueEx(hKey, szTemp, 0L, REG_SZ, LPBYTE(szDirName), (DWORD(lstrlen(szDirName)) + 1UL) * sizeof(TCHAR));
            RegCloseKey(hKey);
         };
      };
   };

   return (lResult == ERROR_SUCCESS);
};


/////////////////////////////////////////////////////////////////////
// retrieve windows common files directory

BOOL RegGetCommonFilesDir(LPTSTR pszPath, UINT cbSize)
{
   TCHAR szTemp[ MAX_PATH ] = {0};
   HKEY hKey = NULL;
   LONG lResult = 0L;

   // get existing Common Files location
   lResult = RegOpenKey(HKEY_LOCAL_MACHINE, REGSTR_PATH_SETUP, &hKey);
   if (lResult == ERROR_SUCCESS)
   {
      if (LoadString(_Module.GetResourceInstance(), REGVAL_COMMONFILESDIR, szTemp, sizeof(szTemp)) > 0)
      {
         DWORD dwSize = cbSize;
         lResult = RegQueryValueEx(hKey, szTemp, NULL, NULL, LPBYTE(pszPath), &dwSize);
      }
      else
      {  // bad data
         lResult = ERROR_INVALID_DATA;
      };

	  RegCloseKey(hKey);
   };

   // manufacture if not good to here
   if (lResult != ERROR_SUCCESS)
   {
      TCHAR szDirName[ MAX_PATH ] = {0};
      if (RegGetProgramFilesDir(szDirName, sizeof(szDirName)))
      {
         // manufacture program files location
         lstrcat(szDirName, _T("\\") DEFAULT_COMMONFILES);
         if (CreateDirectory(szDirName, NULL))
         {
            lResult = RegCreateKey(HKEY_LOCAL_MACHINE, REGSTR_PATH_SETUP, &hKey);
            if (lResult == ERROR_SUCCESS)
            {
               LoadString(_Module.GetResourceInstance(), REGVAL_COMMONFILESDIR, szTemp, sizeof(szTemp));
               lResult = RegSetValueEx(hKey, szTemp, 0L, REG_SZ, LPBYTE(szDirName), (lstrlen(szDirName) + 1) * sizeof(TCHAR));
               RegCloseKey(hKey);
            };
         };
      };
   };

   return (lResult == ERROR_SUCCESS);
};


/////////////////////////////////////////////////////////////////////
// retrieve the symantec directory

BOOL RegGetSymantecDir(LPTSTR pszPath, UINT cbSize)
{
   LONG lResult = ERROR_SUCCESS;
   TCHAR szTemp[ MAX_PATH ] = {0};
   if (LoadString(_Module.GetResourceInstance(), REGKEY_SYMANTECDIR, szTemp, sizeof(szTemp)) > 0)
   {
      HKEY hKey = NULL;
      lResult = RegOpenKey(HKEY_LOCAL_MACHINE, szTemp, &hKey);
      if (lResult == ERROR_SUCCESS)
      {
         if (LoadString(_Module.GetResourceInstance(), REGVAL_SYMANTECDIR_LOCATION1, szTemp, sizeof(szTemp)) > 0)
         {
            DWORD dwSize = cbSize;
            lResult = RegQueryValueEx(hKey, szTemp, NULL, NULL, LPBYTE(pszPath), &dwSize);
         }
         else
         {  // bad data (but how???)
            lResult = ERROR_INVALID_DATA;
         };
		 RegCloseKey(hKey);
      };
   }
   else
   {  // bad data (but how???)
      lResult = ERROR_INVALID_DATA;
   };;

   // default to progfiles/symatnec
   if ((lResult != ERROR_SUCCESS) && RegGetProgramFilesDir(pszPath, cbSize))
   {
      lstrcat(pszPath, _T("\\"));
      lstrcat(pszPath, DEFAULT_SYMANTEC);
      lResult = ERROR_SUCCESS;
   };

   return (lResult == ERROR_SUCCESS);
};


/////////////////////////////////////////////////////////////////////
// retrieve the symantec directory

BOOL RegGetSymantecSharedDir(LPTSTR pszPath, UINT cbSize)
{
   BOOL fResult = FALSE;
   if (RegGetCommonFilesDir(pszPath, cbSize))
   {
      TCHAR szTemp[ MAX_PATH ] = {0};
      lstrcat(pszPath, _T("\\") DEFAULT_SYMANTEC_SHARED);
      fResult = TRUE;
   }
   return fResult;
}


LPTSTR BSlash(LPTSTR szString0, LPCTSTR szString1, LPCTSTR szString2)
{

	if(_T('\\') == szString1[strlen(szString1) -1])
	{
		if(szString2[0] == _T('\\'))
		{
			_tcscpy(szString0, szString1);
			_tcscat(szString0, &szString2[1]);
		}
		else
		{
			_tcscpy(szString0, szString1);
			_tcscat(szString0, szString2);
		}
	}
	else
	{
		if(szString2[0] == _T('\\'))
		{
			_tcscpy(szString0, szString1);
			_tcscat(szString0, szString2);
		}
		else
		{
			_tcscpy(szString0, szString1);
			_tcscat(szString0, _T("\\"));
			_tcscat(szString0, szString2);
		}
	}
	return(szString0);
}


BOOL RegDBCreateKeyEx(LPCTSTR szKey, LPCTSTR szClass)
{

	HKEY tmpKey = NULL;
	TCHAR szTmpKey[MAX_PATH] = {0};

	//Note: this isn't DBCS safe, but this entry in the registry is never a DBCS character

	if(szKey[0] == '\\')
		_tcscpy(szTmpKey, &szKey[1]);
	else
		_tcscpy(szTmpKey, szKey);


	DWORD dwRetval = RegCreateKey(g_DefaultRoot, szTmpKey, &tmpKey);
	RegCloseKey(tmpKey);


#ifdef _DEBUG
	if(dwRetval != ERROR_SUCCESS)
	{
		TCHAR szMsgBuf[MAX_PATH];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwRetval,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			szMsgBuf,
			MAX_PATH,
			NULL );

		MessageBox( NULL, szMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );

	}
#endif

	return(dwRetval == ERROR_SUCCESS ? TRUE : FALSE);
}

BOOL RegDBSetKeyValueEx (LPCTSTR szKey, LPCTSTR szName, const int nType, LPCTSTR szValue, int nSize)
{
	HKEY tmpKey = NULL;

	TCHAR szTmpKey[MAX_PATH] = {0};

	//Note: this isn't DBCS safe, but this entry in the registry is never a DBCS character

	if(szKey[0] == '\\')
		_tcscpy(szTmpKey, &szKey[1]);
	else
		_tcscpy(szTmpKey, szKey);


	if(nSize = -1)
		nSize = _tcslen(szValue);

	if(ERROR_SUCCESS == RegOpenKeyEx(g_DefaultRoot, szTmpKey, 0, KEY_ALL_ACCESS, &tmpKey))
	{
		RegSetValueEx(tmpKey, szName, 0, nType, (BYTE*) szValue, nSize);

		RegCloseKey(tmpKey);
	}
	else
	{
		return(FALSE);
	}

	return(TRUE);
}


BOOL RegDBGetKeyValueEx (LPCTSTR szKey, LPCTSTR szName, DWORD& nvType, LPTSTR svValue, DWORD& nvSize)
{
	HKEY tmpKey = NULL;

	TCHAR szTmpKey[MAX_PATH] = {0};

	//Note: this isn't DBCS safe, but this entry in the registry is never a DBCS character

	if(szKey[0] == '\\')
		_tcscpy(szTmpKey, &szKey[1]);
	else
		_tcscpy(szTmpKey, szKey);


	if(ERROR_SUCCESS == RegOpenKeyEx(g_DefaultRoot, szTmpKey, 0, KEY_ALL_ACCESS, &tmpKey))
	{
		DWORD dwRetval = RegQueryValueEx(tmpKey, szName, 0, &nvType, (BYTE*) svValue, &nvSize);
		if(ERROR_SUCCESS != dwRetval)
		{
#ifdef _DEBUG
			TCHAR szMsgBuf[MAX_PATH];
			FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				dwRetval,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				szMsgBuf,
				MAX_PATH,
				NULL );

			MessageBox( NULL, szMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
#endif
			RegCloseKey(tmpKey);
			return(FALSE);
		}
		RegCloseKey(tmpKey);
	}
	else
	{
		return(FALSE);
	}

	return(TRUE);
}


BOOL RegDBDeleteKeyValue (LPCTSTR szKey, LPCTSTR szName)
{
	HKEY tmpKey = NULL;

	TCHAR szTmpKey[MAX_PATH] = {0};

	//Note: this isn't DBCS safe, but this entry in the registry is never a DBCS character

	if(szKey[0] == '\\')
		_tcscpy(szTmpKey, &szKey[1]);
	else
		_tcscpy(szTmpKey, szKey);


	if(ERROR_SUCCESS == RegOpenKey(g_DefaultRoot, szTmpKey, &tmpKey))
	{
		if(ERROR_SUCCESS != RegDeleteValue(tmpKey, szName))
		{
			RegCloseKey(tmpKey);
			return(FALSE);
		}
	}
	else
	{
		return(FALSE);
	}

	RegCloseKey(tmpKey);
	return(TRUE);
}

BOOL RegDBDeleteKey(LPCTSTR szKey)
{
	TCHAR szTmpKey[MAX_PATH] = {0};

	//Note: this isn't DBCS safe, but this entry in the registry is never a DBCS character

	if(szKey[0] == '\\')
		_tcscpy(szTmpKey, &szKey[1]);
	else
		_tcscpy(szTmpKey, szKey);

	if(ERROR_SUCCESS != RegRecursiveDeleteKey(g_DefaultRoot, szTmpKey))
		return(FALSE);

	return(TRUE);
}


