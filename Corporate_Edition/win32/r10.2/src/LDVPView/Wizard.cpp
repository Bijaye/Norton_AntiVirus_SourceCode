// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: Wizard.cpp
//  Purpose: CWizard Implementation file. Base class for Task wizards
//
//	Date: 1-20-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#include "stdafx.h"
#include "LDVPView.h"
#include "Wizard.h"
#include "WizardPage.h"

// include SLICLIB 1.6
#include "slic.h"
#include <atlbase.h>

#include "oem_obj.h"
#include "slicwrap.h"
#include "SLICLicenseData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CWizard, CPropertySheet)


//----------------------------------------------------------------
// Constructor
//----------------------------------------------------------------
CWizard::CWizard( long ID, CWnd* pParentWnd )
:CClientPropSheet(_T("NOT USED"), pParentWnd, 0)
{
	long dSubscriptionMode = SLIC_MODE_ELS;
	// If we can't get an instance, we can assume that this is an unlicensed version.
	//CComPtr<ISLICLicenseRepositorySCS> slic;

	m_bInitialized = FALSE;

	//Set the Wizard mode
	SetWizardMode();

	m_psh.dwFlags |= PSH_HASHELP;

	// Add the page, if we know the type requested.
	CPropertyPage* page_to_add = NULL;

	switch( ID )
	{
	case TASK_ID_RTS:
		page_to_add = &m_pageRTSMonitor;
		break;

	case TASK_ID_SCHEDULE:
		page_to_add = &m_pageSchedule;
		break;

	case TASK_ID_VIRUS_BIN:
		page_to_add = &m_pageVirusBin;
		break;
	
	case TASK_ID_EVENT_LOG:
		page_to_add = &m_pageEventLog;
		break;
	
	case TASK_ID_VIRUS_HISTORY:
		page_to_add = &m_pageVirusHistory;
		break;

	case TASK_ID_SCAN_HISTORY:
		page_to_add = &m_pageScanHistory;
		break;

	case TASK_ID_VIEW_BACKUP:
		page_to_add = &m_pageBackup;
		break;

	case TASK_ID_VIEW_REPAIR:
		page_to_add = &m_pageRepairedItems;
		break;

	case TASK_ID_LICENSING:	
		//if( SUCCEEDED( slic.CoCreateInstance( CLSID_SLICLicenseRepository, NULL, CLSCTX_INPROC_SERVER ) ) )
		//{
		//	CComPtr<ISLICLicense> lic;

		//	CComBSTR license_status_dscr;
		//	SLIC_STATUS slic_status = SLICSTATUS_OK;

		//	slic->IsInSubscriptionMode(&dSubscriptionMode);

		//}
	
		if (OEMObj.IsOEMBuild() && (!OEMObj.IsOEM_RealELSMode() /* || dSubscriptionMode */))
			page_to_add = &m_pageLSLicensing;
		else
			page_to_add = &m_pageLicensing;
		break;
	case TASK_ID_TAMPERBEHAVIOR_HISTORY:
		page_to_add = &m_pageTamperBehaviorHistory;
		break;

	default:
		break;
	}

	if( page_to_add != NULL )
	{
		AddPage( page_to_add );
	}
}

//----------------------------------------------------------------
// Destructor
//----------------------------------------------------------------
CWizard::~CWizard()
{
	int iPage = GetPageCount() -1;

	//Remove all of my pages
	for( ; iPage >= 0; iPage-- )
	{
		RemovePage( iPage );
	}
}


BEGIN_MESSAGE_MAP(CWizard, CPropertySheet)
	//{{AFX_MSG_MAP(CWizard)
	ON_WM_SETFOCUS()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//----------------------------------------------------------------
// OnInitDialog
//----------------------------------------------------------------
BOOL CWizard::OnInitDialog()
{
	InitEngine();

	CClientPropSheet::OnInitDialog();

	m_bInitialized = TRUE;
	
	return TRUE;
}

//----------------------------------------------------------------
// OnClose
//----------------------------------------------------------------
void CWizard::OnClose()
{
	CClientPropSheet::OnClose();
}

void CWizard::OnSetFocus(CWnd* pOldWnd) 
{
	CClientPropSheet::OnSetFocus(pOldWnd);
	
	if( GetActivePage() )
		GetActivePage()->SetFocus();
}

void CWizard::OnDestroy() 
{
	CPropertyPage *ptrPage = GetActivePage();

	if( ptrPage )
	{
		//Now, inform the active child page that
		//	it is about to be destroyed so it can
		//	do what it needs to do with the open scan.
		GetActivePage()->SendMessage( WM_COMMAND, MAKEWPARAM( UWM_DESTROYING, 0 ), 0L );
	}
	
	CClientPropSheet::OnDestroy();

	
}

void CWizard::OnSize(UINT nType, int cx, int cy) 
{
	CWizardPage	*ptrPage;

	//First, hide the tab control. This will stop
	//	it from re-appearing as is seems to do every so often.
	CTabCtrl *pTab = GetTabControl();
	
	if( pTab )
		pTab->ShowWindow( SW_HIDE );


	if( m_bInitialized )
	{
		MoveWindow( 0, 0, cx, cy, TRUE );

		SetButtonsPos();

		int iPage = GetPageCount() -1;

		//Remove all of my pages
		for( ; iPage >= 0; iPage-- )
		{
			ptrPage = (CWizardPage*)GetPage( iPage );
			if( ptrPage && ::IsWindow( ptrPage->m_hWnd ) )
			{
				ptrPage->SizePage( cx, cy );
			}
		}	
	}
}

void CWizard::SetButtonsPos()
{
	
	CWnd *ptrWnd;
	RECT buttonRect,
		 dlgRect;

	//Figure out the current client rect
	GetClientRect( &dlgRect );

	//Move the Scan button
	if( ptrWnd = GetDlgItem( 0x3025 ) )
	{
		ptrWnd->GetWindowRect( &buttonRect );
		ScreenToClient( &buttonRect );
		ptrWnd->MoveWindow( dlgRect.right - ( buttonRect.right - buttonRect.left ) - 20, dlgRect.bottom - ( buttonRect.bottom - buttonRect.top ) - 10, buttonRect.right - buttonRect.left, buttonRect.bottom - buttonRect.top );
	}

	//Make the line adjust properly
	if( ptrWnd = GetDlgItem( 0x3026 ) )
	{
		CRect	tempRect;

		ptrWnd->GetWindowRect( &tempRect );
		ScreenToClient( &tempRect );
		ptrWnd->MoveWindow( tempRect.left, buttonRect.top - 12, buttonRect.right - tempRect.left, tempRect.bottom - tempRect.top );
	}



	//Move the Back button
	if( ptrWnd = GetDlgItem( 0x3023 ) )
	{
		ptrWnd->MoveWindow( buttonRect.left - ( buttonRect.right - buttonRect.left ) - 6, buttonRect.top, buttonRect.right - buttonRect.left, buttonRect.bottom - buttonRect.top);
		if( ptrWnd->IsWindowVisible() )
		{
			ptrWnd->GetWindowRect( &buttonRect );
			ScreenToClient( &buttonRect );
		}
	}
	
	//Move the help button
	if( ptrWnd = GetDlgItem( IDHELP ) )
		ptrWnd->MoveWindow( buttonRect.left - ( buttonRect.right - buttonRect.left ) - 6, buttonRect.top, buttonRect.right - buttonRect.left, buttonRect.bottom - buttonRect.top );
}

void CWizard::OnPaint() 
{
	CPaintDC	dc(this); // device context for painting
	CBrush		brushBackground;
	RECT		rect;

	GetClientRect( &rect );
	brushBackground.CreateSolidBrush( (COLORREF)GetSysColor( COLOR_BTNFACE ) );
	dc.FillRect( &rect, &brushBackground );

	dc.SetBkColor( (COLORREF)GetSysColor( COLOR_BTNFACE ) );

	brushBackground.DeleteObject();
	//Paint the background with the dialog color

}

void CWizard::OnLButtonDown(UINT nFlags, CPoint point)
{
	// Activate the property page if the user clicks
	// the mouse in the client area
	if( GetActivePage() )
		GetActivePage()->SetFocus();
}

void CWizard::OnRButtonDown(UINT nFlags, CPoint point)
{
	// Activate the property page if the user clicks
	// the mouse in the client area
	if( GetActivePage() )
		GetActivePage()->SetFocus();
}

void CWizard::ShowWindow(int nCmdShow)
{
	CPropertySheet::ShowWindow(nCmdShow);

	//MH 08.08.00
	//Brought over EA's earlier fix for a similar problem.
 	//Force an LButton down of the mouse so that it gets the mouseactivate message.
 	//This is done so that we can still tab after help exits.
 	CWnd* pWnd;
 	pWnd = AfxGetMainWnd();
 	if(pWnd)
 	{
 		if(pWnd->m_hWnd)
 			PostMessage(WM_MOUSEACTIVATE, (WPARAM)pWnd->m_hWnd,(MAKELONG(HTCLIENT,WM_LBUTTONDOWN)) );
 	}
}

/*------------------------------------------------------------------
*
* $History: $
*
*
  ------------------------------------------------------------------*/

