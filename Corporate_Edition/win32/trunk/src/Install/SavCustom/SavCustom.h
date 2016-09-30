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

//Literal strings DO NOT localize
#define NUM_TEMP_FOLDER_FILES 3
static char * pszDestFiles[NUM_TEMP_FOLDER_FILES] =
				{"msvcp71.dll",
				"msvcr71.dll",
				// "cba.dll",
				// "msgsys.dll",
				// "nts.dll",
				// "pds.dll",
				// "Transman.dll",
				// "ScsComms.dll",
				"vdefhub.zip"};

void SetMSIEditControlText( LPCTSTR, DWORD );
CString GetMSIProperty( MSIHANDLE hInstall, CString strProperty );

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
