// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: TaskpadExt.cpp
//  Purpose: CTaskPadExt Implementation file
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
#include "TaskpadExt.h"
#include "MscFuncs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//----------------------------------------------------------------
//
// CTaskpadObject class
//
//----------------------------------------------------------------
IMPLEMENT_DYNAMIC( CTaskpadObject, CObject );

//----------------------------------------------------------------
// Constructor
//----------------------------------------------------------------
CTaskpadObject::CTaskpadObject()
:	CObject(),
	m_hbitmap( NULL ),
	m_text( _T( "" ) ),
	m_handle( NULL ),
	m_image( 0 ),
    m_pTaskpad (NULL)
{
}

//----------------------------------------------------------------
// Constructor - Overloaded
//----------------------------------------------------------------
CTaskpadObject::CTaskpadObject( const CTaskpadObject& object )
:	CObject(),
	m_hbitmap( NULL ),
	m_text( _T( "" ) ),
	m_handle( NULL ),
	m_image( 0 ),
    m_pTaskpad (NULL)

{
	copy( object );
}

//----------------------------------------------------------------
// Destructor
//----------------------------------------------------------------
CTaskpadObject::~CTaskpadObject()
{
}

//----------------------------------------------------------------
// copy
//----------------------------------------------------------------
void CTaskpadObject::copy( const CTaskpadObject& object )
{
	m_hbitmap  = object.m_hbitmap;
	m_text     = object.m_text;
	m_handle   = object.m_handle;
	m_image    = object.m_image;
	m_pTaskpad = object.m_pTaskpad;
}

//----------------------------------------------------------------
// OPerator = 
//----------------------------------------------------------------
CTaskpadObject& CTaskpadObject::operator=( const CTaskpadObject& object )
{
	copy( object );

	return *this;
}

//----------------------------------------------------------------
//
// CTaskGroup class
//
//----------------------------------------------------------------

IMPLEMENT_DYNAMIC( CTaskGroup, CTaskpadObject );

//----------------------------------------------------------------
// Constructor
//----------------------------------------------------------------
CTaskGroup::CTaskGroup()
:	CTaskpadObject(), bCanDragDropChildren (FALSE), m_bScanTaskGroup(FALSE)
{
}

//----------------------------------------------------------------
// Constructor - Overloaded
//----------------------------------------------------------------
CTaskGroup::CTaskGroup( const CTaskGroup& group )
:	CTaskpadObject(), 
    bCanDragDropChildren (group.bCanDragDropChildren),
    m_bScanTaskGroup(group.m_bScanTaskGroup)
{
}

//----------------------------------------------------------------
// Destructor
//----------------------------------------------------------------
CTaskGroup::~CTaskGroup()
{
}

//----------------------------------------------------------------
// Operator =
//----------------------------------------------------------------
CTaskGroup& CTaskGroup::operator=( const CTaskGroup& group )
{
	CTaskpadObject::operator=( group );
	bCanDragDropChildren = group.bCanDragDropChildren;
	m_bScanTaskGroup = group.m_bScanTaskGroup;
	return *this;
}

//----------------------------------------------------------------
//
// CSubTask class
//
//----------------------------------------------------------------
IMPLEMENT_DYNAMIC( CSubTask, CTaskpadObject );

//----------------------------------------------------------------
// Constructor
//----------------------------------------------------------------
CSubTask::CSubTask()
:	CTaskpadObject(),
	m_identifier( 0 ),
	m_description( CString( _T( "" ) ) )
{
	m_dwTaskType = TASK_EXTERNAL;
	m_image = 3;
	m_recordType = 0;
}

//----------------------------------------------------------------
// Constructor - Overloaded
//----------------------------------------------------------------
CSubTask::CSubTask( const CSubTask& task )
:	CTaskpadObject(),
	m_identifier( 0 ),
	m_description( CString( _T( "" ) ) )
{
	copy( task );
	m_dwTaskType = TASK_EXTERNAL;
	m_image = 3;
	m_recordType = 0;
}

//----------------------------------------------------------------
// Destructor
//----------------------------------------------------------------
CSubTask::~CSubTask()
{
}

//----------------------------------------------------------------
// copy
//----------------------------------------------------------------
void CSubTask::copy( const CSubTask& task )
{
	CTaskpadObject::copy( task );
	m_identifier = task.m_identifier;
	m_description = task.m_description;
	m_strGroup = task.m_strGroup;
	m_dwTaskType = task.m_dwTaskType;
	m_guid = task.m_guid;
	m_recordType = task.m_recordType;
	m_strInternalName = task.m_strInternalName;
}

//----------------------------------------------------------------
// Operator =
//----------------------------------------------------------------
CSubTask& CSubTask::operator=( const CSubTask& task )
{
	copy( task );

	return *this;
}

//----------------------------------------------------------------
// DrawTask
//----------------------------------------------------------------
BOOL CSubTask::DrawTask( UINT x, UINT* y, UINT cxMax, 
	CDC* pDC, CFont* pHeaderFont, 
	CFont* pDescriptionFont, CImageList *ptrList )
{
	BOOL rc = TRUE;
	CRect rect;
	CRect objectRect;
	CFont* pOldFont;
	COLORREF shadow = (COLORREF)GetSysColor( COLOR_3DDKSHADOW );
	COLORREF hilight = (COLORREF)GetSysColor( COLOR_3DHILIGHT );
	COLORREF oldColor;
	int oldBkMode;
	CDC bDC;
	CString strItem;

	oldBkMode = pDC->SetBkMode( TRANSPARENT );

	// Make sure we're drawing the text with the system colors.
	oldColor = pDC->SetTextColor( (COLORREF)GetSysColor( COLOR_WINDOWTEXT ) );

	objectRect.left = x;
	objectRect.right = x + 16;
	objectRect.top = *y;
	objectRect.bottom = *y + 16;
	
	ptrList->Draw( pDC, m_image, CPoint( objectRect.left, objectRect.top ), ILD_NORMAL );

	//pDC->Draw3dRect( objectRect, shadow, hilight );

	//I don't want any underscores in the Taskpad
	//	so remove them all now
	strItem = m_text;
	RemoveChars( strItem, _T("&" ) );

	objectRect.left = x + 26;
	objectRect.right = x + cxMax - 10;
	objectRect.top = *y;
	pOldFont = pDC->SelectObject( pHeaderFont );
	pDC->DrawText( strItem, objectRect, DT_WORDBREAK | DT_CALCRECT );

	//objectRect.right = x + cxMax - 10;
	pDC->DrawText( strItem, objectRect, DT_WORDBREAK );
	//Save off my rect for when people click on it
	m_textRect = objectRect;

	objectRect.left = x + 2;
	objectRect.right = x + cxMax - 12;
	objectRect.top = objectRect.bottom + 7;
	pDC->SelectObject( pDescriptionFont );
	pDC->DrawText( m_description, objectRect, DT_WORDBREAK | DT_CALCRECT);		

	objectRect.right = x + cxMax - 12;
	pDC->DrawText( m_description, objectRect, DT_WORDBREAK );

	objectRect.left -= 2;
	objectRect.right += 2;
	objectRect.top -= 2;
	objectRect.bottom += 2;
	//pDC->Draw3dRect( objectRect, shadow, hilight );

	pDC->SelectObject( pOldFont );
	pDC->SetTextColor( oldColor );

	*y = objectRect.bottom;

	rc = TRUE;

//cleanup:
	pDC->SetBkMode( oldBkMode );

	return rc;
}

//----------------------------------------------------------------
//
// CTaskPadExt class
//
//----------------------------------------------------------------

//----------------------------------------------------------------
// Constructor
//----------------------------------------------------------------
CTaskpadExt::CTaskpadExt()
:	CTreeCtrl()
{
	m_wGroupID = 0;
}

//----------------------------------------------------------------
// Destructor
//----------------------------------------------------------------
CTaskpadExt::~CTaskpadExt()
{
}


BEGIN_MESSAGE_MAP(CTaskpadExt, CTreeCtrl)
	//{{AFX_MSG_MAP(CTaskpadExt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//----------------------------------------------------------------
// AddTaskGroup
//----------------------------------------------------------------
CTaskGroup *CTaskpadExt::AddTaskGroup( CTaskGroup* pGroup, BOOL bRootItem /* = FALSE */ )
{
	CTaskGroup		*pData = NULL;
	try
	{
		pData = new CTaskGroup;
	}
	catch (std::bad_alloc &){return pData;}
	TV_INSERTSTRUCT insertStruct;
	HTREEITEM		hItem;

	ASSERT( pGroup->m_text.GetLength() );

	// copy the input data
	*pData = *pGroup;

	//Add the unique group ID to the map object
	m_GroupIds.SetAt( m_wGroupID++, pData );

	// insert at the root of the tree
	insertStruct.hParent = bRootItem ? TVI_ROOT : GetRootItem();

	insertStruct.hInsertAfter = TVI_LAST;
	
	insertStruct.item.mask = TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE
		| TVIF_SELECTEDIMAGE;

	insertStruct.item.pszText = LPSTR_TEXTCALLBACK;
	insertStruct.item.iImage = I_IMAGECALLBACK;
	insertStruct.item.iSelectedImage = I_IMAGECALLBACK;
	insertStruct.item.lParam = (LPARAM)pData;

	hItem = InsertItem( &insertStruct );
	if ( hItem )
	{
		pData->m_handle = hItem;
		pData->m_pTaskpad = this;

		*pGroup = *pData;
	}
	else	
	{
		TRACE( "Taskpad: Unable to add %s group\n", pData->m_text );
		delete pData;
		pData = NULL;
	}

	return pData;
}

//----------------------------------------------------------------
// AddSubTask
//----------------------------------------------------------------
CSubTask *CTaskpadExt::AddSubTask( CTaskGroup* pGroup, CSubTask* pTask )
{
	CSubTask		*pData = NULL;
	try
	{
		pData = new CSubTask;
	}
	catch (std::bad_alloc &) {return pData;}
	TV_INSERTSTRUCT insertStruct;
	HTREEITEM		hItem;

	ASSERT( pGroup->m_handle );
	ASSERT( pGroup->m_text.GetLength() );
	ASSERT( pTask->m_text.GetLength() );

	pTask->m_strGroup = pGroup->m_text;

	// copy the input data
	*pData = *pTask;

	// insert at the root of the tree
	insertStruct.hParent = pGroup->m_handle;
	insertStruct.hInsertAfter = TVI_LAST;
	
	insertStruct.item.mask = TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE
		| TVIF_SELECTEDIMAGE;

	insertStruct.item.pszText = LPSTR_TEXTCALLBACK;
	insertStruct.item.iImage = I_IMAGECALLBACK;
	insertStruct.item.iSelectedImage = I_IMAGECALLBACK;
	insertStruct.item.lParam = (LPARAM)pData;

	hItem = InsertItem( &insertStruct );

	if ( hItem )
	{
		pData->m_handle = hItem;
		pData->m_pTaskpad = this;
	}
	else
	{
		TRACE( "Taskpad: Unable to add %s group\n", pData->m_text );
		delete pData;
		pData = NULL;
	}
//	else
//		*pTask = *pData;

	return pData;
}


