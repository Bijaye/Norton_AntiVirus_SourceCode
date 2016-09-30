// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// ScanHistory.cpp : implementation file
//

#include "stdafx.h"
#include "ldvpview.h"
#include "ScanHistory.h"
#include "Wizard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScanHistory property page

IMPLEMENT_DYNCREATE(CScanHistory, CWizardPage)

CScanHistory::CScanHistory() : CWizardPage(CScanHistory::IDD)
{
	//{{AFX_DATA_INIT(CScanHistory)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_psp.dwFlags |= PSP_HASHELP;
	//Set the Help Context ID
	SetHelpID( IDD );

	m_pResultsViewDlg = NULL;
}

CScanHistory::~CScanHistory()
{
}

void CScanHistory::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScanHistory)
	DDX_Control(pDX, IDC_TITLE, m_ctlTitle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScanHistory, CPropertyPage)
	//{{AFX_MSG_MAP(CScanHistory)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_MESSAGE(UWM_CLOSING, OnResultsDlgClosing )
	ON_MESSAGE(UWM_ADDCHILD, OnCreateScanDlg )
	ON_MESSAGE(UWM_READ_LOGS, OnReadLogs )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScanHistory message handlers

BOOL CScanHistory::OnWizardFinish() 
{
	//Fire an event telling the App to close the task
	((CLDVPViewApp*)AfxGetApp())->m_ptrCtl->NotifyEndTask();

	return CWizardPage::OnWizardFinish();
}

BOOL CScanHistory::OnSetActive() 
{
	CWizardPage::OnSetActive();
	// terrym 8-28-00 have to parse the raw resouce string now since
	// both the menu and resource string are enbedded
	CString sMenu,sTaskpad,sRaw;

	CWizard	*ptrParent = (CWizard*)GetParent();
	
	//Set the Context-sensitive ID
	SetHelpID(  IDD );
	
	// terrym 8-28-00 have to parse the raw resouce string now since
	// both the menu and resource string are enbedded
	sRaw.LoadString( IDS_TASK_SCAN_HISTORY );
	ConvertRawMenuString(sRaw,sMenu,sTaskpad);

	//Load the options
	ptrParent->HideBack();
	ptrParent->m_strTitle=sTaskpad;
	ptrParent->SetFinishText( IDS_CLOSE );
	ptrParent->SetWizardButtons( PSWIZB_FINISH );

	return TRUE;
}

void CScanHistory::SizePage( int cx, int cy )
{
	//First, call the base class version of this
	//		virtual method
	CWizardPage::SizePage( cx, cy );

	CRect	rect,
			dlgRect;

	m_ctlTitle.GetWindowRect( &rect );
	GetClientRect( &dlgRect );

	ScreenToClient( &rect );
	
	rect.top = rect.bottom + 2;
	rect.bottom = dlgRect.bottom - 10;
	rect.left = 0;
	rect.right = dlgRect.right - 10;

	if( ::IsWindow( m_hChildWnd ) )
		CWnd::FromHandle( m_hChildWnd )->SetWindowPos( &wndTop, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW | SWP_NOZORDER);
}

//----------------------------------------------------------------
// OnResultsDlgClosing
//----------------------------------------------------------------
long CScanHistory::OnResultsDlgClosing( WPARAM wParam, LPARAM )
{
	if( wParam == 1 )
	{
		//Now, dismiss the page
		CWizard		*ptrParent = (CWizard*)GetParent();
		ptrParent->PressButton( PSBTN_FINISH );

	}

	m_hChildWnd = NULL;

	return 1;
}

//----------------------------------------------------------------
// OnCreateScanDlg
//----------------------------------------------------------------
long CScanHistory::OnCreateScanDlg( WPARAM wParam, LPARAM )
{
	CWizard	*pWiz = (CWizard*)GetParent();

	m_hChildWnd = (HWND)wParam;

	if( pWiz )
	{
		//Now, size the dialog to my size
		CRect rect;

		pWiz->GetWindowRect( &rect );
		SizePage( rect.right - rect.left, rect.bottom - rect.top );
	}

	return 1;
}

BOOL CScanHistory::OnInitDialog() 
{
	RESULTSVIEW		view;
	WPARAM			wParam = 0;

	CPropertyPage::OnInitDialog();
	
	memset( &view, 0x00, sizeof( view ) );

	view.Size = sizeof( view );
	view.hWndParent = m_hWnd;
	view.Flags = RV_FLAGS_CHILD;
	view.Type = RV_TYPE_SCANHISTORY;
    view.GetFileStateForView = GetFileStateForView;
	view.TakeAction = TakeAction;
	view.GetCopyOfFile = GetCopyOfFile;
	view.ViewClosed = ViewClosed;
	view.Context = this;
	view.Title = m_strTaskName.GetBuffer( m_strTaskName.GetLength() );
	view.szFilterSettingsKey = "ScanHistorySettings";

	if( CreateResultsView && ( RV_SUCCESS == CreateResultsView( &view ) ) )
	{
		m_pResultsViewDlg = view.pResultsViewDlg;
	}
	else
	{
		//Notify of an error
		AfxMessageBox(IDS_SCANDIALOGS_ERROR);
		wParam = -1;
	}

	m_strTaskName.ReleaseBuffer();

	PostMessage( UWM_READ_LOGS, wParam, 0 );

	return TRUE;
}

void CScanHistory::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	PaintTitle( &dc );	
}

LRESULT CScanHistory::OnReadLogs( WPARAM wParam, LPARAM )
{
	CWaitCursor	wait;

	if( wParam == -1 )
	{
		//End the task
		((CPropertySheet*)GetParent())->PressButton( PSBTN_FINISH );
	}
	else
	{
		//Start by redrawing the window.
		RedrawWindow();
		ReadLogFiles(m_pResultsViewDlg);
	}

	return 0;
}

void CScanHistory::OnLButtonDown(UINT nFlags, CPoint point) 
{
	
	CPropertyPage::OnLButtonDown(nFlags, point);

	SetFocus();
}

void CScanHistory::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CPropertyPage::OnRButtonDown(nFlags, point);

	SetFocus();
}
