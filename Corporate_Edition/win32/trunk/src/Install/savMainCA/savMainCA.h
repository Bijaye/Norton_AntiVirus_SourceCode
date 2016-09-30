// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// savMainCA.h : main header file for the SAVMAINCA DLL
//

#if !defined(AFX_SAVMAINCA_H__EF33C11B_28EA_415F_903D_B8C2BECA3776__INCLUDED_)
#define AFX_SAVMAINCA_H__EF33C11B_28EA_415F_903D_B8C2BECA3776__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#define LOG_PREFIX "SAVINST: "
// Sync changes to this constant with SAV ATOM_REBOOTREQUIRED and WPRotect32's ATOM_INSTALL_REBOOT_REQUIRED
#define ATOM_REBOOTREQUIRED "NavCorpRebootRequired"
// RTVSCan service name 
#define RTVSERVICENAME "Symantec AntiVirus"	
#define DEFWATCHSERVICENAME "DefWatch"
// Name of RTVScan service in 8.x and earlier
#define NAVAP_SERVICE_NAME      "NAVAP"
// NAVAP service name in 8.x and earlier
#define OLDRTVSERVICENAME       "Norton AntiVirus Server"
// SAVRT Driver used for service shutdown via SCM
#define SAVRT_SERVICE_NAME		"SAVRT"
#define SAVRTPEL_SERVICE_NAME	"SAVRTPEL"
#define NAVEX15_SERVICE_NAME	"NAVEX15"
#define NAVENG_SERVICE_NAME		"NAVENG"
#define MAX_LOOPS				60
#define RTVSCAN_MAX_LOOPS		MAX_LOOPS * 5		

#define VPTRAY_CLASS	"LANDeskVPTray"
#define VPC32_CLASS     "LANDeskVPC32"
#define DEFWATCH_CLASS  "DefWatchDaemonWndClass"

// From vpcommon.h
#define ERROR__NOT_FOUND               0x20000049
#define ERROR_NO_MORE                  0x20000038

// Windows Security Center configuration property names
#define PROPERTY_WSC_CONTROL			"WSCCONTROL"
#define PROPERTY_WSC_DEFSUPTODATE		"WSCAVUPTODATE"
#define PROPERTY_WSC_AVALERTCONTROL		"WSCAVALERT"

#ifdef UNICODE
#define GET_MODULE_BASE_NAME  "GetModuleBaseNameW"
#else
#define GET_MODULE_BASE_NAME  "GetModuleBaseNameA"
#endif // !UNICODE

#define REG_PRODUCTCONTROL	_T("SOFTWARE\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion\\ProductControl")

#include "resource.h"		// main symbols

int CopyDir( MSIHANDLE, CString, CString );
BOOL DirExist( LPCSTR );
BOOL DeleteDir( MSIHANDLE, TCHAR*, TCHAR* );
BOOL CALLBACK EnumChildProc( HWND , LPARAM );
BOOL CreateProcessAndWait(LPCTSTR szCmd, LPINT pnExitCode);

UINT MSILogMessage( MSIHANDLE , TCHAR [] );
UINT MSILogMessage( MSIHANDLE , CString& );
UINT StopService ( MSIHANDLE, TCHAR [], INT nLoopCount );
UINT MSIModifyApp2WFP( MSIHANDLE hInstall, bool bAddApp );

CString GetTempDir();
CString GetMSIProperty( MSIHANDLE, CString );

void __fastcall AddSlash( char* pszPath, size_t nNumBytes );
void __fastcall AddSlash( CString& strPath );

UINT _stdcall CheckUninstallPassword( MSIHANDLE );
UINT _stdcall ProcessGRC( MSIHANDLE );
UINT _stdcall SetOSInfo( MSIHANDLE );
UINT _stdcall SetLastStartTime( MSIHANDLE );
UINT _stdcall CreateGlobalAddAtom( MSIHANDLE );
UINT _stdcall StopRTVScan( MSIHANDLE );
UINT _stdcall StartVPTray( MSIHANDLE );
UINT _stdcall RemoveVirusProtect6Hive( MSIHANDLE );
UINT _stdcall DisableCancelButton( MSIHANDLE );
UINT _stdcall DeleteClientPKI ( MSIHANDLE );
UINT _stdcall MsxmlFixup ( MSIHANDLE );
UINT _stdcall MigStopRTVScan( MSIHANDLE hInstall );
UINT _stdcall MSIAddApp2WFP( MSIHANDLE hInstall );
UINT _stdcall MSIRemoveApp2WFP( MSIHANDLE hInstall );


/////////////////////////////////////////////////////////////////////////////
// CSavMainCAApp
// See savMainCA.cpp for the implementation of this class
//

class CSavMainCAApp : public CWinApp
{
public:
	CSavMainCAApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSavMainCAApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSavMainCAApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAVMAINCA_H__EF33C11B_28EA_415F_903D_B8C2BECA3776__INCLUDED_)
