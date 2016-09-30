// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: Taskpad.Cpp
//  Purpose: CTaskPad Implementation file
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
#include "WProtect32.h"
#include "Taskpad.h"
#include "ldvptaskfs.h"
#include "TaskpadView.h"
#include "DefaultView.h"
#include "MainFrm.h"
#include "TaskDescriptionView.h"
#include "Taskview.h"
#include "ClientReg.h"
#include "WProtect32Doc.h"
#include "rpc.h"

#include "ldvpsnapin.h"
#include "ConfigureSnapIN.h"
#include "admininfo.h"

#include "vphtmlhelp.h"
#include "SymSaferRegistry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constants
#define IMAGE_LIST_IMAGE_SIZE  16
#define MASK_IDB_OBJECTS       RGB(0, 255, 0)

extern CString	m_gHelpFileName;
extern CString m_gEncyclopediaName;

#include "ScanShared.h"

//
// Definitions
//
//For backward compatibility, I need to know the GUIDS
//	of the two task OCX's. I know this is a dumb way to
//	accomplish this, but it is the easiest.

//"BA0C6364-7218-11D0-8865-444553540000"
EXTERN_C const GUID TASK_GUID_VPC50 \
        = { 0xba0c6364, 0x7218, 0x11d0, { 0x88, 0x65,  0x44,  0x45,  0x53,  0x54,  0,  0 } };
//"64B4A5AE-0799-11d1-812A-00A0C95C0756"
EXTERN_C const GUID TASK_GUID_VPC51 \
        = { 0x64b4a5ae, 0x799, 0x11d1, { 0x81, 0x2a, 0x0, 0xa0, 0xc9, 0x5c, 0x7, 0x56} };

void AddTaskToMenu( CTaskpad *ptrTaskPad, CString strGroup, DWORD dwID, CString strTaskName )
{
	CMenu		*mainMenu = AfxGetMainWnd()->GetMenu();
	CTaskGroup	*pGroup;

	if( mainMenu )
	{
		int		iMenuCount = mainMenu->GetMenuItemCount();
		int		iLoop;
		CString strMenuGroup;
		CMenu	*ptrSubMenu = NULL;

		//Find the right menu
		for( iLoop = 0; iLoop < iMenuCount; iLoop++ )
		{
			mainMenu->GetMenuString( iLoop, strMenuGroup, MF_BYPOSITION );

			//If I found it, then break out of the loop
			if( 0 == strMenuGroup.CompareNoCase( strGroup ) )
			{
				ptrSubMenu = mainMenu->GetSubMenu( iLoop );
				break;
			}

			strMenuGroup.Empty();
		}

		//If there is no menu group by this name,
		//	ignore it
		if( ptrSubMenu )
		{
			// added by terrym 8-29-00  The string in the list is now stored with out the &
			RemoveChars( strGroup, _T('&') );

			//Find the group ID and use it to make the ID for this menu item.
			//	I do this to guarantee uniqueness with the ID.
			POSITION pos = ptrTaskPad->m_GroupIds.GetStartPosition();
			WORD	wGroupID;
			while(pos)
			{
				ptrTaskPad->m_GroupIds.GetNextAssoc( pos, wGroupID, (CObject*&)pGroup );
				if( 0 == pGroup->m_text.CompareNoCase( strGroup )  )
				{
					//Set pos to NULL to drop out of the loop
					pos = NULL;
				}
			}

			//If the group word is 0, then I need to mask in the wGroupID from above
			WORD wordID = LOWORD( dwID ),
				 wordGroupID = HIWORD( dwID );

			if( wordGroupID == 0 )
				dwID = MAKELONG( wordID, wGroupID );

			ptrSubMenu->AppendMenu( MF_STRING | MF_ENABLED, StoreSnapInCommand( dwID ), (LPCTSTR)strTaskName );
		}
	}
}

//----------------------------------------------------------------
// Constructor
//----------------------------------------------------------------
CTaskpad::CTaskpad()
:	CTaskpadExt(),
	m_displayedParent( NULL ),
	m_oldItem( NULL ),
	m_bScanningComponentInstalled( FALSE )
{
	m_ySize = 0;
	m_bDragging = FALSE;
	m_bCanUpdateRegKey = FALSE;

	cursorNo = LoadCursor( NULL, IDC_NO );

    CAdminInfo  AdminInfo;

    // See if we're a normal user
    m_bCanUpdateRegKey = AdminInfo.CanProcessUpdateRegKey( NULL, HKEY_LOCAL_MACHINE, szReg_Key_Main );

}

//----------------------------------------------------------------
// Destructor
//----------------------------------------------------------------
CTaskpad::~CTaskpad()
{
	m_ySize = 0;
	m_bDragging = FALSE;

	cursorNo = LoadCursor( NULL, IDC_NO );

    // Ensure that we have a valid window handle
	if( AfxGetMainWnd()->m_hWnd != NULL )
	    ::WinHelp( AfxGetMainWnd()->m_hWnd, m_gEncyclopediaName, HELP_QUIT, 0 );

}


BEGIN_MESSAGE_MAP(CTaskpad, CTaskpadExt)
	//{{AFX_MSG_MAP(CTaskpad)
	ON_NOTIFY_REFLECT(TVN_GETDISPINFO, OnGetdispinfo)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemexpanding)
	ON_WM_MOUSEMOVE()
	ON_NOTIFY_REFLECT(NM_DBLCLK, Ondblclk)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelChanged)
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBegindrag)
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_MESSAGE( UWM_SELECTBYPOINT, SelectTaskByPoint )
END_MESSAGE_MAP()

//----------------------------------------------------------------
// CreateGroup
//----------------------------------------------------------------
CTaskGroup *CTaskpad::CreateGroup(LPCTSTR szGroup, BOOL bScanningTasks, long lImageID)
{
	CTaskGroup	tskGroup;
	HTREEITEM	item;
	CSubTask	tskTask;

	//First, see if the group already exists

	//All groups are children of the Root item
	item = GetChildItem( GetRootItem() );

	//All groups are children of the Root,
	// so i need to look at all children to find
	// the group I am looking for
	while( item != NULL )
	{
		CString strItem;
		CString strGroup( szGroup );
		CTaskGroup *ptrGroup;
		TV_ITEM	tvItem;

		tvItem.hItem = item;
		tvItem.mask = TVIF_HANDLE;

		GetItem( &tvItem );
		ptrGroup = (CTaskGroup*)tvItem.lParam;

		if( ptrGroup )
		{
			if( ptrGroup->m_text == strGroup )
			{
				//Found it!
				return NULL;
			}
		}

		//Otherwise, get the next item
		item = GetNextSiblingItem( item );

	}

	//If I am still in this funciton, then I never found the
	//	group. I can now create the new group
	tskGroup.m_text = szGroup;
	tskGroup.m_bScanTaskGroup = bScanningTasks;
	tskGroup.m_image = lImageID;

	// and add it
	return AddTaskGroup( &tskGroup );
}

//----------------------------------------------------------------
// OnCreateTask
//----------------------------------------------------------------
CSubTask *CTaskpad::CreateTask(LPCTSTR GroupName, LPCTSTR TaskName, LPCTSTR TaskDescript, long TaskID, GUID guid, long lImageID /*==17*/, TaskType taskType /* = TASK_EXTERNAL */ )
{
	HTREEITEM	item;
	CTaskGroup	tskGroup;
	CSubTask	tskTask;

	//If the first three strings are empty, that signals
	//	me that this is simply the Recording task.
	if( ( *GroupName == NULL ) &&
		( *TaskName  == NULL ) &&
		( *TaskDescript == NULL ) )
	{
		m_recordingGUID = guid;
		m_recordingTaskID = TaskID;
		return NULL;
	}

	// *** Add the item to the TaskPad. ***

	//First, find the group

	//All groups are children of the Root item
	item = GetChildItem( GetRootItem() );

	//All groups are children of the Root,
	// so i need to look at all children to find
	// the group I am looking for
	while( item != NULL )
	{
		CString strItem;
		CString strGroup( GroupName );
		CTaskGroup *ptrGroup;
		TV_ITEM	tvItem;

		tvItem.hItem = item;
		tvItem.mask = TVIF_HANDLE;

		GetItem( &tvItem );
		ptrGroup = (CTaskGroup*)tvItem.lParam;

		if( ptrGroup )
		{
			if( ptrGroup->m_text == strGroup )
			{
				//Found it!
				tskGroup = *ptrGroup;
				break;
			}
		}

		//Otherwise, get the next item
		item = GetNextSiblingItem( item );

	}

	//If the item is NULL, then I never found it.
	// That means I need to add it
	if( item == NULL )
	{
		//Create the group
		tskGroup.m_text = GroupName;
		tskGroup.m_bScanTaskGroup = TRUE;
		// and add it
		AddTaskGroup( &tskGroup );

		//Create a Menu group in the MainFrame to match
		//	the TaskPad group
		CMenu *mainMenu = AfxGetMainWnd()->GetMenu();
		if( mainMenu )
		{
			int		iMenuCount = mainMenu->GetMenuItemCount();
			CMenu	menuNewGroup;

			menuNewGroup.CreateMenu();
			mainMenu->InsertMenu( iMenuCount-1, MF_BYPOSITION | MF_POPUP, (UINT)menuNewGroup.m_hMenu, GroupName );
		}

	}

	//Now, add the item to the group
	tskTask.m_text = TaskName;
	tskTask.m_description = TaskDescript;
	tskTask.m_identifier = TaskID;
	tskTask.m_image = lImageID;
	tskTask.m_guid = guid;
	tskTask.m_dwTaskType = taskType;

	return AddSubTask( &tskGroup, &tskTask );
}

//----------------------------------------------------------------
// CreateTask #2:  Creates a task with an icon supplied by the task DLL
//----------------------------------------------------------------

CSubTask* CTaskpad::CreateTask(LPCTSTR GroupName, LPCTSTR TaskName, LPCTSTR TaskDescript, long TaskID, GUID guid, CString iconFilename, UINT iconResourceID, TaskType taskType )
// Creates a task with an icon supplied by the task DLL
{
    HMODULE     moduleHandle    = NULL;
    HICON       iconHandle      = NULL;
    DWORD       imageID         = 0;
    CSubTask*   returnVal       = NULL;

    // Validate parameters
    if (iconFilename == "")
        return NULL;

    // Load the icon, add it to our image list, and then defer to the regular CreateTask
    moduleHandle = LoadLibrary((LPCTSTR) iconFilename);
    if (moduleHandle != NULL)
    {
        iconHandle = LoadIcon(moduleHandle, MAKEINTRESOURCE(iconResourceID));
        if (iconHandle != NULL)
        {
            imageID = m_images.Add(iconHandle);
            if (imageID != -1)
                returnVal = CreateTask(GroupName, TaskName, TaskDescript, TaskID, guid, imageID, taskType);
            DeleteObject(iconHandle);
            iconHandle = NULL;
        }
        FreeLibrary(moduleHandle);
        moduleHandle = NULL;
    }

    return returnVal;
}

//----------------------------------------------------------------
// SelectItem
//	KLUDGE ALERT
//		When calling SelectItem the title bar changes
//		to just the Doc title. To get around this, I am now
//		grabbing the title and restoring it after the call.
//----------------------------------------------------------------
BOOL CTaskpad::SelectItem( HTREEITEM item )
{
	BOOL bRet;
	CString	strTitle;

	AfxGetMainWnd()->GetWindowText( strTitle );

	// MH 08.25.00
	// If the user selects an already selected item, start the task
	// manually so that we call FireTask() every time.
	HTREEITEM hSelected = GetSelectedItem();

	if( hSelected == item )
		{
		bRet = StartSelectedTask( item );
		}
	else
		{
		bRet = CTreeCtrl::SelectItem( item );
		}

	AfxGetMainWnd()->SetWindowText( strTitle );

	return bRet;
}

//----------------------------------------------------------------
// DestroyTree
//----------------------------------------------------------------
void CTaskpad::DestroyTree( HTREEITEM item /* = TVI_ROOT */ )
{
	HTREEITEM nextItem;
	CTaskpadObject* pData;

	if ( item == TVI_ROOT )
		item = GetRootItem();

	if ( !item )
		goto cleanup;

	//Get the Children of this item
	nextItem = GetChildItem( item );
	if ( nextItem )
	{
		//And destroy it
		DestroyTree( nextItem );
	}

	//Free the memory associated with the Tree item
	pData = (CTaskpadObject*)GetItemData( item );
	delete pData;
	//EA start of fix 06/05/2001
	pData = NULL;
	SetItemData(item, (DWORD)pData);
	//EA end of fix 06/05/2001
	//And get the next item
	nextItem = GetNextSiblingItem( item );
	if ( nextItem )
	{
		//And destroy it also
		DestroyTree( nextItem );
	}

cleanup:
	return;
}


//----------------------------------------------------------------
// AddDefaultTasks
//----------------------------------------------------------------
BOOL CTaskpad::AddDefaultTasks()
{
	CTaskGroup group;
	CSubTask task;
	BOOL rc = TRUE;
//	int iLoop;

	//Set the Group bitmap to NULL
	group.m_hbitmap = NULL;

	//Set the CanDragDropChildren flag
	group.SetCanDragDropChildren( TRUE );

	DWORD dwClientType = GetClientType();
	DWORD bClientTypeServer = (dwClientType == CLIENT_TYPE_SERVER);
	DWORD bUnmanagedClient = (dwClientType != CLIENT_TYPE_SERVER) && (dwClientType != CLIENT_TYPE_CONNECTED);
	// are startup scans turned on, and not locked?
	DWORD bStartupScansEnabled = StartupScansEnabled() && !StartupScansLocked();

	if( m_bScanningComponentInstalled && !bClientTypeServer )
	{
		// always show this option for unmanaged clients
		if (bStartupScansEnabled || bUnmanagedClient)
		{
			VERIFY( group.m_text.LoadString( IDS_DEFTASK_STARTUP ) );
			group.m_image = 14;
			group.m_bScanTaskGroup = FALSE;
			AddTaskGroup( &group );

			task.m_identifier = m_recordingTaskID;
			task.m_guid = m_recordingGUID;
			task.m_dwTaskType = TASK_RECORDABLE;
			task.m_recordType = RECORDING_STARTUP;
			task.m_text.LoadString(IDS_NEW_STARTUP_TASK);
			task.m_description.LoadString( IDS_NEW_STARTUP );
			task.m_image = 11;
			AddSubTask( &group, &task );

			//Load any saved tasks for this group
			LoadTaskGroup( group.m_text, szReg_Value_Startup );
		}

		VERIFY( group.m_text.LoadString( IDS_DEFTASK_SCRIPTED ) );
		group.m_image = 7;
		group.m_bScanTaskGroup = FALSE;
		AddTaskGroup( &group );

		//Add the New task
		task.m_identifier = m_recordingTaskID;
		task.m_guid = m_recordingGUID;
		task.m_dwTaskType = TASK_RECORDABLE;
		task.m_text.LoadString(IDS_NEW_SCRIPTED_TASK);
		task.m_description.LoadString( IDS_NEW_SCRIPTED );
		task.m_recordType = RECORDING_SCRIPTED;
		task.m_image = 11;
		AddSubTask( &group, &task );

		//Load any saved tasks for this group
		LoadTaskGroup( group.m_text, szReg_Value_Scripted );

		VERIFY( group.m_text.LoadString( IDS_GROUP_SCHEDULED_TASKS ) );
		group.m_image = 13;
		group.m_bScanTaskGroup = FALSE;
		AddTaskGroup( &group );

		//Add the New task
		task.m_text.LoadString(IDS_NEW_SCHEDULED_TASK);
		task.m_recordType = RECORDING_SCHEDULE;
		task.m_description.LoadString( IDS_NEW_SCHEDULED );
		task.m_image = 11;
		AddSubTask( &group, &task );

		//Load any saved tasks for this group
		LoadTaskGroup( group.m_text, szReg_Value_Scheduled );
	}

	group.SetCanDragDropChildren( FALSE );

	VERIFY( group.m_text.LoadString( IDS_DEFTASK_HELP ) );
	group.m_image = 1;
	group.m_bScanTaskGroup = FALSE;
	AddTaskGroup( &group );

	task.m_dwTaskType = TASK_HELP;
	task.m_image = 16;
	task.m_text.LoadString(IDS_HELP_CONTENTS);
	task.m_identifier = IDS_HELP_CONTENTS;
	task.m_description.LoadString( IDS_HELP_CONTENTS_DESCRIPTION );
	AddSubTask( &group, &task );

	/*VERIFY( group.m_text.LoadString( IDS_VIRUS_ENCYCLOPEDIA ) );
	group.m_image = 2;
	group.m_bScanTaskGroup = FALSE;
	AddTaskGroup( &group );


	task.m_dwTaskType = TASK_HELP;
	task.m_image = 16;
	for( iLoop = IDS_VIRUSES_SEARCH; iLoop <= IDS_VIRUSES_Z; iLoop++ )
	{
		task.m_text.LoadString(iLoop);
		task.m_identifier = iLoop;
		task.m_description.LoadString( ++iLoop );
		AddSubTask( &group, &task );
	}*/

	Expand( GetRootItem(), TVE_EXPAND );

	return rc;
}

//----------------------------------------------------------------
// ShowChildren
//----------------------------------------------------------------
BOOL CTaskpad::ShowChildren( HTREEITEM parent, BOOL bShow )
{
	CSplitter *splitter = ((CMainFrame*)GetParentFrame())->m_wndSplitter;

	//Show the window
	if( bShow )
	{
		CTaskDescriptionView *ptrView;

		m_displayedParent = parent;

		//Switch views
		splitter->ReplaceView( 0, 1, RUNTIME_CLASS(CTaskDescriptionView), CSize(0,0));

		//Replace the View in the second pane
		ptrView = (CTaskDescriptionView*)splitter->GetPane( 0, 1 );
		ptrView->SetTaskPadPtr( this );
		ptrView->Invalidate();
		ptrView->UpdateWindow();
	}
	else
	// Hide the window
	{
		//Switch views
		splitter->ReplaceView( 0, 1, RUNTIME_CLASS(CDefaultView), CSize(0,0));
	}

	return TRUE;
}

//----------------------------------------------------------------
// SizeTaskWindow
//----------------------------------------------------------------
BOOL CTaskpad::SizeTaskWindow()
{
	return TRUE;
}

//----------------------------------------------------------------
// DrawTaskWindow
//----------------------------------------------------------------
BOOL CTaskpad::DrawTaskWindow( CDC* pDC )
{
	BOOL		rc = TRUE;
	CRect		rect;
	HTREEITEM	item;
	CFont		*pFont;
	LOGFONT		lf;
	CFont		boldFont,
				*pOldFont,
				titleFont;
	UINT		cxMax;
	UINT		cyMax;
	UINT		x;
	UINT		y;
	CBrush		brushBackground;
	CString		strTask,
				strGroup;
	int			oldBkMode;
	COLORREF	oldColor;

	//Get the proper view
	CTaskDescriptionView* ptrView = (CTaskDescriptionView*)((CMainFrame*)GetParentFrame())->m_wndSplitter->GetPane( 0, 1 );

	//Make sure the item has children
	item = GetChildItem( m_displayedParent );
	if ( !item )
		goto cleanup;

	// create the header font
	pFont = GetFont();
	pFont->GetLogFont( &lf );
	//lf.lfWeight = 700;
	lf.lfUnderline = TRUE;
	boldFont.CreateFontIndirect( &lf );

	lf.lfUnderline = FALSE;
	//lf.lfHeight += 32;
	lf.lfHeight += (lf.lfHeight > 0) ? 12 : -12;
	titleFont.CreateFontIndirect( &lf );

	ptrView->GetClientRect( &rect );

	//Paint the background grey
	brushBackground.CreateSolidBrush( (COLORREF)GetSysColor( COLOR_BTNFACE ) );
	pDC->FillRect( &rect, &brushBackground );
	brushBackground.DeleteObject();

	cxMax = rect.right - rect.left - 10;

	rect.top += 10;
	rect.left += 10;

	//Draw the title
	oldBkMode = pDC->SetBkMode( TRANSPARENT );
	GetSelTask( strTask, strGroup );
	pOldFont = pDC->SelectObject( &titleFont );
	oldColor = pDC->SetTextColor( (COLORREF)GetSysColor( COLOR_WINDOWTEXT ) );
	pDC->DrawText( strTask, rect, DT_WORDBREAK | DT_CALCRECT );
	pDC->DrawText( strTask, rect, DT_WORDBREAK );
	pDC->SelectObject( pOldFont );
	pDC->SetBkMode( oldBkMode );
	pDC->SetTextColor( oldColor );

	//Initialize my X and Y values
	y = rect.bottom + 10;
	x = rect.left + 10;

	// Fix 1-3S77L5.
	// Move items down the task list so that the File System Auto-Protect task in the Configure task
	// list does not appear in the same location as the Enable Auto-Protect checkbox.
	// A lot of users double-click the task item, and accidentally disable Auto-Protect when the 
	// File System Auto-Protect UI was displayed.
	//
	y = y + 12;

	//While there are children
	while( item )
	{
		CSubTask* pData;

		//Get the data for the child
		VERIFY( pData = (CSubTask*)GetItemData( item ) );

		//Draw the item
		cyMax = rect.bottom - y - 10;
		VERIFY( pData->DrawTask( x, &y, cxMax, pDC,
					&boldFont, pFont, &m_images ) );

		//Move down the screen (but make sure the scroll bar does not appear with the default UI size).
		y = y + 18;

		//And get the next item
		item = GetNextSiblingItem( item );
	}

cleanup:
	m_ySize = y;
	return rc;
}

/////////////////////////////////////////////////////////////////////////////
// CTaskpad message handlers

//----------------------------------------------------------------
// OnGetdispInfo
//----------------------------------------------------------------
void CTaskpad::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	CTaskpadObject* pObject;
	TV_ITEM* pItem;
	CString	 strItem;

	VERIFY( pItem = &(pTVDispInfo->item) );
	//VERIFY( pObject = (CTaskpadObject*)(pItem->lParam) );
	//EA start of fix 06/05/2001
	pObject = (CTaskpadObject*)(pItem->lParam);
	//EA end of fix 06/05/2001
	if( ( pItem->mask & TVIF_TEXT ) == TVIF_TEXT)
	{
		//EA start of fix 06/05/2001
		if(pObject) //EA end of fix 06/05/2001
			strItem = pObject->m_text;

		//I don't want any underscores in the Taskpad
		//	so remove them all now
		RemoveChars( strItem, _T("&") );

		_tcscpy( pItem->pszText, (LPCTSTR)strItem );
	}

	//Make sure we are displaying the proper image
	//EA start of fix 06/05/2001
	if(pObject)
	{
		pItem->iImage = pObject->m_image;
		pItem->iSelectedImage = pObject->m_image;
	}
	//EA end of fix 06/05/2001
	pItem->state = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

	*pResult = 0;
}

//----------------------------------------------------------------
// OnDestroy
//----------------------------------------------------------------
void CTaskpad::OnDestroy()
{
	DestroyTree();

	VERIFY( m_images.DeleteImageList() );

	//And destroy the Tree control
	CTreeCtrl::OnDestroy();
}

//----------------------------------------------------------------
// OnCreate
//----------------------------------------------------------------
int CTaskpad::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CTaskGroup group;

	//Call the default implementation
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	CFont* pFont;
	LOGFONT lf;
	CRect rect;
	BOOL rc = -1;

	VERIFY( group.m_text.LoadString( IDS_LDVP_TITLE ) );
	group.m_image = 0;
	AddTaskGroup( &group, TRUE );

	//Create the Image list
	// It would be better to get these bitmaps out of LVTask.ocx and other binaries
	//	   and add them to the image list here.  That would require adding
	//	   interfaces and probably changing image list indexes in a lot of places.
	//	   (Those should be constants anyway.)
	if ( !m_images.Create( IDB_OBJECTS, IMAGE_LIST_IMAGE_SIZE, 10, MASK_IDB_OBJECTS ) )
	{
		TRACE( "Taskpad: Failed to create imagelist\n" );
		goto cleanup;
	}

	SetImageList( &m_images, TVSIL_NORMAL );

	// create the fonts
	pFont = GetFont();
	pFont->GetLogFont( &lf );

	m_normalFont.CreateFontIndirect( &lf );

	lf.lfUnderline = 1;
	m_underlineFont.CreateFontIndirect( &lf );

	rc = 0;

cleanup:

	return rc;
}

//----------------------------------------------------------------
// OnItemExpanding
//----------------------------------------------------------------
void CTaskpad::OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	TV_ITEM* pItem;
	CSplitter *splitter = ((CMainFrame*)GetParentFrame())->m_wndSplitter;
	CTaskView *ptrTasks = (CTaskView*)splitter->GetPane( 0, 1 );

	pItem = &(pNMTreeView->itemNew);
	Select( pItem->hItem, TVGN_CARET );

	//If we are expanding, show the children
//	if( pNMTreeView->action == TVE_EXPAND )
//	{
		if ( GetParentItem( pItem->hItem ) )
			goto cleanup;

		if( pItem->hItem != GetRootItem() )
			ShowChildren( pItem->hItem, TRUE );
//	}
	//otherwise, hide the children
//	else
//	{
//		ShowChildren( pItem->hItem, FALSE );
//	}

cleanup:
	*pResult = 0;
}


//----------------------------------------------------------------
// OnBeginDrag
//----------------------------------------------------------------
void CTaskpad::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM	pParentItem = GetParentItem( pNMTreeView->itemNew.hItem );
	CTaskGroup	*pTaskGroup;
	TV_ITEM		tvParent,
				tvItem;
	CSubTask	*pTask;

	*pResult = 0;

	memset( &tvParent, '\0', sizeof( TV_ITEM ) );
	memset( &tvItem, '\0', sizeof( TV_ITEM ) );

	//Figure out if the item can be drug

	//If it doesn't have a parent, then it definiately cannot
	// drag
	if( !pParentItem )
	{
		return;
	}

	//Now I know it's a child. Figure out if it
	// is a child of a dynamic group
	tvParent.hItem = pParentItem;
	tvParent.mask = TVIF_HANDLE;
	GetItem( &tvParent );
	pTaskGroup = (CTaskGroup*)tvParent.lParam;

	tvItem.hItem = pNMTreeView->itemNew.hItem;
	tvItem.mask = TVIF_HANDLE;
	GetItem( &tvItem );
	pTask = (CSubTask*)tvItem.lParam;

	if( pTaskGroup )
	{
		if( !pTaskGroup->CanDragDropChildren() )
			return;

		//Now I know the parent group allows Drag/Drop,
		// check the item itself.
		//NOTE: ONly external tasks can be drag/dropped.
		if( pTask->m_dwTaskType != TASK_EXTERNAL )
			return;
	}

	//Now, set the dragging item data
	m_draggingItem = pNMTreeView->itemNew;

	//Tell the list to create an image for dragging
	cursorOld = GetCursor();
	SetCapture();
	m_bDragging = TRUE;

}

//----------------------------------------------------------------
// OnLButtonUp
//----------------------------------------------------------------
void CTaskpad::OnLButtonUp(UINT nFlags, CPoint point)
{
	HTREEITEM		item,
					parentItem;
	UINT			flags;
	CSubTask		*ptrTask;
	CTaskGroup		*ptrGroup;
	TV_ITEM			tvItem;
	CString			strDescription,
					strFilename,
					strScheduledGroup,
					strNewGroup;

	if( m_bDragging )
	{
		//If we are over a valid drop item
		if( cursorOld == GetCursor() )
		{
			//Figure out if we are over a parent item or a child item
			item = HitTest( point, &flags );
			parentItem = GetParentItem( item );
			if( parentItem != GetRootItem() )
			{
				TRACE0("Dropped on a Task\n" );
			}
			else
			{
				TRACE0("Dropped on a Group\n" );
				parentItem = item;
			}

			//First, fill in my CSubTask and CTaskGroup
			// pointers so I have all the data I need
			memset( &tvItem, '\0', sizeof (TV_ITEM ) );
			tvItem.hItem = parentItem;
			tvItem.mask = TVIF_HANDLE;
			GetItem( &tvItem );

			ptrGroup = (CTaskGroup*)tvItem.lParam;

			memset( &tvItem, '\0', sizeof (TV_ITEM ) );
			tvItem.hItem = m_draggingItem.hItem;
			tvItem.mask = TVIF_HANDLE;
			GetItem( &tvItem );

			ptrTask = (CSubTask*)tvItem.lParam;

			HTREEITEM hItemParent = GetParentItem( m_draggingItem.hItem );
			// MH 08.27.00
			// Make sure that we're not being dropped in the same task group.  We
			// only want to create the new task if we're being dragged and dropped
			// from one task group to another.
			if( hItemParent != ptrGroup->m_handle )
			{
				strScheduledGroup.LoadString( IDS_GROUP_SCHEDULED_TASKS );

				//Save off the new group
				strNewGroup = ptrGroup->m_text;

				//Get the task info from the registry
				GetCustomTaskInfo( ptrTask->m_strGroup, ptrTask->m_text, strDescription, strFilename, ptrTask->m_guid, TRUE );

				//If the item is dropped on a Scheduled Scan group,
				// call the Modal dialog box
				if( strScheduledGroup == ptrGroup->m_text )
				{
					if( !GetScheduleInfo( ptrTask->m_text, strDescription, strFilename, ptrTask->m_guid ) )
					{
						//If they canceled, then re-add the task where it
						// was
						strNewGroup = ptrTask->m_strGroup;
					}
				}
				//Otherwise, I need to remove the schedule information
				//	from the scan key
				else
				{
					RemoveScheduleInfo( strFilename );
				}

				//update the registry & CSubTask with the proper values
				// for the new group
				SaveCustomTaskInfo( strNewGroup, ptrTask->m_text, ptrTask->m_description, strFilename, ptrTask->m_guid );

				//Add it to the Taskpad under the new group
				CreateTask( strNewGroup, ptrTask->m_text, strDescription, ptrTask->m_identifier, ptrTask->m_guid );

				//and remove it from it's previous location
				DeleteItem( m_draggingItem.hItem );

				delete ptrTask;
			}
			else
			{
				TRACE0("Dropped on InValid item\n");
			}
		}
		else
		{
			TRACE0("Dropped on InValid item\n" );
		}

		m_bDragging = FALSE;
		memset( &m_draggingItem, '\0', sizeof( TV_ITEM ) );
		SetCursor( cursorOld );
		ReleaseCapture();
	}

	CTreeCtrl::OnLButtonUp(nFlags, point);
}

//----------------------------------------------------------------
// OnMouseMove
//----------------------------------------------------------------
void CTaskpad::OnMouseMove(UINT nFlags, CPoint point)
{
	CPoint				pt( GetMessagePos() );
	UINT				flags;
	HTREEITEM			item;
	CTaskpadObject		*pData;
	CRect				rect;
	CDC					*pdc;
	CFont				*oldFont;
	int					oldBkMode;
	COLORREF			oldColor;
//	COLORREF			ColorRef;
	BOOL				selected;
	BOOL				bFontChanged=FALSE;
	HTREEITEM			pParentItem;
	TV_ITEM				tvParent;
	CTaskGroup			*pTaskGroup = NULL;
	CString				strItem;
	LOGFONT lf;
	LOGFONT curlf;
	CFont* curFont = GetFont();

	ScreenToClient( &pt );
	item = HitTest( pt, &flags );
	pParentItem = GetParentItem( item );

	if ( item && ( flags & TVHT_ONITEMLABEL ) )
	{
		VERIFY( pData = (CTaskpadObject*)GetItemData( item ) );
		GetItemRect( item, &rect, TRUE );
		selected = ( ( GetSelectedItem() == item ) ? TRUE : FALSE );

		if( m_bDragging )
		{

			//Check if I am over an item I can drop on.
			if( pParentItem == GetRootItem() || !pParentItem )
			{
				pParentItem = item;
			}


			tvParent.hItem = pParentItem;
			tvParent.mask = TVIF_PARAM;

			GetItem( &tvParent );
			pTaskGroup = (CTaskGroup*)tvParent.lParam;

			if( pTaskGroup )
			{
				if( (!pTaskGroup->CanDragDropChildren()) ||
					pParentItem == GetParentItem( m_draggingItem.hItem ) )
				{
					SetCursor( cursorNo );
					return;
				}
			}

			SetCursor( cursorOld );

			//Otherwise, draw the item as a drop target
//			ColorRef = RGB( 0xff, 0x00, 0x00 );
		}
		else
		{
//			ColorRef = RGB( 0x00, 0x00, 0xff );
		}

		//Draw the item
		rect.top += 1;
		rect.bottom += 1;
		rect.left += 2;
		rect.right += 2;

		//I don't want any underscores in the Taskpad
		//	so remove them all now
		strItem = pData->m_text;
		RemoveChars( strItem, _T("&" ) );

		VERIFY( pdc = GetDC() );

		curFont->GetLogFont(&curlf);

		m_normalFont.GetLogFont(&lf);

		// Compare the LOGFONT members to see if
		// the font has changed
		if( _tcscmp(lf.lfFaceName, lf.lfFaceName) != 0 ||
			curlf.lfHeight != lf.lfHeight ||
			curlf.lfItalic != lf.lfItalic ||
			curlf.lfWeight != lf.lfWeight ||
			curlf.lfStrikeOut != lf.lfStrikeOut ||
			curlf.lfWidth != lf.lfWidth )
		{
			// The font has changed.
			bFontChanged = TRUE;
		}


		if( m_bDragging )
		{
				oldFont = pdc->SelectObject( &m_underlineFont );
		}
		else
		{
			if( FALSE == bFontChanged )
			{
				// Nothing's changed so keep the normal font.
				oldFont = pdc->SelectObject( &m_normalFont );
			}
			else
			{
				// The font has changed, so create the new font.
				m_normalFont.CreateFontIndirect(&curlf);
				curlf.lfUnderline = TRUE;
				m_underlineFont.CreateFontIndirect(&curlf);
				oldFont = pdc->SelectObject( &m_normalFont );
			}
		}
		oldBkMode = pdc->SetBkMode( TRANSPARENT );

//		oldColor = pdc->SetTextColor( RGB( 0x00, 0x00, 0x00 )  );
		oldColor = pdc->SetTextColor( (COLORREF)GetSysColor( COLOR_WINDOWTEXT ) );
		if ( selected && ( this == GetFocus() ) )
			oldColor = pdc->SetTextColor( RGB( 0xff, 0xff, 0xff ) );

//		pdc->DrawText( pData->m_text, &rect, 0 );
//		pdc->DrawText( strItem, &rect, 0 );
		pdc->SetTextColor( oldColor );
		pdc->SetBkMode( oldBkMode );
		pdc->SelectObject( oldFont );
		ReleaseDC( pdc );
	}
	else if( m_bDragging )
	{
		SetCursor( cursorNo );
	}


cleanup:
	CTreeCtrl::OnMouseMove(nFlags, point);
}

//----------------------------------------------------------------
// Ondblclck
//----------------------------------------------------------------
void CTaskpad::Ondblclk(NMHDR* pNMHDR, LRESULT* pResult)
{
	//Fire the task
	FireTask( TRUE );
	AfxGetMainWnd()->PostMessage( TASK_FOCUS, 0, 0L );
	*pResult = 0;
}

//----------------------------------------------------------------
// OnSelChanged
//----------------------------------------------------------------
void CTaskpad::OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	TV_ITEM			*pItem;
	CTaskGroup		*ptrTask;
	NM_TREEVIEW		*pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	CSplitter		*splitter = ((CMainFrame*)GetParentFrame())->m_wndSplitter;

	pItem = &(pNMTreeView->itemNew);

	//If the item clicked is a Task, then start it.
	ptrTask = (CTaskGroup*)pItem->lParam;

	if( ptrTask == NULL || pItem->hItem == GetRootItem() )
	{
		((CMainFrame*)AfxGetApp()->m_pMainWnd)->SwitchToView( RUNTIME_CLASS( CDefaultView ) );
		return;
	}

	//I can tell if it is a task my getting the ItemData
	if( ptrTask->IsKindOf( RUNTIME_CLASS( CTaskGroup ) ) )
	{
		HTREEITEM hTempItem = GetNextVisibleItem( pItem->hItem );

		//If the children are visible
//		if( (hTempItem != NULL ) && (hTempItem == GetChildItem(pItem->hItem) ) )
//		{
			//Show the descriptions
			ShowChildren( pItem->hItem, TRUE );
//		}
//		else
//		{

//		}
	}
	else
	{
		//If it's not a taskgroup, then it's a subtask.
		FireTask( FALSE );
	}

	AfxGetMainWnd()->PostMessage( TASK_FOCUS, 0, 0L );
	*pResult = 0;
}

//----------------------------------------------------------------
// SelectConfigurationSnapInTask
//----------------------------------------------------------------
void CTaskpad::SelectConfigurationSnapInTask( WORD wID, WORD wGroupID )
{
	BOOL		bFound = FALSE;
	HTREEITEM	treeItem,
				childItem;
	CSubTask	*ptrTask;
	CString		strCategory;
	TV_ITEM		item;
	CLDVPSnapIn	*snapIn = NULL;
	CString sRaw, sMenu, sTaskpad;
	memset( &item, 0x00, sizeof( item ) );

	sRaw.LoadString( IDS_GROUP_CONFIGURE );
	ConvertRawMenuString(sRaw,sMenu,sTaskpad);

	strCategory = sTaskpad;
	//All groups are children of the Root item
	treeItem = GetChildItem( GetRootItem() );

	while( treeItem != NULL && !bFound )
	{
		//First, find the group
		if( strCategory == GetItemText( treeItem ) )
		{
			bFound = TRUE;
			continue;
		}

		treeItem = GetNextSiblingItem( treeItem );
	}

	strCategory.ReleaseBuffer();

	//Make sure the root item is expanded
	Expand( GetRootItem(), TVE_EXPAND );

	//Make sure the configuration group is expanded
	Expand( treeItem, TVE_EXPAND );

	//Now, look at the children to find the right task
	childItem = GetChildItem( treeItem );

	//RE-initialize my bFound variable
	bFound = FALSE;

	while( childItem != NULL && !bFound )
	{

		item.hItem = childItem;
		item.mask = TVIF_HANDLE;

		GetItem( &item );

		ptrTask = (CSubTask*)item.lParam;

		if( ptrTask )
		{
			try
			{
				snapIn = new CLDVPSnapIn( ptrTask->m_strInternalName );
				if( snapIn )
				{
					snapIn->Read();
					if( ptrTask->IsKindOf( RUNTIME_CLASS( CSubTask ) ) &&
						ptrTask->m_identifier == wID &&
						//Cast the type to a WORD to cut off the MAIL vs File portion of the type
						(WORD)snapIn->GetType() == wGroupID )
					{
						//And select the item
						SelectItem( childItem );

						bFound = TRUE;
					}

					delete snapIn;
				}
			}
			catch (std::bad_alloc &){}
		}

		//Otherwise, get the next item
		childItem = GetNextSiblingItem( childItem );
	}


}

//----------------------------------------------------------------
// SelectTask
//----------------------------------------------------------------
void CTaskpad::SelectTask( DWORD dwID, CString strCategory )
{
	BOOL		bFound = FALSE;
	HTREEITEM	item,
				childItem;
	CSubTask	*ptrTask;

	//See if we are to select the REcording task
	if( dwID == (DWORD)-1 )
	{
		dwID = m_recordingTaskID;
	}

	//All groups are children of the Root item
	item = GetChildItem( GetRootItem() );

	//Make sure the root item is expanded
	Expand( GetRootItem(), TVE_EXPAND );

	//Walk through the items, looking for
	//	the passed ID
	while( item != NULL && !bFound )
	{
		//First, find the group
		if( strCategory == GetItemText( item ) )
		{
			childItem = GetChildItem( item );

			while( childItem != NULL && !bFound )
			{
				TV_ITEM	tvItem;

				tvItem.hItem = childItem;
				tvItem.mask = TVIF_HANDLE;

				GetItem( &tvItem );

				ptrTask = (CSubTask*)tvItem.lParam;
				if( ptrTask &&
					ptrTask->IsKindOf( RUNTIME_CLASS( CSubTask ) ) &&
					ptrTask->m_identifier == dwID )
				{
					//Make sure the parent is expanded
					Expand( item, TVE_EXPAND );
					//And select the item
					SelectItem( childItem );

					bFound = TRUE;
				}

				//Otherwise, get the next item
				childItem = GetNextSiblingItem( childItem );
			}
		}

		//Otherwise, get the next item
		item = GetNextSiblingItem( item );
	}
}

//----------------------------------------------------------------
// CreateGroupWithMenu
//----------------------------------------------------------------
CTaskGroup *CTaskpad::CreateGroupWithMenu( UINT iID, int iImageNum )
{
	CString		strTemp, sMenu, sTaskpad;
	CMenu		*mainMenu;
	CTaskGroup	*ptrGroup;


	//First, add the View group
	strTemp.LoadString( iID );

	// 8-23-00 terrym the taskpad strings and the menu strings need to be different now due to translation issues.
	// the two strings are combine into one resource.  The first part is the menu string.  The second string is the
	// taskpad string.  The strings are seperated by a token '----'
	// this function will parse out the raw resource string into the menu and taskpad components.
	ConvertRawMenuString(strTemp,sMenu,sTaskpad);


	//If I can't create it, return - it's already there
	if( !(ptrGroup = CreateGroup( sTaskpad, FALSE, iImageNum ) ) )
		return ptrGroup;

	//Create a Menu group in the MainFrame to match
	//	the TaskPad group
	mainMenu = AfxGetMainWnd()->GetMenu();
	if( mainMenu )
	{
		int		iMenuCount = mainMenu->GetMenuItemCount();
		CMenu	menuNewGroup;

		menuNewGroup.CreateMenu();
		mainMenu->InsertMenu( iMenuCount-1, MF_BYPOSITION | MF_POPUP, (UINT)menuNewGroup.m_hMenu, sMenu );
		menuNewGroup.Detach();
	}

	return ptrGroup;
}

//----------------------------------------------------------------
// GetExternalTasks
//----------------------------------------------------------------
void CTaskpad::GetExternalTasks()
{
	CTaskView			*ptrView;
	CSplitter			*splitter = ((CMainFrame*)GetParentFrame())->m_wndSplitter;
	HKEY				pKey;
	CString				strTaskOCXKey;
	TCHAR				szGUID[40];
	DWORD				dwIndex = 0,
						dwSize;
	GUID				guid;

	//get the configuration snap-ins
	GetConfigurationSnapIns();

	//Now, get the key that holds all registered OCX's
	strTaskOCXKey.Format( "%s\\%s", szReg_Key_Main, szReg_Key_GUIDS );

	dwSize = 40;
	//Try to open the key
	if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
										strTaskOCXKey,
										NULL,
										KEY_READ,
										&pKey ) )
	{
		//Find all GUIDS for my registered OCX's
		while( ERROR_NO_MORE_ITEMS != RegEnumValue( pKey,
													dwIndex,
													szGUID,
													&dwSize,
													NULL,
													NULL,
													NULL,
													NULL ) )

		{
			//Turn the GUID from a string form into a GUID form
			if( RPC_S_OK == UuidFromString( (unsigned char *)szGUID, &guid ) )
			{

				//Set it into the App
				((CWProtect32App*)AfxGetApp())->m_ocxGUID = guid;

				//Activate the TaskView - it will fire up the OCX specified
				//	 by the GUID in the main app.
				splitter->ReplaceView( 0, 1, RUNTIME_CLASS(CTaskView), CSize(0,0));
				ptrView = (CTaskView*)splitter->GetPane( 0, 1 );
				ptrView->SetTaskPadPtr( this );
				//Get the tasks
				ptrView->Initialize();
				//Re-activate the default view
				splitter->ReplaceView( 0, 1, RUNTIME_CLASS(CDefaultView), CSize(0,0));
			}

			dwIndex++;
			dwSize = 40;
		}

		//Close the key
		RegCloseKey( pKey );
	}

	ExpandScanningTasks();

	//Re-activate the default view
	splitter->ReplaceView( 0, 1, RUNTIME_CLASS(CDefaultView), CSize(0,0));
}


//----------------------------------------------------------------
// InitGroups
//----------------------------------------------------------------
void CTaskpad::InitGroups()
{
	CTaskGroup  *ptrGroup;

	//I need to make sure that these three groups exists FIRST.
	//	IF they have no tasks, I will verify that later and remove them.
	//	I do this so I can be assured of the order of these three
	//	groups.

	CreateGroupWithMenu( IDS_GROUP_VIEW, 4 );
	if( ptrGroup = CreateGroupWithMenu( IDS_GROUP_SCAN, 6 ) )
		ptrGroup->m_bScanTaskGroup = TRUE;

    // If we have more rights than a normal user then
    // create the configure group.
    if ( m_bCanUpdateRegKey )
    {
	    CreateGroupWithMenu( IDS_GROUP_CONFIGURE, 3 );
    }
}

//----------------------------------------------------------------
// VerifyGroups
//----------------------------------------------------------------
void CTaskpad::VerifyGroups()
{
	CMenu		*mainMenu;
	CTaskGroup	*ptrGroup;
	int			iIndex,
				iMenuIndex = 2;

	//Remove each of the three special groups that are still empty.
	mainMenu = AfxGetMainWnd()->GetMenu();

	//Now, if the View category doesn't have any items, remove it
	//	( the View component may not be installed )
	//	I treat this as a special case because I want to force the
	//		view menu item to be next to Edit IFF it exists.

	HTREEITEM item = GetChildItem( GetRootItem() ),
			  itemRemove;


	//Check the first three groups
	for( iIndex = 0; iIndex < 3; iIndex++ )
	{
		if( GetChildItem( item ) )
		{
			iMenuIndex++;
			item = GetNextSiblingItem( item );
		}
		else
		{
			if( ptrGroup = (CTaskGroup*)GetItemData( item ) )
				delete ptrGroup;

			//Get the next item for iteration now so I can delete
			//	the one I am looking at
			itemRemove = item;
			item = GetNextSiblingItem( item );

			DeleteItem( itemRemove );
			//I also need to remove the Menu as well
			if( mainMenu )
			{
				mainMenu->DeleteMenu( iMenuIndex, MF_BYPOSITION );
			}
		}
	}


	if( mainMenu && !m_bScanningComponentInstalled )
	{
		//If we don't have any scanning component, then I need to
		//	remove the Edit menu as well
		mainMenu->DeleteMenu( 1, MF_BYPOSITION );
	}
}

//----------------------------------------------------------------
// ExpandScanningTasks
//----------------------------------------------------------------
void CTaskpad::ExpandScanningTasks()
{
	TV_ITEM		tvItem;
	CTaskGroup	*ptrGroup;

	//Expand the scanning tasks group(s)
	HTREEITEM item = GetChildItem( GetRootItem() );
	//Find the Scanning tasks group
	while( item != NULL )
	{
		tvItem.hItem = item;
		tvItem.mask = TVIF_HANDLE;

		GetItem( &tvItem );
		ptrGroup = (CTaskGroup*)tvItem.lParam;

		if( ptrGroup )
		{
			if( ptrGroup->m_bScanTaskGroup )
			{
				m_bScanningComponentInstalled = Expand( item, TVE_EXPAND );
			}
		}

		item = GetNextSiblingItem( item );
	}

}

//----------------------------------------------------------------
// GetConfigurationSnapIns
//----------------------------------------------------------------
void CTaskpad::GetConfigurationSnapIns()
{
	CLDVPSnapIn			snapIn;
	CSubTask			*ptrTask;
	CString				strTaskName,
						strMenuName,
						strGroup,
						strPageType,
						strDescription,
						sGroupMenu,
						sGroupTaskpad;
	DWORD				dwPageMask;
	int					iImageID;

    // If we're a normal user just return. More rights are
    // needed to configure real time scanning.
    if ( !m_bCanUpdateRegKey )
    {
        return;
    }

	strGroup.LoadString( IDS_GROUP_CONFIGURE );

	// 8-23-00 terrym the taskpad strings and the menu strings need to be different now due to translation issues.
	// the two strings are combine into one resource.  The first part is the menu string.  The second string is the
	// taskpad string.  The strings are seperated by a token '----'
	// this function will parse out the raw resource string into the menu and taskpad components.
	ConvertRawMenuString(strGroup,sGroupMenu,sGroupTaskpad);

	//Now, get the configuration snap-ins
	while( snapIn.Enumerate() )
	{
		//Add the appropriate tasks for the pageMask that this component supports
		dwPageMask = 1;
		//Check all bits
		for( int iBit = 0; iBit < 32; iBit++ )
		{
			//Now, add the appropriate task to this group
			if( dwPageMask & snapIn.GetPageMask() )
			{
				//First, get the proper text to append to the task name
				//and also get the appropriate icon
				switch( dwPageMask )
				{
				//We will never display Manual Scan pages in teh task pad!
				case SNAP_IN_PAGE_MANUAL:
					dwPageMask *= 2;
					continue;
				case SNAP_IN_PAGE_REALTIME:
					strPageType.LoadString( IDS_REALTIME_PROTECTION );
					//Now, figure out which Realtime Icon to display
					if( snapIn.GetType() & SNAP_IN_MAIL_ICON )
						iImageID = 18;
					else if( snapIn.GetType() & SNAP_IN_GATEWAY_ICON )
						iImageID = 18;
					else
						iImageID = 28;
					break;
				case SNAP_IN_PAGE_IDLE:
                    // Better yet, don't display it ever. -- MBROWN 6/24/99

 					//Don't display Idle Scan on the server
					//if( GetClientType() == CLIENT_TYPE_SERVER )
						continue;
					/*else
					{
						strPageType.LoadString( IDS_IDLE_PROTECTION );
						iImageID = 10;
					}*/
					break;
				}

                if (snapIn.GetType() & SNAP_IN_CUSTOM_NAME)
                {
                    strTaskName = snapIn.GetDisplayName();
                    strMenuName = snapIn.GetTaskName();
                }
                else
                {
                    strTaskName.Format("%s %s", snapIn.GetDisplayName(), strPageType);
                    strMenuName.Format("%s %s", snapIn.GetTaskName(), strPageType);
                }
				AfxFormatString1(strDescription, IDS_CONFIG_TASK_DESCRIPTION, strTaskName);

				/*----------------------------------------------------------------*/
				// It has been decided that we will not display the mail snap-ins
				//	on a servar machine. This being the case, I need to now check
				//	the type here fo both the client and the snap-in. If we are
				//	running as a server AND the snap-in is NOT file system,
				//	don't allow the snap-in to occur.
				/*----------------------------------------------------------------*/
				if ((GetClientType() == CLIENT_TYPE_SERVER ) &&
					!(snapIn.GetType() & (SNAP_IN_FILE_SYSTEM | SNAP_IN_SYMPROTECT)))
					continue;

                if (snapIn.GetType() & SNAP_IN_CUSTOM_ICON)
                    ptrTask = CreateTask(sGroupTaskpad, strTaskName, strDescription, dwPageMask, snapIn.GetGuid(), snapIn.GetIconFilename(), snapIn.GetIconResourceID(), TASK_INTERNAL);
                else
                    ptrTask = CreateTask(sGroupTaskpad, strTaskName, strDescription, dwPageMask, snapIn.GetGuid(), iImageID, TASK_INTERNAL);
                if (ptrTask != NULL)
				{
					//All I need additionally is the Internal Name
					ptrTask->m_strInternalName = snapIn.GetStorage();

					//and add the task to the proper menu
					AddTaskToMenu( this, sGroupMenu, MAKELONG( (WORD)dwPageMask, (WORD)(TASK_GROUP_CONFIG_SNAP_IN |  snapIn.GetType() ) ), strMenuName );
				}

			}

			//Increment my pagemask for the next type
			dwPageMask *= 2;
		}
	}
}

//----------------------------------------------------------------
// LoadTaskGroup
//----------------------------------------------------------------
void CTaskpad::LoadTaskGroup( CString strGroupName, CString strRegName )
{
	HKEY		pKey;
	CString		strKeyName,
				strFilename,
				strDescription;
	DWORD		dwIndex = 0,
				dwSize;
	FILETIME	tm;
	CStringList listKeys;
	CString		strTaskname;
	POSITION	pos;
	TCHAR		szTaskname[124];
	int			iIndex;
	GUID		guid;

	dwSize = 124;

	while( -1 != (iIndex = strRegName.Find( "\\" )) )
	{
		strRegName.SetAt( iIndex, '~' );
	}

	//Generate the proper key to hold the values
	strKeyName.Format( "%s\\%s\\%s", szReg_Key_Main, szReg_Key_CustomTasks, strRegName );

	//Enumerate all keys under this group
	if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_CURRENT_USER,
											strKeyName,
											0,
											KEY_READ,
											&pKey) )
	{
		//Enumerate through the keys adding all under it
		while( ERROR_SUCCESS == RegEnumKeyEx(
												pKey,
												dwIndex,
												szTaskname,
												&dwSize,
												NULL,
												NULL,
												NULL,
												&tm ) )

		{
			dwIndex++;
			dwSize = 124;

			//Now, get the rest of the data from the reg
			listKeys.AddTail( CString( szTaskname ) );

		} // End while enumerating

		RegCloseKey( pKey );

		//Now that I have all of the keys, I need to open each one and
		// get the info from them

		pos = listKeys.GetHeadPosition();

		while( pos != NULL )
		{
			//Get the item at pos
			strTaskname = listKeys.GetNext( pos );

			//Replace the ~ with forward slashes
			while( -1 != (iIndex = strTaskname.Find( _T('~') )) )
			{
				strTaskname.SetAt( iIndex, _T('\\') );
			}

			GetCustomTaskInfo( strGroupName, strTaskname, strDescription, strFilename, guid );

			CreateTask( strGroupName, (const char *)strTaskname, strDescription, ID_TASK_CUSTOM, guid );

		} //End while pos

	} // End IF opened key

} //End method LoadTaskGroup


//----------------------------------------------------------------
// LoadTaskGroup
//----------------------------------------------------------------
void CTaskpad::LoadCustomTask( CSubTask *ptrTask )
{
	CString		strTaskID,
				strDescription;

	//Get the filename
	GetCustomTaskInfo( ptrTask->m_strGroup, ptrTask->m_text, strDescription, strTaskID, ptrTask->m_guid );

}


//----------------------------------------------------------------
// GetCustomTaskInfo
//----------------------------------------------------------------
void CTaskpad::GetCustomTaskInfo( CString strACategory, CString strTaskname, CString &strDescription, CString &strTaskKey, GUID &guid, BOOL bDeleteAfterGet /* = FALSE */ )
{
	DWORD		dwSize = MAX_PATH;
	BYTE		*szTaskKey,
				*szTaskGUID,
				*szDescript;
	HKEY		pKey;
	int			iIndex;
	CString		strCategory,
				strTemp;

	strCategory.Empty();

	//First, figure out if this category is one of the
	//	categories I know about ( it should ALWAYS be)
	strTemp.LoadString( IDS_DEFTASK_STARTUP );
	if( strACategory == strTemp )
	{
		strCategory = szReg_Value_Startup;
		goto gotacategory;
	}

	strTemp.LoadString( IDS_GROUP_SCHEDULED_TASKS );
	if( strACategory == strTemp )
	{
		strCategory = szReg_Value_Scheduled;
		goto gotacategory;
	}

	strTemp.LoadString( IDS_DEFTASK_SCRIPTED );
	if( strACategory == strTemp )
	{
		strCategory = szReg_Value_Scripted;
		goto gotacategory;
	}

	//If I got here, then I didn't find it, and I need
	//	to set it to what was sent in.
	strCategory = strACategory;

gotacategory:

	CString strKeyName;

	while( -1 != (iIndex = strTaskname.Find( _T('\\') )) )
	{
		strTaskname.SetAt( iIndex, _T('~') );
	}

	while( -1 != (iIndex = strCategory.Find( _T('\\') )) )
	{
		strCategory.SetAt( iIndex, _T('~') );
	}

	//Generate the proper key to hold the values
	strKeyName.Format( "%s\\%s\\%s\\%s", szReg_Key_Main, szReg_Key_CustomTasks, strCategory, strTaskname );

	if( ERROR_SUCCESS == RegOpenKeyEx(
										HKEY_CURRENT_USER,
										strKeyName,
										0,
										KEY_READ,
										&pKey ) )
	{
		dwSize = MAX_PATH;
		szTaskKey = (BYTE*)strTaskKey.GetBuffer( dwSize );
		//Get the CustomTaskKey
		SymSaferRegQueryValueEx(	pKey,
							//szReg_Val_CustomTaskKey,
							NULL,
							NULL,
							NULL,
							szTaskKey,
							&dwSize );

		strTaskKey.ReleaseBuffer();


		//HACK ALERT!
		// In NT3.51, I am getting GUIDS back with]
		// a - at the end. To solve this, I am going to
		//	going to chop off any extra characters over 36
		if( strTaskKey.GetLength() > 36 )
			strTaskKey = strTaskKey.Left( 36 );



		dwSize = 1024;
		szDescript = (BYTE*)strDescription.GetBuffer( dwSize );
		//and get the description
		SymSaferRegQueryValueEx (
							pKey,
							szReg_Val_Description,
							0,
							NULL,
							(BYTE*)szDescript,
							&dwSize
						);

		strDescription.ReleaseBuffer();

		dwSize = 40;
		szTaskGUID = (unsigned char*)LocalAlloc( LPTR, 40 );
		//and get the GUID
		SymSaferRegQueryValueEx (
							pKey,
							szReg_Val_OCXGUID,
							0,
							NULL,
							(BYTE*)szTaskGUID,
							&dwSize
						);

		UuidFromString( (unsigned char*)szTaskGUID, &guid );
		LocalFree( szTaskGUID );

		RegCloseKey( pKey );

	}

	//If we are supposed to delete after we get the data, then
	// delete the key now
	if( bDeleteAfterGet )
	{
		RegDeleteKey( HKEY_CURRENT_USER, strKeyName );
	}
}


//----------------------------------------------------------------
// GetCustomTaskInfo
//----------------------------------------------------------------
BOOL CTaskpad::SaveCustomTaskInfo( CString strACategory, CString &strTaskname, CString strDescription, CString strTaskKey, GUID guid )
{
	HKEY			pKey;
	DWORD			dwDisp;
	CString			strKeyName;
	int				iIndex;
	CString			strCategory,
					strTaskTitle,
					strTemp;
	LPTSTR			szGuid			= NULL;

	strCategory.Empty();

	//First, figure out if this category is one of the
	//	categories I know about ( it should ALWAYS be)
	strTemp.LoadString( IDS_DEFTASK_STARTUP );
	if( strACategory == strTemp )
	{
		strCategory = szReg_Value_Startup;
		goto gotacategory;
	}

	strTemp.LoadString( IDS_GROUP_SCHEDULED_TASKS );
	if( strACategory == strTemp )
	{
		strCategory = szReg_Value_Scheduled;
		goto gotacategory;
	}

	strTemp.LoadString( IDS_DEFTASK_SCRIPTED );
	if( strACategory == strTemp )
	{
		strCategory = szReg_Value_Scripted;
		goto gotacategory;
	}

	//If I got here, then I didn't find it, and I need
	//	to set it to what was sent in.
	strCategory = strACategory;

gotacategory:

	//Add it to the registry under the proper
	// heading.
	//Generate the proper key to hold the values

	//Strip out all the forward slashes
	while( -1 != ( iIndex = strTaskname.Find( _T('\\') ) ) )
	{
		strTaskname.SetAt( iIndex, _T('~') );
	}

	while( -1 != (iIndex = strCategory.Find( _T('\\') )) )
	{
		strCategory.SetAt( iIndex, _T('~') );
	}

	//Format the key name of the GUID
	strKeyName.Format( "%s\\%s\\%s", szReg_Key_Main, szReg_Key_CustomTasks, strTaskKey );

	//First, save out the Taskname under the options so it will display
	//	in the progress dialog.
	strTemp.LoadString( IDS_LDVP_TITLE );
	strTaskTitle.Format( "%s - %s", strTemp, strTaskname );
	if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_CURRENT_USER,
										strKeyName,
										0,
										KEY_READ | KEY_WRITE | KEY_CREATE_SUB_KEY,
										&pKey) )
	{
		RegSetValueEx(	pKey,
						szReg_Value_ScanTitle,
						0,
						REG_SZ,
						(BYTE*)(LPCTSTR)strTaskTitle,
						(DWORD)strTaskTitle.GetLength() + 1 );

	}


	int iCounter = 1;
	CString strOldTaskname = strTaskname;

tryagain:
	//Now format the key name of the task itself
	strKeyName.Format( "%s\\%s\\%s\\%s", szReg_Key_Main, szReg_Key_CustomTasks, strCategory, strTaskname );

	//Read the Registry and get the directory
	if( ERROR_SUCCESS == RegCreateKeyEx(	HKEY_CURRENT_USER,
											strKeyName,
											0,
											"",
											REG_OPTION_NON_VOLATILE,
											KEY_READ | KEY_WRITE | KEY_CREATE_SUB_KEY,
											NULL,
											&pKey,
											&dwDisp) )
	{

		if( dwDisp == REG_OPENED_EXISTING_KEY )
		{
			//Close the key
			RegCloseKey( pKey );
			strTemp.Format( "%s_%d", strOldTaskname, iCounter++ );
			strTaskname = strTemp;
			goto tryagain;
		}

		if( iCounter > 1 )
		{
			//Inform the user the task was renamed
			CString strError;
			AfxFormatString2( strError, IDS_ERROR_TASK_EXISTS, strOldTaskname, strTaskname );
			AfxMessageBox( strError );
		}


		//If the key opened, Set the value for the filename
		RegSetValueEx(	pKey,
						//szReg_Val_CustomTaskKey,
						NULL,
						0,
						REG_SZ,
						(BYTE*)(LPCTSTR)strTaskKey,
						(DWORD)strTaskKey.GetLength() + 1);

		RegSetValueEx(	pKey,
						szReg_Val_Description,
						0,
						REG_SZ,
						(BYTE*)(LPCTSTR)strDescription,
						(DWORD)strDescription.GetLength() + 1);

		//Convert the GUID into a string and store it.  UuidToString prototype is weird.
#ifndef UNICODE
        UuidToString(  &guid, (unsigned char**) &szGuid );
#else
        UuidToString(  &guid, (unsigned short**) &szGuid );
#endif
		if (szGuid != NULL)
		{
			RegSetValueEx(	pKey,
							szReg_Val_OCXGUID,
							0,
							REG_SZ,
							(BYTE*)szGuid,
							_tcslen(szGuid) + 1);
#ifndef UNICODE
			RpcStringFree((unsigned char**) &szGuid);
#else
			RpcStringFree((unsigned short**) &szGuid);
#endif
			szGuid = NULL;
		}
		RegCloseKey( pKey );
	}

	return TRUE;
}

//----------------------------------------------------------------
// HandleInternalTask
//----------------------------------------------------------------
void CTaskpad::HandleInternalTask( CSubTask *ptrTask, BOOL bDoubleClick )
{
	CMainFrame		*ptrMainFrame = (CMainFrame*)AfxGetMainWnd();
	CWProtect32Doc	*ptrDoc = (CWProtect32Doc*)ptrMainFrame->GetActiveDocument();
	CTaskView		*ptrView = (CTaskView*)ptrMainFrame->m_wndSplitter->GetPane( 0, 1 );
	BOOL			bIsRecordTask = ( ptrTask->m_identifier == ID_RECORD_NEW ) ||
									( ptrTask->m_identifier == ID_NEW_SCHEDULED ) ||
									( ptrTask->m_identifier == ID_RECORD_STARTUP );
	CSplitter		*splitter = ((CMainFrame*)GetParentFrame())->m_wndSplitter;

	if( ptrTask && ptrTask->m_dwTaskType == TASK_INTERNAL )
	{
		//First, check the GUID. If the App's ptrTask and this
		//	task are different, then I need to switch
		//	views first. This is because ther SwitchView method
		//	won't switch the view if the view being switched to is
		//	already active - the problem is that the view is the
		//	same, but the OCX that it creates is different.
		if( ((CWProtect32App*)AfxGetApp())->m_ptrTask != ptrTask )
			splitter->ReplaceView( 0, 1, RUNTIME_CLASS(CDefaultView), CSize(0,0));

		//Fire the task by swapping to the CConfigureSnapin view
		//Start be setting the GUID into the App
		((CWProtect32App*)AfxGetApp())->m_ptrTask = ptrTask;
		((CMainFrame*)AfxGetApp()->m_pMainWnd)->SwitchToView( RUNTIME_CLASS( CConfigureSnapIn ) );
	}

	//If this is the Help contents, then show the normal Help contents
	if( ptrTask->m_identifier == IDS_HELP_CONTENTS )
	{
		// Set the right pane so that it displays the help group
		HTREEITEM hItem = ptrTask->m_pTaskpad->GetSelectedItem();
		HTREEITEM hParent = GetParentItem( hItem );
		TV_ITEM tvParent;
		CSubTask* pParentTask;

		ZeroMemory( &tvParent, sizeof( TV_ITEM ) );

		tvParent.mask = TVIF_HANDLE | TVIF_PARAM;
		tvParent.hItem = hParent;

		GetItem( &tvParent );
		pParentTask = (CSubTask*)tvParent.lParam;

		// Make sure that the parent is a group
		if( pParentTask->IsKindOf( RUNTIME_CLASS( CTaskGroup ) ) )
			ShowChildren( hParent, TRUE );

		// Start help
        if ( !m_gHelpFileName.IsEmpty() )
        {
			::HtmlHelp( AfxGetMainWnd()->GetSafeHwnd(), m_gHelpFileName, HH_DISPLAY_TOPIC, NULL );
        }

	}
// We no longer use the virus encyclopedia
//		else
//		{
//			::WinHelp( AfxGetMainWnd()->m_hWnd, m_gEncyclopediaName, HELP_CONTEXT, ptrTask->m_identifier );
//		}
//	}
}

//----------------------------------------------------------------
// FireTask
//----------------------------------------------------------------
void CTaskpad::FireTask( BOOL bDoubleClick )
{
	TV_ITEM		tvItem;
	CSubTask	*ptrTask;
	CSplitter	*splitter = ((CMainFrame*)GetParentFrame())->m_wndSplitter;
	CTaskView	*ptrView;
	CString		strTaskName,
				strTaskDescriptiveName,
				strDescription;

	memset( &tvItem, '\0', sizeof( TV_ITEM ) );
	tvItem.mask = TVIF_HANDLE | TVIF_PARAM;
	tvItem.hItem = GetSelectedItem();

	GetItem( &tvItem );

	//If the item clicked is a Task, then start it.
	ptrTask = (CSubTask*)tvItem.lParam;

	//I can tell if it is a task my getting the ItemData
	if( !ptrTask->IsKindOf( RUNTIME_CLASS( CSubTask ) ) )
	{
		//Not a Subtask
		return;
	}

	//OK, it's a Subtask.
	if( (ptrTask->m_dwTaskType != TASK_EXTERNAL) &&
		(ptrTask->m_dwTaskType != TASK_RECORDABLE ) )
	{
		HandleInternalTask( ptrTask, bDoubleClick );
		//Invalidate the right pane
		AfxGetMainWnd()->Invalidate();
	}
	else
	{
		//If this is a custom task, then I want to load the
		// custom task in from the disk before I
		if( ptrTask->m_identifier == ID_TASK_CUSTOM )
		{
			//Get the filename
			GetCustomTaskInfo( ptrTask->m_strGroup, ptrTask->m_text, strDescription, strTaskName, ptrTask->m_guid );

			//Now, set the descriptive name and description
			//	so the OCX can display it
			strTaskDescriptiveName = ptrTask->m_text;
			strDescription = ptrTask->m_description;

			//Also, check the GUID. If it is the old OCX's GUID, then
			//	I need to replace it with the new one. For custom tasks,
			//	this provides backward compatibility.
			if( ptrTask->m_guid == TASK_GUID_VPC50)
				ptrTask->m_guid = TASK_GUID_VPC51;

		}
		//Otherwise, if it is a recording task, then I need to inform
		//	my document class of the type of recording session
		else if( ptrTask->m_identifier == m_recordingTaskID )
		{
			//Set the record type into the document
			CWProtect32Doc *pDoc = (CWProtect32Doc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

			pDoc->m_fRecording = ptrTask->m_recordType;

			//Now, set the descriptive name and description
			//	so the OCX can display it
			strTaskDescriptiveName = ptrTask->m_text;
		}
		else
		{
			strTaskName = "";
		}

		//First, check the GUID. If the App's GUID and this
		//	tasks GUID are different, then I need to switch
		//	views first. This is because ther SwitchView method
		//	won't switch the view if the view being switched to is
		//	already active - the problem is that the view is the
		//	same, but the OCX that it creates is different.
		if( ((CWProtect32App*)AfxGetApp())->m_ocxGUID != ptrTask->m_guid )
			splitter->ReplaceView( 0, 1, RUNTIME_CLASS(CDefaultView), CSize(0,0));

		// set the GUID of the task into the app so when the
		//	view creates the OCX, it will use the GUID from
		//	the app to do so.
		((CWProtect32App*)AfxGetApp())->m_ocxGUID = ptrTask->m_guid;

		//Switch views so I can display the Task property sheet
		splitter->ReplaceView( 0, 1, RUNTIME_CLASS(CTaskView), CSize(0,0), FALSE );
		ptrView = (CTaskView*)splitter->GetPane( 0, 1 );
		if (ptrView )
		{
			ptrView->m_bAlreadySetFocus = FALSE;
			ptrView->SetTaskPadPtr( this );

			//Set the Current task title
			((CWProtect32App*)AfxGetApp())->m_strRunningTaskTitle.Format( "%s\n%s", ptrTask->m_text, ptrTask->m_description );

			//Now, start it
			ptrView->StartTask( ptrTask->m_identifier, strTaskName, strTaskDescriptiveName, strDescription, bDoubleClick );
		}
	}
}

//----------------------------------------------------------------
// FillLBScanTasks
//----------------------------------------------------------------
void CTaskpad::FillLBScanTasks( CListBox *ptrList )
{
	HTREEITEM	item,
				taskItem;
	TV_ITEM		tvItem,
				tvTask;
	CTaskGroup	*ptrGroup;
	CSubTask	*ptrTask;
	int			iIndex;

	item = GetChildItem( GetRootItem() );
	//Find the Scanning tasks group
	while( item != NULL )
	{

		tvItem.hItem = item;
		tvItem.mask = TVIF_HANDLE;

		GetItem( &tvItem );
		ptrGroup = (CTaskGroup*)tvItem.lParam;

		if( ptrGroup )
		{
			if( ptrGroup->m_bScanTaskGroup )
			{
				//Load all the children into the Listbox
				taskItem = GetChildItem( item );

				while( taskItem != NULL )
				{
					//Get the text of the subtask
					tvTask.hItem = taskItem;
					tvTask.mask = TVIF_HANDLE;

					GetItem( &tvTask );
					ptrTask = (CSubTask*)tvTask.lParam;

					if( ptrTask )
					{
						CString strItem = ptrTask->m_text;

						//I don't want any underscores
						//	so remove them all now
						RemoveChars( strItem, _T("&" ) );

						// and place it in the Listbox
						if( -1 != (iIndex = ptrList->AddString( strItem ) ) )
						{
							// finally, add the HTREEITEM of the child as the
							//	extra data
							ptrList->SetItemData( iIndex, (DWORD)taskItem );

						} // End if

					} //End if

					//Get the next sibling
					taskItem = GetNextSiblingItem( taskItem );
				} //End while

			} //End if

		} // End if

		//Otherwise, get the next item
		item = GetNextSiblingItem( item );

	} //End while

}

//----------------------------------------------------------------
// RemoveScheduleInfo
//----------------------------------------------------------------
void CTaskpad::RemoveScheduleInfo( const CString &strFilename )
{
	CString strKey;

	strKey.Format( "%s\\%s\\%s\\%s", szReg_Key_Main, szReg_Key_CustomTasks, strFilename, szReg_Key_ScheduleKey );

	RegDeleteKey( HKEY_CURRENT_USER, strKey );
}

//----------------------------------------------------------------
// GetScheduleInfo
//----------------------------------------------------------------
BOOL CTaskpad::GetScheduleInfo( CString &strTaskName, CString &strDescription, CString strTaskGUID, GUID ocxGUID )
{
	BOOL				bReturn = FALSE;
	CLDVPTaskFS			*ptrOCX;
	RECT				rect;

	memset( &rect, '\0', sizeof (RECT ) );

	try
	{
		//create a instance of the OCX to use
		if( ptrOCX = new CLDVPTaskFS )
		{
			//Look for the default OCX.
			ptrOCX->SetCLSID( ocxGUID );

			if ( ptrOCX->Create( NULL, _T( "" ), WS_CHILD | WS_VISIBLE, rect, this,
					5099 ) )
			{
	//			ptrOCX->SetHelpFilename( "vpc32.hlp" );

				//Now, call the DoModal
				if( ptrOCX->DoModalSchedule( strTaskGUID, 1 ) )
				{

					//Allow them to change the name and description
	//				dlg.m_scriptedTaskName = strTaskName;
	//				dlg.m_taskDescription = strDescription;
	//				dlg.DoModal();
	//				strTaskName = dlg.m_scriptedTaskName;
	//				strDescription = dlg.m_taskDescription;

					//and return
					bReturn = TRUE;

				}

			}

		ptrOCX->DestroyWindow();
		delete ptrOCX;
		}
	}
	catch (...) {}

	return bReturn;
}


void CTaskpad::OnLButtonDown(UINT nFlags, CPoint point)
{
	HTREEITEM item= HitTest( point, &nFlags );

	if( item != GetSelectedItem() && CheckRecording() )
		return;

	//KLUDGE ALERT!!!
	//	I can't get this to select the proper item,
	//	so instead of simply passing on the message, or
	//	re-sending the message, I will actually
	//	figure out which item the mouse was over
	//	and hard-select it.
	//ScreenToClient( &point );
	SelectItem( item );

	CTreeCtrl::OnLButtonDown(nFlags, point);
}

void CTaskpad::GetSelTask( CString &strTaskname, CString &strGroup )
{
	HTREEITEM			item;
	TV_ITEM				tvItem;

	//Get the currently selected task and it's group to
	//	return.
	memset( &item, '\0', sizeof( HTREEITEM ) );
	memset( &tvItem, '\0', sizeof( TV_ITEM ) );

	tvItem.hItem = GetSelectedItem();
	tvItem.mask = TVIF_TEXT;
	tvItem.pszText = strTaskname.GetBuffer( MAX_PATH );
	tvItem.cchTextMax = MAX_PATH;

	//This should fill in the Taskname
	GetItem( &tvItem );

	tvItem.hItem = GetParentItem(tvItem.hItem);
	tvItem.mask = TVIF_TEXT;
	tvItem.pszText = strGroup.GetBuffer( MAX_PATH );
	tvItem.cchTextMax = MAX_PATH;

	//This should fill in the Group
	GetItem( &tvItem );

	//Release my buffers and return
	strGroup.ReleaseBuffer();
	strTaskname.ReleaseBuffer();
}

void CTaskpad::ModifyCurrentTask( CString strTask, CString strDescription, BOOL bFireTask /* = TRUE */ )
{
	CSubTask *ptrTask;

	ptrTask = (CSubTask*)GetItemData( GetSelectedItem() );

	ptrTask->m_text = strTask;
	ptrTask->m_description = strDescription;

	//Redraw the taskpad
	Invalidate();
	UpdateWindow();

	if( bFireTask )
		//Fire the task to update the OCX window
		FireTask(FALSE);
}

void CTaskpad::OnRButtonDown(UINT nFlags, CPoint point)
{
	CWnd		*ptrMainWnd = AfxGetMainWnd();
	CMenu		menuMain,
				*menuPopup;
	HTREEITEM	item= HitTest( point, &nFlags );

	if( item != GetSelectedItem() && CheckRecording() )
		return;

	CTaskpadExt::OnRButtonDown(nFlags, point);

	//When acting as a server, there is no right-click functionality
	if( GetClientType() == CLIENT_TYPE_SERVER )
		return;

	//The right-click should select the item under the mouse so
	//	it will be available for the context-menu.
	SelectItem( item );


	if( ptrMainWnd )
	{
		menuMain.LoadMenu(IDM_POPUP);
		menuPopup = menuMain.GetSubMenu(0);
		ClientToScreen( &point );

		menuPopup->TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON,
									point.x,
									point.y,
									ptrMainWnd,
									NULL );
	}

}

LRESULT CTaskpad::SelectTaskByPoint( WPARAM wParam, LPARAM lParam )
{
	//Walk through my list and select the task that is
	//	drawn in the specified rect
	CPoint			pt;
	CString			strCategory;
	HTREEITEM		item;
	BOOL			bFound = FALSE;
	CSubTask		*ptrTask;

	//Look at all children of the selected item
	if( ( item = GetChildItem( GetSelectedItem() ) ) == NULL )
		//Or, get the FIRST child item that is a sibling of the selected item
		item = GetChildItem( GetParentItem( GetSelectedItem() ) );


	//Make sure the catagory is expanded
	Expand( GetSelectedItem(), TVE_EXPAND );

	//Get the point from the wParam
	pt.x = LOWORD( wParam );
	pt.y = HIWORD( wParam );

	//Walk through the items, looking for
	//	the passed ID
	while( item != NULL && !bFound )
	{
		TV_ITEM	tvItem;

		tvItem.hItem = item;
		tvItem.mask = TVIF_HANDLE | TVIF_PARAM;

		GetItem( &tvItem );

		ptrTask = (CSubTask*)tvItem.lParam;
		if( ptrTask &&
			ptrTask->IsKindOf( RUNTIME_CLASS( CSubTask ) ) &&
			ptrTask->m_textRect.PtInRect( pt ) )
		{
			//And select the item
			SelectItem( item );

			//If the selected item is a Help task, then I need to start it
			if( ptrTask->m_dwTaskType == TASK_HELP )
				FireTask( TRUE );

			bFound = TRUE;
		}

		//Otherwise, get the next item
		item = GetNextSiblingItem( item );
	}

	return -1;
}

//////////////////////////////////////////////////////////////////
//
// Function: StartSelectedTask
//
// Description: Called to fire a task if the user selects an item
// that is already selected.  We need to manually call this
// if this is true b/c selecting an already selected tree item
// isn't caught by OnSelChanged() which normally fires the task.
//
// Params:
//         [IN] HTREEITEM
// Returns:
//         TRUE if successful, FALSE otherwise.
//
//////////////////////////////////////////////////////////////////
// 08.25.00 MHOTTA :: Function Created.
//////////////////////////////////////////////////////////////////

BOOL CTaskpad::StartSelectedTask( HTREEITEM item )
{
	TV_ITEM			tvi;
	CTaskGroup		*ptrTask;
	CSplitter		*splitter = ((CMainFrame*)GetParentFrame())->m_wndSplitter;
	BOOL			bRet = FALSE;

	ZeroMemory( &tvi, sizeof(TV_ITEM) );

	tvi.hItem = item;
	tvi.mask = TVIF_HANDLE;

	if( NULL == GetItem( &tvi ) )
		{
		// Couldn't get a valid item.
		goto bailout;
		}

	//If the item clicked is a Task, then start it.
	ptrTask = (CTaskGroup*)tvi.lParam;

	if( ptrTask == NULL || tvi.hItem == GetRootItem() )
		{
		((CMainFrame*)AfxGetApp()->m_pMainWnd)->SwitchToView( RUNTIME_CLASS( CDefaultView ) );
		goto bailout;
		}

	if( ptrTask->IsKindOf( RUNTIME_CLASS( CTaskGroup ) ) )
		{
		HTREEITEM hTempItem = GetNextVisibleItem( tvi.hItem );

		ShowChildren( tvi.hItem, TRUE );
		}
	else
		{
		//If it's not a taskgroup, then it's a subtask.
		FireTask( FALSE );
		bRet = TRUE;
		}

bailout:
	AfxGetMainWnd()->PostMessage( TASK_FOCUS, 0, 0L );
	return( bRet );
}
