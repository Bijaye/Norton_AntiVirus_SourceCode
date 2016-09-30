// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: WProtect32.h
//  Purpose: CWProtect32 Implementation file
//
//	Date: 2-3-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "TDKObj.h"
#include "ComCache.h"
#include "vpcommon.h"
#include "Taskpadext.h"
#include "MscFuncs.h"

#define TASK_NAME_LIMIT			20
#define UWM_END_TASK			5000
#define UWM_START_SCAN			5021
#define UWM_END_SCAN			5022
#define UWM_STOP_CURRENT_SCAN	5023
#define UWM_DELETESELTASK		5024

//Bit flags for the recording flag
#define RECORDING_SCRIPTED	0x0001
#define RECORDING_SCHEDULE  0x0010
#define RECORDING_STARTUP	0x0100

//#include "ScanObject.h"

//Define the default "un-languagized" values for these help file names
#define HELPFILE		"core"
#define ID_ABOUT_DLG	6000

DWORD GetClientType();
UINT StoreSnapInCommand( DWORD dwID );
void InterpretSnapInCommand( WPARAM &wParam );

//----------------------------------------------------------------
//
// CWProtect32 class
//
//----------------------------------------------------------------
class CWProtect32App : public CWinApp
{
private:
	HANDLE			m_hMutex;
	HINSTANCE		m_hInstance;
	CCOMCache		m_comCache;
	IVirusProtect	*m_pVirusProtect;

	void RegisterOCX();

public:
	CWProtect32App();

	CString			m_strRunningTaskTitle;
	CLSID			m_ocxGUID;
	CSubTask		*m_ptrTask;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWProtect32App)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual void WinHelpInternal(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CWProtect32App)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

