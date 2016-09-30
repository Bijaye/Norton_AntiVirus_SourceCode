// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: LDVPTaskFS.h
//  Purpose: LDVPTaskFS Definition file
//
//	Date: 1-20-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#if !defined( LDVPTASKFS_INCLUDED )
#define LDVPTASKFS_INCLUDED

#include "resource.h"       // main symbols
#include "LDVPTaskFSCtl.h"
#include "ClientReg.h"
#include "MscFuncs.h"

//Define the Task ID's
#define TASK_INVALID				0
#define TASK_ID_SCAN_FLOPPY			1
#define TASK_ID_SCAN_SELECTED		3
#define TASK_ID_SCAN_PCHEALTH		10
#define TASK_ID_CONFIG_EXCLUSIONS	13
#define TASK_ID_SCAN_RECORD			900
#define TASK_ID_SCAN_CUSTOM			999
#define TASK_ID_QUICK_SCAN			4
#define TASK_ID_FULL_SCAN			5

#define UWM_SCAN_STARTING		5021
#define UWM_SCAN_ENDING			5022
#define UWM_DESTROYSCAN			5024

//Define the message I will expect when the user asks the
//	results dialog from CliScan to close.
#define UWM_CLOSING				(WM_USER + 203)
#define UWM_ADDCHILD			(WM_USER + 204)

//Global method for determining if we are installed on a CLient Lite machine
DWORD GetClientType();

//----------------------------------------------------------------
//
// CLDVPTaskApp class
//
//----------------------------------------------------------------
class CLDVPTaskFSApp : public COleControlModule
{
private:
	HINSTANCE	m_hInstance;
	CString		m_strAppname;
	CString		m_strHelpFileName;

public:
	CLDVPTaskFSCtrl *m_ptrCtl;
	BOOL InitInstance();
	int ExitInstance();
	void WinHelpInternal(DWORD dwData, UINT nCmd);//WinHelp override to support HTMLHelp for SAV
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

/*------------------------------------------------------------------
*
* $History: $
*
*
  ------------------------------------------------------------------*/
#endif
