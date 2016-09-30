// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include "stdafx.h"
#include "RegKey.h"
#include "AppInfo.h"
#include "SymSaferRegistry.h"

#define DUMMY_NAME					"DUMMY_@#$!"
#define TILDA						"~"


HKEY GetRegKeyHandle(LPCSTR sHandle)
{
	HKEY hKey = HKEY_LOCAL_MACHINE;
	CString sTemp(sHandle);

	sTemp.MakeUpper();
	if (sTemp == "HKCU")
		hKey = HKEY_CURRENT_USER;
	else if (sTemp == "HKCR")
		hKey = HKEY_CLASSES_ROOT;
	else if (sTemp == "HKUR")
		hKey = HKEY_USERS;

	return hKey;
}


DWORD RegKeyCreateValue(HKEY hKey,LPCSTR sKey,LPCSTR sName,DWORD dwType,LPBYTE pValue,DWORD dwSize,LPCSTR pServer,LPTM pTrans)
{
	DWORD rtnVal,disp;
	TCHAR className[] = EMPTY;
	HKEY regKey;

	if (dwType == REG_SZ)
		dwSize = strlen((LPSTR)pValue)+1;
	else if (!dwSize && dwType == REG_DWORD)
		dwSize = sizeof(DWORD);

	// need to make sure to use direct RegKey functions in case service is down
	if (!pServer || !GetComputerName().CompareNoCase(pServer))
	{
		CString sTemp(sKey);
		if (sTemp.IsEmpty() || sTemp.Left(1) == TILDA)
		{
			if (sTemp.Mid(1).IsEmpty())
				sTemp = REGHEADER;
			else
				sTemp = REGHEADER + sTemp.Mid(1);
		}

		if (!(rtnVal = RegCreateKeyEx(hKey,sTemp,0,className,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,0,&regKey,&disp)))
		{
			if (!(rtnVal = SymSaferRegQueryValueEx(regKey,sName,0,NULL,pValue,&dwSize)))
				rtnVal = ERROR_ALREADY_EXISTS;
			else if (rtnVal == ERROR_FILE_NOT_FOUND)
				rtnVal = RegSetValueEx(regKey,sName,0,dwType,pValue,dwSize);
			RegCloseKey(regKey);
		}
	}
	else if (hKey == HKEY_LOCAL_MACHINE && pTrans)
	{
		CString sTemp(pValue);
		LPBYTE pTmpValue = (LPBYTE)sTemp.GetBuffer(MBUFFER);
		rtnVal = pTrans->RegKeyGetValue(sKey,sName,pTmpValue,MBUFFER,pServer);
		sTemp.ReleaseBuffer();

		if (rtnVal == ERROR_VALUE_NOT_FOUND || rtnVal == ERROR_BLOCK_NOT_READ)
			rtnVal = pTrans->RegKeySetValue(sKey,sName,dwType,pValue,dwSize,pServer);
		else
			rtnVal = REG_OPENED_EXISTING_KEY;
	}
	else
		rtnVal = ERROR_INIT;
	return rtnVal;
}


DWORD RegKeyGetValue(HKEY hKey,LPCSTR sKey,LPCSTR sName,LPBYTE pValue,DWORD dwSize,LPCSTR pServer,LPTM pTrans,LPDWORD pSize)
{
	DWORD rtnVal,dwType;
	HKEY regKey;


	// need to make sure to use direct RegKey functions in case service is down
	if (!pServer || !GetComputerName().CompareNoCase(pServer))
	{
		CString sTemp(sKey);
		if (sTemp.IsEmpty() || sTemp.Left(1) == TILDA)
		{
			if (sTemp.Mid(1).IsEmpty())
				sTemp = REGHEADER;
			else
				sTemp = REGHEADER + sTemp.Mid(1);
		}

		if (!(rtnVal = RegOpenKeyEx(hKey,sTemp,0,KEY_QUERY_VALUE,&regKey)))
		{
			rtnVal = SymSaferRegQueryValueEx(regKey,sName,0,&dwType,pValue,&dwSize);
			if (!rtnVal && pSize)
				*pSize = dwSize;
			RegCloseKey(regKey);
		}
	}
	else if (hKey == HKEY_LOCAL_MACHINE && pTrans)
	{
		rtnVal = pTrans->RegKeyGetValue(sKey,sName,pValue,dwSize,pServer);
	}
	else
		rtnVal = ERROR_INIT;
	return rtnVal;
}	


DWORD RegKeySetValue(HKEY hKey,LPCSTR sKey,LPCSTR sName,DWORD dwType,LPBYTE pValue,DWORD dwSize,LPCSTR pServer,LPTM pTrans)
{
	HKEY regKey;
	TCHAR className[] = "";
	DWORD rtnVal,disp;

	if (dwType == REG_SZ)
		dwSize = strlen((LPSTR)pValue)+1;
	else if (!dwSize && dwType == REG_DWORD)
		dwSize = sizeof(DWORD);

	// need to make sure to use RegSetValueEx in case service is down
	if (!pServer || !GetComputerName().CompareNoCase(pServer))
	{
		CString sTemp(sKey);
		if (sTemp.IsEmpty() || sTemp.Left(1) == TILDA)
		{
			if (sTemp.Mid(1).IsEmpty())
				sTemp = REGHEADER;
			else
				sTemp = REGHEADER + sTemp.Mid(1);
		}

		if (!(rtnVal = RegCreateKeyEx(hKey,sTemp,0,className,REG_OPTION_NON_VOLATILE,KEY_SET_VALUE,0,&regKey,&disp)))
		{
			rtnVal = RegSetValueEx(regKey,sName,0,dwType,pValue,dwSize);
			RegCloseKey(regKey);
		}
	}
	else if (hKey == HKEY_LOCAL_MACHINE && pTrans)
	{
		rtnVal = pTrans->RegKeySetValue(sKey,sName,dwType,pValue,dwSize,pServer);
	}
	else
		rtnVal = ERROR_INIT;
	return rtnVal;
}


DWORD RegKeyCreate(HKEY hKey,LPCSTR sKey,LPCSTR pServer,LPTM pTrans)
{
	HKEY regKey;
	TCHAR className[] = "";
	DWORD rtnVal,disp;

	// need to make sure to use RegSetValueEx in case service is down
	if (!pServer || !GetComputerName().CompareNoCase(pServer))
	{
		CString sTemp(sKey);
		if (sTemp.IsEmpty() || sTemp.Left(1) == TILDA)
		{
			if (sTemp.Mid(1).IsEmpty())
				sTemp = REGHEADER;
			else
				sTemp = REGHEADER + sTemp.Mid(1);
		}

		if (!(rtnVal = RegCreateKeyEx(hKey,sTemp,0,className,REG_OPTION_NON_VOLATILE,KEY_SET_VALUE,0,&regKey,&disp)))
		{
			RegCloseKey(regKey);
		}
	}
	else if (hKey == HKEY_LOCAL_MACHINE && pTrans)
	{
		DWORD dwTemp = 0;
		rtnVal = pTrans->RegKeySetValue(sKey,DUMMY_NAME,REG_DWORD,(LPBYTE)&dwTemp,4,pServer);
		rtnVal = pTrans->RegKeyDeleteValue(pServer,sKey,DUMMY_NAME);
	}
	else
		rtnVal = ERROR_INIT;
	return rtnVal;
}

BOOL RegKeyExists(HKEY hKey,LPCSTR sKey,LPCSTR pServer,LPTM pTrans)
{
	// need to make sure to use RegSetValueEx in case service is down
	if (!pServer || !GetComputerName().CompareNoCase(pServer))
	{
		HKEY regKey;
		CString sTemp(sKey);
		if (sTemp.IsEmpty() || sTemp.Left(1) == TILDA)
		{
			if (sTemp.Mid(1).IsEmpty())
				sTemp = REGHEADER;
			else
				sTemp = REGHEADER + sTemp.Mid(1);
		}

		if (!(RegOpenKeyEx(hKey,sTemp,0,KEY_ALL_ACCESS,&regKey)))
		{
			RegCloseKey(regKey);
			return TRUE;
		}
	}
	else if (hKey == HKEY_LOCAL_MACHINE && pTrans)
	{
	}
	return FALSE;
}

DWORD RegKeyDeleteAll(HKEY hKey, LPCSTR sKeyName)
{
	DWORD dwSize=MBUFFER, dwIndex=0,rtnVal;
	FILETIME fTime;
	TCHAR sName[MBUFFER];
	HKEY regKey;

	if (!(rtnVal = RegOpenKeyEx(hKey,sKeyName,0,KEY_ALL_ACCESS,&regKey)))
	{
		while (RegEnumKeyEx(regKey,dwIndex,sName,&dwSize,NULL,NULL,NULL,&fTime) != ERROR_NO_MORE_ITEMS)
		{
			rtnVal = RegKeyDeleteAll(regKey,sName);
			dwSize = MBUFFER;
		}
		RegCloseKey(regKey);
	}
	if (!rtnVal)
		rtnVal = RegDeleteKey(hKey,sKeyName);
	return rtnVal;
}


DWORD RegKeyDelete(HKEY hKey,LPCSTR sKey,LPCSTR pServer,LPTM pTrans)
{
	HKEY regKey;
	DWORD rtnVal;

	if (!pServer || !GetComputerName().CompareNoCase(pServer))
	{
		CString sTemp(sKey);
		if (sTemp.IsEmpty() || sTemp.Left(1) == TILDA)
		{
			if (sTemp.Mid(1).IsEmpty())
				sTemp = REGHEADER;
			else
				sTemp = REGHEADER + sTemp.Mid(1);
		}

		CString sName(GetFileName(sTemp));
		if (!sTemp.IsEmpty())
			sTemp = GetDirPath(sTemp);

		if (!(rtnVal = RegOpenKeyEx(hKey,sTemp,0,KEY_ALL_ACCESS,&regKey)))
		{
			rtnVal = RegKeyDeleteAll(regKey,sName);
			RegCloseKey(regKey);
		}
	}
	else if (hKey == HKEY_LOCAL_MACHINE && pTrans)
	{
		rtnVal = pTrans->RegKeyDelete(pServer,sKey);
	}
	else
		rtnVal = ERROR_INIT;
	return rtnVal;
}


DWORD RegKeyDeleteValue(HKEY hKey,LPCSTR sKey,LPCSTR sName,LPCSTR pServer,LPTM pTrans)
{
	HKEY regKey;
	DWORD rtnVal;

	if (!pServer || !GetComputerName().CompareNoCase(pServer))
	{
		CString sTemp(sKey);
		if (sTemp.IsEmpty() || sTemp.Left(1) == TILDA)
		{
			if (sTemp.Mid(1).IsEmpty())
				sTemp = REGHEADER;
			else
				sTemp = REGHEADER + sTemp.Mid(1);
		}

		if (!(rtnVal = RegOpenKeyEx(hKey,sTemp,0,KEY_SET_VALUE,&regKey)))
		{
			rtnVal = RegDeleteValue(regKey,sName);
			RegCloseKey(regKey);
		}
	}
	else if (hKey == HKEY_LOCAL_MACHINE && pTrans)
	{
		rtnVal = pTrans->RegKeyDeleteValue(pServer,sKey,sName);
	}
	else
		rtnVal = ERROR_INIT;
	return rtnVal;
}

BOOL RegKeyIsEmpty(HKEY hKey, LPCSTR sKeyName)
{
	DWORD dwSize=MBUFFER, dwSize1=MBUFFER;
	FILETIME fTime;
	TCHAR sName[MBUFFER];
	HKEY regKey;
	BOOL bEmpty = FALSE;

	if (!RegOpenKeyEx(hKey,sKeyName,0,KEY_ALL_ACCESS,&regKey))
	{
		if (RegEnumKeyEx(regKey,0,sName,&dwSize,NULL,NULL,NULL,&fTime) == ERROR_NO_MORE_ITEMS &&
			RegEnumValue(regKey,0,sName,&dwSize1,NULL,NULL,NULL,NULL) == ERROR_NO_MORE_ITEMS)
		{
			bEmpty = TRUE;
		}
		RegCloseKey(regKey);
	}
	return bEmpty;
}

DWORD RegKeyDeleteEmptyKey(HKEY hKey, LPCSTR sKeyName)
{
    DWORD rtnVal = ERROR_NO_MORE_ITEMS;  // default to "no more", in this case no more to delete.

    if (RegKeyIsEmpty(hKey,sKeyName))
		rtnVal = RegDeleteKey(hKey,sKeyName);

    return rtnVal;
}


DWORD StopWin95Service()
{
	int cTime=0;
	DWORD dwValue=4;
	DWORD error;
	CString sKey("~\\ProductControl");
	MSG msg;

	while (dwValue != 3 && dwValue != 0 && cTime < 180)
	{
		error = RegKeyGetValue(HKLM,sKey,"ServiceStatus",(LPBYTE)&dwValue,4,NULL);
		
		if (!error && dwValue == 0)
			return 0;

		if (!error && dwValue == 1)
		{
			dwValue = 2;
			error = RegKeySetValue(HKLM,sKey,"ServiceStatus",REG_DWORD,(LPBYTE)&dwValue,4,NULL);
		}
		if (error)
			return error;

		while(PeekMessage(&msg,NULL,NULL,NULL,PM_REMOVE))       
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		cTime ++;
		Sleep(1000);
	}
	if (dwValue != 3)
		return ERROR_TIMEOUT;
	Sleep(2500);
	return 0;
}
