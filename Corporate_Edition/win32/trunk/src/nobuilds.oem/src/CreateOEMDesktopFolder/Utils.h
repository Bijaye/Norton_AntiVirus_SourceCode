// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// Utils.h: interface for the CUtils class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UTILS_H__54785EA2_E6A6_11D2_8B29_0050041850C1__INCLUDED_)
#define AFX_UTILS_H__54785EA2_E6A6_11D2_8B29_0050041850C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <shlobj.h>
#include <tchar.h>
#include <stdio.h>
#include <io.h>

// CreateFontEz Defines
#define CFEZ_ATTR_BOLD          1
#define CFEZ_ATTR_ITALIC        2
#define CFEZ_ATTR_UNDERLINE     4
#define CFEZ_ATTR_STRIKEOUT     8

namespace CUtils  
{
	// Registry utils
	BOOL SetRegistryKey(HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValue, DWORD dwLen);
	BOOL GetRegistryKey(HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszOnValue, DWORD dwLen);
	BOOL SetRegistryKey(HKEY hKey, LPCTSTR pszSubKey, DWORD dwValue);
	BOOL GetRegistryKey(HKEY hKey, LPCTSTR pszSubKey, DWORD dwOnValue);
	LONG GetRegistryKeyValue(HKEY key, LPCTSTR lpszSubkey, LPTSTR lpszValue, UINT nMaxTextLen);
	LONG GetRegistryKeyValue(HKEY key, LPCTSTR lpszSubkey, DWORD* pdwValue);

	// Shell utils
	BOOL CreateShortcut(LPCTSTR lpszDesc, LPCTSTR lpszObjPath, LPCTSTR lpszLinkPath, LPCTSTR lpszIconFile=NULL, DWORD dwIconIndex=0, LPCTSTR lpszArguments=NULL);
	BOOL CreateShortcut(LPCTSTR lpszDesc, LPCTSTR lpszObjPath, int nCSIDLFolder, LPCTSTR lpszFile);
	BOOL CreateShortcut(LPCTSTR lpszDesc, LPCTSTR lpszObjPath, int nCSIDLFolder, UINT nFileNameID, HINSTANCE hInst);
	BOOL GetSystemPath(int nCSIDLFolder, LPTSTR pszPath);
	BOOL FileExistsInSysPath(int nCSIDLFolder, LPTSTR pszFileName);
	BOOL LnkExistsInSysPath(int nCSIDLFolder, UINT nLinkNameID, HINSTANCE hInst);
	BOOL DeleteFileInSysPath(int nCSIDLFolder, LPTSTR pszFileName);
	BOOL DeleteLnkInSysPath(int nCSIDLFolder, UINT nLinkNameID, HINSTANCE hInst);

	// OS Utils
	BOOL IsOSGoldOrOSR2();
	BOOL IsSystemHCMode();

	// Misc Utils
	HFONT CreateFontEz(HDC hdc, UINT nFaceNameID, DWORD dwAddAttrib = 0, HINSTANCE hInst = (HINSTANCE)INVALID_HANDLE_VALUE);
	HFONT CreateFontEz(HDC hdc, LPTSTR szFaceName, UINT uPtSize, DWORD dwAttributes = 0);
	HFONT CreateFontEz(HDC hdc, UINT nFaceNameID, HINSTANCE hInst, UINT uPtSize, DWORD dwAttributes = 0);
	BYTE GetDBCSCharSet();
	UINT GetID(UINT nID);
	HINSTANCE GotoURL(LPCTSTR pszUrl, int nShowcmd);
	HINSTANCE GotoURL(UINT nUrlID, int nShowcmd, HINSTANCE hInst);
	BOOL IsWebBrowserInstalled();
	int ResMessageBox(HWND hWnd, UINT nMessageID, UINT nCaptionID, UINT nType, HINSTANCE hInst);
	int ResSMessageBox(HWND hWnd, UINT nFormatStringID, UINT nCaptionID, UINT nType, HINSTANCE hInst, ...);
	BOOL GetAppPath(LPCTSTR lpszAppExeName, LPTSTR pszPath);
	BOOL IsAppInstalled(LPCTSTR lpszFileName);
	HINSTANCE LaunchApp(LPCTSTR lpszFileName);
	DWORD FindVersion(HKEY hStartAt, LPTSTR pszStartAt);

	// Color functions
	COLORREF RGBtoPALRGB(COLORREF cr);
	COLORREF HLStoRGB(DOUBLE dHue, DOUBLE dLuminance, DOUBLE dSaturation);
	VOID RGBtoHLS(COLORREF clrRGB, DOUBLE *pdHue, DOUBLE *pdLuminance, DOUBLE *pdSaturation);

	// Symantec specific functions
	BOOL GetSymAppPath(LPCTSTR lpszAppName, LPTSTR pszPath);
	BOOL IsSymAppInstalled(LPCTSTR lpszAppName, LPCTSTR lpszFileName);
	DWORD GetSymAppVersion(LPTSTR lpszAppRegName);
	HINSTANCE LaunchSymApp(LPCTSTR lpszAppName, LPCTSTR lpszFileName);
	BOOL IsSpeedStartEnabled();
	BOOL IsSpeedStartVisible();
	BOOL EnableSpeedStart(BOOL bEnable = TRUE, BOOL bVisible = TRUE);
	BOOL IsNAVAPEnabled();
	BOOL EnableNAVAP(BOOL bEnable);
	BOOL IsCrashGuardEnabled();
	BOOL EnabledCrashGuard(BOOL bEnable);
	BOOL IsNPRBEnabled();
	void EnableNPRB(BOOL bEnable);
};

#endif // !defined(AFX_UTILS_H__54785EA2_E6A6_11D2_8B29_0050041850C1__INCLUDED_)
