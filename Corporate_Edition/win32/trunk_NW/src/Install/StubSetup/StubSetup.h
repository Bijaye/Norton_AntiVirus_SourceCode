// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// StubSetup.h : main header file for the STUBSETUP application
//

#if !defined(AFX_STUBSETUP_H__AB3B81E5_FD93_11D3_BEEA_00A0CC272509__INCLUDED_)
#define AFX_STUBSETUP_H__AB3B81E5_FD93_11D3_BEEA_00A0CC272509__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "MSI.H"
#include <regstr.h>

#define CLIENTTYPE_KEY							_T("ClientType")
#define CLIENTTYPE_CONNECTED					0x00000001
#define CLIENTTYPE_STANDALONE					0x00000002
#define CLIENTTYPE_SERVER						0x00000004

#define MEDIA_KEY_HIVE_NT _T("Installer\\Products\\")
#define MEDIA_KEY_HIVE_9X _T("Software\\Microsoft\\Installer\\Products\\")
#define MEDIA_KEY_STRING_AFTER_PRODUCTCODE _T("\\SourceList\\Media")
#define TEMPORARY_HIVE _T("SOFTWARE\\Symantec\\MediaNavcorp")
#define PRODUCTNAME _T("Symantec AntiVirus Corporate Edition")
#define PRODUCTNAMESTRING _T("ProductName")
#define MEDIAPACKAGE _T("MediaPackage")


/////////////////////////////////////////////////////////////////////////////
// CStubSetupApp:
// See StubSetup.cpp for the implementation of this class
//

class CStubSetupApp : public CWinApp
{
public:
	CStubSetupApp();
	~CStubSetupApp();

private:
	STARTUPINFO m_SInfo;
	PROCESS_INFORMATION m_PInfo;

	BOOL m_bWinNT;
	BOOL m_bWin95; // Windows 95 specifically -- not equivalent to Win9x
	BOOL m_bWin9x;
	CString m_strWinSysDir;
	CString m_strSupportDir;
	CString m_strModuleDir;

	HINSTANCE m_hMSILib;

	INSTALLUILEVEL (WINAPI *m_MsiSetInternalUI)(INSTALLUILEVEL,HWND *);
	UINT	(WINAPI *m_MsiOpenPackage)(LPCTSTR,MSIHANDLE *);
	UINT	(WINAPI *m_MsiCloseHandle)(MSIHANDLE);
	UINT	(WINAPI *m_MsiGetProductProperty)(ULONG,LPCTSTR,LPTSTR,DWORD *);
	UINT	(WINAPI *m_MsiGetFileVersion)(LPCSTR,LPCSTR,DWORD *,LPCSTR,DWORD *);
	UINT	(WINAPI *m_MsiEnableLog)(DWORD,LPCTSTR,DWORD);
	BOOL	LaunchAppAndWait(CString& strCmdLine);
	BOOL	ResolveMSIFunctions(void);
	DWORD	GetCurrentVersion(void);
	DWORD	GetPackageVersion(void);
	void	InitMSI(void);
	UINT    GetDriveInfo(CString& VolumeName, CString& RootPathName);
	BOOL    RegDeleteValue( HKEY hMKey, LPCTSTR lpszKey );
	BOOL    RegCopyValue( HKEY hMDKey, LPCTSTR lpszSourceKey, HKEY hMSKey, LPCTSTR lpszDestinationKey );
	BOOL    GetMediaKeyFromEncryptProductCode(HKEY hMKey, CString& MediaHiveKey);
	DWORD   GetClientType(void);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStubSetupApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CStubSetupApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STUBSETUP_H__AB3B81E5_FD93_11D3_BEEA_00A0CC272509__INCLUDED_)
