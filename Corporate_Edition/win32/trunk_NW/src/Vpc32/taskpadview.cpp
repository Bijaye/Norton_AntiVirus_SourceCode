// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: TaskpadView.Cpp
//  Purpose: CTaskpadView Implementation file
//
//	Date: 2-3-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#include "stdafx.h"
#include "WProtect32.h"
#include "MainFrm.h"
#include "WProtect32Doc.h"
#include "TaskpadView.h"
#include "ClientReg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// Definitions
//
#define IDC_TASKPAD				1000

IMPLEMENT_DYNCREATE(CTaskpadView, CView)

BEGIN_MESSAGE_MAP(CTaskpadView, CView)
	//{{AFX_MSG_MAP(CTaskpadView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_CUT, OnCut)
	ON_UPDATE_COMMAND_UI(ID_CUT, OnUpdateCut)
	ON_COMMAND(ID_PASTE, OnPaste)
	ON_UPDATE_COMMAND_UI(ID_PASTE, OnUpdatePaste)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//----------------------------------------------------------------
// Constructor
//----------------------------------------------------------------
CTaskpadView::CTaskpadView()
{
	m_ptrCut = NULL;
}

//----------------------------------------------------------------
// Destructor
//----------------------------------------------------------------
CTaskpadView::~CTaskpadView()
{
	//If we are exiting with an item in the Cut buffer,
	//	remove it.
	if( m_ptrCut )
	{
		DeleteCut();
	}

}

//----------------------------------------------------------------
// PreCreateWindow
//----------------------------------------------------------------
BOOL CTaskpadView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

//----------------------------------------------------------------
// OnDraw
//----------------------------------------------------------------
void CTaskpadView::OnDraw(CDC* pDC)
{
}

/////////////////////////////////////////////////////////////////////////////
// CTaskpadView diagnostics

#ifdef _DEBUG
void CTaskpadView::AssertValid() const
{
	CView::AssertValid();
}

void CTaskpadView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CWProtect32Doc* CTaskpadView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWProtect32Doc)));
	return (CWProtect32Doc*)m_pDocument;
}
#endif //_DEBUG

//----------------------------------------------------------------
// OnCreate
//----------------------------------------------------------------
int CTaskpadView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	
	DWORD dwStyle = GetClassLong( m_hWnd, GCL_STYLE );
	::SetClassLong( m_hWnd, GCL_STYLE,
					dwStyle & ~(CS_VREDRAW | CS_HREDRAW ) );
	BOOL rc = -1L;
	CRect rect;
	
	rect.SetRect( 3, 3, 4, 4 );
	if ( !m_taskpad.Create( WS_VISIBLE | WS_CHILD 
			| TVS_HASBUTTONS | TVS_HASLINES | TVS_SHOWSELALWAYS
			| TVS_LINESATROOT,
			rect, this, IDC_TASKPAD ) )
	{
		AfxMessageBox( IDS_ERR_CREATE_TASKPAD );
		goto cleanup;
	}

	rc = 0;

cleanup:
	return rc;
}

//----------------------------------------------------------------
// OnDestroy
//----------------------------------------------------------------
void CTaskpadView::OnDestroy() 
{
	if ( IsWindow( m_taskpad.m_hWnd ) )
		m_taskpad.DestroyWindow();

	CView::OnDestroy();
}

//----------------------------------------------------------------
// OnSize
//----------------------------------------------------------------
void CTaskpadView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	m_taskpad.SetWindowPos( &wndTop, 0, 0, cx, cy, SWP_NOZORDER );
	m_taskpad.SizeTaskWindow();


	//This is a soecial case - I need to inform the other
	//	pane that I have resized so it can make sure it
	//	it no smaller than it's smallest size.
	if( CWnd *pWnd = AfxGetMainWnd() )
		pWnd->SendMessage( UWM_TASKPAD_RESIZED, 0, 0L );
}

//----------------------------------------------------------------
// OnEraseBkgnd
//----------------------------------------------------------------
BOOL CTaskpadView::OnEraseBkgnd(CDC* pDC) 
{
	CRect rc;
	CBrush brGray( (COLORREF)GetSysColor( COLOR_BTNFACE ) );

	GetClientRect( &rc );
	pDC->FillRect( rc, &brGray );

	return TRUE;
}

//----------------------------------------------------------------
// OnIntitialUpdate
//----------------------------------------------------------------
void CTaskpadView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();

	m_taskpad.InitGroups();
	
	//In order to get the tasks out of the OCX, I need to create the
	//	view and call Initialize.
	//I can then unload the OCX until I need it ( when the View is 
	//	created ).
	m_taskpad.GetExternalTasks();	

	//Add the default tasks
	m_taskpad.AddDefaultTasks();

	//And expand the scanning tasks
	m_taskpad.ExpandScanningTasks();

	m_taskpad.VerifyGroups();

	AfxGetMainWnd()->SendMessage( UWM_POST_INITIALIZE, 0, 0 );
}

//----------------------------------------------------------------
// CanDeleteSelTask
//----------------------------------------------------------------
BOOL CTaskpadView::CanDeleteSelTask()
{
	BOOL		bReturn = FALSE;
	TV_ITEM		tvItem;
	HTREEITEM	item;

	memset( &tvItem, '\0', sizeof( TV_ITEM ) );

	//Determine whether a task or group is selected
	if( NULL == (tvItem.hItem = m_taskpad.GetSelectedItem() ) )
	{
		return FALSE;
	}

	tvItem.mask = TVIF_PARAM;
	m_taskpad.GetItem( &tvItem ); 
	
	ptrTask = (CSubTask*)tvItem.lParam;

	//First, figure out if it is a Subtask or not
	//If it isn't a subtask, then we will return FALSE
	if( ptrTask->IsKindOf( RUNTIME_CLASS( CSubTask ) ) )
	{
		item = tvItem.hItem;

		if( ptrTask->m_dwTaskType != TASK_EXTERNAL )
			return FALSE;

		//Now find out if we can delete it
		//We can only delete it if the group is a drag-drop group
		memset( &tvItem, '\0', sizeof( TV_ITEM ) );

		tvItem.hItem = m_taskpad.GetParentItem( item );
		m_taskpad.GetItem( &tvItem );
		
		ptrGroup = (CTaskGroup*)tvItem.lParam;

		if( ptrGroup->CanDragDropChildren() )
		{
			bReturn = TRUE;
		}

	}

	return bReturn;
}


//----------------------------------------------------------------
// DeleteSelTask
//----------------------------------------------------------------
void CTaskpadView::DeleteSelTask()
{
	
	CString		strDescription,
				strTaskname,
				strFilePath;
	DWORD		dwSize = MAX_PATH;
//	CLDVPTaskFS	*ptrTaskOCX;
	RECT		rect;

	memset( &rect, 0, sizeof( RECT ));

	//If we can't delete the task, return
	//If we can, then the pointers have been filled in,
	//	so I can use them in the next call
	if( !CanDeleteSelTask() )
	{
		return;
	}

	//Make sure the user really wants to delete the task
	CString strMessage;

	AfxFormatString1( strMessage, IDS_DELETE_TASK, ptrTask->m_text );

	if( IDYES == AfxMessageBox( strMessage, MB_YESNO ) )
	{
		//Otherwise, get the info out of the registry
		//This serves two purposes
		//	1. it will remove the entry from the registry
		//	2. it will get the filename so I can delete it
		m_taskpad.GetCustomTaskInfo( ptrGroup->m_text, ptrTask->m_text, strDescription, strTaskname, ptrTask->m_guid, TRUE );

		//Remove the item from the OCX's persistent storage
		AfxGetMainWnd()->SendMessage( WM_COMMAND, MAKEWPARAM( UWM_DELETESELTASK, 0), 0L );

		//Finally, remove the item from the tree
		m_taskpad.DeleteItem( m_taskpad.GetSelectedItem() );

		//Free the memory
		delete ptrTask;
	}
}

void CTaskpadView::Cut()
{
	CString		strDescription,
				strFilePath;

	//Take the current selection
	//	and remove it from the Task pad.
	if( CanCut() )
	{

		//If there is already an item in the Cut buffer,
		//	remove it
		if( m_ptrCut )
		{
			DeleteCut();
		}
		
		m_taskpad.GetCustomTaskInfo( ptrGroup->m_text, ptrTask->m_text, strDescription, m_strCutTaskname, ptrTask->m_guid, TRUE );

		//Finally, remove the item from the tree
		m_taskpad.DeleteItem( m_taskpad.GetSelectedItem() );

		m_ptrCut = ptrTask;
	}
}

BOOL CTaskpadView::CanPasteCurGroup()
{
	TV_ITEM		tvItem;
	HTREEITEM	item;
	
	//If there is a cut item,
	//	add it to the Task pad
	if( m_ptrCut )
	{
		//Find out where we are
		memset( &tvItem, '\0', sizeof( TV_ITEM ) );

		//Determine whether a task or group is selected
		if( NULL == (tvItem.hItem = m_taskpad.GetSelectedItem() ) )
		{
			return FALSE;
		}

		tvItem.mask = TVIF_PARAM;
		m_taskpad.GetItem( &tvItem ); 
		
		ptrTask = (CSubTask*)tvItem.lParam;

		//First, figure out if it is a Subtask or not
		if( ptrTask->IsKindOf( RUNTIME_CLASS( CSubTask ) ) )
		{
			item = tvItem.hItem;
			//I am only interested in the Parent
			memset( &tvItem, '\0', sizeof( TV_ITEM ) );

			tvItem.hItem = m_taskpad.GetParentItem( item );
			m_taskpad.GetItem( &tvItem );
			
			ptrGroup = (CTaskGroup*)tvItem.lParam;

		}
		else
		{
			ptrGroup = (CTaskGroup*)tvItem.lParam;
		}

		//By now I should have a valid group pointer,
		//	Check it to see if I can paste here
		if( !ptrGroup->CanDragDropChildren() )
		{
			return FALSE;
		}

	}
	else
	{
		return FALSE;
	}

	return TRUE;
}


void CTaskpadView::Paste()
{
	CString strScheduledGroup;

	//Make sure we can paste into the 
	//	current group.
	if( CanPasteCurGroup() )
	{
		strScheduledGroup.LoadString( IDS_GROUP_SCHEDULED_TASKS );

		/*---------------------------------------------*/
		//	My pointer is filled in by the call 
		//		to CanPasteCurGroup, so all
		//		I need to do is add
		//		this item to the task pad.
		/*---------------------------------------------*/

		//If the item is dropped on a Scheduled Scan group,
		// call the Modal dialog box
		if( strScheduledGroup == ptrGroup->m_text )
		{
			if( !m_taskpad.GetScheduleInfo( ptrTask->m_text, m_ptrCut->m_description, m_strCutTaskname, m_ptrCut->m_guid ) )
			{
				//If they canceled, end this operation
				return;
			}
		}
		else
		{
			m_taskpad.RemoveScheduleInfo( m_strCutTaskname );
		}

		//update the registry & CSubTask with the proper values
		// for the new group
		m_taskpad.SaveCustomTaskInfo( ptrGroup->m_text, m_ptrCut->m_text, m_ptrCut->m_description, m_strCutTaskname, m_ptrCut->m_guid );

		//Add it to the Taskpad under the new group
		m_taskpad.CreateTask( ptrGroup->m_text, m_ptrCut->m_text, m_ptrCut->m_description, m_ptrCut->m_identifier, m_ptrCut->m_guid);

		//Reset my Cut item
		delete m_ptrCut;
		m_ptrCut = NULL;

		m_strCutTaskname.Empty();
	}
}

void CTaskpadView::DeleteCut()
{
	CString		strDescription,
				strTaskname;
//	CLDVPTaskFS	*ptrTaskOCX;
	RECT		rect;

	memset( &rect, 0, sizeof( RECT ));

	if( m_ptrCut )
	{
		delete m_ptrCut;
		m_ptrCut = NULL;
	}
}

BOOL CTaskpadView::CanCut()
{
	//I can cut the same items that I can delete
	//	so I will simply call that function
	return CanDeleteSelTask();
}

void CTaskpadView::OnCut() 
{
	Cut();
}

void CTaskpadView::OnUpdateCut(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( CanCut() );
}

void CTaskpadView::OnPaste() 
{
	Paste();
}

void CTaskpadView::OnUpdatePaste(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( CanPasteCurGroup() );	
}

void CTaskpadView::FocusToRoot()
{
	m_taskpad.SelectItem( m_taskpad.GetRootItem() );
}
