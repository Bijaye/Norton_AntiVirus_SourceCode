// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// SavCustom.h : main header file for the NAVCUST2 DLL
//

#if !defined(AFX_SAVCUSTOM_H__9D22C95D_E149_11D2_932C_00C04F91B157__INCLUDED_)
#define AFX_SAVCUSTOM_H__9D22C95D_E149_11D2_932C_00C04F91B157__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

// Local defines
#define	DllExport		__declspec ( dllexport )
#define DllImport		__declspec ( dllimport )

#define	MAX_PATHS		1024

// Local defines
//
#define SILENT_VERBOSE_START	2000
#define SILENT_VERBOSE_REBOOT	2001
#define SILENT_VERBOSE_END		2002

// AMS Install Flags
#define AMS_FLAGS_SERVER		0x0001
#define AMS_FLAGS_CONSOLE		0x0002
#define AMS_FLAGS_CLIENT		0x0004
#define AMS_FLAGS_CLIENTLITE	0x0008
#define AMS_PRODUCT_NAME		"LDVP"
#define NAVCORP_ROOT_KEY		"Software\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion"
#define	OLD_AMS_ROOT_KEY		"Software\\Symantec\\Symantec AntiVirus Corporate Edition"
#define AMSINSTALLED_REGVALUE	"AMSClientInstalled"
#define AMSINSTALLPATH_REGVALUE	"AMSInstallPath"
#define MAX_STR_LEN				512

//Literal strings DO NOT localize
const char INSTALLPATH[]="Software\\Symantec\\InstalledApps";	//Symantec Antivirus Coporate Edition";
const char	BACKSLASH[]		="\\";
#define NUM_TEMP_FOLDER_FILES 9
static char * pszDestFiles[NUM_TEMP_FOLDER_FILES] =
				{"msvcp71.dll",
				"msvcr71.dll",
				"cba.dll",
				"msgsys.dll",
				"nts.dll",
				"pds.dll",
				"Transman.dll",
				"ScsComms.dll",
				"vdefhub.zip"};

LONG __stdcall DoServerListDlg( HWND hWndSetup, TCHAR* lpszSelectedServer );
DWORD __stdcall InitDomains ();

BOOL __stdcall IsServerPrimary( LPSTR );
BOOL __stdcall ServerHaveParent( LPSTR );
BOOL __stdcall TMIsServerSecure( LPSTR );
BOOL __stdcall ServerExistsinAddressCache( LPSTR );
LONG __stdcall TMFindComputer( TCHAR* , bool );
BOOL __stdcall TMInit();
BOOL __stdcall TMParentConnect( LPTSTR );
BOOL __stdcall TMVerifyDomain(LPSTR szInput, LPSTR szOutput);
DWORD __stdcall TMFindAllComputers();
DWORD __stdcall TMStopTransman ();
DWORD __stdcall TMFreeTransman ();
void __stdcall TMStartDomains();
void __stdcall TMCleanupDomains();
void SetMSIEditControlText( LPCTSTR, DWORD );

/////////////////////////////////////////////////////////////////////////////
// CSavCustomApp
// See SavCustom.cpp for the implementation of this class
//

class CSavCustomApp : public CWinApp
{
public:
	CSavCustomApp();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSavCustomApp)
	public:
		virtual BOOL InitInstance();
		virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSavCustomApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAVCUSTOM_H__9D22C95D_E149_11D2_932C_00C04F91B157__INCLUDED_)
