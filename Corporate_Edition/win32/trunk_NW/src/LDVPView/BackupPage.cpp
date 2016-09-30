// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// BackupPage.cpp: implementation of the CBackupPage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ldvpview.h"
#include "Wizard.h"
#include "resultitem.h"
#include "BackupPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CBackupPage, CWizardPage)

/////////////////////////////////////////////////////////////////////////////
// CBackupPage dialog


CBackupPage::CBackupPage() : CWizardPage(CBackupPage::IDD)
{
	//{{AFX_DATA_INIT(CBackupPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_psp.dwFlags |= PSP_HASHELP;
	//Set the Help Context ID
	SetHelpID( IDD );

	m_pResultsViewDlg = 0;
}

CBackupPage::~CBackupPage()
{
}

void CBackupPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBackupPage)
	DDX_Control(pDX, IDC_TITLE, m_ctlTitle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBackupPage, CPropertyPage)
	//{{AFX_MSG_MAP(CBackupPage)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_MESSAGE(UWM_CLOSING, OnResultsDlgClosing )
	ON_MESSAGE(UWM_ADDCHILD, OnCreateScanDlg )
	ON_MESSAGE(UWM_READ_LOGS, OnFillBin )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBackupPage message handlers

long CBackupPage::OnResultsDlgClosing( WPARAM wParam, LPARAM )
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


void CBackupPage::SizePage( int cx, int cy )
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


long CBackupPage::OnCreateScanDlg( WPARAM wParam, LPARAM )
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

LRESULT CBackupPage::AddItemToView( IVBin2 *ptrVBin, VBININFO *pVBinInfoHeadRecord )
{
    IVBinSession *  pVBinSession    = NULL;
    VBININFO        cVBinInfoChild  = {0};
    HANDLE          hVBinFind       = 0;
    PEVENTBLOCK     pEB             = NULL;

    // Make sure this is a backup item.
    if ( pVBinInfoHeadRecord->Flags & VBIN_BACKUP )
    {
        // First, add this item to the results view.
        AddLogLineToResultsView( m_pResultsViewDlg, NULL, pVBinInfoHeadRecord->LogLine );

        // If it is a session, also, add all of the contained records.
        if ( pVBinInfoHeadRecord->dwRecordType == VBIN_RECORD_SESSION_OBJECT )
        {
            if ( ERROR_SUCCESS == ptrVBin->OpenSession( pVBinInfoHeadRecord->dwSessionID, &pVBinSession ) )
            {
                // See if we can find an item within this session.
                if ( hVBinFind = pVBinSession->FindFirstItem( &cVBinInfoChild ) )
                {
                    do
                    {
                        // Add this item to the results view.
                        AddLogLineToResultsView( m_pResultsViewDlg, NULL, cVBinInfoChild.LogLine );

                        // Initialize our record for the next query.
			            memset( &cVBinInfoChild, 0x00, sizeof( cVBinInfoChild ) );
			            cVBinInfoChild.Size = sizeof( VBININFO );
                    }
                    while( pVBinSession->FindNextItem( hVBinFind, &cVBinInfoChild ) );

                    pVBinSession->FindClose( hVBinFind );
                }
            }                    
        }
    }

	return 0;
}


LRESULT CBackupPage::OnFillBin( WPARAM wParam, LPARAM )
{
	CWaitCursor		wait;
	IVBin2			*ptrVBin = NULL;
	DWORD			dwIndex = 0;
	VBININFO		vBinInfo;
    HANDLE          hVBinFind = NULL;

	if( wParam == -1 )
	{
		//End the task
		((CPropertySheet*)GetParent())->PressButton( PSBTN_FINISH );
	}
	else
	{
		//Start by redrawing the window.
		RedrawWindow();

		//If I have a valid VBin object,
		//	I need to enum the items and add them to the 
		//	results view.
		//NOTE: If I couldn't create the results view, then I won't have
		//	a valid VBin pointer, and I will not get into this code.
		if( SUCCEEDED( CoCreateLDVPObject( CLSID_Cliscan, IID_IVBin, (void**)&ptrVBin ) ) )
		{
			memset( &vBinInfo, 0x00, sizeof( vBinInfo ) );
			vBinInfo.Size = sizeof( VBININFO );

            // Enumerate all the items in the virus bin.
            hVBinFind = ptrVBin->FindFirstItem(&vBinInfo);

            // Did we get a valid handle?
            if ( hVBinFind )
            {
			    do 
			    {
                    // Add this item to the view, if applicable.
                    AddItemToView( ptrVBin, &vBinInfo );

                    // Initialize before getting the next item.
				    memset( &vBinInfo, 0x00, sizeof( vBinInfo ) );
				    vBinInfo.Size = sizeof( VBININFO );
                } while( ptrVBin->FindNextItem(hVBinFind, &vBinInfo) );

                // We're done.
                ptrVBin->FindClose(hVBinFind);

            }

			ptrVBin->Release();
		}

		AddLogLineToResultsView( m_pResultsViewDlg, NULL, NULL );

        Invalidate();
		OnPaint();
	}

	return 0;
}


BOOL CBackupPage::OnInitDialog() 
{
	
	RESULTSVIEW		view;
	WPARAM			wParam = 0;

	CPropertyPage::OnInitDialog();

	memset( &view, 0x00, sizeof( view ) );

	view.Size = sizeof( view );
	view.hWndParent = m_hWnd;
	view.Flags = RV_FLAGS_CHILD;
	view.Type = RV_TYPE_BACKUP;
    view.GetFileStateForView = GetFileStateForView;
	view.TakeAction = TakeAction;
	view.GetCopyOfFile = GetCopyOfFile;
	view.ViewClosed = ViewClosed;
    view.GetVBinData = GetVBinData;
	view.Context = this;
	view.Title = m_strTaskName.GetBuffer( m_strTaskName.GetLength() );
	view.szFilterSettingsKey = "VirusBinSettings";

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

void CBackupPage::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	PaintTitle( &dc );	
}

BOOL CBackupPage::OnSetActive() 
{	
	CWizardPage::OnSetActive();

	// terrym 8-28-00 have to parse the raw resouce string now since
	// both the menu and resource string are enbedded
	CString sMenu,sTaskpad,sRaw;

    CWizard	*ptrParent = (CWizard*)GetParent();
	
	// terrym 8-28-00 have to parse the raw resouce string now since
	// both the menu and resource string are enbedded
	sRaw.LoadString( IDS_TASK_BACKUP );
	ConvertRawMenuString(sRaw,sMenu,sTaskpad);

	//Load the options
	ptrParent->HideBack();
	ptrParent->m_strTitle=sTaskpad;
	ptrParent->SetFinishText( IDS_CLOSE );
	ptrParent->SetWizardButtons( PSWIZB_FINISH );

	return TRUE;
}

BOOL CBackupPage::OnWizardFinish() 
{
	//Fire an event telling the App to close the task
	((CLDVPViewApp*)AfxGetApp())->m_ptrCtl->NotifyEndTask();

	return CWizardPage::OnWizardFinish();
}

void CBackupPage::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CPropertyPage::OnLButtonDown(nFlags, point);

	SetFocus();
}

void CBackupPage::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CPropertyPage::OnRButtonDown(nFlags, point);

	SetFocus();
}
