//-----------------------------------------------------------------------------
//  RegKey.Cpp
//  A component of the MacNuke Project
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
//  Copyright 1997, 2005 (c) Symantec Corp.. All rights reserved.
//  SYMANTEC CONFIDENTIAL
//
//  Revision History
//
//  Who                     When        What / Why
//
//  tedn@thuridion.com	  12/22/03		Commented out variable in CRegistryKey::ReadValueEnum 
//										to get rid of compiler warning.
//
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "RegKey.h"
#include "SymSaferRegistry.h"

CRegistryKey::CRegistryKey(HKEY hKey, LPCTSTR key, BOOL create)
{
	fInHKey = hKey;
	fKey = new TCHAR[lstrlen(key) + 1];
	lstrcpy(fKey, key);
	// Create will set the fIsNewKey private data member
	if (create)
		Create();
}

CRegistryKey::~CRegistryKey() { 
    if( fHKey != NULL ) //AKENNED
        RegCloseKey(fHKey); 
    if( fKey != NULL ) //AKENNED
        delete [] fKey;  
}

LONG CRegistryKey::Create()
{
	fIsNewKey = (ERROR_SUCCESS != Open());
	fLastError = RegCreateKey(fInHKey, fKey, &fHKey);

	return fLastError;
}

LONG CRegistryKey::Open()
{
	fLastError = RegOpenKey(fInHKey, fKey,	&fHKey);

	return fLastError;
}

LONG CRegistryKey::DeleteKey(LPCTSTR key)
{
	const int	MAX_STR = 512;
	LONG		lret;
	TCHAR		keybuf[ MAX_STR ];
	TCHAR		*keybufp;
	DWORD		dwSize = MAX_STR;
	HKEY		hkey;
	FILETIME	ftime;

	// copy key we were passed and add a backslash
	lstrcpyn ( keybuf, key, MAX_STR );
	keybufp = keybuf + lstrlen ( keybuf );
	if ( *( keybufp - 1 ) != '\\' ) *keybufp++ = '\\';

	// open the key we were passed and enumerate its subkeys
	// which are concatenated to the key we were passed
    lret = ::RegOpenKey ( fHKey, key, &hkey );
	if ( lret )
		return lret;

	while ( !lret )
	{
		// recursively delete keys
		dwSize = MAX_STR - ( keybufp - keybuf );
        lret = ::RegEnumKeyEx ( hkey, 0, keybufp, &dwSize, NULL, NULL, NULL, &ftime);
		if (!lret)
			lret = DeleteKey ( keybuf );
	}

	RegCloseKey ( hkey );
	if ( lret == ERROR_NO_MORE_ITEMS )
	{
		// finally delete the fkey we were passed
		return ::RegDeleteKey ( fHKey, key );
	}
	return lret;	// return here if there was an error
}

LONG CRegistryKey::DeleteValue(LPCTSTR value)
{
	return ::RegDeleteValue(fHKey, value);
}
LONG CRegistryKey::GetNumberOfValues()
{
	const int MAX_STR = 512;
	LONG ret = 0;
	LONG number = 0;
	DWORD size;
	TCHAR key[MAX_STR];
	while (ret == 0)
	{
		size = MAX_STR;

		ret = ::RegEnumValue(fHKey, number, key, &size, NULL, NULL, NULL, NULL);
		if (ret == 0)
			number++;
	}
	return number;
}
LONG CRegistryKey::GetNumberOfSubkeys()
{
	const int MAX_STR = 512;
	LONG ret = 0;
	LONG number = 0;
	DWORD size;
	TCHAR key[MAX_STR];
	while (ret == 0)
	{
		size = MAX_STR;
		ret = ::RegEnumKeyEx(fHKey, number, key, &size, NULL, NULL, NULL, NULL);
		if (ret == 0)
			number++;
	}
	return number;
}


LONG CRegistryKey::ReadKeyValue(LPTSTR value, long *valueBufferSize)
{
	fLastError = SymSaferRegQueryValue(fHKey, NULL, (char *)value, valueBufferSize);

	return fLastError;
}

LONG CRegistryKey::ReadValue(LPCTSTR name, LPTSTR value, LPDWORD valueBufferSize)
{
    fLastError = 0; //AKENNED
    if( NULL != fHKey ) { //AKENNED

	    DWORD dataType;
	    fLastError = SymSaferRegQueryValueEx(fHKey, name, NULL, &dataType, (unsigned char *)value, valueBufferSize);

	    unsigned char *val = (unsigned char *)value ;
	    int i = 0 ;
	    if(val != (unsigned char *)0L) {
		    int len = 0 ;
		    int spc = 0 ;
		    int qt  = 0 ;
		    while(val[len] != '\0') {
			    if(val[len] == '\"')
				    qt++ ;
			    else if((val[len] == ' ') && (qt == 1))
				    spc++ ;
			    len++ ;
		    }
		    if(len >= 2) {
			    // Removing only begin and end quotes.
			    if((val[0] == '\"') &&
			       (val[len-1] == '\"') && (qt==2)) {
				    for(i=0;i<len-2;i++)
					    val[i] = val[i+1] ;
				    val[len-2] = '\0';
			    } else if((val[0] == '\"') && (qt > 1) && (spc == 0)) {
				    qt = 1 ;
				    while(val[qt] != '\"')
					    qt++ ;
				    for(i=0;i<qt-1;i++)
					    val[i] = val[i+1] ;
				    for(i=qt-1;i<len-2;i++)
					    val[i] = val[i+1] ;
				    val[len-2] = '\0';
			    }
		    }
	    }

    }
	return fLastError;
}

LONG CRegistryKey::WriteValue(LPCTSTR name, LPTSTR value)
{
	fLastError = RegSetValueEx(fHKey, name, 0, REG_SZ, (LPBYTE) value, lstrlen(value) + 1);
	return fLastError;
}

LONG CRegistryKey::ReadValueEnum(int numberValue, LPTSTR name, LPDWORD nameSize)
{
	//LONG ret = 0;
  return ::RegEnumValue(fHKey, numberValue, name, nameSize, NULL, NULL, NULL, NULL);
}

LONG CRegistryKey::ReadValue(LPCTSTR name, DWORD &numberValue)
{
	DWORD type;
	DWORD bufferSize = sizeof(DWORD);

	fLastError = SymSaferRegQueryValueEx(fHKey, name, NULL, &type, (LPBYTE) &numberValue, &bufferSize);

	return fLastError;
}

LONG CRegistryKey::WriteValue(LPCTSTR name, DWORD numberValue)
{
	fLastError = RegSetValueEx(fHKey, name, 0, REG_DWORD, (LPBYTE) &numberValue, sizeof(DWORD));
	return fLastError;
}

LONG CRegistryKey::ReadValue(LPCTSTR name, LPVOID value, LPDWORD valueSize)
{
	DWORD type;

	fLastError = SymSaferRegQueryValueEx(fHKey, name, NULL, &type, (LPBYTE) value, valueSize);

	return fLastError;
}
LONG CRegistryKey::WriteValue(LPCTSTR name, LPVOID buffer, DWORD bufferSize)
{
	fLastError = RegSetValueEx(fHKey, name, 0, REG_BINARY, (LPBYTE) buffer, bufferSize);
	return fLastError;
}
