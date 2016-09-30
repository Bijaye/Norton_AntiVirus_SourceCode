// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// UserDir.h: interface for the CUserDir class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USERDIR_H__58ECA9CA_2042_45E0_A26D_F7316FB2D021__INCLUDED_)
#define AFX_USERDIR_H__58ECA9CA_2042_45E0_A26D_F7316FB2D021__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define NAV_APP_DATA    1
#define NAV_USER_DATA   2

#define CREATE_USER_LOGS    1
#define READ_USER_LOGS      2

class CUserDir  
{
public:
	DWORD GetAppDataDirectory(LPTSTR lpAppDataDir, size_t nDataDirBytes);
	DWORD GetUserDirectory(LPTSTR lpUserDir, size_t nUserDirBytes);
	BOOL UserDirsNeeded(DWORD dwFlags);
	BOOL UserDirectoryExists(LPTSTR lpUserDir);
	DWORD CreateUserDirectory(LPTSTR lpDataDirectory);
	CUserDir();
	virtual ~CUserDir();

protected:
	DWORD GetClientType();
	DWORD GetSystemDataDir(DWORD dwFlags, LPTSTR lpSystemDataDir);
	BOOL IsWinNT();
	BOOL IsWindows2000();
};

#endif // !defined(AFX_USERDIR_H__58ECA9CA_2042_45E0_A26D_F7316FB2D021__INCLUDED_)
