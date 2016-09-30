////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "startbrowser.h"

namespace NAVToolbox
{
CStartBrowser::CStartBrowser(void)
{
}

bool CStartBrowser::ShowURL ( LPCTSTR lpszURL )
{
    TCHAR szApplication[MAX_PATH] = {0};
    TCHAR szCommandLine[MAX_PATH + 128] = {0};
	STARTUPINFO	si;
	PROCESS_INFORMATION	pi;

	if (getExtensionApp( szApplication, sizeof(szApplication)/sizeof(TCHAR)))
	{
		if (!percentArgs(szCommandLine, szApplication, lpszURL))
		{
			// didn't have %1, append " ",URL
			lstrcat(szCommandLine, _T(" "));
			lstrcat(szCommandLine, lpszURL);
		}

		memset(&si, 0, sizeof(si));
		si.cb = sizeof(si);
		si.wShowWindow = SW_SHOW;

		if (CreateProcess(0, szCommandLine, 0, 0, FALSE, 0, 0, 0, &si, &pi))
		{
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
        else
        {
            return false;
        }
	}

    return true;
}

CStartBrowser::~CStartBrowser(void)
{
}


bool CStartBrowser::getExtensionApp(LPTSTR szApplication, int iSizeApp)
{
	TCHAR szKey[128] = {0};
	bool bSuccess = false;

    if (getRegValue(HKEY_CLASSES_ROOT, _T(".html"), 0, (LPBYTE)szKey, 
		sizeof(szKey)/sizeof(TCHAR)))
	{
		_tcscat(szKey, _T("\\shell\\open\\command"));

		if (getRegValue(HKEY_CLASSES_ROOT, szKey, 0, (LPBYTE)szApplication, iSizeApp))
		{
			bSuccess = true;
		}
	}

	return bSuccess;
}


// This routine formats a string given %N arguments that represent
// substitutions (like printf()) of the Nth [1..9] string parameter.
//
// For example, the call percentArgs(string, "%2 %1", "one", "two"),
// places the string "two one" into "string".
bool CStartBrowser::percentArgs(LPTSTR szString, LPTSTR szFormat, ...)
{
	va_list	ap;
	int	argN;
	TCHAR* argS = NULL;
	bool bArgSubstituted = false;

	while(*szFormat)
	{
		if(*szFormat == '%')
		{
			szFormat++;
			if(*szFormat == '%')
			{
				*szString++ = '%';
			}
			else if('1' <= *szFormat && *szFormat <= '9')
			{
				argN = *szFormat++ - '0';
				va_start(ap, szFormat);
				while(argN--)
				{
					argS = va_arg(ap, LPTSTR);
				}

				va_end(ap);
				if(argS)
				{
					while(*argS)
					{
						*szString++ = *argS++;
					}

					bArgSubstituted = true;
				}
			}
			else
			{
				*szString++ = *szFormat++;
			}
		}
		else
		{
			*szString++ = *szFormat++;
		}
	}
	*szString = '\0';

	return bArgSubstituted;
}


bool CStartBrowser::getRegValue(HKEY hKey, LPTSTR szSubKey, LPTSTR szValueKey, LPBYTE lpValueData, DWORD dwValueSize)
{
	bool bSuccess = false;
	DWORD dwType;
	HKEY hQueryKey;

	if (RegOpenKeyEx(hKey, szSubKey, 0, KEY_QUERY_VALUE, &hQueryKey) == ERROR_SUCCESS)
	{
		if (RegQueryValueEx(hQueryKey, szValueKey, 0, &dwType, lpValueData, 
				&dwValueSize) == ERROR_SUCCESS)
		{
			bSuccess = true;
		}

		RegCloseKey(hQueryKey);
	}

	return bSuccess;
}

} // namespace NAVToolbox
