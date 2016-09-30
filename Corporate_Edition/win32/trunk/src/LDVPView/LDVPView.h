// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: LDVPView.h
//  Purpose: LDVPView Definition file
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

#include "resource.h"       // main symbols
#include "LDVPViewCtl.h"
#include "vpcommon.h"
#include "ClientReg.h"

//Define the Task ID's
#define TASK_INVALID				0

//ID sent in by the owner of the OCX requesting the task to start/stop
#define TASK_ID_RTS				1
#define TASK_ID_SCHEDULE		2
#define TASK_ID_VIRUS_HISTORY	3
#define TASK_ID_EVENT_LOG		4
#define TASK_ID_VIRUS_BIN		5
#define TASK_ID_SCAN_HISTORY	6
#define TASK_ID_VIEW_BACKUP     7
#define TASK_ID_VIEW_REPAIR     8
#define TASK_ID_LICENSING       9
#define TASK_ID_TAMPERBEHAVIOR_HISTORY	10

//Mask for adding pages to the Modal property sheet
#define PAGE_TASK_RTS			2
#define PAGE_TASK_SCHEDULE		4
#define PAGE_TASK_VIRUS_HISTORY	8
#define PAGE_TASK_EVENT_LOG		16
#define PAGE_TASK_VIRUS_BIN		32

#define UWM_SCAN_STARTING		5021
#define UWM_SCAN_ENDING			5022
#define UWM_END_SCAN			5023
#define UWM_DESTROYSCAN			5024
#define UWM_READ_LOGS			5025
#define UWM_SCANDELIVER			5026


#define SCAN_DLGS_DYNAMIC
#include "ScnDlgex.h"
#include "MscFuncs.h"

extern tAddLogLineToResultsView AddLogLineToResultsView;

//Prototypes for callback methods
extern "C"
{
DWORD GetFileStateForView(void *Context,LPARAM Data,char *LogLine,DWORD *State, DWORD *ViewType);
DWORD TakeAction(void *Context,LPARAM Data,char *LogLine,DWORD Action, DWORD dwState, bool bSvcStopOrProcTerminate);
#ifdef _USE_CCW
DWORD GetCopyOfFile(void *Context,LPARAM Data,char *LogLine,wchar_t *DestFile,DWORD State);
#else	//_USE_CCW
DWORD GetCopyOfFile(void *Context,LPARAM Data,char *LogLine,char *DestFile,DWORD State);
#endif	//_USE_CCW
DWORD ViewClosed(void *Context, BOOL bNotUsed);
DWORD GetVBinData(void *Context,LPARAM Data,char *LogLine,void *pvbindata);
}

UINT ReadLogFiles( CResultsView *pResultsViewDlg );

//----------------------------------------------------------------
//
// CLDVPViewApp class
//
//----------------------------------------------------------------
class CLDVPViewApp : public COleControlModule
{
private:
	HINSTANCE	m_hInstance;
	CString		m_strAppname;
	CString		m_strHelpFileName;

public:
	CLDVPViewCtrl *m_ptrCtl;
	BOOL InitInstance();
	int ExitInstance();
	void WinHelpInternal(DWORD dwData, UINT nCmd);//WinHelpInternal override to redirect help calls to HTMLHelp for SAV
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
