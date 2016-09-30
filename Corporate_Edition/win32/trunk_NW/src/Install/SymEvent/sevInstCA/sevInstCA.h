// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// sevInstCA.h : main header file for the SEVINSTCA DLL
//

#if !defined(AFX_SEVINSTCA_H__1F7FF97E_52CC_4CFC_AD9B_38473CB52AA2__INCLUDED_)
#define AFX_SEVINSTCA_H__1F7FF97E_52CC_4CFC_AD9B_38473CB52AA2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#pragma warning( disable : 4786 )

#include "resource.h"		// main symbols
#include "msi.h"
#include "msiquery.h"
#pragma warning( push, 3 )
#include <string>
#include <list>
#pragma warning( pop )
using namespace std;
typedef list<string> StringList;

#define NUM_FIELDS		4
#define LOG_PREFIX		"SEVINST: "
#define SEVINSTMUTEX	_T("SYMEVENTMIGRATION")
#define GSEVINSTMUTEX	_T("Global\\SYMEVENTMIGRATION")
#define SEVINSTPROP		_T("SEVINSTCADATA")
#define SEVINSTALLER	_T("sevinst.exe")
#define SEVINSTALLER64  _T("sevinst64.exe")
#define SEVINSTALLERAMD64 _T("sevinst64x86.exe")
#define SAVTEMPID 		_T("SAVTEMPID")

// Windows function to determine if we are in an AMD64 processor.
// Not present in all versions of Kernel32.
typedef void (WINAPI *pfGetNativeSystemInfo)( LPSYSTEM_INFO lpSystemInfo );

UINT MSILogMessage( MSIHANDLE , CString );
CString getCustomActionData( MSIHANDLE  );
// Breaks inputString into an array of words delimited by separator
HRESULT Breakapart( string inputString, StringList* words, char separator = ';' );
//Sets pszPath to the ProgramFiles directory value from the registry, setting the default if needed
//Returns TRUE on success, else FALSE
BOOL GetProgramFilesDir( LPTSTR pszPath, UINT cbSize, bool isWin64 );
//Sets pszPath to the CommonFiles directory value from the registry, setting the default if needed
//Returns TRUE on success, else FALSE
BOOL GetCommonFilesDir( LPTSTR pszPath, UINT cbSize, bool isWin64 );
//Sets pszPath to the Symantec Common directory (Common Files\Symantec Shared)
//Returns TRUE on success, else FALSE
BOOL GetSymantecSharedDir( LPTSTR pszPath, UINT cbSize, bool isWin64 );

int parseCustomActionData( CString packedString, CStringArray* dataFields, MSIHANDLE hInstall = NULL );
BOOL launchSevInst( CString, CString, CString, MSIHANDLE hInstall, bool isWin64 );
BOOL mutexActive();

BOOL IsAMD64( MSIHANDLE );

BOOL ValidateProductSuite (LPSTR );
BOOL IsTerminalServicesEnabled();

extern "C" __declspec(dllexport) UINT __stdcall BuildSymEventCAData( MSIHANDLE );
extern "C" __declspec(dllexport) UINT __stdcall BuildRemoveCAData( MSIHANDLE );
extern "C" __declspec(dllexport) UINT __stdcall RegisterWithSymEvent( MSIHANDLE );
extern "C" __declspec(dllexport) UINT __stdcall UnregisterFromSymEvent( MSIHANDLE );

/////////////////////////////////////////////////////////////////////////////
// CSevInstCAApp
// See sevInstCA.cpp for the implementation of this class
//

class CSevInstCAApp : public CWinApp
{
public:
	CSevInstCAApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSevInstCAApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSevInstCAApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEVINSTCA_H__1F7FF97E_52CC_4CFC_AD9B_38473CB52AA2__INCLUDED_)
