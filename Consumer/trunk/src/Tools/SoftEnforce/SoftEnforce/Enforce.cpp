////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include ".\enforce.h"
#include "SoftEnforceConst.h"
#include "ProductLine.h"


Enforce::Enforce(void)
{
}

Enforce::~Enforce(void)
{
}
bool Enforce::SoftCheck(LPCTSTR subKey)
{
	
	HKEY hKey = NULL;
	CString fullKey =_T("");
	fullKey.Format("%s%s", COMMONKEY, subKey);
	
	if (ERROR_SUCCESS==RegOpenKeyEx(HKEY_LOCAL_MACHINE, fullKey, 0, KEY_READ, &hKey))
	{
		RegCloseKey(hKey);
		return true;
	}
	else
		return false;
}
bool Enforce::CreateKeys(LPCTSTR subKey)
{
	HKEY hKey = NULL;
	DWORD dwcPath = 1024;

	CString fullKey = _T("");
	fullKey.Format("%s%s", COMMONKEY, subKey);

	if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_LOCAL_MACHINE, fullKey, 0, NULL, REG_OPTION_NON_VOLATILE,KEY_WRITE, NULL, &hKey,&dwcPath))
	{
		RegCloseKey(hKey);
		return true;
	}
	else
		return false;
}
bool Enforce::SetValues(LPCTSTR subKey)
{
	//we need to add 2 string values in order for this to work with NAV 07
	//UninstallString and DisplayName
	HKEY hKey = NULL;

	CString fullKey= _T("");
	fullKey.Format("%s%s", COMMONKEY, subKey);
	
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, fullKey, 0, KEY_SET_VALUE, &hKey))
	{
		return false;
	}
	else
	{
		//add a display name
		if (ERROR_SUCCESS != RegSetValueEx(hKey, DISPLAYNAME, 0, REG_SZ, LPBYTE(subKey), (lstrlen(subKey) + 1) * sizeof(TCHAR)))
		{
			RegCloseKey(hKey);
			return false;
		}
		//add an empty string
		if (ERROR_SUCCESS != RegSetValueEx(hKey, UNINSTALLSTRING, 0, REG_SZ, LPBYTE(_T("")), (lstrlen(_T("")) + 1) * sizeof(TCHAR)))
		{
			RegCloseKey(hKey);
			return false;
		}
			
	}
	RegCloseKey(hKey);
	return true;
}

void Enforce::LoadData()
{
	FILE * pFile;
	CString strTemp = "";
	m_vProduct.clear();
	m_vData.clear();
	m_strName = _T("");

	pFile = fopen(_T("Data.csv"), "r");

	if (pFile == NULL)
	{
		//fprintf( stdout, "can not open input file! \n");
		//return false;
	}else{
		

		while(1)
		{
			char charTemp = (char) fgetc(pFile);
			
			if (feof(pFile))
			{
				break;
			}

			if (charTemp != ',' && charTemp != '\0' && charTemp != '\n')
			{
				strTemp.AppendChar(charTemp);
			}
			else if (charTemp == ',')
			{
				if (strTemp != _T(""))
					m_vProduct.push_back(strTemp);
				strTemp.Format(_T(""));	
			}
			else if (charTemp == '\n')
			{
				if (strTemp != _T(""))
					m_vProduct.push_back(strTemp);
				m_vData.push_back(m_vProduct);
				strTemp.Format(_T(""));
				m_vProduct.clear();
			}
		}

		fclose (pFile);

	}
	
}
void Enforce::EditData()
{
	//CString strINIFilename;
	//m_edtINIFilename.GetWindowText( strINIFilename );

	::ShellExecute( NULL, _T("open"), _T("Data.csv"), NULL, NULL, SW_SHOWNORMAL);

}
void Enforce::SaveData()
{

}