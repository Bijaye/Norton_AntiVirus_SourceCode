// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: TaskView.Cpp
//  Purpose: CTaskView Implementation file
//
//	Date: 2-3-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#include "stdafx.h"
#include <new>
#include "wprotect32.h"
#include "TaskView.h"
#include "Splitter.h"
#include "MainFrm.h"
#include "WProtect32Doc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CTaskView, CView)

BEGIN_EVENTSINK_MAP(CTaskView, CView)
    //{{AFX_EVENTSINK_MAP(CTaskView)
	ON_EVENT(CTaskView, IDC_TASK_FS, 1 /* CreateGroup */, OnCreateGroup, VTS_BSTR VTS_BOOL VTS_I4)
	ON_EVENT(CTaskView, IDC_TASK_FS, 2 /* CreateTask */, OnCreateTask, VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4 VTS_I4)
	ON_EVENT(CTaskView, IDC_TASK_FS, 3 /* TaskEnding */, OnTaskEnding, VTS_NONE)
	ON_EVENT(CTaskView, IDC_TASK_FS, 4 /* TaskStarting */, OnTaskStarting, VTS_NONE)
	ON_EVENT(CTaskView, IDC_TASK_FS, 5 /* ScanStarting */, OnScanStarting, VTS_NONE)
	ON_EVENT(CTaskView, IDC_TASK_FS, 6 /* ScanEnding */, OnScanEnding, VTS_NONE)
	ON_EVENT(CTaskView, IDC_TASK_FS, 7 /* SaveScan */, OnSaveScan, VTS_BSTR VTS_BSTR VTS_BSTR)
	ON_EVENT(CTaskView, IDC_TASK_FS, 8 /* BeginRecording */, OnBeginRecording, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


//----------------------------------------------------------------
// OnCreateGroup
//----------------------------------------------------------------
void CTaskView::OnCreateGroup(LPCTSTR szGroup, BOOL bScanningTasks, long lmageID) 
{
	CString sMenuGroup, sTaskpadGroup;
	
	if (!szGroup)
		return;

	//Call the Taskpad and tell it to add the items

	if( m_ptrTaskPad )
	{
		// 8-23-00 terrym the taskpad strings and the menu strings need to be different now due to translation issues.
		// the two strings are combine into one resource.  The first part is the menu string.  The second string is the
		// taskpad string.  The strings are seperated by a token '----' 
		// this function will parse out the raw resource string into the menu and taskpad components.
		m_ptrTaskPad->ConvertRawMenuString(szGroup,sMenuGroup,sTaskpadGroup);
		if( m_ptrTaskPad->CreateGroup( sTaskpadGroup, bScanningTasks, lmageID ) )
		{
			//Create a Menu group in the MainFrame to match
			//	the TaskPad group
			CMenu *mainMenu = AfxGetMainWnd()->GetMenu();
			if( mainMenu )
			{
				int		iMenuCount = mainMenu->GetMenuItemCount();
				CMenu	menuNewGroup;

				menuNewGroup.CreateMenu();
				mainMenu->InsertMenu( iMenuCount-1, MF_BYPOSITION | MF_POPUP, (UINT)menuNewGroup.m_hMenu, sMenuGroup );
				menuNewGroup.Detach();
			}
		}

	}
}

//----------------------------------------------------------------
// OnCreateTask - Event from OCX
//----------------------------------------------------------------
void CTaskView::OnCreateTask(LPCTSTR GroupName, LPCTSTR TaskName, LPCTSTR TaskDescript, long TaskID, long lImageID ) 
{
	CString sMenuGroup, sTaskpadGroup;
	CString sMenuTask, sTaskpadTask;
	
	if (GroupName==NULL || TaskName == NULL)
		return;
	
		
	//Call the Taskpad and tell it to add the items
	if( m_ptrTaskPad )
	{
		// 8-23-00 terrym the taskpad strings and the menu strings need to be different now due to translation issues.
		// the two strings are combine into one resource.  The first part is the menu string.  The second string is the
		// taskpad string.  The strings are seperated by a token '----' 
		// this function will parse out the raw resource string into the menu and taskpad components.
		m_ptrTaskPad->ConvertRawMenuString(GroupName,sMenuGroup,sTaskpadGroup);
		m_ptrTaskPad->ConvertRawMenuString(TaskName,sMenuTask,sTaskpadTask);

		m_ptrTaskPad->CreateTask( sTaskpadGroup, sTaskpadTask, TaskDescript, TaskID, ((CWProtect32App*)AfxGetApp())->m_ocxGUID, lImageID );

		//Also add the task to the proper menu
		AddTaskToMenu( m_ptrTaskPad, sMenuGroup, (UINT)MAKEWORD( (BYTE)TaskID, 0 ), sMenuTask );
	}
}

void CTaskView::OnTaskEnding()
{
	//When the task ends, switch to the default view
	AfxGetMainWnd()->PostMessage( WM_COMMAND, MAKEWPARAM( UWM_END_TASK, 0 ), (LPARAM)NULL );
}

void CTaskView::OnTaskStarting()
{
}

void CTaskView::OnScanStarting()
{
	//Close down the interface when a scan starts
	AfxGetMainWnd()->PostMessage( WM_COMMAND, MAKEWPARAM( UWM_START_SCAN, 0 ), (LPARAM)NULL );
}

void CTaskView::OnScanEnding()
{
	//Re-enable the interface
	AfxGetMainWnd()->PostMessage( WM_COMMAND, MAKEWPARAM( UWM_END_SCAN, 0 ), (LPARAM)NULL );
}

void CTaskView::OnBeginRecording() 
{
	//Set the Document recording flag to the passed value
	CWProtect32Doc	*pDoc = (CWProtect32Doc*)GetDocument();

	pDoc->m_bRecordingStarted = TRUE;
}

void CTaskView::OnSaveScan(LPCTSTR szDescriptiveName, LPCTSTR szDescription, LPCTSTR szTaskname) 
{
	//Get the current group
	CString			strGroup,
					strDescriptiveName( szDescriptiveName );
	CWProtect32Doc	*pDoc = (CWProtect32Doc*)GetDocument();
	UINT			iID;
	GUID			guid = ((CWProtect32App*)AfxGetApp())->m_ocxGUID;

	//Determine if we are saving a Scripted task or a
	//	scheduled task
	if( pDoc->m_fRecording & RECORDING_SCRIPTED )
	{
		iID = IDS_DEFTASK_SCRIPTED;
	}
	else if( pDoc->m_fRecording & RECORDING_SCHEDULE )
	{
		iID = IDS_GROUP_SCHEDULED_TASKS;
	}
	else if( pDoc->m_fRecording & RECORDING_STARTUP )
	{
		iID = IDS_DEFTASK_STARTUP;
	}
	else
	{
		CString		strGroup,
					strID,
					strTaskname,
					strDescription;
		GUID		guid;

		//Get the task that is highlighted
		m_ptrTaskPad->GetSelTask( strTaskname, strGroup );
		
		//First, get the custom task info
		m_ptrTaskPad->GetCustomTaskInfo( strGroup,  strTaskname, strDescription, strID, guid, TRUE );
		strTaskname = szDescriptiveName;
		strDescription = szDescription;
		m_ptrTaskPad->SaveCustomTaskInfo( strGroup,  strTaskname, strDescription, strID, guid );
		m_ptrTaskPad->ModifyCurrentTask( strTaskname, szDescription/*, FALSE */);

		return;
	}

	strGroup.LoadString( iID );

	//Reset the Recording flag
	pDoc->CancelRecord();

	//And save out the custom task
	m_ptrTaskPad->SaveCustomTaskInfo( strGroup,  strDescriptiveName, szDescription, szTaskname, guid );

	//And finaly, add it to the task pad
	m_ptrTaskPad->CreateTask( strGroup, strDescriptiveName, szDescription, ID_TASK_CUSTOM, guid );
  
	//Update the Tree view
	m_ptrTaskPad->Invalidate();
	m_ptrTaskPad->UpdateWindow();

}

//----------------------------------------------------------------
// Constructor
//----------------------------------------------------------------
CTaskView::CTaskView()
{
	m_ptrTaskOCX = NULL;
	m_ptrTaskPad = NULL;
	m_bShowedPleaseWait = FALSE;
	m_bAlreadySetFocus = FALSE;
}

//----------------------------------------------------------------
// Destructor
//----------------------------------------------------------------
CTaskView::~CTaskView()
{
}


BEGIN_MESSAGE_MAP(CTaskView, CView)
	//{{AFX_MSG_MAP(CTaskView)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//----------------------------------------------------------------
// OnDraw
//----------------------------------------------------------------
void CTaskView::OnDraw(CDC* pDC)
{
	
	CDocument	*pDoc = GetDocument();
	CBrush		brushBackground;
	RECT		rect;
	CString		strLoading;
	CFont		pFont;
	int			x, y;
	TEXTMETRIC	tm;

	GetClientRect( &rect );
	
	strLoading.LoadString( IDS_LOADING_TASK );

	//Calculate the center
	pDC->GetTextMetrics( &tm );
	x = ( rect.right - ( strLoading.GetLength() * tm.tmAveCharWidth ) ) / 2;
	y = rect.bottom / 2;

	//Paint the background grey
	brushBackground.CreateSolidBrush( (COLORREF)GetSysColor( COLOR_BTNFACE ) );
	pDC->FillRect( &rect, &brushBackground );

	pDC->SetBkColor( (COLORREF)GetSysColor( COLOR_BTNFACE ) );
	
	//Draw out some text informing the user that
	// we are loading the requested task
	//I only want to perform this once, so I will
	//	set a flag
	if( !m_bShowedPleaseWait )
	{
		pDC->TextOut( x, y, strLoading );
	}

	brushBackground.DeleteObject();

	m_bShowedPleaseWait = TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CTaskView diagnostics

#ifdef _DEBUG
void CTaskView::AssertValid() const
{
	CView::AssertValid();
}

void CTaskView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

//----------------------------------------------------------------
// OnInitialUpdate
//----------------------------------------------------------------
void CTaskView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();

//	GetParentFrame()->RecalcLayout();
//	ResizeParentToFit();

	m_ptrTaskOCX->ShowWindow( SW_SHOWNORMAL );

}

//----------------------------------------------------------------
// OnSize
//----------------------------------------------------------------
void CTaskView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

//	ResizeParentToFit();

	if( m_ptrTaskOCX )
	{
		if( IsWindow( m_ptrTaskOCX->m_hWnd ) )
		{
			TRACE("CTaskView::OnSize\n" );
			m_ptrTaskOCX->SetWindowPos( &wndTop, 0, 0, cx, cy, SWP_SHOWWINDOW );
		}
	}
}

//----------------------------------------------------------------
// OnCreate
//----------------------------------------------------------------
BOOL CTaskView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	BOOL bReturn = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

	//Create the Task OCX
	//Adjust the sizer for the PropertySheet
	try
	{
		m_ptrTaskOCX = new CLDVPTaskFS;
	}
	catch(std::bad_alloc &)
    {
        m_ptrTaskOCX = NULL;
    }
	m_ptrTaskOCX->SetCLSID( ((CWProtect32App*)AfxGetApp())->m_ocxGUID );

	if ( !m_ptrTaskOCX->Create( NULL, _T( "" ), WS_CHILD | WS_VISIBLE, rect, this,
			IDC_TASK_FS ) )
	{
		TRACE("\nFailed to create Tasks OCX" );
		return FALSE;
	}

//	m_ptrTaskOCX->SetHelpFilename( "vpc32.hlp" );

	return bReturn;
}

//----------------------------------------------------------------
// OnDestroy
//----------------------------------------------------------------
void CTaskView::OnDestroy() 
{
	//Cancel any recording
//	((CWProtect32Doc*)GetDocument())->m_fRecording = 0;

	//Make sure we don't have any Tasks running
	m_ptrTaskOCX->EndTask();

	m_ptrTaskOCX->DestroyWindow();
	delete m_ptrTaskOCX;

	m_ptrTaskOCX = NULL;

	CView::OnDestroy();
}

//----------------------------------------------------------------
// Initialize
//----------------------------------------------------------------
void CTaskView::Initialize()
{
	//Initialize the Task OCX
	m_ptrTaskOCX->Initialize();
}

void CTaskView::DelTask()
{
	m_ptrTaskOCX->DeleteTask();
}

//----------------------------------------------------------------
// SelectTask
//----------------------------------------------------------------
void CTaskView::SelectTask( DWORD dwID, CString strCategory )
{
	//Tell the TaskPad to select the task with
	//	the passed ID
	if( m_ptrTaskPad )
	{
		m_ptrTaskPad->SelectTask( dwID, strCategory );
	}
}

//----------------------------------------------------------------
// SelectTask
//----------------------------------------------------------------
void CTaskView::SelectTask( WORD wID, WORD wGroupID )
{
	CTaskGroup	*pGroup = NULL;
	CString		strGroupText;

	//I need to figure out which 
	if( !(wGroupID & TASK_GROUP_CONFIG_SNAP_IN ) )
	{
		//Get the group string from the group ID
		m_ptrTaskPad->m_GroupIds.Lookup( wGroupID, (CObject*&)pGroup);

		//Strip off the & from the group name
		strGroupText = pGroup->m_text;
		RemoveChars( strGroupText, _T('&') );

		//Tell the TaskPad to select the task with
		//	the passed ID
		if( m_ptrTaskPad )
		{
			m_ptrTaskPad->SelectTask( wID, strGroupText );
		}
	}
	else
	{
		if( m_ptrTaskPad )
		{
			m_ptrTaskPad->SelectConfigurationSnapInTask( wID, wGroupID & ~TASK_GROUP_CONFIG_SNAP_IN );
		}
	}

}

//----------------------------------------------------------------
// StartTask
//----------------------------------------------------------------
void CTaskView::StartTask( DWORD dwID, CString strTaskName, const CString &strTaskDescriptiveName, const CString &strDescription, BOOL bDoubleClick /* = FALSE */ )
{
	BOOL		bRecording = ((CWProtect32Doc*)GetDocument())->m_fRecording != 0 ;

	m_ptrTaskOCX->SetTaskDescriptiveName( strTaskDescriptiveName );
	m_ptrTaskOCX->SetTaskDescription( strDescription ); 
	
	//If the taskname is empty, then I need to generate a unique value for it
	if( strTaskName.IsEmpty() )
		strTaskName = GenerateUniqueKey();

	//Now, tell the OCX the name of the task
	m_ptrTaskOCX->SetTaskName( strTaskName );
	
	//Check the Document and see if we are performing a 
	// Create Scheduled scan. If so, set the property in the
	//	OCX so it knows how to act.
	m_ptrTaskOCX->SetRecordingSchedule( ((CWProtect32Doc*)GetDocument())->m_fRecording & RECORDING_SCHEDULE );
	//The OCX also needs to know if we are recording anything,
	//	so it can change the text on the button from "Scan" to "Save"
	m_ptrTaskOCX->SetRecording( bRecording );

	//Send a size message before I start the task
	CRect rect;
	GetClientRect( &rect );

	m_ptrTaskOCX->StartTask( dwID );

	m_ptrTaskOCX->SendMessage( WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM( rect.right, rect.bottom ) );

	if( bDoubleClick )
	{
		//This tells the OCX to skip the first page
		//	and start the task immediatly, if it can.
		// If bSkipFirstpage is false, then this does
		//	nothing.
		m_ptrTaskOCX->SetSkipFirst( TRUE );
/*
		//If the first page could NOT be skipped, 
		//	then the property would be reset to FALSE,
		//	so I re-check it here
		if( m_ptrTaskOCX->GetSkipFirst() )
		{
			return;
		}
*/	}

}

//---------------------------------------------------------------
// GenerateUniqueKey
//---------------------------------------------------------------
CString CTaskView::GenerateUniqueKey()
{
	UUID		myID;
	CString		strTaskname;
	BYTE		*szTaskname;

	//First, generate a unique name	
	UuidCreate( &myID );
	UuidToString( &myID, &szTaskname );
	strTaskname = szTaskname;
	RpcStringFree( &szTaskname );

	//Return the Taskname
	return strTaskname;

}

//----------------------------------------------------------------
// EndTask
//----------------------------------------------------------------
void CTaskView::EndTask()
{
	m_ptrTaskOCX->EndTask();
}

//----------------------------------------------------------------
// OnNotifyEndTask
//----------------------------------------------------------------
void CTaskView::OnNotifyEndTask()
{
	AfxGetMainWnd()->PostMessage( WM_COMMAND, MAKEWPARAM( UWM_END_TASK, 0 ), (LPARAM)NULL  );	
}

BOOL CTaskView::PreTranslateMessage(MSG* pMsg) 
{
	//I need to do this to get the Del key in edit fields
	//	in the Tasks. If I don't do this, the Del key never
	//	gets to my edit fields.
	if( pMsg->message == WM_KEYDOWN		&&
		pMsg->wParam == VK_DELETE		&&
		IsDialogMessage( pMsg )			)
	{
		return TRUE;
	}
	else
	{
		return CView::PreTranslateMessage(pMsg);
	}
}


void CTaskView::OnSetFocus(CWnd* pOldWnd) 
{
	/*----------------------------------------------*/
	//	KLUDGE ALERT:
	//	O.K., here's the deal. When the OCX is activated,
	//		it doesn't set focus to the first control
	//		in the dialog. Not a huge problem, but then
	//		none of the keystrokes get to the window either,
	//		since the modeless dialog on the OCX doesn't
	//		have input focus. So, to solve this I intercept 
	//		the Focus message to the view that houses the
	//		OCX ( this one ) and forward it to the OCX.
	//		Inside the oCX, I then pass it along to the
	//		proper window, and eventually the control.
	//	However, if I do it more than ONE time ( the first]
	//		time the OCX is activated ), then I have to 
	//		double-click in the OCX window on any buttons
	//		in order to activate them. It also causes some
	//		timing problems, which in turn IPF the app.
	//		So, I also check to make sure I only do this once
	//		each time the OCX is created and this view gets
	//		focus. Whew.
	/*----------------------------------------------*/
	
	if( ::IsWindow( m_ptrTaskOCX->m_hWnd ) && !m_bAlreadySetFocus )
	{
		m_bAlreadySetFocus = TRUE;
		//If I have a previous window, send it along
		if( pOldWnd )
		{
			::PostMessage( m_ptrTaskOCX->m_hWnd, WM_SETFOCUS, (WPARAM)pOldWnd->m_hWnd, 0L );
		}
		//Otherwise, send NULL
		else
		{
			::PostMessage( m_ptrTaskOCX->m_hWnd, WM_SETFOCUS, 0, 0L );
		}
	}
	else
	{
		CView::OnSetFocus(pOldWnd);
	}
}

void CTaskView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	//EA 03/09/2000
	//added code to fix STS 330341,STS 330720 when the user clicks on the rt pane, or goes to hlp
	//the focus goes to the temp frame window so then if the user click on tab there was no key board access
	//to the task view pane controls put code to detect tab and if tab key pressed then
	//setting focus to the ocx's window
	if(nChar == 9)
	{
		if( ::IsWindow( m_ptrTaskOCX->m_hWnd )  )
		{
			::PostMessage( m_ptrTaskOCX->m_hWnd, WM_SETFOCUS, 0, 0L );
		}
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
	//EA 03/09/2000
	
}

